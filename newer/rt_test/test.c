#include "common.h"

void recents_view(){
	lart_application_set_process_name("com.aromadev.recents");
	LIBAROMA_WINDOWP win = libaroma_window(
    NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
	//int *apps_count;
    LART_APP **apps_list = lart_sysui_get_running_applications(/*apps_count*/);
    //LIBAROMA_CONTROLP list = libaroma_ctl_list(); //make list with apps
    /*
		int i;
		for (i=0; i<apps_count; i++){
			LART_APP *app = apps_list[i];
			libaroma_ctl_listitem_check(i, app->title, app->name);
		}
    */
	word primary_color = RGB(00aacc);
	libaroma_window_show(win);
	lart_application_set_primary_color(primary_color);
	lart_application_set_foreground();
	byte onpool=1;
	do {
		LIBAROMA_MSG msg;
		dword command=libaroma_window_pool(win,&msg);
		byte cmd	= LIBAROMA_CMD(command);
		word id	 = LIBAROMA_CMD_ID(command);
		byte param = LIBAROMA_CMD_PARAM(command);

		if (cmd==1){
			if (id==1){
				if (param==1){
					//handle item click
				}
			}
		}
	} while (onpool && lart_application_is_run());
	libaroma_window_free(win);
}

int app_run(LART_APP *app /*char * program, char * param*/){

	char *program = app->program;
	char *param = app->param;
	TLOG("starting program \"%s\" with param \"%s\"", program, param);

	if (0==strcmp(program, "recents")) {
		recents_view();
		return 1;
	}
	LIBAROMA_WINDOWP win = libaroma_window(
    NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
	word primary_color = RGB(009385);

	if (strcmp(program,"Test2")==0){
		lart_application_set_process_name("com.aromadev.test2");
		primary_color = RGB(446688);
	}
	else if (strcmp(program,"Test3")==0){
		lart_application_set_process_name("com.aromadev.test3");
		primary_color = RGB(666666);
	}
	else{
		lart_application_set_process_name("com.aromadev.launcher");
	}

	LIBAROMA_CONTROLP bar = libaroma_ctl_bar(
		win, 1,
		0, 0, LIBAROMA_SIZE_FULL, 56,
		program, primary_color, RGB(ffffff)
	);
	libaroma_ctl_bar_set_icon(
		bar,NULL,0,LIBAROMA_CTL_BAR_ICON_DRAWER,0
	);
	libaroma_ctl_bar_set_touchable_title(bar,1);

	if (strcmp(program,"Test2")==0){
		libaroma_ctl_bar_set_icon(
			bar,NULL,0,LIBAROMA_CTL_BAR_ICON_ARROW,0
		);
		libaroma_ctl_button(
			win, 6, 0, 60, LIBAROMA_SIZE_FULL, 60,
			"This is Test 2", LIBAROMA_CTL_BUTTON_RAISED, 0
		);
	}
	else if (strcmp(program,"Test3")==0){
		libaroma_ctl_bar_set_icon(
			bar,NULL,0,LIBAROMA_CTL_BAR_ICON_ARROW,0
		);
		libaroma_ctl_button(
			win, 6, 0, 60, LIBAROMA_SIZE_FULL, 60,
			"This is Test 3", LIBAROMA_CTL_BUTTON_RAISED, 0
		);
	}
	else{
		libaroma_ctl_button(
			win, 6, 0, 60, LIBAROMA_SIZE_FULL, 60,
			"Start Test App 3", LIBAROMA_CTL_BUTTON_RAISED, 0
		);

		libaroma_ctl_button(
			win, 7, 0, 120, LIBAROMA_SIZE_FULL, 60,
			"Start Test App 2", LIBAROMA_CTL_BUTTON_RAISED, 0
		);
	}

	libaroma_window_anishow(win, LIBAROMA_WINDOW_SHOW_ANIMATION_CIRCLE, 500);
	lart_application_set_foreground();
	lart_application_set_primary_color(primary_color);

	printf("APP ==> POOL\n");
	byte onpool=1;
	do{
		LIBAROMA_MSG msg;
		dword command=libaroma_window_pool(win,&msg);
		byte cmd	= LIBAROMA_CMD(command);
		word id	 = LIBAROMA_CMD_ID(command);
		byte param = LIBAROMA_CMD_PARAM(command);

		if (msg.msg==LIBAROMA_MSG_KEY_POWER){
			if (0==strcmp(program, "Test1") && msg.state==0){
				lart_application_start("recents", "");
			}
		}
		else if (cmd==1){
			if (id==1){
				if (param==1){
					printf("Drawer touch exit\n");
					onpool=0;
				}
			}
			else if (id==6){
				if (strcmp(program,"Test1")==0){
					lart_application_start("Test3", "");
				}
			}
			else if (id==7){
				if (strcmp(program,"Test1")==0){
					lart_application_start("Test2", "");
				}
			}
		}
	} while(onpool && lart_application_is_run());
	libaroma_window_free(win);//, LIBAROMA_WINDOW_SHOW_ANIMATION_CIRCLE, 500);

	return 1;
}

byte setactive=1;

//systemui thread
void sysui_handler(){
	TLOG("SYSUI_HANDLER: STARTING");
	int newapp;
	newapp = lart_application_start("Test1","");
	TLOG("SYSUI_HANDLER: New app id: %d", newapp);
	while (lart_sysui_isactive()){
		//if (setactive>=4) {
			//break;
		//}
		libaroma_sleep(2000);
		//handle_notifications_and_exit_message

		lart_sysui_print_running_apps();
		//setactive++;
	}
}

void statusbar_ondraw(LIBAROMA_CANVASP cv,word fgcolor){
	//draw on cv using fgcolor as accent pls
	TLOG("statusbar w=%d h=%d", cv->w, cv->h);
	libaroma_draw_rect(cv, 0, 0, cv->w, cv->h, RGB(00aacc), 0xFF);
}

