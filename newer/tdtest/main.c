#include "common.h"

extern pthread_t tg_update_tid;
extern void *tg_update_thread(void *cookie);
int tg_running, tdclient_id;
char tdlib_version[16];
byte main_debug;

int main(int argc, char **argv){
	main_debug=1;
	onauth=1;
	onrequest=0;
	contactn=0;
	//disable logging
	td_execute("{\"@type\":\"setLogVerbosityLevel\", \"new_verbosity_level\":0}");
	//init client id
	tdclient_id=td_create_client_id();
	//start update listener thread
	tg_running=1;
	pthread_create(&tg_update_tid, NULL, tg_update_thread, NULL);
	td_send(tdclient_id, "{\"@type\":\"getAuthorizationState\"}");
	//interactive cli - v2 :D
	char *line;
	size_t size;
	int len;
	while (tg_running){
		if (onauth){
			mysleep(500);
			continue;
		}
		line = NULL;
		printf("TDLIB> ");
		fflush(stdout);
		if ((len=getline(&line, &size, stdin)) != -1) {
			if (line[len-1]=='\n'){ //remove trailing new line
				line[len-1]='\0';
			}
			if (0==strcmp(line, "exit")){
				printf("Exiting...\n");
				if (!main_debug) main_debug=1;
				td_send(tdclient_id, "{\"@type\":\"close\"}");
				while (tg_running){mysleep(200);}
			}
			else if (0==strcmp(line, "debug")){
				printf("Toggling debug (old=%d, new=%d)\n", main_debug, !main_debug);
				main_debug=!main_debug;
			}
			else if (0==strcmp(line, "version")){
				printf("MLX Telegram client 0.01, powered by TDLib %s\n", tdlib_version);
			}
			else if (0==strcmp(line, "getme")){
				printf("#%1.0f: %s %s - @%s (%s)\n", myuser()->id, myuser()->fname, myuser()->lname, myuser()->uname, myuser()->phone);
			}
			else if (0==strcmp(line, "contacts")){
				//td_send(tdclient_id, "{\"@type\":\"getContacts\"}"); //ask for contacts at startup
				byte temp_nodebug=0;
				if (contactn>0){
					if (main_debug){
						main_debug=0;
						temp_nodebug=1;
					}
					printf("Listing %d contacts\n", contactn);
					int i;
					for (i=0; i<contactn; i++){
						TD_USERP contact = contacts[i];
						printf("#%1.0f: %s %s - @%s (%s)\n", contact->id, contact->fname, contact->lname, contact->uname, contact->phone);
					}
					if (temp_nodebug) main_debug=1;
				}
				else {
					printf("No contacts to show.\n");
				}
			}
			free(line);
		}
	}
	int i;
	logd("[MAIN] cleanup...");
	for (i=0; i<contactn; i++){
		TD_USERP contact = contacts[i];
		if (contact->uname) free(contact->uname);
		if (contact->fname) free(contact->fname);
		if (contact->lname) free(contact->lname);
		free(contact);
	}
	logd("[MAIN] bye");
	return 0;
}