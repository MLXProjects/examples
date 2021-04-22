byte screen_islocked=0;

void show_lockscreen(LIBAROMA_WINDOWP parent){
	LIBAROMA_WINDOWP win = libaroma_window(
		NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
	LIBAROMA_CANVASP bg=libaroma_canvas(win->w, win->h);
	libaroma_draw_rect(bg, 0, 0, bg->w, bg->h, RGB(000000), 0xFF);
	LIBAROMA_CONTROLP imgctl = libaroma_ctl_image_canvas(win, 1, bg, 0, 0, win->w, win->h);
	animate_window_transition(parent, win, "fade", NULL, 0);
	do{
		LIBAROMA_MSG msg;
		dword command=libaroma_window_pool(win,&msg);
		byte cmd	= LIBAROMA_CMD(command);
		word id		= LIBAROMA_CMD_ID(command);
		byte param	= LIBAROMA_CMD_PARAM(command);
		
		switch (msg.msg){
			case LIBAROMA_MSG_KEY_POWER:
			case LIBAROMA_MSG_KEY_SELECT:
				{
					if (msg.state==0){
						if (debug) printf("Close key pressed!\n");
						win->onpool=0;
					}
				}
				break;
		}
		
	}
	while(win->onpool);
	libaroma_canvas_free(bg);
	animate_window_transition(win, parent, "fade", NULL, 0);
}

