#include "layer.h"

#define _mylayermgr_check(win, ret) MYLAYER_MANAGERP layermgr = _mylayermgr_get(win); \
								if (!layermgr){ \
									loge("window not layered"); \
									return ret; \
								}

#define _mylayer_check(win, ret) MYLAYERP layer = _mylayer_get(win); \
								if (!layer){ \
									loge("window is not a layer"); \
									return ret; \
								}
/* layer manager handler callbacks */
byte _mylayermgr_msghook(LIBAROMA_WINDOWP win, LIBAROMA_MSGP msg, dwordp val){
	if (msg->msg==LIBAROMA_MSG_WIN_RESIZE){
		/* resize  */
	}
	logi("called %s", __func__);
	return 0;
}

/* layer default handler callbacks */
void _mylayer_prefree(LIBAROMA_WINDOWP win){
	logi("called %s", __func__);
}
void _mylayer_postfree(LIBAROMA_WINDOWP win){
	logi("called %s", __func__);
}
byte _mylayer_sync(LIBAROMA_WINDOWP win,int x,int y,int w,int h){
	logi("called %s", __func__);
	libaroma_wm_sync(x, y, w, h);
	return 0;
}
byte _mylayer_ctl_drawflush(LIBAROMA_WINDOWP win,LIBAROMA_CONTROLP ctl,LIBAROMA_CANVASP cv,byte idk){
	logi("called %s", __func__);
	return 0;
}
byte _mylayer_ctl_erasebg(LIBAROMA_WINDOWP win,LIBAROMA_CONTROLP ctl,LIBAROMA_CANVASP cv){
	logi("called %s", __func__);
	return 0;
}
byte _mylayer_ctl_isvisible(LIBAROMA_WINDOWP win,LIBAROMA_CONTROLP ctl){
	logi("called %s", __func__);
	return 0;
}
LIBAROMA_CANVASP _mylayer_ctl_drawbegin(LIBAROMA_WINDOWP win,LIBAROMA_CONTROLP ctl){
	logi("called %s", __func__);
	return 0;
}

/*
 * Variable		: _mylayermgr_handler
 * Descriptions: layer manager handler
 */
static LIBAROMA_WINDOW_HANDLER _mylayermgr_handler={
	prefree:NULL,
	postfree:NULL,
	updatebg:NULL,
	invalidate:NULL,
	sync:NULL,
	message_hooker:_mylayermgr_msghook,
	control_draw_flush:NULL,
	control_erasebg:NULL,
	control_isvisible:NULL,
	control_draw_begin:NULL
};

/*
 * Variable		: _mylayer_defhandler
 * Descriptions: layer window handler
 */
static LIBAROMA_WINDOW_HANDLER _mylayer_defhandler={
	prefree:_mylayer_prefree,
	postfree:_mylayer_postfree,
	updatebg:NULL,
	invalidate:NULL,
	sync:_mylayer_sync,
	message_hooker:NULL,
	control_draw_flush:_mylayer_ctl_drawflush,
	control_erasebg:_mylayer_ctl_erasebg,
	control_isvisible:_mylayer_ctl_isvisible,
	control_draw_begin:_mylayer_ctl_drawbegin
};

/*
 * Function		: _mylayermgr_get
 * Return Value: MYLAYER_MANAGERP
 * Descriptions: get layer manager from window
 */
static inline MYLAYER_MANAGERP _mylayermgr_get(LIBAROMA_WINDOWP win){
	if (!win){
		return NULL;
	}
	if (win->parent!=NULL){
		return NULL;
	}
	if (win->handler!=&_mylayermgr_handler){
		return NULL;
	}
	return (MYLAYER_MANAGERP) win->client_data;
} /* End of _libaroma_window_layer_check */

/*
 * Function		: _mylayer_get
 * Return Value: MYLAYERP
 * Descriptions: get layer from window
 */
static inline MYLAYER_MANAGERP _mylayer_get(LIBAROMA_WINDOWP win){
	if (!win){
		loge("layer window NULL");
		return NULL;
	}
	if (win->parent==NULL){
		loge("layer window parent NULL");
		return NULL;
	}
	if (win->handler!=&_mylayer_defhandler){
		loge("layer window defhandler not match");
		return NULL;
	}
	return (MYLAYERP) win->client_data;
} /* End of _libaroma_window_layer_check */

