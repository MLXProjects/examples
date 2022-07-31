#ifndef __layout_parser_c__
#define __layout_parser_c__
#include "common.h"

#define _layout_parse_istrue(var) (var!=NULL && (strcmp(var, "true")==0))

int _layout_parse_curdepth;
extern LAYOUT_WINLIST _layout_wins;
LIBAROMA_WINDOWP to_show=NULL;
char *to_showid;

int layout_parse_measure(char *string, int ifnull, byte size, byte y){
	if (string==NULL) return ifnull;
	int ret=atoi(string);
	int len=strlen(string);
	if (ret==0 && strlen>1){
		/* word used as measurement */
		if (strcmp(string, "half")==0) return size?LIBAROMA_SIZE_HALF:LIBAROMA_POS_HALF;
		if (strcmp(string, "max")==0) return size?LIBAROMA_SIZE_FULL:0;
		if (strcmp(string, "third")==0) return size?LIBAROMA_SIZE_THIRD:LIBAROMA_POS_1P3;
		if (strcmp(string, "quarter")==0) return size?LIBAROMA_SIZE_QUARTER:LIBAROMA_POS_1P4;
		if (strcmp(string, "twothird")==0) return size?LIBAROMA_SIZE_THIRD:LIBAROMA_POS_2P3;
		if (strcmp(string, "threequarter")==0) return size?LIBAROMA_SIZE_QUARTER:LIBAROMA_POS_3P4;
	}
	if (len>1){
		if (len>2){
			// e.g. width="50%"
			if (string[len-1]=='%'){
			ret=libaroma_px((int)(((y?libaroma_fb()->h:libaroma_fb()->w)/100.0)*ret));
			}
			// e.g. width="48px"
			else if (string[len-2]=='p' && string[len-1]=='x'){
				return libaroma_px(ret);
			}
			// e.g. width="48dp"
			else if(string[len-2]=='d' && string[len-1]=='p'){
				return libaroma_dp(ret);
			}
		}
		// e.g. width="5%"
		else if (string[1]=='%'){
			ret=libaroma_px((int)(((y?libaroma_fb()->h:libaroma_fb()->w)/100.0)*ret));
		}
	}
	return ret;
}

