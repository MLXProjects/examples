#include "common.h"
LIBAROMA_ZIP zip;

#define DEFAULT_WINSIZE 980, 600
#define DEFAULT_WINTITLE "ADraw"

static inline byte path_iszip(char *path){
	 if (0==strncmp(path+(strlen(path)-4), ".zip", 4)) {
		 return 1;
	 }
	 return 0;
}

int main(int argc, char **argv){
	logi("starting up");
	char *zip_path = NULL;
	libaroma_sdl_startup_size(DEFAULT_WINSIZE);
	libaroma_sdl_window_title(DEFAULT_WINTITLE);
	//libaroma_gfx_startup_dpi(160);
	if (argc==4){
		/* test for recovery */
		 if (atoi(argv[2])!=0) libaroma_config()->runtime_monitor=LIBAROMA_START_MUTEPARENT;
	}
	int i;
	for (i=1; i<argc; i++){
		if (0==strncmp(argv[i], "-dpi=", 5)){
			logi("overriding dpi to %d", atoi(argv[i]+5));
			libaroma_gfx_startup_dpi(atoi(argv[i]+5));
		}
		else if (0==strncmp(argv[i], "-size=", 6)){
			int w, h;
			char *wstr, *hstr;
			hstr=wstr=argv[i]+6;
			while (*hstr++!='\0'){
				if (*hstr=='x') {
					hstr++;
					break;
				}
			}
			if (*hstr=='\0'){
				loge("window size parameter (%s) missing height", argv[i]+6);
				continue;
			}
			w=atoi(wstr);
			h=atoi(hstr);
			logi("overriding window size to %dx%d", w, h);
			libaroma_sdl_startup_size(w, h);
		}
		else if (0==strncmp(argv[i], "-res=", 5)){
			zip_path=argv[i]+5;
		}
		else if (path_iszip(argv[i])){
			zip_path=argv[i];
		}
	}
	byte tried_default=0;
	if (zip_path==NULL) {
		zip_path="./adraw-res.zip";
		tried_default=1;
	}
	zip = libaroma_zip(zip_path);
	if (!zip){
		if (!tried_default) zip = libaroma_zip("./adraw-res.zip");
		if (!zip){
			loge("failed to load resources zip");
			return 0;
		}
	}
	if (!libaroma_start()){
		loge("libaroma start failed");
		libaroma_zip_release(zip);
		return 1;
	}
	libaroma_stream_set_uri_callback(stream_uri_cb);
	libaroma_font(0, libaroma_stream("res:///fonts/Roboto-Regular.ttf"));
	
	adraw_home();
	
	int ret=0;
	ret = libaroma_end();
	libaroma_zip_release(zip);
	
	return (ret)?0:1;
}