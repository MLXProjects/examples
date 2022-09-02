#include "common.h"

/* booklist onclick handler */
extern void br_bookclick(LIBAROMA_CONTROLP ctl);

/* initialize home fragment */
byte br_home_initfrag(){
	ui_whome = libaroma_ctl_fragment_new_window(mainfrag, ID_WIN_HOME);
	if (!ui_whome){
		LOGE("ui home window create failed");
		return 0;
	}
	pager = libaroma_ctl_pager(ui_whome, ID_CTL_PAGER, 2, 0, 48, ui_wmain->width, ui_wmain->height-48);
	if (!pager){
		LOGE("ui pager create failed");
		return 0;
	}
	LIBAROMA_WINDOWP pager_win = libaroma_ctl_pager_get_window(pager);
	if (!pager_win){
		LOGE("failed to get pager window");
		return 0;
	}
	lbooklist = libaroma_ctl_list(pager_win, ID_CTL_LBOOKLIST, 0, 0, LIBAROMA_SIZE_HALF, LIBAROMA_SIZE_FULL, 0, 0, (br_settings()->darkmode)?(RGB(0)):(RGB(FFFFFF)), LIBAROMA_CTL_LIST_WITH_SHADOW);
	if (!lbooklist){
		LOGE("ui left book list create failed");
		return 0;
	}
	rbooklist = libaroma_ctl_list(pager_win, ID_CTL_RBOOKLIST, LIBAROMA_POS_HALF, 0, LIBAROMA_SIZE_HALF, LIBAROMA_SIZE_FULL, 0, 0, (br_settings()->darkmode)?(RGB(0)):(RGB(FFFFFF)), LIBAROMA_CTL_LIST_WITH_SHADOW);
	if (!rbooklist){
		LOGE("ui right book list create failed");
		return 0;
	}
	libaroma_control_set_onclick(lbooklist, &br_bookclick);
	libaroma_control_set_onclick(rbooklist, &br_bookclick);
	LIBAROMA_XML book;
	int i=0;
	for (book=bible->child; book!=NULL; book=book->next){
		char *name=libaroma_xml_attr(book, "n");
		//LOGD("Found %s", name);
		LIBAROMA_CTL_LIST_ITEMP item = 
			libaroma_listitem_menu(i<39?lbooklist:rbooklist, 0, name, NULL, NULL,
				LIBAROMA_LISTITEM_MENU_SMALL|LIBAROMA_LISTITEM_MENU_INDENT_NOICON|
				LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN,
			-1);
		if (!item){
			LOGE("failed to create item #%d", i);
		}
		item->tag=book;
		i++;
	}
	chaplist = libaroma_ctl_list(ui_wchap, ID_CTL_CHAPLIST, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL, 0, 0, (br_settings()->darkmode)?(RGB(0)):(RGB(FFFFFF)), LIBAROMA_CTL_LIST_WITH_SHADOW);
	if (!chaplist){
		LOGE("ui chapter list create failed");
		return 0;
	}
	return 1;
}