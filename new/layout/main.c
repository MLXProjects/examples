#ifndef __layout_main_c__
#define __layout_main_c__
#include "common.h"
LIBAROMA_ZIP zip;

int main(int argc, char **argv){
	LOGI("startin' up");
	byte ret=0;
	if (argc>3 && atoi(argv[1]) != 0) {
		libaroma_config()->runtime_monitor = LIBAROMA_START_MUTEPARENT;
	}
	#ifdef _WIN32
	char *zip_path=(argc>1)?argv[(argc>3)?3:1]:"./res.zip"; /* hardcoded path as last resort */
	#else
	char *zip_path="/home/mlx/libaroma/apps/layout/zip/res.zip";
	//char *zip_path="D:/MLX/Documents/libaroma/apps/layout/zip/res.zip";
	#endif
	if (layout_gui_init(zip_path)){
		layout_gui_loop();
		return layout_gui_cleanup();
	}
	return 0;
}

#endif /*__layout_main_c__*/