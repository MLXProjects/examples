#ifndef __br_winmain_c__
#define __br_winmain_c__

#include "main.h"

/* TODO: switch to using libaroma_stack instead of pointer array */
static BR_PAGE_STACK _br={0};

BR_PAGE_STACKP br(){
	return &_br;
}

LIBAROMA_STACKP br_history;
/*
static BR_PAGE_STACK _br_history={0};
BR_PAGE_STACKP br_history(){
	return &_br_history;
}*/

LIBAROMA_WINDOWP mainwin, mainsb, fragmain, fragpagerwin, fragread, fragsett, fragfavs;
LIBAROMA_CONTROLP appbar, fragment, fragtabs, fragpager, mainmenu, othermenu, readlist, sblist, sblogo, settlist, favlist;
LIBAROMA_CTL_BAR_TOOLSP bar_tools;
byte load_status;
int pager_change;
/*
void *anim_cb(
		LIBAROMA_CANVASP dst,
		LIBAROMA_CANVASP source_page,
		LIBAROMA_CANVASP target_page,
		int source_x, int source_w, int source_h,
		int target_x, int target_w, int target_h,
		float state
){
	libaroma_art_draw_switch_animation(3, dst, source_page, target_page, source_x, 0, source_w, source_h, target_x, 0, target_w, target_h, state);
}*/

void main_appbar_update(char *title, char *subtitle, byte arrow){
	libaroma_ctl_bar_set_icon(
		appbar,NULL,0,arrow?LIBAROMA_CTL_BAR_ICON_ARROW:LIBAROMA_CTL_BAR_ICON_DRAWER, 0
	);
	libaroma_ctl_bar_set_title(
		appbar, title, 0
	);
	libaroma_ctl_bar_set_subtitle(
		appbar, subtitle, 0
	);
	if (bar_tools){
		libaroma_ctl_bar_tools_free(bar_tools);
		bar_tools=NULL;
		libaroma_ctl_bar_set_tools(appbar, NULL, 0);
		libaroma_ctl_bar_set_touchable_title(appbar, 1);
	}
	libaroma_ctl_bar_update(appbar);
}

void _winmain_onstart(){
	/* clear current book info, is this even needed? */
	curbook=-1;
	curchap=-1;
}

void _winmain_onshow(){
	/* reset appbar */
	main_appbar_update("BR", NULL, 0);
	/* trigger page change */
	if (pager_change != -1)
	{
		if (libaroma_ctl_pager_get_active_page(fragpager) != pager_change){
			libaroma_sleep(100); //minimal delay for ripple to finish
			libaroma_ctl_pager_set_active_page(fragpager, pager_change, 0.0);
		}
		pager_change=-1;
	}
}

void _winmain_pool(){
	
	LIBAROMA_MSG msg;
	dword command	=libaroma_window_pool(mainwin,&msg);
	byte cmd		=LIBAROMA_CMD(command);
	word id			=LIBAROMA_CMD_ID(command);
	byte param		=LIBAROMA_CMD_PARAM(command);

	/*if (msg.msg==LIBAROMA_MSG_KEY_SELECT || msg.msg==LIBAROMA_MSG_KEY_POWER){
		if (msg.state==0){
			br_exit();
		}
		return;
	}*/
	switch(cmd){
		case LIBAROMA_CMD_CLICK:{
			switch (id){
				case ID_APPBAR:{
					if (param==1) libaroma_window_sidebar_show(mainsb, 1);
					else if (param==ID_APPBAR_TITLE) br_exit();
				} break;
				case ID_FRAGMAIN_LIST:{
					LIBAROMA_CTL_LIST_ITEMP touched = (LIBAROMA_CTL_LIST_ITEMP) msg.d;
					if (touched->tag != NULL) { //item contains book in tag
						LIBAROMA_RECT rect={0};
						libaroma_ctl_list_item_position(
							(touched->id<39)?mainmenu:othermenu,
							touched,
							&rect,
							0);
						curbook=touched->id;
						curchap=1;
						//br_page_set_pdata(ID_FRAGREAD, (void*)
						//br_page_setanim(ID_FRAGREAD, fragani_default, &rect, NULL);
						br_page_switch(ID_FRAGREAD);
						//br_page_resetanim(ID_FRAGMAIN);
					}
				} break;
				case ID_SIDEBAR_LIST:{
					LIBAROMA_CTL_LIST_ITEMP item = (LIBAROMA_CTL_LIST_ITEMP) msg.d;
					if (item){
						libaroma_window_sidebar_show(mainsb, 0);
						switch (item->id){
							case ID_SIDEBAR_TAB1:{
								libaroma_ctl_pager_set_active_page(fragpager, 0, 0.0);
							} break;
							case ID_SIDEBAR_TAB2:{
								libaroma_ctl_pager_set_active_page(fragpager, 1, 0.0);
							} break;
							case ID_SIDEBAR_FAVS:{
								br_page_switch(ID_FAVORITES);
							} break;
							case ID_SIDEBAR_SETTINGS:{
								br_page_switch(ID_SETTINGS);
							} break;
							case ID_SIDEBAR_ABOUT:{
								libaroma_sleep(500);
								libaroma_dialog_confirm(
									"About", "BR - Bible Reader v0.1a", libaroma_lang_get("accept"), NULL, settings()->colorset, 
									LIBAROMA_DIALOG_DIM_PARENT|LIBAROMA_DIALOG_CANCELABLE
								);
							} break;
						}
					}
				} break;
			}
		} break;
		case LIBAROMA_CMD_HOLD:{
			printf("Holded control %d, %d\n", id, param);
		} break;
	}
}

