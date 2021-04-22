/*byte has_sb(int h){
	if (!h) return 0;
	byte res = h>(libaroma_fb()->h-STATUSBAR_HEIGHT);
	if (debug) printf("height %d has %sstatusbar\n", h, res?"":"no ");
	return res;
}*/
#include <fcntl.h>

float vibrate_rate = 0.5;
void set_vibrate_rate(byte rate) {
  vibrate_rate = ((float) rate) / 10;
}
//-- VIBRATE FUNCTION
int vibrate(int timeout_ms) {
  if (vibrate_rate == 0) {
    // NO VIBRATE
    return 0;
  }
  
  char str[20];
  int fd;
  int ret;
  fd = open("/sys/class/timed_output/vibrator/enable", O_WRONLY);
  
  if (fd < 0) {
    return -1;
  }
  
  ret = snprintf(str, sizeof(str), "%d", (int) round((float) timeout_ms * vibrate_rate));
  ret = write(fd, str, ret);
  close(fd);
  
  if (ret < 0) {
    return -1;
  }
  
  return 0;
}

static  FILE  * acmd_pipe;
//*
//* Pass Recovery PIPE
//*
FILE * apipe() {
  return acmd_pipe;
}

#define LIBAROMA_WINDOW_SHOW_ANIMATION_SCALE 123
void animate(LIBAROMA_CONTROLP imgctl,
		LIBAROMA_CANVASP back,
		LIBAROMA_CANVASP fore,
		byte animation, int duration, byte reverse){
	if (!back || !fore){
		return;
	}
	printf("animate: called!\n");
	LIBAROMA_WINDOWP win=imgctl->window;
	printf("animate: type %ld\n", animation);
	long start = libaroma_tick();
	int delta = 0;
	//duration=3000;
	LIBAROMA_CANVASP tempcv=libaroma_canvas_dup(back);
	//libaroma_png_save(back, "/tmp/back.png");
	//libaroma_png_save(fore, "/tmp/fore.png");
	while ((delta=libaroma_tick()-start)<duration){ //loop between animation frames
		//printf("animate: loop\n");
		float state = ((float) delta)/((float) duration);
		if (state>=1.0){
			break;
		}
		libaroma_draw(tempcv, back, 0, 0, 0);
		switch (animation){
			case LIBAROMA_WINDOW_SHOW_ANIMATION_FADE:
				{
					float swift_out_state = libaroma_cubic_bezier_swiftout(state); //get variation based off of state
					float bstate = 255.0 * swift_out_state;
					byte bbstate = (byte) round(bstate);
					libaroma_draw_opacity(
						tempcv, fore, 0,0,0, bbstate //draw fore on top of back
					);
				}
				break;
			case LIBAROMA_WINDOW_SHOW_ANIMATION_SCALE:
				{
					float swift_out_state = libaroma_cubic_bezier_swiftout(state);
					int center_x=fore->w/2;
					int center_y=fore->h/2;
					int w;
					int h;
					int x;
					int y;
					w=swift_out_state * (fore->w);
					h=swift_out_state * (fore->h);
					x=(fore->w/2) - (swift_out_state * (fore->w/2));
					y=(fore->h/2) - (swift_out_state * (fore->h/2));
					//float wdelta=fore->w/w;
					//float hdelta=fore->h/h;
					if (reverse){
						w=(fore->w - (swift_out_state * fore->w));
						h=(fore->h - (swift_out_state * fore->h));
						x=center_x-(w/2);
						y=center_y-(h/2);
					}
					//printf("Scale %sdrawn at %d, %d with WxH %dx%d\n", reverse?"(reversed) ":"", x, y, w, h);
					libaroma_draw_scale_smooth(
						tempcv,
						fore,
						x, y, w, h, 0, 0, fore->w, fore->h
					);	
				}
				break;
			case LIBAROMA_WINDOW_SHOW_ANIMATION_SLIDE_LEFT:
			case LIBAROMA_WINDOW_SHOW_ANIMATION_PAGE_LEFT:
				{
					float swift_out_state = libaroma_cubic_bezier_swiftout(state);
					int x = fore->w - (swift_out_state * fore->w);
					int w = fore->w - x;
					if (reverse){
						x=(swift_out_state * fore->w);// + fore->w;
						w=fore->w - x;
					}
					if (w>0){
						
						if (animation==LIBAROMA_WINDOW_SHOW_ANIMATION_SLIDE_LEFT){
							int back_x=fore->w - (fore->w - w);
							int back_w=fore->w - w;
							if (w<fore->w){
								libaroma_draw_ex(
									tempcv,
									back,
									0, 0, back_x, 0, back_w, fore->h,
									0, 0xff
								);
							}
						}
						libaroma_draw_ex(
							tempcv,
							fore,
							x, 0, 0, 0, w, fore->h,
							0, 0xff
						);
					}
				}
				break;
			case LIBAROMA_WINDOW_SHOW_ANIMATION_SLIDE_TOP:
			case LIBAROMA_WINDOW_SHOW_ANIMATION_PAGE_TOP:
				{
					float swift_out_state = libaroma_cubic_bezier_swiftout(state);
					int y = fore->h - (swift_out_state * fore->h);
					int h = fore->h - y;
					if (reverse){
						y=(swift_out_state * fore->h);
						h=fore->h - y;
					}
					if (h>0){
						if (animation==LIBAROMA_WINDOW_SHOW_ANIMATION_SLIDE_TOP){
							if (h<fore->h){
								libaroma_draw_ex(
									tempcv,
									back,
									0, 0, 0, fore->h - (fore->h - h), fore->w, fore->h-h,
									0, 0xff
								);
							}
						}
						libaroma_draw_ex(
							tempcv,
							fore,
							0, y, 0, 0, fore->w, h,
							0, 0xff
						);
					}
				}
				break;
			case LIBAROMA_WINDOW_SHOW_ANIMATION_SLIDE_BOTTOM:
			case LIBAROMA_WINDOW_SHOW_ANIMATION_PAGE_BOTTOM:
				{
					float swift_out_state = libaroma_cubic_bezier_swiftout(state);
					int y = 0 - (fore->h - (swift_out_state * fore->h));
					int h = fore->h + y;
					if (reverse){
						y= 0 - (swift_out_state * fore->h);
						h=fore->h + y;
					}
					if (h>0){
						if (animation==LIBAROMA_WINDOW_SHOW_ANIMATION_SLIDE_BOTTOM){
							if (h<fore->h){
								libaroma_draw_ex(
									tempcv,
									back,
									0, h, 0, 0, fore->w, fore->h-h,
									0, 0xff
								);
							}
						}
						libaroma_draw_ex(
							tempcv,
							fore,
							0, 0, 0, fore->h-h, fore->w, h,
							0, 0xff
						);
					}
				}
				break;
			case LIBAROMA_WINDOW_SHOW_ANIMATION_SLIDE_RIGHT:
			case LIBAROMA_WINDOW_SHOW_ANIMATION_PAGE_RIGHT:
				{
					float swift_out_state = libaroma_cubic_bezier_swiftout(state);
					int x = 0 - (fore->w - (swift_out_state * fore->w));
					int w = fore->w + x;
					if (reverse){
						x=-(swift_out_state * fore->w);
						w=fore->w + x;
					}
					if (w>0){
						if (animation==LIBAROMA_WINDOW_SHOW_ANIMATION_SLIDE_RIGHT){
							if (w<fore->w){
								libaroma_draw_ex(
									tempcv,
									back,
									w, 0, 0, 0, fore->w - w, fore->h,
									0, 0xff
								);
							}
						}
						libaroma_draw_ex(
							tempcv,
							fore,
							0, 0, fore->w-w, 0, w, fore->h,
							0, 0xff
						);
					}
				}
				break;
			default:
				start=0;
				break;
		}
		libaroma_ctl_image_set_canvas(imgctl, tempcv, 1); //update shown image
		if (libaroma_wm()->active_window!=win) 
			libaroma_wm_set_active_window(win); //show the window with the image control (actually containing the first animation frame)
	}
	libaroma_canvas_free(tempcv);
	printf("animate: end\n");
	return;
}

