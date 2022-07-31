#ifndef __bootmenu_filemgr_c__
#define __bootmenu_filemgr_c__
#ifdef __cplusplus
extern "C" {
#endif
	
#include "common.h"
#include <libgen.h>
/*
typedef struct {
	char *path;
	char *name;
} BOOTMENU_FILE, * BOOTMENU_FILEP;
*/
static BOOTMENU_FM _bootmenu_fm={0};

byte _bootmenu_fm_bar_touchable=0;

BOOTMENU_FMP bootmenu_fm(){
	return &_bootmenu_fm;
}

#define bootmenu_fm_basename(path) bootmenu_fm_basename_ex(path, 1)
char *bootmenu_fm_basename_ex(char *path, byte skip_last_slash) {
	if (!path) return NULL;
	char *token;
	char *ret=NULL;
	char *rest = path;
	while ((token = strtok_r(rest, "/", &rest))){
		if (skip_last_slash && (token+1=='\0')) break;
		ret=token;
	}
	return ret;
}

byte bootmenu_filemgr_init(word bgcolor){
	LIBAROMA_CANVASP arrow=libaroma_canvas_ex(libaroma_dp(48), libaroma_dp(48), 1);
	if (!arrow) return 0;
	libaroma_art_arrowdrawer(arrow, 1.0, 0, 0, 0, arrow->w, 0, 0xFF, 1, 0.5);
	if (libaroma_color_isdark(winmain->appbar_bg)) libaroma_canvas_fillcolor(arrow, RGB(FFFFFF));
	_bootmenu_fm.up_arrow=libaroma_canvas_rotate(arrow, 90);
	libaroma_canvas_free(arrow);
	if (!_bootmenu_fm.up_arrow) return 0;
	_bootmenu_fm.bar_tools=libaroma_ctl_bar_tools(1);
	if (!_bootmenu_fm.bar_tools) return 0;
	libaroma_ctl_bar_tools_set(_bootmenu_fm.bar_tools, 0, ID_APPBAR_UP, NULL, _bootmenu_fm.up_arrow, LIBAROMA_CTL_BAR_TOOL_ICON_SHARED);
	_bootmenu_fm.folder_cv=libaroma_image_uri("res:///icons/folder.png");
	if (!_bootmenu_fm.folder_cv) return 0;
	_bootmenu_fm.file_cv=libaroma_image_uri("res:///icons/file.png");
	if (!_bootmenu_fm.file_cv) return 0;
	_bootmenu_fm.bgcolor=bgcolor;
	_bootmenu_fm.onmain_list=1;
	return 1;
}

byte bootmenu_filemgr_goup(){
	if (_bootmenu_fm.cwd==NULL) return 0;
	if (!strcmp(_bootmenu_fm.cwd, "/")){
		printf("Hey, where did you want to get?\n");
		return 0;
	}
	/* TODO: check if malloc+cut is even needed, taking into accout basename's behavior */
	printf("going one level up from %s :D\n", _bootmenu_fm.cwd);
	char *curdir_str = strdup(_bootmenu_fm.cwd);
	char *curdir_name = bootmenu_fm_basename_ex(curdir_str, 0);
	if (!curdir_name) return 0;
	printf("curdir_name is %s, str is %s\n", curdir_name, curdir_str);
	int path_len=(strlen(_bootmenu_fm.cwd)-strlen(curdir_name));
	printf("path_len is %d ((%d-%d))\n", path_len, strlen(_bootmenu_fm.cwd), strlen(curdir_name));
	free(curdir_str);
	if (path_len<1) return 0;
	char *target_path=malloc(path_len+1);
	if (target_path==NULL) return 0;
	snprintf(target_path, path_len, "%s", _bootmenu_fm.cwd); /* cut current directory name from new path */
	bootmenu_filemgr_loadpath(target_path);
	return 1;
}

byte bootmenu_ramdisk_tryload(char *path){
	pid_t process;
	process = fork();
	if (process < 0){
		// fork() failed.
		printf("fork failed\n");
		return 2;
	}
	if (process == 0){
		int cmd_len=strlen(path) + 38;
		char *full_cmd = malloc(cmd_len+1);
		snprintf(full_cmd, cmd_len, "cd /new_root && zcat %s | cpio -idmv", path);
		char *args[] = { "/sbin/sh", "-c", full_cmd, 0};
		// sub-process
		execve(args[0], &args[0], NULL);
		free(full_cmd);
		printf("execv failed\n"); // Ne need to check execve() return value. If it returns, you know it failed.
		return 0;
	}
	int status;
	pid_t wait_result;
	
	while ((wait_result = wait(&status)) != -1){
		//printf("Process %lu returned result: %d\n", (unsigned long) wait_result, status);
		return status?status:1;
	}
}

void bootmenu_filelist_onclick(LIBAROMA_CONTROLP list){	
	LIBAROMA_CTL_LIST_ITEMP item = libaroma_ctl_list_get_touched_item(list);
	if (!item) return;
	
	/* handle item type */
		printf("MLX: going to check item path\n");
		if (item->tag!=NULL){ //check for symlink
		char *link_path = (char *)item->tag;
		printf("MLX: item path is %s\n", link_path);
		if (!strcmp(link_path, "..")){ //go one level up
			bootmenu_filemgr_goup();
			return;
		}
		if (!libaroma_file_exists(link_path)) return; /* dir/file not found? */
		DIR* trydir = opendir(link_path);
		if(trydir != NULL){ /* dir */
			closedir(trydir);
			bootmenu_filemgr_loadpath(link_path);
		}
		else { /* file */
			if (libaroma_dialog_confirm("Load ramdisk?", link_path, "OK", "Cancel", libaroma_colorget(NULL, winmain), LIBAROMA_DIALOG_DIM_PARENT|LIBAROMA_DIALOG_WITH_SHADOW|LIBAROMA_DIALOG_CANCELABLE)==1){
				/*int rdpath_fd = open("/tmp/rd.path", O_CREAT|O_WRONLY|O_TRUNC, S_IRWXG|S_IRWXO|S_IRWXU);
				if (rdpath_fd==-1){
					printf("Failed to create file %s\n", link_path);
					return;
				}
				write(rdpath_fd, link_path, strlen(link_path)+1);
				close(rdpath_fd);*/
				if (bootmenu_ramdisk_tryload(link_path)==1);{
					winmain->onpool=0; /* trigger libaroma close */
				}
			}
		}
	}
}

byte bootmenu_filemgr_loadpath(char *path){
	byte target_id=(bootmenu_fm()->onmain_list)?ID_FILEMFRAG:ID_FILESFRAG;
	printf("filemgr_loadpath %s going to use id %d\n", path, target_id);
	/* check if can go up */
	if (!strcmp(path, "/")){
		libaroma_ctl_bar_set_tools(appbar, NULL, 0);
		_bootmenu_fm.at_root=1;
		/*if (_bootmenu_fm_bar_touchable){
			_bootmenu_fm_bar_touchable=0;
			libaroma_ctl_bar_set_touchable_title(appbar, 0);
		}*/
	}
	else {
		_bootmenu_fm.at_root=0;
		libaroma_ctl_bar_set_tools(appbar, _bootmenu_fm.bar_tools, 0);
		/*if (!_bootmenu_fm_bar_touchable){
			libaroma_ctl_bar_set_touchable_title(appbar, 1);
			_bootmenu_fm_bar_touchable=1;
		}*/
	}
	//printf("filemgr_loadpath at_root=%d\n", _bootmenu_fm.at_root);
	LIBAROMA_WINDOWP filefrag=(target_id==ID_FILEMFRAG)?filemfrag:filesfrag;
	LIBAROMA_CONTROLP oldlist, newlist;
	/* replace old list with new */
	oldlist=libaroma_window_getid(filefrag, ID_FILELIST);
	if (oldlist) libaroma_window_del(filefrag, oldlist);
	newlist=libaroma_ctl_list(filefrag, ID_FILELIST, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL, 0, 0, _bootmenu_fm.bgcolor, LIBAROMA_CTL_LIST_WITH_SHADOW);
	if (!newlist){
		filelist=NULL;
		return 0;
	}
	libaroma_control_set_onclick(newlist, &bootmenu_filelist_onclick);
	filelist=newlist;
	/* free previous strings */
	printf("loadpath Freeing previous cwd %s, base %s and dir %s\n", _bootmenu_fm.cwd, _bootmenu_fm.base, _bootmenu_fm.dir);
	printf("loadpath cwd %p, base %p and dir %p\n", _bootmenu_fm.cwd, _bootmenu_fm.base, _bootmenu_fm.dir);
	if (_bootmenu_fm.cwd!=NULL) {
		printf("freeing cwd\n");
		free(_bootmenu_fm.cwd);
		if (_bootmenu_fm.base==_bootmenu_fm.cwd)
			_bootmenu_fm.base=NULL;
		_bootmenu_fm.cwd=NULL;
		printf("freed cwd\n");
	}
	if (_bootmenu_fm.base!=NULL) {
		printf("freeing base\n");
		free(_bootmenu_fm.base);
		_bootmenu_fm.base=NULL;
		printf("freed base\n");
	}
	if (_bootmenu_fm.dir!=NULL) {
		printf("freeing dir\n");
		free(_bootmenu_fm.dir);
		_bootmenu_fm.dir=NULL;
		printf("freed dir\n");
	}
	_bootmenu_fm.cwd = path;
	if (_bootmenu_fm.at_root){
		_bootmenu_fm.base=path;
		_bootmenu_fm.dir=NULL;
	}
	else {
	/* set basename for title */
		_bootmenu_fm.base= strdup(path);
		//printf("loadpath bpath %s new base %s and dir %s\n", path, _bootmenu_fm.base, _bootmenu_fm.dir);
	}
	/* populate list with items */
	aui_fetch(path, filelist);
	/* ignore icon update if not coming from home */
	byte iconflag=(libaroma_ctl_fragment_get_active_window_id(fragctl)==ID_HOMEFRAG)?LIBAROMA_CTL_BAR_ICON_DRAWER_TO_ARROW:0;
	/* show target fragment */
	libaroma_ctl_fragment_set_active_window(fragctl, target_id, 0, 300, 0, NULL, NULL, NULL);
	char *dpath=NULL;
	if (!_bootmenu_fm.at_root){
		_bootmenu_fm.dir=strdup(path);
		dpath=_bootmenu_fm.dir;
		dpath=dirname(dpath); /* dirname over second pointer to prevent main pointer modification (thanks weird dirname()) */
	}
	/* update appbar */
	bootmenu_bar_update(
						(_bootmenu_fm.at_root)?_bootmenu_fm.base:bootmenu_fm_basename(_bootmenu_fm.base),
						dpath, iconflag);
	/* switch target list */
	bootmenu_fm()->onmain_list=!(bootmenu_fm()->onmain_list);
	
	return 1;
}

byte bootmenu_filemgr_exit(){
	printf("FREEING FILE MANAGER\n");
	libaroma_ctl_bar_set_tools(appbar, NULL, 0);
	libaroma_ctl_bar_tools_free(_bootmenu_fm.bar_tools);
	libaroma_canvas_free(_bootmenu_fm.up_arrow);
	libaroma_canvas_free(_bootmenu_fm.folder_cv);
	libaroma_canvas_free(_bootmenu_fm.file_cv);
}

#ifdef __cplusplus
}
#endif
#endif /* __bootmenu_filemgr_c__ */  
