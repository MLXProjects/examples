LIBAROMA_CANVASP cv_resize(LIBAROMA_CANVASP cv, int w, int h, byte stretch){
	LIBAROMA_CANVASP res=libaroma_canvas(w, h);
	if (stretch)
		libaroma_draw_scale_smooth(res, cv, 0, 0, res->w, res->h, 0, 0, cv->w, cv->h);
	else libaroma_draw_ex(res, cv, 0, 0, 0, 0, res->w, res->h, 0, 0xFF);
	return res;
}


void template_page(LIBAROMA_WINDOWP parent){

	LIBAROMA_WINDOWP win = libaroma_window(
		NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);//30, 40, 180, 240);
	
	LIBAROMA_CONTROLP prog=libaroma_ctl_progress(win, 1, 16, 16, 64, 64, 
								LIBAROMA_CTL_PROGRESS_INDETERMINATE|LIBAROMA_CTL_PROGRESS_CIRCULAR, 3, 1);
	
	//navbar_draw(win);
	
	//printf("Showing window!\n");
	//statusbar_end_thread(); /* stop sb thread to play window open/close animations */
	//libaroma_window_anishow(win, LIBAROMA_WINDOW_SHOW_ANIMATION_PAGE_TOP, 0);
	libaroma_wm_set_active_window(win);
	//statusbar_start_thread(); /* start it again so it actually works */
	//printf("Window should be visible.\n");
	/*
						printf("Select key pressed!\n");
						LIBAROMA_CANVASP cv=libaroma_canvas_alpha(64, 64);//libaroma_image_uri("res:///icons/alpha.png");
						//libaroma_draw_scale_smooth(cv, libaroma_image_uri("res:///icons/menu.png"), 0, 0, 16, 16, 0, 0, 64, 64);
						//memset(cv->alpha, 0, cv->s);
						libaroma_draw_rect(cv, 16, 16, 32, 32, RGB(FF0000), 0xFF);
						//libaroma_canvas_fillalpha(cv, 0, 0, 64, 64, 0xFF);
						//libaroma_draw_alphapixel(cv, 1, 1, 0x00);
						//cv->alpha[1]=0x11;
						//libaroma_png_save(cv,"/tmp/test.png");
						libaroma_draw(win->dc, cv, 10, 0, 1);
						printf("w=%d, h=%d\n", cv->w, cv->h);*/
	int asd=1;
	do{				
		LIBAROMA_MSG msg;
		dword command=libaroma_window_pool(win,&msg);
		byte cmd	= LIBAROMA_CMD(command);
		word id		= LIBAROMA_CMD_ID(command);
		byte param	= LIBAROMA_CMD_PARAM(command);
		
		switch (msg.msg){
			case LIBAROMA_MSG_KEY_POWER:
					if (msg.state==0){
						printf("Power key pressed!\n");
						win->onpool=0;
						//libaroma_png_save(win->dc,"/tmp/dc.png");
						//libaroma_draw(libaroma_fb()->canvas, win->dc, prog->x, prog->y, 1);
						//win->onpool=0;
					}
					break;
			case LIBAROMA_MSG_KEY_SELECT:
				{
					if (msg.state==0){
						printf("Select key pressed!\n");
						if (!asd) {
							printf("\n\nRestoring window!\n\n");
							libaroma_window_resize(win, 30, 40, 180, 240);
							asd++;
						}
						else {
							printf("\n\nMaximizing window!\n\n");
							libaroma_window_resize(win, 0, 0, 240, 320);
							asd--;
						}
					}
				}
				break;
		}
		
		if (cmd == 1){ //window received click event, handle it
			
			if (id==ID_NAVBACK){
				//libaroma_window_invalidate(win, 1);
				//libaroma_canvas_blank(libaroma_fb()->canvas, 0xFF);
				libaroma_window_resize(win, 60, 80, 120, 160);
			} 
			if (id==ID_NAVHOME){/*
				printf("old w=%d, h=%d\n", cv->w, cv->h);
				LIBAROMA_CANVASP tempcv=NULL;
				printf("Duplicating old canvas\n");
				if (1) tempcv=libaroma_canvas_dup(cv);
				printf("Resizing\n");
				cv->w=cv->h=cv->l=64;
				cv->s=64*2;
				printf("Allocating data size\n");
				free(cv->data);
				cv->data=(wordp) calloc(cv->s,2);
				printf("w=%d, h=%d, tw=%d, th=%d\n", cv->w, cv->h, tempcv->w, tempcv->h);
				printf("Drawing old into new canvas\n");
				//libaroma_draw_scale_smooth(cv, tempcv, 0, 0, cv->w, cv->h, 0, 0, tempcv->w, tempcv->h);
				printf("Drawing into window\n");
				libaroma_canvas_free_ex1(&tempcv, 0);
				libaroma_draw(win->dc, cv, 0, 0, 0);*/
				//libaroma_window_composer_resize(win, 0, 0, 240, 320);
				printf("Window manager properties: \n");
				printf("Window count: %d\n\n", libaroma_wm()->windows->n);
				printf("Window properties: \n");
				printf("Alpha: %d\n", win->alpha);
				printf("WxH: %dx%d\t\tX/Y: %d, %d\n\n", win->w, win->h, win->x, win->y);
				printf("Composer information for this window: \n");
				printf("WxH: %dx%d\t\tX/Y: %d, %d\n\n", win->draww, win->drawh, win->drawx, win->drawy);
				//printf("\n");
			} 
			if (id==ID_NAVMENU){
				/*
				if (asd>3) asd=0;
				else asd++;
				libaroma_ctl_progress_value(prog, asd);
				*/
				//libaroma_canvas_fillalpha(win->dc, 0, 0, win->w, win->h, 0xFF);		
				//libaroma_draw(libaroma_fb()->canvas, win->dc, win->x, win->y, 1);
				//libaroma_wm_syncarea();
				//printf("asd: %d\n", asd);
				libaroma_canvas_blank(libaroma_fb()->canvas, 0xFF);
			} 
		}
		
	}
	while(win->onpool);
	libaroma_ctl_progress_type(prog, LIBAROMA_CTL_PROGRESS_DETERMINATE);
	printf("Closing window!\n");
	//statusbar_end_thread(); /* needed in order to play window animations */
	libaroma_window_free(win);
	//statusbar_start_thread();/* start it again so the statusbar animation plays */
}
