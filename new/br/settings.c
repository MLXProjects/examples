#ifndef __br_settings_c__
#define __br_settings_c__

#include "main.h"

static BR_SETTINGS _settings={0};

BR_SETTINGSP settings(){
	return &_settings;
}

byte settings_load(){
	
	return 0;
}

byte settings_save(){
	
	return 0;
}
extern void _settings_lang_en();

byte settings_mkdefaults(){
	_settings.pager_ani=LIBAROMA_ART_SWITCH_ANIMATION_SLIDE;
	_settings.fragani_flags = FRAGANI_SHOW/*|FRAGANI_NOSLIDE*/;
	_settings.font_id=0;
	_settings.font_size=4;
	_settings.anim_duration=400;
	_settings.colorset=libaroma_colorget(NULL, mainwin);
	settings_dark_mode(0, 0);
	//init lang here pls
	libaroma_lang_cleanup();
	if (!settings_lang_load("en-US")) /* en-US locale - default */
		_settings_lang_en();
	return 1;
}

byte settings_dark_mode(byte dark_mode, byte update){
	if (dark_mode){
		_settings.dark_mode=1;
		_settings.vnum_color = RGB(ff6363);
		_settings.saved_bg = RGB(d3d32e);
		//_settings.saved_fg = RGB(dbdbdb);
		_settings.colorset->primary=RGB(065b9e);
		_settings.colorset->primary_text=RGB(FFFFFF);
		_settings.colorset->primary_icon=RGB(b4c7d6);
		_settings.colorset->primary_light=RGB(BBDEFB);
		_settings.colorset->accent=RGB(2E7D32);
		_settings.colorset->window_bg=RGB(6b6969);
		_settings.colorset->window_text=RGB(a3a3a3);
		_settings.colorset->control_bg=RGB(6b6969);
		_settings.colorset->control_primary_text=RGB(a3a3a3);
		_settings.colorset->control_secondary_text=RGB(dddddd);
		_settings.colorset->divider=RGB(B6B6B6);
		_settings.colorset->dialog_bg=RGB(6b6969);
		_settings.colorset->dialog_primary_text=RGB(a3a3a3);
		_settings.colorset->dialog_secondary_text=RGB(dddddd);
	}
	else {
		_settings.dark_mode=0;
		_settings.vnum_color = RGB(a33636);
		_settings.saved_bg = RGB(ffff66);
		_settings.colorset->primary=RGB(2196F3);
		_settings.colorset->primary_text=RGB(FFFFFF);
		_settings.colorset->primary_icon=RGB(FFFFFF);
		_settings.colorset->primary_light=RGB(BBDEFB);
		_settings.colorset->accent=RGB(2E7D32);
		_settings.colorset->window_bg=RGB(E0E0E0);
		_settings.colorset->window_text=RGB(212121);
		_settings.colorset->control_bg=RGB(FFFFFF);
		_settings.colorset->control_primary_text=RGB(212121);
		_settings.colorset->control_secondary_text=RGB(727272);
		_settings.colorset->divider=RGB(B6B6B6);
		_settings.colorset->dialog_bg=RGB(FFFFFF);
		_settings.colorset->dialog_primary_text=RGB(212121);
		_settings.colorset->dialog_secondary_text=RGB(727272);
	}
	/* invalidate window after color changes */
	if (update){
		LIBAROMA_MSG _msg;
		libaroma_wm_compose(
				&_msg, LIBAROMA_MSG_WIN_INVALIDATE,
				(voidp) mainwin, 0, 0);
		libaroma_window_process_event(mainwin, &_msg);
	}
	return 1;
}

void _settings_lang_en(){
	libaroma_lang_set("settings", "Settings");
	libaroma_lang_set("chapter", "Chapter");
	libaroma_lang_set("dark_mode", "Dark mode");
	libaroma_lang_set("slow_mode", "Slow motion");
	libaroma_lang_set("slow_mode_extra", "A.K.A. butter mode");
	libaroma_lang_set("scale", "Scale");
	libaroma_lang_set("favs", "Favorites");
	libaroma_lang_set("old", "Old");
	libaroma_lang_set("new", "New");
	libaroma_lang_set("ot", "Old Testment");
	libaroma_lang_set("nt", "New Testment");
	libaroma_lang_set("about", "About");
	libaroma_lang_set("accept", "Accept");
	libaroma_lang_set("cancel", "Cancel");
	libaroma_lang_set("yes", "Yes");
	libaroma_lang_set("no", "No");
}

byte settings_lang_load(char *locale){
	int uri_len=20+strlen(locale);
	char *lang_uri = malloc(uri_len);
	snprintf(lang_uri, uri_len, "res:///lang/%s.prop", locale);
	LIBAROMA_STREAMP stream = libaroma_stream(lang_uri);
	if (!stream){
		printf("Failed to load %s lang\n", locale);
		free(lang_uri);
		return 0;
	}
	libaroma_lang_set("settings", libaroma_getprop("settings", stream, 0));
	libaroma_lang_set("chapter", libaroma_getprop("chapter", stream, 0));
	libaroma_lang_set("dark_mode", libaroma_getprop("dark_mode", stream, 0));
	libaroma_lang_set("slow_mode", libaroma_getprop("slow_mode", stream, 0));
	libaroma_lang_set("slow_mode_extra", libaroma_getprop("slow_mode_extra", stream, 0));
	libaroma_lang_set("scale", libaroma_getprop("scale", stream, 0));
	libaroma_lang_set("favs", libaroma_getprop("favs", stream, 0));
	libaroma_lang_set("old", libaroma_getprop("old", stream, 0));
	libaroma_lang_set("new", libaroma_getprop("new", stream, 0));
	libaroma_lang_set("ot", libaroma_getprop("ot", stream, 0));
	libaroma_lang_set("nt", libaroma_getprop("nt", stream, 0));
	libaroma_lang_set("about", libaroma_getprop("about", stream, 0));
	libaroma_lang_set("accept", libaroma_getprop("accept", stream, 0));
	libaroma_lang_set("cancel", libaroma_getprop("cancel", stream, 0));
	libaroma_lang_set("yes", libaroma_getprop("yes", stream, 0));
	libaroma_lang_set("no", libaroma_getprop("no", stream, 0));
	libaroma_stream_close(stream);
	free(lang_uri);
	return 1;
}

