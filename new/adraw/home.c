#include "common.h"

LIBAROMA_WINDOWP win = NULL;
LIBAROMA_CANVASP canvas = NULL;
LIBAROMA_CANVASP layer1 = NULL;
LIBAROMA_CANVASP layer2 = NULL;
LIBAROMA_CONTROLP image = NULL, alpha_label = NULL;;

word pri_cl=0, sec_cl=0;
byte sel_second=0;
enum tool curtool = TOOL_PEN;
int eraser_size = 8;
byte rect_type = TOOL_RECT_FILL;
byte circle_type = TOOL_CIRCLE_FILL;
byte color_alpha=255;

byte adraw_home_init(){
	canvas=libaroma_canvas(win->w-libaroma_dp(76), win->h-libaroma_dp(76));
	if (!canvas){
		loge("canvas create failed");
		return 0;
	}
	logi("canvas create done (%dx%d)", canvas->w, canvas->h);
	libaroma_canvas_setcolor(canvas, RGB(FFFFFF), 0xFF);
	/* default colors */
	logi("set default colors");
	pri_cl=DEFAULT_PRICOLOR;
	sec_cl=DEFAULT_SECCOLOR;
	
	/* create buttons */
	int i=0;
	int x=0;
	int y=0;
	enum tool tool_id;
	byte btnstyle=LIBAROMA_CTL_BUTTON_RAISED;
	for (tool_id=TOOL_PEN; tool_id<TOOL_COLOR1; tool_id++){
		if (i>7) btnstyle=LIBAROMA_CTL_BUTTON_RAISED|LIBAROMA_CTL_BUTTON_COLORED;
		LIBAROMA_CONTROLP toolbtn = libaroma_ctl_button(win, tool_id, (i%2==0)?0:38, y, 38, 38,NULL, LIBAROMA_CTL_BUTTON_RAISED, 0);
		if (!toolbtn){
			loge("toolbtn #%i create failed", tool_id);
			return 0;
		}
		libaroma_control_set_onclick(toolbtn, &seltool_onclick);
		if (i%2!=0) y+=38;
		i++;
	}
	for (;tool_id<TOOL_COLOR2+1; tool_id++){
		LIBAROMA_CONTROLP colorbtn = libaroma_ctl_button(win, tool_id, 0, y, 76, 38, NULL, LIBAROMA_CTL_BUTTON_RAISED|LIBAROMA_CTL_BUTTON_COLORED, (tool_id==TOOL_COLOR1)?pri_cl:sec_cl);
		if (!colorbtn){
			loge("colorbtn #%i create failed", tool_id);
			return 0;
		}
		libaroma_control_set_onclick(colorbtn, &seltool_onclick);
		y+=38;
	}
	image = libaroma_ctl_image_canvas_ex(win, 0, canvas, 76, 0, libaroma_px(canvas->w), libaroma_px(canvas->h), LIBAROMA_CTL_IMAGE_SHARED);
	if (!image){
		loge("image create failed");
		return 0;
	}
	y=(win->height-76);
	for (i=0; i<28; i++){
		if (x>(win->width-38)){
			if (i==13){
				x=0;
				y+=38;
			}
			continue;
		}
		LIBAROMA_CONTROLP btn = libaroma_ctl_button(win, 0, x, y, 38, 38, NULL, LIBAROMA_CTL_BUTTON_RAISED|LIBAROMA_CTL_BUTTON_COLORED, libaroma_rgb_to16(colors[i]));
		if (!btn){
			loge("button #%d create failed", i+1);
			return 0;
		}
		x+=38;
		btn->tag=(voidp) &colors[i];
		libaroma_control_set_onclick(btn, &color_onclick);
		if (i==13){
			y+=38;
			x=0;
		}
	}
	alpha_label = libaroma_ctl_label(win, 0, "Opacity: 255", win->width-96, win->height-60, 196, 20, RGB(0), 3, LIBAROMA_TEXT_CENTER|LIBAROMA_TEXT_SINGLELINE, 100);
	LIBAROMA_CONTROLP alpha_slider = libaroma_ctl_slider(win, 0, win->width-96, win->height-40, 96, 40, 255, 255);
	libaroma_ctl_slider_set_onchange(alpha_slider, &alpha_slider_onupdate);
	return 1;
}

