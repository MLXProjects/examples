#ifndef __ptero_gui_c__
#define __ptero_gui_c__
#ifdef __cplusplus
extern "C" {
#endif
#include "common.h"

/*
 * Function		: ptero_gui_loop
 * Parameters	: none
 * Return type	: byte
 * Description	: loop to handle window events
 */
byte ptero_gui_loop(){
	LIBAROMA_WINDOWP win = pterodon()->winmain;
	libaroma_window_anishow(win, 12, 400);
	LIBAROMA_RECTP last_rect;
	LIBAROMA_MSG msg;
	dword command;
	byte cmd, param;
	word id;
	do {
		command	= libaroma_window_pool(win,&msg);
		cmd		= LIBAROMA_CMD(command);
		id		= LIBAROMA_CMD_ID(command);
		param	= LIBAROMA_CMD_PARAM(command);
		
		if (msg.msg == LIBAROMA_MSG_EXIT) {
			printf("MLX: close event received\n");
			pterodon()->running = 0;
			break;
		}
		else if (msg.msg == LIBAROMA_MSG_TOUCH){
			switch (msg.state){
				case LIBAROMA_HID_EV_STATE_DOWN:
					//libaroma_wm_cursor_setvisible(1);
					break;
				case LIBAROMA_HID_EV_STATE_UP:
					//libaroma_wm_cursor_setvisible(0);
					break;
			}
		}
	} while (pterodon()->running);
	libaroma_window_aniclose(win, 12, 400);
	
	return 1;
}

/*
 * Function		: ptero_gui_main
 * Parameters	: none
 * Return type	: byte
 * Description	: initialize & parse layout, enter main window loop
 */
byte ptero_gui_main(){
	if (!ptero_layout_parse(pterodon()->winmain, layout_xml->child, 0, 0)) {
		printf("MLX: %s\n", "failed to parse layout");
		return 0;
	}
	ptero_gui_loop();
	
	return 1;
}

void ptero_ctl_click(LIBAROMA_CONTROLP ctl) {
	printf("CONTROL CLICK CALLBACK CALLED :D\n");
	
	if (libaroma_ctl_list_is_valid(ctl)){ /* handle list item click */
		LIBAROMA_CTL_LIST_ITEMP item = libaroma_ctl_list_get_touched_item(ctl);
		if (item && item->tag) {
			printf("MLX: list item has tag, checking for callbacks\n");
			PTERO_CONTROLP extra = (PTERO_CONTROLP) item->tag;
			if (extra->onclick) {
				printf("MLX: click callback found! parsing\n");
				ptero_parse_actionset(extra->onclick);
			}
		}
	}
	else if (ctl->tag) { /* handle control click */
		printf("MLX: control has tag, checking for callbacks\n");
		PTERO_CONTROLP extra = (PTERO_CONTROLP) ctl->tag;
		if (extra->onclick) {
			printf("MLX: click callback found! parsing\n");
			ptero_parse_actionset(extra->onclick);
		}
	}
}

void ptero_ctl_hold(LIBAROMA_CONTROLP ctl) {
	printf("CONTROL HOLD CALLBACK CALLED :D\n");
	
	if (libaroma_ctl_list_is_valid(ctl)){ /* handle list item hold */
		LIBAROMA_CTL_LIST_ITEMP item = libaroma_ctl_list_get_touched_item(ctl);
		if (item && item->tag) {
			printf("MLX: list item has tag, checking for callbacks\n");
			PTERO_CONTROLP extra = (PTERO_CONTROLP) item->tag;
			if (extra->onhold) {
				printf("MLX: hold callback found! parsing\n");
				ptero_parse_actionset(extra->onhold);
			}
		}
	}
	else if (ctl->tag) { /* handle control hold */
		printf("MLX: control has tag, checking for callbacks\n");
		PTERO_CONTROLP extra = (PTERO_CONTROLP) ctl->tag;
		if (extra->onhold) {
			printf("MLX: hold callback found! parsing\n");
			ptero_parse_actionset(extra->onhold);
		}
	}
}

#ifdef __cplusplus
}
#endif
#endif /* __ptero_gui_c__ */ 
 
