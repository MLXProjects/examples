/********************************************************************[libaroma]*
 * Copyright (C) 2011-2015 Ahmad Amarullah (http://amarullz.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *______________________________________________________________________________
 *
 * Filename    : full_test.c
 * Description : libaroma full test file
 *
 * + This is part of libaroma, an embedded ui toolkit.
 * + 04/02/15 - Author(s): Ahmad Amarullah
 * + 22/04/21 - Contributor: Michael Jauregui
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <aroma.h>

/* control ids, pointers and other info */
//list tab
LIBAROMA_CONTROLP main_list;
#define ID_LIST_MAIN		10
//anims tab
LIBAROMA_CONTROLP bar_upd_btn, panim_btn, txt_gap_btn;
#define ID_ANIBTN_BARUPD	20
#define ID_ANIBTN_TXTGAP	21
#define ID_ANIBTN_SETPANIM	22
//buttons tab
byte rgb_mode=0;
#define ID_BTN_EXIT			30
#define ID_BTN_RGB			31
//progress tab
byte cprog_inverted, hprog_inverted, cprog_determinate, hprog_determinate;
#define ID_PROG_CIRCULAR	40
#define ID_PROG_HORIZONTAL	41
#define ID_PROGBTN_INVERTH	42
#define ID_PROGBTN_INVERTC	43
#define ID_PROGBTN_TOGGLEH	44
#define ID_PROGBTN_TOGGLEC	45

LIBAROMA_CONTROLP exit_btn, rgb_btn, bar_upd_btn, panim_btn, txt_gap_btn, circular_prog, horizontal_prog, sb_list;
LIBAROMA_WINDOWP sidebar;
LIBAROMA_CANVASP list_icon, list_icon2, list_icon3 = NULL;

void list_tab_init(LIBAROMA_WINDOWP pagerwin, int maxwidth){

	/* list */
	main_list = libaroma_ctl_list(
		pagerwin, 77, /* win, id */
		0, 0, maxwidth, LIBAROMA_SIZE_FULL, /* x,y,w,h */
		0, 0 /*8*/, /* horiz, vert padding */
		RGB(ffffff), /* bgcolor */
		LIBAROMA_CTL_SCROLL_WITH_SHADOW
		|LIBAROMA_CTL_SCROLL_WITH_HANDLE
	);

	libaroma_listitem_image(
		main_list,1,
		libaroma_image_uri("res:///wall2.jpg"),
		120,
		LIBAROMA_LISTITEM_IMAGE_FREE|LIBAROMA_LISTITEM_WITH_SEPARATOR|
		LIBAROMA_LISTITEM_IMAGE_FILL|LIBAROMA_LISTITEM_IMAGE_PROPORTIONAL|
		LIBAROMA_CTL_LIST_ITEM_RECEIVE_TOUCH,
		-1);

	list_icon =
		libaroma_image_uri("res:///png/card.png");
	list_icon2 =
		libaroma_image_uri("res:///png/gesture.png");
	list_icon3 =
		libaroma_image_uri("res:///png/inbox.png");

	/* fill color
	libaroma_canvas_fillcolor(list_icon,libaroma_colorget(NULL,NULL)->primary);
	libaroma_canvas_fillcolor(list_icon2,libaroma_colorget(NULL,NULL)->primary);
	libaroma_canvas_fillcolor(list_icon3,libaroma_colorget(NULL,NULL)->primary);
	*/
	char main_text[256];
	char extra_text[256];
	int itm=0;
	int kdv=0;
	for (itm=0;itm<50;itm++){
		if (itm==10){
			libaroma_listitem_image(
				main_list,1,
				libaroma_image_uri("res:///png/wall1.png"),
				150,
				LIBAROMA_LISTITEM_IMAGE_FREE|LIBAROMA_LISTITEM_WITH_SEPARATOR|
				LIBAROMA_LISTITEM_IMAGE_FILL|LIBAROMA_LISTITEM_IMAGE_PROPORTIONAL|
				LIBAROMA_CTL_LIST_ITEM_RECEIVE_TOUCH|LIBAROMA_LISTITEM_IMAGE_PARALAX,
			-1);
		}

		if (itm%6==4){
			libaroma_listitem_divider(
				main_list, 1, (((kdv++)%2)==0)?LIBAROMA_LISTITEM_DIVIDER_SUBSCREEN:
					LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN, -1);
		}

		if (itm%15==0){
			snprintf(main_text,256,"List Caption %i",itm);
			libaroma_listitem_caption(
				main_list, 200, main_text, -1);
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
				main_list, itm+10, 0,
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
				main_list, itm+10, 0,
				main_text,
				(itm%3!=0)?extra_text:NULL,
				((itm%3==1)?list_icon:(itm%3==2)?list_icon2:list_icon3),
				LIBAROMA_LISTITEM_CHECK_INDENT_NOICON|
				LIBAROMA_LISTITEM_WITH_SEPARATOR|
				LIBAROMA_LISTITEM_CHECK_SHARED_ICON|
				add_flags,
				-1
			);
		}
	}
}

