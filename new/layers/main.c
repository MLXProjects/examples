#include <aroma.h>
#include <stdio.h>
#include <pthread.h>

LIBAROMA_ZIP zip;
LIBAROMA_STREAMP stream_uri_cb(char * uri);
int winmain();
LIBAROMA_CONTROLP label;
void mlx_slider_changecb(LIBAROMA_CONTROLP ctl, int value);

byte batt_mon_enable;
pthread_t batt_mon;
int global_brightness=0;
#define MLX_BACKLIGHT_PATH	"/sys/class/backlight/msmfb_bl0"

void *batt_mon_thread(){
	while (batt_mon_enable){
		byte batt_updated=0;
		if (batt_updated){
			batt_updated=0;
			char val[32];
			snprintf(val, 32, "value: %s", "value");
			libaroma_ctl_label_set_text(label, val, 1);
		}
	}
	return NULL;
}

#define mlx_random_color() libaroma_rgb(libaroma_random_range(0, 255), libaroma_random_range(0, 255), libaroma_random_range(0, 255))

void mlx_runonce(){
	LIBAROMA_CANVASP arrow=libaroma_canvas_ex(libaroma_dp(48), libaroma_dp(48), 1);
	LIBAROMA_PATHP content = libaroma_path(libaroma_dp(25), libaroma_dp(25));
	libaroma_path_add(content, libaroma_dp(34), libaroma_dp(34));
	libaroma_path_add(content, libaroma_dp(30), libaroma_dp(34));
	libaroma_path_add(content, libaroma_dp(25), libaroma_dp(38));
	libaroma_path_draw(arrow, content, RGB(FFFFFF), 0xFF, 1, 0.0);
	libaroma_canvas_fillcolor(arrow, RGB(FFFFFF));
	libaroma_png_save(arrow, "/home/mlx/projects/libaroma/arrow.png");
	libaroma_canvas_free(arrow);
}

int main(int argc, char **argv){
	LIBAROMA_CANVASP aroma_logo;
	FILE *acmd_pipe = NULL;
	if (argc>3 && atoi(argv[1]) != 0) {
		libaroma_debug_set_prefix("ui_print ");
		int fd = atoi(argv[2]);
		acmd_pipe = fdopen(fd, "wb");
		libaroma_debug_set_output(acmd_pipe);
		printf("MLX: will mute parent\n");
		libaroma_config()->runtime_monitor = LIBAROMA_START_MUTEPARENT;
	}// else libaroma_config()->runtime_monitor = LIBAROMA_START_SAFE;
	printf("MLX: startup\n");
	zip = libaroma_zip(argv[(argc>3)?3:1]);
	if (!zip) return 0;
	libaroma_stream_set_uri_callback(stream_uri_cb);
	//libaroma_gfx_startup_dpi(160);
	//libaroma_gfx_override_rgb(1, 16, 8, 0);
	libaroma_config_force_cursor();
	libaroma_config()->sdl_mousemove=1;
	libaroma_config()->sdl_nocursor=1;
	//libaroma_config_cursor_path("res:///touch.png");
	libaroma_start();
	//aroma_logo = libaroma_svg_string(libaroma_art_svg_logo, 0, 160);
	//libaroma_png_save(aroma_logo, "/home/mlx/projects/libaroma/aroma.png");
	//libaroma_wm_cursor_setvisible(0);
	//libaroma_wm_cursor_setshadow(0);
	//mlx_runonce();
	//libaroma_fb_setdpi(240);
	libaroma_gfx_startup_dpi(240);
	libaroma_config_cursor_path("res:///cursor.png");
	if (!libaroma_font(0, libaroma_stream_mzip(zip, "roboto.ttf"))) {
		libaroma_end();
		return 0;
	}
	winmain();
	libaroma_end();
	if (acmd_pipe) {
		libaroma_debug_set_output(stdout);
		fclose(acmd_pipe);
	}
	
	return 1;
}

void mlx_brightness_set(int value){
	if (libaroma_file_exists(MLX_BACKLIGHT_PATH "/brightness")){
		FILE *fp;
		fp = fopen(MLX_BACKLIGHT_PATH "/brightness", "w");
		fprintf(fp, "%d", value);
		fclose(fp);
	}
}

