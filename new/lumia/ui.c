#ifndef __aromafm_ui_c__
#define __aromafm_ui_c__
#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

void aromafm_statusbar_update(char *text){
	libaroma_draw_rect(libaroma_fb()->canvas, 0, 0, libaroma_fb()->w, aromafm_statusbar_height(), RGB(006b61), 0xFF);
	libaroma_draw_text(libaroma_fb()->canvas, text, 0, 0, RGB(FFFFFF), libaroma_fb()->w, LIBAROMA_FONT(0, 4)|LIBAROMA_TEXT_CENTER|LIBAROMA_TEXT_SINGLELINE, aromafm_statusbar_height());
}

byte aromafm_sb_init(word bgcolor){
	sidebar = libaroma_window_sidebar(winmain, 0);
	if (!sidebar) return 0;
	sidelist = libaroma_ctl_list(sidebar, ID_SIDELIST, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL, 0, 0, bgcolor, LIBAROMA_CTL_LIST_WITH_SHADOW);
	if (!sidelist) return 0;
	libaroma_control_set_onclick(sidelist, &sidelist_onclick);
	libaroma_listitem_caption(sidelist, ID_SIDELIST_FOLDERS, "Folders", -1);
	libaroma_listitem_menu(sidelist, ID_SIDELIST_FILEMGR, "The one open now", NULL, NULL, LIBAROMA_LISTITEM_MENU_SMALL, -1);
	libaroma_listitem_caption(sidelist, 0, "Tools", -1);
	libaroma_listitem_menu(sidelist, ID_SIDELIST_SETT, "Settings", NULL, NULL, LIBAROMA_LISTITEM_MENU_SMALL, -1);
	libaroma_listitem_menu(sidelist, ID_SIDELIST_ABOUT, "About", NULL, NULL, LIBAROMA_LISTITEM_MENU_SMALL, -1);
	libaroma_listitem_menu(sidelist, ID_SIDELIST_EXIT, "Exit", NULL, NULL, LIBAROMA_LISTITEM_MENU_SMALL, -1);
	return 1;
}

byte aromafm_init(word bgcolor){
	LIBAROMA_CANVASP arrow=libaroma_canvas_ex(libaroma_dp(48), libaroma_dp(48), 1);
	if (!arrow) return 0;
	libaroma_art_arrowdrawer(arrow, 1.0, 0, 0, 0, arrow->w, 0, 0xFF, 1, 0.5);
	if (libaroma_color_isdark(winmain->appbar_bg)) libaroma_canvas_fillcolor(arrow, RGB(FFFFFF));
	aromafm()->up_arrow=libaroma_canvas_rotate(arrow, 90);
	libaroma_canvas_free(arrow);
	if (!aromafm()->up_arrow) return 0;
	aromafm()->bar_tools=libaroma_ctl_bar_tools(1);
	if (!aromafm()->bar_tools) return 0;
	libaroma_ctl_bar_tools_set(aromafm()->bar_tools, 0, ID_APPBAR_UP, NULL, aromafm()->up_arrow, LIBAROMA_CTL_BAR_TOOL_ICON_SHARED);
	aromafm()->folder_cv=libaroma_image_uri("res:///icons/folder.png");
	if (!aromafm()->folder_cv) return 0;
	aromafm()->file_cv=libaroma_image_uri("res:///icons/file.png");
	if (!aromafm()->file_cv) return 0;
	aromafm()->bgcolor=bgcolor;
	aromafm()->onmain_list=1;
	return 1;
}

