#include "common.h"

static BR_SETTINGS _br_settings;

/* public settings accessor */
BR_SETTINGSP br_settings(){
	return &_br_settings;
}

void br_settlist_onclick(LIBAROMA_CONTROLP ctl){
	LIBAROMA_CTL_LIST_ITEMP item = libaroma_ctl_list_get_touched_item(settlist);
	libaroma_sleep(300);
	if (!item) return;
	switch (item->id){
		case ID_SETT_SLOWMO:{
		} break;
		case ID_SETT_DARKMODE:{
		} break;
		case ID_SETT_SCALE:{
		} break;
		case ID_SETT_TEXTSIZE:{
			LIBAROMA_LISTITEM_TEMPLATE templates[4]={
				{LIBAROMA_LIST_ITEM_KIND_CHECK,1,"<size=2>Pequeño",NULL,0,0,2,0,LIBAROMA_LISTITEM_MENU_SMALL|LIBAROMA_LISTITEM_CHECK_OPTION},
				{LIBAROMA_LIST_ITEM_KIND_CHECK,2,"<size=3>Normal",NULL,0,0,3,1,LIBAROMA_LISTITEM_MENU_SMALL|LIBAROMA_LISTITEM_CHECK_OPTION},
				{LIBAROMA_LIST_ITEM_KIND_CHECK,3,"<size=4>Medio",NULL,0,0,4,0,LIBAROMA_LISTITEM_MENU_SMALL|LIBAROMA_LISTITEM_CHECK_OPTION},
				{LIBAROMA_LIST_ITEM_KIND_CHECK,4,"<size=5>Grande",NULL,0,0,5,0,LIBAROMA_LISTITEM_MENU_SMALL|LIBAROMA_LISTITEM_CHECK_OPTION}
			};
			libaroma_sleep(300);
			int ret = libaroma_dialog_list("Tamaño", "CANCELAR", NULL, templates, 4, 1, libaroma_colorget(NULL,NULL),LIBAROMA_DIALOG_DIM_PARENT|LIBAROMA_DIALOG_WITH_SHADOW|LIBAROMA_DIALOG_CANCELABLE);
			LOGD("dialog return %d", ret);
			libaroma_window_show(ui_wmain);
			
		} break;
		case ID_SETT_LOCALE:{
		} break;
		case ID_SETT_VERSION:{
		} break;
	}
}

byte br_settings_initfrag(){
	ui_wsettings = libaroma_ctl_fragment_new_window(mainfrag, ID_WIN_SETTINGS);
	if (!ui_wsettings){
		LOGE("ui settings window create failed");
		return 0;
	}
	settlist = libaroma_ctl_list(ui_wsettings, ID_CTL_SETTLIST, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL, 0, 0, (br_settings()->darkmode)?(RGB(0)):(RGB(FFFFFF)), LIBAROMA_CTL_LIST_WITH_SHADOW);
	if (!settlist){
		LOGE("ui sidebar list create failed");
		return 0;
	}
	libaroma_listitem_check(settlist, ID_SETT_SLOWMO, br_settings()->slowmo, 
		libaroma_lang_get("sett.slowmo"), libaroma_lang_get("sett.slowmo_extra"), NULL,
		LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_CHECK_SWITCH,
	-1);
	libaroma_listitem_check(settlist, ID_SETT_DARKMODE, br_settings()->darkmode, 
		libaroma_lang_get("sett.darkmode"), libaroma_lang_get("sett.darkmode_extra"), NULL,
		LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_CHECK_SWITCH,
	-1);
	libaroma_listitem_check(settlist, ID_SETT_SCALE, br_settings()->scale, libaroma_lang_get("sett.scale"), libaroma_lang_get("sett.scale_extra"), NULL,
		LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_CHECK_SWITCH,
	-1);
	libaroma_listitem_menu(settlist, ID_SETT_TEXTSIZE, libaroma_lang_get("sett.textsize"), NULL, NULL,
		LIBAROMA_LISTITEM_WITH_SEPARATOR,
	-1);
	libaroma_listitem_menu(settlist, ID_SETT_LOCALE, libaroma_lang_get("sett.locale"), libaroma_lang_get("lang.name"), NULL,
		LIBAROMA_LISTITEM_WITH_SEPARATOR,
	-1);
	libaroma_listitem_menu(settlist, ID_SETT_VERSION, libaroma_lang_get("sett.version"), NULL, NULL,
		LIBAROMA_LISTITEM_WITH_SEPARATOR,
	-1);
	
	libaroma_control_set_onclick(settlist, &br_settlist_onclick);
	return 1;
}

