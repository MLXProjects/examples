#ifndef __tools_h__
#define __tools_h__

#include <aroma.h>
#include <time.h>

#ifdef __cplusplus
extern "C"{
#endif

/*
 * THREADS
 */
typedef pthread_t LIBAROMA_THREAD;
#define libaroma_thread_create(th,cb,cookie) \
	pthread_create(th,NULL,cb,cookie)
/* recommended */
#define libaroma_thread_join(th) pthread_join(th,NULL)
/* optional */
#define libaroma_thread_detach(th) 			pthread_detach(th)
#define libaroma_thread_kill(th) 			pthread_kill(th,0)
#define libaroma_thread_kill_ex(th, sig) 	pthread_kill(th, sig)
#define libaroma_thread_cancel(th)			pthread_cancel(th)

#define ALOG(...)	printf("MLX: "); \
					printf(__VA_ARGS__); \
					printf("\n")

extern LIBAROMA_ZIP zip;
extern LIBAROMA_STREAMP stream_uri_callback(char * uri);
extern int win_x;
extern int win_y;
extern void bg_update(LIBAROMA_WINDOWP win, LIBAROMA_CANVASP cv);
extern void winmain();
extern byte libaroma_draw_skew(
	LIBAROMA_CANVASP dst, LIBAROMA_CANVASP src,
	int x1, int y1, int x2, int y2,
	int x3, int y3, int x4, int y4
);
#ifdef __cplusplus
}
#endif

#endif //__tools_h__
