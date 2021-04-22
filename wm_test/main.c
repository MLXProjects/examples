#include "tools.h"

#ifdef __cplusplus
extern "C"{
#endif

LIBAROMA_ZIP zip;
int win_x;
int win_y;

int main(int argc, char ** argv){
	zip=libaroma_zip(argv[3]);
	libaroma_stream_set_uri_callback(stream_uri_callback);
	libaroma_start();
	libaroma_font(0, libaroma_stream("res:///roboto.ttf"));
	win_x=20;
	win_y=40;
	winmain();

	libaroma_end();

	return 0;
}

LIBAROMA_STREAMP stream_uri_callback(char * uri){
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
