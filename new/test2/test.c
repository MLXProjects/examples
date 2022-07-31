#ifndef __test_winmain_c__
#define __test_winmain_c__
#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

void test_loop(LIBAROMA_WINDOWP win);

int active_fragment;
LIBAROMA_CANVASP ab_icon;
LIBAROMA_CTL_BAR_TOOLSP bar_tools;
LIBAROMA_CONTROLP appbar, fragment, pager, test_list, hprog, cprog;

byte test(){
	/* create main app window */
	printf("MLX: %s\n", "creating main window");
	LIBAROMA_WINDOWP win = libaroma_window(NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
	if (!test_init(win)) return 0;
	
	printf("MLX: %s\n", "showing main window");
	libaroma_window_anishow(win, LIBAROMA_WINDOW_SHOW_ANIMATION_PAGE_LEFT, 6000);
		
	/* main window loop */
	printf("MLX: %s\n", "running window loop");
	test_loop(win);
	
	/* close & free window */
	printf("MLX: %s\n", "closing window");
	libaroma_window_aniclose(win, LIBAROMA_WINDOW_SHOW_ANIMATION_PAGE_LEFT, 6000);
	/* free appbar icon */
	//libaroma_canvas_free(ab_icon);
	
	printf("MLX: bye\n");
	return 1;
}

void test_loop(LIBAROMA_WINDOWP win){
	byte runapp = 1;
	int change_id=0;
	byte hprog_inverted=0, cprog_inverted=0, hprog_determinate=0, cprog_determinate;
	LIBAROMA_RECTP last_rect;
	LIBAROMA_MSG msg;
	dword command;
	byte cmd, param;
	word id;
	do {
		command	= libaroma_window_pool(win,&msg);
		cmd		= LIBAROMA_CMD(command);
		id		= LIBAROMA_CMD_ID(command);
		param	= LIBAROMA_CMD_PARAM(command);
		
		if (msg.msg==LIBAROMA_MSG_EXIT || msg.msg==LIBAROMA_MSG_KEY_SELECT || msg.msg==LIBAROMA_MSG_KEY_POWER){
			if (msg.state==0){
				printf("Screenshot... and exit\n");
				libaroma_png_save(libaroma_fb()->canvas,"./test2.png");
				runapp = 0;
				break;
			}
		}
		else if (cmd){
			printf("MLX: id=%i, msg.key=%i\n", id, msg.key);
			if (id == ID_APPBAR){
				if (msg.key==0){ //appbar arrow/drawer button
					if (active_fragment != ID_FRAGMENT_MAIN) 
						test_switch_window(ID_FRAGMENT_MAIN, 1, last_rect);
				}
				continue;
			}
			switch (active_fragment){
				case ID_FRAGMENT_MAIN:{
					LIBAROMA_RECT rect = {0};
					LIBAROMA_CONTROLP list = (LIBAROMA_CONTROLP) msg.d;
					LIBAROMA_CTL_LIST_ITEMP item = libaroma_ctl_list_get_touched_item(list);
					libaroma_ctl_list_item_position(list, item, &rect, 0);
					switch (msg.key){/*
						case ID_MAIN_LISTITEM_DBG:
							test_switch_window(ID_FRAGMENT_DBG, 0, &rect);
							break;
						case ID_MAIN_LISTITEM_LST:
							test_switch_window(ID_FRAGMENT_LST, 0, &rect);
							break;*/
						case ID_MAIN_LISTITEM_SETT:
							test_switch_window(ID_FRAGMENT_SETT, 0, &rect);
							break;
						case ID_MAIN_LISTITEM_BTNS:;
							//test_switch_window(ID_FRAGMENT_BTNS, 0, &rect);
							/*int oldsz[4]={0, 0, libaroma_dp(win->width), libaroma_dp(win->height)};
							printf("MLX: old size %dx%d (%ddpx%ddp)\n", oldsz[2], oldsz[3], win->width, win->height);
							libaroma_fb_setdpi(120);
							libaroma_window_resize(win, oldsz[0], oldsz[1], libaroma_px(oldsz[2]), libaroma_px(oldsz[3]));
							printf("MLX: new size %dx%d (%ddpx%ddp)\n", win->w, win->h, win->width, win->height);*/
							break;
						case ID_MAIN_LISTITEM_PROG:
							test_switch_window(ID_FRAGMENT_PROG, 0, &rect);
							break;
						case ID_MAIN_LISTITEM_XML:
							test_switch_window(ID_FRAGMENT_XML, 0, &rect);
							break;
						case ID_MAIN_LISTITEM_EXIT:
							runapp=0;
							break;
					}
					last_rect = &rect;
				} break;
				case ID_FRAGMENT_SETT:{
					if (id==ID_SETT_TOGGLE){
						if (change_id==0){
							libaroma_ctl_bar_set_tools(
								appbar,NULL, 0);
							libaroma_ctl_bar_set_subtitle(
								appbar,
								"an embedded ui toolkit", 0
							);
							libaroma_ctl_bar_set_color(
								appbar, RGB(9C27B0), RGB(ffffff), 1
							);
						}
						else if (change_id==1){
							libaroma_ctl_bar_set_subtitle(
								appbar,
								NULL, 0
							);
							libaroma_ctl_bar_set_title(
								appbar,
								"Inbox", 1
							);
						}
						else if (change_id==2){
							libaroma_ctl_bar_set_touchable_title(appbar,1);
							libaroma_ctl_bar_set_subtitle(
								appbar,
								"Title now touchable", 0
							);
							libaroma_ctl_bar_set_color(
								appbar, RGB(009385), RGB(ffffff), 0
							);
							libaroma_ctl_bar_set_icon(
								appbar,ab_icon,0,0,1
							);
						}
						else if (change_id==3){
							libaroma_ctl_bar_set_tools(
								appbar,bar_tools, 1);
						}
						else if (change_id==4){
							libaroma_ctl_bar_set_subtitle(
								appbar,
								NULL, 0
							);
							libaroma_ctl_bar_set_menuid(appbar,50,1);
						}
						else if (change_id==5){
							libaroma_ctl_bar_tools_set(
								bar_tools, 1, 3, "three", ab_icon,
								LIBAROMA_CTL_BAR_TOOL_SWITCH|LIBAROMA_CTL_BAR_TOOL_ICON_SHARED
							);
							libaroma_ctl_bar_set_icon(
								appbar,NULL,0,LIBAROMA_CTL_BAR_ICON_DRAWER,1
							);
						}
						else if ((change_id==6)||(change_id==8)){
							libaroma_ctl_bar_tools_set(
								bar_tools, 1, 3, "three", NULL,
								LIBAROMA_CTL_BAR_TOOL_SWITCH|
								LIBAROMA_CTL_BAR_TOOL_SWITCH_CHECKED
							);
							libaroma_ctl_bar_set_icon(
								appbar,NULL,0,LIBAROMA_CTL_BAR_ICON_ARROW,1
							);
						}
						else if (change_id==7){
							libaroma_ctl_bar_set_touchable_title(appbar,0);
							libaroma_ctl_bar_tools_set(
								bar_tools, 1, 3, "three", NULL,
								LIBAROMA_CTL_BAR_TOOL_SWITCH
							);
							libaroma_ctl_bar_set_title(
								appbar,
								"Libaroma Demo", 0
							);
							libaroma_ctl_bar_set_subtitle(
								appbar,
								"an embedded ui toolkit...", 0
							);
							libaroma_ctl_bar_set_icon(
								appbar,NULL,0,LIBAROMA_CTL_BAR_ICON_DRAWER,1
							);
						}
						else if (change_id==9){
							libaroma_ctl_bar_tools_set(
								bar_tools, 1, 3, "three", ab_icon,
								LIBAROMA_CTL_BAR_TOOL_SWITCH|LIBAROMA_CTL_BAR_TOOL_ICON_SHARED
							);
							libaroma_ctl_bar_set_icon(
								appbar,ab_icon,0,0,1
							);
							libaroma_ctl_bar_set_color(
								appbar, RGB(311B92), RGB(ffffff), 1
							);
						}
						else if (change_id==10){
							libaroma_ctl_bar_set_subtitle(
								appbar,
								NULL, 0
							);
							libaroma_ctl_bar_set_menuid(appbar,0,0);
							libaroma_ctl_bar_set_tools(
								appbar,NULL, 0);
							libaroma_ctl_bar_set_title(
								appbar,
								"Libaroma Test", 0
							);
							libaroma_ctl_bar_set_icon(
								appbar,NULL,0,0,1
							);
							change_id=-1;
						}
						change_id++;
					}
				} break;
				case ID_FRAGMENT_BTNS:{
					
				} break;
				case ID_FRAGMENT_PROG:{
					switch (id){
						
						case ID_PROG_HINV:{
							byte flags = LIBAROMA_CTL_PROGRESS_INDETERMINATE;
							if (hprog_determinate){
								hprog_determinate=0;
								int max=libaroma_ctl_progress_get_max(hprog);
								libaroma_ctl_progress_value(hprog, max);
							}
							if (hprog_inverted)
								hprog_inverted=0;
							else {
								flags |= LIBAROMA_CTL_PROGRESS_QUERY;
								hprog_inverted=1;
							}
							libaroma_ctl_progress_type(hprog, flags);
						} break;
						case ID_PROG_CINV:{
							byte flags = LIBAROMA_CTL_PROGRESS_CIRCULAR;
							if (cprog_determinate){
								cprog_determinate=0;
								int max=libaroma_ctl_progress_get_max(cprog);
								libaroma_ctl_progress_value(cprog, max);
							}
							if (cprog_inverted){
								cprog_inverted=0;
								flags |= LIBAROMA_CTL_PROGRESS_INDETERMINATE;
							}
							else {
								flags |= LIBAROMA_CTL_PROGRESS_QUERY;
								cprog_inverted=1;
							}
							libaroma_ctl_progress_type(cprog, flags);
						} break;
						case ID_PROG_HUPD:{
							int cur_val=libaroma_ctl_progress_get_value(hprog);
							
							//if current value is lower than the max one, increase it
							if (cur_val<libaroma_ctl_progress_get_max(hprog))
								libaroma_ctl_progress_value(hprog, cur_val+1);
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
									libaroma_ctl_progress_value(hprog, 0);
								}
								libaroma_ctl_progress_type(hprog, flags);
							}
						} break;
						case ID_PROG_CUPD:{
							int cur_val=libaroma_ctl_progress_get_value(cprog);
							
							//if current value is lower than the max one, increase it
							if (cur_val<libaroma_ctl_progress_get_max(cprog))
								libaroma_ctl_progress_value(cprog, cur_val+1);
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
									libaroma_ctl_progress_value(cprog, 0);
								}
								libaroma_ctl_progress_type(cprog, flags);
							}
						} break;
					}
				} break;
				case ID_FRAGMENT_XML:{
					
				} break;
			}
		}
	} while (runapp);
}
byte fraganim_ran;

