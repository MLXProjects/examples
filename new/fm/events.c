#ifndef __aromafm_events_c__
#define __aromafm_events_c__
#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

void sidelist_onclick(LIBAROMA_CONTROLP list){
	byte cur_id=libaroma_ctl_fragment_get_active_window_id(fragctl);
	LIBAROMA_CTL_LIST_ITEMP item = libaroma_ctl_list_get_touched_item(list);
	if (!item) return;
	libaroma_window_sidebar_show(sidebar, 0);
	switch (item->id){
		case ID_SIDELIST_FILEMGR:
			if (cur_id!=ID_FILEMFRAG && cur_id!=ID_FILESFRAG){
				libaroma_ctl_fragment_set_active_window(fragctl, (aromafm()->onmain_list)?ID_FILEMFRAG:ID_FILESFRAG, 0, 400, 0, NULL, NULL, NULL);
				aromafm_bar_update((aromafm_atroot())?aromafm()->base:aromafm_basename(aromafm()->base), aromafm()->dir, LIBAROMA_CTL_BAR_ICON_ARROW_TO_DRAWER);
			}
			break;
		case ID_SIDELIST_SETT:
			libaroma_ctl_fragment_set_active_window(fragctl, ID_SETTFRAG, 0, 400, 0, NULL, NULL, NULL);
			aromafm_bar_update("Settings", NULL, LIBAROMA_CTL_BAR_ICON_DRAWER_TO_ARROW);
			break;
		case ID_SIDELIST_ABOUT:
			libaroma_sleep(500);
			libaroma_dialog_confirm("AROMA FM", 
									"Libaroma engine by:\n"
									"<i>Ahmad Amarullah</i>\n"
									"(<u>amarullz@xda</u>)\n"
									"New aromafm by:\n"
									"<i>MLXProjects</i>\n"
									"(<u>mlx@xda</u>)",
															"OK",
						   NULL, libaroma_colorget(NULL, winmain), LIBAROMA_DIALOG_DIM_PARENT|LIBAROMA_DIALOG_WITH_SHADOW|LIBAROMA_DIALOG_CANCELABLE);
			break;
		case ID_SIDELIST_EXIT:
			winmain->onpool=0;
			/* send message to main window in order to exit */
			break;
	}
}

void aromafm_list_onclick(LIBAROMA_CONTROLP list){	
	LIBAROMA_CTL_LIST_ITEMP item = libaroma_ctl_list_get_touched_item(list);
	if (!item) return;
	
	if (aromafm()->selecting){
		libaroma_listitem_set_selected(list, item, libaroma_listitem_ischecked(item)?0:1);
	}
	else { /* handle item type */
		printf("MLX: going to check item path\n");
		if (item->tag!=NULL){ //check for file path
			char *file_path = (char *)item->tag;
			printf("MLX: item path is %s\n", file_path);
			if (!strcmp(file_path, "..")){ //go one level up
				printf("MLX: should go up\n");
				aromafm_goup();
				return;
			}
			printf("MLX: checking if file exists\n");
			if (!libaroma_file_exists(file_path))
				return; /* dir/file not found? */
			printf("MLX: seems to be there\n");
			if(aromafm_isdir(file_path)){ /* dir */
				printf("MLX: file is dir, moving there\n");
				aromafm_loadpath(file_path);
			}
			else { /* file */
				printf("file is file, trying to show info\n");
				char *strdup_fpath=strdup(file_path);
				char *strdupv2=strdup_fpath;
				int type = aromafm_gettype(aromafm_basename_ex(strdup_fpath, 0));
				printf("file type=%d (%s)\n", type, strdup_fpath);
				free(strdup_fpath);
				char type_str[64];
				switch (type){
					case AFM_FILE_PNG:
						snprintf(type_str, 64, "PNG image");
						break;
					case AFM_FILE_JPG:
					case AFM_FILE_JPEG:
						snprintf(type_str, 64, "JPG image");
						break;
					case AFM_FILE_TXT:
						snprintf(type_str, 64, "Text file");
						break;
					case AFM_FILE_SH:
						snprintf(type_str, 64, "Shell script");
						break;
					case AFM_FILE_APK:
						snprintf(type_str, 64, "Android package");
						break;
					case AFM_FILE_SAPK:
						snprintf(type_str, 64, "Android split package");
						break;
					case AFM_FILE_PROP:
						snprintf(type_str, 64, "Configuration file");
						break;
					case AFM_FILE_SVG:
						snprintf(type_str, 64, "Vector image");
						break;
					case AFM_FILE_HTML:
						snprintf(type_str, 64, "HTML document");
						break;
					case AFM_FILE_XML:
						snprintf(type_str, 64, "XML document");
						break;
					case AFM_FILE_TAR:
						snprintf(type_str, 64, "Compressed (tar) file");
						break;
					case AFM_FILE_ZIP:
						snprintf(type_str, 64, "Compressed (zip) file");
						break;
					case AFM_FILE_TWBAK:
						snprintf(type_str, 64, "TWRP partition backup");
						break;
					case AFM_FILE_INFO:
						snprintf(type_str, 64, "Information file");
						break;
					case AFM_FILE_GIF:
						snprintf(type_str, 64, "Animated image");
						break;
					case AFM_FILE_MP3:
						snprintf(type_str, 64, "MP3 audio");
						break;
					case AFM_FILE_MP4:
						snprintf(type_str, 64, "MP4 video");
						break;
					case AFM_FILE_IMG:
						snprintf(type_str, 64, "Partition image");
						break;
					default:
						snprintf(type_str, 64, "Unknown");
						break;
				}
				int dlg_len=strlen(file_path)+strlen(type_str)+128;
				char *dlg_text = malloc(dlg_len);
				snprintf(dlg_text, dlg_len, "Name: \n%s\n"
											"Path: \n%s\n"
											"Type: %s\n",
			 file_path, "this folder xd", type_str
				);
				libaroma_dialog_confirm("File info", dlg_text, "OK", NULL, libaroma_colorget(NULL, winmain), LIBAROMA_DIALOG_DIM_PARENT|LIBAROMA_DIALOG_WITH_SHADOW|LIBAROMA_DIALOG_CANCELABLE);
				free(dlg_text);
			}
		}
	}
}	

