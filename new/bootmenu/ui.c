#ifndef __bootmenu_ui_c__
#define __bootmenu_ui_c__
#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

byte bootmenu_haltreboot=0;
byte scrolling=0;

void homelist_onclick(LIBAROMA_CONTROLP list){
	if (scrolling>0) {
		printf("MLX: click cancelled by scroll\n");
		scrolling--;
		return;
	}
	LIBAROMA_CTL_LIST_ITEMP item = libaroma_ctl_list_get_touched_item(list);
	if (!item) return;
	switch (item->id){
		case ID_HOMELIST_DROID:
			break;
		case ID_HOMELIST_RECV:
			break;
		case ID_HOMELIST_AFM:{
			libaroma_ctl_fragment_set_active_window(fragctl, ID_SETTFRAG, 0, 400, 0, NULL, NULL, NULL);
			bootmenu_bar_update("Aroma FM", NULL, LIBAROMA_CTL_BAR_ICON_DRAWER_TO_ARROW);
			
			pid_t child_pid;
			int child_status;
			
			child_pid = fork();
			if(child_pid == 0) {
				/* This is done by the child process. */
				execl("/tmp/aromafm", "/tmp/aromafm", "3", "0", "/sdcard/aromafm.zip", NULL);
				//execl("/usr/bin/aromafm", "/usr/bin/aromafm", "3", "0", "/usr/bin/aromafm.zip", NULL);
				
				/* If execv returns, it must have failed. */
				exit(0);
			}
			else {
				/* This is run by the parent.  Wait for the child
				 * to terminate. */
				pid_t tpid;
				do {
					tpid = wait(&child_status);
				} while(tpid != child_pid);
			}
			libaroma_ctl_fragment_set_active_window(fragctl, ID_HOMEFRAG, 0, 400, 0, NULL, NULL, NULL);
			bootmenu_bar_update("Bootmenu", NULL, LIBAROMA_CTL_BAR_ICON_ARROW_TO_DRAWER);
		} break;
		case ID_HOMELIST_FILEMGR:
			bootmenu_filemgr_loadpath(strdup("/"));
			break;
		case ID_HOMELIST_SETT:
			libaroma_ctl_fragment_set_active_window(fragctl, ID_SETTFRAG, 0, 400, 0, NULL, NULL, NULL);
			bootmenu_bar_update("Settings", NULL, LIBAROMA_CTL_BAR_ICON_DRAWER_TO_ARROW);
			break;
		case ID_HOMELIST_REBOOT:
			bootmenu_haltreboot=1;
			winmain->onpool=0;
			break;
		case ID_HOMELIST_PWOFF:
			bootmenu_haltreboot=2;
			winmain->onpool=0;
			break;
		case ID_HOMELIST_EXIT:
			winmain->onpool=0;
			/* send message to main window in order to exit */
			break;
	}
}
	
