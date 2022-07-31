#ifndef __br_tools_c__
#define __br_tools_c__

#include "main.h"

byte fraganim_ran=0;

voidp _br_history_find_id(int id, byte ret_page){
	if (!br_history) return NULL;
	LIBAROMA_STACK_ITEMP item;
	for (item=br_history->last; item; item=item->prev){
		BR_PAGEP page = (BR_PAGEP)(item->val);
		if (page->id==id){
			return (voidp)(ret_page?page:item);
		}
	}
	printf("br_history_find cannot find page %d\n", id);
	return NULL;
}

byte _br_history_add(BR_PAGEP page){
	if (!br_history || !page) return 0;
	libaroma_stack_add_at(br_history, br_history->n, (voidp)page, sizeof(BR_PAGE));
	return 1;
}

byte _br_history_del(int id){
	if (!br_history) return 0;
	LIBAROMA_STACK_ITEMP item = (LIBAROMA_STACK_ITEMP) _br_history_find_id(id, 0);
	if (!item) return 0;
	libaroma_stack_item_delete(br_history, item);
	return 1;
}

byte _br_history_switch(BR_PAGEP page){
	if (!br_history) return 0;
	if (br_history->n<1){ /* add first page and return */
		return _br_history_add(page);
	}
	if (page->id==br_history_last()->id) {
		printf("br_history_switch cannot reactivate active page\n");
		return 0;
	}
	
	/* get page to set as last (current) */
	LIBAROMA_STACK_ITEMP item = (LIBAROMA_STACK_ITEMP) _br_history_find_id(page->id, 0);
	if (!item) { /* add page to history - leave previous open */
		return _br_history_add(page);
	}
	else { /* close all forward pages as we're going back */
		LIBAROMA_STACK_ITEMP last = br_history->last;
		if (!last) return 0;
		BR_PAGEP last_page = (BR_PAGEP) last->val;
		while (last_page){ /* delete last page until reaching target */
			if (last_page->id == page->id) return 1; /* reached target */
			BR_PAGEP old_last = (BR_PAGEP) libaroma_stack_pop(br_history);
			free(old_last);
			last = br_history->last;
			last_page = (BR_PAGEP) last->val;
		}
		return 0;
		/*
		int i=0;
		for (last = br_history->last; last; last=last->prev){
			i++;
			BR_PAGEP last_page = (BR_PAGEP) last->val;	
			//if target page found, return
			if (last_page->id==page->id) return 1;
			if (last_page->onexit) last_page->onexit(); // call closing page onexit callback
			//libaroma_stack_item_delete(br_history, last);
			libaroma_stack_pop(br_history);
			free(last_page); //free struct clone made by libaroma_stack_add_at
		}*/
	}
	return 0;
}

/*
void br_switch_window(int id, void *pool, void *onexit, LIBAROMA_TRANSITION_CB anim, LIBAROMA_RECTP rc1, LIBAROMA_RECTP rc2,
						char *title, char *subtitle){
	fraganim_ran=0; // TODO: put this variable on BR_MAINP struct 
	//if (br()->poolexit) br()->poolexit();
	main_appbar_update(title, subtitle, (id==ID_FRAGMAIN)?1:0);
	libaroma_ctl_fragment_set_active_window(fragment, id, 1, settings()->anim_duration, 0, anim, rc1, rc2);
	br()->winpool = pool;
	br()->poolexit= onexit;
}
*/

BR_PAGEP _br_page_find(int id){
	BR_PAGEP page;
	int i;
	for (i=br()->pagen; i>0; i--){
		page = br()->pages[i-1];
		if (id==page->id){
			return page;
		}
	}
	return NULL;
}

// br_page find macro
#define _BR_PAGE(fname,id,ret) \
			BR_PAGEP page = _br_page_find(id); \
			if (!page){ \
				printf("%s: cannot find page %d\n", fname, id); \
				return ret; \
			}

byte br_page_new(int id, void *onstart, void *pool, void *onexit, void *onshow){
	if (_br_page_find(id)){
		printf("br_page_new: page already exists\n");
		return 0; 
	}
	BR_PAGEP page = malloc(sizeof(BR_PAGE));
	if (!page){
		printf("br_page_new: cannot allocate new page\n");
		return 0;
	}
	// initialize page info
	page->id=id;
	page->anim_cb=NULL;
	page->inrect =NULL;
	page->outrect=NULL;
	page->onstart=onstart;
	page->pool	 =pool;
	page->onexit =onexit;
	page->onshow =onshow;
	printf("br_page_new: created page #%d at %p, pool is at %p\n", id, page, page->pool);
	// realloc page array with storage for new page
	BR_PAGEP *pages = br()->pages;
	BR_PAGEP *tmp = realloc(pages, (sizeof(BR_PAGEP))*(br()->pagen+1));
	if (!tmp){
		printf("br_page_new: cannot realloc page list\n");
		free(page);
		return 0;
	}
	br()->pages=tmp;
	br()->pages[br()->pagen]=page;
	br()->pagen+=1;
	return 1;
}

