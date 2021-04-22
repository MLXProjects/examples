#ifndef __libaroma_settings_c__
#define __libaroma_settings_c__

#include "popen_noshell.h"
#include <aroma.h>
#include "statusbar.c"
#include "about.c"
#include "wifi.c"
#include "bt.c"
#include <unistd.h>
#include <errno.h>


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
	
	//libaroma_wm_set_workspace(0, 0, libaroma_fb()->w, libaroma_fb()->h);	
}

LIBAROMA_WINDOWP win;

void powerdialog(LIBAROMA_WINDOWP parent){	

	#define POWERDIALOG_ITEMS	2
	#define ID_PWDLG_OFF		13
	#define ID_PWDLG_REBOOT		14	
	
	word menuflags =
		LIBAROMA_LISTITEM_MENU_INDENT_NOICON|
		LIBAROMA_LISTITEM_MENU_SMALL_ICON|
		LIBAROMA_LISTITEM_MENU_FREE_ICON|
		LIBAROMA_LISTITEM_WITH_SEPARATOR|
		LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN;
	
	LIBAROMA_CANVASP power_icon = libaroma_image_uri("res:///icons/power.png");
	libaroma_canvas_fillcolor(power_icon, win->colorset->accent);
	LIBAROMA_CANVASP reboot_icon = libaroma_image_uri("res:///icons/reboot.png");
	libaroma_canvas_fillcolor(reboot_icon, win->colorset->accent);
	LIBAROMA_LISTITEM_TEMPLATE templates[POWERDIALOG_ITEMS] = {
		/* kind of item, id, title, subtitle, image/icon, image height, selected, 
			flags */
		{LIBAROMA_LIST_ITEM_KIND_MENU, ID_PWDLG_OFF, "Power off", NULL, power_icon, NULL, NULL, 
			menuflags}, 
		{LIBAROMA_LIST_ITEM_KIND_MENU, ID_PWDLG_REBOOT, "Reboot", NULL, reboot_icon, NULL, NULL, 
			menuflags}
	};
	
	int res=libaroma_dialog_list(
		"Power menu",				//title
		NULL,						//button 1
		NULL,	 					//button 2
		templates, 					//items list
		POWERDIALOG_ITEMS, 			//items count
		1, 							//close dialog on select
		NULL, 						//colorset
		LIBAROMA_DIALOG_DIM_PARENT|LIBAROMA_DIALOG_WITH_SHADOW| //flags
		LIBAROMA_DIALOG_ACCENT_BUTTON|LIBAROMA_DIALOG_CANCELABLE
	);
	//printf("DIALOG RESULT: %i\n",res);
	if (res==ID_PWDLG_OFF){
		printf("Close now! \n");
		win->onpool=0;
	}
	//show main window again
	libaroma_window_anishow(parent, LIBAROMA_WINDOW_SHOW_ANIMATION_FADE, 300);
} 

#define PIPE_READ 0
#define PIPE_WRITE 1

char *cmd_out(char *args[], const int max_buffer) {
	FILE *fp;
	char buf[max_buffer];
	char retval[max_buffer];
	strcpy(retval, "");
	int status;
	struct popen_noshell_pass_to_pclose pclose_arg;
	
	fp = popen_noshell(args[0], (const char * const *)args, "r", &pclose_arg, 0);
	if (!fp) {
		printf("Function failed: %s\n", "popen_noshell()");
		return retval;
	}
	while (fgets(buf, sizeof(buf)-1, fp)) {
		strcat(retval, buf);
	}
	//printf("%s\n", retval);
	status = pclose_noshell(&pclose_arg);
	if (status == -1) {
		printf("Function failed: %s\n", "pclose_noshell()");
	}	
	int lenght = strlen(retval); 
	if( retval[lenght-1] == '\n') {
		retval[lenght-1] = 0;
	}
	return retval;
}

