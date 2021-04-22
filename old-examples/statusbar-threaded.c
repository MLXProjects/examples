#include <time.h>

int STATUSBAR_HEIGHT=20;

char *getcurtime(/*byte show_seconds*/){	
	time_t s, val = 1; 
    struct tm* current_time;   
    s = time(NULL); 
    current_time = localtime(&s); 
	char curtime[16];	
    snprintf(curtime, 16, "%02d:%02d:%02d", 
           current_time->tm_hour, 
           current_time->tm_min, 
           /*(!show_seconds)?NULL:*/current_time->tm_sec); 
	char *string = malloc(16);
    strcpy(string, curtime);
	//printf("STATUSBAR: Current time: %s\n", string);
    return string;
}

char *getcurbatt(){
	FILE *fp;
    char str[16] = "";
    char str2[16] = "%";
    char* filename = "/sys/class/power_supply/battery/capacity";
 
    fp = fopen(filename, "r");
    if (fp == NULL){
        printf("STATUSBAR: Could not open file %s",filename);
		char *string = malloc(16);
		strcpy(string, "lol");
		return string;
    }
	fgets(str, 16, fp);
    fclose(fp);
	char *string = malloc(16);
	const char nl[4] = "\n"; 
    strtok(str, nl); 	
    strcat(str, str2);
    strcpy(string, str);
	//printf("STATUSBAR: Battery level: %s", string);
	//snprintf(string, "%");
    return string;
}

int msleep(long msec){
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

	nanosleep(&ts, &ts);

    return 0;
}

void clock_update(LIBAROMA_CONTROLP clock){
	if (clock!=NULL) {libaroma_ctl_label_set_text(clock, getcurtime(), 1);}
}

void batt_update(LIBAROMA_CONTROLP battery){
	if (battery!=NULL) {libaroma_ctl_label_set_text(battery, getcurbatt(), 1);}
}

struct SB_CONTROLS {
	LIBAROMA_WINDOWP parent;
	LIBAROMA_CONTROLP clock;
	LIBAROMA_CONTROLP battery;
};
LIBAROMA_WINDOWP curwindow=NULL;
void *statusbar_update_caller(void* bg_color){
	//struct SB_CONTROLS *controls = (struct SB_CONTROLS*) win_controls;
	word color=(word) bg_color;
	printf("STATUSBAR: Update caller kicks in\n");
	while (1/*libaroma_window_isactive(controls->parent)*/) {
		//printf("STATUSBAR: Checking if parent is null\n");
		if (curwindow != NULL){ 
			//printf("STATUSBAR: Checking if parent is closed or visible\n");
			/*
			if (controls->parent->closed) {
				//printf("STATUSBAR: It seems to be closed :O\n");
				break;
			}*/
			if (libaroma_window_isactive(curwindow)){
				//printf("STATUSBAR: It seems to be visible :P\n");
				
				libaroma_gradient_ex(curwindow->bg, 0, 0, curwindow->w, STATUSBAR_HEIGHT, RGB(000000), color, -1, 0, 0xFF, 0xFF);
				libaroma_gradient_ex(curwindow->dc, 0, 0, curwindow->w, STATUSBAR_HEIGHT, RGB(000000), color, -1, 0, 0xFF, 0xFF);
				//clock_update(controls->clock);
				//batt_update(controls->battery);
				
				libaroma_draw_text(
					curwindow->dc,
					getcurtime(),
					0, 0, RGB(FFFFFF), curwindow->w, 
					LIBAROMA_FONT(0,3)|
					LIBAROMA_TEXT_SINGLELINE|
					LIBAROMA_TEXT_LEFT, 
					140);
					
				//printf("STATUSBAR: clock created\n");

				libaroma_draw_text(
					curwindow->dc,
					getcurbatt(),
					0, 0, RGB(FFFFFF), curwindow->w, 
					LIBAROMA_FONT(0,3)|
					LIBAROMA_TEXT_SINGLELINE|
					LIBAROMA_TEXT_RIGHT, 
					140);
			} 
		}
		msleep(1000);
	};
}

void statusbar_updater_start(pthread_t sb_thread, void* controls){
	//statusbar_active=1;
	printf("STATUSBAR: Creating statusbar updater thread\n");
	if(pthread_create(&sb_thread, NULL, statusbar_update_caller, controls)) {
		fprintf(stderr, "STATUSBAR: Error creating thread\n");
	}
	printf("STATUSBAR: Created\n");
}