void anims_tab_init(LIBAROMA_WINDOWP pagerwin, int maxwidth){
	LIBAROMA_CONTROLP exit_btn = libaroma_ctl_button(
		pagerwin, ID_BTN_EXIT, maxwidth, 0, maxwidth, 60,
		"Exit Libaroma", LIBAROMA_CTL_BUTTON_COLORED|LIBAROMA_CTL_BUTTON_RAISED,
		RGB(880000)
	);

	bar_upd_btn = libaroma_ctl_button(
		pagerwin, ID_ANIBTN_BARUPD, maxwidth, 60, maxwidth, 60,
		"Update App Bar", LIBAROMA_CTL_BUTTON_RAISED, 0
	);

	txt_gap_btn = libaroma_ctl_button(
		pagerwin, ID_ANIBTN_TXTGAP, maxwidth, 120, maxwidth, 60,
		"Change App Bar Text Gap", LIBAROMA_CTL_BUTTON_RAISED, 0
	);

	panim_btn = libaroma_ctl_button(
		pagerwin, ID_ANIBTN_SETPANIM, maxwidth, 180, maxwidth, 60,
		"Pager: slide", LIBAROMA_CTL_BUTTON_RAISED, 0
	);
}

void buttons_tab_init(LIBAROMA_WINDOWP pagerwin, int maxwidth){
	rgb_btn = libaroma_ctl_button(
		pagerwin, ID_BTN_RGB, maxwidth*2, 0, maxwidth, 60,
		"Set screen RGB", LIBAROMA_CTL_BUTTON_COLORED|LIBAROMA_CTL_BUTTON_RAISED, RGB(008800)
	);

	LIBAROMA_CONTROLP buttonx_2 = libaroma_ctl_button(
		pagerwin, 181, maxwidth*2, 60, 72, 72,
		"X", LIBAROMA_CTL_BUTTON_COLORED|LIBAROMA_CTL_BUTTON_RAISED|LIBAROMA_CTL_BUTTON_CIRCLE,
		RGB(44688)
	);

	LIBAROMA_CONTROLP buttonx_3 = libaroma_ctl_button(
		pagerwin, 182, maxwidth*2+72, 60, 72, 72,
		"A", LIBAROMA_CTL_BUTTON_COLORED|LIBAROMA_CTL_BUTTON_RAISED|LIBAROMA_CTL_BUTTON_CIRCLE,
		RGB(ffffff)
	);
}

