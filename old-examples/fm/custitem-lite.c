LIBAROMA_CANVASP custitem
	(LIBAROMA_CANVASP icon, char *text, char *extra, 
	word bg_color, word accent, 
	int width, int height){
	
	if (!width) width=libaroma_dp(180);
	if (!height)height=libaroma_dp(54);	
	if (!bg_color) bg_color=RGB(EEEEE);
	if (!accent) accent=RGB(000000);
	
	int debug=1;
	
	if (debug) printf("Creating canvas with width %d and height %d\n", width, height);
	LIBAROMA_CANVASP cv=libaroma_canvas(width, height);
	libaroma_canvas_fillcolor(cv, bg_color);
	int iconx=libaroma_dp(5);
	int iconsz=libaroma_dp(28);
	int icony=(cv->h/2)-(iconsz/2);
	word textflags=LIBAROMA_FONT(0, 3)|LIBAROMA_TEXT_SINGLELINE;
	
	int textpadding=libaroma_dp(6);
	int textx=iconx+iconsz+textpadding;
	int textw=(cv->w-textx)-textpadding;
	LIBAROMA_TEXT txt=libaroma_text(text, accent, textw, textflags, 0);
	int texth=libaroma_text_height(txt);
	int texty=(cv->h/2)-(texth/2)-2;
	if (extra) {
		word extraflags=LIBAROMA_FONT(0, 2)|LIBAROMA_TEXT_SINGLELINE;
		LIBAROMA_TEXT extratxt=libaroma_text(extra, RGB(777777), textw, extraflags, 0);
		texty=(cv->h/2)-texth;
		int extray=(cv->h/2);
		if (debug) printf("Drawing extra text \"%s\" at x: %d and y: %d\n", extra, textx, extray);
		libaroma_draw_text(cv, extra, textx, extray, RGB(777777), textw, extraflags, NULL);
		//libaroma_text_free(extratxt);
	}
	if (debug) printf("Drawing \"%s\" at x: %d, y: %d with width %d and height %d\n", text, textx, texty, textw, texth);
	libaroma_draw_text(cv, text, textx, texty, accent, textw, textflags, NULL);
	//libaroma_text_free(txt);
	if (debug) printf("Drawing icon with x %d, y %d and size %d\n", iconx, icony, iconsz);
	libaroma_draw_scale_smooth(cv, icon, iconx, icony, iconsz, iconsz, 0, 0, icon->w, icon->h);	
}