byte br_page_switch(int id){
	_BR_PAGE("br_page_switch", id, 0);
	printf("switching to page #%d\n", page->id);
	if (!_br_history_switch(page)) return 0;
	if (page->onstart) page->onstart();
	if (page->onshow) page->onshow();
	libaroma_ctl_fragment_set_active_window(fragment, id, 1, settings()->anim_duration, 0, 
											page->anim_cb, page->inrect, page->outrect);
	return 1;
}

byte br_page_goback(){
	if (br_history->n<2) {
		printf("br_page_goback cannot go before a single page\n");
		return 0;
	}
	BR_PAGEP target = (BR_PAGEP)(br_history->last->prev->val);
	//printf("current page is #%d, going to #%d\n", br_history_last()->id, target->id);
	//BR_PAGEP item = (BR_PAGEP) libaroma_stack_get(br_history->n-2);
	return br_page_switch(target->id);
}

void *br_page_get_pdata(int id){
	_BR_PAGE("br_page_get_pdata", id, NULL);
	return page->pdata;
}

byte br_page_set_pdata(int id, void *pdata){
	_BR_PAGE("br_page_set_pdata", id, 0);
	page->pdata=pdata;
	return 1;
}

byte br_page_setanim(int id, void *anim_cb, LIBAROMA_RECTP in_rect, LIBAROMA_RECTP out_rect){
	_BR_PAGE("br_page_setanim", id, 0);
	page->anim_cb = anim_cb;
	page->inrect = in_rect;
	page->outrect = out_rect;
	return 1;
}

/*
byte br_page_del(int id){
	_BR_PAGE("del", id, 0);
	if (page==cur_page) //switch to previous window, if any, otherwise call br_exit
	free(page);
	return 1;
}*/
/*
// switch id at index to last pos or delete it 
byte _br_history_switchdel_index(int index, byte del){
	if ((index < 0) || (index > br_history()->pagen-1)){
		printf("_br_history_switchdel_index cannot go outside of array bounds\n");
		return 0;
	}
	BR_PAGEP page=br_history()->pages[index];
	int i;
	for (i=index; i<br_history()->pagen-1; i++){ // shift array to left of index 
		br_history()->pages[i]=br_history()->pages[i+1];
	}
	if (del){ // decrease array size and item count
		BR_PAGEP *pages= br_history()->pages;
		BR_PAGEP *tmp = realloc(pages, (sizeof(BR_PAGEP))*(br_history()->pagen-2));
		if (!tmp){
			printf("br_history_init: cannot realloc page list\n");
			return 0;
		}
		br_history()->pages=tmp;
		br_history()->pagen--;
		//br_history()->current=br_history_last());
	}
	else { // put id at last 
		br_history()->pages[br_history()->pagen-1] = page;
	}
	return 1;
}

int br_history_find(int id){
	int i;
	for (i=br_history()->pagen; i>0; i--){ // reverse search is faster if going to previous page
		BR_PAGEP page = br_history()->pages[i-1];
		if (page->id==id)
			return i-1;
	}
	return -1;
}

byte br_history_add(int id){
	// realloc id array with storage for new id 
	BR_PAGEP *pages = br_history()->pages;
	BR_PAGEP *tmp = realloc(pages, (sizeof(BR_PAGEP))*(br_history()->pagen+1));
	if (!tmp){
		printf("br_history_init: cannot realloc page list\n");
		return 0;
	}
	br_history()->pages=tmp;
	br_history()->pages[br_history()->pagen]=_br_page_find(id);
	br_history()->pagen++;
	return 1;
}

byte br_history_switch(int id){
	// find page index
	int idx=br_history_find(id);
	if (idx<0)
		return 0;
	return _br_history_switchdel_index(idx, 0); // switch to page 
}

byte br_history_del(int id){
	// find page index
	int idx=br_history_find(id);
	if (idx<0)
		return 0;
	_br_history_switchdel_index(idx, 1); // delete page
	return 1;
}

byte br_page_goback(){
	if (br_history()->pagen<2){
		printf("br_page_goback cannot go before index 0\n");
		return 0;
	}
	
	BR_PAGEP page = br_history()->pages[br_history()->pagen-2];
	if (!br_page_switch(page->id), 1) return 0;
	if (br_history_prev()!=br_history_first()) // if it's not home, delete previous page
		return br_history_del(br_history_prev());
	return 1;
}*/

word custcolor_handler(const char *tag){
	/*if (){
		
	}*/
	int taglen = strlen(tag);
	if (taglen==4 ||taglen==7)
		return libaroma_rgb_from_string(tag)/*RGB(FF0000)*/;
	return -1;
}

