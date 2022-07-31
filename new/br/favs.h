#ifndef __br_favs_h__
#define __br_favs_h__

typedef struct {
	int bindex; /* book index */
	int cindex; /* chapter index */
	byte saved;
	byte selected;
	LIBAROMA_XML versicle;
	LIBAROMA_CANVASP orig_cv;
} SAVED_VERS, * SAVED_VERSP;

extern LIBAROMA_STACKP favorites;

extern byte favs_init();
extern void favs_add(SAVED_VERSP vers);
extern void favs_del(SAVED_VERSP vers);
extern byte favs_clean();
extern void favs_show();
extern byte vers_selmode_handler(LIBAROMA_CTL_LIST_ITEMP item, byte holding, char *title, char *subtitle);
extern void versicles_handle_selected(byte save, byte uncheck);

#endif /*__br_favs_h__*/