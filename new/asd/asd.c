#include "asd.h"
LIBAROMA_WINDOWP win, layer;
LIBAROMA_CONTROLP ctl;

void winmain(){
	win = libaroma_window(0, 0, 0, 0, 0);
	if (!win) return;
	ctl = libaroma_ctl_progress(win, 0, (win->width/2)-24, (win->height/2)-24, 48, 48, LIBAROMA_CTL_PROGRESS_INDETERMINATE|LIBAROMA_CTL_PROGRESS_CIRCULAR, 4, 1);
	if (!ctl) {libaroma_window_free(win); return;}
	if (!mylayermgr_init(win)){
		loge("layermgr init failed");
		layer=NULL;
	}
	else {
		logi("layermgr init done");
		layer = mylayer_new(win);
		if (!layer) {
			loge("layer create failed");
		}
		else {
			logi("layer create done");
		}
	}
	libaroma_window_anishow(win, 12, 500);
	LIBAROMA_MSG msg;
	dword command;
	byte cmd;
	word id;
	byte param;
	win->onpool=1;
	byte vislayer=0;
	do {
		command	= libaroma_window_pool(win,&msg);
		cmd		= LIBAROMA_CMD(command);
		id		= LIBAROMA_CMD_ID(command);
		param	= LIBAROMA_CMD_PARAM(command);
		
		if (msg.msg==LIBAROMA_MSG_EXIT){
			win->onpool=0;
		}
		else if (msg.msg==LIBAROMA_MSG_TOUCH && msg.state==0){
			vislayer=!vislayer;
			logi("should toggle visible layer");
			if (layer) mylayer_hideshow(layer, vislayer);
		}
	} while(win->onpool);
	mylayermgr_free(win);
	libaroma_window_aniclose(win, 12, 500);
}