/* TODO: center target (this gives a precious reveal-like effect) */
void fragani_default(
	LIBAROMA_CANVASP dst,
	LIBAROMA_CANVASP bottom,
	LIBAROMA_CANVASP top,
	float state,
	LIBAROMA_RECTP r1,
	LIBAROMA_RECTP r2
){
	byte opening=1;
	if (r2){
		r1=r2;
		opening=0;
		LIBAROMA_CANVASP top2 = top;
		top = bottom;
		bottom=top2;
	}
	if (!r1){
		return;
	}
	state=libaroma_motion_fluid(state);
	if (state>1.0f) state=1.0f;
	int to_h, to_y;
	if (opening){
		to_y=(r1->y+r1->h)*(1-state);
		to_h=(dst->h*state);
	}
	else {
		to_y=((r1->y+r1->h)*state);
		to_h=(dst->h-(to_y))*(1-state);
	}
	
	if (opening && !fraganim_ran){
		libaroma_draw(bottom, dst, 0, 0, 0);
		fraganim_ran=1;
	}
	if (settings()->fragani_flags&FRAGANI_NOSLIDE) {
		libaroma_draw_ex(dst, bottom, 0, 0, 0, 0, dst->w, to_y, 0, 0xFF);
		libaroma_draw_ex(dst, bottom, 0, to_y+to_h, 0, dst->h-(to_y+to_h), dst->w, dst->h-(r1->y+r1->h), 0, 0xFF);
	}
	else {
		libaroma_draw_ex(dst, bottom, 0, 0, 0, (r1->y+r1->h)-to_y, dst->w, to_y/*(r1->y+r1->h)*/, 0, 0xFF);
		libaroma_draw_ex(dst, bottom, 0, to_y+to_h, 0, r1->y+r1->h/*to_y+to_h*/, dst->w, dst->h-(r1->y+r1->h), 0, 0xFF);
	}
	if (settings()->fragani_flags&FRAGANI_SCALE)
		libaroma_draw_scale_nearest(dst, top, 0, to_y, dst->w, to_h, 0, 0, top->w, top->h);
	else libaroma_draw_ex(dst, top, 0, to_y, 0, 0, dst->w, to_h, 0, 0xFF);
}

/* stream uri callback */
LIBAROMA_STREAMP stream_uri_cb(char * uri){
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
		LIBAROMA_STREAMP ret=libaroma_stream_mzip(zip, uri + 7);
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

#ifndef _WIN32
char *_find_xml_encoding(char *xml_file, size_t max_size){
	if (!xml_file || max_size<20) return NULL; //valid xml has more than 20 characters
	char *defstart, *defend, *lookhere, *ret;
	defstart=strstr(xml_file, "<?xml");
	if (defstart==NULL) {
		printf("Failed to find XML declaration tag\n");
		return NULL;
	}
	defend = strstr(defstart+4, "\?>");
	if (defend==NULL) {
		printf("Failed to find XML declaration closing\n");
		return NULL;
	}
	if (defstart+4==defend) {
		printf("XML declaration contains no data\n");
		return NULL;
	}
	int findlen=defend-defstart;
	lookhere=libaroma_stristr(defstart+4, "encoding=\"", findlen);
	if (lookhere==NULL) return NULL;
	int retlen=(strstr(lookhere+1, "\"")-lookhere);
	if (*(lookhere+1+(retlen/2))=='\"') retlen/=2; //if encoding uses two bytes for a char, adapt it to one
	ret=malloc(retlen+1);
	snprintf(ret, retlen+1, "%s", lookhere+10);
	printf("Encoding found (%d): %s\n", retlen, ret);
	return ret;
}
size_t _iso8859_1_to_utf8(char *content, size_t max_size){
    char *src, *dst;
    //first run to see if there's enough space for the new bytes
    for (src = dst = content; *src; src++, dst++)
    {
        if (*src & 0x80)
        {
            // If the high bit is set in the ISO-8859-1 representation, then
            // the UTF-8 representation requires two bytes (one more than usual).
            ++dst;
        }
    }
    size_t req_size=dst - content + 1;
    if (req_size > max_size)
    {
        printf("WARNING: expanding buffer from %ld to %ld\n", max_size, req_size);
        char *tmp = realloc(content, req_size);
        if (tmp==NULL) {
			printf("Failed to expand buffer\n");
			return -1;
		}
		content = tmp;
    }

    *(dst + 1) = '\0';
    while (dst > src)
    {
        if (*src & 0x80)
        {
            *dst-- = 0x80 | (*src & 0x3f);                     // trailing byte
            *dst-- = 0xc0 | (*((unsigned char *)src--) >> 6);  // leading byte
        }
        else
        {
            *dst-- = *src--;
        }
    }
    return (req_size<max_size)?max_size:req_size;  // return actual buffer size (either expanded or original)
}
#endif /* _WIN32 */

#endif /* __br_tools_c__ */