void animate_window_transition(LIBAROMA_WINDOWP source, LIBAROMA_WINDOWP target, char *animation, char *anim_direction, byte reverse){
	if (source==NULL && target==NULL) return;
	byte anim=0;
	printf("animate: getting anim type from settings\n");
	if (animation==NULL) anim=LIBAROMA_WINDOW_SHOW_ANIMATION_SCALE;
	else if (0==strcmp(animation, "fade"))
		anim=LIBAROMA_WINDOW_SHOW_ANIMATION_FADE;
	else if (0==strcmp(animation, "slide")){
		if (anim_direction==NULL) anim=LIBAROMA_WINDOW_SHOW_ANIMATION_SLIDE_TOP;
		else if (0==strcmp(anim_direction, "left"))
			anim=LIBAROMA_WINDOW_SHOW_ANIMATION_SLIDE_LEFT;
		else if (0==strcmp(anim_direction, "right"))
			anim=LIBAROMA_WINDOW_SHOW_ANIMATION_SLIDE_RIGHT;
		else if (0==strcmp(anim_direction, "bottom"))
			anim=LIBAROMA_WINDOW_SHOW_ANIMATION_SLIDE_BOTTOM;
		else anim=LIBAROMA_WINDOW_SHOW_ANIMATION_SLIDE_TOP; //default
	}
	else if (0==strcmp(animation, "page")){
		if (anim_direction==NULL) anim=LIBAROMA_WINDOW_SHOW_ANIMATION_PAGE_TOP;
		else if (0==strcmp(anim_direction, "left"))
			anim=LIBAROMA_WINDOW_SHOW_ANIMATION_PAGE_LEFT;
		else if (0==strcmp(anim_direction, "right"))
			anim=LIBAROMA_WINDOW_SHOW_ANIMATION_PAGE_RIGHT;
		else if (0==strcmp(anim_direction, "bottom"))
			anim=LIBAROMA_WINDOW_SHOW_ANIMATION_PAGE_BOTTOM;
		else anim=LIBAROMA_WINDOW_SHOW_ANIMATION_PAGE_TOP; //default
	}
	printf("Creating drawable window!\n");
	byte src_hassb=0;
	byte trg_hassb=0;
	if (!source) src_hassb=1;
	if (!target) trg_hassb=1;
	LIBAROMA_WINDOWP drawwin=libaroma_window(NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
	LIBAROMA_CANVASP srccv=libaroma_canvas(source?source->w:target->prev_screen->w, source?source->h:(target->prev_screen->h-(src_hassb?STATUSBAR_HEIGHT:0)));
	LIBAROMA_CANVASP trgcv=libaroma_canvas(target?target->w:source->prev_screen->w, target?target->h:(source->prev_screen->h-(trg_hassb?STATUSBAR_HEIGHT:0)));
	LIBAROMA_CONTROLP targetimg=libaroma_ctl_image_canvas(drawwin, 1, srccv, 0, 0, drawwin->w, drawwin->h);
	libaroma_draw_ex(srccv, source?source->dc:target->prev_screen, 0, 0, 0, /* save source canvas */
											src_hassb?STATUSBAR_HEIGHT:0, srccv->w, srccv->h, 0, 0xFF);	
	//libaroma_png_save(srccv, "/tmp/srccv.png");
	if (!target) { /* closing source window */
		printf("Cloning source canvas!\n");
		libaroma_draw_ex(trgcv, source->prev_screen, 0, 0, 0, trg_hassb?STATUSBAR_HEIGHT:0, 
												trgcv->w, trgcv->h, 0, 0xFF);
		//libaroma_png_save(trgcv, "/tmp/trgcv.png");
		libaroma_wm_set_active_window(drawwin); //show the window with the image control (actually containing an empty canvas)
		printf("Delay #1\n");
	}
	else { /* switching to target window */
		//libaroma_sleep(500);
		printf("Drawing target canvas into drawable target!\n");
		target->lock_sync = 1;
		libaroma_wm_set_active_window(target);
		target->active=2;
		libaroma_window_invalidate(target, 10);
		libaroma_draw_ex(trgcv, target->dc, 0, 0, 0,trg_hassb?STATUSBAR_HEIGHT:0,
												trgcv->w, trgcv->h, 0, 0xFF);
		target->lock_sync = 0;
		target->active=0;
		printf("Showing drawable window!\n");
		//libaroma_wm_set_active_window(drawwin); //show the window with the image control (actually containing an empty canvas)
	}
	printf("Updating drawable window\n");
	long time_start=libaroma_tick();
	printf("Starting timer\n");
	animate(targetimg, reverse?trgcv:srccv, reverse?srccv:trgcv, anim, settings()->anim_speed, reverse);
	long time_taken=libaroma_tick()-time_start;
	printf("Time taken: %ld\n", time_taken);
	printf("Showing target window!\n");
	libaroma_wm_set_active_window(target);

	printf("Freeing drawable %swindow \n", source?"and source ":"");
	if (source) libaroma_window_free(source);
	//if (!source) libaroma_window_free(target);
	libaroma_window_free(drawwin);
	//libaroma_canvas_free(drawcv);
}