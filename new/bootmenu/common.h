#ifndef __bootmenu_common_h__
#define __bootmenu_common_h__
#include <aroma.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <linux/reboot.h>
#include <sys/syscall.h>

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
} BOOTMENU_FM, * BOOTMENU_FMP;

/* fragment ids */
#define ID_HOMEFRAG			1
#define ID_SETTFRAG			2
#define ID_FILEMFRAG		3
#define ID_FILESFRAG		4

/* control ids */
#define ID_APPBAR			1
#define ID_APPBAR_UP		10
#define ID_FRAGCTL			2
#define ID_HOMELIST			11
#define ID_HOMELIST_DROID	110
#define ID_HOMELIST_RECV	111
#define ID_HOMELIST_FILEMGR	112
#define ID_HOMELIST_SETT	113
#define ID_HOMELIST_REBOOT	114
#define ID_HOMELIST_PWOFF	115
#define ID_HOMELIST_EXIT	116
#define ID_HOMELIST_AFM		117
#define ID_SETTLIST			20
#define ID_FILELIST			30

extern LIBAROMA_WINDOWP winmain, homefrag, settfrag, filemfrag, filesfrag;
extern LIBAROMA_CONTROLP appbar, fragctl, homelist, filelist, settlist;
extern byte bootmenu_haltreboot;

extern BOOTMENU_FMP bootmenu_fm();
extern byte bootmenu_filemgr_init(word bgcolor);
extern byte bootmenu_filemgr_loadpath(char *path);
extern byte bootmenu_filemgr_goup();
extern byte bootmenu_filemgr_exit();
extern int aui_fetch(char * path, LIBAROMA_CONTROLP list);
extern byte bootmenu_bar_update(char *title, char *subtitle, byte arrowflag);

#endif /* __bootmenu_common_h__ */
