#include "common.h"

/* public variables */
byte br_ui_deftrans_ran=0;
LIBAROMA_ZIP res_zip;
LIBAROMA_XML bible, cur_book, cur_chap;
LIBAROMA_WINDOWP ui_wmain, ui_whome, ui_wchap, ui_wread, ui_wsettings;
LIBAROMA_CONTROLP appbar, mainfrag, pager, tabs, lbooklist, rbooklist, chaplist, readlist, settlist;
int br_ui_maxvnumw;
char *res_path;

/* uri callback prototype */
LIBAROMA_STREAMP stream_uri_cb(char * uri);

int main(int argc, char **argv){
	LOGD("argc=%d", argc);
	res_path = argv[(argc>3)?3:1];
	res_zip=libaroma_zip(res_path);
	if (!res_zip){
		LOGE("failed to load zip at %s", res_path);
		return 0;
	}
	libaroma_stream_set_uri_callback(stream_uri_cb);
	/* use runtime monitor if not on win32 */
	#ifndef _WIN32
	if (argc>3 && atoi(argv[2])!=0) libaroma_config()->runtime_monitor=LIBAROMA_START_MUTEPARENT;
	else libaroma_config()->runtime_monitor=LIBAROMA_START_SAFE;
	#endif
	if (!br_ui_init()){
		LOGE("ui init failed");
		goto bye;
	}
	if (!br_ui_loop()){
		LOGE("something went wrong at main loop");
	}
bye:
	libaroma_end();
	libaroma_zip_release(res_zip);
	return 0;
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
		LIBAROMA_STREAMP ret=libaroma_stream_mzip(res_zip, uri + 7);
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