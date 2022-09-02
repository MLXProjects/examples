#include "common.h"

byte br_verload(){
	char path[64];
	snprintf(path, 64, "res:///xml/%s.xml", br_settings()->version);
	LIBAROMA_STREAMP xml_stream = libaroma_stream(path);
	if (!xml_stream){
		LOGE("failed to load path %s", path);
		return 0;
	}
	int stream_len=xml_stream->size-1;
	char *stream_string = libaroma_stream_to_string(xml_stream, 1);
	if (!stream_string){
		LOGE("failed to get contents of %s", path);
		return 0;
	}
	LIBAROMA_XML loaded = libaroma_xml(stream_string, stream_len);
	if (!loaded){
		LOGE("XML parse failed for %s", path);
		return 0;
	}
	LIBAROMA_XML oldbible=bible;
	bible = loaded;
	if (oldbible!=NULL) libaroma_xml_free(oldbible);
	return 1;
}

void br_ui_barupdate(char *text, char *subtext, byte icon){
	if (appbar_icon!=icon){
		if (icon==LIBAROMA_CTL_BAR_ICON_DRAWER_TO_ARROW)
			appbar_icon=LIBAROMA_CTL_BAR_ICON_ARROW;
		else appbar_icon=icon;
		libaroma_ctl_bar_set_icon(appbar, NULL, 0, icon, 0);
	}
	libaroma_ctl_bar_set_title(appbar, text, 0);
	libaroma_ctl_bar_set_subtitle(appbar, subtext, 0);
	libaroma_ctl_bar_update(appbar);
}

void br_chapclick(LIBAROMA_CONTROLP ctl){
	LIBAROMA_CTL_LIST_ITEMP item = libaroma_ctl_list_get_touched_item(ctl);
	if (!item) return;
	LIBAROMA_XML chapter = (LIBAROMA_XML) item->tag;
	/* initialize versicle reader window */
	ui_wread = libaroma_ctl_fragment_new_window(mainfrag, ID_WIN_VERSREAD);
	if (!ui_wread){
		LOGE("ui read window create failed");
		return;
	}
	readlist = libaroma_ctl_list(ui_wread, ID_CTL_READLIST, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL, 0, 0, (br_settings()->darkmode)?(RGB(0)):(RGB(FFFFFF)), LIBAROMA_CTL_LIST_WITH_HANDLE|LIBAROMA_CTL_LIST_WITH_SHADOW);
	if (!readlist){
		LOGE("ui read list create failed");
		return;
	}
	//libaroma_control_set_onclick(readlist, &br_versclick);
	LIBAROMA_XML vers;
	int i=1;
	char vnum_str[8];
	for (vers=chapter->child; vers!=NULL; vers=vers->next){
		snprintf(vnum_str, 8, "%i", i);
		LIBAROMA_TEXT vers_text = libaroma_text(vers->txt, 0, (ui_wchap->w-br_ui_maxvnumw)-libaroma_dp(4), LIBAROMA_FONT(0,br_settings()->textsize), 110);
		LIBAROMA_CANVASP vers_cv=libaroma_canvas(ui_wchap->w, libaroma_text_height(vers_text)+libaroma_dp(4));
		if (!vers_cv){
			LOGE("failed to create item canvas for %d", i);
			return;
		}
		LOGD("create item canvas for %d (%dx%d)", i, vers_cv->w, vers_cv->h);
		libaroma_canvas_setcolor(vers_cv, RGB(FFFFFF), 0xFF);
		libaroma_draw_text(vers_cv, vnum_str, 0, 0, RGB(0), br_ui_maxvnumw, LIBAROMA_TEXT_BOLD|LIBAROMA_TEXT_RIGHT|LIBAROMA_TEXT_SINGLELINE|LIBAROMA_FONT(0,br_settings()->textsize), 110);
		libaroma_text_draw(vers_cv, vers_text, br_ui_maxvnumw+libaroma_dp(4), 0);
		LIBAROMA_CTL_LIST_ITEMP item = 
			libaroma_listitem_image(readlist, 0, vers_cv, vers_cv->h,
				LIBAROMA_LISTITEM_IMAGE_FREE|LIBAROMA_CTL_LIST_ITEM_RECEIVE_TOUCH,
			-1);
		if (!item){
			LOGE("failed to create item %i", i);
			return;
		}
		item->tag=vers;
		i++;
	}
	LIBAROMA_RECT top_rect={0, 0, 0, 0};
	char text_title[64];
	snprintf(text_title, 64, "%s %s", libaroma_xml_attr(cur_book, "n"), libaroma_xml_attr(chapter, "n"));
	cur_chap=chapter;
	br_ui_barupdate(text_title, NULL, LIBAROMA_CTL_BAR_ICON_ARROW);
	libaroma_ctl_fragment_set_active_window(mainfrag, ID_WIN_VERSREAD, 0, br_settings()->slowmo?4000:br_settings()->anitime, 0, &br_ui_deftrans, NULL, &br_ui_toprect);
}

void br_bookclick(LIBAROMA_CONTROLP ctl){
	LIBAROMA_CTL_LIST_ITEMP item = libaroma_ctl_list_get_touched_item(ctl);
	if (!item) return;
	LIBAROMA_XML book = (LIBAROMA_XML) item->tag;
	/* initialize chapter select window */
	ui_wchap = libaroma_ctl_fragment_new_window(mainfrag, ID_WIN_CHAPSEL);
	if (!ui_wchap){
		LOGE("ui chapter window create failed");
		return;
	}
	chaplist = libaroma_ctl_list(ui_wchap, ID_CTL_CHAPLIST, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL, 0, 0, (br_settings()->darkmode)?(RGB(0)):(RGB(FFFFFF)), LIBAROMA_CTL_LIST_WITH_HANDLE|LIBAROMA_CTL_LIST_WITH_SHADOW);
	if (!chaplist){
		LOGE("ui chapter list create failed");
		return;
	}
	libaroma_control_set_onclick(chaplist, &br_chapclick);
	LIBAROMA_XML chapter;
	int i=1;
	char num[16];
	for (chapter=book->child; chapter!=NULL; chapter=chapter->next){
		sprintf(num, "%i", i);
		LIBAROMA_CTL_LIST_ITEMP item = 
			libaroma_listitem_menu(chaplist, 0, num, NULL, NULL,
				LIBAROMA_LISTITEM_MENU_SMALL|LIBAROMA_LISTITEM_MENU_INDENT_NOICON|
				LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN,
			-1);
		if (!item){
			LOGE("failed to create item %i", i);
			return;
		}
		item->tag=chapter;
		i++;
	}
	br_ui_barupdate(libaroma_xml_attr(book, "n"), NULL, LIBAROMA_CTL_BAR_ICON_DRAWER_TO_ARROW);
	cur_book=book;
	libaroma_ctl_fragment_set_active_window(mainfrag, ID_WIN_CHAPSEL, 0, br_settings()->slowmo?4000:br_settings()->anitime, 0, &br_ui_deftrans, NULL, &br_ui_toprect);
}