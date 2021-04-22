#ifndef __tools_h__
#define __tools_h__

#include <aroma.h>
#include <time.h>

#ifdef __cplusplus
extern "C"{
#endif

#define ALOG(...)	printf(__VA_ARGS__); \
					printf("\n")

#define clock_init(win) clock_init_ex(win, 0)
extern LIBAROMA_CONTROLP slider_btn;

extern word rand_color();
extern clock_t benchmarker;
extern double c_benchmark(char *func);
extern LIBAROMA_ZIP zip;
extern LIBAROMA_STREAMP stream_uri_callback(char * uri);
extern int MAX(int val1, int val2);
extern void winmain();
extern void clock_init_ex(LIBAROMA_WINDOWP win, int x);
extern int insidecontrol(LIBAROMA_CONTROLP ctl, int x, int y, int pager_offset);
extern void slide_brightness(int x, int offset);
extern int get_brightness();
extern void set_brightness(int value);
#ifdef __cplusplus
}
#endif

#endif //__tools_h__
