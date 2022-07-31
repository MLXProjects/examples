#ifndef __aromafm_c__
#define __aromafm_c__
#ifdef __cplusplus
extern "C" {
#endif
	
#include "common.h"

static AROMAFM _aromafm={0};

AROMAFMP aromafm(){
	return &_aromafm;
}

byte aromafm_atroot(){
	return _aromafm.at_root;
}

byte aromafm_goup(){
	if (_aromafm.cwd==NULL) return 0;
	if (!strcmp(_aromafm.cwd, "/")){
		printf("Hey, where did you want to get?\n");
		return 0;
	}
	if (aromafm()->selecting){
		if (libaroma_dialog_confirm("Selecting items", "Cancel item selection?", "Yes", "No", libaroma_colorget(NULL, winmain), LIBAROMA_DIALOG_DIM_PARENT|LIBAROMA_DIALOG_WITH_SHADOW|LIBAROMA_DIALOG_CANCELABLE)==2)
			return 1; /* continue in select mode */
		else aromafm_statusbar_reset();
	}
	
	/* TODO: check if malloc+cut is even needed, taking into accout basename's behavior */
	printf("going one level up from %s :D\n", _aromafm.cwd);
	char *curdir_str = strdup(_aromafm.cwd);
	char *curdir_name = aromafm_basename_ex(curdir_str, 0);
	if (!curdir_name) return 0;
	printf("curdir_name is %s, str is %s\n", curdir_name, curdir_str);
	int path_len=(strlen(_aromafm.cwd)-strlen(curdir_name));
	printf("path_len is %d ((%d-%d))\n", path_len, strlen(_aromafm.cwd), strlen(curdir_name));
	free(curdir_str);
	if (path_len<1) return 0;
	char *target_path=malloc(path_len+1);
	if (target_path==NULL) return 0;
	snprintf(target_path, path_len, "%s", _aromafm.cwd); /* cut current directory name from new path */
	aromafm_loadpath(target_path);
	return 1;
}

unsigned long aromafm_gettype(char *text){
	printf("MLX: gettype for file %s\n", text);
	if (!text) return 0;
	char *ext = strrchr(text, '.');
	printf("MLX: checking extension %s\n", ext);
	if (!ext) return 0;
	if (ext+1=='\0') return 0;
	unsigned long hash = libaroma_hash(ext+1);
	printf("MLX: return type is %lu\n", hash);
	return hash;
}

/*
byte aromafm_file_open(char *path){
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
		return status?status:1;
	}
}*/

byte aromafm_pathlist_add(char *path);

byte aromafm_loadpath(char *path){
	byte target_id=(aromafm()->onmain_list)?ID_FILESFRAG:ID_FILEMFRAG;
	/* switch target list */
	printf("loadpath switching target list\n");
	aromafm()->onmain_list=!(aromafm()->onmain_list);
	printf("filemgr_loadpath %s going to use id %d\n", path, target_id);
	/* check if can go up */
	#ifdef _WIN32
	if (!strcmp(path, "C:")){
	#else
	if (!strcmp(path, "/")){
	#endif
		if (!_aromafm.at_root) /* reset tools only if previous dir wasn't rootdir */
			libaroma_ctl_bar_set_tools(appbar, NULL, 0);
		_aromafm.at_root=1;
	}
	else {
		if (_aromafm.at_root) /* set tools only if coming from rootdir */
			libaroma_ctl_bar_set_tools(appbar, _aromafm.bar_tools, 0);
		_aromafm.at_root=0;
	}
	//printf("filemgr_loadpath at_root=%d\n", _aromafm.at_root);
	LIBAROMA_WINDOWP filefrag=(target_id==ID_FILEMFRAG)?filemfrag:filesfrag;
	LIBAROMA_CONTROLP oldlist, newlist;
	/* replace old list with new */
	oldlist=libaroma_window_getid(filefrag, ID_FILELIST);
	if (oldlist) libaroma_window_del(filefrag, oldlist);
	newlist=libaroma_ctl_list(filefrag, ID_FILELIST, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL, 0, 0, _aromafm.bgcolor, LIBAROMA_CTL_LIST_WITH_SHADOW|LIBAROMA_CTL_LIST_WITH_HANDLE);
	if (!newlist){
		filelist=NULL;
		return 0;
	}
	libaroma_control_set_onclick(newlist, &aromafm_list_onclick);
	filelist=newlist;
	/* free previous strings */
	printf("loadpath Freeing previous cwd %s, base %s and dir %s\n", _aromafm.cwd, _aromafm.base, _aromafm.dir);
	printf("loadpath cwd %p, base %p and dir %p\n", _aromafm.cwd, _aromafm.base, _aromafm.dir);
	if (_aromafm.cwd!=NULL) {
		printf("freeing cwd\n");
		free(_aromafm.cwd);
		if (_aromafm.base==_aromafm.cwd)
			_aromafm.base=NULL;
		_aromafm.cwd=NULL;
		printf("freed cwd\n");
	}
	if (_aromafm.base!=NULL) {
		printf("freeing base\n");
		free(_aromafm.base);
		_aromafm.base=NULL;
		printf("freed base\n");
	}
	if (_aromafm.dir!=NULL) {
		printf("freeing dir\n");
		free(_aromafm.dir);
		_aromafm.dir=NULL;
		printf("freed dir\n");
	}
	_aromafm.cwd = path;
	if (_aromafm.at_root){
		_aromafm.base=path;
		_aromafm.dir=NULL;
	}
	else {
	/* set basename for title */
		_aromafm.base= strdup(path);
		//printf("loadpath bpath %s new base %s and dir %s\n", path, _aromafm.base, _aromafm.dir);
	}
	/* populate list with items */
	aui_fetch(path, filelist);
	/* show target fragment */
	libaroma_ctl_fragment_set_active_window(fragctl, target_id, 0, 300, 0, NULL, NULL, NULL);
	printf("loadpath switching fragment\n");
	char *dpath=NULL;
	if (!_aromafm.at_root){
		printf("loadpath setting new path\n");
		_aromafm.dir=strdup(path);
		/*dpath=*/dirname(_aromafm.dir);
		//if (strcmp(dpath, "/")) /* for some reason, dirname breaks if passing / */
			//dpath=dirname(dpath); /* dirname over second pointer to prevent main pointer modification (thanks weird dirname()) */
	}
	printf("loadpath updating appbar\n");
	/* update appbar */
	aromafm_bar_update((_aromafm.at_root)?_aromafm.base:aromafm_basename(_aromafm.base), _aromafm.dir, 0);
	
	return 1;
}

#ifdef __cplusplus
}
#endif
#endif /* __aromafm_c__ */  