void winmain(){
	
	#define ID_APPBAR			2
	#define ID_CTLLIST			3
	#define ID_CONTEXTMENU		4
	#define ID_CAPTION1			5
	#define ID_CAPTION2			6
	#define ID_WIFICHK			7
	#define ID_BTCHK			8
	#define ID_SIMBTN			9
	#define ID_RRSETTINGSBTN	10
	#define ID_INFOBTN			11
	#define ID_DLGLIST			12
	/* menu flags */
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
	int ctlY = STATUSBAR_HEIGHT;
	int ctlH = 46;
	
	printf("creating null colorset\n");
	LIBAROMA_COLORSETP my_colorset = NULL;
	printf("initializing colorset\n");
	my_colorset = libaroma_colorget(NULL, win);/*
	my_colorset->window_bg = RGB(424242);
	my_colorset->window_text = RGB(E5E5E5);
	my_colorset->primary = RGB(80CBC4);
	my_colorset->primary_light = RGB(E5E5E5);*/
	my_colorset->accent = RGB(80CBC4);/*
	my_colorset->control_bg = RGB(424242);
	my_colorset->control_primary_text = RGB(E5E5E5);
	my_colorset->control_secondary_text = RGB(E5E5E5);
	my_colorset->dialog_bg = my_colorset->control_bg;
	my_colorset->dialog_primary_text = my_colorset->control_primary_text;
	my_colorset->dialog_secondary_text = my_colorset->control_secondary_text;*/
	printf("stablishing colorset\n");
	win->colorset = my_colorset;
	
	// status bar will be created after window being shown, 
	// so let's create a dark region (to overcome how strange
	// would a bright region right at the appbar's top look)
	
	libaroma_draw_rect(win->bg, 0, 0, win->w, STATUSBAR_HEIGHT, RGB(000000), 0x00);
	libaroma_draw_rect(win->dc, 0, 0, win->w, STATUSBAR_HEIGHT, RGB(000000), 0x00);
	
	printf("creating bar\n");
	LIBAROMA_CONTROLP bar = libaroma_ctl_bar(
		win, ID_APPBAR,
		0, ctlY, LIBAROMA_SIZE_FULL, ctlH,
		"Settings", RGB(263238), RGB(ffffff)
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
			|LIBAROMA_LISTITEM_CHECK_HAS_SUBMENU
				|LIBAROMA_LISTITEM_WITH_SEPARATOR
				|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN;
	
	_TITLE(ID_CAPTION1, "Wireless & networks");
		LIBAROMA_CTL_LIST_ITEMP wifichk = 
			_CHECK(ID_WIFICHK, "Wi-Fi", wifi_icon, NULL, netitemsflags);
		LIBAROMA_CTL_LIST_ITEMP btchk = 
			_CHECK(ID_BTCHK, "Bluetooth", bt_icon, NULL, netitemsflags);
	_DIV(300);
	_TITLE(ID_CAPTION2, "System");
		_ITEM(ID_RRSETTINGSBTN, "RR settings", rr_orig_icon, NULL);
		_ITEM(ID_INFOBTN, "About phone", info_icon, NULL);

	
	libaroma_ctl_bar_set_icon_mask(bar,1,1);
	//libaroma_ctl_bar_set_touchable_title(bar,1);
	
	libaroma_ctl_bar_set_menuid(bar, ID_CONTEXTMENU, 1);
	
	libaroma_window_anishow(win, LIBAROMA_WINDOW_SHOW_ANIMATION_PAGE_TOP, 400);
	
	libaroma_wm_set_active_window(win);
	pthread_t sb_thread = statusbar_draw(win);
	
	do{
				
		LIBAROMA_MSG msg;
		dword command=libaroma_window_pool(win,&msg);
		byte cmd	= LIBAROMA_CMD(command);
		word id	 = LIBAROMA_CMD_ID(command);
		byte param = LIBAROMA_CMD_PARAM(command);
		if (msg.msg==LIBAROMA_MSG_KEY_SELECT || msg.msg==LIBAROMA_MSG_KEY_POWER){
			if (msg.state==0){
				//libaroma_png_save(libaroma_fb()->canvas,"/tmp/screenshot.png");
				printf("Close key pressed!\n");
				win->onpool = 0;
				break;
			}
		}
		//printf("Main window received MSG msg: %i\n", msg.msg);
		
		if (cmd == 1){
			printf("ID: %i, MSG key: %i\n", id, msg.key);
			switch (msg.key){
				case ID_WIFICHK:
					wifi_settings(win);
					break;
				case ID_BTCHK:
					bt_settings(win);
					break;
				case ID_RRSETTINGSBTN:
					//powerdialog(win);
					//libaroma_window_anishow(win, LIBAROMA_WINDOW_CLOSE_ANIMATION_PAGE_BOTTOM, 400);
					break;
				case ID_INFOBTN:
					//statusbar_updater_stop();
					printf("Calling about.c\n");
					about(win);
					//statusbar_updater_start();
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