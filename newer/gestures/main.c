#include <aroma.h>
#define loge(...) {printf("E/APP(): "); printf(__VA_ARGS__); printf("\n");}
#define logi(...) {printf("I/APP(): "); printf(__VA_ARGS__); printf("\n");}
#define logd(...) {printf("D/APP(): "); printf(__VA_ARGS__); printf("\n");}

#define GESTURE_NONE	0
#define GESTURE_UP		1
#define GESTURE_DOWN	2
#define GESTURE_LEFT	3
#define GESTURE_RIGHT	4
typedef struct {
	/* animation coords */
	LIBAROMA_RECT min_rect;
	int curx;
	int cury;
	int curw;
	int curh;
	/* actual coords */
	int x;
	int y;
	int w;
	int h;
	/* misc data */
	LIBAROMA_WINDOWP win;
	LIBAROMA_CANVASP old;
	LIBAROMA_CANVASP cv;
	byte visible;
	byte draw;
} MLX_WIN, * MLX_WINP;
#define MIN_LWINX	libaroma_dp(8)
#define MIN_RWINX	((libaroma_fb()->w/2)+libaroma_dp(8))
#define MIN_WINY	libaroma_dp(8)
#define MIN_WINW	((libaroma_fb()->w/2)-libaroma_dp(16))
#define MIN_WINH	((libaroma_fb()->h/2)-libaroma_dp(16))
#define MAX_WINW	(libaroma_fb()->w)
#define MAX_WINH	(libaroma_fb()->h)
#define WIN_THIRDW	(libaroma_fb()->w/3)

LIBAROMA_WINDOWP win;
LIBAROMA_CANVASP wmc;
float gest_state;
byte ongesture, gest_type, win_exist;
long touchdown_tick, ani_tick;
MLX_WINP mywin;

void mywin_draw_frame();
byte mywin_uithread();
byte mywin_msghandler(LIBAROMA_WMP wm, LIBAROMA_MSGP msg);
byte mywin_sync(LIBAROMA_WINDOWP win,
	int x,int y,int w,int h);

static LIBAROMA_WINDOW_HANDLER mywin_handler={
	prefree:NULL,
	postfree:NULL,
	updatebg:NULL,
	invalidate:NULL,
	sync:mywin_sync,
	message_hooker:NULL,
	control_draw_flush:NULL,
	control_erasebg:NULL,
	control_isvisible:NULL,
	control_draw_begin:NULL
};

void winmain(){
	mywin=NULL;
	wmc=libaroma_fb()->canvas;
	libaroma_wm_set_message_handler(&mywin_msghandler);
	libaroma_wm_set_ui_thread(&mywin_uithread);
	libaroma_wm_client_start();
	mywin=calloc(sizeof(MLX_WIN), 1);
	if (!mywin){
		loge("mywin alloc failed");
		return;
	}
	mywin->cv=libaroma_canvas(MAX_WINW, MAX_WINH);
	if (!(mywin->cv)){
		loge("mywin->cv failed");
		free(mywin);
		return;
	}
	mywin->min_rect.x=mywin->curx=MIN_LWINX;
	mywin->x=0;
	mywin->min_rect.y=mywin->cury=MIN_WINY;
	mywin->y=0;
	mywin->min_rect.w=mywin->curw=MIN_WINW;
	mywin->w=MAX_WINW;
	mywin->min_rect.h=mywin->curh=MIN_WINH;
	mywin->h=MAX_WINH;
	gest_state=0.0f;
	win=libaroma_window(0, 0, 0, 0, 0);
	if (!win){
		loge("win init fail");
		libaroma_canvas_free(mywin->cv);
		free(mywin);
		return;
	}
	win->handler=&mywin_handler;
	libaroma_ctl_progress(win, 0, (win->width/2)-24, (win->height/2)-24, 48, 48, LIBAROMA_CTL_PROGRESS_CIRCULAR|LIBAROMA_CTL_PROGRESS_INDETERMINATE, 4, 1);
	mywin->win=win;
	//win->lock_sync=1; //don't sync fb content at win update
	libaroma_wm_set_active_window(win); //draw to wm canvas
	//swap canvases
	mywin->old=win->dc;
	libaroma_draw(mywin->cv, mywin->old, 0, 0, 0);
	win->dc=mywin->cv;
	libaroma_window_invalidate(win, 10);
	win_exist=1;
	logd("mywin done");
	mywin->visible=0;
	//mywin_draw_frame();
	//mywin->draw=1;
	gest_type=0;
	LIBAROMA_MSG msg;
	while (win->onpool){
		libaroma_window_pool(win, &msg);
		if (msg.msg==LIBAROMA_MSG_EXIT){
			logd("msg exit from pool");
			break;
		}
	}
	//swap again
	win->dc=mywin->old;
	libaroma_wm_set_active_window(NULL); //unset active win
	libaroma_window_free(win);
	if (mywin){
		if (mywin->cv) libaroma_canvas_free(mywin->cv);
		free(mywin);
	}
}

byte mywin_sync(LIBAROMA_WINDOWP win, int x,int y,int w,int h){
	logd("mywin_sync");
	libaroma_canvas_setcolor(wmc, 0, 0xFF);
	mywin->curx=mywin->x+((libaroma_fb()->w/2)*(1.0f-gest_state));
	mywin->cury=mywin->y+((libaroma_fb()->h/2)*(1.0f-gest_state));
	mywin->curw=mywin->w-(mywin->w*(1.0f-gest_state));
	mywin->curh=mywin->h-(mywin->h*(1.0f-gest_state));
	libaroma_draw_scale(wmc, mywin->cv, mywin->curx, mywin->cury, mywin->curw, mywin->curh, 0, 0, mywin->win->w, mywin->win->h, 0);
	libaroma_fb_sync();
	return 1;
}