int winmain(){
	int reached=0;
	LIBAROMA_WINDOWP win = libaroma_window(NULL, 0, 0, 0, 0);
	if (!win) return 0;
	
	LIBAROMA_CONTROLP fragment, pager;
	LIBAROMA_WINDOWP fragwin, pagwin;
	/*
	fragment = libaroma_ctl_fragment(win, 1, 0, 0, win->width, win->height);
	if (!fragment) goto rip;
	fragwin = libaroma_ctl_fragment_new_window(fragment, 1);
	if (!fragwin) goto rip;
	libaroma_ctl_fragment_set_active_window(fragment, 1, 0, 0, 0, NULL, NULL, NULL);
	pager = libaroma_ctl_pager(fragwin, 2, 1, 0, 0, win->width, win->height);
	if (!pager) goto rip;
	pagwin = libaroma_ctl_pager_get_window(pager);*/
	pagwin=win;
	libaroma_window_anishow(win, 12, 400);
	if (!pagwin) goto rip;
	
	LIBAROMA_CONTROLP cprog = libaroma_ctl_progress(pagwin, 1, (win->width-48)>>1, 24, 48, 48, LIBAROMA_CTL_PROGRESS_INDETERMINATE|LIBAROMA_CTL_PROGRESS_CIRCULAR, 4, 1);
	LIBAROMA_CONTROLP sl_btn = libaroma_ctl_button(pagwin, 2, 0, 96, LIBAROMA_SIZE_THIRD, 48, "update slider", LIBAROMA_CTL_BUTTON_RAISED, 0);
	LIBAROMA_CONTROLP c_btn = libaroma_ctl_button(pagwin, 3, LIBAROMA_POS_1P3, 96, LIBAROMA_SIZE_THIRD, 48, "cursor shadow", LIBAROMA_CTL_BUTTON_RAISED, 0);
	LIBAROMA_CONTROLP cch_btn = libaroma_ctl_button(pagwin, 4, LIBAROMA_POS_2P3, 96, LIBAROMA_SIZE_THIRD, 48, "cursor icon", LIBAROMA_CTL_BUTTON_RAISED, 0);
	LIBAROMA_CONTROLP slider = libaroma_ctl_slider(pagwin, 5, 0, 150, LIBAROMA_SIZE_FULL, 24, 255, 255);
	label=libaroma_ctl_label(pagwin, 6, "value: 0", 0, 190, LIBAROMA_SIZE_FULL, 40, 0, 3, LIBAROMA_TEXT_CENTER, 0);
	
	libaroma_ctl_slider_set_onchange(slider, mlx_slider_changecb);
	
	LIBAROMA_WINDOWP sidebar = libaroma_window_sidebar(win, 0);
	if (!sidebar) goto rip;
	batt_mon_enable=1;
	//pthread_create(&batt_mon, NULL, &batt_mon_thread, NULL);
	LIBAROMA_CANVASP tmp_res=libaroma_image_uri("res:///touch.png");
	LIBAROMA_CANVASP res=libaroma_canvas_ex(libaroma_dp(48), libaroma_dp(48), 1);
	libaroma_draw_scale_smooth(res, tmp_res, 0, 0, res->w, res->h, 0, 0, tmp_res->w, tmp_res->h);
	libaroma_canvas_free(tmp_res);
	tmp_res=NULL;
	LIBAROMA_RECTP last_rect;
	LIBAROMA_MSG msg;
	dword command;
	byte cmd, param;
	word id;
	win->onpool=1;
	byte screen_on=1;
	int cursor_mode=0;
	long last_tick=0;
	do {
		command	= libaroma_window_pool(win,&msg);
		cmd		= LIBAROMA_CMD(command);
		id		= LIBAROMA_CMD_ID(command);
		param	= LIBAROMA_CMD_PARAM(command);
		
		if (msg.msg == LIBAROMA_MSG_EXIT || msg.msg == LIBAROMA_MSG_KEY_SELECT) {
			printf("MLX: close event received\n");
			break;
		}
		else if (msg.msg == LIBAROMA_MSG_KEY_POWER){
			if (msg.state==0){ /* key up */
				//if (libaroma_tick()-last_tick)
			}
			else{ /* key down */
				//last_tick=libaroma_tick();
			}
			char val[32];
			snprintf(val, 32, "state: %d", msg.state);
			libaroma_ctl_label_set_text(label, val, 1);
		}
		else if (msg.msg == LIBAROMA_MSG_MOUSE){
			char val[32];
			if (msg.state == LIBAROMA_HID_EV_STATE_MOVE){
				snprintf(val, 32, "mouse move (%d, %d)", msg.x, msg.y);
			}
			else {
				switch (msg.key){
					case LIBAROMA_HID_WHEEL_KEYCODE:{
						snprintf(val, 32, "mouse wheel=%d (%d, %d)", msg.state, msg.x, msg.y);
					} break;
					case LIBAROMA_HID_LMOUSE_KEYCODE:{
						snprintf(val, 32, "mouse left=%d (%d, %d)", msg.state, msg.x, msg.y);
					} break;
					case LIBAROMA_HID_RMOUSE_KEYCODE:{
						snprintf(val, 32, "mouse right=%d (%d, %d)", msg.state, msg.x, msg.y);
					} break;
				}
			}
			libaroma_ctl_label_set_text(label, val, 1);
		}
		else if (msg.msg == LIBAROMA_MSG_TOUCH){
			switch (msg.state){
				case LIBAROMA_HID_EV_STATE_DOWN:
					//libaroma_wm_cursor_setvisible(1);
					break;
				case LIBAROMA_HID_EV_STATE_UP:
					//libaroma_wm_cursor_anivisible(0);
					break;
			}
		}
		if (cmd==LIBAROMA_CMD_CLICK) {
			if (id==2){
				float state;
				last_tick=libaroma_tick();
				if (screen_on){ /* turn off */
					while ((state=1.0-libaroma_duration_state(last_tick, 300)) > 0.0){
						state=libaroma_motion_accelerate(state);
						mlx_brightness_set(global_brightness*state);
					}
					screen_on=0;
				}
				else { /* turn on */
					while ((state=libaroma_duration_state(last_tick, 300)) < 1.0){
						state=libaroma_motion_accelerate(state);
						mlx_brightness_set(global_brightness*state);
					}
					screen_on=1;
				}
				if (libaroma_ctl_slider_get_value(slider)+40 <= libaroma_ctl_slider_get_max(slider))
					libaroma_ctl_slider_value(slider, libaroma_ctl_slider_get_value(slider)+40);
				else libaroma_ctl_slider_value(slider, 0);
				//sb_visible=(sb_visible)?0:1;
				//libaroma_window_sidebar_show(sidebar, sb_visible);
			}
			else if (id==3){
				libaroma_wm_cursor_setshadow_color(1, mlx_random_color());
			}
			else if (id==4){
				cursor_mode++;
				LIBAROMA_CANVASP target_cur;
				switch (cursor_mode){
					case 0: //arrow
						target_cur=libaroma_image_uri("res:///cursor.png");
						break;
					case 1: //busy
						target_cur=libaroma_image_uri("res:///cursor2.png");
						break;
					case 2: //default
						cursor_mode=-1;
						target_cur=NULL;
						break;
				}
				libaroma_wm_cursor_setimg(target_cur);
				//libaroma_wm_cursor_setshadow(1);
				if (target_cur) libaroma_canvas_free(target_cur);
			}
		}
		else if (cmd==LIBAROMA_CMD_HOLD){
			if (id==2) {
				batt_mon_enable=0;
				void *ret;
				//pthread_join(&batt_mon, &ret)
				break;
			}
		}
	} while (win->onpool);
	libaroma_wm_set_ui_thread(NULL);
	printf("MLX: closing window\n");
	libaroma_window_aniclose(win, 12, 400);
	return 1;
rip:
	printf("MLX: program went RIP\n");
	libaroma_window_free(win);
	return 0;
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

void mlx_slider_changecb(LIBAROMA_CONTROLP ctl, int value){
	char val[32];
	snprintf(val, 32, "value: %d", value);
	libaroma_ctl_label_set_text(label, val, 1);
	mlx_brightness_set(value);
	global_brightness=value;
}