void _br_settings_locale_en(){
	libaroma_lang_set("lang.name", "English (hardcoded)");
	libaroma_lang_set("ui.settings", "Settings");
	libaroma_lang_set("ui.chapter", "Chapter");
	libaroma_lang_set("ui.favs", "Favorites");
	libaroma_lang_set("ui.old_tab", "OLD");
	libaroma_lang_set("ui.new_tab", "NEW");
	libaroma_lang_set("ui.ot", "Old Testment");
	libaroma_lang_set("ui.nt", "New Testment");
	libaroma_lang_set("ui.about", "About");
	libaroma_lang_set("ui.accept", "Accept");
	libaroma_lang_set("ui.cancel", "Cancel");
	libaroma_lang_set("ui.yes", "Yes");
	libaroma_lang_set("ui.no", "No");
	libaroma_lang_set("sett.darkmode", "Dark mode");
	libaroma_lang_set("sett.darkmode_extra", "Low light theme");
	libaroma_lang_set("sett.slowmo", "Slow motion");
	libaroma_lang_set("sett.slowmo_extra", "A.K.A. butter mode");
	libaroma_lang_set("sett.scale", "Scale");
	libaroma_lang_set("sett.scale_extra", "Replaces reveal at startup");
	libaroma_lang_set("sett.textsize", "Font size");
	libaroma_lang_set("sett.locale", "Language");
	libaroma_lang_set("sett.version", "Version");
}

/* reload locale from settings */
byte br_settings_updlocale(){
	int uri_len=20+strlen(_br_settings.locale);
	char lang_uri[64];
	snprintf(lang_uri, uri_len, "res:///lang/%s.prop", _br_settings.locale);
	LOGD("Loading locale from %s", lang_uri);
	LIBAROMA_STREAMP stream = libaroma_stream(lang_uri);
	if (!stream){
		LOGE("Failed to load %s locale", _br_settings.locale);
		return 0;
	}
	char *lang_string=libaroma_stream_to_string(stream, 0);
	if (!lang_string){
		LOGE("Locale has no text? That's weird");
		return 0;
	}
	/* 
	 * TODO: please, find a decent way to parse prop files 
	 * (this reallocates a lot of times - see libaroma/src/utils/stream.c)
	 */
	libaroma_lang_set("lang.name", libaroma_getprop_ex("lang.name", libaroma_stream_to_string(stream, 0), 1));
	libaroma_lang_set("ui.settings", libaroma_getprop_ex("ui.settings", libaroma_stream_to_string(stream, 0), 1));
	libaroma_lang_set("ui.chapter", libaroma_getprop_ex("ui.chapter", libaroma_stream_to_string(stream, 0), 1));
	libaroma_lang_set("ui.favs", libaroma_getprop_ex("ui.favs", libaroma_stream_to_string(stream, 0), 1));
	libaroma_lang_set("ui.old_tab", libaroma_getprop_ex("ui.old_tab", libaroma_stream_to_string(stream, 0), 1));
	libaroma_lang_set("ui.new_tab", libaroma_getprop_ex("ui.new_tab", libaroma_stream_to_string(stream, 0), 1));
	libaroma_lang_set("ui.ot", libaroma_getprop_ex("ui.ot", libaroma_stream_to_string(stream, 0), 1));
	libaroma_lang_set("ui.nt", libaroma_getprop_ex("ui.nt", libaroma_stream_to_string(stream, 0), 1));
	libaroma_lang_set("ui.about", libaroma_getprop_ex("ui.about", libaroma_stream_to_string(stream, 0), 1));
	libaroma_lang_set("ui.accept", libaroma_getprop_ex("ui.accept", libaroma_stream_to_string(stream, 0), 1));
	libaroma_lang_set("ui.cancel", libaroma_getprop_ex("ui.cancel", libaroma_stream_to_string(stream, 0), 1));
	libaroma_lang_set("ui.yes", libaroma_getprop_ex("ui.yes", libaroma_stream_to_string(stream, 0), 1));
	libaroma_lang_set("ui.no", libaroma_getprop_ex("ui.no", libaroma_stream_to_string(stream, 0), 1));
	libaroma_lang_set("sett.darkmode", libaroma_getprop_ex("sett.darkmode", libaroma_stream_to_string(stream, 0), 1));
	libaroma_lang_set("sett.darkmode_extra", libaroma_getprop_ex("sett.darkmode_extra", libaroma_stream_to_string(stream, 0), 1));
	libaroma_lang_set("sett.slowmo", libaroma_getprop_ex("sett.slowmo", libaroma_stream_to_string(stream, 0), 1));
	libaroma_lang_set("sett.slowmo_extra", libaroma_getprop_ex("sett.slowmo_extra", libaroma_stream_to_string(stream, 0), 1));
	libaroma_lang_set("sett.scale", libaroma_getprop_ex("sett.scale", libaroma_stream_to_string(stream, 0), 1));
	libaroma_lang_set("sett.scale_extra", libaroma_getprop_ex("sett.scale_extra", libaroma_stream_to_string(stream, 0), 1));
	libaroma_lang_set("sett.locale", libaroma_getprop_ex("sett.locale", libaroma_stream_to_string(stream, 0), 1));
	libaroma_lang_set("sett.textsize", libaroma_getprop_ex("sett.textsize", libaroma_stream_to_string(stream, 0), 1));
	libaroma_lang_set("sett.version", libaroma_getprop_ex("sett.version", libaroma_stream_to_string(stream, 0), 1));
	libaroma_stream_close(stream);
	return 1;
}

