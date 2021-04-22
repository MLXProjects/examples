#include "tools.h"

LIBAROMA_CONTROLP slider_btn;

#define settings_init(win) settings_init_ex(win, 0)
void settings_init_ex(LIBAROMA_WINDOWP win, int x){
	int cur_br=get_brightness();
	if (cur_br==-1) cur_br=255;
	slider_btn=libaroma_ctl_button(win, 2, (x+cur_br)-24, win->h/2-24, 48, 48, "",
		LIBAROMA_CTL_BUTTON_CIRCLE|LIBAROMA_CTL_BUTTON_RAISED|LIBAROMA_CTL_BUTTON_DISABLED|
		LIBAROMA_CTL_BUTTON_KEEP_COLOR|LIBAROMA_CTL_BUTTON_COLORED, RGB(AAAAAA));
}