byte bootmenu_home_init(word bgcolor, word fillcolor){
	/* initialize appbar icon as drawer */
	libaroma_ctl_bar_set_icon(appbar, NULL, 0, LIBAROMA_CTL_BAR_ICON_DRAWER, 0);
	/* load some icons */
	LIBAROMA_CANVASP 
		afm_cv=libaroma_image_uri("res:///icons/aromafm.png"),
		recovery_cv=libaroma_image_uri("res:///icons/twrp.png"),
		settings_cv=libaroma_image_uri("res:///icons/settings.png"),
		reboot_cv=libaroma_image_uri("res:///icons/reboot.png"),
		shutdown_cv=libaroma_image_uri("res:///icons/shutdown.png"),
		exit_cv=libaroma_image_uri("res:///icons/exit.png");
	if (!afm_cv || !settings_cv || !recovery_cv || !reboot_cv || !shutdown_cv)
		return 0;
	
	/* fill icons */
	libaroma_canvas_fillcolor(settings_cv, fillcolor);
	libaroma_canvas_fillcolor(recovery_cv, fillcolor);
	libaroma_canvas_fillcolor(reboot_cv, fillcolor);
	libaroma_canvas_fillcolor(shutdown_cv, fillcolor);
	libaroma_canvas_fillcolor(exit_cv, fillcolor);
	
	/* populate home fragment */
	homelist=libaroma_ctl_list(homefrag, ID_HOMELIST, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL, 0, 0, bgcolor, LIBAROMA_CTL_LIST_WITH_SHADOW);
	if (!homelist) return 0;
	libaroma_listitem_caption(homelist, 0, "SYSTEM", -1);
	libaroma_listitem_menu(homelist, ID_HOMELIST_DROID, "Android", NULL, libaroma_image_uri("res:///icons/droid.png"), 	LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN|LIBAROMA_LISTITEM_MENU_FREE_ICON, -1);
	libaroma_listitem_menu(homelist, ID_HOMELIST_RECV, "Recovery", NULL, recovery_cv, LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN|LIBAROMA_LISTITEM_MENU_FREE_ICON, -1);
	libaroma_listitem_menu(homelist, ID_HOMELIST_AFM, "Aroma FM", NULL, afm_cv, LIBAROMA_LISTITEM_MENU_FREE_ICON, -1);
	libaroma_listitem_caption(homelist, 0, "TOOLS", -1);
	libaroma_listitem_menu(homelist, ID_HOMELIST_FILEMGR, "Open ramdisk", NULL, libaroma_image_uri("res:///icons/filemgr.png"), LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN|LIBAROMA_LISTITEM_MENU_FREE_ICON, -1);
	libaroma_listitem_menu(homelist, ID_HOMELIST_SETT, "Settings", NULL, settings_cv, LIBAROMA_LISTITEM_MENU_FREE_ICON, -1);
	libaroma_listitem_caption(homelist, 0, "POWER", -1);
	libaroma_listitem_menu(homelist, ID_HOMELIST_REBOOT, "Reboot", NULL, reboot_cv, LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN|LIBAROMA_LISTITEM_MENU_FREE_ICON, -1);
	libaroma_listitem_menu(homelist, ID_HOMELIST_PWOFF, "Power off", NULL, shutdown_cv, LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN|LIBAROMA_LISTITEM_MENU_FREE_ICON, -1);
	libaroma_listitem_menu(homelist, ID_HOMELIST_EXIT, "Exit", NULL, exit_cv, LIBAROMA_LISTITEM_MENU_FREE_ICON, -1);
	/* set list click handler */
	libaroma_control_set_onclick(homelist, &homelist_onclick);
	return 1;
}

