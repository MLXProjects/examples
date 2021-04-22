 #include <sys/types.h> 
 #include <sys/stat.h>

struct FILE_INFO { 
	char *name;
	byte type;
};

char * aui_dir_active_path = NULL;
byte fm_stop_enum = 0;

//*
//* Expand Path Size
//*
void aui_setpath(char ** buf, char * pre, char * post, byte addslash) {
	int prel = strlen(((pre == NULL) ? "" : pre));
	int postl = strlen(((post == NULL) ? "" : post));
	
	if ((prel + postl == 0) && (!addslash)) {
		return;
	}
	
	int len = prel + postl + 1 + addslash;
	char * newdir = malloc(len);
	snprintf(newdir, len, ((addslash) ? "%s%s/" : "%s%s"),
					 ((pre == NULL) ? "" : pre), ((post == NULL) ? "" : post));
					 
	if (*buf != NULL) {
		free(*buf);
	}
	
	*buf = newdir;
}

int aui_dtcheck(char * name, byte t, byte addsz) {
	if (t == 10) {
		char * buf = NULL;
		aui_setpath(&buf, aui_dir_active_path, name, 0);
		
		if (buf) {
			struct stat statbuf;
			
			if (!stat(buf, &statbuf)) {
				if (S_ISDIR(statbuf.st_mode)) {
					free(buf);
					return addsz + 4;
				}
				else {
					free(buf);
					return addsz + 8;
				}
			}
			
			free(buf);
		}
		
		return 10;
	}
	
	return t;
}

int aui_fsort(const struct dirent ** pa, const struct dirent ** pb) {
	struct dirent * a = (struct dirent *)*pa;
	struct dirent * b = (struct dirent *)*pb;
	byte adt = aui_dtcheck(a->d_name, a->d_type, 0);
	byte bdt = aui_dtcheck(b->d_name, b->d_type, 0);
	
	if (adt < bdt) {
		return 1;
	}
	else if (adt > bdt) {
		return -1;
	}
	
	return strcasecmp(b->d_name, a->d_name);
}

int aui_getdircount(char * path) {
	struct dirent ** files;
	int n = scandir(path, &files, 0, alphasort);
	int c = 0;
	
	if (n > 0) {
		while (n--) {
			char * dname = files[n]->d_name;
			/*	//commented out because I use this function to know how many items are on the list
			if (!strcmp(dname, ".") || !strcmp(dname, "..")) {
				continue;
			}*/
			c++;			
			free(files[n]);
		}		
		free(files);
	}
	
	return c;
}

void aui_bytesize(char * buf, int val) {
	if (val > 524288)
		snprintf(buf, 64, "%1.1f MB",
						 ((float)val) / 1048576);
	else if (val > 1024) {
		snprintf(buf, 64, "%i KB", round(val / 1024));
	}
	else {
		snprintf(buf, 64, "%i Bytes", val);
	}
}

void aui_filesize(char * buf, char * path) {
	struct stat fst;
	
	if (!stat(path, &fst)) {
		aui_bytesize(buf, fst.st_size);
	}
	else {
		snprintf(buf, 64, "0 Bytes");
	}
}

void aui_fileperm(char * buf, char * path) {
	struct stat fst;
	
	if (!stat(path, &fst)) {
		buf[8] = (fst.st_mode & S_IXOTH) ? 'x' : '-';
		buf[7] = (fst.st_mode & S_IWOTH) ? 'w' : '-';
		buf[6] = (fst.st_mode & S_IROTH) ? 'r' : '-';
		buf[5] = (fst.st_mode & S_IXGRP) ? 'x' : '-';
		buf[4] = (fst.st_mode & S_IWGRP) ? 'w' : '-';
		buf[3] = (fst.st_mode & S_IRGRP) ? 'r' : '-';
		buf[2] = (fst.st_mode & S_IXUSR) ? 'x' : '-';
		buf[1] = (fst.st_mode & S_IWUSR) ? 'w' : '-';
		buf[0] = (fst.st_mode & S_IRUSR) ? 'r' : '-';
		//-- SUID, GUID, STICKY
		buf[2] = (fst.st_mode & S_ISUID) ? ((buf[2] == 'x') ? 's' : 'S') : buf[2];
		buf[5] = (fst.st_mode & S_ISGID) ? ((buf[2] == 'x') ? 's' : 'S') : buf[5];
		buf[8] = (fst.st_mode & S_ISVTX) ? ((buf[2] == 'x') ? 't' : 'T') : buf[8];
		buf[9] = 0;
	}
	else {
		snprintf(buf, 10, "---------");
	}
}

