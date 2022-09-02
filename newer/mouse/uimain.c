#include "common.h"
#include <pthread.h>
#include <SDL/sdl.h>

pthread_t updater_tid;
LIBAROMA_WINDOWP win;
LIBAROMA_CONTROLP image, minimap;
LIBAROMA_CANVASP maincv, area, minimap_cv, mini_maincv;
static LIBAROMA_RECT viewport={0};
byte mouse_down=0, mouse_panning=0;
double scale_factor;


//void *mlx_ui_thread(void *data);
byte (*ori_ui_thread)(LIBAROMA_WINDOWP win);
byte mlx_ui_thread(LIBAROMA_WINDOWP data);

void uimain(){
	//init canvas variables to null
	maincv=area=minimap_cv=mini_maincv=NULL;
	//init libaroma window
	win=libaroma_window(0,0,0,0,0);
	if (!win) return;
	/* set ui thread hook */
	ori_ui_thread=win->ui_thread;
	win->ui_thread=&mlx_ui_thread;
	//allocate canvases
	maincv = libaroma_canvas(win->w*3, win->w*3);
	if (!maincv) goto exit;
	//fill canvas with something
	LIBAROMA_CANVASP randomimg=libaroma_image(libaroma_stream_file("D:\\MLX\\Pictures\\static_wikia_nocookie_net-1200.png"), 1);
	if (!randomimg) goto exit;
	libaroma_draw_scale_smooth(maincv, randomimg, 0, 0, maincv->w, maincv->h, 0, 0, randomimg->w, randomimg->h);
	libaroma_canvas_free(randomimg);
	randomimg=NULL;
	//prepare viewport
	byte centered_viewport=0;
	if (centered_viewport){
		viewport.x=win->w+libaroma_dp(16);
		viewport.y=win->h+libaroma_dp(24);
	}
	else {
		viewport.x=maincv->w-(win->w-libaroma_dp(32));
		viewport.y=0;
	}
	viewport.w=win->w-libaroma_dp(32);
	viewport.h=win->h-libaroma_dp(120);
	area=libaroma_canvas_area(maincv, viewport.x, viewport.y, viewport.w, viewport.h);
	//area=libaroma_canvas(viewport.w, viewport.h);
	if (!area) goto exit;
	//libaroma_draw_ex(area, maincv, 0, 0, viewport.x, viewport.y, viewport.w, viewport.h, 0, 0xFF);
	image=libaroma_ctl_image_canvas_ex(win, 0, area, 16, 24, libaroma_px(viewport.w), libaroma_px(viewport.h), LIBAROMA_CTL_IMAGE_SHARED);
	if (!image) goto exit;
	//create minimap
	scale_factor=((double)libaroma_dp(96)/(double)maincv->h);
	minimap_cv = libaroma_canvas(((double)maincv->w)*scale_factor, libaroma_dp(96));
	if (!minimap_cv) goto exit;
	LIBAROMA_CANVASP temp_minicv=libaroma_image(libaroma_stream_file("D:\\MLX\\Pictures\\Summoners_Rift_Minimap.png"), 1);
	mini_maincv=libaroma_canvas(minimap_cv->w, minimap_cv->h);
	if (!temp_minicv || !mini_maincv) goto exit;
	libaroma_draw_scale_smooth(mini_maincv, temp_minicv, 0, 0, mini_maincv->w, mini_maincv->h, 0, 0, temp_minicv->w, temp_minicv->h);
	libaroma_canvas_free(temp_minicv);
	//libaroma_draw_scale_smooth(mini_maincv, maincv, 0, 0, mini_maincv->w, mini_maincv->h, 0, 0, maincv->w, maincv->h);
	libaroma_draw(minimap_cv, mini_maincv, 0, 0, 0);
	libaroma_draw_rect(minimap_cv, 
						((double)viewport.x)*scale_factor, ((double)viewport.y)*scale_factor,
						((double)viewport.w)*scale_factor, libaroma_dp(1), 
						RGB(FFFFFF), 0xFF);
	libaroma_draw_rect(minimap_cv, 
						((double)viewport.x)*scale_factor, (((double)viewport.y)*scale_factor+((double)viewport.h)*scale_factor)-libaroma_dp(1),
						((double)viewport.w)*scale_factor, libaroma_dp(1), 
						RGB(FFFFFF), 0xFF);
	libaroma_draw_rect(minimap_cv, 
						((double)viewport.x)*scale_factor, ((double)viewport.y)*scale_factor,
						libaroma_dp(1), ((double)viewport.h)*scale_factor,
						RGB(FFFFFF), 0xFF);
	libaroma_draw_rect(minimap_cv, 
						(((double)viewport.x)*scale_factor+((double)viewport.w)*scale_factor)-libaroma_dp(1), ((double)viewport.y)*scale_factor,
						libaroma_dp(1), ((double)viewport.h)*scale_factor, 
						RGB(FFFFFF), 0xFF);
	minimap=libaroma_ctl_image_canvas_ex(win, 0, minimap_cv, libaroma_px(win->w-minimap_cv->w), win->height-96, libaroma_px(minimap_cv->w), 96, LIBAROMA_CTL_IMAGE_SHARED);
	if (!minimap) goto exit;
	
	libaroma_window_show(win);
	LIBAROMA_MSG msg={0};
	win->onpool=1;
	byte tryagain=0;
	byte moveflag=0;
	do {
		libaroma_window_pool(win, &msg);
		if (msg.msg==LIBAROMA_MSG_EXIT){
			win->onpool=0;
			break;
		}
		moveflag=0;
		if (msg.msg==LIBAROMA_MSG_TOUCH){
			if (msg.state==2){ //mouse_move
				if (mouse_down){
					int grow_x=((double)(msg.x-(win->w-minimap_cv->w)))/scale_factor;
					int grow_y=((double)(msg.y-(win->h-minimap_cv->h)))/scale_factor;
					if (grow_x<(viewport.w/2)) grow_x=0;
					else if (grow_x>(maincv->w-(viewport.w/2))) grow_x=(maincv->w-viewport.w);
					else grow_x-=(viewport.w/2);
					if (grow_y<(viewport.h/2)) grow_y=0;
					else if (grow_y>(maincv->h-(viewport.h/2))) grow_y=(maincv->h-viewport.h);
					else grow_y-=(viewport.h/2);
					printf("dragging (%dx%d)->(%dx%d)\n", msg.x, (msg.y-(win->h-libaroma_dp(96))), grow_x, grow_y);
					//update minimap
					viewport.x=grow_x;
					viewport.y=grow_y;
					libaroma_canvas_area_update(area, maincv, viewport.x, viewport.y, viewport.w, viewport.h);
					libaroma_ctl_image_update(image);
					libaroma_draw(minimap_cv, mini_maincv, 0, 0, 0);
					libaroma_draw_rect(minimap_cv, 
										((double)viewport.x)*scale_factor, ((double)viewport.y)*scale_factor,
										((double)viewport.w)*scale_factor, libaroma_dp(1), 
										RGB(FFFFFF), 0xFF);
					libaroma_draw_rect(minimap_cv, 
										((double)viewport.x)*scale_factor, (((double)viewport.y)*scale_factor+((double)viewport.h)*scale_factor)-libaroma_dp(1),
										((double)viewport.w)*scale_factor, libaroma_dp(1), 
										RGB(FFFFFF), 0xFF);
					libaroma_draw_rect(minimap_cv, 
										((double)viewport.x)*scale_factor, ((double)viewport.y)*scale_factor,
										libaroma_dp(1), ((double)viewport.h)*scale_factor,
										RGB(FFFFFF), 0xFF);
					libaroma_draw_rect(minimap_cv, 
										(((double)viewport.x)*scale_factor+((double)viewport.w)*scale_factor)-libaroma_dp(1), ((double)viewport.y)*scale_factor,
										libaroma_dp(1), ((double)viewport.h)*scale_factor, 
										RGB(FFFFFF), 0xFF);
					libaroma_ctl_image_update(minimap);
				}
				else {
					if (msg.y<=libaroma_dp(16)){
						moveflag |= MOVE_TOP;
					}
					else if (msg.y>=win->h-libaroma_dp(16)){
						moveflag |= MOVE_BOT;
					}
					if (msg.x<=libaroma_dp(16)){
						moveflag |= MOVE_LEFT;
					}
					else if (msg.x>=win->w-libaroma_dp(16)){
						moveflag |= MOVE_RIGHT;
					}
					mouse_panning=moveflag;
				}
			}
			else if (msg.state==1){ //mouse_down
				printf("mouse down\n");
				mouse_down=1;
			}
			else { //mouse_up
				printf("mouse up\n");
				mouse_down=0;
				if (msg.key==LIBAROMA_HID_RMOUSE_KEYCODE){
					tryagain=!tryagain;
					SDL_WM_GrabInput(tryagain?SDL_GRAB_ON:SDL_GRAB_OFF);
				}
			}
		}
	} while (win->onpool);
exit:
	libaroma_window_free(win);
	if (mini_maincv) libaroma_canvas_free(mini_maincv);
	if (minimap_cv) libaroma_canvas_free(minimap_cv);
	if (randomimg) libaroma_canvas_free(randomimg);
	if (area) libaroma_canvas_free(area);
	if (maincv) libaroma_canvas_free(maincv);
}

