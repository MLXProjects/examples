#include <aroma.h>
#include <sys/reboot.h>
#include <sys/syscall.h>
LIBAROMA_ZIP zip;
#include "common.h"

LIBAROMA_STREAMP stream_uri_cb(char * uri);
void aroma_init();

void closewin(LIBAROMA_WINDOWP win, LIBAROMA_WINDOWP parent, byte anim, int duration){
	if (!parent){
		LIBAROMA_WINDOWP closewin=libaroma_window(NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
		LIBAROMA_CANVASP closecv=libaroma_canvas(win->prev_screen->w, win->prev_screen->h-STATUSBAR_HEIGHT);
		printf("Size: %d, %d\n", closecv->h, closewin->h);
		libaroma_draw_ex(closecv, win->prev_screen, 0, 0, 0, STATUSBAR_HEIGHT, closecv->w, closecv->h-STATUSBAR_HEIGHT, 0, 0xFF);
		libaroma_ctl_image_canvas(closewin, 999, closecv, 0, 0, closewin->w, closewin->h);
		libaroma_window_anishow(closewin, anim, duration);
		libaroma_window_free(closewin);		
	}
	else libaroma_window_anishow(parent, anim, duration);
	libaroma_window_free(win);
}

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
	/* for recovery apps, uncomment the lines below */
	pid_t pp = getppid();
	kill(pp, 19);
	byte reboot_now=install_doit(argv[3]);
	
	libaroma_end();
	switch (reboot_now){
		case 0: //do nothing
			break;
		case 1: //reboot normally
			reboot(RB_AUTOBOOT);
			break;
		case 2: //reboot recovery
			break;
		case 3: //turn off
			break;
	}
	/* for recovery apps, uncomment the line below */
	kill(pp, 18);
	
	return 0;
}

LIBAROMA_STREAMP stream_uri_cb(char * uri){
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
	libaroma_stream_set_uri_callback(stream_uri_cb);
	if (zip==NULL && debug) printf("ZIP LOAD FAILED!\n");
	config_read(zip);
	main_bgcolor=settings()->back_color;//RGB(222222);//RGB(000000); 	/* uncomment this for black bg */
	main_accent=settings()->fore_color;//RGB(ed1c24);//RGB(ed1c24); 		/* and this for red accent */
	STATUSBAR_HEIGHT=libaroma_dp(STATUSBAR_HEIGHT);
	/* load font id=0 */
	if (debug) printf("Loading fonts\n");
	libaroma_font(FONT_LIGHT, libaroma_stream("res:///fonts/aller.light.ttf"));
	libaroma_font(FONT_LIGHT_ITALIC, libaroma_stream("res:///fonts/aller.light-italic.ttf"));
	libaroma_font(FONT_MONOSPACE, libaroma_stream("res:///fonts/mono.ttf"));
	libaroma_font(FONT_MONOSPACE_ITALIC, libaroma_stream("res:///fonts/mono-italic.ttf"));
	
	if (debug) printf("Setting up workspace! \n");
	libaroma_wm_set_workspace(0, STATUSBAR_HEIGHT, libaroma_fb()->w, libaroma_fb()->h - STATUSBAR_HEIGHT);	
}
