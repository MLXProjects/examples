//#include "statusbar.c"
#include <sys/sysinfo.h>

//int sysinfo(struct sysinfo *info);

char *cmd_out(char *args[], int max_buffer);

void powerdialog(LIBAROMA_WINDOWP parent);

char *substring(char string[], int size, char delim){	
	int i;
	for (i=0;i<size;i++){
		if (string[i]==delim) { return string+i+1; }
	}
	return string;
}

void about(LIBAROMA_WINDOWP parent){
	#define ID_APPBAR			2
	#define ID_CTLLIST			3
	#define ID_CONTEXTMENU		4
	#define ID_RRVERSION		5
	#define ID_RRABOUT			6
	#define ID_STATUS			7
	#define ID_LEGALS			8
	#define ID_MODEL			9
	#define ID_LAVERSION		10
	#define ID_SECURITYVER		11
	#define ID_BASEBAND			12
	#define ID_KERNELVER		13
	#define ID_CPUINFO			14
	#define ID_RAMINFO			15
	#define ID_BUILDDATE		16
	#define ID_BUILDVER			17
	#define ID_SELINUX			18
	#define ID_ARROW			19
	/* menu flags */
	word menuflags =
		LIBAROMA_LISTITEM_WITH_SEPARATOR;
	
	LIBAROMA_WINDOWP win;
		
	/* menu item macro */
	#define _ITEM(id,text,ico,extra) \
		libaroma_listitem_menu(\
			list, id, text, extra, (ico!=NULL)?ico:NULL, \
			menuflags,-1)
	#define _CHECK(id,text,ico,extra,flags) \
		libaroma_listitem_check(\
			list, id, 0, text, extra, (ico!=NULL)?ico:NULL, \
			flags,-1)
	#define _TITLE(id,text) \
		libaroma_listitem_caption_color(list, id, text, \
			libaroma_colorget(NULL,win)->accent, -1)
	#define _DIV(id) \
		libaroma_listitem_divider(list, id, \
			LIBAROMA_LISTITEM_DIVIDER_SUBSCREEN,-1)
	
	win = libaroma_window(
		NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
	int ctlY = STATUSBAR_HEIGHT;
	int ctlH = 46;
	
	printf("creating null colorset\n");
	LIBAROMA_COLORSETP my_colorset = NULL;
	printf("initializing colorset\n");
	my_colorset = libaroma_colorget(NULL, win);/*
	my_colorset->window_bg = RGB(424242);
	my_colorset->window_text = RGB(E5E5E5);
	my_colorset->primary = RGB(80CBC4);
	my_colorset->primary_light = RGB(E5E5E5);*/
	my_colorset->accent = RGB(80CBC4);/*
	my_colorset->control_bg = RGB(424242);
	my_colorset->control_primary_text = RGB(E5E5E5);
	my_colorset->control_secondary_text = RGB(E5E5E5);
	my_colorset->dialog_bg = my_colorset->control_bg;
	my_colorset->dialog_primary_text = my_colorset->control_primary_text;
	my_colorset->dialog_secondary_text = my_colorset->control_secondary_text;*/
	printf("stablishing colorset\n");
	win->colorset = my_colorset;
	
	printf("creating bar\n");
	LIBAROMA_CONTROLP bar = libaroma_ctl_bar(
		win, ID_APPBAR,
		0, ctlY, LIBAROMA_SIZE_FULL, ctlH,
		"About phone", RGB(263238), RGB(ffffff)
	);
	ctlY += bar->h;	
	
	//libaroma_draw_rect(win->dc, 0, 0, win->w, STATUSBAR_HEIGHT, win->appbar_bg, 0xFF);
	
	printf("creating list\n");
	LIBAROMA_CONTROLP list = libaroma_ctl_list(
		win, ID_CTLLIST, /* win, id */
		0, ctlY, win->w, LIBAROMA_SIZE_FULL, /* x,y,w,h */
		0, 0 /*8*/, /* horiz, vert padding */
		win->colorset->control_bg, /* bgcolor */
		LIBAROMA_CTL_SCROLL_WITH_SHADOW
		|LIBAROMA_CTL_SCROLL_WITH_HANDLE
	);
	ctlY += list->h;
	LIBAROMA_CANVASP aroma_icon = libaroma_image_uri("res:///icons/aroma.png");
	//libaroma_canvas_fillcolor(aroma_icon, win->colorset->accent);
	
	word netitemsflags = LIBAROMA_LISTITEM_CHECK_SWITCH
			|LIBAROMA_LISTITEM_MENU_SMALL_ICON
			|LIBAROMA_LISTITEM_CHECK_HAS_SUBMENU
				|LIBAROMA_LISTITEM_WITH_SEPARATOR
				|LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN;
	
	/* read info here please */
	
	_ITEM(ID_RRVERSION, "Libaroma", aroma_icon, "LIBAROMA-v1-20201121-asmarandana");
	_ITEM(ID_RRABOUT, "About Libaroma", NULL, "Everything about the library");
	_ITEM(ID_STATUS, "Status", NULL, "Phone number, signal, etc.");
	//libaroma_listitem_menu(list, ID_STATUS, "Estado", "Eso :v", NULL, LIBAROMA_LISTITEM_WITH_SEPARATOR,-1);	
	_ITEM(ID_LEGALS, "Legal information", NULL, NULL);
	char * devmodel = libaroma_getprop("ro.product.model", libaroma_stream_file("/system/build.prop"), 1);
		printf("Model: %s\n", devmodel);
	if (devmodel == NULL) { 
		devmodel = libaroma_getprop("ro.product.model", libaroma_stream_file("/default.prop"), 1); 
		printf("Model: %s\n", devmodel);
	}
	_ITEM(ID_MODEL, "Model number", NULL, devmodel);
	_ITEM(ID_LAVERSION, "Libaroma version", NULL, "1.0.0");
	_ITEM(ID_SECURITYVER, "Security patch level", NULL, "never xd");
	_ITEM(ID_BASEBAND, "Baseband version", NULL, "Unknown");
	char *getkver_args[] = {"cat", "/proc/version" , (char*)0};
	_ITEM(ID_KERNELVER, "Kernel version", NULL, cmd_out(getkver_args, 512));
	char *getcpuinfo_args[] = {"grep", "Processor" , "/proc/cpuinfo", (char*)0};
	//printf("Step 1\n");
	char *getcpuinfo = cmd_out(getcpuinfo_args, 128);
	char getcpuinfo2[512] = "";
	strncpy(getcpuinfo2, getcpuinfo, sizeof(getcpuinfo2)-1);
	getcpuinfo2[sizeof(getcpuinfo2)-1] = '\0';	
	char getcpuinfo3[512] = "";
	int i;
	int i2 = 0;
	byte found = 0;
	for (i=0;i<sizeof(getcpuinfo2);i++){
		if (found){
			getcpuinfo3[i2] = getcpuinfo2[i];
			i2++;
		}
		else {
			if (getcpuinfo2[i]==':') { found = 1; }
		}
	}
	_ITEM(ID_CPUINFO, "CPU", NULL, substring(getcpuinfo2, sizeof(getcpuinfo2), ':')+1);
	struct sysinfo info;
	sysinfo(&info);
	char memtotal[64]="";
	sprintf(memtotal, "%lu", info.totalram/1024/1024);
	strcat(memtotal, "MB");
	_ITEM(ID_RAMINFO, "RAM", NULL, memtotal);
	_ITEM(ID_BUILDDATE, "Build date", NULL, "Fri Nov 21 01:58 UTC-3 2020");
	_ITEM(ID_BUILDVER, "Build number", NULL, "libaroma-1.0.0-150204");
	//printf("Estado de SELinux: %s\n", cmd_out({"getenforce"}));
	char *getselinux_args[] = {"getenforce" , (char*)0};
	char * selinux_status = cmd_out(getselinux_args, 16);
	if (selinux_status == NULL) { selinux_status = "Unknown"; }
	_ITEM(ID_SELINUX, "SELinux status", NULL, selinux_status);

	//libaroma_ctl_bar_set_icon_mask(bar,1,1);
	
	LIBAROMA_CTL_BAR_TOOLSP bar_tools=libaroma_ctl_bar_tools(0);
	/*
	libaroma_ctl_bar_tools_set(
		bar_tools, 0, ID_ARROW, "", NULL, NULL);
	*/
	libaroma_ctl_bar_set_tools(
		bar,bar_tools, 0);
	
	libaroma_ctl_bar_set_icon(
		bar,NULL,0,LIBAROMA_CTL_BAR_ICON_ARROW,1);
	
	//libaroma_ctl_bar_set_touchable_title(bar,1);
	
	libaroma_ctl_bar_set_menuid(bar, ID_CONTEXTMENU, 1);
	
	libaroma_window_anishow(win, LIBAROMA_WINDOW_SHOW_ANIMATION_PAGE_TOP, 400);
	
	libaroma_wm_set_active_window(win);
	pthread_t sb_thread = statusbar_draw(win);
	do{
				
		LIBAROMA_MSG msg;
		dword command=libaroma_window_pool(win,&msg);
		byte cmd	= LIBAROMA_CMD(command);
		word id	 = LIBAROMA_CMD_ID(command);
		byte param = LIBAROMA_CMD_PARAM(command);
		if (msg.msg==LIBAROMA_MSG_KEY_POWER){
			if (msg.state==0){
				//libaroma_png_save(libaroma_fb()->canvas,"/tmp/screenshot.png");
				printf("Power key pressed!\n");
				win->onpool = 0;
				break;
			}
		}
		
		printf("Window Command = (CMD: %i, ID: %i, Param: %i, Msg: %i, Key: %i, State: %i, X: %i, Y: %i)\n",
			LIBAROMA_CMD(command),
			LIBAROMA_CMD_ID(command),
			LIBAROMA_CMD_PARAM(command), 
			msg.msg, msg.key, msg.state, msg.x, msg.y);
		
		if (cmd == 1){
			if (id==ID_APPBAR){
				if (param==1){ //back icon pressed
				printf("Back icon pressed!\n");
				win->onpool=0;
				}
			}
			printf("ID: %i, MSG key: %i, PARAM: %i\n", id, msg.key, param);
			switch (msg.key){
				case ID_LAVERSION:
					break;
			}
		}
		
	}
	while(win->onpool);
	//window pool exited, so the window should close.
	
	if (!win->onpool){
		win->closed = 1;
		//msleep(2000);
		printf("Window closing!\n");
		statusbar_updater_stop(sb_thread);
		//msleep(2000);
	}
	/* undef menu item macro */
	#undef _DIV
	#undef _TITLE
	#undef _ITEM
	#undef _ITEMB
	
	libaroma_window_aniclose(win, parent, LIBAROMA_WINDOW_CLOSE_ANIMATION_PAGE_BOTTOM, 300);
}