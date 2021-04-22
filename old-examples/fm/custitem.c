#define CUSTITEM_NO_CENTER		0x01
#define CUSTITEM_NO_CIRCLE		0x02
#define CUSTITEM_NO_FILL_ICON	0x04
#define CUSTITEM_CIRCLE_RIGHT	0x08
#define CUSTITEM_SMALL			0x16
#define custitem(icon, text, extra, w, h) \
	custitem_ex(icon, text, extra, NULL, NULL, w, h, CUSTITEM_NO_CIRCLE|CUSTITEM_NO_FILL_ICON|CUSTITEM_SMALL)

int screen_issmall=0;

LIBAROMA_CANVASP custitem_ex
	(LIBAROMA_CANVASP icon, char *text, char *extra, 
	word bg_color, word accent, 
	int width, int height, /*
	byte isright, byte drawcircle, 
	byte fillicon, byte small*/
	word flags){
	int debug=0;
	byte isright=(flags&CUSTITEM_CIRCLE_RIGHT);
	byte drawcircle=!(flags&CUSTITEM_NO_CIRCLE);
	byte fillicon=!(flags&CUSTITEM_NO_FILL_ICON);
	byte small=(flags&CUSTITEM_SMALL);
	if (!width) width=screen_issmall?libaroma_dp(120):libaroma_dp(180);
	if (!height){
		if (screen_issmall)
			height=small?libaroma_dp(38):libaroma_dp(54);
		else height=small?libaroma_dp(28):libaroma_dp(54);
	}
	if (!bg_color) bg_color=RGB(EEEEEE);
	if (!accent) accent=RGB(000000);
	if (debug) printf("Creating canvas with width %d and height %d\n", width, height);
	LIBAROMA_CANVASP cv=libaroma_canvas(width, height);
	int circlex=libaroma_dp(5);
	int circley=cv->h/2;
	int circlesz=libaroma_dp(80);
	int textpadding=libaroma_dp(6);
	int textx=circlex+(circlesz/2)+textpadding;
	int textw=(cv->w-textx)-textpadding;
	int iconx=libaroma_dp(5);
	int iconsz=libaroma_dp(28);
	int icony=(cv->h/2)-(iconsz/2);
	byte centered=0;
	word textflags=LIBAROMA_FONT(0, /*small?2:*/3)|LIBAROMA_TEXT_SINGLELINE;
	if (isright){
		circlex=cv->w-circlex;
		iconx=circlex-iconsz;
		textx=textpadding;
		if (icon) {
			if (debug) printf("Right control, setting right text!\n");
		}
		else {
			if (debug) printf("%s", (flags&CUSTITEM_NO_CENTER)?"":"Right control with no icon, setting centered text!\n");
			centered=(flags&CUSTITEM_NO_CENTER)?0:1;
		}
	}
	if (!icon) {
		textw=cv->w-(textpadding/2);
		if (!isright){
			textx=textpadding;
			if (debug) printf("%s", (flags&CUSTITEM_NO_CENTER)?"":"Left control with no icon, setting centered text!\n");
			centered=(flags&CUSTITEM_NO_CENTER)?0:1;
		}
	}
	LIBAROMA_TEXT txt=libaroma_text(text, accent, textw, textflags, 0);
	int texth=libaroma_text_height(txt);
	int origw=libaroma_text_width(txt);
	int texty=(cv->h/2)-(texth/2)-2;
	libaroma_canvas_fillcolor(cv, bg_color);
	if (extra) {
		if (debug) printf("Extra text is going to be drawn!\n");
		word extraflags=LIBAROMA_FONT(0, 2)|LIBAROMA_TEXT_SINGLELINE;
		LIBAROMA_TEXT extratxt=libaroma_text(extra, RGB(777777), textw, extraflags, 0);
		texty=(cv->h/2)-texth;
		int extray=(cv->h/2);//+libaroma_text_height(extratxt);
		if (debug) printf("Drawing extra text \"%s\" at x: %d and y: %d\n", extra, textx, extray);
		libaroma_draw_text(cv, extra, textx, extray, RGB(777777), textw, extraflags|(isright?LIBAROMA_TEXT_RIGHT:0), NULL);
		libaroma_text_free(extratxt);
	}
	if (debug) printf("Drawing \"%s\" at x: %d, y: %d with width %d and height %d\n", text, textx, texty, textw, texth);
	//libaroma_text_draw(cv, txt, textx, texty);	
	if (centered)
		libaroma_draw_text(cv, text, textx, texty, accent, textw, textflags|LIBAROMA_TEXT_CENTER, NULL);
	else libaroma_draw_text(cv, text, textx, texty, accent, textw, textflags|(isright?LIBAROMA_TEXT_RIGHT:0), NULL);
		
	if (drawcircle){
		if (debug) printf("Drawing circle with x %d, y %d and size %d\n", circlex, circley, circlesz);
		libaroma_draw_circle(cv, accent, circlex, circley, circlesz, 0xFF);
	}
	if (fillicon && icon!=NULL) libaroma_canvas_fillcolor(icon, drawcircle?bg_color:accent);
	
	if (icon!=NULL){
		if (debug) printf("Drawing icon with x %d, y %d and size %d\n", iconx, icony, iconsz);
		libaroma_draw_scale_smooth(cv, icon, iconx, icony, iconsz, iconsz, 0, 0, icon->w, icon->h);
	}
	//libaroma_draw_pixel(cv, circlex, circley, RGB(FFFFFF), 0xFF);
	libaroma_text_free(txt);
	return cv;
}