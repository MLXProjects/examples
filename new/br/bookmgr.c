#ifndef __br_bookmgr_c__
#define __br_bookmgr_c__

#include "main.h"

LIBAROMA_XML xml_bible;
LIBAROMA_XML bible[66];

extern LIBAROMA_CONTROLP mainmenu, othermenu;
int curbook, curchap, vnum_width;
static int chapter_len;
static LIBAROMA_RECTP item_rect;

#ifndef _WIN32
extern char *_find_xml_encoding(char *xml_file, size_t max_size);
extern size_t _iso8859_1_to_utf8(char *content, size_t max_size);
#endif /* _WIN32 */
byte bible_init(){
	curbook=0;
	curchap=0;
	
	LIBAROMA_STREAMP xmlfile = libaroma_stream_mzip(zip, "xml/KJV.xml");
	#ifndef _WIN32 // latin->unicode conversion seems broken on Winbugs, let's hope it's on a Freetype renderizable encoding
	char *encoding = _find_xml_encoding(xmlfile->data, xmlfile->size);
	if (encoding != NULL){
		if (0==strcmp(encoding, "iso-8859-1")){
			size_t reqsize = _iso8859_1_to_utf8(xmlfile->data, xmlfile->size);
			if (reqsize>xmlfile->size) {
				xmlfile->size=reqsize;
			}
			else if (reqsize<0) return NULL;
		}
		free(encoding);
	}
	#endif /* _WIN32 */
	xml_bible = libaroma_xml(xmlfile->data, xmlfile->size);
	if (!xml_bible) return 0;
	int i=0;
	
	#define _ITEM(text,ico,extra) \
		libaroma_listitem_menu( \
			(i<39)?mainmenu:othermenu, 1, text, extra, NULL, LIBAROMA_LISTITEM_MENU_INDENT_NOICON| \
			LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN,-1)
	LIBAROMA_XML book, chap, vers;
	for (book=libaroma_xml_child(xml_bible, "b"); book; book=libaroma_xml_next(book)){
		bible[i]=book;
		LIBAROMA_CTL_LIST_ITEMP item;
		item=_ITEM(libaroma_xml_attr(book, "n"), NULL, NULL);
		if (!item){
			printf("win_init: failed to create list item %d\n", i);
			return 0;
		}
		item->tag = (voidp) book; //save book pointer as item tag
		i++;
	} 
	#undef _ITEM
	LIBAROMA_TEXT vcount_sample; /* used to measure four-char rendered text width */
	vcount_sample = libaroma_text("BBB", RGB(0), libaroma_fb()->w, 
									LIBAROMA_FONT(0, 4)|LIBAROMA_TEXT_SINGLELINE|LIBAROMA_TEXT_BOLD|LIBAROMA_TEXT_RIGHT, 0);
	if (vcount_sample==NULL) {
		printf("bible_init: Cannot create sample text\n");
		return 0;
	}
	vnum_width = libaroma_text_width(vcount_sample);
	libaroma_text_free(vcount_sample);
	return 1;
}

static char *appbar_title, *appbar_subtitle;

void _reader_onstart(){
	chapter_load(bible[curbook]);
	chapter_len = strlen(libaroma_lang_get("chapter"))+4;
	appbar_subtitle = malloc(chapter_len);
}

void _reader_onshow(){
	appbar_title = libaroma_xml_attr(bible[curbook], "n");
	snprintf(appbar_subtitle, chapter_len, "%s 1", libaroma_lang_get("chapter"));
	main_appbar_update(appbar_title, appbar_subtitle, 1);
}

void _reader_pool(){
	LIBAROMA_MSG msg;
	dword command	=libaroma_window_pool(mainwin,&msg);
	byte cmd		=LIBAROMA_CMD(command);
	word id			=LIBAROMA_CMD_ID(command);
	byte param		=LIBAROMA_CMD_PARAM(command);
	if (cmd==LIBAROMA_CMD_CLICK){
		switch (id){
			case ID_APPBAR:{
				if (param==1) {
					//BR_PAGEP page = br_history_last(); /* get current page */
					//br_page_setanim(br_history_prev()->id, page->anim_cb, page->inrect, page->outrect); /*invert rect to make close anim*/
					br_page_goback();
					//br_page_resetanim(br_history_last()); /* previous page is last after goback */
				}
				else if (param==2){ /* show chapter selector */
					/* maybe custom layout? idk */
					/* set title to just book name while selecting chapter */
				} 
				else if (param==10){ /* discard & uncheck all selected versicles */
					/*versicles_handle_selected(0, 1);
					main_appbar_update(appbar_title, appbar_subtitle, 1);*/
				}
				else if (param==11){ /* save all selected versicles */
					/*versicles_handle_selected(1, 0);
					main_appbar_update(appbar_title, appbar_subtitle, 1);*/
				}
			} break;
			case ID_FRAGREAD_LIST:{
				vers_selmode_handler((LIBAROMA_CTL_LIST_ITEMP) msg.d, 0, appbar_title, appbar_subtitle);
			} break;
			case ID_SIDEBAR_LIST:{
				LIBAROMA_CTL_LIST_ITEMP item = libaroma_ctl_list_get_touched_item(sblist);
				if (item){
					libaroma_window_sidebar_show(mainsb, 0);
					switch (item->id){
						case ID_SIDEBAR_TAB1:{
							pager_change=0;
							//BR_PAGEP page = br_history_first(); /* first page is always home */
							//br_page_setanim(page->id, page->anim_cb, page->outrect, page->inrect); /*invert rect to make close anim*/
							br_page_switch(ID_FRAGMAIN);
							//br_page_resetanim(page->id);
						} break;
						case ID_SIDEBAR_TAB2:{
							pager_change=1;
							//BR_PAGEP page = br_history_first(); /* first page is always home */
							//br_page_setanim(ID_FRAGMAIN, page->anim_cb, page->outrect, page->inrect); /*invert rect to make close anim*/
							br_page_switch(ID_FRAGMAIN);
							//br_page_resetanim(ID_FRAGMAIN);
						} break;
						case ID_SIDEBAR_FAVS:{
							br_page_switch(ID_FAVORITES);
						} break;
						case ID_SIDEBAR_SETTINGS:{
							br_page_switch(ID_SETTINGS);
						} break;
					}
				}
			} break;
		}
	}
	else if (cmd==LIBAROMA_CMD_HOLD){
		if (id==ID_FRAGREAD_LIST){
			//handle versicle hold
			vers_selmode_handler((LIBAROMA_CTL_LIST_ITEMP) msg.d, 1, appbar_title, appbar_subtitle);
		}
	}
	
}

