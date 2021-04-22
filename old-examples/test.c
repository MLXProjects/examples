#ifndef __libaroma_emptywindow_c__
#define __libaroma_emptywindow_c__

#include <aroma.h>

LIBAROMA_ZIP zip;

/* stream uri callback */
LIBAROMA_STREAMP recovery_stream_uri_cb(char * uri){
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
	libaroma_stream_set_uri_callback(recovery_stream_uri_cb);
	/* load font id=0 */
	libaroma_font(0,
		libaroma_stream(
		"res:///fonts/Roboto-Regular.ttf"
		)
	);
}

LIBAROMA_WINDOWP win;

void winmain(){
	
	#define ID_APPBAR			1
	#define ID_CTLLIST			2
	#define ID_CAPTION1			5
	#define ID_CAPTION2			6
	#define ID_WIFICHK			7
	#define ID_BTCHK			8
	#define ID_SIMBTN			9
	#define ID_RRSETTINGSBTN	10
	#define ID_INFOBTN			11
	
	word menuflags =
		LIBAROMA_LISTITEM_MENU_INDENT_NOICON|
		LIBAROMA_LISTITEM_MENU_SMALL_ICON|
		LIBAROMA_LISTITEM_MENU_FREE_ICON|
		LIBAROMA_LISTITEM_WITH_SEPARATOR|
		LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN;
		
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
	
	win = libaroma_window(
		NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
	int ctlY = 0;
	int ctlH = 46;
		
	printf("creating bar\n");
	LIBAROMA_CONTROLP bar = libaroma_ctl_bar(
		win, ID_APPBAR,
		0, ctlY, LIBAROMA_SIZE_FULL, ctlH,
		"Configuración", RGB(263238), RGB(ffffff)
	);
	ctlY += bar->h;
	
	printf("creating list\n");
	LIBAROMA_CONTROLP list = libaroma_ctl_list(
		win, ID_CTLLIST, /* win, id */
		0, ctlY, win->w, LIBAROMA_SIZE_FULL, /* x,y,w,h */
		0, 0 /*8*/, /* horiz, vert padding */
		win->colorset->control_bg, /* bgcolor */
		LIBAROMA_CTL_SCROLL_WITH_SHADOW
		|LIBAROMA_CTL_SCROLL_WITH_HANDLE
	);
	ctlY += list->h;
	printf("Loading icons\n");
	LIBAROMA_CANVASP info_icon = libaroma_image_uri("res:///icons/info.png");
	libaroma_canvas_fillcolor(info_icon, win->colorset->accent);
	LIBAROMA_CANVASP wifi_icon = libaroma_image_uri("res:///icons/wifi.png");
	libaroma_canvas_fillcolor(wifi_icon, win->colorset->accent);
	LIBAROMA_CANVASP bt_icon = libaroma_image_uri("res:///icons/bt.png");
	libaroma_canvas_fillcolor(bt_icon, win->colorset->accent);
	LIBAROMA_CANVASP rr_orig_icon = libaroma_image_uri("res:///icons/rr-orig.png");
	libaroma_canvas_fillcolor(rr_orig_icon, win->colorset->accent);
	
	word netitemsflags = LIBAROMA_LISTITEM_CHECK_SWITCH
			|LIBAROMA_LISTITEM_MENU_SMALL_ICON
			//|LIBAROMA_LISTITEM_CHECK_HAS_SUBMENU /* commented because it's from new version */
				|LIBAROMA_LISTITEM_WITH_SEPARATOR
				|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN;
	
	_TITLE(ID_CAPTION1, "Conectividad");
		LIBAROMA_CTL_LIST_ITEMP wifichk = 
			_CHECK(ID_WIFICHK, "Wi-Fi", wifi_icon, NULL, netitemsflags);
		LIBAROMA_CTL_LIST_ITEMP btchk = 
			_CHECK(ID_BTCHK, "Bluetooth", bt_icon, NULL, netitemsflags);
	_DIV(300); /* 300 can be any ID number */
	_TITLE(ID_CAPTION2, "Sistema");
		_ITEM(ID_RRSETTINGSBTN, "Configuraciones", rr_orig_icon, NULL);
		_ITEM(ID_INFOBTN, "Acerca de", info_icon, NULL);
	
	libaroma_window_anishow(win, LIBAROMA_WINDOW_SHOW_ANIMATION_PAGE_TOP, 400);
	
	byte onpool = 1;
	do{
				
		LIBAROMA_MSG msg;
		dword command=libaroma_window_pool(win,&msg);
		byte cmd	= LIBAROMA_CMD(command);
		word id	 = LIBAROMA_CMD_ID(command);
		byte param = LIBAROMA_CMD_PARAM(command);
		if (msg.msg==LIBAROMA_MSG_KEY_SELECT){
			if (msg.state==0){
				printf("Close key pressed!\n");
				onpool = 0;
				break;
			}
		}
		
		if (cmd == 1){
			printf("ID: %i, MSG key: %i\n", id, msg.key);
			switch (msg.key){
				case ID_WIFICHK:
					break;
				case ID_BTCHK:
					break;
				case ID_RRSETTINGSBTN:
					break;
				case ID_INFOBTN:
					break;
			}
		}
		
	}
	while(onpool);
	//window pool exited, so the window should close.
	
	//libaroma_window_aniclose(win, NULL, LIBAROMA_WINDOW_CLOSE_ANIMATION_PAGE_BOTTOM, 300);	
	/* the function above is from the new version, so let's use the original method*/
	//this frees the window's memory
	libaroma_window_free(win);
	
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
	