/* defaults */
void _br_settings_default(){
	_br_settings.slowmo=0;
	_br_settings.darkmode=0;
	_br_settings.anitime=400;
	_br_settings.textsize=3;
	_br_settings.locale="en-US";
	_br_settings.version="RV1960";
}

void br_settings_load(){
	_br_settings_default();
	char *sett_path[256];
	snprintf(sett_path, 256, "file://%s.cfg", res_path);
	LIBAROMA_STREAMP sett_stream = libaroma_stream(sett_path);
	if (sett_stream){
		LOGD("loading settings at %s", sett_path);
		/* 
		 * TODO: please, find a decent way to parse prop files 
		 * (this reallocates a lot of times - see libaroma/src/utils/stream.c)
		 */
		char *slowmo, *darkmode, *anitime, *textsize, *locale, *version;
		slowmo=libaroma_getprop_ex("slowmo", libaroma_stream_to_string(sett_stream, 0), 1);
		if (slowmo) _br_settings.slowmo=atoi(slowmo);
		darkmode=libaroma_getprop_ex("darkmode", libaroma_stream_to_string(sett_stream, 0), 1);
		if (darkmode) _br_settings.darkmode=atoi(darkmode);
		anitime=libaroma_getprop_ex("anitime", libaroma_stream_to_string(sett_stream, 0), 1);
		if (anitime) _br_settings.anitime=atoi(anitime);
		textsize=libaroma_getprop_ex("textsize", libaroma_stream_to_string(sett_stream, 0), 1);
		if (textsize) _br_settings.textsize=atoi(textsize);
		locale=libaroma_getprop_ex("locale", libaroma_stream_to_string(sett_stream, 0), 1);
		if (locale) _br_settings.locale=locale;
		version=libaroma_getprop_ex("version", libaroma_stream_to_string(sett_stream, 0), 1);
		if (version) _br_settings.version=version;
		LOGD("settings=%s, %s, %s, %s, %s, %s", slowmo, darkmode, anitime, textsize, locale, version);
		br_ui_needreload=1;
		libaroma_stream_close(sett_stream);
	}
	if (!br_settings_updlocale()){
		/* default to English */
		_br_settings_locale_en();
	}
	return 1;
}

byte br_settings_save(){
	char *sett_path[256];
	snprintf(sett_path, 256, "file://%s.cfg", res_path);
	LOGD("going to write settings file %s", sett_path);
	FILE *sett_fp = fopen(sett_path, "wb");
	if (!sett_fp){
		LOGE("failed to open settings file %s", sett_path);
		return 0;
	}
	fprintf(sett_fp,"slowmo=%d\n"
					"darkmode=%d\n"
					"anitime=%d\n"
					"textsize=%d\n"
					"locale=%s\n"
					"version=%s\n",
					_br_settings.slowmo, _br_settings.darkmode, 
					_br_settings.anitime, _br_settings.textsize, 
					_br_settings.locale, _br_settings.version); 
	fclose(sett_fp);
	return 1;
}