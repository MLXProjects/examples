#ifndef __adraw_funcs_h__
#define __adraw_funcs_h__

#ifdef PAINT_LOG_FILELINE
/* use while loop for removing path from string */
#define logi(...) { char *logi_file=__FILE__, *logi_tmpfile=logi_file;\
					while (*logi_tmpfile!='\0') { \
						if (*logi_tmpfile=='/' || *logi_tmpfile=='\\') logi_file=logi_tmpfile+1; \
						logi_tmpfile++;\
					} \
					printf("I/%s:%d->%s(): ", logi_file, __LINE__, __func__); printf(__VA_ARGS__); printf("\n"); \
				  }
#define loge(...) { char *loge_file=__FILE__, *loge_tmpfile=loge_file;\
					while (*(loge_tmpfile++)!='\0') { \
						if (*loge_tmpfile=='/' || *loge_tmpfile=='\\') loge_file=loge_tmpfile+1; \
					} \
					printf("E/%s:%d->%s(): ", loge_file, __LINE__, __func__); printf(__VA_ARGS__); printf("\n"); \
				  }
#else
#define logi(...) { printf("I/%s(): ", __func__); printf(__VA_ARGS__); printf("\n"); }
#define loge(...) { printf("E/%s(): ", __func__); printf(__VA_ARGS__); printf("\n"); }
#endif

extern LIBAROMA_STREAMP stream_uri_cb(char * uri);
extern void adraw_home();
extern void color_onclick(LIBAROMA_CONTROLP ctl);
extern void seltool_onclick(LIBAROMA_CONTROLP ctl);
extern LIBAROMA_CANVASP adraw_toolicon(byte tool_id);
extern void adraw_cv_update(int x, int y, int w, int h, byte blend);
extern void alpha_slider_onupdate(LIBAROMA_CONTROLP ctl, int val);
extern void adraw_paint(int x, int y, word color);
extern void adraw_cv_blend();

/* stack functions */
extern ADRAW_STACKP adraw_stack(int max_size);
extern byte adraw_stack_free(ADRAW_STACKP stack);
extern byte adraw_stack_push(ADRAW_STACKP stack, int x, int y);
extern byte adraw_stack_pop(ADRAW_STACKP stack, int *x, int *y);
#define adraw_stack_top(stack) ((stack==NULL)?NULL:stack->items[stack->count-1])

#endif //__adraw_funcs_h__