/*
 * Function		: _mylayermgr_ui_thread
 * Return Value: byte
 * Descriptions: layer manager ui thead
 */
byte _mylayermgr_ui_thread(LIBAROMA_WINDOWP win){
	_mylayermgr_check(win,0);
	byte needsync=0;
	libaroma_mutex_lock(layermgr->mutex);
	if (layermgr->ori_ui_thread!=NULL) needsync=layermgr->ori_ui_thread(win);
	logi("layermgr ui_thread here");
	int i;
	MYLAYERP layer;
	LIBAROMA_WINDOWP laywin;
	for (i=0; i<layermgr->layern; i++){
		layer=layermgr->layers[i];
		laywin=layer->win;
		if (layer->isvisible){
			logi("rendering layer #%d", i);
			if (laywin->ui_thread(laywin)) needsync=1;
		}
	}
	libaroma_mutex_unlock(layermgr->mutex);
	return needsync;
}

/*
 * Function		: _mylayer_defui_thread
 * Return Value: byte
 * Descriptions: layer default ui thread
 */
byte _mylayer_defui_thread(LIBAROMA_WINDOWP win){
	logi("rendering layer");
	MYLAYERP layer = _mylayer_get(win);
	libaroma_mutex_lock(layer->mutex);
	LIBAROMA_WINDOWP parent = win->parent;
	_mylayermgr_check(parent, 0);
	libaroma_draw(layermgr->wmc, parent->dc, 0, 0, 0);
	libaroma_draw_ex(layermgr->wmc, win->dc, 0, 0, 0, 0, win->w, win->h, 0, 0x55);
	libaroma_wm_updatesync(0, 0, 0, 0, 1);
	libaroma_mutex_unlock(layer->mutex);
	return 1;
}

/*
 * Function		: mylayer_init
 * Return Value: void
 * Descriptions: initialize layer manager
 */
byte mylayermgr_init(LIBAROMA_WINDOWP win){
	if (!win){
		loge("invalid window");
		return 0;
	}
	if (win->parent){
		loge("cannot init child window as layered");
		return 0;
	}
	if (win->client_data){
		if (win->handler!=&_mylayermgr_handler){
			loge("window handler already in use");
			return 0;
		}
		return 1;
	}
	MYLAYER_MANAGERP layermgr =
		(MYLAYER_MANAGERP) calloc(sizeof(MYLAYER_MANAGER),1);
	if (!layermgr){
		loge("cannot alloc layermgr data");
		return 0;
	}
	libaroma_mutex_init(layermgr->mutex);
	libaroma_mutex_lock(layermgr->mutex);
	layermgr->win = win;
	layermgr->direct_cv=1;
	layermgr->tdc=libaroma_canvas(win->w, win->h);
	layermgr->ori_ui_thread=win->ui_thread;
	win->handler = &_mylayermgr_handler;
	win->client_data = (void*) layermgr;
	win->ui_thread=_mylayermgr_ui_thread;
	libaroma_mutex_unlock(layermgr->mutex);
	return 1;
}

/*
 * Function		: mylayer_new
 * Return Value: void
 * Descriptions: create layer
 */
