#ifndef __bootmenu__c__
#define __bootmenu__c__
#ifdef __cplusplus
extern "C" {
#endif

#define _XOPEN_SOURCE 500

#include "common.h"

LIBAROMA_ZIP zip;
LIBAROMA_STREAMP stream_uri_cb(char * uri);
LIBAROMA_WINDOWP winmain, homefrag, settfrag, filemfrag, filesfrag;
LIBAROMA_CONTROLP appbar, fragctl, homelist, filelist, settlist;

int main(int argc, char **argv){
	/* must pass at least bin name and zip path as arguments */
	if (argc<1) return 1;
	/* load zip to memory */
	zip=libaroma_zip(argv[(argc>3)?3:1]);
	/* set DRM as first */
#ifdef LIBAROMA_GFX_MINUI
	libaroma_config()->gfx_first_backend = LIBAROMA_GFX_MINUI;
#endif /* LIBAROMA_GFX_MINUI */
	/* start aroma */
	if (!libaroma_start()) return 1;
	/* hide cursor, if any */
	libaroma_wm_cursor_setvisible(0);
	/* set proper dpi for ldpi */
	//libaroma_fb_setdpi(120);
	/* init resources */
	libaroma_stream_set_uri_callback(stream_uri_cb);
	libaroma_font(0, libaroma_stream("res:///roboto.ttf"));
	/* decorate statusbar */
	int sb_height=libaroma_dp(24);
	libaroma_wm_set_workspace(0, sb_height, libaroma_fb()->w, libaroma_fb()->h-sb_height);
	libaroma_draw_rect(libaroma_fb()->canvas, 0, 0, libaroma_fb()->w, sb_height, RGB(006b61), 0xFF);
	//libaroma_draw_text_ex(libaroma_fb()->canvas, "Libaroma", 0, 0, RGB(FFFFFF), libaroma_fb()->w, LIBAROMA_FONT(0, 4)|LIBAROMA_TEXT_CENTER|LIBAROMA_TEXT_SINGLELINE, sb_height, 1, libaroma_dp(4), RGB(0), 0xFF, 0, 0);
	//LIBAROMA_TEXT header=libaroma_text("Libaroma", RGB(FFFFFF), libaroma_fb()->w, );
	libaroma_draw_text(libaroma_fb()->canvas, "Libaroma", 0, 0, RGB(FFFFFF), libaroma_fb()->w, LIBAROMA_FONT(0, 4)|LIBAROMA_TEXT_CENTER|LIBAROMA_TEXT_SINGLELINE, sb_height);
	/* start ui */
	byte ret=bootmenu_ui();
	ret=libaroma_end();
	bootmenu_haltreboot_handler();
	
	return ret?0:1;
}

void bootmenu_haltreboot_handler(){
	if (bootmenu_haltreboot<1) return;
	sync();
	switch (bootmenu_haltreboot){
		case 1:
			syscall(SYS_reboot, LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2, LINUX_REBOOT_CMD_RESTART);
			break;
		case 2:
			syscall(SYS_reboot, LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2, LINUX_REBOOT_CMD_POWER_OFF, NULL);
			break;
	}
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
#endif /* __bootmenu__c__ */ 
