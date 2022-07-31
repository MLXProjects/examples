#ifndef __ptero_common_h__
#define __ptero_common_h__

#include <aroma.h>
#include "defines.h"
/*
typedef struct {
	char *name;
	LIBAROMA_SARRAYP color_aliases;
	LIBAROMA_COLORSETP colorset;
} PTERO_THEME, * PTERO_THEMEP;*/

typedef struct {
	LIBAROMA_XML onclick;	/* control onclick actionset */
	LIBAROMA_XML onhold;	/* control onhold actionset */
	char *id;				/* control id string */
	int itemn;				/* child id/window count */
	char **win_ids;			/* child id list */
	LIBAROMA_WINDOWP *wins;	/* child window list */
} PTERO_CONTROL, * PTERO_CONTROLP;

typedef struct {
	byte running;
	
	/* main ui */
	LIBAROMA_WINDOWP winmain;
	LIBAROMA_WINDOWP sidebar;/*
	LIBAROMA_COLORSETP colorset;
	LIBAROMA_COLORSETP colorset_original;*/
	word global_fill_icons;
	//PTERO_THEMEP theme;
	
	/* xml variables storage */
	LIBAROMA_SARRAYP var_stor;
} PTERODON, * PTERODONP;

/* global variables */
extern LIBAROMA_STREAMP layout_stream;
//extern LIBAROMA_STREAMP settings_stream;
extern LIBAROMA_XML layout_xml;
extern LIBAROMA_ZIP global_zip;
//extern ;

/* global functions */
extern PTERODONP pterodon();
extern LIBAROMA_STREAMP ptero_stream_uri_cb(char *uri);
extern byte ptero_init(int argc, char **argv);
extern byte ptero_main();
extern byte ptero_gui_main();
extern byte ptero_colorset_parse(LIBAROMA_XML colorset_tag);
extern byte ptero_layout_init();
extern byte ptero_layout_parse(LIBAROMA_WINDOWP win, LIBAROMA_XML parsed_layout, int x_off, int max_w);
extern char *ptero_variable_get(char *name);
extern byte ptero_variable_set(char *name, char *value);
extern byte ptero_exit();
extern LIBAROMA_CONTROLP ptero_find_ctl_by_id(LIBAROMA_WINDOWP win, char *id);
extern void ptero_ctl_click(LIBAROMA_CONTROLP ctl);
extern void ptero_ctl_hold(LIBAROMA_CONTROLP ctl);
//extern byte ptero_settings_init();

#endif /* __ptero_common_h__ */
