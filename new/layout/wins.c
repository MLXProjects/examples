#ifndef __layout_wins_c__
#define __layout_wins_c__
#include "common.h"

LAYOUT_WINLIST _layout_wins={0};

byte layout_wins_add(LIBAROMA_WINDOWP win, char *id){
	if (!win && !id){
		LOGI("invalid window");
		return 0;
	}
	_layout_wins.wins[_layout_wins.winn]=win;
	_layout_wins.ids[_layout_wins.winn]=id;
	_layout_wins.winn++;
	return 1;
}

byte layout_wins_delindex(int index, byte free){
	if (index<0) return 0;
	int i;
	for (i=index; i<_layout_wins.winn; i++){ /* shift stack */
		if (free) libaroma_window_free(_layout_wins.wins[i]);
		if (i<_layout_wins.winn-1){
			_layout_wins.wins[i]=_layout_wins.wins[i+1];
			_layout_wins.ids[i]=_layout_wins.ids[i+1];
		}
		else {
			_layout_wins.wins[i]=NULL;
			_layout_wins.ids[i]=NULL;
		}
	}
	_layout_wins.wins[_layout_wins.winn]=NULL;
	_layout_wins.ids[_layout_wins.winn]=NULL;
	_layout_wins.winn--;
	return 1;
}

LIBAROMA_WINDOWP layout_wins_get(char *id){
	if (_layout_wins.winn<1){
		LOGI("empty list");
		return NULL;
	}
	int i;
	for (i=_layout_wins.winn; i>0; i--){
		if (_layout_wins.ids[i-1]!=NULL)
			if (strcmp(id, _layout_wins.ids[i])!=0)
				return _layout_wins.wins[i];
	}
	LOGI("window \"%s\" not found", id);
	return NULL;
}

int layout_wins_getindex(LIBAROMA_WINDOWP win, char *id){
	if (_layout_wins.winn<1){
		LOGI("empty list");
		return NULL;
	}
	if (win==NULL && id==NULL) return -1;
	int i;
	for (i=_layout_wins.winn; i>0; i--){
		if (_layout_wins.ids[i-1]!=NULL && id!=NULL){
			if (strcmp(id, _layout_wins.ids[i-1])!=0)
				return i;
		}
		else if (win==_layout_wins.wins[i-1]) return i-1;
	}
	LOGI("window not found");
	return -1;
}

#endif /*__layout_wins_c__*/