byte bootmenu_ui(){
	winmain=libaroma_window(NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
	//libaroma_ctl_button(win, 2, 0, 0, LIBAROMA_SIZE_FULL, 60, "Close", LIBAROMA_CTL_BUTTON_RAISED, RGB(FFFFFF));
	
	/* create primary controls */
	appbar = libaroma_ctl_bar(winmain, 1, 0, 0, LIBAROMA_SIZE_FULL, 56, "Bootmenu", RGB(009385), RGB(FFFFFF));
	fragctl = libaroma_ctl_fragment(winmain, ID_FRAGCTL, 0, 56, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
	/* create home fragment */
	homefrag = libaroma_ctl_fragment_new_window(fragctl, ID_HOMEFRAG);
	/* create further fragments */
	settfrag = libaroma_ctl_fragment_new_window(fragctl, ID_SETTFRAG);
	filemfrag = libaroma_ctl_fragment_new_window(fragctl, ID_FILEMFRAG);
	filesfrag = libaroma_ctl_fragment_new_window(fragctl, ID_FILESFRAG);
	/* call individual init routines */
	if (!bootmenu_home_init(RGB(FFFFFF), RGB(0)))
		goto rip1;
	if (!bootmenu_filemgr_init(RGB(FFFFFF)))
		goto rip1;
	
	/* set home as active window */
	libaroma_ctl_fragment_set_active_window(fragctl, ID_HOMEFRAG, 0, 0, 0, NULL, NULL, NULL);
	/* show main window */
	libaroma_window_anishow(winmain, 12, 400);
	winmain->onpool=1;
	int last_simtouch_x=-1, last_simtouch_y=-1;
	LIBAROMA_CTL_LIST_ITEMP focused=NULL;
	long last_keytick=0;
	do {
		LIBAROMA_MSG msg;
		dword command	= libaroma_window_pool(winmain,&msg);
		byte cmd		= LIBAROMA_CMD(command);
		word id			= LIBAROMA_CMD_ID(command);
		byte param		= LIBAROMA_CMD_PARAM(command);
		if (msg.msg==LIBAROMA_MSG_EXIT || ((msg.key==116||msg.key==107) && !msg.state))
			winmain->onpool=0;
		if (msg.msg!=LIBAROMA_MSG_TOUCH){
			if (msg.key==158){
				libaroma_msg_post(
					LIBAROMA_MSG_TOUCH,
					msg.state,
					0,
					libaroma_dp(36),
					libaroma_dp(54),
					NULL
				);
			}
			else if (!msg.state){
				switch(libaroma_ctl_fragment_get_active_window_id(fragctl)){
					case ID_HOMEFRAG:{
						switch (msg.key){
							case 232:{
								//if (scrolling) continue;
								if (libaroma_ctl_list_get_touched_item(homelist)){
									libaroma_msg_post(
										LIBAROMA_MSG_TOUCH,
										0,
										0,
										last_simtouch_x,
										last_simtouch_y,
										NULL
									);
									libaroma_msg_post(
										LIBAROMA_MSG_TOUCH,
										1,
										0,
										last_simtouch_x,
										last_simtouch_y,
										NULL
									);
									libaroma_msg_post(
										LIBAROMA_MSG_TOUCH,
										0,
										0,
										last_simtouch_x,
										last_simtouch_y,
										NULL
									);
									/*libaroma_window_post_command_ex(
										LIBAROMA_CMD_SET(LIBAROMA_CMD_CLICK, 0, homelist->id),
																	0, libaroma_ctl_list_get_touched_item(homelist)->id, 0, (voidp) homelist
									);*/
								}
							} break;
							case 115:
							case 0x67:{
								// prevent too fast key repeat (200ms)
								if (libaroma_tick()-last_keytick<200)
									continue;
								last_keytick=libaroma_tick();
								
								printf("MLX: up key\n");
								if (!focused){ /* first focus */
									focused=libaroma_ctl_list_get_first_item(homelist);
									printf("MLX: up loaded default\n");
									if (focused){
										/* skip captions/dividers, get to focusable */
										while (focused!=NULL && (libaroma_listitem_iscaption(focused)||libaroma_listitem_isdivider(focused))){
											focused=focused->next;
										}
									}
								}
								else {
									do {
										printf("MLX: going one item back\n");
										focused=focused->prev;
										printf("MLX: focused=%p\n", focused);
									} while (focused->prev!=NULL && (libaroma_listitem_iscaption(focused)||libaroma_listitem_isdivider(focused)));
									printf("MLX: found focused=%p\n", focused);
									if (focused->prev==NULL){
										/* already at first item, jump to last */
										printf("MLX: jump to last\n");
										focused=libaroma_ctl_list_get_last_item(homelist);
										if (focused){
											/* skip captions/dividers, but in reverse order */
											while (focused!=NULL && (libaroma_listitem_iscaption(focused)||libaroma_listitem_isdivider(focused)));{
												focused=focused->prev;
											}
										}
									}
								}
								/*LIBAROMA_CTL_LIST_ITEMP item=libaroma_ctl_list_get_item_internal(homelist, focused_idx, 0);
								if (item){
									//item=item->prev;
									if (item && libaroma_listitem_iscaption(item))
										item=item->prev;
								}
								else {
									item=libaroma_ctl_list_get_first_item(homelist);
									if (item && libaroma_listitem_iscaption(item))
										item=item->next;
								}*/
								if (focused) {
									if (last_simtouch_x!=-1){
										//printf("MLX: first send touch up at (%d, %d)\n", last_simtouch_x, last_simtouch_y);
										printf("MLX: release previous touch\n");
										scrolling++;
										libaroma_msg_post(
											LIBAROMA_MSG_TOUCH,
											0,
											0,
											last_simtouch_x,
											last_simtouch_y,
											NULL
										);
									}
									//printf("MLX focusing to %p\n", item);
									if (libaroma_ctl_list_scroll_to_item(homelist, focused, 1)){
										libaroma_sleep(200);
										LIBAROMA_RECT item_rect={0};
										libaroma_ctl_list_item_position(homelist, focused, &item_rect, 1);
										//printf("MLX: item info %dx%d (%d, %d)\n", item_rect.w, item_rect.h, item_rect.x, item_rect.y);
										int x, y;
										x=item_rect.x+(item_rect.w/2);
										y=item_rect.y+(item_rect.h/2)+libaroma_dp(24);
										//printf("MLX: send touch down at (%d, %d)\n", x, y);
										printf("MLX: hold touch\n");
										libaroma_msg_post(
											LIBAROMA_MSG_TOUCH,
											1,
											0,
											x,
											y,
											NULL
										);
										last_simtouch_x=x;
										last_simtouch_y=y;
									}
								}
								else printf("MLX failed to focus item\n");
							} break;
							case 0x6c:
							case 114:{
								// prevent too fast key repeat (200ms)
								if (libaroma_tick()-last_keytick<200)
									continue;
								last_keytick=libaroma_tick();
								
								printf("MLX: down key\n");
								if (!focused){ /* first focus */
									focused=libaroma_ctl_list_get_first_item(homelist);
									printf("MLX: down loaded default\n");
								}
								if (focused){
									/* skip captions/dividers, get to focusable */
									do {
										focused=focused->next;
									}
									while (focused!=NULL && (libaroma_listitem_iscaption(focused)||libaroma_listitem_isdivider(focused)));
								}
								/*else {
									if (focused->prev==NULL){
										// already at first item, jump to last
										focused=libaroma_ctl_list_get_last_item(homelist);
										if (focused){
											// skip captions/dividers, but in reverse order
											while (focused!=NULL && (libaroma_listitem_iscaption(focused)||libaroma_listitem_isdivider(focused))){
												focused=focused->prev;
											}
										}
									}
									else focused=focused->prev;
								}*/
								if (focused) {
									if (last_simtouch_x!=-1){
										//printf("MLX: first send touch up at (%d, %d)\n", last_simtouch_x, last_simtouch_y);
										printf("MLX: release previous touch\n");
										scrolling++;
										libaroma_msg_post(
											LIBAROMA_MSG_TOUCH,
											0,
											0,
											last_simtouch_x,
											last_simtouch_y,
											NULL
										);
									}
									//printf("MLX focusing to %p\n", item);
									if (libaroma_ctl_list_scroll_to_item(homelist, focused, 1)){
										libaroma_sleep(200);
										LIBAROMA_RECT item_rect={0};
										libaroma_ctl_list_item_position(homelist, focused, &item_rect, 1);
										//printf("MLX: item info %dx%d (%d, %d)\n", item_rect.w, item_rect.h, item_rect.x, item_rect.y);
										int x, y;
										x=item_rect.x+(item_rect.w/2);
										y=item_rect.y+(item_rect.h/2)+libaroma_dp(24);
										//printf("MLX: send touch down at (%d, %d)\n", x, y);
										printf("MLX: hold touch\n");
										libaroma_msg_post(
											LIBAROMA_MSG_TOUCH,
											1,
											0,
											x,
											y,
											NULL
										);
										last_simtouch_x=x;
										last_simtouch_y=y;
									}
								}
								else printf("MLX failed to focus item\n");
							} break;
							/*{
								if (libaroma_tick()-last_keytick<200)
									continue;
								last_keytick=libaroma_tick();
								printf("MLX: down key\n");
								LIBAROMA_CTL_LIST_ITEMP item=libaroma_ctl_list_get_touched_item(homelist);
								if (item){
									item=item->next;
									if (item && libaroma_listitem_iscaption(item))
										item=item->next;
								}
								else {
									item=libaroma_ctl_list_get_first_item(homelist);
									if (item && libaroma_listitem_iscaption(item))
										item=item->next;
								}
								if (item) {
									if (last_simtouch_x!=-1){
										//printf("MLX: first send touch up at (%d, %d)\n", last_simtouch_x, last_simtouch_y);
										printf("MLX: release previous touch\n");
										scrolling++;
										libaroma_msg_post(
											LIBAROMA_MSG_TOUCH,
											0,
											0,
											last_simtouch_x,
											last_simtouch_y,
											NULL
										);
									}
									//printf("MLX focusing to %p\n", item);
									if (libaroma_ctl_list_scroll_to_item(homelist, item, 1)){
										//libaroma_sleep(200);
										LIBAROMA_RECT item_rect={0};
										libaroma_ctl_list_item_position(homelist, item, &item_rect, 1);
										//printf("MLX: item info %dx%d (%d, %d)\n", item_rect.w, item_rect.h, item_rect.x, item_rect.y);
										int x, y;
										x=item_rect.x+(item_rect.w/2);
										y=item_rect.y+(item_rect.h/2)+libaroma_dp(24);
										//printf("MLX: send touch down at (%d, %d)\n", x, y);
										printf("MLX: hold touch\n");
										libaroma_msg_post(
											LIBAROMA_MSG_TOUCH,
											1,
											0,
											x,
											y,
											NULL
										);
										last_simtouch_x=x;
										last_simtouch_y=y;
									}
								}
								else printf("MLX failed to focus item\n");
							} break;
							*/
							case 0x69:{
								printf("left key?\n");
							} break;
							case 0x6a:{
								printf("right key?\n");
							} break;
						}
					} break;
				}
			}
		}
		if (cmd==LIBAROMA_CMD_CLICK){
			if (id==ID_APPBAR && param==1){ /* arrow/drawer touched */
				if (libaroma_ctl_fragment_get_active_window_id(fragctl) != ID_HOMEFRAG){
					libaroma_ctl_fragment_set_active_window(fragctl, ID_HOMEFRAG, 0, 400, 0, NULL, NULL, NULL);
					libaroma_ctl_bar_set_tools(appbar, NULL, 0);
					/* set icon */
					bootmenu_bar_update("Bootmenu", NULL, LIBAROMA_CTL_BAR_ICON_ARROW_TO_DRAWER);
				continue;
				}
			}
			switch (libaroma_ctl_fragment_get_active_window_id(fragctl)){
				case ID_FILEMFRAG:
				case ID_FILESFRAG:{
					if (param==ID_APPBAR_UP){ /* title touched */
						bootmenu_filemgr_goup();
					}
				} break;
			}
		}
		else {
			//printf("msg=%d key=%d state=%d (%d, %d)\n", msg.msg, msg.key, msg.state, msg.x, msg.y);
		}
	} while(winmain->onpool);
	libaroma_png_save(libaroma_fb()->canvas, "/tmp/asd.png");
	
	/* handle onclose free */
	switch (libaroma_ctl_fragment_get_active_window_id(fragctl)){
		case ID_FILEMFRAG:
		case ID_FILESFRAG:{
			bootmenu_filemgr_exit();
		} break;
	}
	
	libaroma_window_aniclose(winmain, 12, 400);
	return 0;
rip1:
	libaroma_window_free(winmain);
	return 1;
}

byte bootmenu_bar_update(char *title, char *subtitle, byte iconflag){
	/* set title/subtitle */
	libaroma_ctl_bar_set_title(appbar, title, 0);
	libaroma_ctl_bar_set_subtitle(appbar, subtitle, 0);
	
	if (iconflag) /* pass 0 to not update icon */
		libaroma_ctl_bar_set_icon(appbar, NULL, 0, iconflag, 0);
	/* update now */
	libaroma_ctl_bar_update(appbar);
	return 1;
}

#ifdef __cplusplus
}
#endif
#endif /* __bootmenu_ui_c__ */  
