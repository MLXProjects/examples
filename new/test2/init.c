#ifndef __test_init_c__
#define __test_init_c__
#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
	
byte main_win_init(LIBAROMA_CONTROLP fragment);
byte pset_win_init(LIBAROMA_CONTROLP fragment);
byte appbar_win_init(LIBAROMA_CONTROLP fragment);
byte btns_win_init(LIBAROMA_CONTROLP fragment);
byte prog_win_init(LIBAROMA_CONTROLP fragment);
byte xml_win_init(LIBAROMA_CONTROLP fragment);
byte debug_win_init(LIBAROMA_CONTROLP fragment);
byte lst_win_init(LIBAROMA_CONTROLP fragment);
byte test_list_options(LIBAROMA_CONTROLP ctl, LIBAROMA_CTL_LIST_ITEMP item, int id, byte checked, voidp data, byte state);
extern void test_ovs_effect(LIBAROMA_CONTROLP list, LIBAROMA_CANVASP canvas, byte istop);

byte test_init(LIBAROMA_WINDOWP win){
	printf("MLX: %s\n", "start creating controls");
	/* fill main window with controls */
	appbar = libaroma_ctl_bar(win, ID_APPBAR, 0, 0, LIBAROMA_SIZE_FULL, 56, "Libaroma test", test_config()->appbar_bg, test_config()->appbar_fg);
	if (!appbar) return 0;
	ab_icon = libaroma_image_uri("res:///images/ic_settings_data_usage.png");
	libaroma_canvas_fillcolor(ab_icon, RGB(FFFFFF));
	// set bar tools 
	bar_tools=libaroma_ctl_bar_tools(2);
	if (!bar_tools) return 0;
	libaroma_ctl_bar_tools_set(
		bar_tools, 0, 1, "one", ab_icon, LIBAROMA_CTL_BAR_TOOL_ICON_SHARED
	);
	libaroma_ctl_bar_tools_set(
		bar_tools, 1, 3, "two", ab_icon,
		LIBAROMA_CTL_BAR_TOOL_SWITCH|LIBAROMA_CTL_BAR_TOOL_ICON_SHARED
	);
	libaroma_ctl_bar_set_tools(
		appbar,bar_tools, 1);
	libaroma_ctl_bar_set_icon(
		appbar,NULL,0,LIBAROMA_CTL_BAR_ICON_DRAWER, 1
	);
	
	printf("MLX: %s\n", "appbar done");
	
	/* create fragment for multiple windows */
	fragment = libaroma_ctl_fragment(win, ID_FRAGMENT, 0, 56, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
	printf("MLX: %s\n", "fragment created");
	
	/* now initialize individual windows */
	if (!main_win_init(fragment)) goto fail;
	printf("MLX: %s\n", "main fragment done");
	if (!appbar_win_init(fragment)) goto fail;
	printf("MLX: %s\n", "appbar fragment done");
	if (!btns_win_init(fragment)) goto fail;
	printf("MLX: %s\n", "buttons fragment done");
	if (!prog_win_init(fragment)) goto fail;
	printf("MLX: %s\n", "progress fragment done");
	if (!xml_win_init(fragment)) goto fail;/*
	printf("MLX: %s\n", "xml fragment done");
	if (!debug_win_init(fragment)) goto fail;
	printf("MLX: %s\n", "debug fragment done");
	if (!lst_win_init(fragment)) goto fail;
	printf("MLX: %s\n", "lst fragment done");*/
	
	/* set main window as active */
	active_fragment=ID_FRAGMENT_MAIN;
	printf("MLX: %s\n", "main is now active fragment");
	
	goto ok;
	
fail:
	printf("MLX: %s\n", "failed to init somewhere");
	libaroma_window_free(win);
	return 0;
ok:
	printf("MLX: %s\n", "init succeeded");
	return 1;
}

byte main_win_init(LIBAROMA_CONTROLP fragment){
	LIBAROMA_WINDOWP win = libaroma_ctl_fragment_new_window(fragment, ID_FRAGMENT_MAIN);
	if (!win) return 0;
	printf("MLX: %s\n", "created new window");
	/* set first fragment window as active to avoid weird fragment's behavior later (e.g. hangs when switching windows) */
	libaroma_ctl_fragment_set_active_window(fragment, ID_FRAGMENT_MAIN, 0, 0, 0, NULL, NULL, NULL);
	printf("MLX: %s\n", "set active done");
	
	LIBAROMA_CONTROLP tabs = libaroma_ctl_tabs(win, ID_TABS, 0, 0, fragment->width, 48, test_config()->appbar_bg, test_config()->appbar_fg, 0, 0);
	if (!tabs) return 0;
	printf("MLX: %s\n", "tabs control created");
	pager = libaroma_ctl_pager(win, ID_PAGER, 3, 0, 48, fragment->width, fragment->height-48);
	if (!pager) return 0;
	printf("MLX: %s\n", "pager control created");
	
	/* set pager & tab text */
	char * tab_texts[3]={ "HOME", "LIST SAMPLE", "SETTINGS" };
	libaroma_ctl_tabs_set_texts(tabs,tab_texts,3,0);
	libaroma_ctl_tabs_set_pager(tabs,pager);
	printf("MLX: %s\n", "tabs text & pager set");
	
	/* get window from pager */
	LIBAROMA_WINDOWP pagerwin = libaroma_ctl_pager_get_window(pager);
	printf("MLX: %s\n", "got window from pager");
	int pagerw = fragment->width;
	
	/* initialize pager tabs */
	/* first page */
	printf("MLX: %s\n", "initializing first page");
	
	LIBAROMA_CONTROLP main_list = libaroma_ctl_list(pagerwin, ID_MAIN_LIST, 0, 0, pagerw, LIBAROMA_SIZE_FULL, 0, 0, RGB(FFFFFF), LIBAROMA_CTL_LIST_WITH_SHADOW);
	if (!main_list) return 0;
	printf("MLX: %s\n", "main_list done");
	
	/* load icons from zip */
	/*LIBAROMA_CANVASP bug_icon =
		libaroma_image_uri("res:///images/bug.png");
	LIBAROMA_CANVASP lst_icon =
		libaroma_image_uri("res:///images/list.png");*/
	LIBAROMA_CANVASP settings_icon =
		libaroma_image_uri("res:///images/settings.png");
	LIBAROMA_CANVASP btn_icon =
		libaroma_image_uri("res:///images/select.png");
	LIBAROMA_CANVASP prog_icon =
		libaroma_image_uri("res:///images/progress.png");
	LIBAROMA_CANVASP xml_icon =
		libaroma_image_uri("res:///images/xml.png");
	LIBAROMA_CANVASP exit_icon =
		libaroma_image_uri("res:///images/exit.png");
	//TODO: should check those icons for NULL (failed to load)?
	printf("MLX: %s\n", "icons load done");
	
	/* fill icons with primary color */
	/*libaroma_canvas_fillcolor(bug_icon,libaroma_colorget(NULL,NULL)->primary);
	libaroma_canvas_fillcolor(lst_icon,libaroma_colorget(NULL,NULL)->primary);*/
	libaroma_canvas_fillcolor(settings_icon,libaroma_colorget(NULL,NULL)->primary);
	libaroma_canvas_fillcolor(btn_icon,libaroma_colorget(NULL,NULL)->primary);
	libaroma_canvas_fillcolor(prog_icon,libaroma_colorget(NULL,NULL)->primary);
	libaroma_canvas_fillcolor(xml_icon,libaroma_colorget(NULL,NULL)->primary);
	libaroma_canvas_fillcolor(exit_icon,libaroma_colorget(NULL,NULL)->primary);
	printf("MLX: %s\n", "icons fill done");
	
	/* fill list with items */
	/*libaroma_listitem_caption(main_list, 1, "Debug utils", -1);
	libaroma_listitem_menu(main_list, ID_MAIN_LISTITEM_DBG, "Read dmesg", "Like cat /proc/kmsg", bug_icon, LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN, -1);
	libaroma_listitem_menu(main_list, ID_MAIN_LISTITEM_LST, "List /dev", "Like ls -l /dev", lst_icon, LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN, -1);*/
	libaroma_listitem_caption(main_list, 1, "Samples", -1);
	libaroma_listitem_menu(main_list, ID_MAIN_LISTITEM_SETT, "Appbar settings", "Look at the different appbar modes", settings_icon, LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN, -1);
	libaroma_listitem_menu(main_list, ID_MAIN_LISTITEM_BTNS, "Buttons", "Check out all the button styles", btn_icon, LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN, -1);
	libaroma_listitem_menu(main_list, ID_MAIN_LISTITEM_PROG, "Progress bars", "A page containing progress bars", prog_icon, LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN, -1);
	libaroma_listitem_menu(main_list, ID_MAIN_LISTITEM_XML, "Custom XML layout", "Load interface from XML file", xml_icon, LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN, -1);
	libaroma_listitem_divider(main_list, 1, LIBAROMA_LISTITEM_DIVIDER_SUBSCREEN, -1);
	libaroma_listitem_caption(main_list, 1, "Leave", -1);
	libaroma_listitem_menu(main_list, ID_MAIN_LISTITEM_EXIT, "Exit Libaroma", "I hope you enjoyed this :)", exit_icon, LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN, -1);
	libaroma_listitem_divider(main_list, 1, LIBAROMA_LISTITEM_DIVIDER_SUBSCREEN, -1);
	printf("MLX: %s\n", "main win init done");
	
	/* second page */
	printf("MLX: %s\n", "initializing second page");
	test_list = libaroma_ctl_list(pagerwin, 1, pagerw, 0, pagerw, LIBAROMA_SIZE_FULL, 0, 0, RGB(FFFFFF), LIBAROMA_CTL_LIST_WITH_SHADOW|LIBAROMA_CTL_LIST_WITH_HANDLE);
	if (!test_list) return 0;
	
	/* load icon from zip */
	LIBAROMA_CANVASP list_icon =
	libaroma_image_uri("res:///images/ic_settings_data_usage.png");
	
	/* fill color */
	libaroma_canvas_fillcolor(list_icon,libaroma_colorget(NULL,NULL)->primary);
	
	/* populate list with sample items */
	char main_text[256];
	char extra_text[256];
	int itm=0;
	int kdv=0;
	for (itm=0;itm<50;itm++){
		if (itm==10){
			libaroma_listitem_image(
				test_list,1,
				libaroma_image_uri("res:///images/mlx.png"),
									150,
						   LIBAROMA_LISTITEM_IMAGE_FREE|LIBAROMA_LISTITEM_WITH_SEPARATOR|
						   LIBAROMA_LISTITEM_IMAGE_FILL|LIBAROMA_LISTITEM_IMAGE_PROPORTIONAL|
						   LIBAROMA_CTL_LIST_ITEM_RECEIVE_TOUCH|LIBAROMA_LISTITEM_IMAGE_PARALAX,
						   -1);
		}
		
		if (itm%6==4){
			libaroma_listitem_divider(
				test_list, 1, (((kdv++)%2)==0)?LIBAROMA_LISTITEM_DIVIDER_SUBSCREEN:
				LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN, -1);
		}
		
		if (itm%15==0){
			snprintf(main_text,256,"List Caption %i",itm);
			libaroma_listitem_caption(
				test_list, 200, main_text, -1);
		}
		
		snprintf(main_text,256,"Item id#%i",itm);
		word add_flags=0;
		if (itm%3==1){
			add_flags=LIBAROMA_LISTITEM_CHECK_SWITCH;
			snprintf(extra_text,256,
					 "Secondary text for list item %i is three line list item text",itm);
		}
		else if (itm%3==2){
			snprintf(extra_text,256,"Secondary text %i",itm);
		}
		if (itm%5==0){
			libaroma_listitem_check(
				test_list, itm+10, 0,
				main_text,
				(itm%3!=0)?extra_text:NULL,
									NULL,
						   LIBAROMA_LISTITEM_WITH_SEPARATOR|
						   LIBAROMA_LISTITEM_CHECK_LEFT_CONTROL|
						   add_flags,
						   -1
			);
		}
		else{
			libaroma_listitem_check(
				test_list, itm+10, 0,
				main_text,
				(itm%3!=0)?extra_text:NULL,
									((itm%3==1)?list_icon:list_icon),
									LIBAROMA_LISTITEM_CHECK_INDENT_NOICON|
									LIBAROMA_LISTITEM_WITH_SEPARATOR|
									LIBAROMA_LISTITEM_CHECK_SHARED_ICON|
									add_flags,
						   -1
			);
		}
	}
	
	/* third page */
	printf("MLX: %s\n", "initializing third page");
	LIBAROMA_CONTROLP lset_list = libaroma_ctl_list(pagerwin, 1, pagerw*2, 0, pagerw, LIBAROMA_SIZE_FULL, 0, 0, RGB(FFFFFF), LIBAROMA_CTL_LIST_WITH_SHADOW|LIBAROMA_CTL_LIST_WITH_HANDLE);
	if (!lset_list) return 0;
	
#define _MKCHECK(id, selected, title) \
			libaroma_listitem_check_set_cb(lset_list, \
				libaroma_listitem_check(lset_list, id, selected, title, NULL, NULL, LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_CHECK_SWITCH, -1), \
			test_list_options, NULL)

#define _MKOPT(id, selected, title) \
			libaroma_listitem_check_set_cb(lset_list, \
				libaroma_listitem_check(lset_list, id, selected, title, NULL, NULL, LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_CHECK_OPTION, -1), \
			test_list_options, NULL)
	
	libaroma_listitem_caption(lset_list, 1, "List decorations", -1);
		_MKCHECK(ID_LSET_LISTITEM_HANDLE, 1, "Fast scroller");
		_MKCHECK(ID_LSET_LISTITEM_SHADOW, 1, "Shadow at top");
		_MKCHECK(ID_LSET_LISTITEM_GLOW, 0, "Custom overscroll");
	libaroma_listitem_check_create_named_group(lset_list, "Pager animation");
		_MKOPT(ID_PSET_LISTITEM_SLIDE, 1, "Default");
		_MKOPT(ID_PSET_LISTITEM_STRCH, 0, "Stretch");
		_MKOPT(ID_PSET_LISTITEM_CLEAN, 0, "Clean");
		_MKOPT(ID_PSET_LISTITEM_RVEAL, 0, "Reveal");
		_MKOPT(ID_PSET_LISTITEM_SCALE, 0, "Scale");
		_MKOPT(ID_PSET_LISTITEM_STKIN, 0, "Stack in");
		_MKOPT(ID_PSET_LISTITEM_STKOV, 0, "Stack over");
		_MKOPT(ID_PSET_LISTITEM_CIRCL, 0, "Circle");
		_MKOPT(ID_PSET_LISTITEM_FADE, 0, "Fade");
		_MKOPT(ID_PSET_LISTITEM_STRIN, 0, "Stretch in");
	
#undef _MKOPT
#undef _MKCHECK
	
	return 1;
}

byte appbar_win_init(LIBAROMA_CONTROLP fragment){
	LIBAROMA_WINDOWP win = libaroma_ctl_fragment_new_window(fragment, ID_FRAGMENT_SETT);
	LIBAROMA_CONTROLP abtoggle = libaroma_ctl_button(win, ID_SETT_TOGGLE, 0, 0, LIBAROMA_SIZE_FULL, 60, "Update App Bar", LIBAROMA_CTL_BUTTON_RAISED, 0);
	if (!abtoggle) return 0;
	return 1;
}

byte btns_win_init(LIBAROMA_CONTROLP fragment){
	LIBAROMA_WINDOWP win = libaroma_ctl_fragment_new_window(fragment, ID_FRAGMENT_BTNS);
	//LIBAROMA_CONTROLP  = libaroma_ctl_(win, ID_, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
	//if (!) return 0;
	return 1;
}

byte prog_win_init(LIBAROMA_CONTROLP fragment){
	LIBAROMA_WINDOWP win = libaroma_ctl_fragment_new_window(fragment, ID_FRAGMENT_PROG);
	//LIBAROMA_CONTROLP  = libaroma_ctl_(win, ID_, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
	//if (!) return 0;
	
	int y=0;
	
	/* progress bar */
	hprog = libaroma_ctl_progress(
		win, ID_PROG_HORIZ,
		0, 20, LIBAROMA_SIZE_FULL, 8,
		LIBAROMA_CTL_PROGRESS_INDETERMINATE,
		4,
		4
	);
	y+=28;
	
	/* progress circle */
	cprog = libaroma_ctl_progress(
		win, ID_PROG_CIRCLE,
		(fragment->window->width*0.5)-24, y+20, 48, 48,
		LIBAROMA_CTL_PROGRESS_INDETERMINATE|LIBAROMA_CTL_PROGRESS_CIRCULAR,
		4,
		4
	);
	//libaroma_ctl_progress_timing(horizontal_prog, 250);
	y+=68;
	
	LIBAROMA_CONTROLP btn1 = libaroma_ctl_button(
		win, ID_PROG_HINV, 0, y+20, LIBAROMA_SIZE_FULL, 60,
		"Invert horizontal", LIBAROMA_CTL_BUTTON_RAISED,
		0
	);
	y+=80;
	
	LIBAROMA_CONTROLP btn2 = libaroma_ctl_button(
		win, ID_PROG_CINV, 0, y, LIBAROMA_SIZE_FULL, 60,
		"Invert circular", LIBAROMA_CTL_BUTTON_RAISED,
		0
	);
	
	y+=60;
	
	LIBAROMA_CONTROLP btn3 = libaroma_ctl_button(
		win, ID_PROG_HUPD, 0, y, LIBAROMA_SIZE_FULL, 60,
		"Update horizontal", LIBAROMA_CTL_BUTTON_RAISED,
		0
	);
	y+=60;
	
	LIBAROMA_CONTROLP btn4 = libaroma_ctl_button(
		win, ID_PROG_CUPD, 0, y, LIBAROMA_SIZE_FULL, 60,
		"Update circular", LIBAROMA_CTL_BUTTON_RAISED,
		0
	);
	return 1;
}

byte debug_win_init(LIBAROMA_CONTROLP fragment){
	LIBAROMA_WINDOWP win = libaroma_ctl_fragment_new_window(fragment, ID_FRAGMENT_DBG);
	if (!win) return 0;
	LIBAROMA_CONTROLP dbglist = libaroma_ctl_list(win, 0, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL, 0, 0, RGB(FFFFFF), LIBAROMA_CTL_LIST_WITH_HANDLE|LIBAROMA_CTL_LIST_WITH_SHADOW);
	if (!dbglist) return 0;
	LIBAROMA_STREAMP stream = libaroma_stream("file:///efiesp/dmesg.log");
	if (stream) {
		char *dmesg = libaroma_stream_to_string(stream, 1);
		libaroma_listitem_text(dbglist, 0, dmesg, RGB(0), 0, 0, 0, LIBAROMA_LISTITEM_TEXT_FREE);
	}
	return 1;
}

byte lst_win_init(LIBAROMA_CONTROLP fragment){
	LIBAROMA_WINDOWP win = libaroma_ctl_fragment_new_window(fragment, ID_FRAGMENT_LST);
	if (!win) return 0;
	LIBAROMA_CONTROLP lstlist = libaroma_ctl_list(win, 0, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL, 0, 0, RGB(FFFFFF), LIBAROMA_CTL_LIST_WITH_HANDLE|LIBAROMA_CTL_LIST_WITH_SHADOW);
	if (!lstlist) return 0;
	LIBAROMA_STREAMP stream = libaroma_stream("file:///efiesp/dev.lst");
	if (stream) {
		char *dmesg = libaroma_stream_to_string(stream, 1);
		libaroma_listitem_text(lstlist, 0, dmesg, RGB(0), 0, 0, 0, LIBAROMA_LISTITEM_TEXT_FREE);
	}
	return 1;
}

byte xml_win_init(LIBAROMA_CONTROLP fragment){
	LIBAROMA_WINDOWP win = libaroma_ctl_fragment_new_window(fragment, ID_FRAGMENT_XML);
	
	test_xml_layout_parse(win);
	
	return 1;
}

byte test_list_options(LIBAROMA_CONTROLP ctl, LIBAROMA_CTL_LIST_ITEMP item, int id, byte checked, voidp data, byte state){
	if (state==2) return 1; // ignore callback on item destroy
	byte cur_flags;
	switch (id){
		case ID_LSET_LISTITEM_HANDLE:
			printf("Should %s fast scroller\n", checked?"disable":"enable");
			cur_flags = libaroma_ctl_scroll_getflags(test_list);
			if (checked){
				cur_flags &= ~LIBAROMA_CTL_LIST_WITH_HANDLE;
			}
			else {
				cur_flags |= LIBAROMA_CTL_LIST_WITH_HANDLE;
			}
			break;
		case ID_LSET_LISTITEM_SHADOW:
			printf("Should %s top shadow\n", checked?"disable":"enable");
			cur_flags = libaroma_ctl_scroll_getflags(test_list);
			if (checked){
				cur_flags &= ~LIBAROMA_CTL_LIST_WITH_SHADOW;
			}
			else {
				cur_flags |= LIBAROMA_CTL_LIST_WITH_SHADOW;
			}
			break;
		case ID_LSET_LISTITEM_GLOW:
			if (checked){
				libaroma_ctl_scroll_set_ovs_callback(test_list, NULL);
			}
			else {
				libaroma_ctl_scroll_set_ovs_callback(test_list, test_ovs_effect);
			}
			break;
		case ID_PSET_LISTITEM_SLIDE:
			if (!checked){
				libaroma_ctl_pager_set_animation(pager, LIBAROMA_ART_SWITCH_ANIMATION_SLIDE);
			}
			break;
		case ID_PSET_LISTITEM_STRCH:
			if (!checked){
				libaroma_ctl_pager_set_animation(pager, LIBAROMA_ART_SWITCH_ANIMATION_STRETCH);
			}
			break;
		case ID_PSET_LISTITEM_CLEAN:
			if (!checked){
				libaroma_ctl_pager_set_animation(pager, LIBAROMA_ART_SWITCH_ANIMATION_CLEAN);
			}
			break;
		case ID_PSET_LISTITEM_RVEAL:
			if (!checked){
				libaroma_ctl_pager_set_animation(pager, LIBAROMA_ART_SWITCH_ANIMATION_REVEAL);
			}
			break;
		case ID_PSET_LISTITEM_SCALE:
			if (!checked){
				libaroma_ctl_pager_set_animation(pager, LIBAROMA_ART_SWITCH_ANIMATION_SCALE);
			}
			break;
		case ID_PSET_LISTITEM_STKIN:
			if (!checked){
				libaroma_ctl_pager_set_animation(pager, LIBAROMA_ART_SWITCH_ANIMATION_STACKIN);
			}
			break;
		case ID_PSET_LISTITEM_STKOV:
			if (!checked){
				libaroma_ctl_pager_set_animation(pager, LIBAROMA_ART_SWITCH_ANIMATION_STACKOVER);
			}
			break;
		case ID_PSET_LISTITEM_CIRCL:
			if (!checked){
				libaroma_ctl_pager_set_animation(pager, LIBAROMA_ART_SWITCH_ANIMATION_CIRCLE);
			}
			break;
		case ID_PSET_LISTITEM_FADE:
			if (!checked){
				libaroma_ctl_pager_set_animation(pager, LIBAROMA_ART_SWITCH_ANIMATION_FADE);
			}
			break;
		case ID_PSET_LISTITEM_STRIN:
			if (!checked){
				libaroma_ctl_pager_set_animation(pager, LIBAROMA_ART_SWITCH_ANIMATION_STRETCH_IN);
			}
			break;
	}
	return libaroma_ctl_scroll_setflags(test_list, cur_flags);
}

#ifdef __cplusplus
}
#endif
#endif /* __test_init_c__ */  
