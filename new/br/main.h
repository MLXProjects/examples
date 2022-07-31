#ifndef __br_main_h__
#define __br_main_h__

#include <aroma.h>

typedef struct {
	int id;
	void *anim_cb;
	LIBAROMA_RECTP inrect;
	LIBAROMA_RECTP outrect;
	void (*onstart)();
	void (*pool)();
	void (*onexit)();
	void (*onshow)();
	void *pdata; /* page data */
	//int index;
	
} BR_PAGE, * BR_PAGEP;

typedef struct {
	BR_PAGEP *pages;
	int pagen;
} BR_PAGE_STACK, * BR_PAGE_STACKP;

#include "defs.h"
#include "funcs.h"
#include "bookmgr.h"
#include "settings.h"
#include "favs.h"

/* VARIABLES */
extern BR_PAGEP *pages;
extern LIBAROMA_ZIP zip;
extern LIBAROMA_WINDOWP mainwin, mainsb, fragmain, fragpagerwin, fragread, fragsett, fragfavs;
extern LIBAROMA_CONTROLP appbar, fragment, fragtabs, fragpager, mainmenu, othermenu, readlist, sblist, sblogo, settlist, favlist;
extern LIBAROMA_CTL_BAR_TOOLSP bar_tools;
extern byte load_status;
extern int pager_change;
extern LIBAROMA_XML xml_bible;

#endif /* __br_main_h__ */
