#ifndef __tools_h__
#define __tools_h__

#include <aroma.h>
#include <time.h>

#ifdef __cplusplus
extern "C"{
#endif

#define ALOG(...)	printf("MLX: "); \
					printf(__VA_ARGS__); \
					printf("\n")

extern LIBAROMA_ZIP zip;
extern LIBAROMA_STREAMP stream_uri_callback(char * uri);
extern int win_x;
extern int win_y;
extern void bg_update(LIBAROMA_WINDOWP win, LIBAROMA_CANVASP cv);
extern void winmain();
#ifdef __cplusplus
}
#endif

#endif //__tools_h__
