#ifndef __aromafm_home_c__
#define __aromafm_home_c__
#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

byte mlx_home_init(word bgcolor){
	homelist = libaroma_ctl_list(homefrag, 0, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL, 0, 0, bgcolor, LIBAROMA_CTL_LIST_WITH_SHADOW);
	if (!homelist) return 0;
	word hflags=LIBAROMA_LISTITEM_MENU_INDENT_NOICON|LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN;
	libaroma_listitem_menu(homelist, ID_HOMELIST_FM, "File manager", "Browse files :D", NULL, hflags, -1);
	libaroma_listitem_menu(homelist, ID_HOMELIST_DM, "Kernel logs", "a.k.a. dmesg", NULL, hflags, -1);
	libaroma_listitem_menu(homelist, ID_HOMELIST_EVT, "Events tester", "Live /dev/input manager", NULL, hflags, -1);
	libaroma_listitem_menu(homelist, ID_HOMELIST_SH, "Shell 🙂", "If it has bugs, write your own lol", NULL, hflags, -1);
	libaroma_listitem_menu(homelist, ID_HOMELIST_RB, "Reboot", NULL, NULL, hflags, -1);
	libaroma_listitem_menu(homelist, ID_HOMELIST_SHD, "Shutdown", NULL, NULL, hflags, -1);
	libaroma_listitem_menu(homelist, ID_HOMELIST_BYE, "Exit", NULL, NULL, hflags, -1);
	libaroma_control_set_onclick(homelist, &homelist_onclick);
	return 1;
}

#ifdef __cplusplus
}
#endif
#endif /* __aromafm_home_c__ */  
  