byte aui_ishidden_file(const char * fn) {
	/*	useless
	if (strcmp(fn, "/dev") == 0) {
		return 1;
	}
	else if (strcmp(fn, "/proc") == 0) {
		return 1;
	}
	else if (strcmp(fn, "/sys") == 0) {
		return 1;
	}
	else if (strcmp(fn, "/acct") == 0) {
		return 1;
	}
	else if (strcmp(fn, "/vendor") == 0) {
		return 1;
	}*/
	
	return 0;
}

int aui_fetch(char * path, LIBAROMA_CONTROLP list) {
	//printf("aui_fetch started with path %s!\n", path);
	//printf("Allocating space for path char*\n");
	aui_dir_active_path = malloc((strlen(path)*sizeof(char))+1);
	//printf("Copying path into current path char*\n");
	aui_dir_active_path = path;
	//printf("Done\n");
	int count=aui_getdircount(aui_dir_active_path);
	//printf("Folder has %d items!\n", count);
	struct dirent ** files;
	int n = scandir(path, &files, 0, *aui_fsort);
	if (strcmp(aui_dir_active_path, "/")){
		LIBAROMA_CTL_LIST_ITEMP listitem = libaroma_listitem_image(list, 1, 
												custitem(folder_icon, "..", "Up a level", list->w, 
												libaroma_dp(54)), 55, LIBAROMA_LISTITEM_IMAGE_FILL
												|LIBAROMA_CTL_LIST_ITEM_RECEIVE_TOUCH, -1);	
		/* customization starts here */
		struct FILE_INFO *file_info = malloc(sizeof(struct FILE_INFO));
		(*file_info).name=malloc(255);
		strcpy((*file_info).name, realpath("..", NULL));
		(*file_info).type=4;
		listitem->tag = (void*) file_info;
	}
	
	if (n > 0) {
		while (n--) {
			if (fm_stop_enum) return;
			//printf("Getting info about file/folder %d\n", n);
			char * dname = files[n]->d_name;
			
			if (!strcmp(dname, ".") || !strcmp(dname, "..")) {
				free(files[n]);
				continue;
			}
			
			if ((dname[0] == '.') /*&& (auic()->showhidden == 0)*/) {
				free(files[n]);
				continue;
			}
			
			//-- GET TYPE
			//printf("Getting file/folder type\n");
			byte dtype = aui_dtcheck(dname, files[n]->d_type, 20);
			char * full_path = NULL;
			aui_setpath(&full_path, aui_dir_active_path, dname, 0);
			
			if (full_path) {
				//printf("Checking file v2, dtype is %d\n", dtype);
				if (/*auic()->showhidden == 0*/1) {
					if (aui_ishidden_file(full_path)) {
						free(full_path);
						free(files[n]);
						continue;
					}
				}
					
				//-- DIR
				if (dtype == 4) {
					int cnt = aui_getdircount(full_path);
					char desc[256];
					char perm[10];
					aui_fileperm(perm, full_path);
					char formats[256];
					char formats2[256];
					snprintf(formats, 256,
									 "%d ", (cnt>1)?cnt:1);
					snprintf(formats2, 256,
									 /*alang_get*/((cnt >
															1) ?
														 "files" :
														 "file"),
									 formats);
					snprintf(desc, 256, "%s",
									 formats2);/*
					afbox_add(FB, dname, desc, 0,
										&UI_ICONS[22], dtype, perm, 0,
										selectedDefault);*/
					//printf("Concatenating description with permissions!\n");
					char *perms=malloc(64);/*
					strcat(perm, ", ");
					strcat(perm, desc);*/
					sprintf(perms, "%s", perm);
					if (fm_stop_enum) { //without this check, the loop will keep adding items
						fm_stop_enum=0; //even if we changed the directory (and that would not be nice).
						free(full_path); 
						free(files[n]);
						free(files);
						return; 
					}
					LIBAROMA_CTL_LIST_ITEMP listitem = libaroma_listitem_image(list, 1, 
												custitem(folder_icon, dname, perms, list->w, 
												libaroma_dp(54)), 55, LIBAROMA_LISTITEM_IMAGE_FILL
												|LIBAROMA_CTL_LIST_ITEM_RECEIVE_TOUCH, -1);	
					//printf("Found folder %s with desc %s and perms %s\n", dname, desc, perm);
					/* customization starts here */
					struct FILE_INFO *file_info = malloc(sizeof(struct FILE_INFO));
					(*file_info).name=malloc(255);
					strcpy((*file_info).name, realpath(path, NULL));
					strcat((*file_info).name, dname);
					(*file_info).type=dtype;
					listitem->tag = (void*) file_info;
				}
				//-- FILE
				else if (dtype == 8) {
					char desc[256];
					char dsz[256];
					char perm[10];
					aui_fileperm(perm, full_path);
					aui_filesize(dsz, full_path);
					snprintf(desc, 256, "%s",
									 dsz);/*
					afbox_add(FB, dname, desc, 0,
										&UI_ICONS[21], dtype, perm, 0,
										selectedDefault);*/
										
					//printf("Concatenating description with permissions!\n");
					char *perms=malloc(64);/*
					strcat(perm, ", ");
					strcat(perm, desc);*/
					sprintf(perms, "%s", perm);
					if (fm_stop_enum) { 
						fm_stop_enum=0;
						free(full_path); 
						free(files[n]);
						free(files);
						return; 
					}
					LIBAROMA_CTL_LIST_ITEMP listitem = libaroma_listitem_image(list, 1, 
												custitem(file_icon, dname, perms, list->w, 
												libaroma_dp(54)), 55, LIBAROMA_LISTITEM_IMAGE_FILL
												|LIBAROMA_CTL_LIST_ITEM_RECEIVE_TOUCH, -1);	
					//printf("Found file %s with desc %s and perms %s\n", dname, desc, perm);
					/* customization starts here */
					struct FILE_INFO *file_info = malloc(sizeof(struct FILE_INFO));
					(*file_info).name=malloc(255);
					strcpy((*file_info).name, realpath(path, NULL));
					strcat((*file_info).name, dname);
					(*file_info).type=dtype;
					listitem->tag = (void*) file_info;
				}
				//-- LINK FILE / DIR
				else if ((dtype == 28) || (dtype == 24)) {
					char * desc = NULL;
					char buf[4096];
					int len =
						readlink(full_path, buf,
										 sizeof(buf));
										 
					if (len >= 0) {
						buf[len] = 0;
						/*
						desc =
							aui_strip(buf,
												agw() -
												(agdp() * 46), 0);*/
												
					}
					/*
					afbox_add(FB, dname,
										((desc ==
											NULL) ?
										 alang_get("symlink.error") :
										 desc), 0,
										&UI_ICONS[(dtype ==
															 24) ? 25 : 24],
										dtype, "", 0,
										selectedDefault);
										*/
										
					if (fm_stop_enum) { 
						fm_stop_enum=0;
						free(full_path); 
						free(files[n]);
						free(files);
						return; 
					}
					LIBAROMA_CTL_LIST_ITEMP listitem = libaroma_listitem_image(list, 1, 
												custitem((dtype==28)?linkfile_icon:linkfolder_icon, dname, 
												"Symlink", list->w, libaroma_dp(54)), 55, 
												LIBAROMA_LISTITEM_IMAGE_FILL|LIBAROMA_CTL_LIST_ITEM_RECEIVE_TOUCH, 
												-1);												
					//printf("Found symlink %s with desc %s and perms \n", dname, desc);
					/* customization starts here */
					struct FILE_INFO *file_info = malloc(sizeof(struct FILE_INFO));
					(*file_info).name=malloc(255);
					strcpy((*file_info).name, realpath(path, NULL));
					strcat((*file_info).name, dname);
					(*file_info).type=dtype;
					listitem->tag = (void*) file_info;
					
					if (desc != NULL) {
						free(desc);
					}
				}
				//-- ERROR LINK
				else if (dtype == 10) {					
					/*
					afbox_add(FB, dname,
										alang_get("symlink.error"), 0,
										&UI_ICONS[23], dtype, "", 0,
										selectedDefault);*/
					if (fm_stop_enum) { 
						fm_stop_enum=0;
						free(full_path); 
						free(files[n]);
						free(files);
						return; 
					}
					LIBAROMA_CTL_LIST_ITEMP listitem = libaroma_listitem_image(list, 1, 
												custitem(linkbroken_icon, dname, "Broken link", list->w, 
												libaroma_dp(54)), 55, LIBAROMA_LISTITEM_IMAGE_FILL|
												LIBAROMA_CTL_LIST_ITEM_RECEIVE_TOUCH, -1);
					//printf("Found broken symlink %s with desc  and perms \n", dname);
					/* customization starts here */
					struct FILE_INFO *file_info = malloc(sizeof(struct FILE_INFO));
					(*file_info).name=malloc(255);
					strcpy((*file_info).name, realpath(path, NULL));
					strcat((*file_info).name, dname);
					(*file_info).type=dtype;
					listitem->tag = (void*) file_info;
				}
				free(full_path);
			}
			
			free(files[n]);
		}
		
		free(files);
	}
	
	return 1;
}