void _reader_onexit(){
	free(appbar_subtitle);
	LIBAROMA_CTL_LIST_ITEMP itm;
	/* free SAVED_VERSP structs */
	for (itm=libaroma_ctl_list_get_first_item(readlist); itm; itm=itm->next){
		SAVED_VERSP saved_vers = (SAVED_VERSP) itm->tag;
		if (saved_vers->saved/* && saved_vers->orig_cv*/) /* if versicle is saved should have orig_cv */
			libaroma_canvas_free(saved_vers->orig_cv);
		free(saved_vers); 
	}
	
	/* cleanup previous list */
	if (readlist){
		libaroma_window_del(fragread, readlist);
		libaroma_control_free(readlist);
		readlist=NULL;
	}
	
	curbook=-1;
	curchap=-1;
}

extern byte fraganim_ran;

byte chapter_load(LIBAROMA_XML book){
	if (!book || !fragread) return 0;
	
	LIBAROMA_XML chapter = libaroma_xml_child(book, "c");
	if (!chapter) {
		printf("chapter_load: failed to find chapter\n");
		return 0;
	}
	
	/* create reader list */
	readlist = libaroma_ctl_list(
		fragread, ID_FRAGREAD_LIST,
		0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL,
		0, 0,
		settings()->colorset->control_bg,
		LIBAROMA_CTL_SCROLL_WITH_SHADOW|LIBAROMA_CTL_SCROLL_WITH_HANDLE
	);
	if (!readlist) {
		printf("chapter_load: failed to create reader list\n");
		return 0;
	}
	
	LIBAROMA_XML vers;
	int cv_w, text_w,text_h, text_y;
	int pad = libaroma_dp(8), half_pad=(pad/2);
	text_y = text_h = pad;
	cv_w = mainwin->w-(pad*2);
	text_w = cv_w-vnum_width-half_pad;
	int i=0;
	for (vers = libaroma_xml_child(chapter, "v"); vers; vers=libaroma_xml_next(vers)){ /* TODO: NULL checks */
		i++;
		char vcount[4];
		snprintf(vcount, 4, "%d", i); /* instead of i, can use libaroma_xml_attr(vers, "n") */
		LIBAROMA_TEXT txt = libaroma_text(vers->txt, settings()->colorset->window_text, text_w, LIBAROMA_FONT(0, 4), 0);
		int text_h = libaroma_text_height(txt)+half_pad;
		LIBAROMA_CANVASP text_cv = libaroma_canvas(cv_w, text_h);
		libaroma_draw_rect(text_cv, 0, 0, text_cv->w, text_h, settings()->colorset->control_bg, 0xFF);
		libaroma_draw_text(text_cv, vcount, 0, 0, settings()->vnum_color, vnum_width, 
							LIBAROMA_FONT(0, 4)|LIBAROMA_TEXT_SINGLELINE|LIBAROMA_TEXT_BOLD|LIBAROMA_TEXT_RIGHT, 0);
		libaroma_text_draw(text_cv, txt, vnum_width+half_pad, 0);
		LIBAROMA_CTL_LIST_ITEMP item = libaroma_listitem_image(readlist, i, text_cv, 0, 
											LIBAROMA_LISTITEM_IMAGE_FREE|LIBAROMA_CTL_LIST_ITEM_RECEIVE_TOUCH, -1);
		if (!item){
			printf("chapter_load: failed to create item %d\n", i);
			libaroma_canvas_free(text_cv);
			libaroma_text_free(txt);
			return 0;
		}
		SAVED_VERSP saved_vers = malloc(sizeof(SAVED_VERS));
		if (!saved_vers){
			printf("chapter_load: failed to allocate versicle %d info\n", i);
			libaroma_canvas_free(text_cv);
			libaroma_text_free(txt);
			return 0;
		}
		saved_vers->orig_cv=NULL;
		saved_vers->saved=0;
		saved_vers->selected=0;
		saved_vers->bindex=curbook;
		saved_vers->cindex=curchap;
		saved_vers->versicle=vers;
		item->tag = (voidp) saved_vers;
		libaroma_text_free(txt);
	}
	
	return 1;
}

byte read_init(){
	fragread = libaroma_ctl_fragment_new_window(
		fragment, ID_FRAGREAD
	);
	if (!fragread) {
		printf("read_init: failed to create reader fragment window\n");
		return 0;
	}
	if (!br_page_new(ID_FRAGREAD, _reader_onstart, _reader_pool, _reader_onexit, _reader_onshow)){
		printf("read_init: Failed to initialize page\n");
		return 0;
	}
	load_status++;
	return 1;
}

void br_clean(){
	favs_clean();
	libaroma_xml_free(xml_bible);
	int i;
	for (i=0; i<66; i++)
		bible[i]=NULL;
}

#endif /* __br_bookmgr_c__ */