#include "custitem.c"
void closewin(LIBAROMA_WINDOWP win, LIBAROMA_WINDOWP parent, byte anim, int duration){
	if (parent==NULL){
		LIBAROMA_WINDOWP closewin=libaroma_window(NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
		LIBAROMA_CANVASP closecv=libaroma_canvas(win->prev_screen->w, win->prev_screen->h-STATUSBAR_HEIGHT);
		libaroma_draw_ex(closecv, win->prev_screen, 0, 0, 0, STATUSBAR_HEIGHT, closecv->w, closecv->h, 0, 0xFF);
		libaroma_ctl_image_canvas(closewin, 999, closecv, 0, 0, closewin->w, closewin->h);
		libaroma_window_anishow(closewin, anim, duration);
		libaroma_window_free(closewin);	
	}
	else libaroma_window_anishow(parent, anim, duration);
	libaroma_window_free(win);
}