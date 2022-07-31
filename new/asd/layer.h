#ifndef __layer_h__
#define __layer_h__

#include "asd.h"

typedef struct {
	LIBAROMA_WINDOWP win;
	byte (*msg_hook)(LIBAROMA_WINDOWP, LIBAROMA_MSGP, dwordp);
	byte isvisible;
	LIBAROMA_MUTEX mutex;
	//generic user data pointer
	void *tag;
} MYLAYER, *MYLAYERP;

typedef struct {
	LIBAROMA_WINDOWP win;
	MYLAYERP *layers;
	int layern;
	byte direct_cv;
	LIBAROMA_CANVASP tdc;
	LIBAROMA_CANVASP wmc;
	byte (*ori_ui_thread)(LIBAROMA_WINDOWP);
	
	byte sync;
	LIBAROMA_MUTEX mutex;
} MYLAYER_MANAGER, *MYLAYER_MANAGERP;

extern byte mylayermgr_init(LIBAROMA_WINDOWP win);

extern void mylayermgr_free(LIBAROMA_WINDOWP win);

LIBAROMA_WINDOWP mylayer_new(LIBAROMA_WINDOWP win);

extern void mylayer_del(LIBAROMA_WINDOWP laywin);

extern byte mylayer_hideshow(LIBAROMA_WINDOWP win, byte show);
#define mylayer_hide(win) mylayer_hideshow(win, 0)
#define mylayer_show(win) mylayer_hideshow(win, 1)

#endif /*__layer_h__*/