#ifndef __br_main_c__
#define __br_main_c__

#include "main.h"

LIBAROMA_ZIP zip;
extern int winmain();
#ifndef _WIN32
	#define RLOG(...) printf("ui_print MLX: "); printf(__VA_ARGS__)
	#include <unistd.h>
	#include <stdlib.h>
	#include <sys/wait.h>
	#include <string.h>
	#define mlx_log(...) fprintf(log, __VA_ARGS__); fprintf(log, "\n")
#else
	#define RLOG(...)
#endif


byte br_init(int argc, char **argv){
	byte ret=0;
	zip=libaroma_zip(argv[(argc<4)?1:3]);
	if (zip==NULL){
		printf("Failed to load ZIP at %s\n", argv[(argc<4)?1:3]);
		goto bye;
	}// else printf("MLXPROJECT(): ZIP loaded\n");
	libaroma_stream_set_uri_callback(stream_uri_cb);
	//libaroma_config()->custom_color_handler = custcolor_handler;
	ret=libaroma_start();
	if (!ret) goto bye;
	
	/* load font - id=0 */
	ret = libaroma_font(0,
		libaroma_stream(
			"res:///fonts/Roboto-Regular.ttf"
		)
	);
	if (!ret) {
		printf("ERROR: Font failed to load, exiting.\n");
		libaroma_end();
		goto bye;
	}
	ret = winmain();
	if (!ret){
		switch (load_status){
			case 2: //free reader window
				libaroma_window_free(fragread);
			case 1: //free main window
				libaroma_window_free(mainwin);
				break;
		}
	}
clean:
	br_clean();
	ret = libaroma_end();
	libaroma_zip_release(zip);
bye: //jump here if program fails before libaroma init
	{
		char *args[3]={"start", "recovery", NULL};
		execvp(args[0], args);
	}
	return ret?0:1;
}

int main(int argc, char **argv){
	RLOG("Project starting up\n");
	#ifndef _WIN_32
	//FILE *log;
	//log = fopen("/tmp/log", "w");
	if (argc>=4){
		if (argv[2]!='0') //valid recovery fd, mute parent
			libaroma_config()->runtime_monitor = LIBAROMA_START_MUTEPARENT;
	}
	#endif
	//TODO: close /tmp/log pipe on exit
	return br_init(argc, argv);
}

#endif /* __br_main_c__ */