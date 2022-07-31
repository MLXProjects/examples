#ifndef __test_layout_parser_c__ 
#define __test_layout_parser_c__ 
#ifdef __cplusplus
extern "C" {
#endif

#include "common.h" 

enum ITEM_KIND{
	OPTION,
	SWITCH,
	CHECK,
	MENU,
	DIVIDER,
	IMAGE,
	TEXT
};

int test_measure_xml(char *text, byte usey){
	if (!text) return 0;
	int ret=0;
	int textlen = strlen(text);
	if (text[textlen-1] == '%'){ //measure percentage size
		printf("MLX: measuring percentage size from %s\n", text);
		char percentage[16];
		snprintf(percentage, (textlen>16)?16:textlen, "%s", text);
		int perc = atoi(percentage);
		printf("MLX: percentage=%s, max=%d, textlen=%d\n", percentage, (usey?fragment->window->h:fragment->window->w), textlen);
		ret = ((usey?fragment->window->h:fragment->window->w)/100)*perc;
		ret = libaroma_px(ret);
	}
	else if (text[textlen-2] == 'd' && text[textlen-1] == 'p') { //use dp to measure
		printf("MLX: measuring dp\n");
		char *size = malloc(textlen-1);
		snprintf(size, textlen-2, "%s", text);
		ret = atoi(size);
		free(size);
	}
	else if (text[textlen-2] == 'p' && text[textlen-1] == 'x') { //use px to measure
		printf("MLX: measuring px\n");
		char *size = malloc(textlen-1);
		snprintf(size, textlen-2, "%s", text);
		ret = libaroma_px(atoi(size));
		free(size);
	}
	else ret=atoi(text);
	printf("MLX: measuring returns %d\n", ret);
	return ret;
}

int test_measure_pos(char *text, byte usey){
	if (!text) return 0;
	if (0==strcmp(text, "half_parent")) return LIBAROMA_POS_HALF;
	else if (0==strcmp(text, "left_quarter")) return LIBAROMA_POS_1P4;
	else if (0==strcmp(text, "right_quarter")) return LIBAROMA_POS_3P4;
	else if (0==strcmp(text, "left_third")) return LIBAROMA_POS_1P3;
	else if (0==strcmp(text, "right_third")) return LIBAROMA_POS_2P3;
	else return test_measure_xml(text, usey);
}

int test_measure_size(char *text, byte usey){
	if (!text) return 0;
	if (0==strcmp(text, "match_parent")) return LIBAROMA_SIZE_FULL;
	else if (0==strcmp(text, "half_parent")) return LIBAROMA_SIZE_HALF;
	else if (0==strcmp(text, "third_parent")) return LIBAROMA_SIZE_THIRD;
	else if (0==strcmp(text, "quarter_parent")) return LIBAROMA_SIZE_QUARTER;
	else return test_measure_xml(text, usey);
}

byte test_xml_layout_parse(LIBAROMA_WINDOWP win){
	printf("MLX: opening XML stream\n");
	LIBAROMA_STREAMP xml_stream = libaroma_stream_mzip(zip, "others/layout.xml");
	if (!xml_stream) return 0;
	printf("MLX: XML stream opened, parsing\n");
	LIBAROMA_XML xml_layout = libaroma_xml(xml_stream->data, xml_stream->size);
	if (!xml_layout) {
		printf("MLX: could not parse XML\n");
		libaroma_stream_close(xml_stream);
		return 0;
	}
	printf("MLX: XML parsed, width=%s, height=%s\n", libaroma_xml_attr(xml_layout, "width"), libaroma_xml_attr(xml_layout, "height"));
	LIBAROMA_XML control;
	for (control = xml_layout->child; control; control=(control->next)?control->next:control->sibling){
		printf("MLX: request to make control %s\n", control->name);
		char *x=libaroma_xml_attr(control, "x"), 
			*y=libaroma_xml_attr(control, "y"),
			*width=libaroma_xml_attr(control, "width"), 
			*height=libaroma_xml_attr(control, "height"), 
			*id=libaroma_xml_attr(control, "id"),
			*text=libaroma_xml_attr(control, "text"),
			*color=libaroma_xml_attr(control, "bgcolor");
		//printf("MLX: (unparsed) x=%s, y=%s, w=%s, h=%s, id=%s\n", x, y, width, height, id);
		
		int mx, my, mw, mh;
		mx=test_measure_pos(x, 0);
		my=test_measure_pos(y, 1);
		mw=test_measure_size(width, 0);
		mh=test_measure_size(height, 1);
		byte real_id=0;
		if (id!=NULL)
			real_id=(byte) atoi(id);
		
		word bgcolor = RGB(FFFFFF);
		if (color!=NULL) bgcolor = libaroma_rgb_from_string(color);
		
		//printf("MLX_XML: control x=%d, y=%d, width=%d, height=%d, id=%d\n", mx, my, mw, my, real_id);
		if (0==strcmp(control->name, "list")){
			byte flags=0;
			char *use_handle=libaroma_xml_attr(control, "use_handle");
			char *top_shadow=libaroma_xml_attr(control, "top_shadow");
			if (use_handle && (0==strcmp(use_handle, "true"))) flags |= LIBAROMA_CTL_LIST_WITH_HANDLE;
			if (top_shadow && (0==strcmp(top_shadow, "true"))) flags |= LIBAROMA_CTL_LIST_WITH_SHADOW;
			
			//create control first
			LIBAROMA_CONTROLP list = libaroma_ctl_list(win, real_id, mx, my, mw, mh, 0, 0, bgcolor, flags);
			if (!list) continue; //TODO: warn here
			
			//traverse list items
			LIBAROMA_XML item;
			for (item=libaroma_xml_child(control, "item"); item; item=libaroma_xml_next(item)){
				char *type = libaroma_xml_attr(item, "type");
				char *item_idstr = libaroma_xml_attr(item, "id");
				char *item_text = libaroma_xml_attr(item, "text");
				char *item_subtext = libaroma_xml_attr(item, "subtext");
				char *item_isleft = libaroma_xml_attr(item, "isleft");
				char *item_icopath = libaroma_xml_attr(item, "icon");
				LIBAROMA_CANVASP item_icon = NULL;
				if (item_icopath != NULL) item_icon = libaroma_image_uri(item_icopath);
				byte item_id=1;
				if (item_idstr != NULL) item_id = atoi(item_idstr);
				
				byte item_flags=0;
				enum ITEM_KIND kind;
				printf("MLX: request to make %s item\n", type);
				if (0==strcmp(type, "check")) kind = CHECK;
				else if (0==strcmp(type, "option")) kind = OPTION;
				else if (0==strcmp(type, "switch")) kind = SWITCH;
				else if (0==strcmp(type, "menu")) kind = MENU;
				printf("MLX: item kind defined\n");
				switch (kind){
					case CHECK:
					case OPTION:
					case SWITCH:
						printf("MLX: adjusting flags...\n");
						if (kind==OPTION) flags |= LIBAROMA_LISTITEM_CHECK_OPTION;
						else if (kind==SWITCH) flags |= LIBAROMA_LISTITEM_CHECK_SWITCH;
						if (item_isleft != NULL) 
							if (0==strcmp(item_isleft, "true")) flags |= LIBAROMA_LISTITEM_CHECK_LEFT_CONTROL;
						char *sel_str = libaroma_xml_attr(item, "selected");
						byte selected = 0;
						if (sel_str != NULL) 
							if (0==strcmp(sel_str, "true")) selected = 1;
						libaroma_listitem_check(list, item_id, selected, item_text, item_subtext, item_icon, flags, -1);
						break;
					case MENU:
						break;
				}
			}
		}
		else if (0==strcmp(control->name, "button")){
			libaroma_ctl_button(win, real_id, mx, my, mw, mh, text, LIBAROMA_CTL_BUTTON_RAISED, bgcolor);
		}
	}
	printf("MLX: parsing XML finished\n");
	
	return 1;
}

#ifdef __cplusplus
}
#endif
#endif /* __test_layout_parser_c__ */
