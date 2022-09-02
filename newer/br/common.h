#ifndef __br_common_h__
#define __br_common_h__

/* headers */
#include <aroma.h>
#include "settings.h"

/* logging macros */
#ifdef BR_LOG
#if BR_LOG>=1
	#ifdef BR_FUNCLOG
	#define LOGE(...) { printf("[ERROR] %s(): ", __func__); printf(__VA_ARGS__); printf("\n"); }
	#else
	#define LOGE(...) { printf("[ERROR]: "); printf(__VA_ARGS__); printf("\n"); }
	#endif //BR_FUNCLOG
	#if BR_LOG>=2
		#ifdef BR_FUNCLOG
		#define LOGW(...) { printf("[WARNING] %s(): ", __func__); printf(__VA_ARGS__); printf("\n"); }
		#else
		#define LOGW(...) { printf("[WARNING]: "); printf(__VA_ARGS__); printf("\n"); }
		#endif //BR_FUNCLOG
		#if BR_LOG>=3
			#ifdef BR_FUNCLOG
			#define LOGD(...) { printf("[DEBUG] %s(): ", __func__); printf(__VA_ARGS__); printf("\n"); }
			#else
			#define LOGD(...) { printf("[DEBUG]: "); printf(__VA_ARGS__); printf("\n"); }
			#endif //BR_FUNCLOG
		#endif //BR_LOG>=3
	#endif //BR_LOG>=2
#endif //BR_LOG>=1
#endif //BR_LOG

/* public variables */
extern char *res_path;
extern LIBAROMA_ZIP res_zip;
extern LIBAROMA_XML bible, cur_book, cur_chap;
extern LIBAROMA_WINDOWP ui_wmain, ui_sidebar, ui_whome, ui_wchap, ui_wread, ui_wsettings;
extern LIBAROMA_CONTROLP appbar, mainfrag, pager, tabs, sblist, lbooklist, rbooklist, chaplist, readlist, settlist;
extern byte br_ui_deftrans_ran;
extern int br_ui_maxvnumw;
extern LIBAROMA_RECT br_ui_toprect;
extern byte appbar_icon;

/* function prototypes */
/* home */
extern byte br_home_initfrag();
/* parser */
extern byte br_verload();
extern byte br_parse(LIBAROMA_XML book);
/* ui main */
extern byte br_ui_init();
extern void br_ui_barupdate(char *text, char *subtext, byte icon);
extern byte br_ui_loop();
extern void br_ui_free();
extern void br_ui_deftrans(
	LIBAROMA_CANVASP dst,
	LIBAROMA_CANVASP bottom,
	LIBAROMA_CANVASP top,
	float state,
	LIBAROMA_RECTP r1,
	LIBAROMA_RECTP r2
);

/* id macros */
//window ids
#define ID_WIN_HOME			1
#define ID_WIN_CHAPSEL		2
#define ID_WIN_VERSREAD		3
#define ID_WIN_SETTINGS		4
#define ID_WIN_EMPTY		255
//control ids
#define ID_CTL_APPBAR		10
#define ID_CTL_MAINFRAG		11
#define ID_CTL_PAGER		12
#define ID_CTL_SBLIST		13
#define ID_CTL_LBOOKLIST	14
#define ID_CTL_RBOOKLIST	15
#define ID_CTL_CHAPLIST		16
#define ID_CTL_READLIST		17
#define ID_CTL_SETTLIST		18
//setting ids
#define ID_SETT_SLOWMO		1
#define ID_SETT_DARKMODE	2
#define ID_SETT_SCALE		3
#define ID_SETT_TEXTSIZE	4
#define ID_SETT_LOCALE		5
#define ID_SETT_VERSION		6

#endif //__br_common_h__