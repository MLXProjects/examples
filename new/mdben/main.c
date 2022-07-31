#include <aroma.h>
#define BENCOLOR_GOOD RGB(7BBF2B)
#define BENCOLOR_BAD RGB(FF4343)
byte good=1;
byte clock=0;
word bencolor;

void *updater(void *inst){
	LIBAROMA_CONTROLP clk=(LIBAROMA_CONTROLP) inst;
	
}

byte ot_render(LIBAROMA_CONTROLP clk, LIBAROMA_CANVASP bg){
	word bencolor=good?BENCOLOR_GOOD:BENCOLOR_BAD;
	LIBAROMA_CANVASP cv=libaroma_canvas(bg->w, bg->w);
	libaroma_canvas_fillcolor(cv, bencolor);
	LIBAROMA_PATHP lpath=libaroma_path(0, 0);
	libaroma_path_add(lpath, (cv->w/2)*0.80, cv->h/2);
	libaroma_path_add(lpath, 0, cv->h);
	LIBAROMA_PATHP rpath=libaroma_path(cv->w, 0);
	libaroma_path_add(rpath, (cv->w/2)*1.20, cv->h/2);
	libaroma_path_add(rpath, cv->w, cv->h);
	libaroma_path_draw(cv, lpath, 0, 0xFF, NULL, 0.5);
	libaroma_path_draw(cv, rpath, 0, 0xFF, NULL, 0.5);
	if (!clock) libaroma_ctl_clock_setcolor(clk, LIBAROMA_CTL_CLOCK_COLOR_CENTER, bencolor);
	libaroma_draw_mask_circle(bg, cv, bg->w/2, bg->h/2, cv->w/2, cv->h/2, cv->w, 0xFF);
	return libaroma_ctl_clock_setbg(clk, bg);
}

byte ot_clktoggle(LIBAROMA_CONTROLP clk){
	word bencolor=good?BENCOLOR_BAD:BENCOLOR_GOOD;
	libaroma_ctl_clock_setcolor(clk, LIBAROMA_CTL_CLOCK_COLOR_CENTER, bencolor);
	libaroma_ctl_clock_setcolor(clk, LIBAROMA_CTL_CLOCK_COLOR_HOUR, bencolor);
	libaroma_ctl_clock_setcolor(clk, LIBAROMA_CTL_CLOCK_COLOR_MINUTE, bencolor);
	libaroma_ctl_clock_setcolor(clk, LIBAROMA_CTL_CLOCK_COLOR_SECOND, bencolor);
	libaroma_ctl_clock_setalpha(clk, clock?0xFF:0x01);
	if (!clock) libaroma_ctl_clock_setcolor(clk, LIBAROMA_CTL_CLOCK_COLOR_CENTER, good?BENCOLOR_GOOD:BENCOLOR_BAD);
}

int main(int argc, char **argv){
	libaroma_config()->runtime_monitor=LIBAROMA_START_MUTEPARENT;
	libaroma_start();
	libaroma_wm_cursor_setvisible(0);
	
	LIBAROMA_WINDOWP win=libaroma_window(0, 0, (libaroma_fb()->h-libaroma_fb()->w)/2, 0, libaroma_wm()->w);
	LIBAROMA_CANVASP bg=libaroma_canvas(libaroma_fb()->w, libaroma_fb()->w);
	
	LIBAROMA_CONTROLP clk=libaroma_ctl_clock(win, 0, 0, 0, 0, 0, bg, LIBAROMA_CTL_CLOCK_RIPPLE|LIBAROMA_CTL_CLOCK_NOBORDER);
	libaroma_ctl_clock_setalpha(clk, 0x01);
	ot_render(clk, bg);
	libaroma_window_anishow(win, 12, 400);
	LIBAROMA_MSG msg;
	byte cmd;
	while (1){
		cmd = LIBAROMA_CMD(libaroma_window_pool(win,&msg));
		if (msg.msg == LIBAROMA_MSG_KEY_POWER || msg.msg == LIBAROMA_MSG_KEY_SELECT) {
			break;
		}
		else if (cmd==LIBAROMA_CMD_CLICK){
			good=!good;
			ot_render(clk, bg);
			if (clock)
				ot_clktoggle(clk);
		}
		else if (cmd==LIBAROMA_CMD_HOLD){
			clock=!clock;
			ot_clktoggle(clk);
		}
	}
	//libaroma_sleep(15000);
	libaroma_window_aniclose(win, 12, 400);
	libaroma_end();
	
	return 0;
}
