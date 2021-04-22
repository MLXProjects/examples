#include "tools.h"

void winmain(){
	LIBAROMA_WINDOWP win=libaroma_window(NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
	win->onupdatebg=bg_update;
	if (win->bg) {
		libaroma_canvas_free(win->bg);
		win->bg=NULL;
	}
	win->bg=libaroma_canvas(win->w, win->h);
	bg_update(win, win->bg);
	LIBAROMA_CONTROLP btn=libaroma_ctl_button(win, 1, 0, 0, 96, 48, "Button", NULL, NULL);
	ALOG("decorate is %d", win->flags&LIBAROMA_WINDOW_DECORATE);

	libaroma_window_anishow(win, LIBAROMA_WINDOW_SHOW_ANIMATION_CIRCLE, 500);
	byte onpool=1;
	do{
		LIBAROMA_MSG msg;
		dword command=libaroma_window_pool(win,&msg);
		byte cmd	= LIBAROMA_CMD(command);
		word id	 = LIBAROMA_CMD_ID(command);
		byte param = LIBAROMA_CMD_PARAM(command);

		if (msg.msg == LIBAROMA_MSG_EXIT ||
			msg.msg == LIBAROMA_MSG_KEY_SELECT ||
			msg.msg == LIBAROMA_MSG_KEY_POWER){
			if (msg.state==0){
				onpool=0;
			}
		}
		else if (cmd==LIBAROMA_CMD_HOLD){
			if (id==1){
				ALOG("Button holded");
			}
		}
		else if (cmd==LIBAROMA_CMD_CLICK){
			if (id==1){
				ALOG("Button pressed");
				onpool=0;
			}
		}
		//ALOG("Received event with id %i, msg %i, cmd %i, param %i, state %i, key %i, x %i and y %i",
		//		id, msg.msg, cmd, param, msg.state, msg.key, msg.x, msg.y);
	} while (onpool);

	libaroma_window_aniclose(win, LIBAROMA_WINDOW_SHOW_ANIMATION_CIRCLE, 500);
}

void bg_update(LIBAROMA_WINDOWP win, LIBAROMA_CANVASP cv){
	LIBAROMA_CANVASP bg=libaroma_image_uri("res:///bg.png");
	//LIBAROMA_CANVASP bg2=libaroma_canvas(cv->w, cv->h);
	libaroma_draw_scale_nearest(cv, bg, 0, 0, cv->w, cv->h, 0, 0, bg->w, bg->h);
	libaroma_canvas_free(bg);
}
