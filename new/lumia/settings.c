#ifndef __aromafm_settings_c__
#define __aromafm_settings_c__
#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

/* special key ids */
#define MLX_KB_SHIFT		1
#define MLX_KB_NUMPAD		2
//#define MLX_KB_		

LIBAROMA_CANVASP text_cv;
LIBAROMA_CTL_LIST_ITEMP kbd_in;
LIBAROMA_CONTROLP shift_btn;
char *text_buffer;
int buf_cnt;
int cur_buf;

byte kb_visible=0;
byte caps_lock=1;
byte kb_onsyms=0;
long cl_tick=0;

const char *kb_alphabet_nocaps[] = {
	"q", "w", "e", "r", "t", "y", "u", "i", "o", "p",
	"a", "s", "d", "f", "g", "h", "j", "k", "l", "ñ", 
	"z", "x", "c", "v", "b", "n", "m"
};

const char *kb_alphabet_caps[] = {
	"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",
	"A", "S", "D", "F", "G", "H", "J", "K", "L", "Ñ", 
	"Z", "X", "C", "V", "B", "N", "M"
};

const char *kb_symbols_default[] = {
	"1", "2", "3", "4", "5", "6", "7", "8", "9", "0", 
	"@", "#", "$", "_", "&", "-", "+", "(", ")", "/",
	"*", "\"", "\'" ":", ";", "!", "?"
};

const char *kb_symbols_more[] = {
	"1", "2", "3", "4", "5", "6", "7", "8", "9", "0", 
	"£", "¢", "€	", "¥", "^", "º", "=", "{", "}", "\\",
	"%", "©", "®" "™", "?", "[", "]",
	"<", " ", ">"
};

void mlx_kb_togglecaps(){
	if (kb_onsyms) return; /* don't allow caps toggle on symbols input mode */
	//printf("MLX: toggle caps\n");
	caps_lock=!caps_lock;
	if (cl_tick) cl_tick=0;
	LIBAROMA_CONTROLP shift_btn = libaroma_window_getid(settfrag, MLX_KB_SHIFT);
	if (caps_lock){
		libaroma_ctl_button_text(shift_btn, "<u>sh");
	}
	else libaroma_ctl_button_text(shift_btn, "sh");
	int i;
	for (i=0; i<kbfrag->childn; i++){
		LIBAROMA_CONTROLP key=kbfrag->childs[i];
		if (!key) return;
		if (key->id>64 && key->id<123 && (key->id<91 || key->id>96)){
			char str[2];
			str[0]=caps_lock?key->id-32:key->id+32;
			str[1]='\0';
			key->id=str[0];
			libaroma_ctl_button_text(key, str);
		}
	}
}

void mlx_kb_togglesyms(){
	if (!kb_onsyms) return; /* don't allow syms toggle on letter input mode */
	if (kb_onsyms==1) kb_onsyms=2;
	else kb_onsyms=1;
	LIBAROMA_CONTROLP shift_btn = libaroma_window_getid(settfrag, MLX_KB_SHIFT);
	/* update shift key */
	if (kb_onsyms==2){
		libaroma_ctl_button_text(shift_btn, "<u>=<");
	}
	else libaroma_ctl_button_text(shift_btn, "=<");
	int i;
	for (i=0; i<kbfrag->childn; i++){
		LIBAROMA_CONTROLP key=kbfrag->childs[i];
		if (!key) return;
		if (key->id>32 && key->id<41 && (key->id<91 || key->id>96)){
			char str[2];
			str[0]=caps_lock?key->id-32:key->id+32;
			str[1]='\0';
			key->id=str[0];
			libaroma_ctl_button_text(key, str);
		}
	}
}

void mlx_kb_togglemode(){
	if (!kb_onsyms) kb_onsyms=2;
	else kb_onsyms=0;
	mlx_kb_togglesyms();
}

