#ifndef __test_common_h__
#define __test_common_h__

#include <aroma.h>

/* global variables */
extern LIBAROMA_ZIP zip;
extern int active_fragment;
extern LIBAROMA_CANVASP ab_icon;
extern LIBAROMA_CTL_BAR_TOOLSP bar_tools;
extern LIBAROMA_CONTROLP appbar, fragment, pager, test_list, cprog, hprog;

/* global functions */
extern byte test();
extern byte test_init(LIBAROMA_WINDOWP win);
extern byte test_switch_window(int id, byte isback, LIBAROMA_RECTP rect);
extern byte test_xml_layout_parse(LIBAROMA_WINDOWP win);

/* control IDs */
#define ID_APPBAR				1
#define ID_SIDEBAR				2
#define ID_SIDE_				2
#define ID_FRAGMENT				3
#define ID_FRAGMENT_MAIN		30
#define ID_FRAGMENT_SETT		31
#define ID_FRAGMENT_BTNS		32
#define ID_FRAGMENT_PROG		33
#define ID_FRAGMENT_XML			34
#define ID_FRAGMENT_DBG			35
#define ID_FRAGMENT_LST			36
#define ID_TABS					4
#define ID_PAGER				5
#define ID_MAIN_LIST			60
#define ID_MAIN_LISTITEM_PSET	600
#define ID_MAIN_LISTITEM_SETT	601
#define ID_MAIN_LISTITEM_BTNS	602
#define ID_MAIN_LISTITEM_PROG	603
#define ID_MAIN_LISTITEM_XML	604
#define ID_MAIN_LISTITEM_EXIT	605
#define ID_MAIN_LISTITEM_DBG	606
#define ID_MAIN_LISTITEM_LST	607
#define ID_PSET_LIST			610
#define ID_PSET_LISTITEM_SLIDE	610
#define ID_PSET_LISTITEM_STRCH	611
#define ID_PSET_LISTITEM_CLEAN	612
#define ID_PSET_LISTITEM_RVEAL	613
#define ID_PSET_LISTITEM_SCALE	614
#define ID_PSET_LISTITEM_STKIN	615
#define ID_PSET_LISTITEM_STKOV	616
#define ID_PSET_LISTITEM_CIRCL	617
#define ID_PSET_LISTITEM_FADE	618
#define ID_PSET_LISTITEM_STRIN	619
#define ID_LSET_LIST			620
#define ID_LSET_LISTITEM_HANDLE	620
#define ID_LSET_LISTITEM_SHADOW	621
#define ID_LSET_LISTITEM_GLOW	622
#define ID__LIST				6
#define ID__LISTITEM_			6
#define ID_SETT_TOGGLE			7
#define ID_BTN_					8
#define ID_PROG_HORIZ			90
#define ID_PROG_CIRCLE			91
#define ID_PROG_HINV			92
#define ID_PROG_CINV			93
#define ID_PROG_HUPD			94
#define ID_PROG_CUPD			95
#define ID_XML_					0

/* app return codes */
#define TEST_RET_OK				0 /* nothing failed, nice :D */
#define TEST_RET_ZIP			1 /* failed to open zip file */
#define TEST_RET_AROMA			2 /* libaroma failed to init */
#define TEST_RET_FONT			3 /* 1st font failed to load */
#define TEST_RET_SETT			4 /* settings failed to load */
#define TEST_RET_IDK			9 /* something else happened */

/* config manager */
typedef struct{
	word appbar_bg;
	word appbar_fg;
	bytep rgb_custom;
	byte fragani_scale;
} TEST_CONFIG, * TEST_CONFIGP;

TEST_CONFIGP test_config();

/* fragment transition callback */
void fragani_default(
	LIBAROMA_CANVASP dst,
	LIBAROMA_CANVASP bottom,
	LIBAROMA_CANVASP top,
	float state,
	LIBAROMA_RECTP r1,
	LIBAROMA_RECTP r2
);

#endif /* __test_common_h__ */