void statusbar_updater_stop(pthread_t sb_thread){
	//statusbar_active=0;
	printf("STATUSBAR: Closing statusbar updater thread\n");
	if(pthread_join(sb_thread, NULL)) {
		fprintf(stderr, "STATUSBAR: Error joining thread\n");
	}
}
/*
byte statusbar_draw(){
		
	byte hasAppbar = win->appbar_bg!=NULL;
	word color = hasAppbar?win->appbar_bg:RGB(000000);//win->colorset->window_bg;
	
	printf("STATUSBAR: Drawing gradient!\n");
	libaroma_gradient_ex(win->bg, 0, 0, win->w, STATUSBAR_HEIGHT, RGB(000000), color, -1, 0, 0xFF, hasAppbar?0xFF:0x00);
	libaroma_gradient_ex(win->dc, 0, 0, win->w, STATUSBAR_HEIGHT, RGB(000000), color, -1, 0, 0xFF, hasAppbar?0xFF:0x00);
	
	libaroma_draw_text(
		sb_canvas,
		text,
		0, 0, RGB(000000), sb_canvas->w, 
		LIBAROMA_FONT(0,3)|
		LIBAROMA_TEXT_SINGLELINE|
		LIBAROMA_TEXT_CENTER, 
		140);
		
	
}*/

pthread_t statusbar_init(LIBAROMA_WINDOWP win, word override_bg){
	
	pthread_t sb_thread;
	/*
	int clockw = 60;
	int battw = 40;
	int midX = (win->w / 2) - (clockw / 2);
	*/
	byte hasAppbar = win->appbar_bg!=NULL;
	word color = hasAppbar?win->appbar_bg:RGB(000000);//win->colorset->window_bg;
	if (override_bg) {
		printf("STATUSBAR: overriding background color by flag!\n");
		color=override_bg;
	}
	printf("STATUSBAR: Drawing gradient!\n");
	libaroma_gradient_ex(win->bg, 0, 0, win->w, STATUSBAR_HEIGHT, RGB(000000), color, -1, 0, 0xFF, 0xFF);
	libaroma_gradient_ex(win->dc, 0, 0, win->w, STATUSBAR_HEIGHT, RGB(000000), color, -1, 0, 0xFF, 0xFF);
	
	printf("STATUSBAR: creating controls!\n");
	
	/*LIBAROMA_CONTROLP clklabel; 
	libaroma_ctl_label_valign(win, 1, getcurtime(), 0, 0, win->w, 14, 
								RGB(FFFFFF), 0, 2, LIBAROMA_TEXT_CENTER, 14, 1);
	*/
	
	libaroma_draw_text(
		win->dc,
		getcurtime(),
		0, 0, RGB(FFFFFF), win->w, 
		LIBAROMA_FONT(0,3)|
		LIBAROMA_TEXT_SINGLELINE|
		LIBAROMA_TEXT_LEFT, 
		140);
		
	printf("STATUSBAR: clock created\n");

	libaroma_draw_text(
		win->dc,
		getcurbatt(),
		0, 0, RGB(FFFFFF), win->w, 
		LIBAROMA_FONT(0,3)|
		LIBAROMA_TEXT_SINGLELINE|
		LIBAROMA_TEXT_RIGHT, 
		140);
	//LIBAROMA_CONTROLP battlabel = libaroma_ctl_label_valign(win, 1, getcurbatt(), 0, 0, win->w, 14, 
	//											RGB(FFFFFF), 0, 2, LIBAROMA_TEXT_RIGHT, 14, 1);
	
		
	printf("STATUSBAR: batt label created\n");
	/*struct SB_CONTROLS *sb_controls;// = malloc(sizeof(struct SB_CONTROLS));
	sb_controls = malloc(sizeof(struct SB_CONTROLS));
	(*sb_controls).parent = win;
	(*sb_controls).clock = clklabel;
	(*sb_controls).battery = battlabel;*/
	printf("STATUSBAR: controls created.\n");
	curwindow=win;
	statusbar_updater_start(sb_thread, (void*) color/*sb_controls*/);
	return sb_thread;
}
