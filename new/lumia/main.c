#ifndef __aromafm__c__
#define __aromafm__c__
#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

LIBAROMA_ZIP zip;
LIBAROMA_STREAMP stream_uri_cb(char * uri);
LIBAROMA_WINDOWP winmain, sidebar, homefrag, settfrag, filemfrag, filesfrag, busyfrag, kbfrag;
LIBAROMA_CONTROLP appbar, fragctl, sidelist, filelist, settlist, homelist, busyprog, kbfctl;

int main(int argc, char **argv){
	/* must pass at least bin name and zip path as arguments */
	if (argc<1) return 1;
	/* load zip to memory */
	zip=libaroma_zip(argv[(argc>3)?3:1]);
	/* set DRM as first */
#ifdef LIBAROMA_GFX_MINUI
	libaroma_config()->gfx_first_backend = LIBAROMA_GFX_MINUI;
#endif /* LIBAROMA_GFX_MINUI */
	/* prepare monitor */
	//libaroma_config()->runtime_monitor = LIBAROMA_START_SAFE;
	if (argc>3 && atoi(argv[2])!=0) { /* running from recovery */
		libaroma_config()->runtime_monitor = LIBAROMA_START_MUTEPARENT;
	}
	/* obtain string hashes */
	/*printf(
		"***************HASHES***************\n"
		"apk = %lu\n"
		"sapk= %lu\n"
		"prop= %lu\n"
		"zip = %lu\n"
		"jar = %lu\n"
		"png = %lu\n"
		"jpg = %lu\n"
		"jpeg= %lu\n"
		"tar = %lu\n"
		"win = %lu\n"
		"info= %lu\n"
		"gif = %lu\n"
		"mp4 = %lu\n"
		"html= %lu\n"
		"txt = %lu\n"
		"sh  = %lu\n"
		"rar = %lu\n"
		"mp3 = %lu\n"
		"xml = %lu\n"
		"svg = %lu\n"
		"img = %lu\n"
		"*************END HASHES*************\n",
		libaroma_hash("apk"),
		libaroma_hash("sapk"),
		libaroma_hash("prop"),
		libaroma_hash("zip"),
		libaroma_hash("jar"),
		libaroma_hash("png"),
		libaroma_hash("jpg"),
		libaroma_hash("jpeg"),
		libaroma_hash("tar"),
		libaroma_hash("win"),
		libaroma_hash("info"),
		libaroma_hash("gif"),
		libaroma_hash("mp4"),
		libaroma_hash("html"),
		libaroma_hash("txt"),
		libaroma_hash("sh"),
		libaroma_hash("rar"),
		libaroma_hash("mp3"),
		libaroma_hash("xml"),
		libaroma_hash("svg"),
		libaroma_hash("img")
	);
	libaroma_zip_release(zip);
	return 0;
	*/
	/* start libaroma */
	if (!libaroma_start()) return 1;
	/* init resources */
	libaroma_stream_set_uri_callback(stream_uri_cb);
	//libaroma_font(0, libaroma_stream("res:///fonts/Roboto-Regular.ttf"));
	libaroma_font(0, libaroma_stream("res:///fonts/NotoColorEmoji.ttf"));
	libaroma_font(1, libaroma_stream("res:///fonts/Monospace.ttf"));
	/* decorate statusbar */
	libaroma_wm_set_workspace(0, aromafm_statusbar_height(), libaroma_fb()->w, libaroma_fb()->h-(aromafm_statusbar_height()*3));
	//libaroma_draw_text_ex(libaroma_fb()->canvas, "Libaroma", 0, 0, RGB(FFFFFF), libaroma_fb()->w, LIBAROMA_FONT(0, 4)|LIBAROMA_TEXT_CENTER|LIBAROMA_TEXT_SINGLELINE, sb_height, 1, libaroma_dp(4), RGB(0), 0xFF, 0, 0);
	//LIBAROMA_TEXT header=libaroma_text("Libaroma", RGB(FFFFFF), libaroma_fb()->w, );
	/* start ui */
	byte ret=aromafm_ui();
	if (!libaroma_end()) return 0;
	
	return ret?0:1;
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

char *aromafm_basename_ex(char *path, byte skip_last_slash) {
	if (!path) return NULL;
	char *token;
	char *ret=NULL;
	char *rest = path;
	while ((token = strtok_r(rest, "/", &rest))){
		if (skip_last_slash && (token+1=='\0')) break;
		ret=token;
	}
	return ret;
}

#ifdef __cplusplus
}
#endif
#endif /* __aromafm__c__ */ 
 