void adraw_home(){
	win = libaroma_window(NULL, 0, 0, 0, 0);
	if (!win) {
		loge("window create failed");
		return;
	}
	logi("window create done (%dx%d)", win->width, win->height);
	if (!adraw_home_init(win)){
		loge("window init failed");
		goto exit;
	}
	logi("window init done");
	layer1 = libaroma_canvas_dup(canvas);
	if (!layer1){
		loge("layer1 create failed");
		goto exit;
	}
	layer2 = libaroma_canvas_dup(canvas);
	if (!layer1){
		loge("layer2 create failed");
		goto exit;
	}
	logi("layers init done");
	
	/* show window */
	libaroma_window_anishow(win, LIBAROMA_WINDOW_SHOW_ANIMATION_FADE, 300);
	byte i;
	for (i=TOOL_PEN; i<TOOL_COLOR1; i++){
		LIBAROMA_CONTROLP toolbtn = libaroma_window_getid(win, i);
		if (toolbtn->id==TOOL_PEN) libaroma_ctl_button_style(toolbtn, LIBAROMA_CTL_BUTTON_FLAT, 0);
		libaroma_ctl_button_seticon(toolbtn, adraw_toolicon(i), 0);
	}
	LIBAROMA_MSG msg;
	byte isdrawing=0, selecting=0;
	int firstx, firsty, lastx, lasty, drawx, drawy, draww, drawh;
	byte invert_drawx, invert_drawy;
	do {
		libaroma_window_pool(win, &msg);
		if (msg.state==0){
			if (msg.msg==LIBAROMA_MSG_EXIT || msg.msg==LIBAROMA_MSG_KEY_POWER){
				break;
			}
		}
		if (msg.msg==LIBAROMA_MSG_TOUCH || msg.msg==LIBAROMA_MSG_MOUSE){
			/* check for event inside of canvas bounds */
			if ((isdrawing || msg.state==1) && 
				(msg.x>=image->x && msg.x<=(image->x+image->w) && msg.y>=image->y && msg.y<=(image->y+image->h))){
				msg.x-=image->x;
				msg.y-=image->y;
				//logi("cur color = %s", (msg.key==LIBAROMA_MSG_KEY_RMOUSE)?(sel_second?"primary - right click":"secondary - right click"):(sel_second?"secondary - left click":"primary - left click"));
				word curcolor = (msg.key==LIBAROMA_MSG_KEY_RMOUSE)?(sel_second?pri_cl:sec_cl):(sel_second?sec_cl:pri_cl);
				if (!isdrawing) isdrawing=1;
				switch (curtool){
					case TOOL_PEN:{
						if (msg.state==1){
							lastx=msg.x;
							lasty=msg.y;
						}
						libaroma_draw_line(layer1, lastx, lasty, msg.x, msg.y, 1.0, curcolor, color_alpha, 0);
						libaroma_draw(canvas, layer1, 0, 0, 0);
						libaroma_ctl_image_update(image);
						lastx=msg.x;
						lasty=msg.y;
					} break;
					case TOOL_PAINT:{
						if (msg.state!=1) continue;
						adraw_paint(msg.x, msg.y, curcolor);
						adraw_cv_update(0, 0, canvas->w, canvas->h, 0);
					} break;
					case TOOL_ERASER:{
						drawx=(msg.x-(eraser_size/2));
						drawy=(msg.y-(eraser_size<<2));
						draww=drawh=eraser_size;
						libaroma_draw_rect(layer2, drawx, drawy, draww, drawh, (curcolor==sec_cl)?pri_cl:sec_cl, 0xFF);
						adraw_cv_update(drawx, drawy, draww, drawh, (msg.state==0)?1:0);
					} break;
					case TOOL_CIRCLE:
					case TOOL_RECT:{
						if (msg.state==1){
							firstx=msg.x;
							firsty=msg.y;
						}
						else {
							if (msg.x<firstx){
								drawx=msg.x;
								draww=firstx-drawx;
							}
							else {
								drawx=firstx;
								draww=msg.x-firstx;
							}
							if (msg.y<firsty){
								drawy=msg.y;
								drawh=firsty-drawy;
							}
							else {
								drawy=firsty;
								drawh=msg.y-firsty;
							}
							if (curtool==TOOL_CIRCLE){
								libaroma_draw_ex(layer2, layer1, drawx, drawy, drawx, drawy, draww, drawh, 0, 0xFF);
								libaroma_draw_circle(layer2, curcolor, drawx+(draww/2), drawy+(drawh/2), (draww>drawh)?draww:drawh, color_alpha);
							}
							else {
								if (rect_type==TOOL_RECT_FILL){
									libaroma_draw_ex(layer2, layer1, drawx, drawy, drawx, drawy, draww, drawh, 0, 0xFF);
									libaroma_draw_rect(layer2, drawx, drawy, draww, drawh, curcolor, color_alpha);
								}
								else {
									libaroma_draw_ex(layer2, layer1, drawx+1, drawy+1, drawx+1, drawy+1, draww-2, drawh-2, 0, 0xFF);
									libaroma_draw_line(layer2, drawx, drawy, drawx+draww, drawy, 1, curcolor, color_alpha, 0);
									libaroma_draw_line(layer2, drawx, drawy, drawx, drawy+drawh, 1, curcolor, color_alpha, 0);
									libaroma_draw_line(layer2, (drawx+draww)-1, drawy, (drawx+draww)-1, drawy+drawh, 2, curcolor, color_alpha, 0);
									libaroma_draw_line(layer2, drawx, (drawy+drawh)-1, drawx+draww, (drawy+drawh)-1, 1, curcolor, color_alpha, 0);
								}
							}
							adraw_cv_update(drawx, drawy, draww, drawh, (msg.state==0)?1:0);
						}
					} break;
				}
				if (msg.state==0) isdrawing=0;
			}
			else if (isdrawing){
				if (msg.state==0) isdrawing=0;
				else {
					msg.x-=image->x;
					msg.y-=image->y;
					lastx=msg.x;
					lasty=msg.y;
				}
			}
		}
	} while (win->onpool);
	libaroma_png_save(canvas, "./canvas.png");
	libaroma_png_save(layer1, "./layer1.png");
	libaroma_png_save(layer2, "./layer2.png");
exit:
	if (layer2) libaroma_canvas_free(layer2);
	if (layer1) libaroma_canvas_free(layer1);
	if (canvas) libaroma_canvas_free(canvas);
	libaroma_window_aniclose(win, LIBAROMA_WINDOW_SHOW_ANIMATION_FADE, 300);
}