byte aromafm_list_oncheck(
	LIBAROMA_CONTROLP ctl,
	LIBAROMA_CTL_LIST_ITEMP item,
	int id,
	byte checked,
	voidp data,
	byte state
){
	if (state)
		aromafm()->sel_count++;
	else aromafm()->sel_count--;
	
	if (aromafm()->sel_count) {
		if (!aromafm()->selecting) aromafm()->selecting=1;
		char count_str[128];
		snprintf(count_str, 128, "%d items selected\n", aromafm()->sel_count);
		aromafm_statusbar_update(count_str);
	}
	else {
		if (aromafm()->selecting)
			aromafm()->selecting=0;
		aromafm_statusbar_reset();
	}
	return 1;
}
byte closing=0;
void aromafm_navbar_onclick(byte button_id){
	switch (button_id){
		case FM_NAVBAR_BACK: /* send back key event */
			libaroma_msg_post(LIBAROMA_MSG_KEY_BACK, 1, 0, 0, 0, NULL);
			break;
		case FM_NAVBAR_HOME:{
			LIBAROMA_WINDOWP active_win = libaroma_wm_get_active_window();
			if (active_win!=winmain){ /* if dialog shown, switch to main and free dialog */
				libaroma_msg_post(LIBAROMA_MSG_EXIT, 1, 0, 0, 0, NULL); /* trigger dialog close */
			}
			else {/* switch to main fragment */
				byte cur_id = libaroma_ctl_fragment_get_active_window_id(fragctl);
				if (cur_id!=ID_FILEMFRAG && cur_id!=ID_FILESFRAG){
					libaroma_ctl_fragment_set_active_window(fragctl, (aromafm()->onmain_list)?ID_FILEMFRAG:ID_FILESFRAG, 0, 400, 0, NULL, NULL, NULL);
					aromafm_bar_update((aromafm_atroot())?aromafm()->base:aromafm_basename(aromafm()->base), aromafm()->dir, LIBAROMA_CTL_BAR_ICON_ARROW_TO_DRAWER);
				}
				else { /* already at filemgr fragment */
					if (!aromafm_atroot()) aromafm_loadpath(strdup("/"));
				}
			}
		} break;
		case FM_NAVBAR_MENU:
			break;
	}
}

void aromafm_navbar_onhold(byte button_id){
	aromafm_navbar_onclick(button_id); /* behave exactly like click event */
}

#ifdef __cplusplus
}
#endif
#endif /* __aromafm_events_c__ */  
  
