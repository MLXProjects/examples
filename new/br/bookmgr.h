#ifndef __br_main_h__
	#error "please include main.h instead of this"
#endif
#ifndef __br_bookmgr_h__
#define __br_bookmgr_h__

extern LIBAROMA_XML bible[];
extern int curbook, curchap;
extern int vnum_width;

extern byte bible_init();
extern byte chapter_load(LIBAROMA_XML book);

#endif /* __br_bookmgr_h__ */