#include "tools.h"

static LIBAROMA_THREAD mkwin;
void mkwin_thread(void *parent){
	winmain();
}

void winmain(){
	mkwin=NULL;
	LIBAROMA_WINDOWP win=libaroma_window_ex(NULL, win_x, win_y, LIBAROMA_SIZE_HALF, LIBAROMA_SIZE_HALF, LIBAROMA_WINDOW_DECORATE);
	char *title=malloc(32);
	sprintf(title, "Window #%d", libaroma_wm()->windown);
	libaroma_window_settitle(win, title);
	free(title);
	win->onupdatebg=bg_update;
	if (win->bg) {
		libaroma_canvas_free(win->bg);
		win->bg=NULL;
	}
	win->bg=libaroma_canvas(win->w, win->h);
	bg_update(win, win->bg);
	win_x=win->x+win->w+20;
	//win_y=40;
	LIBAROMA_CONTROLP btn=libaroma_ctl_button(win, 1, 0, 0, 96, 48, "Button", NULL, NULL);

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
				//libaroma_png_save(libaroma_fb()->canvas, "/tmp/clock.png");
				onpool=0;
			}
		}
		else if (cmd==LIBAROMA_CMD_HOLD){
			if (id==1){
				ALOG("Button holded");
				//winmain();
				libaroma_thread_create(&mkwin, mkwin_thread, (voidp)win);
				ALOG("Hi again :D");
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
	/*byte skew=1;
		//libaroma_draw(cv, bg2, 0, 0, 0);
	if (skew){
		libaroma_draw_skew(cv, bg2,
							0,		//x1
							0,		//y1
							cv->w,	//x2
							0,		//y2
							(cv->w
								/3),//x3
							cv->h,	//y3
							((cv->w
							 /3)*2),//x4
							cv->h	//y4
		);
	}
	else {
		libaroma_draw_skew(cv, bg2,
							0,		//x1
							0,		//y1
							cv->w,	//x2
							0,		//y2
							0,		//x3
							cv->h,	//y3
							cv->w,	//x4
							cv->h	//y4
		);
	}
	libaroma_canvas_free(bg2);*/
}

byte libaroma_draw_skew(
	LIBAROMA_CANVASP dst, LIBAROMA_CANVASP src,
	int x1, int y1, int x2, int y2,
	int x3, int y3, int x4, int y4
){
	int topw, botw, topx, botx;
	topw=x2-x1;
	topx=(dst->w-topw)/2;
	botw=x4-x3;
	botx=(dst->w-botw)/2;
	ALOG("Skewing with new top x=%d, w=%d and bot x=%d, w=%d", topx, topw, botx, botw);

	int curw=topw;

	int y, x;
	for (y=0; y<dst->h; y++){
		ALOG("Processing line %d", y);
		wordp cur_line=dst->data+(dst->l*y);
		wordp src_data=src->data+(src->l*y);
		float ratio=(((float)dst->h-y)/(float)dst->h);
		curw=botw+((topw-botw)*ratio);
		ALOG("Line width is %d (ratio=%1.2f)", curw, ratio);
		int xoff=(dst->w-curw)/2;
		for (x=0; x<curw; x++){
			ALOG("Processing x %d", x);
			*cur_line=src_data;
			*cur_line++;
		}
	}
}
