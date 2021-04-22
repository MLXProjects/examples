#include "tools.h"

void winmain(){
	srand(time(0));
	//LIBAROMA_COLORSETP colorset=calloc(sizeof(LIBAROMA_COLORSETP), 1);
	//colorset->window_bg=RGB(222222);
	LIBAROMA_WINDOWP win=libaroma_window(NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
	//win->colorset=colorset;
	LIBAROMA_CONTROLP pager=libaroma_ctl_pager(win, 1, 3, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
	LIBAROMA_WINDOWP pager_win=libaroma_ctl_pager_get_window(pager);
	clock_init(pager_win);
	settings_init_ex(pager_win, pager->w);
	//libaroma_ctl_pager_set_active_page(pager, 1, 0);
	libaroma_window_anishow(win, LIBAROMA_WINDOW_SHOW_ANIMATION_CIRCLE, 500);
	byte onpool = 1;
	byte line_n=0;
	byte dragging=0;
	do{
		LIBAROMA_MSG msg;
		dword command=libaroma_window_pool(win,&msg);
		byte cmd	= LIBAROMA_CMD(command);
		word id	 = LIBAROMA_CMD_ID(command);
		byte param = LIBAROMA_CMD_PARAM(command);

		switch (msg.msg){
			case LIBAROMA_MSG_KEY_SELECT:
			case LIBAROMA_MSG_KEY_POWER:{
					if (msg.state==0){
						libaroma_png_save(libaroma_fb()->canvas, "/tmp/clock.png");
						onpool=0;
					}
				}
				break;
			case LIBAROMA_MSG_TOUCH:{
					switch (msg.state){
						case LIBAROMA_HID_EV_STATE_DOWN:
							if (insidecontrol(slider_btn, msg.x, msg.y, pager->w))
								dragging = 1;
							break;
						case LIBAROMA_HID_EV_STATE_MOVE:
							if (dragging){
								int x=msg.x - (slider_btn->w/2);
								if (x<(pager->w)+26) x=(pager->w)+26;
								else if (x>((pager->w*2)-26-(slider_btn->w)))
									x=((pager->w*2)-26-slider_btn->w);
								printf("btn moved to x %d\n", x);
								//libaroma_control_resize(slider_btn, x, slider_btn->y, slider_btn->w, slider_btn->h);
								slide_brightness(x, (pager->w)+52);
								//moveslider(btn, progress, curval, x);
							}
							//printf("X: %d, Y: %d\n", msg.x, msg.y);
							break;
						case LIBAROMA_HID_EV_STATE_UP:
							if (dragging){
								int x=msg.x - (slider_btn->w/2);
								if (x<(pager->w+26)) x=(pager->w)+26;
								else if (x>((pager->w*2)-26-(slider_btn->w)))
									x=((pager->w*2)-26-slider_btn->w);
								printf("btn dropped at x %d\n", x);//moveslider(slider_btn, progress, curval, x);
								libaroma_control_resize(slider_btn, x, slider_btn->y, slider_btn->w, slider_btn->h);
								dragging = 0;
							}
							break;
					}
				}
				break;
			}
		//ALOG("Received event with id %i, msg %i, cmd %i, param %i, state %i, key %i, x %i and y %i",
		//		id, msg.msg, cmd, param, msg.state, msg.key, msg.x, msg.y);
	} while (onpool);

	libaroma_window_aniclose(win, LIBAROMA_WINDOW_SHOW_ANIMATION_CIRCLE, 500);
}