LIBAROMA_WINDOWP mylayer_new(LIBAROMA_WINDOWP win){
	_mylayermgr_check(win,NULL);
	MYLAYERP layer = calloc(sizeof(MYLAYER), 1);
	if (!layer){
		loge("mylayer_new window layer alloc failed");
		return NULL;
	}
	LIBAROMA_WINDOWP laywin = (LIBAROMA_WINDOWP)
		calloc(sizeof(LIBAROMA_WINDOW),1);
	if (!win){
		libaroma_mutex_unlock(layermgr->mutex);
		ALOGW("mylayer_new alloc layer data failed");
		return NULL;
	}
	laywin->w=win->w;
	laywin->h=win->h;
	laywin->client_data=layer;
	laywin->handler=&_mylayer_defhandler;
	laywin->parent=win;
	laywin->bg=libaroma_canvas(win->w, win->h);
	laywin->dc=libaroma_canvas(win->w, win->h);
	laywin->ui_thread=_mylayer_defui_thread;
	libaroma_mutex_lock(layermgr->mutex);
	MYLAYERP *layerlist = realloc(layermgr->layers, sizeof(MYLAYERP)*(layermgr->layern+1));
	if (!layerlist){
		loge("mylayer_new realloc layer list failed");
		free(laywin);
		free(layer);
		return NULL;
	}
	libaroma_mutex_init(layer->mutex);
	layer->win=laywin;
	layerlist[layermgr->layern]=layer;
	layermgr->layers=layerlist;
	layermgr->layern+=1;
	libaroma_mutex_unlock(layermgr->mutex);
	return laywin;
}

/*
 * Function		: mylayer_del
 * Return Value: void
 * Descriptions: delete layer
 */
void mylayer_del(LIBAROMA_WINDOWP laywin){
	_mylayer_check(laywin,);
	MYLAYER_MANAGERP layermgr = _mylayermgr_get(laywin->parent);
	libaroma_mutex_lock(layermgr->mutex);
	int i;
	/* find layer */
	for (i=0; i<layermgr->layern; i++){
		if (layermgr->layers[i]==layer){
			/* decrease layer count and save i */
			layermgr->layern--;
			break;
		}
	}
	/* shift all next layers */ 
	for (; i<layermgr->layern; i++){
		layermgr->layers[i] = layermgr->layers[i+1];
	}
	MYLAYERP *layerlist = realloc(layermgr->layers, sizeof(MYLAYERP)*(layermgr->layern-1));
	if (!layerlist && layermgr->layern!=0){
		/* things will still work, but this is a memory leak */
		loge("mylayer_del layerlist realloc failed");
	}
	libaroma_mutex_free(layer->mutex);
	if (laywin->bg) libaroma_canvas_free(laywin->bg);
	if (laywin->dc) libaroma_canvas_free(laywin->dc);
	free(layer->win);
	if (layerlist) layermgr->layers=layerlist;
	libaroma_mutex_unlock(layermgr->mutex);
	return;
}

/*
 * Function		: mylayermgr_free
 * Return Value: void
 * Descriptions: release layered window data
 */
void mylayermgr_free(LIBAROMA_WINDOWP win){
	_mylayermgr_check(win,);
	libaroma_mutex_lock(layermgr->mutex);
	int i;
	for (i=0; i<layermgr->layern; i++){
		mylayer_del(layermgr->layers[i]->win);
	}
	free(layermgr->layers);
	layermgr->layers=NULL;
	layermgr->layern=0;
	//TODO: send free message to all layers & process it - not sure if possible
	win->ui_thread=layermgr->ori_ui_thread;
	libaroma_mutex_unlock(layermgr->mutex);
	libaroma_mutex_free(layermgr->mutex);
	win->client_data=NULL;
	win->handler=NULL;
	free(layermgr);
	return;
}

byte mylayer_hideshow(LIBAROMA_WINDOWP win, byte show){
	_mylayer_check(win, 0);
	_mylayermgr_check(win->parent, 0);
	logi("locking layer");
	libaroma_mutex_lock(layer->mutex);
	logi("layer locked");
	layer->isvisible=show;
	if (show){
		/* swap wm canvas and temp dc */
		layermgr->direct_cv=0;
		layermgr->wmc=win->parent->dc;
		win->parent->dc=layermgr->tdc;
		/* copy current window dc to temp canvas - mantains background */
		libaroma_draw(layermgr->tdc, layermgr->wmc, 0, 0, 0);
	}
	else {
		/* swap temp dc and wm canvas */
		layermgr->direct_cv=1;
		layermgr->tdc=win->parent->dc;
		win->parent->dc=layermgr->wmc;
		libaroma_draw(layermgr->wmc, layermgr->tdc, 0, 0, 0);
	}
	libaroma_mutex_unlock(layer->mutex);
	logi("layer unlocked");
	return 1;
}