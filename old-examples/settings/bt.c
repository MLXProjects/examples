
char *cmd_out(char *args[], int max_buffer);

void powerdialog(LIBAROMA_WINDOWP parent);

void bt_settings(LIBAROMA_WINDOWP parent){
	#define ID_APPBAR			2
	#define ID_CTLLIST			3
	#define ID_CONTEXTMENU		4
	#define ID_TESTITEM			5
	/* menu flags */
	word menuflags =
		LIBAROMA_LISTITEM_WITH_SEPARATOR;
	
	LIBAROMA_WINDOWP win;
		
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
	
	printf("creating bar\n");
	LIBAROMA_CONTROLP bar = libaroma_ctl_bar(
		win, ID_APPBAR,
		0, ctlY, LIBAROMA_SIZE_FULL, ctlH,
		"Bluetooth", RGB(263238), RGB(ffffff)
	);
	ctlY += bar->h;	
	
	//libaroma_draw_rect(win->dc, 0, 0, win->w, STATUSBAR_HEIGHT, win->appbar_bg, 0xFF);
	
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
	
	//_ITEM(ID_TESTITEM, "Red 1", wifi_icon, "Deshabilitado");
	_TITLE(ID_TESTITEM, "Paired devices");

	LIBAROMA_CTL_BAR_TOOLSP bar_tools=libaroma_ctl_bar_tools(0);
	
	libaroma_ctl_bar_set_tools(
		bar,bar_tools, 0);
	
	libaroma_ctl_bar_set_icon(
		bar,NULL,0,LIBAROMA_CTL_BAR_ICON_ARROW,1);
	
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
		if (msg.msg==LIBAROMA_MSG_KEY_POWER){
			if (msg.state==0){
				//libaroma_png_save(libaroma_fb()->canvas,"/tmp/screenshot.png");
				printf("Power key pressed!\n");
				win->onpool = 0;
				break;
			}
		}
		
		printf("Window Command = (CMD: %i, ID: %i, Param: %i, Msg: %i, Key: %i, State: %i, X: %i, Y: %i)\n",
			LIBAROMA_CMD(command),
			LIBAROMA_CMD_ID(command),
			LIBAROMA_CMD_PARAM(command), 
			msg.msg, msg.key, msg.state, msg.x, msg.y);
		
		if (cmd == 1){
			if (id==ID_APPBAR){
				if (param==1){ //back icon pressed
				printf("Back icon pressed!\n");
				win->onpool=0;
				}
			}
			printf("ID: %i, MSG key: %i, PARAM: %i\n", id, msg.key, param);
			switch (msg.key){
				case ID_TESTITEM:
					
					break;
			}
		}
		
	}
	while(win->onpool);
	//window pool exited, so the window should close.
	
	if (!win->onpool){
		win->closed = 1;
		//msleep(2000);
		printf("Window closing!\n");
		statusbar_updater_stop(sb_thread);
		//msleep(2000);
	}
	/* undef menu item macro */
	#undef _DIV
	#undef _TITLE
	#undef _ITEM
	#undef _ITEMB
	
	libaroma_window_aniclose(win, parent, LIBAROMA_WINDOW_CLOSE_ANIMATION_PAGE_BOTTOM, 300);
}