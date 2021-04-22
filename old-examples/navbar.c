int NAVBAR_HEIGHT=30;

#define		navbar_draw(win)\
				navbar_draw_ex(win, NULL, NULL)
				
#define			ID_NAVMENU		71
#define			ID_NAVHOME		72
#define			ID_NAVBACK		73

/*	Function:		navbar_draw_ex
	Return type:	void
	Description:	Draws a simple navbar in the window
*/
void navbar_draw_ex(LIBAROMA_WINDOWP win, word bg_color, word accent){

	//printf("Creating navbar!\n");

	if (!bg_color) bg_color=RGB(000000);
	
	LIBAROMA_CANVASP menu_icon = libaroma_canvas_ex(libaroma_dp(NAVBAR_HEIGHT), libaroma_dp(NAVBAR_HEIGHT), 0xFF);
	LIBAROMA_CANVASP home_icon = libaroma_canvas_ex(libaroma_dp(NAVBAR_HEIGHT), libaroma_dp(NAVBAR_HEIGHT), 0xFF);
	LIBAROMA_CANVASP back_icon = libaroma_canvas_ex(libaroma_dp(NAVBAR_HEIGHT), libaroma_dp(NAVBAR_HEIGHT), 0xFF);
	// let's use 64x64 PNGs in order to get some quality 
	libaroma_draw_scale_smooth(menu_icon, libaroma_image_uri("res:///icons/menu.png"), 
									0, 0, menu_icon->w, menu_icon->h, 0, 0, 64, 64); 
	libaroma_draw_scale_smooth(home_icon, libaroma_image_uri("res:///icons/home.png"), 
									0, 0, home_icon->w, home_icon->h, 0, 0, 64, 64);
	libaroma_draw_scale_smooth(back_icon, libaroma_image_uri("res:///icons/back.png"),
									0, 0, back_icon->w, back_icon->h, 0, 0, 64, 64);
									
	if (accent){
		libaroma_canvas_fillcolor(menu_icon, accent);
		libaroma_canvas_fillcolor(home_icon, accent);
		libaroma_canvas_fillcolor(back_icon, accent);
	}

	int menubtn_x=(win->w/2)/2;
	int backbtn_x=((win->w/2)/2)*3;	
	int navbar_y=libaroma_px(win->h-libaroma_dp(NAVBAR_HEIGHT));
	
	libaroma_draw_rect(win->bg, 0, navbar_y, libaroma_px(win->w), libaroma_dp(30), bg_color, 0);
	LIBAROMA_CONTROLP menubtn_list = libaroma_ctl_list(
		win, ID_NAVMENU,
		0, navbar_y, 96, NAVBAR_HEIGHT,
		0, 0,
		bg_color,
		LIBAROMA_CTL_SCROLL_NO_INDICATOR);
	libaroma_listitem_image(menubtn_list, ID_NAVMENU, menu_icon, 
								NAVBAR_HEIGHT, LIBAROMA_CTL_LIST_ITEM_RECEIVE_TOUCH, -1);
	LIBAROMA_CONTROLP homebtn_list = libaroma_ctl_list(
		win, ID_NAVHOME,
		libaroma_px((win->w/2)-(libaroma_dp(96)/2)), navbar_y, 96, NAVBAR_HEIGHT,
		0, 0,
		bg_color,
		LIBAROMA_CTL_SCROLL_NO_INDICATOR);
	libaroma_listitem_image(homebtn_list, ID_NAVHOME, home_icon, 
								NAVBAR_HEIGHT, LIBAROMA_CTL_LIST_ITEM_RECEIVE_TOUCH, -1);	
	LIBAROMA_CONTROLP backbtn_list = libaroma_ctl_list(
		win, ID_NAVBACK,
		libaroma_px(win->w-libaroma_dp(96)), navbar_y, 96, NAVBAR_HEIGHT,
		0, 0,
		bg_color,
		LIBAROMA_CTL_SCROLL_NO_INDICATOR);
	libaroma_listitem_image(backbtn_list, ID_NAVBACK, back_icon, 
								NAVBAR_HEIGHT, LIBAROMA_CTL_LIST_ITEM_RECEIVE_TOUCH, -1);	

	/*printf("********DEBUG********\n");
	printf("Navbar height: %dpx->%ddp\n", NAVBAR_HEIGHT, libaroma_dp(NAVBAR_HEIGHT));
	printf("Menulist size: %dx%d at %d, %d\n", menubtn_list->w, menubtn_list->h, menubtn_list->x, menubtn_list->y);
	printf("Homelist size: %dx%d at %d, %d\n", homebtn_list->w, homebtn_list->h, homebtn_list->x, homebtn_list->y);
	printf("Backlist size: %dx%d at %d, %d\n", backbtn_list->w, backbtn_list->h, backbtn_list->x, backbtn_list->y);
	printf("*********************\n");
	*/
}