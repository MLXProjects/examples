#ifndef __ptero_main_c__
#define __ptero_main_c__
#ifdef __cplusplus
extern "C" {
#endif
#include "common.h"

/*
 * Function		: main
 * Parameters	: arg count & array
 * Return type	: int
 * Description	: main program function
 */
int main(int argc, char **argv){
	byte ret=0;
	if (argc>3 && atoi(argv[1]) != 0) {
		printf("MLX: will mute parent\n");
		libaroma_config()->runtime_monitor = LIBAROMA_START_MUTEPARENT;
	}
	if (!ptero_init(argc, argv)) return 1;
	if (!ptero_main()) return 2;
	if (!ptero_exit()) return 3;
	
	return ret;
}

#ifdef __cplusplus
}
#endif
#endif /* __ptero_main_c__ */
