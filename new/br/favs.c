#ifndef __br_favs_c__
#define __br_favs_c__

#include "main.h"

//SAVED_VERSP *favorites;
LIBAROMA_STACKP favorites;
byte vers_selmode;
int vers_selcount;
//static int favs_count;

byte _favs_alloc(){
	vers_selmode=0;
	vers_selcount=0;
	//favorites=malloc(sizeof(SAVED_VERSP));
	favorites=libaroma_stack(NULL);
	if (!favorites){
		printf("favs_init: failed to allocate stack\n");
		return 0;
	}
	/* TODO: load favorites from somewhere (e.g. settings file) */
	return 1;
}

byte favs_clean(){
	libaroma_stack_free(favorites);
	favorites=NULL;
	return 1;
}

void favs_add(SAVED_VERSP vers){
	if (!vers){
		printf("favs_add: cannot add null item to stack\n");
		return;
	}
	if (vers->saved) return; /* vers already saved */
	libaroma_stack_add_at(favorites, favorites->n, (voidp)vers, sizeof(SAVED_VERSP));
	vers->saved=1;
}

void favs_del(SAVED_VERSP vers){
	if (!vers) return;
	if (!vers->saved) return; /* vers not in stack */
	int i;
	/* reverse search to find faster in common cases 
	 * (like the user added the last vers to favorites by mistake)
	 */
	
	for (i=favorites->n; i>0; i--){ 
		SAVED_VERSP saved_vers = (SAVED_VERSP) libaroma_stack_get(favorites, i-1);
		if (saved_vers->versicle == vers->versicle){
			libaroma_stack_delete(favorites, i-1);
			saved_vers->saved=0;
			return;
		}
	}
}

/* save original canvas, highlight clone and set new */
void _vers_set_selected(LIBAROMA_CTL_LIST_ITEMP item){
	SAVED_VERSP vers=item->tag;
	vers->orig_cv = libaroma_canvas_dup(libaroma_listitem_image_get(item)); /* duplicate original canvas */
	LIBAROMA_CANVASP holded_cv=libaroma_canvas_dup(vers->orig_cv); /* highlight second clone canvas */
	libaroma_draw_rect(holded_cv, 0, 0, holded_cv->w, holded_cv->h, settings()->saved_bg, 0x80);
	libaroma_listitem_image_set(item, holded_cv); /* set highlighted canvas as visible */
	vers->selected=1;
	vers_selcount++;
	/*
	2nd method, maybe re-render text?
	*/
}

/* revert canvas and free holded_cv */
void _vers_set_unselected(LIBAROMA_CTL_LIST_ITEMP item){
	SAVED_VERSP vers=item->tag;
	if (vers->saved) return; /* don't unselect saved items */
	libaroma_listitem_image_set(item, vers->orig_cv);
	vers->orig_cv=NULL;
	vers->selected=0;
	vers_selcount--;
	/*
	2nd method, maybe re-render text?
	*/
}

byte vers_selmode_handler(LIBAROMA_CTL_LIST_ITEMP item, byte holding, char *title, char *subtitle){
	SAVED_VERSP vers = (SAVED_VERSP) item->tag;
	if (!vers_selmode && holding){ /* enable select mode to handle further clicks */
		vers_selmode=1;
		if (!vers->orig_cv){ /* switch to holded item canvas */
			_vers_set_selected(item);
		}
		LIBAROMA_CANVASP favs_icon=libaroma_image_uri("res:///png/favs.png");
		LIBAROMA_CANVASP cancel_icon=libaroma_image_uri("res:///png/cancel.png");
		libaroma_canvas_fillcolor(favs_icon, settings()->colorset->primary_text);
		libaroma_canvas_fillcolor(cancel_icon, settings()->colorset->primary_text);
		bar_tools=libaroma_ctl_bar_tools(2);
		libaroma_ctl_bar_tools_set(
			bar_tools, 0, ID_APPBAR_FIRST, "one", cancel_icon, LIBAROMA_CTL_BAR_TOOL_ICON_FREE
		);
		libaroma_ctl_bar_tools_set(
			bar_tools, 1, ID_APPBAR_SECOND, "two", favs_icon, LIBAROMA_CTL_BAR_TOOL_ICON_FREE
		);
		libaroma_ctl_bar_set_tools(appbar, bar_tools, 0);
		libaroma_ctl_bar_set_touchable_title(appbar, 0);
		libaroma_ctl_bar_update(appbar);
		libaroma_ctl_list_invalidate_item(readlist, item);
	}
	else if (vers_selmode && !holding){ /* handle item clicks while in select mode, ignore holds */
		if (!vers->selected){
			_vers_set_selected(item);
		}
		else {
			_vers_set_unselected(item);
		}
		libaroma_ctl_list_invalidate_item(readlist, item);
	}
	if (!vers_selcount && bar_tools){ /* revert bar to original state */
		libaroma_ctl_bar_tools_free(bar_tools);
		bar_tools=NULL;
		libaroma_ctl_bar_set_tools(appbar, NULL, 0);
		libaroma_ctl_bar_set_touchable_title(appbar, 1);
		main_appbar_update(title, subtitle, 1);
		vers_selmode=0;
	}
}