int winmain(){
	load_status=0;
	
	if (!settings_load()){ /* initialize settings in an early stage to apply them */
		printf("Failed to load settings, fallback to defaults...\n");
		if (!settings_mkdefaults()){
			printf("Failed to create default settings\n");
			return 0;
		}
	}
	br_history = libaroma_stack(NULL);
	if (!br_history){
		printf("Failed to initialize history stack\n");
		return 0;
	}
	if (!win_init()){ /* first, main window to get where to draw things */
		printf("Failed to initialize main window\n");
		return 0;
	}
	if (!settings_init()){ /* next, initialize settings window */
		printf("Failed to pre-initialize settings\n");
		return 0;
	}
	if (!bible_init()){ /* after that, load the Bible into memory... */
		printf("Failed to initialize Bible\n");
		return 0;
	}
	if (!read_init()){ /* ...and finally init versicle reader window */
		printf("Failed to pre-initialize reader\n");
		return 0;
	}
	if (!favs_init()){ /* ...find saved versicles... */
		printf("Failed to pre-initialize favorites\n");
	}
	br_page_switch(ID_FRAGMAIN);
	
	//show window
	libaroma_window_anishow(mainwin, 12, settings()->anim_duration);
	
	//start main pool
	do {
		br_history_last()->pool();
	} while (mainwin->onpool);
	
	if (br_history_last()->onexit) br_history_last()->onexit();
	
	libaroma_window_aniclose(mainwin, 12, settings()->anim_duration);
	
	return 1;
}

