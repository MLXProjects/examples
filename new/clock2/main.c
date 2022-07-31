#include <aroma.h>

void winmain();
LIBAROMA_STREAMP stream_uri_cb(char * uri);
void mlx_slider_changecb(LIBAROMA_CONTROLP ctl, int value);

LIBAROMA_ZIP zip;

#define BACKLIGHT_PATH "/sys/class/leds/lcd-backlight"

int main(int argc, char **argv){
	if (argc>3){
		zip=libaroma_zip(argv[3]);
		libaroma_stream_set_uri_callback(stream_uri_cb);
		if (atoi(argv[2])!=0)
			libaroma_config()->runtime_monitor = LIBAROMA_START_MUTEPARENT;
	}
	else if (argc>1)
		zip=libaroma_zip(argv[1]);
	libaroma_config()->sdl_wm_width=360;
	libaroma_config()->sdl_wm_height=360;
	libaroma_start();
	libaroma_font(0, libaroma_stream("res:///roboto.ttf"));
	libaroma_wm_cursor_setvisible(0);
	winmain();
	libaroma_end();
	return 0;
}

void mlx_brightness_set(int value){
	if (libaroma_file_exists(BACKLIGHT_PATH "/brightness")){
		FILE *fp;
		fp = fopen(BACKLIGHT_PATH "/brightness", "w");
		fprintf(fp, "%d", value);
		fclose(fp);
	}
}

void mlx_cpu_manager(byte slower){
	if (libaroma_file_exists("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor")){
		FILE *fp;
		fp = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor", "w");
		fprintf(fp, "%s", (slower)?"powersave":"performance");
		fclose(fp);
	}
}

int global_brightness=255;

void mlx_onupdatebg(LIBAROMA_WINDOWP win, LIBAROMA_CANVASP bg){
	printf("MLX: background update\n");
	//libaroma_draw(bg, win->prev_screen, 0, 0, 0);
}
LIBAROMA_CONTROLP hourlist;
void mlx_sethourclick(LIBAROMA_CONTROLP list){
	LIBAROMA_CTL_LIST_ITEMP item = libaroma_ctl_list_get_touched_item(list);
	if (!item) return;
	//LIBAROMA_RECT item_pos={0};
	//libaroma_ctl_list_item_position(list, &item_pos, 0);
	//libaroma_ctl_scroll_request_pos(list, item_pos->)
	libaroma_ctl_list_scroll_to_item(list, item, 1);
}

void mlx_sethouraddsub(LIBAROMA_CONTROLP btn){
	//LIBAROMA_CTL_LIST_ITEMP item = libaroma_ctl_list_get_touched_item(hourlist);
	//if (!item) return;
	//LIBAROMA_RECT item_pos={0};
	//libaroma_ctl_list_item_position(list, &item_pos, 0);
	//libaroma_ctl_scroll_request_pos(list, item_pos->)
	//libaroma_ctl_list_scroll_to_item(list, item, 1);
}

byte clock_sett_init(LIBAROMA_WINDOWP win, int xoff){
	LIBAROMA_CONTROLP subbtn=libaroma_ctl_button(win, 90, xoff, 0, 64, 38, "-", LIBAROMA_CTL_BUTTON_RAISED|LIBAROMA_CTL_BUTTON_COLORED, RGB(E0E0E0));
	libaroma_control_set_onclick(subbtn, &mlx_sethouraddsub);
	hourlist = libaroma_ctl_list(win, 91, xoff, 38, 64, 48, 0, 0, RGB(FFFFFF), LIBAROMA_CTL_LIST_NO_INDICATOR);
	libaroma_control_set_onclick(hourlist, &mlx_sethourclick);
	char num_str[64];
	int i;
	for (i=1; i<13; i++){
		snprintf(num_str, 64, "<@center>%d", i);
		libaroma_listitem_menu(hourlist, i, num_str, NULL, NULL, LIBAROMA_LISTITEM_MENU_SMALL, -1);
	}
	LIBAROMA_CONTROLP addbtn=libaroma_ctl_button(win, 92, xoff, 86, 64, 38, "+", LIBAROMA_CTL_BUTTON_RAISED|LIBAROMA_CTL_BUTTON_COLORED, RGB(E0E0E0));
	libaroma_control_set_onclick(addbtn, &mlx_sethouraddsub);
}

