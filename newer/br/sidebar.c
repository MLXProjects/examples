#include "common.h"

LIBAROMA_WINDOWP ui_sidebar;
LIBAROMA_CONTROLP sblist;

void br_sblist_onclick(LIBAROMA_CONTROLP ctl){
	LIBAROMA_CTL_LIST_ITEMP item = libaroma_ctl_list_get_touched_item(sblist);
	if (!item) return;
	switch (item->id){
		case ID_CTL_LBOOKLIST:{
			br_ui_barupdate(BR_APPNAME, NULL, LIBAROMA_CTL_BAR_ICON_ARROW_TO_DRAWER);
			libaroma_ctl_fragment_set_active_window(mainfrag, ID_WIN_HOME, 0, 100, 0, NULL, NULL, NULL);
			libaroma_sleep(16);
			libaroma_ctl_pager_set_active_page(pager, 0, 0.0);
		} break;
		case ID_CTL_RBOOKLIST:{
			br_ui_barupdate(BR_APPNAME, NULL, LIBAROMA_CTL_BAR_ICON_ARROW_TO_DRAWER);
			libaroma_ctl_fragment_set_active_window(mainfrag, ID_WIN_HOME, 0, 100, 0, NULL, NULL, NULL);
			libaroma_sleep(16);
			libaroma_ctl_pager_set_active_page(pager, 1, 0.0);
		} break;
		case ID_CTL_SETTLIST:{
			br_ui_barupdate(libaroma_lang_get("ui.settings"), NULL, LIBAROMA_CTL_BAR_ICON_DRAWER_TO_ARROW);
			libaroma_ctl_fragment_set_active_window(mainfrag, ID_WIN_SETTINGS, 0, br_settings()->slowmo?4000:(br_settings()->anitime/2), 0, NULL, NULL, NULL);
		} break;
	}
	libaroma_window_sidebar_show(ui_sidebar, 0);
}

byte br_sidebar_init(){
	
	ui_sidebar = libaroma_window_sidebar(ui_wmain, 0);
	if (!ui_sidebar){
		LOGE("ui sidebar create failed");
		return 0;
	}
	sblist = libaroma_ctl_list(ui_sidebar, ID_CTL_SBLIST, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL, 0, 0, RGB(FFFFFF), LIBAROMA_CTL_LIST_WITH_SHADOW);
	if (!sblist){
		LOGE("ui sidebar list create failed");
		return 0;
	}
	libaroma_listitem_menu(sblist, ID_CTL_LBOOKLIST, libaroma_lang_get("ui.ot"), NULL, NULL,
		LIBAROMA_LISTITEM_MENU_SMALL/*|LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN*/,
	-1);
	libaroma_listitem_menu(sblist, ID_CTL_RBOOKLIST, libaroma_lang_get("ui.nt"), NULL, NULL,
		LIBAROMA_LISTITEM_MENU_SMALL/*|LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN*/,
	-1);
	libaroma_listitem_menu(sblist, ID_CTL_SETTLIST, libaroma_lang_get("ui.settings"), NULL, NULL,
		LIBAROMA_LISTITEM_MENU_SMALL/*|LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN*/,
	-1);
	libaroma_control_set_onclick(sblist, &br_sblist_onclick);
	return 1;
}