#include <aroma.h>
#define ID_HOME		1
#define ID_ADDTASK	2
#define ID_EDITTASK	3

typedef struct {
	char title[96];
	char desc[256];
	byte done;
	
} MLX_TASK, * MLX_TASKP;

LIBAROMA_ZIP zip;
LIBAROMA_WINDOWP win, home_win, addtask_win, edittask_win;
LIBAROMA_CONTROLP bar, fragment, /*pager, tabs, */todo_list;
LIBAROMA_CTL_BAR_TOOLSP bar_tools;

void mlx_gopage(int id){
	LIBAROMA_CTL_BAR_TOOLSP tools;
	char *title;
	int icon_id;
	switch (id){
		case ID_HOME:{
			tools=bar_tools;
			title="To-Do";
			icon_id=0;
		} break;
		case ID_ADDTASK:{
			tools=NULL;
			title="Add task";
			icon_id=LIBAROMA_CTL_BAR_ICON_ARROW;
		} break;
		case ID_EDITTASK:{
			tools=NULL;
			LIBAROMA_CTL_LIST_ITEMP item = libaroma_ctl_list_get_touched_item(todo_list);
			MLX_TASKP task = (MLX_TASKP) item->tag;
			title=task->title;
			icon_id=LIBAROMA_CTL_BAR_ICON_ARROW;
		} break;
	}
	libaroma_ctl_bar_set_tools(bar, tools, 0);
	libaroma_ctl_bar_set_title(bar, title, 0);
	libaroma_ctl_bar_set_icon(bar, NULL, 0, icon_id, 0);
	libaroma_ctl_bar_update(bar);
	libaroma_ctl_fragment_set_active_window(fragment, id, 0, 300, 0, 0, 0, 0);	
}

void mlx_list_fill(){
	char task_txt[32];
	int i;
	for (i=1; i<9; i++){
		MLX_TASKP task = calloc(sizeof(MLX_TASK), 1);
		if (!task) return;
		snprintf(task->title, 96, "Task #%i", i);
		snprintf(task->desc, 256, "To be done");
		LIBAROMA_CTL_LIST_ITEMP item = libaroma_listitem_check(todo_list, i, 0,
										task->title, task->desc, NULL, 
										LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_CHECK_HAS_SUBMENU, -1);
		item->tag = (void*) task;
	}
}

void winmain(){
	win = libaroma_window(0,0,0,0,0);
	if (!win) return;
	LIBAROMA_CANVASP icon = libaroma_image_mzip(zip, "add.svg", 0);
	if (icon) libaroma_canvas_fillcolor(icon, RGB(FFFFFF));
	bar = libaroma_ctl_bar(win, 1, 0, 0, 0, 56, "To-do", RGB(773333), RGB(FFFFFF));
	if (!bar) goto exit;
	bar_tools = libaroma_ctl_bar_tools(1);
	libaroma_ctl_bar_tools_set(bar_tools, 0, 1, NULL, icon, LIBAROMA_CTL_BAR_TOOL_ICON_SHARED);
	libaroma_ctl_bar_set_tools(bar, bar_tools, 1);
	fragment = libaroma_ctl_fragment(win, 2, 0, 56, 0, 0);
	if (!fragment) goto exit;
	home_win = libaroma_ctl_fragment_new_window(fragment, ID_HOME);
	libaroma_ctl_fragment_set_active_window(fragment, ID_HOME, 0, 0, 0, 0, 0, 0);
	addtask_win = libaroma_ctl_fragment_new_window(fragment, ID_ADDTASK);
	edittask_win = libaroma_ctl_fragment_new_window(fragment, ID_EDITTASK);
	/*pager = libaroma_ctl_pager(win, 2, );
	if (!pager) goto exit;
	tabs = libaroma_ctl_tabs(win, 3, 0, 0);
	if (!tabs) goto exit;*/
	todo_list = libaroma_ctl_list(home_win, 4, 0, 0, 0, 0, 0, 0, RGB(FFFFFF), LIBAROMA_CTL_LIST_WITH_SHADOW);
	if (!todo_list) goto exit;
	mlx_list_fill();
	libaroma_window_show(win);
	win->onpool=1;
	LIBAROMA_MSG msg={0};
	dword command;
	byte cmd;
	word id;
	byte param;
	do {
		command	= libaroma_window_pool(win,&msg);
		cmd		= LIBAROMA_CMD(command);
		id		= LIBAROMA_CMD_ID(command);
		param	= LIBAROMA_CMD_PARAM(command);
		if (msg.msg==LIBAROMA_MSG_EXIT){
			break;
		}
		else if (cmd == LIBAROMA_CMD_CLICK){
			if (id==1){
				printf("appbar param=%i\n", param);
				switch (param){
					case 10:{
						printf("MLX: appbar add\n");
						mlx_gopage(ID_ADDTASK);
					} break;
					case 1:{
						printf("MLX: appbar back\n");
						mlx_gopage(ID_HOME);
					} break;
				}
			}
			else if (id==4){
				LIBAROMA_CTL_LIST_ITEMP item = libaroma_ctl_list_get_touched_item(todo_list);
				printf("Task selected");
				if (item && item->tag!=NULL){
					MLX_TASKP task = (MLX_TASKP) item->tag;
					printf("Info:\n"
							"Title: %s\n"
							"Desc: %s\n"
							"Done: %s\n",
							task->title, task->desc, (task->done)?"yeah":"nope");
					mlx_gopage(ID_EDITTASK);
				}
			}
		}
	} while (win->onpool);
exit:
	if (icon) libaroma_canvas_free(icon);
	libaroma_window_free(win);
}

int main(int argc, char **argv){
	zip = libaroma_zip(argv[(argc>3)?3:1]);
	if (!zip) return 1;
	if (libaroma_start()){
		if (libaroma_font(0, libaroma_stream_mzip(zip, "roboto.ttf"))){
			winmain();
			libaroma_end();
		}
	}
	libaroma_zip_release(zip);
	return 0;
}