void winmain(){
	LIBAROMA_WINDOWP win, pwin;
	win=libaroma_window(NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
	if (!win) return;
	//win->prev_screen=libaroma_canvas(win->w, win->h);
	/*LIBAROMA_CANVASP cv = libaroma_image_uri("res:///bg.png");
	if (!cv) return;
	libaroma_draw_scale_smooth(win->prev_screen, cv, 0, 0, win->w, win->h, 0, 0, cv->w, cv->h);
	libaroma_canvas_free(cv);*/
	printf("MLX: setting onupdate\n");
	win->onupdatebg=&mlx_onupdatebg;
	if (win->bg!=NULL)
		libaroma_canvas_free(win->bg);
	win->bg=libaroma_canvas(win->w, win->h);
	mlx_onupdatebg(win, win->bg);
	printf("MLX: setting onupdate done\n");
	LIBAROMA_CONTROLP clk, pager, slider;
	pager=libaroma_ctl_pager(win, 2, 2, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
	pwin = libaroma_ctl_pager_get_window(pager);
	LIBAROMA_CANVASP cv;
	cv = libaroma_image_uri("res:///bg.png");
	if (!cv) cv = libaroma_canvas(libaroma_fb()->w, libaroma_fb()->h);
	clk=libaroma_ctl_clock(pwin, 1, 0, 0, win->width, LIBAROMA_SIZE_FULL, cv, LIBAROMA_CTL_CLOCK_NOBORDER|LIBAROMA_CTL_CLOCK_RIPPLE);
	libaroma_canvas_free(cv);
	//libaroma_ctl_clock_setalpha(clk, 0xff);
	libaroma_ctl_clock_setcolor(clk, LIBAROMA_CTL_CLOCK_COLOR_HOUR, RGB(FFFFFF));
	libaroma_ctl_clock_setcolor(clk, LIBAROMA_CTL_CLOCK_COLOR_MINUTE, RGB(FFFFFF));
	libaroma_ctl_clock_setcolor(clk, LIBAROMA_CTL_CLOCK_COLOR_SECOND, RGB(FFFFFF));
	libaroma_ctl_clock_setcolor(clk, LIBAROMA_CTL_CLOCK_COLOR_CENTER, RGB(FFFFFF));
	libaroma_ctl_clock_setcolor(clk, LIBAROMA_CTL_CLOCK_COLOR_BORDER, RGB(440000));
	clock_sett_init(pwin, win->width);
	slider=libaroma_ctl_slider(pwin, 0, win->width+8, (win->height-40)/*/2*/, win->width-16, 40, 255, 255);
	libaroma_ctl_slider_set_onchange(slider, &mlx_slider_changecb);
	libaroma_window_anishow(win, 12, 400);
	libaroma_ctl_pager_set_active_page(pager, 1, 1.0);
	win->onpool=1;
	long tick=0;
	int t_cnt=0;
	byte screen_off=0;
	float state;
	do {
		LIBAROMA_MSG msg;
		dword command	=libaroma_window_pool(win,&msg);
		byte cmd		=LIBAROMA_CMD(command);
		word id			=LIBAROMA_CMD_ID(command);
		byte param		=LIBAROMA_CMD_PARAM(command);

		if (msg.msg==LIBAROMA_MSG_KEY_SELECT || msg.msg==LIBAROMA_MSG_EXIT){
			if (msg.state==0){
				win->onpool = 0;
				break;
			}
		}
		else if (msg.msg==LIBAROMA_MSG_KEY_POWER){
			win->onpool=0;
			/*if (msg.state==0){
				if (screen_off) mlx_brightness_set(global_brightness);
				else mlx_brightness_set(0);
				screen_off=!screen_off;
				win->lock_sync=!win->lock_sync;
			}*/
		}
		else if (cmd==LIBAROMA_CMD_CLICK){
			if (id==clk->id){
				if (tick!=0){
					long res_tick=libaroma_tick()-tick;
					if (res_tick<800){
						printf("tick (%ld, %ld)\n", tick, res_tick);
						win->lock_sync=!win->lock_sync;
						if (screen_off){ /* turn on pls */
							mlx_cpu_manager(0);
							while ((state=libaroma_duration_state(tick, 300))<1.0){
								mlx_brightness_set(global_brightness*state);
							}
						}
						else { /* turn off pls */
							while ((state=1.0-libaroma_duration_state(tick, 300))){
								mlx_brightness_set(global_brightness*state);
							}
							mlx_cpu_manager(1);
						}
						screen_off=!screen_off;
					}
					tick=0;
					t_cnt=0;

				}
				else {
					printf("set tick\n");
					tick=libaroma_tick();
					t_cnt=1;
				}
			}
		}
		else if (cmd==LIBAROMA_CMD_HOLD){
			if (id==clk->id){
				printf("MLX: ripple hold\n");
				win->onpool=0;
				break;
			}
		}/*
		else if (msg.msg==LIBAROMA_MSG_TOUCH){
			if (msg.state==LIBAROMA_HID_EV_STATE_DOWN){
			}
		}*/
	} while (win->onpool);
	libaroma_window_aniclose(win, 12, 400);
}

void mlx_slider_changecb(LIBAROMA_CONTROLP ctl, int value){
	char val[32];
	snprintf(val, 32, "value: %d", value);
	mlx_brightness_set(value);
	global_brightness=value;
}

LIBAROMA_STREAMP stream_uri_cb(char * uri){
	int n = strlen(uri);
	char kwd[11];
	int i;
	for (i = 0; i < n && i < 10; i++) {
		kwd[i] = uri[i];
		kwd[i + 1] = 0;
		if ((i > 1) && (uri[i] == '/') && (uri[i - 1] == '/')) {
			break;
		}
	}
	/* resource stream */
	if (strcmp(kwd, "res://") == 0) {
		LIBAROMA_STREAMP ret=libaroma_stream_mzip(zip, uri + 7);
		if (ret){
			/* change uri */
			snprintf(ret->uri,
					 LIBAROMA_STREAM_URI_LENGTH,
			"%s", uri
			);
			return ret;
		}
	}
	return NULL;
}