byte win_init(){
	mainwin = libaroma_window(
		NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
	//add controls to window
	if (!mainwin){
		printf("win_init: Failed to initialize %s\n", "mainwin");
		return 0;
	}
	
	appbar = libaroma_ctl_bar(
		mainwin, ID_APPBAR, 0, 0, LIBAROMA_SIZE_FULL, 56,
		"BR",			//title
		settings()->colorset->primary,		//background
		settings()->colorset->primary_text	//text
	);
	if (!appbar){
		printf("win_init: Failed to initialize %s\n", "appbar");
		return 0;
	}
	load_status++;
	
	if (!sidebar_init()){
		printf("win_init: Failed to initialize %s\n", "sidebar");
		return 0;
	}
	
	libaroma_ctl_bar_set_icon(
		appbar, NULL, 0, LIBAROMA_CTL_BAR_ICON_DRAWER, 0
	);
	
	libaroma_ctl_bar_set_touchable_title(appbar, 1);
	
	fragment = libaroma_ctl_fragment(
		mainwin, ID_FRAGMENT,
		0, 56, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL
	);
	if (!fragment){
		printf("win_init: Failed to initialize %s\n", "fragment");
		return 0;
	}
	
	fragmain = libaroma_ctl_fragment_new_window(
		fragment, ID_FRAGMAIN
	);
	if (!fragmain){
		printf("win_init: Failed to initialize %s\n", "fragmain");
		return 0;
	}
	//activate fragment window
	libaroma_ctl_fragment_set_active_window(
		fragment, ID_FRAGMAIN,
		1, 0, 0,
		NULL, NULL, NULL
	);
	
	fragtabs=libaroma_ctl_tabs(
		fragmain, ID_FRAGMAIN_TABS,
		0, 0, LIBAROMA_SIZE_FULL, 48,
		settings()->colorset->primary, settings()->colorset->primary_text, 0, 0
	);
	if (!fragtabs){
		printf("win_init: Failed to initialize %s\n", "fragtabs");
		return 0;
	}
	fragpager = libaroma_ctl_pager(
		fragmain, ID_FRAGMAIN_PAGER, 2,
		0, 48, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL
	);
	if (!fragpager){
		printf("win_init: Failed to initialize %s\n", "fragpager");
		return 0;
	}
		
	// set pager & tab text
	char * tab_texts[2];//={ "ANTIGUO", "NUEVO" };
	tab_texts[0]=libaroma_lang_get("old");
	tab_texts[1]=libaroma_lang_get("new");
	libaroma_ctl_tabs_set_texts(fragtabs,tab_texts,2,0);
	libaroma_ctl_tabs_set_pager(fragtabs,fragpager);
	//get pager window
	fragpagerwin = libaroma_ctl_pager_get_window(fragpager);
	if (!fragpagerwin){
		printf("win_init: Failed to initialize %s\n", "fragpagerwin");
		return 0;
	}
	int pw=libaroma_px(fragpager->w);
	mainmenu = libaroma_ctl_list(
		fragpagerwin, ID_FRAGMAIN_LIST,
		0, 0, pw, LIBAROMA_SIZE_FULL,
		0, 0,
		settings()->colorset->control_bg,
		LIBAROMA_CTL_SCROLL_WITH_SHADOW|LIBAROMA_CTL_SCROLL_WITH_HANDLE
	);
	if (!mainmenu){
		printf("win_init: Failed to initialize %s\n", "mainmenu");
		return 0;
	}
	othermenu = libaroma_ctl_list(
		fragpagerwin, ID_FRAGMAIN_LIST,
		pw, 0, pw, LIBAROMA_SIZE_FULL,
		0, 0,
		settings()->colorset->control_bg,
		LIBAROMA_CTL_SCROLL_WITH_SHADOW|LIBAROMA_CTL_SCROLL_WITH_HANDLE
	);
	if (!othermenu){
		printf("win_init: Failed to initialize %s\n", "othermenu");
		return 0;
	}
	
	if (!br_page_new(ID_FRAGMAIN, _winmain_onstart, _winmain_pool, NULL, _winmain_onshow)){
		printf("win_init: Failed to initialize main page\n");
		return 0;
	}
	
	return 1;
}

byte sidebar_init(){
	mainsb = libaroma_window_sidebar(mainwin, 0);
	if (!mainsb){
		printf("sidebar_init: failed to initialize sidebar\n");
		return 0;
	}
	byte sb_nologo=0;
	LIBAROMA_CANVASP cv = libaroma_image_uri("res:///png/aroma.png");
	if (!cv){
		printf("sidebar_init: failed to load main logo\n");
		sb_nologo=1;
	}
	else {
		sblogo = libaroma_ctl_image_canvas_ex(
			mainsb, ID_SIDEBAR_LOGO, cv,
			0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_QUARTER,
			LIBAROMA_CTL_IMAGE_FREE|LIBAROMA_CTL_IMAGE_FILL
		);
		libaroma_canvas_free(cv);
	}
	
	sblist = libaroma_ctl_list(
		mainsb, ID_SIDEBAR_LIST,
		0, sb_nologo?0:LIBAROMA_POS_1P4, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL,
		0, 0,
		settings()->colorset->control_bg,
		LIBAROMA_CTL_SCROLL_WITH_SHADOW
	);
	if (!sblist){
		printf("sidebar_init: failed to initialize sidebar list\n");
		return 0;
	}
	
	int i=32; /* 32 equals to ID_SIDEBAR_TAB1, see defs.h */

	#define _ITEM(text,ico) \
		libaroma_listitem_menu(\
			sblist, i++, text, NULL, ico, \
			LIBAROMA_LISTITEM_MENU_SMALL|LIBAROMA_LISTITEM_WITH_SEPARATOR| \
			LIBAROMA_LISTITEM_CHECK_FREE_ICON|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN,-1)

	_ITEM(libaroma_lang_get("ot"), libaroma_image_uri("res:///png/at.png"));
	_ITEM(libaroma_lang_get("nt"), libaroma_image_uri("res:///png/nt.png"));
	_ITEM(libaroma_lang_get("favs"), libaroma_image_uri("res:///png/favs.png"));
	_ITEM(libaroma_lang_get("settings"), libaroma_image_uri("res:///png/settings.png"));
	_ITEM(libaroma_lang_get("about"), libaroma_image_uri("res:///png/about.png"));
	
	#undef _ITEM
	
	return 1;
}

#endif /* __br_winmain_c__ */