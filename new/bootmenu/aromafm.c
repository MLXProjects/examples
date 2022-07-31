#ifndef __bootmenu_aromafm_c__
#define __bootmenu_aromafm_c__
#ifdef __cplusplus
extern "C" {
#endif
	
#include "common.h"
/*
struct FILE_INFO { 
	char *name;
	byte type;
};*/

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
		aui_setpath(&buf, bootmenu_fm()->cwd, name, 0);
		
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
			if (!strcmp(dname, ".") || !strcmp) {
				continue;
			}
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
	/*	TODO: use hashes comparison */
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
	}
	return 0;
}

int aui_fetch(char * path, LIBAROMA_CONTROLP list){
	printf("MLX: Navigating to %s\n", path);
	//int count=aui_getdircount(aui_dir_active_path);
	//printf("Folder has %d items!\n", count);
	struct dirent ** files;
	int n = scandir(path, &files, 0, *aui_fsort);
	if (0){
		LIBAROMA_CTL_LIST_ITEMP listitem = libaroma_listitem_menu(list, 1, 
												"..", "Up a level", NULL, LIBAROMA_LISTITEM_MENU_INDENT_NOICON, -1);
		
		listitem->tag=(void*) "..";
		
		/* customization starts here */
	}
	printf("cwd = %s\n", bootmenu_fm()->cwd);
	if (n > 0) {
		while (n--) {
			//printf("Getting info about file/folder %d\n", n);
			char * dname = files[n]->d_name;
			
			if (!strcmp(dname, ".") || !strcmp(dname, "..")) {
				free(files[n]);
				continue;
			}
			
			/* if hidden file, skip? 
			if ( (dname[0] == '.')&& (auic()->showhidden == 0)) {
				free(files[n]);
				continue;
			}*/
			
			//-- GET TYPE
			//printf("Getting file/folder type\n");
			byte dtype = aui_dtcheck(dname, files[n]->d_type, 20);
			char * full_path = NULL;
			aui_setpath(&full_path, bootmenu_fm()->cwd, dname, ((dtype==4)||dtype==24)?1:0);
			
			if (full_path) {
				//printf("full_path = %s\n", full_path);
				/*if (auic()->showhidden == 0) {
					if (aui_ishidden_file(full_path)) {
					}
				}*/
				
				//printf("Checking file v2, dtype is %d\n", dtype);
				
				char desc[256];
				char perm[10];
				
				switch (dtype){
					case 4:{ //-- DIR
						int cnt = aui_getdircount(full_path);
						aui_fileperm(perm, full_path);
						char formats[256];
						char formats2[256];
						snprintf(formats, 256,
										"%d ", (cnt>0)?cnt:0);
						snprintf(formats2, 256,
										((cnt==1) ?"file":"files"),
										formats);
						snprintf(desc, 256, "%s",
										formats2);
						//printf("Concatenating description with permissions!\n");
						char perms[12];/*
						strcat(perm, ", ");
						strcat(perm, desc);*/
						snprintf(perms, 12, "%s", perm);
						LIBAROMA_CTL_LIST_ITEMP listitem = libaroma_listitem_menu(list, 1, 
													dname, perms, bootmenu_fm()->folder_cv, LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN, -1);	
						//printf("Found folder %s with desc %s and perms %s\n", dname, desc, perms);
						/* customization starts here */
						listitem->tag=(voidp) full_path;
					} break;
					case 8:{ //-- FILE
						char dsz[256];
						aui_fileperm(perm, full_path);
						aui_filesize(dsz, full_path);
						snprintf(desc, 256, "%s",
										dsz);
						//printf("Concatenating description with permissions!\n");
						char perms[12];/*
						strcat(perm, ", ");
						strcat(perm, desc);*/
						snprintf(perms, 12, "%s", perm);
						LIBAROMA_CTL_LIST_ITEMP listitem = libaroma_listitem_menu(list, 1, 
													dname, perms, bootmenu_fm()->file_cv, LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN, -1);	
						//printf("Found file %s with desc %s and perms %s\n", dname, desc, perms);
						/* customization starts here */
						listitem->tag=(voidp) full_path;
					} break;
					case 24:
					case 28:{ //-- LINK DIR / FILE
						char * desc = NULL;
						char buf[4096];
						int len =
							readlink(full_path, buf,
											sizeof(buf));
											
						if (len >= 0) {
							buf[len] = 0;		
						}
						LIBAROMA_CTL_LIST_ITEMP listitem = libaroma_listitem_menu(list, 1, 
													dname, "Symlink", (dtype==28)?bootmenu_fm()->file_cv:bootmenu_fm()->folder_cv, LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN, 
													-1);
						listitem->tag=(voidp) buf;
						//printf("Found symlink %s with desc %s and perms \n", dname, desc);
						/* customization starts here */
						if (desc != NULL) {
							free(desc);
						}
					} break;
					case 10:{ //-- ERROR LINK
						LIBAROMA_CTL_LIST_ITEMP listitem = libaroma_listitem_menu(list, 1, 
													dname, "Broken link", bootmenu_fm()->file_cv, LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN, -1);
						//printf("Found broken symlink %s with no desc nor perms \n", dname);
						/* customization starts here */
					} break;
					default:{ //-- UNHANDLED
						printf("Found file %s with unhandled type\n", dname);
						free(full_path);
						free(files[n]);
						continue;
					}
				}
			}
			
			free(files[n]);
		}
		
		free(files);
	}
	
	return 1;
}

#ifdef __cplusplus
}
#endif
#endif /* __bootmenu_aromafm_c__ */  
