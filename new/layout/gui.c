#ifndef __layout_gui_c__
#define __layout_gui_c__
#include "common.h"
LIBAROMA_STREAMP _layout_stream_uri_cb(char * uri);
extern LAYOUT_WINLIST _layout_wins;
extern LIBAROMA_WINDOWP to_show;
extern char *to_showid;

byte layout_gui_init(char *zip_path){
	LOGI("let's set up everything!");
	byte ret=0;
	zip=libaroma_zip(zip_path);
	if (!zip){
		LOGI("failed to load zip");
		//return 0; /* zip is mandatory */
	}
	libaroma_stream_set_uri_callback(_layout_stream_uri_cb);
	libaroma_config()->gfx_override_dpi=240;
	LOGI("start libaroma");
	ret=libaroma_start();
	if (!ret){
		LOGI("failed to start libaroma");
		return 0;
	}
	LOGI("we're done here");
	return 1;
}

void layout_gui_loop2(){
	LOGI("hey there, I'm gui_loop");
	layout_parse_xml("file://D:/MLX/Documents/libaroma/apps/layout/zip/layout.xml");
	if (to_show!=NULL){
		LOGI("showing window (%p)", to_show);
		libaroma_window_anishow(to_show, 12, 400);
		to_show->onpool=1;
		LIBAROMA_MSG msg;
		dword command;
		byte cmd, param;
		word id;
		do {
			command	= libaroma_window_pool(to_show,&msg);
			cmd		= LIBAROMA_CMD(command);
			id		= LIBAROMA_CMD_ID(command);
			param	= LIBAROMA_CMD_PARAM(command);
			
			if (msg.msg == LIBAROMA_MSG_EXIT) {
				LOGI("bye bye window");
				break;
			}
		} while(to_show->onpool);
		libaroma_window_aniclose(to_show, 12, 400);
		layout_wins_del(NULL, to_showid, 0);
	}
	LOGI("see u soon! (not really, if we see again there's something weird goin' on here)");
}

void layout_gui_loop(){
	LOGI("hey there, I'm gui_loop");
	libaroma_font(0, libaroma_stream("res:///roboto.ttf"));
	LIBAROMA_WINDOWP test_win=libaroma_window(NULL, 0, libaroma_px(libaroma_fb()->h)-48, LIBAROMA_SIZE_FULL, 48);
	if (!test_win) return;
	libaroma_ctl_button(test_win, 1, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL, "Load layout.xml", LIBAROMA_CTL_BUTTON_RAISED, 0); 
	libaroma_window_anishow(test_win, LIBAROMA_WINDOW_SHOW_ANIMATION_PAGE_TOP, 400);
	test_win->onpool=1;
	LIBAROMA_MSG msg;
	dword command;
	byte cmd, param;
	word id;
	byte resz=0;
	do {
		command	= libaroma_window_pool(test_win,&msg);
		cmd		= LIBAROMA_CMD(command);
		id		= LIBAROMA_CMD_ID(command);
		param	= LIBAROMA_CMD_PARAM(command);
		
		if (msg.msg == LIBAROMA_MSG_EXIT) {
			LOGI("bye bye window");
			break;
		}
		if (cmd==LIBAROMA_CMD_CLICK && id==1){
			if (resz){
				resz=1;
				int rx, ry, rw, rh;
				rx=(test_win->rx>0)?libaroma_px(test_win->rx):test_win->rx;
				ry=(test_win->ry>0)?libaroma_px(test_win->ry):test_win->ry;
				rw=(test_win->rw>0)?libaroma_px(test_win->rw):test_win->rw;
				rh=(test_win->rh>0)?libaroma_px(test_win->rh):test_win->rh;
				libaroma_fb_setdpi(160);
				libaroma_window_resize(test_win, rx, ry, rw, rh);
				//libaroma_msg_post(LIBAROMA_MSG_WIN_INVALIDATE, 0, 0, 0, 0, NULL);
				//libaroma_msg_post(LIBAROMA_MSG_WIN_RESIZE, 0, 0, 0, 0, NULL);
				continue;
			}
			#ifdef _WIN32
			layout_parse_xml("file://D:/MLX/Documents/libaroma/apps/layout/zip/layout.xml");
			#else
			layout_parse_xml("res:///layout.xml");
			#endif
			if (to_show!=NULL){
				LOGI("showing window (%p)", to_show);
				libaroma_window_anishow(to_show, 12, 400);
				to_show->onpool=1;
				LIBAROMA_MSG msg;
				dword command;
				byte cmd, param;
				word id;
				do {
					command	= libaroma_window_pool(to_show,&msg);
					cmd		= LIBAROMA_CMD(command);
					id		= LIBAROMA_CMD_ID(command);
					param	= LIBAROMA_CMD_PARAM(command);
					
					if (msg.msg == LIBAROMA_MSG_EXIT) {
						LOGI("bye bye window");
						break;
					}
				} while(to_show->onpool);
				libaroma_window_aniclose(to_show, 12, 400);
				layout_wins_del(NULL, to_showid, 0);
				if (_layout_wins.winn>0){
					int i;
					for (i=_layout_wins.winn; i>-1; i--){
						layout_wins_delindex(i, 1);
					}
				}
				libaroma_wm_set_active_window(test_win);
			}
		}
	} while (test_win->onpool);
	libaroma_window_aniclose(test_win, LIBAROMA_WINDOW_SHOW_ANIMATION_PAGE_TOP, 400);
	LOGI("see u soon! (not really, if we see again there's something weird goin' on here)");
}

byte layout_gui_cleanup(){
	LOGI("cleanup & exit");
	if (_layout_wins.winn>0){
		int i;
		for (i=_layout_wins.winn; i>-1; i--){
			layout_wins_delindex(i, 1);
		}
	}
	if (zip!=NULL) libaroma_zip_release(zip);
	LOGI("good bye!");
	return libaroma_end();
}

LIBAROMA_STREAMP _layout_stream_uri_cb(char * uri){
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

#endif /*__layout_gui_c__*/