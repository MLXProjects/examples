#ifndef __libaroma_emptywindow_c__
#define __libaroma_emptywindow_c__

#include <aroma.h>
#include "statusbar.c"


LIBAROMA_ZIP zip;

/* stream uri callback */
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
	
	/* init stream callback */
	libaroma_stream_set_uri_callback(stream_uri_cb);
	/* load font id=0 */
	libaroma_font(0,
		libaroma_stream(
		"res:///fonts/Roboto-Regular.ttf"
		)
	);
}

void winmain(){

	#define ID_CTLLIST			1
	#define ID_CTLLIST2			2

	/* menu item macro */
	#define _ITEM(id,text,ico,extra) \
		libaroma_listitem_menu(\
			list, id, text, extra, (ico!=NULL)?ico:NULL, \
			menuflags,-1)
	#define _CHECK(id,text,ico,extra,flags) \
		libaroma_listitem_check(\
			list, id, 0, text, extra, (ico!=NULL)?ico:NULL, \
			flags,-1)
	#define _TITLE(id,text) \
		libaroma_listitem_caption_color(list, id, text, \
			libaroma_colorget(NULL,win)->accent, -1)
	#define _DIV(id) \
		libaroma_listitem_divider(list, id, \
			LIBAROMA_LISTITEM_DIVIDER_SUBSCREEN,-1)
	
	LIBAROMA_WINDOWP win = libaroma_window(
		NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
	int ctlY = STATUSBAR_HEIGHT;
	int ctlH = 46;
	
	LIBAROMA_CANVASP settings_icon = libaroma_image_uri("res:///icons/settings.png");
		
	int listW = libaroma_dp(win->w / 4);
	int listX = (win->w - (listW * 4));
	
	printf("creating list\n");
	LIBAROMA_CONTROLP list = libaroma_ctl_list(
		win, ID_CTLLIST, /* win, id */
		listX, ctlY, settings_icon->w, LIBAROMA_SIZE_FULL, /* x,y,w,h */
		0, 0, /* horiz, vert padding */
		libaroma_colorget(NULL, win)->window_bg, /* bgcolor */
		LIBAROMA_CTL_SCROLL_NO_INDICATOR
	);
	listX += list->w;
	LIBAROMA_CONTROLP list2 = libaroma_ctl_list(
		win, ID_CTLLIST2, /* win, id */
		listX, ctlY, settings_icon->w, LIBAROMA_SIZE_FULL, /* x,y,w,h */
		0, 0, /* horiz, vert padding */
		libaroma_colorget(NULL, win)->window_bg, /* bgcolor */
		LIBAROMA_CTL_SCROLL_NO_INDICATOR
	);
	list->w = settings_icon->w;
	list2->w = settings_icon->w;
	printf("listW=%d, list w/h=%d/%d, listX=%d\n", listW, list->w, list->h, listX);
	printf("listW=%d, list2 w/h=%d/%d, listX=%d\n", listW, list2->w, list2->h, listX);
	//printf("canvas w/h=%d/%d\n", settings_icon->w, settings_icon->h);
	int i = 0;
	for (i=0; i < 9; i++){
		libaroma_listitem_image(list, 10 + i, settings_icon, settings_icon->h, 
		LIBAROMA_LISTITEM_IMAGE_FILL|LIBAROMA_LISTITEM_IMAGE_PROPORTIONAL|
		LIBAROMA_CTL_LIST_ITEM_RECEIVE_TOUCH, -1);
	}
	i = 0;
	for (i=0; i < 9; i++){
		libaroma_listitem_image(list2, 20 + i, settings_icon, settings_icon->h, 
		LIBAROMA_LISTITEM_IMAGE_FILL|LIBAROMA_LISTITEM_IMAGE_PROPORTIONAL|
		LIBAROMA_CTL_LIST_ITEM_RECEIVE_TOUCH, -1);
	}
	libaroma_window_anishow(win, LIBAROMA_WINDOW_SHOW_ANIMATION_PAGE_TOP, 400);	
	
	
	pthread_t sb_thread = statusbar_draw(win);	
	do{				
		LIBAROMA_MSG msg;
		dword command=libaroma_window_pool(win,&msg);
		byte cmd	= LIBAROMA_CMD(command);
		word id	 = LIBAROMA_CMD_ID(command);
		byte param = LIBAROMA_CMD_PARAM(command);
		switch (msg.msg){
			
			case LIBAROMA_MSG_KEY_SELECT: 
			{
				if (msg.state==0){
					//libaroma_png_save(libaroma_fb()->canvas,"/tmp/screenshot.png");
					printf("Close key pressed!\n");
					win->onpool = 0;
					break;
				}
			}
			case LIBAROMA_MSG_TOUCH: {
				//libaroma_ctl_scroll_set_pos(list, libaroma_ctl_scroll_get_scroll(list2, NULL));
				if (msg.state == LIBAROMA_HID_EV_STATE_MOVE//) {
				|| msg.state == LIBAROMA_HID_EV_STATE_UP
				|| msg.state == LIBAROMA_HID_EV_STATE_MOVE) {
					//if (list
					list->handler->message(list, &msg);
				}
				break;
			}
		}
		

		//printf("Main window received MSG msg: %i\n", msg.msg);
		
		if (cmd == 1){
			printf("ID: %i, MSG key: %i\n", id, msg.key);
			switch (msg.key){
				default:
					break;
			}
		}
		
	}
	while(win->onpool);
	
	//window pool exited, so the window should close.	
	if (!win->onpool){
		statusbar_updater_stop(sb_thread);
	}
	
	libaroma_window_aniclose(win, NULL, LIBAROMA_WINDOW_CLOSE_ANIMATION_PAGE_BOTTOM, 300);	
}

int main(int argc, char ** argv){	
	
	printf("ZIP path: %s\n", argv[3]);
	zip = libaroma_zip(argv[3]);
	if (!zip){
		printf("ZIP open failed!!\n");
		printf("ZIP path: /emmc/res.zip\n");
		zip = libaroma_zip("/emmc/res.zip");
		if (!zip){
			printf("ZIP open failed!!\n");
		}
	}
	
	pid_t pp = getppid();
	kill(pp, 19);
	
	aroma_init();
	winmain();
	
	libaroma_end();
	kill(pp, 18);
	
	return 0;
}

#endif