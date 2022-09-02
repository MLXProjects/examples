#include "common.h"

byte br_ui_needreload=0;
LIBAROMA_RECT br_ui_toprect={0};

byte appbar_icon;

byte br_ui_init(){	
	/* initialize main pointers to NULL */
	ui_wmain=bible=cur_book=cur_chap=NULL;
	/* init libaroma */
	if (!libaroma_start()){
		LOGE("libaroma init failed");
		return 0;
	}
	/* load font - without this, the app has no purpose */
	if (!libaroma_font(0,
			libaroma_stream("res:///fonts/Roboto-Regular.ttf")
		)){
		LOGE("font load failed");
		return 0;
	}
	/* load settings & XML */
	br_settings_load();	
	if (!br_verload()){
		LOGE("failed to load default XML");
		return 0;
	}
	/* get sample text width - use 3x any of the widest known numbers */
	LIBAROMA_TEXT maxvnumw_txt=libaroma_text("999", 0, libaroma_fb()->w, LIBAROMA_TEXT_BOLD|LIBAROMA_FONT(0,br_settings()->textsize), 100);
	br_ui_maxvnumw=libaroma_text_width(maxvnumw_txt);
	libaroma_text_free(maxvnumw_txt);
	/* initialize main window */
	ui_wmain = libaroma_window(NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
	if (!ui_wmain){
		LOGE("ui window create failed");
		goto fail;
	}
	appbar = libaroma_ctl_bar(ui_wmain, ID_CTL_APPBAR, 0, 0, LIBAROMA_SIZE_FULL, 56, NULL, RGB(6743B3), RGB(FFFFFF));
	if (!appbar){
		LOGE("ui appbar create failed");
		goto fail;
	}
	mainfrag = libaroma_ctl_fragment(ui_wmain, ID_CTL_MAINFRAG, 0, 56, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
	if (!mainfrag){
		LOGE("ui main fragment create failed");
		goto fail;
	}
	/* init sidebar */
	if (!br_sidebar_init()){
		LOGE("ui sidebar create failed");
		goto fail;
	}
	/* init individual fragments */
	if (!br_home_initfrag()) goto fail;
	if (!br_settings_initfrag()) goto fail;
	/* init empty window - allows smoother transition into home window */
	libaroma_ctl_fragment_new_window(mainfrag, ID_WIN_EMPTY); 
	libaroma_ctl_fragment_set_active_window(mainfrag, ID_WIN_EMPTY, 0, 0, 0, 0, 0, 0);
	return 1;
fail:
	if (ui_wmain!=NULL) libaroma_window_free(ui_wmain);
	br_ui_free();
	return 0;
}

/* main window loop */
byte br_ui_loop(){
	char *tabs_text[2];
	/*
	tabs_text[0]=calloc(12, sizeof(char));
	tabs_text[1]=calloc(12, sizeof(char));
	snprintf(tabs_text[0], 12, "ANTIGUO");
	snprintf(tabs_text[1], 12, "NUEVO");*/
	tabs_text[0]=libaroma_lang_get("ui.old_tab");
	tabs_text[1]=libaroma_lang_get("ui.new_tab");
	libaroma_window_anishow(ui_wmain, LIBAROMA_WINDOW_SHOW_ANIMATION_FADE, 200);
	libaroma_ctl_bar_set_icon(appbar, NULL, 0, LIBAROMA_CTL_BAR_ICON_DRAWER, 0);
	libaroma_ctl_bar_set_title(appbar, BR_APPNAME, 0);
	libaroma_ctl_bar_update(appbar);
	appbar_icon=LIBAROMA_CTL_BAR_ICON_DRAWER;
	LIBAROMA_RECT top_rect={0, 0, 0, 0};
	libaroma_ctl_fragment_set_active_window(mainfrag, ID_WIN_HOME, 0, br_settings()->slowmo?4000:br_settings()->anitime, 1, &br_ui_deftrans, &br_ui_toprect, NULL);
	/* create tabs control AFTER setting window as active, prevents tabs width bug */
	tabs = libaroma_ctl_tabs(ui_whome, ID_CTL_PAGER, 0, 0, LIBAROMA_SIZE_FULL, 48, RGB(6743B3), RGB(FFFFFF), 0, 0);
	if (!tabs){
		LOGE("ui tabs create failed");
	}
	libaroma_ctl_tabs_set_texts(tabs, &tabs_text, 2, 1);
	libaroma_ctl_tabs_set_pager(tabs, pager);
	LIBAROMA_MSG msg;
	dword command;
	byte cmd;
	word id;
	byte param;
	do {
		command	= libaroma_window_pool(ui_wmain,&msg);
		cmd		= LIBAROMA_CMD(command);
		id		= LIBAROMA_CMD_ID(command);
		param	= LIBAROMA_CMD_PARAM(command);
		if (msg.msg==LIBAROMA_MSG_EXIT || msg.msg==LIBAROMA_MSG_KEY_POWER){
			if (libaroma_ctl_fragment_get_active_window_id(mainfrag)!=ID_WIN_HOME){
				if (libaroma_ctl_fragment_get_active_window_id(mainfrag)==ID_WIN_VERSREAD){ /* if on chapter, go to selector */
					cur_chap=NULL;
					br_ui_barupdate(libaroma_xml_attr(cur_book, "n"), NULL, LIBAROMA_CTL_BAR_ICON_ARROW);
					libaroma_ctl_fragment_set_active_window(mainfrag, ID_WIN_CHAPSEL, 0, br_settings()->slowmo?4000:br_settings()->anitime, 1, NULL, NULL, NULL);
				} else if (libaroma_ctl_fragment_get_active_window_id(mainfrag)==ID_WIN_CHAPSEL) { /* go to main window */
					cur_book=NULL;
					br_ui_barupdate(BR_APPNAME, NULL, LIBAROMA_CTL_BAR_ICON_ARROW_TO_DRAWER);
					libaroma_ctl_fragment_set_active_window(mainfrag, ID_WIN_HOME, 0, br_settings()->slowmo?4000:br_settings()->anitime, 1, &br_ui_deftrans, &br_ui_toprect, NULL);
				}else { /* go to main window */
					br_ui_barupdate(BR_APPNAME, NULL, LIBAROMA_CTL_BAR_ICON_ARROW_TO_DRAWER);
					libaroma_ctl_fragment_set_active_window(mainfrag, ID_WIN_HOME, 0, br_settings()->slowmo?4000:br_settings()->anitime, 0, &br_ui_deftrans, &br_ui_toprect, NULL);
				}
			}
			else {
				ui_wmain->onpool=0;
				break;
			}
		}
		else if (cmd==LIBAROMA_CMD_CLICK){
			if (id==ID_CTL_APPBAR){
				if (libaroma_ctl_fragment_get_active_window_id(mainfrag)!=ID_WIN_HOME){
					if (libaroma_ctl_fragment_get_active_window_id(mainfrag)==ID_WIN_VERSREAD){ /* if on chapter, go to selector */
						cur_chap=NULL;
						br_ui_barupdate(libaroma_xml_attr(cur_book, "n"), NULL, LIBAROMA_CTL_BAR_ICON_ARROW);
						libaroma_ctl_fragment_set_active_window(mainfrag, ID_WIN_CHAPSEL, 0, br_settings()->slowmo?4000:br_settings()->anitime, 1, NULL, NULL, NULL);
					} else if (libaroma_ctl_fragment_get_active_window_id(mainfrag)==ID_WIN_CHAPSEL) { /* go to main window */
						cur_book=NULL;
						br_ui_barupdate(BR_APPNAME, NULL, LIBAROMA_CTL_BAR_ICON_ARROW_TO_DRAWER);
						libaroma_ctl_fragment_set_active_window(mainfrag, ID_WIN_HOME, 0, br_settings()->slowmo?4000:br_settings()->anitime, 1, &br_ui_deftrans, &br_ui_toprect, NULL);
					}else { /* go to main window */
						br_ui_barupdate(BR_APPNAME, NULL, LIBAROMA_CTL_BAR_ICON_ARROW_TO_DRAWER);
						libaroma_ctl_fragment_set_active_window(mainfrag, ID_WIN_HOME, 0, br_settings()->slowmo?4000:br_settings()->anitime, 0, &br_ui_deftrans, &br_ui_toprect, NULL);
					}
				}
			}
		}
	} while (ui_wmain->onpool);
	br_settings_save();
	libaroma_window_aniclose(ui_wmain, LIBAROMA_WINDOW_SHOW_ANIMATION_FADE, 200);
	ui_wmain=NULL; /* aniclose also frees the window */
	return 1;
}

byte br_ui_readloop(){
	
	return 1;
}

void br_ui_free(){
	/* release libaroma & parsed xml */
	libaroma_end();
	libaroma_xml_free(bible);
}

/* TODO: center target (this gives a beautiful reveal-like effect) */
void br_ui_deftrans(
	LIBAROMA_CANVASP dst,
	LIBAROMA_CANVASP bottom,
	LIBAROMA_CANVASP top,
	float state,
	LIBAROMA_RECTP r1,
	LIBAROMA_RECTP r2
){
	byte opening=1;
	if (r2){
		r1=r2;
		opening=0;
		LIBAROMA_CANVASP top2 = top;
		top = bottom;
		bottom=top2;
	}
	if (!r1){
		return;
	}
	state=libaroma_motion_fluid(state);
	if (state>1.0f) state=1.0f;
	int to_h, to_y;
	if (opening){
		to_y=(r1->y+r1->h)*(1-state);
		to_h=(dst->h*state);
	}
	else {
		to_y=((r1->y+r1->h)*state);
		to_h=(dst->h-(to_y))*(1-state);
	}
	
	if (opening && !br_ui_deftrans_ran){
		libaroma_draw(bottom, dst, 0, 0, 0);
		br_ui_deftrans_ran=1;
	}
	//if (settings()->fragani_flags&FRAGANI_NOSLIDE) {
	if (0){
		libaroma_draw_ex(dst, bottom, 0, 0, 0, 0, dst->w, to_y, 0, 0xFF);
		libaroma_draw_ex(dst, bottom, 0, to_y+to_h, 0, dst->h-(to_y+to_h), dst->w, dst->h-(r1->y+r1->h), 0, 0xFF);
	}
	else {
		libaroma_draw_ex(dst, bottom, 0, 0, 0, (r1->y+r1->h)-to_y, dst->w, to_y/*(r1->y+r1->h)*/, 0, 0xFF);
		libaroma_draw_ex(dst, bottom, 0, to_y+to_h, 0, r1->y+r1->h/*to_y+to_h*/, dst->w, dst->h-(r1->y+r1->h), 0, 0xFF);
	}
	//if (0)
	if (br_settings()->scale)
		libaroma_draw_scale_nearest(dst, top, 0, to_y, dst->w, to_h, 0, 0, top->w, top->h);
	else libaroma_draw_ex(dst, top, 0, to_y, 0, 0, dst->w, to_h, 0, 0xFF);
}