void _favs_onstart(){
	LIBAROMA_STACK_ITEMP itemp;
	for (itemp=favorites->last; itemp; itemp=libaroma_stack_prev(itemp)){
		SAVED_VERSP vers = (SAVED_VERSP) itemp->val;
		if (vers->saved){
			//add item here pls
		}
	}
}

void versicles_handle_selected(byte save, byte uncheck){
	LIBAROMA_CTL_LIST_ITEMP itm;
	/* free SAVED_VERSP structs */
	for (itm=libaroma_ctl_list_get_first_item(readlist); itm; itm=itm->next){
		SAVED_VERSP saved_vers = (SAVED_VERSP) itm->tag;
		if (save){
			if (saved_vers->selected)
				favs_add(saved_vers);
		}
		if (uncheck && !(saved_vers->saved)){
			_vers_set_unselected(saved_vers);
		}
	}
}

void _favs_onshow(){
	main_appbar_update(libaroma_lang_get("favs"), NULL, 1);
}

void _favs_pool(){
	LIBAROMA_MSG msg;
	dword command	=libaroma_window_pool(mainwin,&msg);
	byte cmd		=LIBAROMA_CMD(command);
	word id			=LIBAROMA_CMD_ID(command);
	byte param		=LIBAROMA_CMD_PARAM(command);

	/*if (msg.msg==LIBAROMA_MSG_KEY_SELECT || msg.msg==LIBAROMA_MSG_KEY_POWER){
		if (msg.state==0){
			br_page_switch(ID_FRAGMAIN, NULL, NULL, NULL);
		}
		return;
	}*/
	switch(cmd){
		case LIBAROMA_CMD_CLICK:{
			switch (id){
				case ID_APPBAR:{
					br_page_goback();
				} break;
				case ID_FAVORITES_LIST:{
					LIBAROMA_CTL_LIST_ITEMP touched = (LIBAROMA_CTL_LIST_ITEMP) msg.d;
					
				} break;
				case ID_SIDEBAR_LIST:{
					LIBAROMA_CTL_LIST_ITEMP item = libaroma_ctl_list_get_touched_item(sblist);
					if (item){
						libaroma_window_sidebar_show(mainsb, 0);
						switch (item->id){
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
							case ID_SIDEBAR_SETTINGS:{
								br_page_switch(ID_SETTINGS);
							} break;
						}
					}
				} break;
			}
		} break;
	}
}

byte favs_init(){
	if (!_favs_alloc()) return 0;
	fragfavs = libaroma_ctl_fragment_new_window(
		fragment, ID_FAVORITES
	);
	if (!fragfavs) {
		printf("favs_init: failed to create settings fragment window\n");
		return 0;
	}
	
	/* create reader list */
	favlist = libaroma_ctl_list(
		fragfavs, ID_FAVORITES_LIST,
		0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL,
		0, 0,
		settings()->colorset->control_bg,
		LIBAROMA_CTL_SCROLL_WITH_SHADOW|LIBAROMA_CTL_SCROLL_WITH_HANDLE
	);
	if (!favlist) {
		printf("favs_init: failed to create settings list\n");
		return 0;
	}
	if (!br_page_new(ID_FAVORITES, NULL, _favs_pool, NULL, _favs_onshow)){
		printf("favs_init: Failed to initialize page\n");
		return 0;
	}
	return 1;
}

#endif /*__br_favs_c__*/