void _settings_onshow(){
	main_appbar_update(libaroma_lang_get("settings"), NULL, 1);
}

void _settings_pool(){
	LIBAROMA_MSG msg;
	dword command	=libaroma_window_pool(mainwin,&msg);
	byte cmd		=LIBAROMA_CMD(command);
	word id			=LIBAROMA_CMD_ID(command);
	byte param		=LIBAROMA_CMD_PARAM(command);
	
	/*if (msg.msg==LIBAROMA_MSG_KEY_SELECT || msg.msg==LIBAROMA_MSG_KEY_POWER){
		if (msg.state==0){
			br_page_switch(ID_FRAGMAIN);
		}
		return;
	}*/
	switch(cmd){
		case LIBAROMA_CMD_CLICK:{
			switch (id){
				case ID_APPBAR:{
					br_page_goback();
				} break;
				case ID_SETTINGS_LIST:{
					LIBAROMA_CTL_LIST_ITEMP touched = (LIBAROMA_CTL_LIST_ITEMP) msg.d;
					switch (touched->id){
						case ID_SETTINGS_DARK:{
							/*
							 * because of how lists are rendered, changing colorset and invalidating window
							 * isn't enough to apply the visual changes.
							 * this should close and re-open the window, optionally saving current open 
							 * book/chapter/versicle (if any open), and then re-open the settings.
							 */
							//settings_dark_mode(!(_settings.dark_mode), 1);
						} break;
						case ID_SETTINGS_SLOWCAM:{
							if (settings()->anim_duration==400){
								settings()->anim_duration=6300;
								/*libaroma_config()->ripple_hold_time=6000;
								libaroma_config()->ripple_touch_time=2500;*/
							}
							else {
								settings()->anim_duration=400;
								/*libaroma_config()->ripple_hold_time=1300;
								libaroma_config()->ripple_touch_time=250;*/
							}
						} break;
						case ID_SETTINGS_SCALE:{
							if (settings()->fragani_flags&FRAGANI_SCALE)
								settings()->fragani_flags &= ~FRAGANI_SCALE;
							else settings()->fragani_flags |= FRAGANI_SCALE;
						} break;
					}
				} break;
				case ID_SIDEBAR_LIST:{
					LIBAROMA_CTL_LIST_ITEMP touched = (LIBAROMA_CTL_LIST_ITEMP) msg.d;
					if (touched){
						libaroma_window_sidebar_show(mainsb, 0);
						switch (touched->id){
							case ID_SIDEBAR_TAB1:{
								pager_change=0;
								br_page_switch(ID_FRAGMAIN);
							} break;
							case ID_SIDEBAR_TAB2:{
								pager_change=1;
								br_page_switch(ID_FRAGMAIN);
							} break;
							case ID_SIDEBAR_FAVS:{
								br_page_switch(ID_FAVORITES);
							} break;
						}
					}
				} break;
			}
		} break;
	}
}

byte settings_init(){
	fragsett = libaroma_ctl_fragment_new_window(
		fragment, ID_SETTINGS
	);
	if (!fragsett) {
		printf("settings_init: failed to create settings fragment window\n");
		return 0;
	}
	
	/* create reader list */
	settlist = libaroma_ctl_list(
		fragsett, ID_SETTINGS_LIST,
		0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL,
		0, 0,
		settings()->colorset->control_bg,
		LIBAROMA_CTL_SCROLL_WITH_SHADOW|LIBAROMA_CTL_SCROLL_WITH_HANDLE
	);
	if (!settlist) {
		printf("settings_show: failed to create settings list\n");
		return 0;
	}
	libaroma_listitem_check(settlist, ID_SETTINGS_DARK, (settings()->dark_mode)?1:0, 
							libaroma_lang_get("dark_mode"), NULL, NULL, 
							LIBAROMA_LISTITEM_CHECK_SWITCH|LIBAROMA_LISTITEM_WITH_SEPARATOR|
							LIBAROMA_LISTITEM_CHECK_INDENT_NOICON|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN,-1
	);
	libaroma_listitem_check(settlist, ID_SETTINGS_SLOWCAM, (settings()->anim_duration==400)?0:1, 
							libaroma_lang_get("slow_mode"), libaroma_lang_get("slow_mode_extra"), NULL, 
							LIBAROMA_LISTITEM_CHECK_SWITCH|LIBAROMA_LISTITEM_WITH_SEPARATOR|
							LIBAROMA_LISTITEM_CHECK_INDENT_NOICON|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN,-1
	);
	libaroma_listitem_check(settlist, ID_SETTINGS_SCALE, (settings()->fragani_flags&FRAGANI_SCALE)?1:0, 
							libaroma_lang_get("scale"), NULL, NULL, 
							LIBAROMA_LISTITEM_CHECK_SWITCH|LIBAROMA_LISTITEM_WITH_SEPARATOR|
							LIBAROMA_LISTITEM_CHECK_INDENT_NOICON|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN,-1
	);
	
	if (!br_page_new(ID_SETTINGS, NULL, _settings_pool, NULL, _settings_onshow)){
		printf("settings_init: Failed to initialize page\n");
		return 0;
	}
	return 1;
}

#endif /* __br_settings_c__ */