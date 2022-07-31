#ifndef __ptero_layout_c__
#define __ptero_layout_c__
#ifdef __cplusplus
extern "C" {
#endif
#include "common.h"

char *unparsed_layout;
LIBAROMA_STREAMP layout_stream;
LIBAROMA_XML layout_xml;

/*
 * Function		: ptero_variable_get
 * Parameters	: variable name
 * Return type	: char*
 * Description	: get variable from memory storage
 */
char *ptero_variable_get(char *name) {
	if (!name) return NULL;
	//printf("MLX: querying variable %s from storage\n", name);
	char *value=NULL;
	int namelen=strlen(name);
	char *temp_name = malloc(namelen);
	snprintf(temp_name, namelen, "%s", name);
	//printf("MLX: looking for %s in storage (len=%d)\n", temp_name, namelen);
	value = libaroma_sarray_get_string(pterodon()->var_stor, temp_name);
	free(temp_name);
	if (!value) printf("MLX: cannot find %s in variable storage\n", temp_name);
	return value;
}

/*
 * Function		: ptero_variable_set
 * Parameters	: variable name and value
 * Return type	: byte
 * Description	: add/update variable to memory storage
 */
byte ptero_variable_set(char *name, char *value) {
	if (!name || !value) return 0;
	if (!pterodon()->var_stor) return 0;
	return libaroma_sarray_set_string(pterodon()->var_stor, name, value);
}

/*
 * Function		: _ptero_parse_attr
 * Parameters	: tag, attr to look for
 * Return type	: char*
 * Description	: parse tag and apply variable calls
 */
char *_ptero_parse_attr(LIBAROMA_XML tag, char *attr) {
	char *found_attr = libaroma_xml_attr(tag, attr);
	
	/*
	char *var = NULL, *save=found_attr, *token;
	while ((token=strtok_r(save, "${", &save))) {
		char *offset = strtok(token, "}");
		if (offset) {
			var = malloc((offset-token)+1);
			snprintf(var, offset-token, "%s", token)
		}
	}*/
	if (found_attr && found_attr[0]=='$' && found_attr[1]=='(') {
		/*int attrlen=strlen(found_attr+2);
		char *temp_attr = malloc(attrlen);
		snprintf(temp_attr, attrlen-1, "%s", found_attr+2);*/
		if (!(found_attr=ptero_variable_get(found_attr+2))) {
			//free(temp_attr);
			return ""; /* return empty string to replace variable request */
		}
		//free(temp_attr);
	}

	return found_attr;
}

/*
 * Function		: _ptero_measure_xml
 * Parameters	: string, vertical flag
 * Return type	: int
 * Description	: parse string and get position/size from percentage, dp or px value (max is only used on percentage measurement)
 */
int _ptero_measure_xml(char *text, int max) {
	if (!text) return 0;
	int ret=0;
	int textlen = strlen(text);
	if (text[textlen-1] == '%') { /* measure percentage size */
		//printf("MLX: measuring percentage size from %s\n", text);
		char percentage[16];
		snprintf(percentage, (textlen>16)?16:textlen, "%s", text);
		int perc = atoi(percentage);
		//printf("MLX: percentage=%s, max=%d, textlen=%d\n", percentage, max, textlen);
		ret = (max/100)*perc;
		ret = libaroma_px(ret);
	}
	else if (text[textlen-2] == 'd' && text[textlen-1] == 'p') { /* use dp to measure */
		//printf("MLX: measuring dp\n");
		char *size = malloc(textlen-1);
		snprintf(size, textlen-1, "%s", text);
		ret = atoi(size);
		free(size);
	}
	else if (text[textlen-2] == 'p' && text[textlen-1] == 'x') { /* use px to measure */
		//printf("MLX: measuring px\n");
		char *size = malloc(textlen-1);
		snprintf(size, textlen-1, "%s", text);
		ret = libaroma_px(atoi(size));
		free(size);
	}
	else ret=atoi(text); /* return given string as int */
	//printf("MLX: measuring %s returns %d (max %d)\n", text, ret, max);
	return ret;
}

/*
 * Function		: _ptero_measure_ois
 * Parameters	: pos string, vertical flag
 * Return type	: int
 * Description	: parse position string to int
 */
int _ptero_measure_pos(char *text, int max) {
	if (!text) return 0;
	if (0==strcmp(text, "half_parent")) return LIBAROMA_POS_HALF;
	else if (0==strcmp(text, "left_quarter")) return LIBAROMA_POS_1P4;
	else if (0==strcmp(text, "right_quarter")) return LIBAROMA_POS_3P4;
	else if (0==strcmp(text, "left_third")) return LIBAROMA_POS_1P3;
	else if (0==strcmp(text, "right_third")) return LIBAROMA_POS_2P3;
	else return _ptero_measure_xml(text, max);
}

/*
 * Function		: _ptero_measure_size
 * Parameters	: size string, vertical flag
 * Return type	: int
 * Description	: parse size string to int
 */
int _ptero_measure_size(char *text, int max) {
	if (!text) return 0;
	if (0==strcmp(text, "match_parent") || 0==strcmp(text, "full")) return LIBAROMA_SIZE_FULL;
	else if (0==strcmp(text, "half_parent")) return (max>0)?max/2:LIBAROMA_SIZE_HALF;
	else if (0==strcmp(text, "third_parent")) return (max>0)?max/3:LIBAROMA_SIZE_THIRD;
	else if (0==strcmp(text, "quarter_parent")) return (max>0)?max/4:LIBAROMA_SIZE_QUARTER;
	else return _ptero_measure_xml(text, max);
}

/*
 * Function		: _ptero_img_parse
 * Parameters	: name, width, height
 * Return type	: LIBAROMA_CANVASP
 * Description	: Parse image name to return a canvas (either internal drawn or loaded from resources), optionally setting sizes
 */
LIBAROMA_CANVASP _ptero_img_parse(char *name, int width, int height) {
	if (!name) return NULL;
	LIBAROMA_CANVASP cv = NULL;
	if (name[0]=='#') { /* libaroma artworker icons - arrow, drawer, etc */
		//printf("MLX: %s %s\n", "requested aroma icon", name);
		int minsize = (width<height)?width:height;
		int dx = (width-minsize)/2,
			dy = (height-minsize)/2;
		if (0==strcmp(name+1, "arrow")) {
			cv = libaroma_canvas_ex(width, height, 1);
			libaroma_art_arrowdrawer(cv, 1, 0, dx, dy, minsize, RGB(FFFFFF), 0xFF, 0, 0.5);
		}
		else if (0==strcmp(name+1, "drawer")) {
			cv = libaroma_canvas_ex(width, height, 1);
			libaroma_art_arrowdrawer(cv, 1, 1, dx, dy, minsize, RGB(FFFFFF), 0xFF, 0, 0.5);
		}
		return cv;
	}
	else if (name[0]=='@') { /* images at res:///images */
		//printf("MLX: %s %s\n", "requested icon", name);
		int namelen = strlen(name+1);
		char *img_path = malloc(19+namelen);
		snprintf(img_path, 19+namelen, "res:///images/%s.png", name+1);
		cv = libaroma_image_uri(img_path);
		if (!cv) {
			//printf("MLX: failed to load %s, trying to find as svg\n", img_path);
			snprintf(img_path, 19+namelen, "res:///images/%s.svg", name+1);
			cv = libaroma_image_uri(img_path);
			if (!cv) printf("MLX: failed to load %s\n", img_path);
		}
		free(img_path);
	}
	else { /* uri */
		cv = libaroma_image_uri(name);
		if (cv) printf("MLX: loaded %s (%dx%d)\n", name, cv->w, cv->h);
		else printf("MLX: loading %s failed\n", name);
	}
	return cv;
}

static int _ptero_tag_parse_depth;
static int last_x, last_y, next_x, next_y;
/*
 * Function		: _ptero_layout_parse_tag
 * Parameters	: window, tag
 * Return type	: byte
 * Description	: Parse individual xml tag to create control or execute function accordingly
 */
byte _ptero_layout_parse_tag(LIBAROMA_WINDOWP win, LIBAROMA_XML tag, int x_off, int max_w){
	if (!win || !tag) {
		printf("MLX: parse tag failed\n");
		return 0;
	}
	dword tag_hash = libaroma_hash(tag->name);
	switch (tag_hash) {
		case PTERO_FUNC_ELSE: //ignored tags
		case PTERO_TAG_ACTIONSET:{
			return 1;
		}
		case PTERO_FUNC_EXIT:{
			return libaroma_msg_post(LIBAROMA_MSG_EXIT, 0, 0, 0, 0, NULL);
		}
		case PTERO_TAG_COLORSET:{
			return ptero_colorset_parse(tag);
		}
		case PTERO_FUNC_SET:{ /* save variable or update control */
			/* TODO: implement control management here 
			 * e.g.: 
			 * <set pager="home_pager" tab_manager="main_tabs" />
			 * <set list="home_list" item="usb_mode" subtext="Mass Storage" />
			 * <set appbar="main_appbar" title="Choose ZIP" icon="#drawer_to_arrow" bgcolor="#00aaff" />
			 * <set appbar="main_appbar" >
			 * 		<appbar_tool id="ab_switch" icon="#appbar_switch" onclick="ab_click" />
			 * </set>
			 * <set fragment="home_frag" window="home_wnd" pager="home_pager" tab_manager="main_tabs" />
			 */
			char *first_attr = tag->attr[0];
			if (!first_attr) {
				printf("MLX: SET: no parameters passed\n");
				return 0;
			}
			dword fa_hash = libaroma_hash(first_attr);
			if (fa_hash == PTERO_HASH_NAME) { /* usual set/clear variable procedure */
				char *var_name = libaroma_xml_attr(tag, "name"),
				*var_value = libaroma_xml_attr(tag, "value");
				if (!var_name) return 0;
				if ((!var_value) && ptero_variable_get(var_name)) {
					libaroma_sarray_delete(pterodon()->var_stor, var_name);
					return 1;
				}
				else return ptero_variable_set(var_name, var_value);
			}
			LIBAROMA_WINDOWP target_win;
			LIBAROMA_CONTROLP target_ctl;
			if (fa_hash == PTERO_TAG_WINDOW) { /* set target window and use second attr as first */
				char *win_id = libaroma_xml_attr(tag, first_attr);
				if (!win_id) {
					printf("MLX: SET: invalid window id \n");
					return 0;
				}
				LIBAROMA_WINDOWP winmain = pterodon()->winmain;
				LIBAROMA_CONTROLP try_fragment = NULL;
				int i, j;
				/* look for fragment controls which may contain the target window */
				for (i=0; i<winmain->childn; i++) {
					if (libaroma_ctl_is_fragment(winmain->childs[i])) {
						try_fragment = winmain->childs[i];
						PTERO_CONTROLP extra = (PTERO_CONTROLP) try_fragment->tag; //libaroma_ctl_fragment_get_window_count(try_fragment);
						if (!extra) {
							printf("MLX: SET: control extra is NULL\n");
							return 0;
						}
						for (j=0; j<extra->itemn; j++) { /* look for win_id in win_ids table */
							if (!strcmp(extra->win_ids[j], win_id)) {
								target_win = extra->wins[j]; /* when checking fragment extra, we MUST have j windows in wins member */
								break;
							}
						}
						if (target_win) break;
					}
				}
				if (!try_fragment) {
					printf("MLX: SET: no fragment control in main window\n");
					return 0;
				}
				if (!target_win) {
					printf("MLX: SET: cannot find window \"%s\"\n", win_id);
					return 0;
				}
				fa_hash = libaroma_hash(tag->attr[2]);
			}
			switch (fa_hash) {
				case PTERO_CTL_APPBAR:{
					
				} break;
				case PTERO_CTL_LIST:{
					
				} break;
				case PTERO_CTL_BUTTON:{
					
				} break;
				case PTERO_CTL_FRAGMENT:{
					
				} break;
				case PTERO_CTL_PAGER:{
					
				} break;
				case PTERO_CTL_IMAGE:{
					
				} break;
				default:{
					printf("MLX: unhandled set target %s\n", first_attr);
				} return 1;
			}
		} break;
		case PTERO_FUNC_SHOW:{ /* show window/sidebar(/dialog?) */ //TODO: decide whether to include dialogs here, or show them directly at creation
			char *show_type = libaroma_xml_attr(tag, "type");
			if (!show_type) return 1;
			if (libaroma_hash_compare(show_type, PTERO_TAG_SIDEBAR)) {
				if (pterodon()->sidebar) {
					libaroma_window_sidebar_show(pterodon()->sidebar, 1);
				}
				else {
					printf("MLX: no sidebar to show\n");
					return 1;
				}
				/*else if (0==strcmp(show_type, "window")){
					char *show_ctl_tag = libaroma_xml_attr(tag, "fragment_tag"),
						*show_id = libaroma_xml_attr(tag, "id");
					if (!show_ctl_tag || !show_id) {
						printf("MLX: didn't you forget something?\n"));
						return 0;
					}
					LIBAROMA_CONTROLP fragment = ptero_find_ctl_by_id(win, show_ctl_tag);
					if (!fragment) {
						printf("MLX: could not find fragment \"%s\"", show_ctl_tag);
					}
					byte id = (byte) atoi(show_id);
				}*/
			}
			return 1;
		} break;
		case PTERO_FUNC_HIDE:{
			char *hide_type = libaroma_xml_attr(tag, "type");
			if (!hide_type) return 1;
			if (libaroma_hash_compare(hide_type, PTERO_TAG_SIDEBAR)) {
				if (pterodon()->sidebar) {
					libaroma_window_sidebar_show(pterodon()->sidebar, 0);
					return 1;
				}
				else {
					printf("MLX: no sidebar to hide\n");
					return 1;
				}
			}
		} break;
		case PTERO_FUNC_IF:
		case PTERO_FUNC_ELIF:{
			if (!tag->child) {
				printf("MLX: empty IF statement (no child tags)\n");
				return 1;
			}
			char *if_var = _ptero_parse_attr(tag, "var");
			if (!if_var) {
				printf("MLX: IF without a variable to test? ...\n");
				return 1;
			}
			char *compare_type, *compare_to;
			compare_type = tag->attr[2]; /* 1st attr in array equals to "var", and 2nd one to if_var, so 3rd must be either NULL or the compare type */
			if (!compare_type) {
				printf("MLX: IF without compare type\n");
				return 1;
			}
			compare_to = _ptero_parse_attr(tag, compare_type);
			printf("MLX: IF going to check for %s %s %s\n", if_var, compare_type, compare_to);
			if (!compare_to) {
				printf("MLX: IF without variable to compare to=%s\n", libaroma_xml_attr(tag, compare_type));
				return 1;
			}
			//return 1;
			dword ct_hash = libaroma_hash(compare_type);
			byte parse_child = 0;
			switch (ct_hash){
				case PTERO_IF_EQUAL:{
					if (!strcmp(if_var, compare_to)) parse_child = 1;
				} break;
				case PTERO_IF_GRTR:{
					if (atoi(if_var) > atoi(compare_to)) parse_child = 1;
				} break;
				case PTERO_IF_SMLLR:{
					if (atoi(if_var) < atoi(compare_to)) parse_child = 1;
				} break;
				case PTERO_IF_GREQ:{
					if (atoi(if_var) >= atoi(compare_to)) parse_child = 1;
				} break;
				case PTERO_IF_SMEQ:{
					if (atoi(if_var) <= atoi(compare_to)) parse_child = 1;
				} break;
				case PTERO_IF_DIFF:{
					if (strcmp(if_var, compare_to)) parse_child = 1;
				} break;
				default:{
					printf("MLX: IF with invalid operand=%s\n", compare_type);
					return 1;
				} break;
			}
			printf("MLX: parse_child got %d\n", parse_child);
			if (parse_child) {
				return ptero_layout_parse(win, tag->child, x_off, max_w);
			}
			else {
				LIBAROMA_XML next_tag = tag->ordered;
				if (!next_tag) {
					printf("MLX: no next tag, we're done :D\n");
					return 1;
				}
				dword nt_hash = libaroma_hash(next_tag->name);
				switch (nt_hash) {
					case PTERO_FUNC_ELIF:{
						printf("MLX: parsing ELIF statement\n");
						return _ptero_layout_parse_tag(win, next_tag, x_off, max_w);
					} break;
					case PTERO_FUNC_ELSE:{
						printf("MLX: parsing ELSE statement\n");
						return ptero_layout_parse(win, next_tag->child, x_off, max_w);
					}
				}
			}
			return 1;
		} break;/*
		case PTERO_:{
			
		} break;*/
		case PTERO_TAG_SIDEBAR:{ // create sidebar into window
			if (pterodon()->sidebar) {
				printf("MLX: sidebar already exists\n");
				return 1;
			}
			pterodon()->sidebar = libaroma_window_sidebar(win, _ptero_measure_size(_ptero_parse_attr(tag, "width"), 0));
			if (pterodon()->sidebar && tag->child) {
				// check for child controls 
				if (!ptero_layout_parse(pterodon()->sidebar, tag->child, 0, 0)) {
					printf("MLX: failed to parse sidebar childs\n");
					return 0;
				}
			}
			return pterodon()->sidebar?1:0;
		} break;
		default: { /* let's hope it is a control */
			LIBAROMA_CONTROLP ctl=NULL;
			char *x=_ptero_parse_attr(tag, "x"), 
			*y=_ptero_parse_attr(tag, "y"),
			*width=_ptero_parse_attr(tag, "width"), 
			*height=_ptero_parse_attr(tag, "height"), 
			*ctl_id=_ptero_parse_attr(tag, "id"),
			*text=_ptero_parse_attr(tag, "text"),
			*color=_ptero_parse_attr(tag, "bgcolor");
			int mx, my, mw, mh;
			/* check if x/y declared in tag */ //TODO: fix this plis
			/*if (!x) mx=next_x;
			else */mx=x_off + _ptero_measure_pos(x, win->width);
			/*if (!y) my=next_y;
			else */my=_ptero_measure_pos(y, win->height);
			mw=_ptero_measure_size(width, (max_w>0)?max_w:win->width);
			mh=_ptero_measure_size(height, win->height);
			if (mw > max_w || mw == 0) mw = max_w; /* override measured width to max width */
			/* if measured pos + size are greater than last value, update last */
			if (mx+mw > next_x) next_x=mx+mw;
			if (my+mh > next_y) next_y=my+mh;
			printf("\nMLX: ********%s w=%d********\n", tag->name, mw);
			
			word bgcolor = libaroma_colorget(NULL, win)->control_bg;
			if (color!=NULL) bgcolor = libaroma_rgb_from_string(color);
			switch (tag_hash){
				case PTERO_CTL_APPBAR:{
					/* please count childs before creating control - same goes for pager */
					char *bar_icopath = _ptero_parse_attr(tag, "icon"),
					*bar_bgcolor = _ptero_parse_attr(tag, "bgcolor"),
					*bar_fgcolor = _ptero_parse_attr(tag, "fgcolor"),
					*bar_title = _ptero_parse_attr(tag, "title"),
					*bar_subtitle = _ptero_parse_attr(tag, "subtitle"),
					*bar_touchable = _ptero_parse_attr(tag, "touchable_title");
					LIBAROMA_CANVASP bar_icon = NULL;
					word bgcolor = bar_bgcolor?libaroma_rgb_from_string(bar_bgcolor):libaroma_colorget(NULL,win)->control_bg,
					fgcolor = bar_fgcolor?libaroma_rgb_from_string(bar_fgcolor):RGB(FFFFFF);
					ctl = libaroma_ctl_bar(win, 1, mx, my, mw, mh, bar_title, bgcolor, fgcolor);
					if (bar_icopath) {
						if (bar_icopath[0]=='#') {
							if (0==strcmp(bar_icopath+1, "drawer"))
								libaroma_ctl_bar_set_icon(ctl, NULL, 0, LIBAROMA_CTL_BAR_ICON_DRAWER, 0);
							else if (0==strcmp(bar_icopath+1, "arrow"))
								libaroma_ctl_bar_set_icon(ctl, NULL, 0, LIBAROMA_CTL_BAR_ICON_ARROW, 0);
						}
						else {
							bar_icon = libaroma_image_uri(bar_icopath);
							libaroma_ctl_bar_set_icon(ctl, bar_icon, 1, 0, 0);
						}
					}
					if (bar_subtitle) libaroma_ctl_bar_set_subtitle(ctl, bar_subtitle, 0);
					if (bar_touchable && libaroma_hash_compare(bar_touchable, PTERO_HASH_TRUE)) libaroma_ctl_bar_set_touchable_title(ctl, 1);
				} break;
				case PTERO_CTL_BUTTON:{
					ctl = libaroma_ctl_button(win, 1, mx, my, mw, mh, text, LIBAROMA_CTL_BUTTON_RAISED, bgcolor);
				} break;
				case PTERO_CTL_LIST:{
					byte flags=LIBAROMA_CTL_LIST_WITH_SHADOW;
					byte list_small_childs=0;
					char *use_handle=_ptero_parse_attr(tag, "use_handle"),
						*top_shadow=_ptero_parse_attr(tag, "top_shadow"),
						*small_items=_ptero_parse_attr(tag, "small_items"),
						*draw_separator=_ptero_parse_attr(tag, "separator");
					word fill_icons=pterodon()->global_fill_icons;
					if (use_handle && libaroma_hash_compare(use_handle, PTERO_HASH_TRUE)) flags |= LIBAROMA_CTL_LIST_WITH_HANDLE;
					if (top_shadow && libaroma_hash_compare(top_shadow, PTERO_HASH_FALSE)) flags &= ~LIBAROMA_CTL_LIST_WITH_SHADOW;
					if (small_items && libaroma_hash_compare(small_items, PTERO_HASH_TRUE)) list_small_childs=1;
					
					ctl = libaroma_ctl_list(win, 1, mx, my, mw, mh, 0, 0, bgcolor, flags);
					if (!ctl) return 0; //TODO: warn here
					if (tag->child) {
						LIBAROMA_XML item;
						for (item=tag->child; item; item=item->ordered) {
							char *item_type = item->name,
								//*item_tag = _ptero_parse_attr(item, "tag"),
								*item_text = _ptero_parse_attr(item, "text"),
								*item_subtext = _ptero_parse_attr(item, "subtext"),
								*item_isleft = _ptero_parse_attr(item, "isleft"),
								*item_imgpath = _ptero_parse_attr(item, "img"),
								*item_width = _ptero_parse_attr(item, "width"),
								*item_height = _ptero_parse_attr(item, "height"),
								*item_small = _ptero_parse_attr(item, "small"),
								*item_recvtouch = _ptero_parse_attr(item, "force_ripple"),
								*item_onclick = _ptero_parse_attr(item, "onclick"),
								*item_onhold = _ptero_parse_attr(item, "onhold"),
								*item_noiconfill = _ptero_parse_attr(item, "icon_nofill");
							if (!item_imgpath) item_imgpath = _ptero_parse_attr(item, "icon");
							byte small=0;
							LIBAROMA_CANVASP item_img = NULL;
							int img_w, img_h;
							if (list_small_childs || (item_small && libaroma_hash_compare(item_small, PTERO_HASH_TRUE))) small=1;
							if (item_imgpath) {
								img_w=_ptero_measure_xml(item_width, ctl->width); /* max param only used in percentage measurements */
								img_h=_ptero_measure_xml(item_height, ctl->height);
								item_img = _ptero_img_parse(item_imgpath, item_width?img_w:libaroma_dp(small?24:40), item_height?img_h:libaroma_dp(small?24:40));
								if ((fill_icons) && 
								(!item_noiconfill || 
									(item_noiconfill && 
									!libaroma_hash_compare(item_noiconfill, PTERO_HASH_TRUE))
								) &&
								strcmp(item_type, "image")) {
									libaroma_canvas_fillcolor(item_img, fill_icons);
								}
							}
							
							word item_flags=LIBAROMA_LISTITEM_WITH_SEPARATOR;
							
							if (draw_separator) {
								dword ds_hash = libaroma_hash(draw_separator);
								if (ds_hash == PTERO_HASH_FALSE)
									item_flags=0;
								else if (ds_hash == libaroma_hash("textalign"))
									item_flags |= LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN;
							}
							char *indent_noicon = _ptero_parse_attr(item, "indent");
							
							LIBAROMA_CTL_LIST_ITEMP actual_item;
							//printf("MLX: \t****%s****\n", item_type);
							if (0) {
								printf("MLX: \t\ttype=%s\n"
											"\t\ttext=%s\n"
											"\t\tsubtext=%s\n"
											"\t\tisleft=%s\n"
											"\t\timg=%s\n"
											"\t\twidth=%s\n"
											"\t\theight=%s\n"
											"\t\tsmall=%s\n",
											item_type, item_text, item_subtext, item_isleft, item_imgpath, item_width, item_height, item_small
								);
								printf("MLX: icon load %s\n", item_img?"succeeded":"failed");
							}
							dword type_hash = libaroma_hash(item_type);
							switch (type_hash) {
								case PTERO_LISTITEM_IMAGE:{
									char *item_parallax = _ptero_parse_attr(item, "parallax"),
									*item_fill = _ptero_parse_attr(item, "fill"),
									*item_proport = _ptero_parse_attr(item, "proportional"),
									*item_fillcolor = _ptero_parse_attr(item, "fillcolor");
									if (item_fillcolor) {
										word fillcolor = libaroma_rgb_from_string(item_fillcolor);
										libaroma_canvas_fillcolor(item_img, fillcolor);
									}
									if (item_parallax && libaroma_hash_compare(item_parallax, PTERO_HASH_TRUE)) item_flags |= LIBAROMA_LISTITEM_IMAGE_PARALAX;
									if (item_fill && libaroma_hash_compare(item_fill, PTERO_HASH_TRUE)) item_flags |= LIBAROMA_LISTITEM_IMAGE_FILL;
									if (item_proport && libaroma_hash_compare(item_proport, PTERO_HASH_TRUE)) item_flags |= LIBAROMA_LISTITEM_IMAGE_PROPORTIONAL;
									if (item_recvtouch && libaroma_hash_compare(item_recvtouch, PTERO_HASH_TRUE)) item_flags |= LIBAROMA_CTL_LIST_ITEM_RECEIVE_TOUCH;
									actual_item = libaroma_listitem_image(ctl, 1, item_img, item_height?img_h:libaroma_px(item_img->h), item_flags, -1);
								} break;
								case PTERO_LISTITEM_MENU:{
									if (indent_noicon && (!strcmp(indent_noicon, "noicon"))) item_flags |= LIBAROMA_LISTITEM_MENU_INDENT_NOICON;
									if (small) item_flags |= LIBAROMA_LISTITEM_MENU_SMALL;
									actual_item = libaroma_listitem_menu(ctl, 1, item_text, item_subtext, item_img, item_flags, -1);
								} break;
								case PTERO_LISTITEM_CAPTION:{
									char *item_color = _ptero_parse_attr(item, "color");
									actual_item = libaroma_listitem_caption_color(ctl, 1, item_text, item_color?libaroma_rgb_from_string(item_color):libaroma_colorget(ctl, NULL)->accent, -1);
								} break;
								case PTERO_LISTITEM_DIVIDER:{
									byte div_flag = 0;
									char *div_subscr = _ptero_parse_attr(item, "subscreen");
									if (div_subscr && libaroma_hash_compare(div_subscr, PTERO_HASH_TRUE)) div_flag = 1;
									actual_item = libaroma_listitem_divider(ctl, 1, div_flag?LIBAROMA_LISTITEM_DIVIDER_SUBSCREEN:0, -1);
								} break;
								case PTERO_LISTITEM_CHECK:
								case PTERO_LISTITEM_SWITCH:
								case PTERO_LISTITEM_OPTION:{
									//printf("MLX: adjusting check flags...\n");
									if (indent_noicon && (!strcmp(indent_noicon, "noicon"))) item_flags |= LIBAROMA_LISTITEM_CHECK_INDENT_NOICON;
									if (small) item_flags |= LIBAROMA_LISTITEM_CHECK_SMALL_ICON;
									if (type_hash==PTERO_LISTITEM_SWITCH) item_flags |= LIBAROMA_LISTITEM_CHECK_SWITCH;
									else if (type_hash==PTERO_LISTITEM_OPTION) item_flags |= LIBAROMA_LISTITEM_CHECK_OPTION;
									if (item_isleft && libaroma_hash_compare(item_isleft, PTERO_HASH_TRUE)) item_flags  |= LIBAROMA_LISTITEM_CHECK_LEFT_CONTROL;
									char *sel_str = _ptero_parse_attr(item, "selected");
									byte selected = 0;
									if (sel_str && libaroma_hash_compare(sel_str, PTERO_HASH_TRUE))
										selected = 1;
									//printf("MLX: %s item is %s\n", item_type, selected?"checked":"unchecked");
									actual_item = libaroma_listitem_check(ctl, 1, selected, item_text, item_subtext, item_img, item_flags, -1);
								} break;
								default:{
									printf("MLX: unhandled listitem type=%s\n", item_type);
								} break;
							}
							if (!actual_item) {
								printf("MLX: failed to create %s item\n", item->name);
								return 0;
							}
							if (item_onclick || item_onhold) {
								printf("MLX: preparing extra item data\n");
								PTERO_CONTROLP extra = calloc(1, sizeof(PTERO_CONTROL));
								if (!extra) return 0;
								LIBAROMA_XML actionset;
								printf("MLX: checking for click/hold callbacks\n");
								for (actionset = libaroma_xml_child(layout_xml, "actionset"); actionset; actionset = actionset->next){
									char *actionset_name = libaroma_xml_attr(actionset, "name");
									printf("MLX: found actionset %s, looking for %s || %s\n", actionset_name, item_onclick, item_onhold);
									if (!actionset_name) {
										printf("MLX: missing actionset name\n");
										continue;
									}
									if (item_onclick && libaroma_hash_compare(actionset_name, libaroma_hash(item_onclick))) {
										extra->onclick = actionset;
										libaroma_control_set_onclick(ctl, ptero_ctl_click);
										printf("MLX: registering click callback\n");
										break;
									}
									if (item_onhold && libaroma_hash_compare(actionset_name, libaroma_hash(item_onhold))) {
										extra->onhold = actionset;
										libaroma_control_set_onhold(ctl, ptero_ctl_hold);
										printf("MLX: registering hold callback\n");
										break;
									}
								}
								actual_item->tag = extra;
							}
						}
					}
				} break;
				case PTERO_CTL_IMAGE:{
					char *img_src = _ptero_parse_attr(tag, "img");
					LIBAROMA_CANVASP src = NULL;
					if (img_src) src = _ptero_img_parse(img_src, mw, mh);
					if (!src) {
						printf("MLX: failed to load %s\n", img_src);
						return 0;
					}
					char *img_center = _ptero_parse_attr(tag, "center"),
					*img_fill = _ptero_parse_attr(tag, "fill"),
					*img_fillcolor = _ptero_parse_attr(tag, "fillcolor");
					if (img_fillcolor) {
						word fgcolor = libaroma_rgb_from_string(img_fillcolor);
						libaroma_canvas_fillcolor(src, fgcolor);
					}
					byte img_flags = 0;
					if (img_center && libaroma_hash_compare(img_center, PTERO_HASH_TRUE)) img_flags |= LIBAROMA_CTL_IMAGE_CENTER;
					if (img_fill && libaroma_hash_compare(img_fill, PTERO_HASH_TRUE)) img_flags |= LIBAROMA_CTL_IMAGE_FILL;
					ctl = libaroma_ctl_image_canvas_ex(win, 1, src, mx, my, mw, mh, img_flags);
				} break;
				case PTERO_CTL_FRAGMENT:{
					ctl = libaroma_ctl_fragment(win, 1, mx, my, mw, mh);
					if (!ctl) return 0;
					if (tag->child) { /* fragment has childs, parse them too */
						LIBAROMA_XML child_win;
						/* handle each child window */
						for (child_win=tag->child; child_win; child_win=child_win->ordered) {
							char *child_id = _ptero_parse_attr(child_win, "id"),
							*child_active = _ptero_parse_attr(child_win, "active");
							byte real_chid = (byte) atoi(child_id);
							LIBAROMA_WINDOWP fragwin = libaroma_ctl_fragment_new_window(ctl, real_chid);
							/* activate window before parsing - needed for pager */
							if (child_active && libaroma_hash_compare(child_active, PTERO_HASH_TRUE)) { /* set active window if declared */
								//printf("MLX: fragment window %s active\n", child_id);
								libaroma_ctl_fragment_set_active_window(ctl, real_chid, 0, 0, 0, NULL, NULL, NULL);
							}
							/* parse all window childs */
							if (!ptero_layout_parse(fragwin, child_win->child, 0, mw)) {
								//printf("MLX: fragment child window parsing failed\n");
								return 0;
							}
						}
					}
				} break;
				case PTERO_CTL_PAGER:{
					int page_count=0;
					LIBAROMA_XML child_page = libaroma_xml_child(tag, "page");
					if (!child_page) {
						printf("MLX: no childs and pager doesn't support later child adding\n");
						return 0;
					}
					/* process page count before creating */
					char **child_titles = NULL;
					for (; child_page; child_page = child_page->next) {
						char **tmp_titles;
						page_count++;
						tmp_titles = realloc(child_titles, sizeof(char*) * page_count);
						if (!tmp_titles) {
							printf("MLX: cannot reallocate page titles!!\n");
							break;
						}
						child_titles = tmp_titles;
						child_titles[page_count-1] = libaroma_xml_attr(child_page, "title");
					}
					if (!page_count) { /* could this ever happen? */
						printf("MLX: pager control with no childs\n");
						return 0;
					}
					/* create pager control */
					ctl = libaroma_ctl_pager(win, 1, page_count, mx, my, mw, mh);
					if (!ctl) {
						printf("MLX: create pager failed\n");
						if (child_titles) free(child_titles);
						return 0;
					}
					/* look for tab control */
					LIBAROMA_XML tabmgr_name = libaroma_xml_attr(tag, "tab_manager");
					if (tabmgr_name) {
						LIBAROMA_CONTROLP tab_ctl = ptero_find_ctl_by_id(win, tabmgr_name);
						if (tab_ctl) {
							libaroma_ctl_tabs_set_pager(tab_ctl, ctl);
							libaroma_ctl_tabs_set_texts(tab_ctl, child_titles, page_count, 1);
						}
						else printf("MLX: tab control \"%s\" not found\n", tabmgr_name);
					}
					LIBAROMA_WINDOWP pager_win = libaroma_ctl_pager_get_window(ctl);
					if (!pager_win) {
						printf("MLX: pager cannot obtain window\n");
						return 0;
					}
					/* parse child tags again */
					page_count = 0;
					for (child_page = libaroma_xml_child(tag, "page"); child_page; child_page = child_page->next) {
						printf("MLX: parsing page %d with max width=%d\n", page_count, ctl->width);
						if (!ptero_layout_parse(pager_win, child_page->child, ctl->width * page_count, ctl->width)) {
							printf("MLX: pager failed to parse child page\n");
							return 0;
						}
						char *page_active = libaroma_xml_attr(child_page, "active");
						if (page_active && libaroma_hash_compare(page_active, PTERO_HASH_TRUE)) libaroma_ctl_pager_set_active_page(ctl, page_count, 0.0);
						page_count++;
					}
					
					if (child_titles) free(child_titles);
					return 1;
				} break;
				case PTERO_CTL_TABMGR:{
					char *fgcolor = libaroma_xml_attr(tag, "fgcolor"),
						*lpad = libaroma_xml_attr(tag, "left_pad"),
						*hpad = libaroma_xml_attr(tag, "horizontal_pad"),
						*target_pager = libaroma_xml_attr(tag, "pager");
					ctl = libaroma_ctl_tabs(win, 1, mx, my, mw, mh, color, fgcolor?fgcolor:libaroma_colorget(NULL, win)->primary_text, lpad?atoi(lpad):0, hpad?atoi(hpad):0);
					if (!ctl) {
						printf("MLX: failed to create tab manager\n");
						return 0;
					}
					if (target_pager) { /* set target pager control */
						LIBAROMA_CONTROLP pager_ctl = ptero_find_ctl_by_id(win, target_pager);
						if (!pager_ctl) {
							printf("MLX: tab_manager failed to find pager=%s\n", target_pager);
							return 0;
						}
						libaroma_ctl_tabs_set_pager(ctl, pager_ctl);
						if (!tag->parent) {
							printf("MLX: tab_manager as root? what's wrong with you?\n");
							return 0;
						}
						/* look for target pager child items */
						LIBAROMA_XML pager_tag;
						for (pager_tag = libaroma_xml_child(tag->parent, "pager"); pager_tag; pager_tag = pager_tag->next) {
							char *try_pager_id = libaroma_xml_attr(pager_tag, target_pager);
							/* if target pager and try pager ids are same */
							if (try_pager_id && (!strcmp(try_pager_id, target_pager))) {
								LIBAROMA_XML child_page = libaroma_xml_child(pager_tag, "page");
								if (!child_page) {
									printf("MLX: no childs and pager doesn't support later child adding\n");
									return 0;
								}
								int page_count = 0;
								char **child_titles;
								for (; child_page; child_page = child_page->next) {
									char **tmp_titles;
									page_count++;
									tmp_titles = realloc(child_titles, sizeof(char*)*page_count);
									if (!tmp_titles) {
										printf("MLX: cannot reallocate page titles!!\n");
										break;
									}
									child_titles = tmp_titles;
									child_titles[page_count-1] = libaroma_xml_attr(child_page, "title");
								}
								libaroma_ctl_tabs_set_texts(ctl, child_titles, page_count, 1);
								break; /* stop looking for target pager */
							}
						}
						if (!pager_tag) {
							printf("MLX: tab_manager pager \"%s\" not found\n", target_pager);
						}
					}
				} break;
				default:{
					printf("MLX: unhandled tag=%s\n", tag->name);
				} return 1;
			}
			if (ctl) {
				//printf("MLX: preparing extra data\n");
				PTERO_CONTROLP extra = calloc(1, sizeof(PTERO_CONTROL));
				if (!extra) {
					printf("MLX: failed to allocate extra info\n");
					return 0;
				}
				extra->id = ctl_id;
				char *ctl_onclick = libaroma_xml_attr(tag, "onclick"),
				*ctl_onhold = libaroma_xml_attr(tag, "onhold");
				LIBAROMA_XML actionset;
				if (ctl_onclick || ctl_onhold) {
					printf("MLX: checking for click/hold callbacks\n");
					for (actionset = libaroma_xml_child(layout_xml, "actionset"); actionset; actionset = actionset->next){
						char *actionset_name = libaroma_xml_attr(actionset, "name");
						printf("MLX: found actionset %s, looking for %s || %s\n", actionset_name, ctl_onclick, ctl_onhold);
						if (!actionset_name) {
							printf("MLX: missing actionset name\n");
							continue;
						}
						if (ctl_onclick && libaroma_hash_compare(actionset_name, libaroma_hash(ctl_onclick))) {
							libaroma_control_set_onclick(ctl, &ptero_ctl_click);
							extra->onclick = actionset;
							printf("MLX: registering click callback\n");
							if (!ctl_onhold) break;
						}
						if (ctl_onhold && libaroma_hash_compare(actionset_name, libaroma_hash(ctl_onhold))) {
							libaroma_control_set_onhold(ctl, &ptero_ctl_hold);
							extra->onhold = actionset;
							printf("MLX: registering hold callback\n");
						}

					}
				}
				ctl->tag = extra;
				return 1;
			}
			else return 0;
		} break;
	}
	return 1;
}

byte ptero_parse_actionset(LIBAROMA_XML actionset){
	if (!actionset) return 0;
	LIBAROMA_XML function;
	byte last_ret = 0;
	printf("MLX: parsing actionset %s\n", libaroma_xml_attr(actionset, "name"));
	for (function = actionset->child; function; function=function->ordered){
		printf("MLX: found function %s\n", function->name);
		return _ptero_layout_parse_tag(pterodon()->winmain, function, 0, 0);
	}
	return 1;
}

/*
 * Function		: ptero_layout_parse
 * Parameters	: XML instance, window
 * Return type	: byte
 * Description	: Parse an xml instance to process all child items for a window
 */
byte ptero_layout_parse(LIBAROMA_WINDOWP win, LIBAROMA_XML parsed_layout, int x_off, int max_w) {
	if (!win || !parsed_layout) return 0;
	//printf("MLX: XML parsed, width=%s, height=%s\n", _ptero_parse_attr(parsed_layout, "width"), _ptero_parse_attr(parsed_layout, "height"));
	LIBAROMA_XML control_tag;
	for (control_tag = parsed_layout; control_tag; control_tag = control_tag->ordered) {
		//printf("MLX: next tag is %s\n", (control_tag->ordered)?control_tag->ordered->name:"NULL");
		if (!_ptero_layout_parse_tag(win, control_tag, x_off, max_w)) return 0;
	}
	//_ptero_tag_parse_depth=0;
	//next_x=next_y=0; /* reset next_x/y variables */
	//printf("MLX: parsing XML finished\n");
	
	return 1;
}

/*
 * Function		: ptero_layout_init
 * Parameters	: none
 * Return type	: byte
 * Description	: initialize main window & xml instance
 */
byte ptero_layout_init() {
	LIBAROMA_WINDOWP win = libaroma_window(NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
	if (!win) return 0;
	pterodon()->winmain = win;
	pterodon()->var_stor = libaroma_sarray(NULL);
	//printf("MLX: opening XML stream\n");
	layout_stream = libaroma_stream_mzip(global_zip, "layout.xml");
	//layout_stream = libaroma_stream_file_ex("/home/mlx/projects/libaroma/apps/pterodon/zip/layout.xml", 1);
	if (!layout_stream) return 0;
	printf("MLX: XML stream opened, parsing\n");
	layout_xml = libaroma_xml(layout_stream->data, layout_stream->size);
	if (!layout_xml) {
		printf("MLX: could not parse XML\n");
		libaroma_stream_close(layout_stream);
		return 0;
	}
	return 1;//ptero_layout_parse(win, layout_xml);
}

#ifdef __cplusplus
}
#endif
#endif /* __ptero_layout_c__ */
