#include "common.h"

void seltool_onclick(LIBAROMA_CONTROLP ctl){
	if (ctl->id==TOOL_COLOR1){
		sel_second=0;
		LIBAROMA_CONTROLP color2 = libaroma_window_getid(win, TOOL_COLOR2);
		return;
	}
	else if (ctl->id==TOOL_COLOR2){
		sel_second=1;
		LIBAROMA_CONTROLP color1 = libaroma_window_getid(win, TOOL_COLOR1);
		return;
	}
	LIBAROMA_CONTROLP oldtool = libaroma_window_getid(win, curtool);
	libaroma_ctl_button_style(oldtool, LIBAROMA_CTL_BUTTON_RAISED, 0);
	libaroma_ctl_button_style(ctl, LIBAROMA_CTL_BUTTON_FLAT, 0);
	curtool = ctl->id;
}

void color_onclick(LIBAROMA_CONTROLP ctl){
	word color = libaroma_rgb_to16(*(dwordp)ctl->tag);
	if (sel_second){
		LIBAROMA_CONTROLP color2 = libaroma_window_getid(win, TOOL_COLOR2);
		libaroma_ctl_button_style(color2, LIBAROMA_CTL_BUTTON_RAISED|LIBAROMA_CTL_BUTTON_COLORED, color);
		sec_cl = color;
	}
	else {
		LIBAROMA_CONTROLP color1 = libaroma_window_getid(win, TOOL_COLOR1);
		libaroma_ctl_button_style(color1, LIBAROMA_CTL_BUTTON_RAISED|LIBAROMA_CTL_BUTTON_COLORED, color);
		pri_cl = color;
	}
}

void alpha_slider_onupdate(LIBAROMA_CONTROLP ctl, int val){
	color_alpha = val;
	char alpha_str[16];
	snprintf(alpha_str, 16, "Opacity: %d", val);
	libaroma_ctl_label_set_text(alpha_label, alpha_str, 1);
}