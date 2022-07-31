#ifndef __ptero_misc_c__
#define __ptero_misc_c__
#ifdef __cplusplus
extern "C" {
#endif
#include "common.h"

LIBAROMA_ZIP global_zip;

/*
 * Function		: ptero_stream_uri_cb
 * Parameters	: uri string
 * Return type	: LIBAROMA_STREAMP
 * Description	: parse uri string
 */
LIBAROMA_STREAMP ptero_stream_uri_cb(char * uri) {
	int n = strlen(uri);
	char kwd[11];
	int i;
	for (i = 0; i < n && i < 10; i++) {
		kwd[i] = uri[i];
		kwd[i + 1] = 0;
		if ((i > 1) && (uri[i] == '/') && (uri[i - 1] == '/')) {
			break;
		}
	}
	/* resource stream */
	if (strcmp(kwd, "res://") == 0) {
		LIBAROMA_STREAMP ret=libaroma_stream_mzip(global_zip, uri + 7);
		if (ret){
			/* change uri */
			snprintf(ret->uri,
					 LIBAROMA_STREAM_URI_LENGTH,
			"%s", uri
			);
			return ret;
		}
	}
	return NULL;
}

/*
 * Function		: ptero_find_ctl_by_id
 * Parameters	: window, id
 * Return type	: LIBAROMA_CONTROLP
 * Description	: find control by id - use string to emulate id from XML
 */
LIBAROMA_CONTROLP ptero_find_ctl_by_id(LIBAROMA_WINDOWP win, char *id) {
	if (!win) return NULL;
	int i;
	for (i=0; i<win->childn; i++){
		LIBAROMA_CONTROLP ctl = win->childs[i];
		if (!ctl) continue;
		if (ctl->tag){
			PTERO_CONTROLP extra = (PTERO_CONTROLP) ctl->tag;
			if (0==strcmp(extra->id, id)) return ctl;
		}
	}
	return NULL;
}

#ifdef __cplusplus
}
#endif
#endif /* __ptero_misc_c__ */
