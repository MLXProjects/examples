#ifndef __br_main_h__
	#error "please include main.h instead of this"
#endif
#ifndef __br_main_h__
	#error "please include main.h instead of this"
#endif
#ifndef __br_funcs_h__
#define __br_funcs_h__
/* switch fragment macro */
/*#define br_switch_window(id, anim, rc1, rc2) \
			libaroma_ctl_fragment_set_active_window(fragment, id, 1, settings()->anim_duration, 0, anim, rc1, rc2)*/
/* set winpool to NULL to exit main loop */
#define br_exit()	mainwin->onpool=0;

/* URI HANDLER */
extern LIBAROMA_STREAMP stream_uri_cb(char * uri);

/* INIT FUNCTIONS */
extern byte win_init();
extern byte sidebar_init();
extern byte read_init();

/* BR_PAGE MANAGEMENT */
extern BR_PAGE_STACKP br();
extern byte br_page_new(int id, void *onstart, void *pool, void *onexit, void *onshow);
extern byte br_page_switch(int id);
extern byte br_page_goback();
extern void *br_page_get_pdata(int id);
extern byte br_page_set_pdata(int id, void *pdata);
#define br_page_resetanim(id) br_page_setanim(id, NULL, NULL, NULL)
extern byte br_page_setanim(int id, void *anim_cb, LIBAROMA_RECTP in_rect, LIBAROMA_RECTP out_rect);

/* BR_HISTORY management */
extern LIBAROMA_STACKP br_history;
#define br_history_first() ((BR_PAGEP) br_history->first->val)
#define br_history_last() ((BR_PAGEP) br_history->last->val)
#define br_history_prev() ((BR_PAGEP)(libaroma_stack_get(br_history, br_history->n-2)))
//#define br_page_goback() br_page_switch(br_history_prev());
/*extern voidp br_history_find(int id, byte ret_page);
extern byte br_history_add(int id);
extern byte br_history_del(int id);
extern byte br_page_switch(int id, byte del_prev);*/

/*extern BR_PAGE_STACKP br_history();
#define br_history_first() br_history()->pages[0]
#define br_history_prev() br_history()->pages[br_history()->pagen-2]
#define br_history_last() br_history()->pages[br_history()->pagen-1]
extern byte br_history_add(int id);
extern byte br_history_del(int id);
extern byte br_page_switch(int id);
extern byte br_page_goback();
extern int br_history_find(int id);*/

/* FRAGMENT ANIMATION SWITCH */
extern void fragani_default(
  LIBAROMA_CANVASP dst,
  LIBAROMA_CANVASP bottom,
  LIBAROMA_CANVASP top,
  float state,
  LIBAROMA_RECTP r1,
  LIBAROMA_RECTP r2
);

extern word custcolor_handler(const char *tag);

#endif /* __br_funcs_h__ */