void mlx_text_draw(char c){
	if (c==8){ /* backspace, delete last char */
		if (cur_buf<1) return;
		cur_buf--;
		text_buffer[cur_buf]='\0';
	}
	else { /* add char */
		if (cur_buf+1==buf_cnt){
			buf_cnt+=64;
			char *tmp=realloc(text_buffer, buf_cnt);
			if (!tmp) return;
			text_buffer=tmp;
		}
		text_buffer[cur_buf]=c;
		cur_buf+=1;
		text_buffer[cur_buf]='\0'; /* this NULL gets overwritten on next char draw */
	}
	LIBAROMA_TEXT txt = libaroma_text(text_buffer, RGB(FFFFFF), text_cv->w, LIBAROMA_FONT(1,4), 0);
	int txth=libaroma_text_height(txt);
	printf("MLX: check for need canvas resize texth=%d cv->h=%d\n", txth, text_cv->h);
	if (txth>text_cv->h) libaroma_canvas_resize(text_cv, text_cv->w, txth);
	if (txth>settlist->h){
		/* scroll to last size */
		printf("MLX: need to scroll to %d\n", txth-settlist->h);
		libaroma_ctl_scroll_request_pos(settlist, txth-settlist->h);
	}
	printf("text_cv clear canvas\n");
	libaroma_canvas_setcolor(text_cv, RGB(0), 0xFF);
	printf("text_cv draw text\n");
	libaroma_text_draw(text_cv, txt, 0, 0);
	printf("text_cv free text\n");
	libaroma_text_free(txt);
	printf("text_cv update image\n");
	libaroma_listitem_image_set(kbd_in, text_cv);
	printf("text_cv image updated\n");
}

void mlx_kb_mgr(LIBAROMA_CONTROLP btn){
	switch (btn->id){
		case MLX_KB_SHIFT:{
			if (caps_lock && cl_tick!=0){
				if (libaroma_tick()-cl_tick<500){
					//printf("should keep caps locked\n");
					caps_lock=2;
				}
				//printf("resetting timer\n");
				cl_tick=0;
			}
			else {
				if (kb_onsyms) mlx_kb_togglesyms();
				else mlx_kb_togglecaps();
				cl_tick=libaroma_tick();
			}
		} break;
		case MLX_KB_NUMPAD:{
			mlx_kb_togglemode();
		} break;
		default:{
			//printf("key %c (%d)\n", btn->id, btn->id);
			mlx_text_draw(btn->id);
			if (btn->id==10){ /* auto enable caps for first new line char */
				if (!caps_lock) {
					mlx_kb_togglecaps();
				}
			}
			else if (caps_lock==1 && (btn->id==' ' || (btn->id>64 && btn->id<91))){ /* disable caps after input if key is letter */
				mlx_kb_togglecaps();
			}
		} break;
	}
}

void mlx_item_click(LIBAROMA_CONTROLP list){
	kb_visible=!kb_visible;
	//printf("MLX: toggle kb=%d\n", kb_visible);
	libaroma_control_resize(settlist, 0, 0, LIBAROMA_SIZE_FULL, (kb_visible)?LIBAROMA_SIZE_HALF:LIBAROMA_SIZE_FULL);
	libaroma_control_resize(kbfctl, 0, (kb_visible)?LIBAROMA_POS_HALF:(winmain->height), LIBAROMA_SIZE_FULL, (kb_visible)?LIBAROMA_SIZE_HALF:1);
}

int x=0, y=0, w=0, h=0;
LIBAROMA_CONTROLP mlx_add_keybtn(char c, char *nice_key){
	if (x+w>winmain->width){
		x=0;
		y+=h;
	}
	char str[2];
	str[0]=c;
	str[1]='\0';
	LIBAROMA_CONTROLP key=libaroma_ctl_button(kbfrag, c, x, y, w, h, (nice_key==NULL)?str:nice_key, LIBAROMA_CTL_BUTTON_RAISED, 0);
	x+=w;
	libaroma_control_set_onclick(key, &mlx_kb_mgr);
	return key;
}