byte test_switch_window(int id, byte isback, LIBAROMA_RECTP rect){
	fraganim_ran=0;
	active_fragment = id;
	printf("MLX: %s %d\n", "switching to id", id);
	libaroma_ctl_fragment_set_active_window(fragment, id, 0, 450, 0, fragani_default, isback?NULL:rect, isback?rect:NULL);
	printf("MLX: id %d %s\n", id, "is now active");
	libaroma_ctl_bar_set_icon(
		appbar,NULL,0,isback?LIBAROMA_CTL_BAR_ICON_DRAWER:LIBAROMA_CTL_BAR_ICON_ARROW, 0
	);
	switch (id){
		case ID_FRAGMENT_MAIN:
			libaroma_ctl_bar_set_title(appbar, "Libaroma test", 0);
			break;
		case ID_FRAGMENT_SETT:
			libaroma_ctl_bar_set_title(appbar, "Appbar settings", 0);
			break;
		case ID_FRAGMENT_BTNS:
			libaroma_ctl_bar_set_title(appbar, "Buttons", 0);
			break;
		case ID_FRAGMENT_PROG:
			libaroma_ctl_bar_set_title(appbar, "Progress bars", 0);
			break;
		case ID_FRAGMENT_XML:
			libaroma_ctl_bar_set_title(appbar, "XML layout", 0);
			break;
	}
	libaroma_ctl_bar_update(appbar);
	printf("MLX: %s\n", "appbar updated");
	return 1;
}
void fragani_default(
	LIBAROMA_CANVASP dst,	//currently active window canvas (ironic, isn't it?)
	LIBAROMA_CANVASP bottom,//who knows? I don't
	LIBAROMA_CANVASP top,	//target window canvas
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
	
	if (opening && !fraganim_ran){
		//clean target canvas if target fragment is btns
		//if (active_fragment==ID_FRAGMENT_BTNS) libaroma_canvas_setcolor(top, libaroma_colorget(NULL, NULL)->window_bg, 0xFF);
		//and fix this, fragment really needs some more work
		libaroma_draw(bottom, dst, 0, 0, 0);
		//finally, don't run this again until another close/open
		fraganim_ran=1;
	}
	
	libaroma_draw_ex(dst, bottom, 0, 0, 0, (r1->y+r1->h)-to_y, dst->w, to_y, 0, 0xFF);
	libaroma_draw_ex(dst, bottom, 0, to_y+to_h, 0, r1->y+r1->h, dst->w, dst->h-(r1->y+r1->h), 0, 0xFF);
	
	if (test_config()->fragani_scale)
		libaroma_draw_scale_nearest(dst, top, 0, to_y, dst->w, to_h, 0, 0, top->w, top->h);
	else libaroma_draw_ex(dst, top, 0, to_y, 0, 0, dst->w, to_h, 0, 0xFF);
}

void test_ovs_effect(LIBAROMA_CONTROLP list, LIBAROMA_CANVASP canvas, byte istop){
	word color = libaroma_colorget(NULL, NULL)->primary;
	if (istop)
		libaroma_gradient_ex(canvas, 0, 0, canvas->w, canvas->h*0.5, color, color, 0, 0, 0xFF, 0x0);
	else
		libaroma_gradient_ex(canvas, 0, canvas->h*0.5, canvas->w, canvas->h-(canvas->h*0.5), color, color, 0, 0, 0x0, 0xFF);
}

#ifdef __cplusplus
}
#endif
#endif /* __test_winmain_c__ */ 
