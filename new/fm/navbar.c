#ifndef __aromafm_navbar_c__
#define __aromafm_navbar_c__
#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

static FM_NAVBAR _navbar={0};

FM_NAVBARP navbar(){
	return &_navbar;
}

byte mlx_navbar_draw(){
	byte res = libaroma_ripple_thread(_navbar.ripple, 0);
	if (res&LIBAROMA_RIPPLE_HOLDED)
		aromafm_navbar_onhold(_navbar.updated_btn);
	if (res&LIBAROMA_RIPPLE_REDRAW){
		_navbar.redraw=1;
		int ripple_i = 0;
		int ripple_p = 0;
		while(libaroma_ripple_loop(_navbar.ripple,&ripple_i,&ripple_p)){
			libaroma_canvas_fillcolor(_navbar.cv, 0);
			int x=0;
			int y=0;
			int size=0;
			byte push_opacity=0;
			byte ripple_opacity=0;
			if (libaroma_ripple_calculation(
				_navbar.ripple, _navbar.button_width, _navbar.cv->h, &push_opacity, &ripple_opacity,
				&x, &y, &size,ripple_p
			)){
				libaroma_draw_circle(
					_navbar.cv, _navbar.accent, x+(_navbar.button_width*_navbar.updated_btn), y, size, push_opacity
				);
			}
			/* draw icons */
			libaroma_draw(_navbar.cv, _navbar.back, (_navbar.button_width-_navbar.cv->h)/2, 0, 3);
			libaroma_draw(_navbar.cv, _navbar.home, _navbar.button_width+((_navbar.button_width-_navbar.cv->h)/2), 0, 3);
			libaroma_draw(_navbar.cv, _navbar.menu, (_navbar.button_width*2)+((_navbar.button_width-_navbar.cv->h)/2), 0, 3);
		}
	} else _navbar.redraw=0;
	
	return _navbar.redraw;
}

byte mlx_navbar_msg(LIBAROMA_WMP wm, LIBAROMA_MSGP msg){
	if (msg->y > (wm->y+wm->h)){ /* touch outside of wm workspace */
		int x=msg->x; /* first button */
		int y=msg->y-(libaroma_wm()->h);
		if (msg->x>libaroma_fb()->w-(_navbar.button_width)){ /* third button */
			if (!_navbar.updating) _navbar.updated_btn=FM_NAVBAR_MENU;
		}
		else if (msg->x>_navbar.button_width){ /* second button */
			if (!_navbar.updating) _navbar.updated_btn=FM_NAVBAR_HOME;
		}
		else { /* first button */
			if (!_navbar.updating) _navbar.updated_btn=FM_NAVBAR_BACK;
		}
		/* fix position */
		x-=_navbar.button_width*_navbar.updated_btn;
		if (x>_navbar.button_width){
			printf("fixing x=%d too big\n", x);
			x=_navbar.button_width;
		}
		else if (x<0){
			printf("fixing x=%d too low\n", x);
			x=0;
		}
		//printf("touch at %d, %d (%d)\n", x, y, _navbar.button_width);
		/* handle touch states */
		switch (msg->state){
			case LIBAROMA_HID_EV_STATE_DOWN:
				_navbar.updating=1;
				libaroma_ripple_down(_navbar.ripple, x, y);
				break;
			case LIBAROMA_HID_EV_STATE_MOVE:
				libaroma_ripple_move(_navbar.ripple, x, y);
				break;
			case LIBAROMA_HID_EV_STATE_UP:{
				byte ret = libaroma_ripple_up(_navbar.ripple, 0);
				if ((ret&LIBAROMA_RIPPLE_TOUCHED) && !(ret&LIBAROMA_RIPPLE_HOLDED))
					aromafm_navbar_onclick(_navbar.updated_btn);
				_navbar.updating=0;
			} break;
		}
		return 1; /* this is equivalent to Android's "don't send navbar events to apps" */
	}
	else if (_navbar.updating){ /* moved outside of navbar while dragging */
		_navbar.updating=0;
		libaroma_ripple_cancel(_navbar.ripple);
		return 1;
	}
	return 0; /* don't intercept messages pls */
}

byte mlx_navbar_setcolor(word color){
	_navbar.accent=color;
	libaroma_canvas_fillcolor(_navbar.back, color);
	libaroma_canvas_fillcolor(_navbar.home, color);
	libaroma_canvas_fillcolor(_navbar.menu, color);
}

byte mlx_navbar_init(){
	/* use framebuffer canvas area */
	_navbar.cv=libaroma_canvas_area(libaroma_fb()->canvas, 0, libaroma_wm()->y+libaroma_wm()->h, libaroma_fb()->w, aromafm_statusbar_height()*2);
	/* load resources */
	LIBAROMA_CANVASP back=libaroma_image_uri("res:///icons/back.png"),
					home=libaroma_image_uri("res:///icons/home.png"),
					menu=libaroma_image_uri("res:///icons/menu.png");
	int ico_size=_navbar.cv->h;
	_navbar.back=libaroma_canvas_ex(ico_size, ico_size, 1);
	_navbar.home=libaroma_canvas_ex(ico_size, ico_size, 1);
	_navbar.menu=libaroma_canvas_ex(ico_size, ico_size, 1);
	/* scale resources */
	libaroma_draw_scale_smooth(_navbar.back, back, 0, 0, ico_size, ico_size, 0, 0, back->w, back->h);
	libaroma_draw_scale_smooth(_navbar.home, home, 0, 0, ico_size, ico_size, 0, 0, home->w, home->h);
	libaroma_draw_scale_smooth(_navbar.menu, menu, 0, 0, ico_size, ico_size, 0, 0, menu->w, menu->h);
	libaroma_canvas_free(back);
	libaroma_canvas_free(home);
	libaroma_canvas_free(menu);
	/* set default color */
	mlx_navbar_setcolor(RGB(FF0000));
	/* alloc ripple */
	_navbar.ripple = calloc(sizeof(LIBAROMA_RIPPLE), 1);
	/* setup button size */
	_navbar.button_width=_navbar.cv->w/3;
	/* set callbacks */
	_navbar.draw_cb=&mlx_navbar_draw;
	libaroma_wm_set_ui_thread(_navbar.draw_cb);
	_navbar.msg_cb=&mlx_navbar_msg;
	libaroma_wm_set_message_handler(_navbar.msg_cb);
	return 1;
}


#ifdef __cplusplus
}
#endif
#endif /* __aromafm_navbar_c__ */  
 