byte mlx_settings_init(word bgcolor){
	kb_visible=1;
	settlist = libaroma_ctl_list(settfrag, ID_SETTFRAG, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_HALF, 0, 0, bgcolor, LIBAROMA_CTL_LIST_WITH_SHADOW);
	if (!settlist) return 0;
	libaroma_control_set_onclick(settlist, &mlx_item_click);
	//libaroma_listitem_menu(settlist, 1, "Toggle kb", NULL, NULL, LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_MENU_SMALL, -1);
	text_cv = libaroma_canvas(winmain->w, winmain->h);
	//libaroma_canvas_setcolor(text_cv, RGB(0), 0xFF);
	buf_cnt=64;
	text_buffer=malloc(buf_cnt);
	cur_buf=0;
	text_buffer[0]='\0';
	kbd_in = libaroma_listitem_image(settlist, 2, text_cv, text_cv->h, LIBAROMA_CTL_LIST_ITEM_REGISTER_THREAD|LIBAROMA_LISTITEM_IMAGE_SHARED, -1);
	kbfctl = libaroma_ctl_fragment(settfrag, ID_KBFCTL, 0, LIBAROMA_POS_HALF, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_HALF);
	if (!kbfctl) return 0;
	kbfrag = libaroma_ctl_fragment_new_window(kbfctl, ID_KBFRAG);
	int def_w=winmain->width/10;
	w=def_w;
	x=0;
	h=w*1.45;
	mlx_add_keybtn('Q', NULL);
	mlx_add_keybtn('W', NULL);
	mlx_add_keybtn('E', NULL);
	mlx_add_keybtn('R', NULL);
	mlx_add_keybtn('T', NULL);
	mlx_add_keybtn('Y', NULL);
	mlx_add_keybtn('U', NULL);
	mlx_add_keybtn('I', NULL);
	mlx_add_keybtn('O', NULL);
	mlx_add_keybtn('P', NULL);
	y+=h; /* next row */
	while (x>(w*2)) /* get padding */
		x-=w;
	x/=4;
	mlx_add_keybtn('A', NULL);
	mlx_add_keybtn('S', NULL);
	mlx_add_keybtn('D', NULL);
	mlx_add_keybtn('F', NULL);
	mlx_add_keybtn('G', NULL);
	mlx_add_keybtn('H', NULL);
	mlx_add_keybtn('J', NULL);
	mlx_add_keybtn('K', NULL);
	mlx_add_keybtn('L', NULL);
	//mlx_add_keybtn('Ñ', NULL);
	x=0;
	y+=h;
	int special_w=winmain->width;
	while (special_w>(w*3))
		special_w-=w;
	special_w/=2;
	w=special_w;
	mlx_add_keybtn(MLX_KB_SHIFT, "<u>sh");
	w=def_w;
	mlx_add_keybtn('Z', NULL);
	mlx_add_keybtn('X', NULL);
	mlx_add_keybtn('C', NULL);
	mlx_add_keybtn('V', NULL);
	mlx_add_keybtn('B', NULL);
	mlx_add_keybtn('N', NULL);
	mlx_add_keybtn('M', NULL);
	w=special_w;
	mlx_add_keybtn(8, "<--");
	y+=h;
	int space_w=winmain->width/2,
		space_x=(winmain->width-space_w)/2;
	x=0;
	w=space_x-def_w;
	mlx_add_keybtn(MLX_KB_NUMPAD, "123");
	w=def_w;
	mlx_add_keybtn(',', NULL);
	/*x=space_x;*/
	w=space_w;
	mlx_add_keybtn(' ', "____");
	w=def_w;
	mlx_add_keybtn('.', NULL);
	w=winmain->w-x;
	mlx_add_keybtn(10, "\\n");
	libaroma_ctl_fragment_set_active_window(kbfctl, ID_KBFRAG, 0, 0, 0, NULL, NULL, NULL);
	return 1;
}

#ifdef __cplusplus
}
#endif
#endif /* __aromafm_settings_c__ */  
  
 
