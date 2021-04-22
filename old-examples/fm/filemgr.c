void *threaded_function(void* win_controls);
void cd(char *dir);

static LIBAROMA_CONTROLP lst=NULL;
LIBAROMA_CANVASP file_icon=NULL;
LIBAROMA_CANVASP folder_icon=NULL;
LIBAROMA_CANVASP linkfile_icon=NULL;
LIBAROMA_CANVASP linkfolder_icon=NULL;
LIBAROMA_CANVASP linkbroken_icon=NULL;

#include "aromafm.c"
#define		ID_FMLIST			1

struct FM_INFO {
	LIBAROMA_CONTROLP list;
	char *path;
};

void list_clear(LIBAROMA_CONTROLP list){
	//printf("list_clear called\n");
	if (!aui_dir_active_path) return;
	
	/* 	TODO: on each cd() call, save the item count. 
		Using the method below, if some file/folder is created
		this may leave some items there (as they did exist before) */
		
	int count=aui_getdircount(aui_dir_active_path);
	if (!count && strcmp(aui_dir_active_path, "/")) count=1; // if not on root, ".." item should exist
	if (count < 1) return;
	//printf("Clearing list!\n");
	if (!libaroma_ctl_list_is_valid(list)) return;
	int i;
	fm_stop_enum=1;
	msleep(10);
	//printf("List has %d items! Clearing...\n", count);
	for (i=0; i<count; i++){
		libaroma_ctl_list_del_item_internal(list, 0, 0);
	}
	fm_stop_enum=0;
}

void filemgr(LIBAROMA_WINDOWP parent){
	#define		ID_BUTTON1		3
	#define		ID_BUTTON2		4

	//printf("Creating window!\n");
	LIBAROMA_WINDOWP win = libaroma_window(
		NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);

	lst=libaroma_ctl_list(win, ID_FMLIST, libaroma_dp(10), libaroma_dp(20), libaroma_px(win->w-libaroma_dp(20)), 
								libaroma_px(win->h-libaroma_dp(NAVBAR_HEIGHT)-libaroma_dp(30)), 0, 0, RGB(FFFFFF), 
								LIBAROMA_CTL_SCROLL_WITH_SHADOW|LIBAROMA_CTL_SCROLL_WITH_HANDLE);

	folder_icon = libaroma_image_uri("res:///icons/folder.png");
	linkfolder_icon = libaroma_image_uri("res:///icons/linkfolder.png");
	file_icon = libaroma_image_uri("res:///icons/file.png");
	linkfile_icon = libaroma_image_uri("res:///icons/linkfile.png");
	linkbroken_icon = libaroma_image_uri("res:///icons/linkbroken.png");
	
	navbar_draw(win);
	printf("Showing window!\n");
	cd("/");
	statusbar_end_thread();
	libaroma_window_anishow(win, LIBAROMA_WINDOW_SHOW_ANIMATION_SLIDE_TOP, 300);
	//statusbar_start_thread();
	printf("Window should be visible.\n");
	do{
		LIBAROMA_MSG msg;
		dword command=libaroma_window_pool(win,&msg);
		byte cmd	= LIBAROMA_CMD(command);
		word id		= LIBAROMA_CMD_ID(command);
		byte param	= LIBAROMA_CMD_PARAM(command);
		
		switch (msg.msg){
			case LIBAROMA_MSG_KEY_POWER:
			case LIBAROMA_MSG_KEY_SELECT:
				{
					if (msg.state==0){
						win->onpool=0;
					}
				}
				break;
		}
		
		if (cmd == 1){
			if (id==ID_NAVBACK){
				win->onpool=0;
			}
			else if (id==ID_BUTTON1){
				cd("/");
			}
			else if (id==ID_FMLIST){
				handle_item_select(lst);
			}
		}
	}
	while(win->onpool);
	fm_stop_enum=1;
	msleep(10);
	printf("Closing window!\n");
	statusbar_end_thread();
	closewin(win, parent, LIBAROMA_WINDOW_SHOW_ANIMATION_SLIDE_BOTTOM, 300);
	statusbar_start_thread();
}

void cd(char *dir){	
	printf("Files in folder: %d\n", aui_getdircount(dir));
	pthread_t lister_thread;
	printf("Allocating fm_info struct!\n");
	struct FM_INFO *fm_info = malloc(sizeof(struct FM_INFO));
	(*fm_info).list=lst;
	(*fm_info).path=dir;
	makethread(lister_thread, (void*) fm_info);
}

void handle_item_select(LIBAROMA_CONTROLP list){
	LIBAROMA_CTL_LIST_ITEMP touched_item;
	touched_item = libaroma_ctl_list_get_touched_item(list);
	if (!touched_item){
		printf("Item getting failed!\n");
		return;
	}
	struct FILE_INFO *file_info = (struct FILE_INFO*) touched_item->tag;
	char *type=malloc(32);
	switch (file_info->type){
		case 4:
			cd (file_info->name);
			strcpy(type, "folder");
			break;
		case 8:
			strcpy(type, "file");
			break;
		case 10:
			strcpy(type, "broken symlink");
			break;
		case 24:
			strcpy(type, "folder symlink");
			break;
		case 28:
			strcpy(type, "file symlink");
			break;
	}
	printf("%s: %s\n", file_info->name, type);	
}

void *threaded_function(void* fm_info){
	//printf("Getting info\n");
	struct FM_INFO *fm = (struct FM_INFO*) fm_info;
	//printf("Clearing list prior to fill it!\n");
	list_clear(lst);
	printf("Calling aui_fetch!\n");
	aui_fetch(fm->path, fm->list);
	printf("aui_fetch done\n");
}

void makethread(pthread_t thread, void *fm_info){
	//printf("MakeThread: creating thread!\n");
	if(pthread_create(&thread, NULL, threaded_function, fm_info) && !thread) {
		fprintf(stderr, "MakeThread: error creating thread...\n");
	}
	else fprintf(stderr, "MakeThread: thread created!\n");
}

void closethread(pthread_t thread){
	//printf("CloseThread: closing thread!\n");
	if(pthread_join(thread, NULL)) {
		fprintf(stderr, "CloseThread: error closing thread, attempting again...\n");
		if (pthread_cancel(thread)){
			fprintf(stderr, "CloseThread: error closing thread!\n");
		}
	}
}