void progress_tab_init(LIBAROMA_WINDOWP pagerwin, int maxwidth){

	int y=0;

	/* progress bar */
	horizontal_prog = libaroma_ctl_progress(
		pagerwin, ID_PROG_HORIZONTAL,
		maxwidth*3, 20, maxwidth, 8,
		LIBAROMA_CTL_PROGRESS_INDETERMINATE,
		4,
		4
	);
	hprog_determinate=0;
	hprog_inverted=0;
	y+=28;

	/* progress circle */
	circular_prog = libaroma_ctl_progress(
		pagerwin, ID_PROG_CIRCULAR,
		(maxwidth*3.5)-24, y+20, 48, 48,
		LIBAROMA_CTL_PROGRESS_INDETERMINATE|LIBAROMA_CTL_PROGRESS_CIRCULAR,
		4,
		4
	);
	//libaroma_ctl_progress_timing(horizontal_prog, 250);
	y+=68;
	cprog_determinate=0;
	cprog_inverted=0;

	LIBAROMA_CONTROLP btn1 = libaroma_ctl_button(
		pagerwin, ID_PROGBTN_INVERTH, maxwidth*3, y+20, maxwidth, 60,
		"Invert horizontal", LIBAROMA_CTL_BUTTON_RAISED,
		0
	);
	y+=80;

	LIBAROMA_CONTROLP btn2 = libaroma_ctl_button(
		pagerwin, ID_PROGBTN_INVERTC, maxwidth*3, y, maxwidth, 60,
		"Invert circular", LIBAROMA_CTL_BUTTON_RAISED,
		0
	);

	y+=60;

	LIBAROMA_CONTROLP btn3 = libaroma_ctl_button(
		pagerwin, ID_PROGBTN_TOGGLEH, maxwidth*3, y, maxwidth, 60,
		"Update horizontal", LIBAROMA_CTL_BUTTON_RAISED,
		0
	);
	y+=60;

	LIBAROMA_CONTROLP btn4 = libaroma_ctl_button(
		pagerwin, ID_PROGBTN_TOGGLEC, maxwidth*3, y, maxwidth, 60,
		"Update circular", LIBAROMA_CTL_BUTTON_RAISED,
		0
	);

	//invert horizontal
	//invert circular
	//make horizontal determinate
	//make circular determinate
	//hold for indeterminate/determinate
}

void sidebar_init(LIBAROMA_WINDOWP win){
	sidebar=libaroma_window_sidebar(win,0);
	if (sidebar){
		printf("SIDEBAR INITIALIZED\n");

		/* list */
		sb_list = libaroma_ctl_list(
			sidebar, 88, /* win, id */
			0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL, /* x,y,w,h */
			0, 0 /*8*/, /* horiz, vert padding */
			RGB(ffffff), /* bgcolor */
			LIBAROMA_CTL_SCROLL_WITH_SHADOW
		);

		libaroma_listitem_image(
			sb_list,1,
			libaroma_image_uri("res:///png/wall2.png"),
			150,
			LIBAROMA_LISTITEM_IMAGE_FREE|LIBAROMA_LISTITEM_WITH_SEPARATOR|
			LIBAROMA_LISTITEM_IMAGE_FILL|LIBAROMA_LISTITEM_IMAGE_PROPORTIONAL|
			LIBAROMA_CTL_LIST_ITEM_RECEIVE_TOUCH,
		-1);
		int r;
		int itm=0;
		int kdv=0;
		char xtext[256];
		for (r=0;r<20;r++){
			if (r%5==0){
				snprintf(xtext,256,"SIDEBAR CAPTION %i",itm);
				libaroma_listitem_caption_color(
					sb_list, 200, xtext, RGB(888888), -1);
			}

			if (r%3==0){
				libaroma_listitem_divider(
					sb_list, 1, (((kdv++)%2)==0)?LIBAROMA_LISTITEM_DIVIDER_SUBSCREEN:
						LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN,
						-1);
			}

			snprintf(xtext,256,"Sidebar Menu #%i",r);
			libaroma_listitem_menu(
				sb_list, r+100,
				xtext, NULL,
				((r%3==1)?list_icon:(r%3==2)?list_icon2:list_icon3),
				LIBAROMA_LISTITEM_CHECK_INDENT_NOICON|
				LIBAROMA_LISTITEM_CHECK_SHARED_ICON|
				LIBAROMA_LISTITEM_CHECK_SMALL_ICON|
				0,
				-1
			);
		}

	}
	else{
		printf("SIDEBAR FAILED\n");
	}

}

/*void _tab_init(LIBAROMA_WINDOWP pagerwin, int maxwidth){

}*/