byte _layout_parse_tag(LIBAROMA_XML tag, void *data){
	if (!tag){
		return 0;
	}
	LOGI("tag=%s (%x) - depth %d", tag->name, libaroma_hash(tag->name), _layout_parse_curdepth);
	char *id=libaroma_xml_attr(tag, "id");
	dword type=libaroma_hash(tag->name);
	if (type==LAYOUT_TAG_WIN){
		if (_layout_wins.winn>63){
			LOGI("cannot allocate more than 64 windows");
			return 0;
		}
		LIBAROMA_WINDOWP win = libaroma_window(NULL, 0, 0, 0, 0);
		if (!win){
			LOGI("cannot allocate window \"%s\"", id);
			return 0;
		}
		char *mustshow = libaroma_xml_attr(tag, "show");
		LOGI("window must be shown? %s", mustshow);
		if (_layout_parse_istrue(mustshow)){
			LOGI("confirmo pana");
			to_show=win;
			to_showid=id;
		}
		data=(void*)win;
		_layout_wins.wins[_layout_wins.winn]=win;
		_layout_wins.ids[_layout_wins.winn]=id;
		_layout_wins.winn++;
	}
	else if (type==LAYOUT_TAG_FONT){
		if (id==NULL){
			LOGI("null id");
			return 0;
		}
		char *font_uri = libaroma_xml_attr(tag, "file");
		if (!font_uri){
			LOGI("invalid font path");
			return 0;
		}
		int req_id=atoi(id);
		if (req_id>255){
			LOGI("id > 255, lowering");
			req_id=255;
		}
		if (!libaroma_font((byte) req_id, libaroma_stream(font_uri))){
			LOGI("failed to load font");
			return 0;
		}
	}
	else if (type==LAYOUT_TAG_ASET){
		
	}
	else {
		if (data==NULL){
			LOGI("cannot create control without parent window");
			return 1;
		}
		int x, y, w, h;
		x=layout_parse_measure(libaroma_xml_attr(tag, "x"), 0, 0, 0);
		y=layout_parse_measure(libaroma_xml_attr(tag, "y"), 0, 0, 1);
		w=layout_parse_measure(libaroma_xml_attr(tag, "width"), 0, 1, 0);
		h=layout_parse_measure(libaroma_xml_attr(tag, "height"), 0, 1, 1);
		LIBAROMA_WINDOWP win = (LIBAROMA_WINDOWP) data;
		LIBAROMA_CONTROLP ctl;
		switch (type){
			case LAYOUT_TAG_BTN:{
				char *text = libaroma_xml_attr(tag, "text");
				char *raised = libaroma_xml_attr(tag, "raised");
				char *color = libaroma_xml_attr(tag, "color");
				char *disabled = libaroma_xml_attr(tag, "disabled");
				char *circle = libaroma_xml_attr(tag, "circle");
				word btncolor;
				byte style = 0;
				if (_layout_parse_istrue(raised)) style |= LIBAROMA_CTL_BUTTON_RAISED;
				if (_layout_parse_istrue(disabled)) style |= LIBAROMA_CTL_BUTTON_DISABLED;
				if (_layout_parse_istrue(circle)) style |= LIBAROMA_CTL_BUTTON_CIRCLE;
				if (color!=NULL){
					style |= LIBAROMA_CTL_BUTTON_COLORED;
					btncolor = libaroma_rgb_from_string(color);
				}
				ctl=libaroma_ctl_button(win, 0, x, y, w, h, text, style, btncolor);
				if (!ctl){
					LOGI("cannot create %s control", tag->name);
					return 0;
				}
			} break;
			case LAYOUT_TAG_PAGR:{
				//ctl=libaroma_ctl_(win, 0, x, y, w, h, );
				if (!ctl){
					LOGI("cannot create %s control", tag->name);
					return 1;
				}
			} break;
			case LAYOUT_TAG_LIST:{
				//ctl=libaroma_ctl_(win, 0, x, y, w, h, );
				if (!ctl){
					LOGI("cannot create %s control", tag->name);
					return 1;
				}
			} break;
			case LAYOUT_TAG_ITEM:{
				//ctl=libaroma_ctl_(win, 0, x, y, w, h, );
				if (!ctl){
					LOGI("cannot create %s control", tag->name);
					return 1;
				}
			} break;
			case LAYOUT_TAG_PROG:{
				//ctl=libaroma_ctl_(win, 0, x, y, w, h, );
				if (!ctl){
					LOGI("cannot create %s control", tag->name);
					return 1;
				}
			} break;
			case LAYOUT_TAG_LABL:{
				//ctl=libaroma_ctl_(win, 0, x, y, w, h, );
				if (!ctl){
					LOGI("cannot create %s control", tag->name);
					return 1;
				}
			} break;
			case LAYOUT_TAG_TEXT:{
				//ctl=libaroma_ctl_(win, 0, x, y, w, h, );
				if (!ctl){
					LOGI("cannot create %s control", tag->name);
					return 1;
				}
			} break;
			case LAYOUT_TAG_TABS:{
				//ctl=libaroma_ctl_(win, 0, x, y, w, h, );
				if (!ctl){
					LOGI("cannot create %s control", tag->name);
					return 1;
				}
			} break;
			case LAYOUT_TAG_IMG:{
				//ctl=libaroma_ctl_(win, 0, x, y, w, h, );
				if (!ctl){
					LOGI("cannot create %s control", tag->name);
					return 1;
				}
			} break;
			case LAYOUT_TAG_SLID:{
				char *max_str=libaroma_xml_attr(tag, "max");
				char *val_str=libaroma_xml_attr(tag, "value");
				if (!max_str){
					LOGI("no max specified");
					return 1; 
				}
				int max, val;
				max=atoi(max_str);
				if (max<1){
					LOGI("invalid max");
					return 1;
				}
				if (!val_str) val=0; 
				ctl=libaroma_ctl_slider(win, 0, x, y, w, h, max, val);
				if (!ctl){
					LOGI("cannot create %s control", tag->name);
					return 1;
				}
			} break;
			case LAYOUT_TAG_BAR:{
				//ctl=libaroma_ctl_(win, 0, x, y, w, h, );
				if (!ctl){
					LOGI("cannot create %s control", tag->name);
					return 1;
				}
			} break;
			case LAYOUT_TAG_FRAG:{
				//ctl=libaroma_ctl_(win, 0, x, y, w, h, );
				if (!ctl){
					LOGI("cannot create %s control", tag->name);
					return 1;
				}
			} break;/*
			case LAYOUT_TAG_:{
				
			} break;*/
			default:
				return 1;
		}
	}
	LIBAROMA_XML child;
	for (child=tag->child; child; child=child->ordered){
		_layout_parse_curdepth++;
		if (!_layout_parse_tag(child, data)){
			LOGI("nope bugs, not today");
			break;
		}
		_layout_parse_curdepth--;
	}
	return 1;
}

byte layout_parse_ex(char *layout, int len){
	if (!layout){
		return 0;
	}
	LOGI("parse xml:\n%s\nwith len=%d", layout, len);
	LIBAROMA_XML xml = libaroma_xml(layout, len);
	if (!xml){
		LOGI("failed to parse xml");
		return 0;
	}
	LIBAROMA_XML tag;
	LOGI("logging tags");
	_layout_parse_curdepth=0;
	/* parse root childs */
	for (tag=xml->child; tag; tag=tag->ordered){
		if (!_layout_parse_tag(tag, NULL)){
			LOGI("Hey bro, something failed miserably, for you safety I'll break this loop");
			break;
		}
	}
	LOGI("release that xml");
	libaroma_xml_free(xml);
	return 1;
}

byte layout_parse_xml(char *path){
	if (!path){
		return 0;
	}
	LOGI("lemme load that for u");
	LIBAROMA_STREAMP stream = libaroma_stream(path);
	if (!stream){
		LOGI("nope - I failed");
		return 0;
	}
	byte ret=0;
	LOGI("duplicate data in order to write it");
	char *dup_data = strdup(stream->data);
	if (!dup_data){
		LOGI("could not duplicate data, sorry");
		
	}
	LOGI("now parse it, I'm so servicial ;)");
	ret=layout_parse_ex(dup_data, stream->size);
	LOGI("and finally unload what we won't use again");
	libaroma_stream_close(stream);
	free(dup_data);
	LOGI("nice to meet u!");
	return ret;
}

#endif /*__layout_parser_c__*/