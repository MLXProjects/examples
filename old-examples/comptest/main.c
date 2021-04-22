#include <aroma.h>
//#include "../statusbar.c"
#include "../navbar.c"
#include "template.c"


LIBAROMA_ZIP zip;
LIBAROMA_STREAMP stream_uri_callback(char * uri);

void aroma_init();

int main(int argc, char ** argv){	
	printf("ZIP path: %s\n", argv[3]);
	zip = libaroma_zip(argv[3]);
	if (!zip){
		printf("ZIP open failed!!\n");
		printf("Second ZIP path: /sdcard/res.zip\n");
		zip = libaroma_zip("/sdcard/res.zip");
		if (!zip){
			printf("ZIP open failed!! Fonts and icons may be unavailable.\n");
		}
	}
	
	aroma_init();
	/* for recovery apps, uncomment the lines below
	pid_t pp = getppid();
	kill(pp, 19); */
	//char *title=malloc(16);
	//sprintf(title, "Test app");
	//statusbar_init(NULL, title);
	template_page(NULL);
	
	libaroma_end();
	
	/* for recovery apps, uncomment the line below
	kill(pp, 18); */
	
	return 0;
}

LIBAROMA_STREAMP stream_uri_callback(char * uri){
	int n = strlen(uri);
	char kwd[11];
	int i;
	for (i = 0; i < n && i < 10; i++) {
		kwd[i] = uri[i];
		kwd[i + 1] = 0;
		if ((i > 1) && (uri[i] == '/') && (uri[i - 1] == '/')) {
			break;
		}
	}
	/* resource stream */
	if (strcmp(kwd, "res://") == 0) {
		LIBAROMA_STREAMP ret=libaroma_stream_mzip(zip, uri + 7);
		if (ret){
			/* change uri */
			snprintf(ret->uri,
				LIBAROMA_STREAM_URI_LENGTH,
				"%s", uri
			);
			return ret;
		}
	}
	return NULL;
}

void aroma_init(){	
	libaroma_start();	
	
	//STATUSBAR_HEIGHT=libaroma_dp(STATUSBAR_HEIGHT);
	/* init stream callback */
	libaroma_stream_set_uri_callback(stream_uri_callback);
	/* load font id=0 */
	libaroma_font(0, libaroma_stream(
		"res:///fonts/Roboto-Regular.ttf"));	
	//libaroma_wm_set_workspace(0, STATUSBAR_HEIGHT, libaroma_fb()->w, libaroma_fb()->h - STATUSBAR_HEIGHT);	
}
