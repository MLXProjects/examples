#ifndef __aromafm_common_h__
#define __aromafm_common_h__
#include <aroma.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct {
	word bgcolor;
	LIBAROMA_CTL_BAR_TOOLSP bar_tools;
	LIBAROMA_CANVASP up_arrow;
	LIBAROMA_CANVASP folder_cv;
	LIBAROMA_CANVASP file_cv;
	char *cwd;
	char *dir;
	char *base;
	byte onmain_list;
	byte at_root;
	byte selecting;
	int sel_count;
	char **pathlist;
	int pathalloc;
} AROMAFM, * AROMAFMP;

typedef struct{
	LIBAROMA_CANVASP cv;	/* navbar canvas */
	LIBAROMA_CANVASP back;	/* back icon */
	LIBAROMA_CANVASP home;	/* home icon */
	LIBAROMA_CANVASP menu;	/* menu icon */
	
	/* callbacks */
	byte (*draw_cb)();
	byte (*msg_cb)(LIBAROMA_WMP wm, LIBAROMA_MSGP msg);
	
	int button_width;		/* button width */
	byte redraw;			/* redraw flag */
	byte updating;			/* updating flag */
	word accent;			/* accent color */
	
	LIBAROMA_RIPPLEP ripple;/* ripple handler */
	byte updated_btn;		/* button id being updated */
} FM_NAVBAR, * FM_NAVBARP;

#define FM_NAVBAR_BACK		0
#define FM_NAVBAR_HOME		1
#define FM_NAVBAR_MENU		2

#define aromafm_statusbar_height()	libaroma_dp(24)

/* handable file type hashes */
#define AFM_FILE_UNKNOWN	0
#define AFM_FILE_SH			21889243
#define AFM_FILE_APK		40763708
#define AFM_FILE_GIF		40698166
#define AFM_FILE_IMG		41681213
#define AFM_FILE_JAR		41025853
#define AFM_FILE_JPG		42271041
#define AFM_FILE_MP3		39452944
#define AFM_FILE_MP4		39518481
#define AFM_FILE_PNG		43188549
#define AFM_FILE_RAR		42598725
#define AFM_FILE_SVG		44826960
#define AFM_FILE_TAR		42991943
#define AFM_FILE_TXT		46137696
#define AFM_FILE_XML		44958033
#define AFM_FILE_ZIP		45089107
#define AFM_FILE_HTML		71434677
#define AFM_FILE_INFO		69796268
#define AFM_FILE_JPEG		69796262
#define AFM_FILE_PROP		73662913
#define AFM_FILE_SAPK		70910383
#define AFM_FILE_TWBAK		44368206

/* fragment ids */
#define ID_SETTFRAG			1
#define ID_FILEMFRAG		2
#define ID_FILESFRAG		3

/* control ids */
#define ID_APPBAR			1
#define ID_APPBAR_UP		10
#define ID_FRAGCTL			2
#define ID_SIDELIST			11
#define ID_SIDELIST_FOLDERS	111
#define ID_SIDELIST_FILEMGR	112
#define ID_SIDELIST_SETT	113
#define ID_SIDELIST_ABOUT	114
#define ID_SIDELIST_EXIT	115
#define ID_SETTLIST			20
#define ID_FILELIST			30

extern LIBAROMA_CONTROLP appbar, fragctl, sidelist, filelist, settlist;
extern LIBAROMA_WINDOWP winmain, sidebar, settfrag, filemfrag, filesfrag;

/* event handlers */
extern void sidelist_onclick(LIBAROMA_CONTROLP list);
extern void aromafm_list_onclick(LIBAROMA_CONTROLP list);
extern byte aromafm_list_oncheck(
	LIBAROMA_CONTROLP ctl,
	LIBAROMA_CTL_LIST_ITEMP item,
	int id,
	byte checked,
	voidp data,
	byte state
);

/* function aliases */
#define aromafm_basename(path) aromafm_basename_ex(path, 1)
#define aromafm_statusbar_reset() aromafm_statusbar_update("AROMA Filemanager")

/* functions */
extern AROMAFMP aromafm();
extern FM_NAVBARP navbar();
extern byte aromafm_goup();
extern byte aromafm_exit();
extern byte aromafm_atroot();
extern byte mlx_navbar_init();
extern void mlx_navbar_animate();
extern byte aromafm_isdir(char *path);
extern byte aromafm_init(word bgcolor);
extern byte aromafm_loadpath(char *path);
extern void aromafm_statusbar_update(char *text);
extern unsigned long aromafm_gettype(char *text);
extern void aromafm_navbar_onhold(byte button_id);
extern void aromafm_navbar_onclick(byte button_id);
extern int aui_fetch(char * path, LIBAROMA_CONTROLP list);
extern char *aromafm_basename_ex(char *path, byte skip_last_slash);
extern byte aromafm_bar_update(char *title, char *subtitle, byte arrowflag);

#endif /* __aromafm_common_h__ */