byte aromafm_ui(){
	winmain=libaroma_window(NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
	
	/* init sidebar */
	/*if (!aromafm_sb_init(RGB(FFFFFF)))
		goto rip;
	*/
	/* create primary controls */
	appbar = libaroma_ctl_bar(winmain, 1, 0, 0, LIBAROMA_SIZE_FULL, 56, "MLX", RGB(009385), RGB(FFFFFF));
	//busyprog = libaroma_ctl_progress(winmain, 0, 0, 56, LIBAROMA_SIZE_FULL, 4, LIBAROMA_CTL_PROGRESS_DETERMINATE, 0, 1);
	//libaroma_ctl_progress_bgcolor(busyprog, 1, RGB(009385));
	fragctl = libaroma_ctl_fragment(winmain, ID_FRAGCTL, 0, 60, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
	/* create app fragments */
	//busyfrag = libaroma_ctl_fragment_new_window(fragctl, ID_BUSYFRAG);
	homefrag = libaroma_ctl_fragment_new_window(fragctl, ID_HOMEFRAG);
	filemfrag = libaroma_ctl_fragment_new_window(fragctl, ID_FILEMFRAG);
	filesfrag = libaroma_ctl_fragment_new_window(fragctl, ID_FILESFRAG);
	settfrag = libaroma_ctl_fragment_new_window(fragctl, ID_SETTFRAG);
	/* call individual init routines */
	if (!mlx_home_init(RGB(FFFFFF)))
		goto rip;
	
	if (!mlx_settings_init(0))
		goto rip;
	
	if (!aromafm_init(RGB(FFFFFF)))
		goto rip;
	
	/* open rootdir as first path */
	libaroma_ctl_fragment_set_active_window(fragctl, ID_HOMEFRAG, 0, 0, 0, NULL, NULL, NULL);
	
	/* set appbar drawer */
	libaroma_ctl_bar_set_icon(appbar, NULL, 0, LIBAROMA_CTL_BAR_ICON_DRAWER, 1);
	
	/* draw statusbar */
	aromafm_statusbar_reset();
	/* draw navbar */
	mlx_navbar_init();
	/* show main window */
	libaroma_window_anishow(winmain, 12, MLX_WINSHOW_DURATION);
	winmain->onpool=1;
	byte updating_nb=0;
	do {
		LIBAROMA_MSG msg;
		dword command	= libaroma_window_pool(winmain,&msg);
		byte cmd		= LIBAROMA_CMD(command);
		word id			= LIBAROMA_CMD_ID(command);
		byte param		= LIBAROMA_CMD_PARAM(command);
		if (msg.msg==LIBAROMA_MSG_EXIT)
			winmain->onpool=0;
		else if (cmd==LIBAROMA_CMD_CLICK){
			if (id==ID_APPBAR){ /* arrow/drawer touched */
				if (param==1){
					switch (libaroma_ctl_fragment_get_active_window_id(fragctl)){
						case ID_HOMEFRAG:{
							//libaroma_window_sidebar_show(sidebar, 1);
						} break;
						case ID_FILEMFRAG:
						case ID_FILESFRAG:
						case ID_SETTFRAG:{
							libaroma_ctl_fragment_set_active_window(fragctl, ID_HOMEFRAG, 0, MLX_ANISWITCH_DURATION, 0, NULL, NULL, NULL);
							aromafm_bar_update("LumiaTest", NULL, LIBAROMA_CTL_BAR_ICON_ARROW_TO_DRAWER);
						} break;
					}
				}
				else switch (libaroma_ctl_fragment_get_active_window_id(fragctl)){
						case ID_FILEMFRAG:
						case ID_FILESFRAG:{
							if (param==ID_APPBAR_UP){ /* title touched */
								aromafm_goup();
							}
						} break;
				}
			}
		}
		else if (msg.msg==LIBAROMA_MSG_TOUCH){
			if (msg.y > (libaroma_wm()->h)){ /* touch outside of window workspace */
				int x=msg.x; /* first button */
				int y=msg.y-(libaroma_wm()->h);
				if (msg.x>libaroma_fb()->w-(navbar()->button_width)){ /* third button */
					if (!navbar()->updating) navbar()->updated_btn=FM_NAVBAR_MENU;
				}
				else if (msg.x>navbar()->button_width){ /* second button */
					if (!updating_nb) navbar()->updated_btn=FM_NAVBAR_HOME;
				}
				else { /* first button */
					if (!updating_nb) navbar()->updated_btn=FM_NAVBAR_BACK;
				}
				/* fix position */
				x-=navbar()->button_width*navbar()->updated_btn;
				if (x>navbar()->button_width){
					printf("fixing x=%d too big\n", x);
					x=navbar()->button_width;
				}
				else if (x<0){
					printf("fixing x=%d too low\n", x);
					x=0;
				}
				//printf("touch at %d, %d (%d)\n", x, y, navbar()->button_width);
				/* handle touch states */
				switch (msg.state){
					case LIBAROMA_HID_EV_STATE_DOWN:
						navbar()->updating=1;
						libaroma_ripple_down(navbar()->ripple, x, y);
						break;
					case LIBAROMA_HID_EV_STATE_MOVE:
						libaroma_ripple_move(navbar()->ripple, x, y);
						break;
					case LIBAROMA_HID_EV_STATE_UP:{
						byte ret = libaroma_ripple_up(navbar()->ripple, 0);
						if ((ret&LIBAROMA_RIPPLE_TOUCHED) && !(ret&LIBAROMA_RIPPLE_HOLDED))
							aromafm_navbar_onclick(navbar()->updated_btn);
						navbar()->updating=0;
						} break;
				}
			}
			else if (navbar()->updating){ /* moved outside of navbar while dragging */
				navbar()->updating=0;
				libaroma_ripple_cancel(navbar()->ripple);
			}
		}
		else {
			printf("msg=%d key=%d (%d, %d)\n", msg.msg, msg.key, msg.x, msg.y);
		}
	} while(winmain->onpool);
	
	/* handle onclose free */
	switch (libaroma_ctl_fragment_get_active_window_id(fragctl)){
		case ID_FILEMFRAG:
		case ID_FILESFRAG:{
			aromafm_exit();
		} break;
	}
	
	libaroma_window_aniclose(winmain, 12, MLX_WINSHOW_DURATION);
	return 1;
rip:
	/* cleanup fm if initialized */
	if (aromafm()->cwd!=NULL){
		aromafm_exit();
	}
	libaroma_window_free(winmain);
	return 0;
}

byte aromafm_bar_update(char *title, char *subtitle, byte iconflag){
	/* set title/subtitle */
	//printf("setting title=%s subtitle=%s\n", title, subtitle);
	libaroma_ctl_bar_set_title(appbar, title, 0);
	libaroma_ctl_bar_set_subtitle(appbar, subtitle, 0);
	
	if (iconflag){ /* pass 0 to not update icon */
		//printf("setting icon\n");
		libaroma_ctl_bar_set_icon(appbar, NULL, 0, iconflag, 0);
	}
	/* update now */
	//printf("updating appbar\n");
	libaroma_ctl_bar_update(appbar);
	return 1;
}

#ifdef __cplusplus
}
#endif
#endif /* __aromafm_ui_c__ */  
 
