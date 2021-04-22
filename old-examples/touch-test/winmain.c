void draw_cursor(int x, int y, int size, byte type);
void switch_cursor(int up);
#define CURSOR_ARROW 1
#define CURSOR_CIRCLE 2
#define CURSOR_RECT 3
LIBAROMA_WINDOWP win;
word bg_color;
LIBAROMA_CANVASP arrow_image=NULL;
byte curtype=0;

void winmain(){
	win = libaroma_window(
		NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
	libaroma_wm_set_active_window(win);
	int circlesz=libaroma_dp(32);
	int oldx=0;
	int oldy=0;
	curtype=CURSOR_ARROW;
	char *text=malloc(64);
	sprintf(text, "Touch test utility v0.1");
	
	bg_color=libaroma_colorget(win, NULL)->window_bg;
	//libaroma_canvas_blank(win->dc);
	do{
		LIBAROMA_MSG msg;
		dword command=libaroma_window_pool(win,&msg);
		byte cmd	= LIBAROMA_CMD(command);
		word id		= LIBAROMA_CMD_ID(command);
		byte param	= LIBAROMA_CMD_PARAM(command);
		switch (msg.msg){
			case LIBAROMA_MSG_KEY_POWER:
			case LIBAROMA_MSG_KEY_SELECT:
				win->onpool=0;
				break;
			case LIBAROMA_MSG_KEY_VOLUP:
				if (msg.state==0)
					switch_cursor(1);
				break;
			case LIBAROMA_MSG_KEY_VOLDOWN:
				if (msg.state==0)
					switch_cursor(0);
				break;
			case LIBAROMA_MSG_TOUCH:
			if (msg.state==LIBAROMA_HID_EV_STATE_DOWN || msg.state==LIBAROMA_HID_EV_STATE_MOVE){
				if (oldx && oldy){
					libaroma_draw_rect(win->dc, oldx-(curtype==CURSOR_CIRCLE?(circlesz/2):0), 
											oldy-(curtype==CURSOR_CIRCLE?(circlesz/2):0), 
											48, 48, bg_color, 0xFF);
				}
				sprintf(text, "Touch X: %d, Y: %d", msg.x, msg.y);
				update_title(text);
				draw_cursor(msg.x, msg.y, circlesz, curtype);
				//libaroma_ctl_label_set_text(label, text, 1);
				oldx=msg.x-(circlesz/2);
				oldy=msg.y-(circlesz/2);
				libaroma_window_sync(win, msg.x-(circlesz/2), msg.y-(circlesz/2), circlesz, circlesz);
			}
			else if (msg.state==LIBAROMA_HID_EV_STATE_UP){
				//libaroma_window_invalidate(win, 1);
				sprintf(text, "Last touch X: %d, Y: %d", msg.x, msg.y);
				update_title(text);
				//oldx=0;
				//oldy=0;
			}
		}
	}
	while (win->onpool);
	if (arrow_image) libaroma_canvas_free(arrow_image);
	libaroma_window_free(win);
}

void draw_cursor(int x, int y, int size, byte type){
	if (win==NULL) return;
	switch(type){
		case CURSOR_ARROW:
			if (arrow_image==NULL) 
				arrow_image=libaroma_image_uri("res:///img/cursor.png");
			libaroma_draw_scale_smooth(win->dc, arrow_image, x, y, size, size, 0, 0, 48, 48);
			//libaroma_draw(win->dc, arrow_image, x, y, 1);
			//libaroma_draw_line_width(win->dc, x, y, x, y+16, 1, RGB(FF0000), 0xFF, 0, 0);
			//libaroma_draw_line_width(win, dc, x+1, y+11, 1, RGB(FF0000), 0xFF, 0, 0);
			//libaroma_draw_line_width(win, dc, , 1, RGB(FF0000), 0xFF, 0, 0);
			break;
		case CURSOR_CIRCLE:
				libaroma_draw_circle(win->dc, RGB(ff0000), x, y, size, 0x88);
			break;
		case CURSOR_RECT:
				libaroma_draw_rect(win->dc, x-(size/2), y-(size/2), size, size, RGB(ff0000), 0x88);
			break;
	}
}

void switch_cursor(int up){
	if (up){
		curtype++;
		if (curtype>3) curtype=1;
	}
	else {
		curtype--;
		if (curtype<1) curtype=3;
	}
	switch (curtype){
		case 1:
			update_title("Switched to cursor");
			break;
		case 2:
			update_title("Switched to circle");
			break;
		case 3:
			update_title("Switched to rectangle");
			break;
	}
}

void update_title(char *text){
	libaroma_draw_rect(win->dc, 0, 0, libaroma_px(win->w), libaroma_dp(16), bg_color, 0xFF);
	libaroma_draw_text(win->dc, text, 0, 0, RGB(000000), libaroma_px(win->w), LIBAROMA_TEXT_CENTER, 0);
	libaroma_window_sync(win, 0, 0, libaroma_px(win->w), libaroma_dp(16));
}