//void *mlx_ui_thread(void *data){
byte mlx_ui_thread(LIBAROMA_WINDOWP data){
	if (mouse_panning){
		byte updated=0;
		if ((mouse_panning&MOVE_TOP)&&viewport.y>=10){
			viewport.y-=10;
			updated=1;
		}
		else if ((mouse_panning&MOVE_BOT)&&viewport.y<=(maincv->h-viewport.h)-10){
			viewport.y+=10;
			updated=1;
		}
		if ((mouse_panning&MOVE_LEFT)&&viewport.x>=10){
			viewport.x-=10;
			updated=1;
		}
		else if ((mouse_panning&MOVE_RIGHT)&&viewport.x<=(maincv->w-viewport.w)-10){
			viewport.x+=10;
			updated=1;
		}
		if (updated){
			char movestr[64];
			sprintf(movestr, "move: ");
			if (mouse_panning&MOVE_TOP) strcat(movestr, "top ");
			if (mouse_panning&MOVE_BOT) strcat(movestr, "bot ");
			if (mouse_panning&MOVE_LEFT) strcat(movestr, "left ");
			if (mouse_panning&MOVE_RIGHT) strcat(movestr, "right ");
			printf("%s\n", movestr);
			libaroma_canvas_area_update(area, maincv, viewport.x, viewport.y, viewport.w, viewport.h);
			libaroma_ctl_image_update(image);
			//libaroma_draw_ex(area, maincv, 0, 0, viewport.x, viewport.y, viewport.w, viewport.h, 0, 0xFF);
			//update minimap
			libaroma_draw(minimap_cv, mini_maincv, 0, 0, 0);
			libaroma_draw_rect(minimap_cv, 
								((double)viewport.x)*scale_factor, ((double)viewport.y)*scale_factor,
								((double)viewport.w)*scale_factor, libaroma_dp(1), 
								RGB(FFFFFF), 0xFF);
			libaroma_draw_rect(minimap_cv, 
								((double)viewport.x)*scale_factor, (((double)viewport.y)*scale_factor+((double)viewport.h)*scale_factor)-libaroma_dp(1),
								((double)viewport.w)*scale_factor, libaroma_dp(1), 
								RGB(FFFFFF), 0xFF);
			libaroma_draw_rect(minimap_cv, 
								((double)viewport.x)*scale_factor, ((double)viewport.y)*scale_factor,
								libaroma_dp(1), ((double)viewport.h)*scale_factor,
								RGB(FFFFFF), 0xFF);
			libaroma_draw_rect(minimap_cv, 
								(((double)viewport.x)*scale_factor+((double)viewport.w)*scale_factor)-libaroma_dp(1), ((double)viewport.y)*scale_factor,
								libaroma_dp(1), ((double)viewport.h)*scale_factor, 
								RGB(FFFFFF), 0xFF);
			libaroma_ctl_image_update(minimap);
		}
	}
	return ori_ui_thread(data);
}