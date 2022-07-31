#include <aroma.h>
#include <pthread.h>

LIBAROMA_ZIP zip;
LIBAROMA_STREAMP stream_uri_cb(char * uri);
int winmain();

int main(int argc, char **argv){
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
	libaroma_config_force_cursor();
	libaroma_config()->sdl_mousemove=1;
	libaroma_config()->sdl_nocursor=1;
	libaroma_start();
	//libaroma_wm_cursor_setshadow(0);
	//libaroma_fb_setdpi(240);
	if (!libaroma_font(0, libaroma_stream_mzip(zip, "roboto.ttf"))) {
		libaroma_end();
		return 0;
	}
	LIBAROMA_CANVASP target_cur=NULL;
	target_cur=libaroma_image_uri("res:///cursor.png");
	libaroma_wm_cursor_setimg(target_cur);
	if (target_cur) libaroma_canvas_free(target_cur);
	winmain();
	libaroma_end();
	if (acmd_pipe) {
		libaroma_debug_set_output(stdout);
		fclose(acmd_pipe);
	}
	return 1;
}

int winmain(){
	int reached=0;
	LIBAROMA_WINDOWP win = libaroma_window(NULL, 0, 0, 0, 0);
	if (!win) return 0;
	
	LIBAROMA_CONTROLP fragment, pager;
	LIBAROMA_WINDOWP fragwin, pagwin;
	
	fragment = libaroma_ctl_fragment(win, 1, 0, 16, win->width, win->height-64);
	if (!fragment) goto rip;
	fragwin = libaroma_ctl_fragment_new_window(fragment, 1);
	if (!fragwin) goto rip;
	libaroma_ctl_fragment_set_active_window(fragment, 1, 0, 0, 0, NULL, NULL, NULL);
	/*pager = libaroma_ctl_pager(fragwin, 2, 1, 0, 0, win->width, win->height);
	if (!pager) goto rip;
	pagwin = libaroma_ctl_pager_get_window(pager);*/
	pagwin=win;
	libaroma_window_anishow(win, 12, 400);
	if (!pagwin) goto rip;
	
	LIBAROMA_CONTROLP cprog = libaroma_ctl_progress(pagwin, 1, 0, 0, 16, 16, LIBAROMA_CTL_PROGRESS_INDETERMINATE|LIBAROMA_CTL_PROGRESS_CIRCULAR, 4, 1);
	LIBAROMA_CONTROLP sl_btn = libaroma_ctl_button(pagwin, 2, 0, win->height-48, LIBAROMA_SIZE_THIRD, 48, "update slider", LIBAROMA_CTL_BUTTON_RAISED, 0);
	LIBAROMA_CONTROLP c_btn = libaroma_ctl_button(pagwin, 3, LIBAROMA_POS_1P3, win->height-48, LIBAROMA_SIZE_THIRD, 48, "cursor shadow", LIBAROMA_CTL_BUTTON_RAISED, 0);
	LIBAROMA_CONTROLP cch_btn = libaroma_ctl_button(pagwin, 4, LIBAROMA_POS_2P3, win->height-48, (win->width/3)-24, 48, "cursor icon", LIBAROMA_CTL_BUTTON_RAISED, 0);
	LIBAROMA_WINDOWP sidebar = libaroma_window_sidebar(win, 0);
	if (!sidebar) goto rip;
	
	
	
	//pthread_create(&batt_mon, NULL, &batt_mon_thread, NULL);
	LIBAROMA_MSG msg;
	dword command;
	byte cmd, param;
	word id;
	win->onpool=1;
	do {
		command	= libaroma_window_pool(win,&msg);
		cmd		= LIBAROMA_CMD(command);
		id		= LIBAROMA_CMD_ID(command);
		param	= LIBAROMA_CMD_PARAM(command);
		
		if (msg.msg == LIBAROMA_MSG_EXIT || msg.msg == LIBAROMA_MSG_KEY_POWER) {
			printf("MLX: close event received\n");
			break;
		}
		/*else if (msg.msg == LIBAROMA_MSG_TOUCH){
			switch (msg.state){
				case LIBAROMA_HID_EV_STATE_DOWN:
					//libaroma_wm_cursor_setvisible(1);
					break;
				case LIBAROMA_HID_EV_STATE_UP:
					//libaroma_wm_cursor_anivisible(0);
					break;
			}
		}*/
		if (cmd==LIBAROMA_CMD_CLICK) {
		}
		else if (cmd==LIBAROMA_CMD_HOLD){
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