#ifndef __br_main_h__
	#error "please include main.h instead of this"
#endif
#ifndef __br_defs_h__
#define __br_defs_h__

/* WINDOW/CONTROL IDS */
//main window IDs
#define ID_APPBAR			1
#define ID_APPBAR_TITLE		2
#define ID_APPBAR_FIRST		3
#define ID_APPBAR_SECOND	4
#define ID_FRAGMENT			10
//main fragment window IDs
#define ID_FRAGMAIN			20
#define ID_FRAGMAIN_TABS	21
#define ID_FRAGMAIN_PAGER	22
#define ID_FRAGMAIN_LIST	23
//sidebar window IDs
#define ID_SIDEBAR_LOGO		30
#define ID_SIDEBAR_LIST		31
#define ID_SIDEBAR_TAB1		32
#define ID_SIDEBAR_TAB2		33
#define ID_SIDEBAR_FAVS		34
#define ID_SIDEBAR_SETTINGS	35
#define ID_SIDEBAR_ABOUT	36
//reader fragment window IDs
#define ID_FRAGREAD			40
#define ID_FRAGREAD_LIST	41
//settings window IDs
#define ID_SETTINGS			50
#define ID_SETTINGS_LIST	51
#define ID_SETTINGS_DARK	52
#define ID_SETTINGS_FONT	53
#define ID_SETTINGS_FONTSZ	54
#define ID_SETTINGS_SLOWCAM	55
#define ID_SETTINGS_SCALE	56
//about window IDs
#define ID_FAVORITES		60
#define ID_FAVORITES_LIST	61
//about window IDs
#define ID_ABOUT			70
#define ID_ABOUT_VER		71

/* FRAGMENT ANIMATION FLAGS */
#define FRAGANI_SHOW		0x1
#define FRAGANI_SCALE		0x2
#define FRAGANI_NOSLIDE		0x4

#endif /* __br_defs_h__ */


