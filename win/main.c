#include <aroma.h>

#ifdef __cplusplus
extern "C"{
#endif

#define ALOG(...)	printf(__VA_ARGS__); \
					printf("\n")

void winmain(){
	LIBAROMA_WINDOWP win=libaroma_window(NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
	LIBAROMA_CONTROLP list=libaroma_ctl_list(win, 1, 0, 0, win->w, win->h-48, 0, 0, libaroma_colorget(NULL, win)->control_bg,
							LIBAROMA_CTL_LIST_WITH_SHADOW|LIBAROMA_CTL_LIST_WITH_HANDLE);
	LIBAROMA_LISTITEM_CHECK_GROUPP grp1=libaroma_listitem_check_create_named_group(list, "Group 1");
	LIBAROMA_CTL_LIST_ITEMP opt1=libaroma_listitem_check(list, 2, 1, "Tap me", "Or not", NULL,
								LIBAROMA_LISTITEM_CHECK_LEFT_CONTROL|LIBAROMA_LISTITEM_CHECK_OPTION|LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN, -1);
	LIBAROMA_CTL_LIST_ITEMP opt2=libaroma_listitem_check(list, 3, 0, "Tap me", "Or not", NULL,
								LIBAROMA_LISTITEM_CHECK_LEFT_CONTROL|LIBAROMA_LISTITEM_CHECK_OPTION|LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN, -1);
	LIBAROMA_CTL_LIST_ITEMP opt3=libaroma_listitem_check(list, 4, 0, "Tap me", "Or not", NULL,
								LIBAROMA_LISTITEM_CHECK_LEFT_CONTROL|LIBAROMA_LISTITEM_CHECK_OPTION|LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN, -1);
	LIBAROMA_LISTITEM_CHECK_GROUPP grp2=libaroma_listitem_check_create_named_group(list, "Group 2");
	LIBAROMA_CTL_LIST_ITEMP opt11=libaroma_listitem_check(list, 2, 1, "Tap me", "Or not", NULL,
								LIBAROMA_LISTITEM_CHECK_LEFT_CONTROL|LIBAROMA_LISTITEM_CHECK_OPTION|LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN, -1);
	LIBAROMA_CTL_LIST_ITEMP opt12=libaroma_listitem_check(list, 3, 0, "Tap me", "Or not", NULL,
								LIBAROMA_LISTITEM_CHECK_LEFT_CONTROL|LIBAROMA_LISTITEM_CHECK_OPTION|LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN, -1);
	LIBAROMA_CTL_LIST_ITEMP opt13=libaroma_listitem_check(list, 4, 0, "Tap me", "Or not", NULL,
								LIBAROMA_LISTITEM_CHECK_LEFT_CONTROL|LIBAROMA_LISTITEM_CHECK_OPTION|LIBAROMA_LISTITEM_WITH_SEPARATOR|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN, -1);

	/*char *text=libaroma_stream_to_string(libaroma_stream("res:///license.txt"), 1);
	libaroma_listitem_text(list, 1, text, 0, 8, 0, 3, 0, 0, -1);*/
	LIBAROMA_CANVASP backicon, menuicon, nexticon;
	backicon=libaroma_image_uri("res:///back.png");
	menuicon=libaroma_image_uri("res:///menu.png");
	nexticon=libaroma_image_uri("res:///next.png");
	LIBAROMA_CONTROLP btn1=libaroma_ctl_button_icon(win, 11, 0, win->h-48, win->w/2-24, 48/*pad, btnY, btnW, btnH*/,
							"Back", backicon, LIBAROMA_CTL_BUTTON_ICON_LEFT, LIBAROMA_CTL_BUTTON_RAISED, 0);
	LIBAROMA_CONTROLP btn2=libaroma_ctl_button_icon(win, 12, (win->w/2)-24, win->h-48, 48, 48,
							"", menuicon, LIBAROMA_CTL_BUTTON_ICON_CENTER, LIBAROMA_CTL_BUTTON_RAISED, 0);
	LIBAROMA_CONTROLP btn3=libaroma_ctl_button_icon(win, 13, (win->w/2) + 24, win->h-48, win->w/2-24, 48/*pad, btnY, btnW, btnH*/,
							"Next", nexticon, LIBAROMA_CTL_BUTTON_ICON_RIGHT, LIBAROMA_CTL_BUTTON_RAISED, 0);
	ALOG("Window size is %dx%d (%dx%d in a dp world)", win->w, win->h, win->width, win->height);
	ALOG("List size is %dx%d", list->w, list->h);

	libaroma_window_show(win);
	byte onpool = 1;
	do{
		LIBAROMA_MSG msg;
		dword command=libaroma_window_pool(win,&msg);
		byte cmd	= LIBAROMA_CMD(command);
		word id	 = LIBAROMA_CMD_ID(command);
		byte param = LIBAROMA_CMD_PARAM(command);

		if (msg.msg==LIBAROMA_MSG_KEY_SELECT || msg.msg==LIBAROMA_MSG_KEY_POWER){ //exit on home/power
			if (msg.state==0){
				libaroma_png_save(libaroma_fb()->canvas, "/tmp/smol.png"); //save screenshot at exit
				onpool=0;
				break;
			}
		}
		else if (cmd==1){
			if (id==1){
				LIBAROMA_LISTITEM_CHECK_GROUPP grp=libaroma_listitem_check_get_selected_group(list);
				int index=libaroma_listitem_check_get_group_selected_index(grp);
				if (grp==NULL)
					printf("MLX: group for touched item not found\n");
				else {
					printf("MLX: item %d touched at group %d\n", index, grp->index);
				}
			}
			else if (id==11){
				printf("MLX_DEBUG: Back button selected\n");
				LIBAROMA_CONTROLP toast=libaroma_ctl_toast(win, 1, "Toast sample", 2000, LIBAROMA_CTL_TOAST_GRAVITY_BOTTOM,
									LIBAROMA_CTL_TOAST_COLORED|LIBAROMA_CTL_TOAST_OPAQUE, RGB(323232));
				printf("MLX_DEBUG: Should show created toast\n");
				libaroma_ctl_toast_show(toast);
				printf("MLX_DEBUG: Done?\n");
			}
		}
	} while (onpool);

	libaroma_window_free(win);
}

LIBAROMA_STREAMP stream_uri_callback(char * uri);

LIBAROMA_ZIP zip;

int main(int argc, char ** argv){

	zip=libaroma_zip("/tmp/res.zip");
	libaroma_stream_set_uri_callback(stream_uri_callback);
	libaroma_start();
	libaroma_font(0, libaroma_stream("res:///roboto.ttf"));
	winmain();

	libaroma_end();

	/* for recovery apps, uncomment the line below*/
	//kill(installer()->parent_pid, 18); //send CONTINUE signal to parent

	return 0;
}

LIBAROMA_STREAMP stream_uri_callback(char * uri){
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

#ifdef __cplusplus
}
#endif