void full_test(){

	byte anim=LIBAROMA_WINDOW_SHOW_ANIMATION_CIRCLE;
	LIBAROMA_WINDOWP win = libaroma_window(
		NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);

	int _y = 0;
	LIBAROMA_CONTROLP bar = libaroma_ctl_bar(
		win, 1,
		0, _y, LIBAROMA_SIZE_FULL, 56,
		"Libaroma Test", RGB(009385), RGB(ffffff)
	);

	libaroma_ctl_bar_set_icon_mask(bar,1,1);

	_y+=56;
	LIBAROMA_CONTROLP tabs=libaroma_ctl_tabs(
		win, 2,
		0, _y, LIBAROMA_SIZE_FULL, 48,
		RGB(009385), RGB(ffffff), 0, 0
	);

	_y+=48;
	LIBAROMA_CONTROLP pager=libaroma_ctl_pager(
		win, 3, 5,
		0, _y, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL
	);

	/* set pager & tab text */
	char * tab_texts[5]={ "LIST", "ANIMATIONS", "BUTTONS", "PROGRESS", "FIFTH TABS" };
	libaroma_ctl_tabs_set_texts(tabs,tab_texts,5,0);
	libaroma_ctl_tabs_set_pager(tabs,pager);

	/* pager window */
	LIBAROMA_WINDOWP win2 = libaroma_ctl_pager_get_window(pager);
	int pw = libaroma_px(pager->w);

	list_tab_init(win2, pw);

	anims_tab_init(win2, pw);

	buttons_tab_init(win2, pw);

	progress_tab_init(win2, pw);

	/* set bar tools */
	LIBAROMA_CTL_BAR_TOOLSP bar_tools=libaroma_ctl_bar_tools(2);
	libaroma_ctl_bar_tools_set(
		bar_tools, 0, 1, "one", list_icon, LIBAROMA_CTL_BAR_TOOL_ICON_SHARED
	);
	libaroma_ctl_bar_tools_set(
		bar_tools, 1, 3, "two", list_icon,
		LIBAROMA_CTL_BAR_TOOL_SWITCH|LIBAROMA_CTL_BAR_TOOL_ICON_SHARED
	);
	libaroma_ctl_bar_set_tools(
		bar,bar_tools, 0);
	libaroma_ctl_bar_set_icon(
		bar,NULL,0,LIBAROMA_CTL_BAR_ICON_DRAWER, 1
	);

	sidebar_init(win);

	/*
	LIBAROMA_CONTROLP buttonsidebar = libaroma_ctl_button(
		sidebar, 88, 0, 0, LIBAROMA_SIZE_FULL, 60,
		"Side Button", LIBAROMA_CTL_BUTTON_COLORED, RGB(008800)
	);
	*/
	// libaroma_window_layer_init(win);

	libaroma_window_anishow(win, anim, 350);
	libaroma_png_save(libaroma_fb()->canvas, "./ss.png");
	// libaroma_window_layer_init(win);
	libaroma_window_invalidate(win, 1);
	byte gap_wide = 1;
	int change_id=0;
	int pager_anim=0;
	byte onpool=1;
	do{
		LIBAROMA_MSG msg;
		dword command	=libaroma_window_pool(win,&msg);
		byte cmd		=LIBAROMA_CMD(command);
		word id			=LIBAROMA_CMD_ID(command);
		byte param		=LIBAROMA_CMD_PARAM(command);

		if (msg.msg==LIBAROMA_MSG_KEY_SELECT || msg.msg==LIBAROMA_MSG_KEY_POWER){
			if (msg.state==0){
				printf("Screenshot... and exit\n");
				libaroma_png_save(libaroma_fb()->canvas,"/tmp/libaroma_screenshot.png");
				onpool = 0;
				break;
			}
		}
		else if (cmd){
			if (cmd==1){
				if (id==1){
					if (param==1){
						// drawer icon touched
						printf("Open sidebar %i\n",
							libaroma_window_sidebar_show(sidebar, 1)
						);
					}
				}
				else if (id==ID_BTN_EXIT){
					printf("Exit Button Pressed...\n");
					onpool = 0;
				}
				else if (id==ID_BTN_RGB){
					if (rgb_mode){
						rgb_mode=0;
						libaroma_ctl_button_text(rgb_btn, "Set screen RGB");
						libaroma_fb_setrgb(16, 8, 0);
					}
					else {
						rgb_mode=1;
						libaroma_ctl_button_text(rgb_btn, "Set screen BGR");
						libaroma_fb_setrgb(0, 8, 16);
					}
				}
						else if (id==182){
					//LINUXDRM_setrgbpos(libaroma_fb(), 16, 8, 0);
				}
				else if (id==ID_ANIBTN_SETPANIM){
					pager_anim++;
					if (pager_anim>10) pager_anim=0;
					byte panim;
					switch (pager_anim){
						case 0:
							panim=LIBAROMA_CTL_PAGER_ANIMATION_SLIDE;
							libaroma_ctl_button_text(panim_btn, "Pager: slide");
						break;
						case 1:
							panim=LIBAROMA_CTL_PAGER_ANIMATION_STRETCH;
							libaroma_ctl_button_text(panim_btn, "Pager: stretch");
						break;
						case 2:
							panim=LIBAROMA_CTL_PAGER_ANIMATION_CLEAN;
							libaroma_ctl_button_text(panim_btn, "Pager: clean");
						break;
						case 3:
							panim=LIBAROMA_CTL_PAGER_ANIMATION_REVEAL;
							libaroma_ctl_button_text(panim_btn, "Pager: reveal");
						break;
						case 4:
							panim=LIBAROMA_CTL_PAGER_ANIMATION_SCALE;
							libaroma_ctl_button_text(panim_btn, "Pager: scale");
						break;
						case 5:
							panim=LIBAROMA_CTL_PAGER_ANIMATION_STACKIN;
							libaroma_ctl_button_text(panim_btn, "Pager: stack in");
						break;
						case 6:
							panim=LIBAROMA_CTL_PAGER_ANIMATION_STACKOVER;
							libaroma_ctl_button_text(panim_btn, "Pager: stack over");
						break;
						case 7:
							panim=LIBAROMA_ART_SWITCH_ANIMATION_CIRCLE;
							libaroma_ctl_button_text(panim_btn, "Pager: circle");
						break;
						case 8:
							panim=LIBAROMA_ART_SWITCH_ANIMATION_FADE;
							libaroma_ctl_button_text(panim_btn, "Pager: fade");
						break;
						case 9:
							panim=LIBAROMA_ART_SWITCH_ANIMATION_STRETCH_IN;
							libaroma_ctl_button_text(panim_btn, "Pager: stretch in");
						break;
						case 10:
							panim=LIBAROMA_ART_SWITCH_ANIMATION_HORIZ_STRIPES;
							libaroma_ctl_button_text(panim_btn, "Pager: stripes");
						break;
					}
					libaroma_ctl_pager_set_animation(pager, panim);
				}
				else if (id==ID_ANIBTN_TXTGAP){
					libaroma_ctl_bar_set_textgap(bar,gap_wide,1);
					gap_wide=gap_wide?0:1;
				}
				else if (id==ID_ANIBTN_BARUPD){
					if (change_id==0){
						libaroma_ctl_bar_set_tools(
							bar,NULL, 0);
						libaroma_ctl_bar_set_subtitle(
							bar,
							"an embedded ui toolkit", 0
						);
						libaroma_ctl_bar_set_color(
							bar, RGB(9C27B0), RGB(ffffff), 1
						);
						libaroma_ctl_tabs_set_color(
							tabs,RGB(9C27B0), RGB(ffffff),1);
					}
					else if (change_id==1){
						libaroma_ctl_bar_set_subtitle(
							bar,
							NULL, 0
						);
						libaroma_ctl_bar_set_title(
							bar,
							"Inbox", 1
						);
					}
					else if (change_id==2){
						libaroma_ctl_bar_set_touchable_title(bar,1);
						libaroma_ctl_bar_set_subtitle(
							bar,
							"Title now touchable", 0
						);
						libaroma_ctl_bar_set_color(
							bar, RGB(009385), RGB(ffffff), 0
						);
						libaroma_ctl_tabs_set_color(
							tabs,RGB(009385), RGB(ffffff),1);
						libaroma_ctl_bar_set_icon(
							bar,list_icon,0,0,1
						);
					}
					else if (change_id==3){
						libaroma_ctl_bar_set_tools(
							bar,bar_tools, 1);
					}
					else if (change_id==4){
						libaroma_ctl_bar_set_subtitle(
							bar,
							NULL, 0
						);
						libaroma_ctl_bar_set_menuid(bar,50,1);
					}
					else if (change_id==5){
						libaroma_ctl_bar_tools_set(
							bar_tools, 1, 3, "three", list_icon,
							LIBAROMA_CTL_BAR_TOOL_SWITCH|LIBAROMA_CTL_BAR_TOOL_ICON_SHARED
						);
						libaroma_ctl_bar_set_icon(
							bar,NULL,0,LIBAROMA_CTL_BAR_ICON_DRAWER,1
						);
					}
					else if ((change_id==6)||(change_id==8)){
						libaroma_ctl_bar_tools_set(
							bar_tools, 1, 3, "three", NULL,
							LIBAROMA_CTL_BAR_TOOL_SWITCH|
							LIBAROMA_CTL_BAR_TOOL_SWITCH_CHECKED
						);
						libaroma_ctl_bar_set_icon(
							bar,NULL,0,LIBAROMA_CTL_BAR_ICON_ARROW,1
						);
					}
					else if (change_id==7){
						libaroma_ctl_bar_set_touchable_title(bar,0);
						libaroma_ctl_bar_tools_set(
							bar_tools, 1, 3, "three", NULL,
							LIBAROMA_CTL_BAR_TOOL_SWITCH
						);
						libaroma_ctl_bar_set_title(
							bar,
							"Libaroma Demo", 0
						);
						libaroma_ctl_bar_set_subtitle(
							bar,
							"an embedded ui toolkit...", 0
						);
						libaroma_ctl_bar_set_icon(
							bar,NULL,0,LIBAROMA_CTL_BAR_ICON_DRAWER,1
						);
					}
					else if (change_id==9){
						libaroma_ctl_bar_tools_set(
							bar_tools, 1, 3, "three", list_icon,
							LIBAROMA_CTL_BAR_TOOL_SWITCH|LIBAROMA_CTL_BAR_TOOL_ICON_SHARED
						);
						libaroma_ctl_bar_set_icon(
							bar,list_icon,0,0,1
						);
						libaroma_ctl_bar_set_color(
							bar, RGB(311B92), RGB(ffffff), 1
						);
						libaroma_ctl_tabs_set_color(
							tabs,RGB(311B92), RGB(ffffff),1);
					}
					else if (change_id==10){
						libaroma_ctl_bar_set_subtitle(
							bar,
							NULL, 0
						);
						libaroma_ctl_bar_set_menuid(bar,0,0);
						libaroma_ctl_bar_set_tools(
							bar,NULL, 0);
						libaroma_ctl_bar_set_title(
							bar,
							"Libaroma Test", 0
						);
						libaroma_ctl_bar_set_icon(
							bar,NULL,0,0,1
						);
						change_id=-1;
					}
					change_id++;
				}
				else if (id==ID_PROGBTN_INVERTH){
					byte flags = LIBAROMA_CTL_PROGRESS_INDETERMINATE;
					if (hprog_determinate){
						hprog_determinate=0;
						int max=libaroma_ctl_progress_get_max(horizontal_prog);
						libaroma_ctl_progress_value(horizontal_prog, max);
					}
					if (hprog_inverted)
						hprog_inverted=0;
					else {
						flags |= LIBAROMA_CTL_PROGRESS_QUERY;
						hprog_inverted=1;
					}
					libaroma_ctl_progress_type(horizontal_prog, flags);
				}
				else if (id==ID_PROGBTN_INVERTC){
					byte flags = LIBAROMA_CTL_PROGRESS_CIRCULAR;
					if (cprog_determinate){
						cprog_determinate=0;
						int max=libaroma_ctl_progress_get_max(circular_prog);
						libaroma_ctl_progress_value(circular_prog, max);
					}
					if (cprog_inverted){
						cprog_inverted=0;
						flags |= LIBAROMA_CTL_PROGRESS_INDETERMINATE;
					}
					else {
						flags |= LIBAROMA_CTL_PROGRESS_QUERY;
						cprog_inverted=1;
					}
					libaroma_ctl_progress_type(circular_prog, flags);
				}
				else if (id==ID_PROGBTN_TOGGLEH){
					int cur_val=libaroma_ctl_progress_get_value(horizontal_prog);

					//if current value is lower than the max one, increase it
					if (cur_val<libaroma_ctl_progress_get_max(horizontal_prog))
						libaroma_ctl_progress_value(horizontal_prog, cur_val+1);
					else {
						//otherwise, switch between determinate and indeterminate
						byte flags = LIBAROMA_CTL_PROGRESS_DETERMINATE;
						if (hprog_determinate){
							hprog_determinate=0;
							flags = LIBAROMA_CTL_PROGRESS_INDETERMINATE;
						}
						else {
							hprog_determinate=1;
							//if going to determinate, reset value
							libaroma_ctl_progress_value(horizontal_prog, 0);
						}
						libaroma_ctl_progress_type(horizontal_prog, flags);
					}
				}
				else if (id==ID_PROGBTN_TOGGLEC){
					int cur_val=libaroma_ctl_progress_get_value(circular_prog);

					//if current value is lower than the max one, increase it
					if (cur_val<libaroma_ctl_progress_get_max(circular_prog))
						libaroma_ctl_progress_value(circular_prog, cur_val+1);
					else {
						//otherwise, switch between determinate and indeterminate
						byte flags = LIBAROMA_CTL_PROGRESS_CIRCULAR;
						if (cprog_determinate){
							cprog_determinate=0;
							flags |= LIBAROMA_CTL_PROGRESS_INDETERMINATE;
						}
						else {
							cprog_determinate=1;
							flags |= LIBAROMA_CTL_PROGRESS_DETERMINATE;
							//if going to determinate, reset value
							libaroma_ctl_progress_value(circular_prog, 0);
						}
						libaroma_ctl_progress_type(circular_prog, flags);
					}
				}
			}
			else if (cmd==LIBAROMA_CMD_HOLD){
				if (id==ID_ANIBTN_SETPANIM){
					pager_anim--;
					if (pager_anim<0) pager_anim=10;
					byte panim;
					switch (pager_anim){
						case 0:
							panim=LIBAROMA_CTL_PAGER_ANIMATION_SLIDE;
							libaroma_ctl_button_text(panim_btn, "Pager: slide");
						break;
						case 1:
							panim=LIBAROMA_CTL_PAGER_ANIMATION_STRETCH;
							libaroma_ctl_button_text(panim_btn, "Pager: stretch");
						break;
						case 2:
							panim=LIBAROMA_CTL_PAGER_ANIMATION_CLEAN;
							libaroma_ctl_button_text(panim_btn, "Pager: clean");
						break;
						case 3:
							panim=LIBAROMA_CTL_PAGER_ANIMATION_REVEAL;
							libaroma_ctl_button_text(panim_btn, "Pager: reveal");
						break;
						case 4:
							panim=LIBAROMA_CTL_PAGER_ANIMATION_SCALE;
							libaroma_ctl_button_text(panim_btn, "Pager: scale");
						break;
						case 5:
							panim=LIBAROMA_CTL_PAGER_ANIMATION_STACKIN;
							libaroma_ctl_button_text(panim_btn, "Pager: stack in");
						break;
						case 6:
							panim=LIBAROMA_CTL_PAGER_ANIMATION_STACKOVER;
							libaroma_ctl_button_text(panim_btn, "Pager: stack over");
						break;
						case 7:
							panim=LIBAROMA_ART_SWITCH_ANIMATION_CIRCLE;
							libaroma_ctl_button_text(panim_btn, "Pager: circle");
						break;
						case 8:
							panim=LIBAROMA_ART_SWITCH_ANIMATION_FADE;
							libaroma_ctl_button_text(panim_btn, "Pager: fade");
						break;
						case 9:
							panim=LIBAROMA_ART_SWITCH_ANIMATION_STRETCH_IN;
							libaroma_ctl_button_text(panim_btn, "Pager: stretch in");
						break;
						case 10:
							panim=LIBAROMA_ART_SWITCH_ANIMATION_HORIZ_STRIPES;
							libaroma_ctl_button_text(panim_btn, "Pager: stripes");
						break;
					}
					libaroma_ctl_pager_set_animation(pager, panim);
				}
			}

			printf("Window Command = (CMD: %i, ID: %i, Param: %i)\n",
				LIBAROMA_CMD(command),
				LIBAROMA_CMD_ID(command),
				LIBAROMA_CMD_PARAM(command)
			);
		}
		/*else if (msg.msg==LIBAROMA_MSG_TOUCH){
		printf("TEST(): TOUCH AT X=%d, Y=%d (W=%d, H=%d)\n", msg.x, msg.y, win->w, win->h);
		}*/
	}
	while(onpool);

	libaroma_ctl_bar_tools_free(bar_tools);

	libaroma_canvas_free(list_icon);
	libaroma_canvas_free(list_icon2);
	libaroma_canvas_free(list_icon3);

	printf("Free Window\n");
	libaroma_window_aniclose(win, anim, 350);
	printf("FREED\n");
}

#ifdef __cplusplus
}
#endif
