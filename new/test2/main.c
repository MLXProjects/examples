#ifndef __test_main_c__
#define __test_main_c__
#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

/* global zip variable */
LIBAROMA_ZIP zip = NULL;

LIBAROMA_STREAMP stream_uri_cb(char * uri);

int main(int argc, char **argv){ /* TODO: check results */
	byte ret = TEST_RET_OK;
	printf("MLX: loading zip at %s\n", argv[(argc>3)?3:1]);
	/* find resources zip path at second/fourth command-line parameter */
	if (argc>1)
		zip = libaroma_zip(argv[(argc>3)?3:1]);
	if (zip==NULL) /* try to load zip from current path with fixed name */
		zip=libaroma_zip("./res.zip");
	if (zip==NULL){
		//should work without zip file, but more limited (e.g. no icons/text, but animations should work)
		return TEST_RET_ZIP;
	}
	/* set uri callback to use zip as source */
	printf("MLX: %s\n", "setting uri callback");
	libaroma_stream_set_uri_callback(stream_uri_cb);
	printf("MLX: %s\n", "checking args");
#ifndef _WIN32 /* START_SAFE uses forking, not available in Win32 */
	if (argc>3){
		if (atoi(argv[2]) != 0){ //if second arg != 0, we're running from android recovery
			if (libaroma_file_exists("/dev/dri/card0")){
				printf("ui_print Your device uses DRM for display, so you can't run this as a flashable zip.\n"
						"ui_print Yu can only run this via adb shell:\n"
						"\tadb shell /tmp/updater %s\n", argv[3]);
				libaroma_zip_release(zip);
				return 0;
			}
			libaroma_config()->runtime_monitor = LIBAROMA_START_MUTEPARENT;
		}
		//else 
			//libaroma_config()->runtime_monitor = LIBAROMA_START_SAFE;
	}
	else {
		//libaroma_config()->runtime_monitor = LIBAROMA_START_SAFE;
	}
	#endif
	printf("MLX: %s\n", "setting startup size");
	libaroma_sdl_startup_size(360, 556);
	printf("MLX: %s\n", "setting startup dpi");
	//libaroma_gfx_startup_dpi(160);
#ifdef LIBAROMA_GFX_MINUI
	printf("MLX: %s\n", "setting gfx backend");
	libaroma_config()->gfx_first_backend = LIBAROMA_GFX_MINUI;
#endif
	libaroma_config()->multicore_init_num=4;
	libaroma_config_force_cursor();
	//libaroma_sdl_startup_nocursor();
	//libaroma_gfx_override_rgb(1, 0, 8, 16);
	printf("MLX: loading cursor & setdpi\n");
	libaroma_config_cursor_path("res:///cursor.png");
	libaroma_config()->gfx_override_dpi=240;
	/* start libaroma engine */
	printf("MLX: %s\n", "starting libaroma");
	if (!libaroma_start()){
		printf("MLX: %s\n", "libaroma init failed, exiting");
		if (zip) libaroma_zip_release(zip);
		return TEST_RET_AROMA;
	}
	//libaroma_wm_cursor_setvisible(1);
	//libaroma_wm_cursor_setshadow(1);
	//libaroma_fb_setdpi(120);
	
	/* load font from zip */
	printf("MLX: %s\n", "loading font from zip");
	if (!libaroma_font(0, libaroma_stream("res:///fonts/Roboto-Regular.ttf"))){
		ret = TEST_RET_FONT;
		goto homerun;
	}
	
	printf("MLX: %s\n", "init settings");
	/* init & handle settings */
	if (!settings_init()){
		printf("MLX: %s\n", "failed to init settings");
		ret = TEST_RET_SETT;
		goto homerun;
	}
	
	printf("MLX: %s\n", "checking for custom RGB for framebuffer");
	if (test_config()->rgb_custom != NULL)
		libaroma_fb_setrgb(test_config()->rgb_custom[0], test_config()->rgb_custom[1], test_config()->rgb_custom[2]);
	
	/* call main test loop */
	printf("MLX: %s\n", "starting up actual test");
	if (!test()) ret = TEST_RET_IDK;
	
homerun:
	printf("MLX: %s\n", "closing libaroma");
	libaroma_canvas_blank(libaroma_fb()->canvas);
	libaroma_fb_sync();
	/* end libaroma engine & cleanup */
	libaroma_end();
	if (zip) /* check if zip is valid before freeing, maybe we're running without it */
		libaroma_zip_release(zip);
	
	printf("MLX: %s\n", "good bye");
	return ret;
}

/* stream uri callback */
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

#ifdef __cplusplus
}
#endif
#endif /* __test_main_c__ */
