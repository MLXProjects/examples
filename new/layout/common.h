#ifndef __layout_common_h__
#define __layout_common_h__

#define LOGI(...) printf("I/%s(): ", __func__); printf(__VA_ARGS__); printf("\n");

#include <aroma.h>
#include "defines.h"

typedef struct {
	LIBAROMA_WINDOWP wins[64];
	char *ids[64];
	int winn;
} LAYOUT_WINLIST, * LAYOUT_WINLISTP;

/* global variables */
extern LIBAROMA_ZIP zip;

/* gui */
extern byte layout_gui_init();
extern byte layout_gui_cleanup();

/* callbacks */
extern void *layout_callback_get(char *tag);
extern byte layout_callback_set(char *tag, void *cb);

/* parser */
extern byte layout_parse_ex(char *layout, int len);

/* wins */
extern byte layout_wins_add(LIBAROMA_WINDOWP win, char *id);
extern byte layout_wins_delindex(int index, byte free);
#define layout_wins_del(win, id, free) layout_wins_delindex(layout_wins_getindex(win, id), free)
extern LIBAROMA_WINDOWP layout_wins_get(char *id);
extern int layout_wins_getindex(LIBAROMA_WINDOWP win, char *id);

#endif /*__layout_common_h__*/