byte mywin_msghandler(LIBAROMA_WMP wm, LIBAROMA_MSGP msg){
	switch (msg->msg){
		case LIBAROMA_MSG_EXIT:
		case LIBAROMA_MSG_KEY_POWER:
		case LIBAROMA_MSG_KEY_SELECT:{
			if (msg->state==0){ //only handle key release message
				if (!mywin->visible){ //maximize
				logd("win hidden");
					mywin->draw=1;
				}
				else {
					if (win_exist==1){ //minimize
					logd("win visible");
						win_exist=2;
						mywin->draw=1;
					}
					else {
						logd("msg exit from handler");
						libaroma_wm_client_stop();
						win->onpool=0;
						return 0;
					}
				}
			}
		} break;
		case LIBAROMA_MSG_TOUCH:{
			//logd("touch %s x, y (%d, %d)", (msg->state==0)?"up":(msg->state==1)?"down":"move", msg->x, msg->y);
			switch (msg->state){
				case LIBAROMA_HID_EV_STATE_DOWN:{
					touchdown_tick=libaroma_tick();
				} break;
				case LIBAROMA_HID_EV_STATE_MOVE:{
					if (!gest_type){
						logd("check for gesture init");
						//if (libaroma_tick()-touchdown_tick<200){
							//touch at x center
							if (msg->x>WIN_THIRDW && msg->x<(WIN_THIRDW*2)){
								if (msg->y>=(MAX_WINH-libaroma_dp(16))){
									ongesture=1;
									gest_type=GESTURE_UP;
									gest_state=0.0f;
									logd("up gesture init");
								}
							}
						//}
					}
					else if (gest_type==GESTURE_UP){
						gest_state=((float)msg->y/(float)MAX_WINH);
						libaroma_canvas_setcolor(wmc, 0, 0xFF);
						mywin->curx=mywin->x+((libaroma_fb()->w/2)*(1.0f-gest_state));
						mywin->cury=mywin->y+((libaroma_fb()->h/2)*(1.0f-gest_state));
						mywin->curw=mywin->w-(mywin->w*(1.0f-gest_state));
						mywin->curh=mywin->h-(mywin->h*(1.0f-gest_state));
						logd("up gesture state=%1.2f (%d, %d)", gest_state, mywin->curx, mywin->cury);
						libaroma_draw_scale(wmc, mywin->cv, mywin->curx, mywin->cury, mywin->curw, mywin->curh, 0, 0, mywin->win->w, mywin->win->h, 0);
						libaroma_fb_sync();
					}
				} break;
				case LIBAROMA_HID_EV_STATE_UP:{
					switch (gest_type){
						case GESTURE_UP:{
							logd("up gesture release");
						} break;
					}
					mywin->draw=1;
				} break;
			}
		} break;
	}
	return 1;
}

byte mywin_uithread(){
	if (mywin){
		if (mywin->draw){
			logd("ui_thread animating");
			if (!ani_tick) {
				ani_tick=libaroma_tick();
			}
			gest_state=libaroma_motion_fluid(libaroma_duration_state(ani_tick, 500));
			if (mywin->visible){
				gest_state=1.0f-gest_state;
			}
			mywin_draw_frame();
			if ((!(mywin->visible) && gest_state>=1.0f) || (mywin->visible && gest_state<=0.0f)){
				ani_tick=0;
				mywin->draw=0;
				mywin->visible=!(mywin->visible);
			}
		}
	}
	return 0;
}

void mywin_draw_frame(){
	libaroma_canvas_setcolor(wmc, 0, 0xFF);
	int x, y, w, h;
	if (mywin->visible){//hiding
		x=mywin->curx+((mywin->min_rect.x-mywin->curx)*(1.0f-gest_state));
		y=mywin->cury+((mywin->min_rect.y-mywin->cury)*(1.0f-gest_state));
		w=mywin->curw+((mywin->min_rect.w-mywin->curw)*(1.0f-gest_state));
		h=mywin->curh+((mywin->min_rect.h-mywin->curh)*(1.0f-gest_state));
	}
	else {//showing
		mywin->curx=x=mywin->x+(mywin->min_rect.x*(1.0f-gest_state));
		mywin->cury=y=mywin->y+(mywin->min_rect.y*(1.0f-gest_state));
		mywin->curw=w=mywin->w-((mywin->w-mywin->min_rect.w)*(1.0f-gest_state));
		mywin->curh=h=mywin->h-((mywin->h-mywin->min_rect.h)*(1.0f-gest_state));
		
	}
	logd("%s at %d, %d (%dx%d)", (mywin->visible)?"hiding":"showing", x, y, w, h);
	//libaroma_draw_rect(wmc, x, y, w, h, RGB(FF0000), 0xFF);
	libaroma_draw_scale(wmc, mywin->cv, x, y, w, h, 0, 0, mywin->win->w, mywin->win->h, 0);
	libaroma_fb_sync();
}

int main(int argc, char **argv){
	if (!libaroma_start()){
		loge("libaroma start failed");
		return 1;
	}
	winmain();
	libaroma_end();
	return 0;
}
