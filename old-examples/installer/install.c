struct CONTROLS {
	LIBAROMA_CONTROLP list;
	LIBAROMA_CONTROLP titlelabel;
	LIBAROMA_CONTROLP filelabel;
	LIBAROMA_CONTROLP countlabel;
	LIBAROMA_CONTROLP cancelbtn;
	LIBAROMA_CONTROLP rebootbtn;
	LIBAROMA_CONTROLP textitem;
	LIBAROMA_CONTROLP progress;
	LIBAROMA_CONTROLP circleprogress;
	LIBAROMA_CANVASP reboot_cv;
	pid_t child_pid;
	char *file_path;
};
#include <unistd.h>

byte installation_inprogress=0;
byte installation_finished=0;
byte cancel_now=0;
int ai_progress_pos=0;
int ai_progress_max=100;


void *threaded_install(void* ctls);
pthread_t start_install(struct CONTROLS *ctls);

byte install_doit(char *flash_path){
	#define		ID_TEXTBOX		2
	#define		ID_CANCELBTN	3
	#define		ID_REBOOTBTN	4
	
	if (debug) printf("called install_doit!\n");
	
	LIBAROMA_CANVASP cancel_icon = libaroma_image_uri("res:///icons/cross.png");
	LIBAROMA_CANVASP reboot_icon = libaroma_image_uri("res:///icons/reboot.png");
	
	if (debug) printf("icons loaded, creating window!\n");
	LIBAROMA_WINDOWP win = libaroma_window(
		NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);

	if (debug) printf("Setting colorset!\n");	
	LIBAROMA_COLORSETP my_colorset=libaroma_colorget(NULL, win);
	my_colorset->accent = main_accent;
	my_colorset->primary = main_accent;
	my_colorset->window_bg = main_bgcolor;
	my_colorset->control_bg = RGB(111111);
	my_colorset->control_primary_text = RGB(E5E5E5);
	my_colorset->dialog_bg = RGB(222222);
	my_colorset->dialog_primary_text = RGB(E5E5E5);
	my_colorset->dialog_secondary_text = RGB(E5E5E5);
	win->colorset=my_colorset;
	
	if (debug) printf("Creating progress circle\n");
	
	byte progflags= LIBAROMA_CTL_PROGRESS_CIRCULAR|
					LIBAROMA_CTL_PROGRESS_DETERMINATE;
	
	LIBAROMA_CONTROLP circleprogress=libaroma_ctl_progress(win, 1, 5, 5, 44, 44, progflags, 1, 1);
	
	int textx=circleprogress->x+circleprogress->w+5;
	int text1h=libaroma_ctl_label_height("Currently installing:", libaroma_px(win->w), settings()->text_italic?FONT_LIGHT_ITALIC:FONT_LIGHT, 3, 
												LIBAROMA_TEXT_SINGLELINE, 0);
	int text2h=libaroma_ctl_label_height(basename(flash_path), libaroma_px(win->w), settings()->text_italic?FONT_LIGHT_ITALIC:FONT_LIGHT, 3, 
												LIBAROMA_TEXT_SINGLELINE, 0);
	int text3h=libaroma_ctl_label_height("1 item remaining", libaroma_px(win->w), settings()->text_italic?FONT_LIGHT_ITALIC:FONT_LIGHT, 3, 
												LIBAROMA_TEXT_SINGLELINE, 0);
	
	if (debug) printf("Text x: %d, height: %d\n", textx, text1h);
	LIBAROMA_CONTROLP titlelabel=libaroma_ctl_label_valign(win, 1, "Currently installing:", textx, /*STATUSBAR_HEIGHT+*/4, libaroma_px(win->w), text1h, 
								main_accent, settings()->text_italic?FONT_LIGHT_ITALIC:FONT_LIGHT, 3, LIBAROMA_TEXT_SINGLELINE, text1h, 0);
	LIBAROMA_CONTROLP filelabel=libaroma_ctl_label_valign(win, 1, basename(flash_path), textx, /*STATUSBAR_HEIGHT+*/4+text1h, 
								libaroma_px(win->w), text2h, main_accent, settings()->text_italic?FONT_LIGHT_ITALIC:FONT_LIGHT, 3, LIBAROMA_TEXT_SINGLELINE, 
								text2h, 0);
	LIBAROMA_CONTROLP countlabel=libaroma_ctl_label_valign(win, 1, "1 item remaining", textx, /*STATUSBAR_HEIGHT+*/4+text1h+text2h, 
								libaroma_px(win->w), text3h+3, main_accent, settings()->text_italic?FONT_LIGHT_ITALIC:FONT_LIGHT, 3, LIBAROMA_TEXT_SINGLELINE, 
								text3h, 0);
	int listY=/*(text1h+text2h+text3h+8)+*/(win->h-NAVBAR_HEIGHT-54);//(libaroma_px(itemlst->y)+libaroma_px(itemlst->h));
	int listH=44;//(libaroma_px(win->h-libaroma_dp(NAVBAR_HEIGHT))-libaroma_px(listY));
	
	int tbY=(/*STATUSBAR_HEIGHT+*/text1h+text2h+text3h+8);
	int tbH=(libaroma_px(win->h-libaroma_dp(NAVBAR_HEIGHT)-libaroma_dp(54))-libaroma_px(tbY));
	
	LIBAROMA_CONTROLP textlst=libaroma_ctl_list(
		win, 1, 0, tbY, libaroma_px(win->w), tbH, 0, 0, 
		main_bgcolor, LIBAROMA_CTL_SCROLL_WITH_HANDLE);
	LIBAROMA_CTL_LIST_ITEMP textitem=libaroma_listitem_text_color(textlst, 1, "", settings()->console_fore,
										0, 0, settings()->text_italic?FONT_MONOSPACE_ITALIC:FONT_MONOSPACE, 2, NULL, 1, -1);	
	
	LIBAROMA_CONTROLP progress=libaroma_ctl_progress(win, 2, 0, libaroma_px(textlst->y)+libaroma_px(textlst->h), libaroma_px(win->w), 10, 
													LIBAROMA_CTL_PROGRESS_DETERMINATE, ai_progress_max, ai_progress_pos);
													
	if (debug) printf("creating cancellst\n");
	LIBAROMA_CONTROLP cancellst = libaroma_ctl_list(
		win, ID_CANCELBTN, /* win, id */
		0, libaroma_px(progress->y)+libaroma_px(progress->h), LIBAROMA_SIZE_HALF, listH, /* x,y,w,h */
		0, 0 /*8*/, /* horiz, vert padding */
		main_bgcolor,/*libaroma_colorget(NULL, win)->control_bg, /* bgcolor */
		LIBAROMA_CTL_SCROLL_NO_INDICATOR
	);
	
	if (debug) printf("creating rebootlst\n");
	LIBAROMA_CONTROLP rebootlst = libaroma_ctl_list(
		win, ID_REBOOTBTN, 
		libaroma_px(cancellst->w), libaroma_px(progress->y)+libaroma_px(progress->h), LIBAROMA_SIZE_HALF, listH,
		0, 0,
		main_bgcolor,//libaroma_colorget(NULL, win)->control_bg, 
		LIBAROMA_CTL_SCROLL_NO_INDICATOR
	);
	
	if (debug) printf("creating items\n");	
	libaroma_listitem_image(cancellst, 1, custitem_ex(cancel_icon, "Cancel", 
								NULL, NULL, NULL, cancellst->w, listH, settings()->text_italic?CUSTITEM_ITALIC:0), 
								listH, LIBAROMA_CTL_LIST_ITEM_RECEIVE_TOUCH, -1);

	navbar_draw(win);
	if (debug) printf("Showing window!\n");
	//statusbar_end_thread();
	statusbar_init(settings()->sb_override_color, "Hi!");
	animate_window_transition(NULL, win, settings()->anim_type, settings()->anim_direction, 0);
	//libaroma_window_anishow(win, 0, 0);
	statusbar()->title=settings()->title;
	if (debug) printf("starting statusbar\n");
	statusbar_start_thread();
	if (debug) printf("creating controls struct\n");
	struct CONTROLS *ctls = malloc(sizeof(struct CONTROLS));
	(*ctls).list=textlst;
	(*ctls).textitem=textitem;
	(*ctls).progress=progress;
	(*ctls).circleprogress=circleprogress;
	(*ctls).titlelabel=titlelabel;
	(*ctls).filelabel=filelabel;
	(*ctls).countlabel=countlabel;
	(*ctls).rebootbtn=rebootlst;
	(*ctls).cancelbtn=cancellst;
	(*ctls).reboot_cv=reboot_icon;
	(*ctls).file_path=flash_path;
	(*ctls).child_pid=0;
	start_install(ctls);
	byte reboot_now=0;
	libaroma_ctl_progress_type(circleprogress, LIBAROMA_CTL_PROGRESS_CIRCULAR|LIBAROMA_CTL_PROGRESS_INDETERMINATE);	
	if (debug) printf("Window should be visible.\n");
	byte back_confirm=0;
	long last_press=0;//libaroma_tick();
	do{
		LIBAROMA_MSG msg;
		dword command=libaroma_window_pool(win,&msg);
		byte cmd	= LIBAROMA_CMD(command);
		word id		= LIBAROMA_CMD_ID(command);
		byte param	= LIBAROMA_CMD_PARAM(command);
		
		switch (msg.msg){
			case LIBAROMA_MSG_KEY_POWER:
				if (msg.state==0) show_lockscreen(win);
				break;
			case LIBAROMA_MSG_KEY_SELECT:
				{
					if (msg.state==0){
						//libaroma_png_save(libaroma_fb()->canvas,"/tmp/screenshot.png");
						if (debug) printf("Close key pressed!\n");
						win->onpool=0;
					}
				}
				break;
		}
		if (cmd == 1){
			vibrate(100);
			if (id==ID_NAVBACK){
				if (!installation_inprogress) {
					win->onpool=0;
					break;
				}
				long press_now=libaroma_tick();
				if (press_now-last_press < 2000){
					back_confirm=1;
				}
				else {
					back_confirm=0;
					last_press=libaroma_tick();
					libaroma_listitem_text_add(textitem, "Press back again to exit\n", 0);
					break;
				}				
				if (back_confirm){
					if (!installation_finished){
						if (installation_inprogress==1){
							libaroma_listitem_text_add(textitem, "Cancelling installation!\n", 0);
							cancel_now=1;
							installation_inprogress=0;
							installation_finished=1;
							libaroma_ctl_list_del_item_internal(cancellst, 0, 0);
							if ((*ctls).child_pid!=NULL){
								kill((*ctls).child_pid, 9);
							}
						}
					}
					win->onpool=0;
				}
			}
			else if (id==ID_NAVMENU){
				libaroma_listitem_text_add(textitem, "Not implemented yet.\n", 0);
			}
			else if (id==ID_NAVHOME){
				libaroma_listitem_text_add(textitem, "Not implemented yet\n", 0);
			}
			else if (id==ID_CANCELBTN&&msg.key==1){
				if (debug) printf("Cancel button pressed!\n");
				if (!installation_finished){
					if (installation_inprogress==1){
						libaroma_listitem_text_add(textitem, "Cancelling installation!\n", 0);
						cancel_now=1;
						installation_inprogress=0;
						installation_finished=1;
						libaroma_ctl_list_del_item_internal(cancellst, 0, 0);
						if ((*ctls).child_pid!=NULL){
							kill((*ctls).child_pid, 9);
						}
					}
				}
			}
			else if (id==ID_REBOOTBTN&&msg.key==1){
				if (debug) printf("Reboot button pressed!\n");
				reboot_now=1;
				win->onpool=0;
			}
		}
	}
	while(win->onpool);
	if (debug) printf("Closing window!\n");
	//statusbar_end_thread();
	//closewin(win, NULL, LIBAROMA_WINDOW_SHOW_ANIMATION_SLIDE_RIGHT, 2000);
	
	//libaroma_ctl_progress_type(circleprogress, progflags);
	statusbar()->title="Bye!";
	statusbar_end_thread();
	animate_window_transition(win, NULL, settings()->anim_type, settings()->anim_direction, 1);
	//statusbar_start_thread();
	return reboot_now;
}

pthread_t start_install(struct CONTROLS *ctls){
	if (installation_inprogress) return;
	pthread_t threadedinstall;
	pthread_create(&threadedinstall, NULL, threaded_install, (void*) ctls);
	return threadedinstall;
}

char * ai_trim(char * chr) {
  char * res = chr;
  char   off = 0;
  
  while ((off = *res)) {
    byte nobreak = 0;
    
    switch (off) {
      case ' ':
      case '\t':
      case '\n':
      case '\r':
        nobreak = 1;
        break;
    }
    
    if (!nobreak) {
      break;
    }
    
    res++;
  }
  
  int i;
  
  for (i = strlen(res) - 1; i >= 0; i--) {
    if ((res[i] == ' ') || (res[i] == '\n') || (res[i] == '\r') || (res[i] == '\t')) {
      res[i] = 0;
    }
    else {
      break;
    }
  }
  
  return res;
}

void *threaded_install(void* ctls){
	if (installation_inprogress)
		return;
	installation_inprogress=1;
	if (debug) printf("Getting info\n");
	struct CONTROLS *controls = (struct CONTROLS*) ctls;
	printf("Started install!\n");
	libaroma_zip_extract(zip, "META-INF/com/google/android/update-binary-installer", "/tmp/update-extracted");
	
	//-- Create Pipe
	int pipefd[2];
	pipe(pipefd);
	char **	argscmd = malloc(sizeof(char *) * 5);
	char	binary[256];
	char	pipestr[10];
	//-- Init Arguments
	snprintf(binary, 255, "%s", "/tmp/update-extracted");
	snprintf(pipestr, 9, "%d", pipefd[1]);
	argscmd[0] = binary;
	argscmd[1] = "3";
	argscmd[2] = pipestr;
	argscmd[3] = controls->file_path;
	argscmd[4] = NULL;
	
	//-- Start Installer
	controls->child_pid = fork();
	if (controls->child_pid == 0) {
		dup2(pipefd[1], STDOUT_FILENO);
		dup2(pipefd[1], STDERR_FILENO);
		close(pipefd[0]);
		execv(binary, argscmd);
		_exit(-1);
	}
	close(pipefd[1]);
	//-- Set New Progress Text
	libaroma_listitem_text_add(controls->textitem, "Starting installation...", 0);
	
	//-- Start Reading Feedback
	char buffer[1024];
	FILE * from_child = fdopen(pipefd[0], "r");
	printf("Installer: Get Events\n");
	
	while (fgets(buffer, sizeof(buffer), from_child) != NULL) {
		char * bufall	= strdup(buffer);
		char * command = strtok(buffer, " \n");
		
		if (command == NULL) {
			free(bufall);
			continue;
		}
		else if (strcmp(command, "progress") == 0) {
			char * fraction_s		= strtok(NULL, " \n");
			char * numfiles_s		= strtok(NULL, " \n");
			float progsize			= strtof(fraction_s, NULL);
			/*
			ai_progress_fract_n = strtol(numfiles_s, NULL, 10);
			ai_progress_fract_c = 0;
			ai_progress_fract_l = alib_tick();
			
			if (ai_progress_fract_n > 0) {
				ai_progress_fract = progsize / ai_progress_fract_n;
			}
			else if (ai_progress_fract_n < 0) {
				ai_progress_fract = progsize / abs(ai_progress_fract_n);
			}
			else {
				ai_progress_fract = 0;
				ai_progress_pos	 = progsize;
			}*/
			int prog_int=progsize*ai_progress_max;
			printf("progress: %2.5f\nfrac_s: %s\nnumfiles_s: %s\n", progsize, fraction_s, numfiles_s);
			printf("Add %d to the progress bar in %s seconds\n", prog_int, numfiles_s);
			if (ai_progress_pos+progsize<=ai_progress_max) 
				ai_progress_pos+=progsize;
			else ai_progress_pos=progsize;			
			libaroma_ctl_progress_value(controls->progress, ai_progress_pos);
		}
		else if (strcmp(command, "set_progress") == 0) {
			char * fraction_s = strtok(NULL, " \n");/*
			ai_progress_fract	 = 0;
			ai_progress_fract_n = 0;
			ai_progress_fract_c = 0;
			ai_progress_pos		 = */
			float progf=strtof(fraction_s, NULL);
			int prog=progf*ai_progress_max;
			printf("Set progress: %2.5f (%d)\n", progf, prog);
			if (prog<=ai_progress_max) ai_progress_pos=prog;
			else ai_progress_pos=ai_progress_max;
			libaroma_ctl_progress_value(controls->progress, ai_progress_pos);
		}
		else if (strcmp(command, "firmware") == 0) {
			//-- Firmware Command
			//fprintf(apipe(), "%s\n", ai_trim(bufall));
			printf("Firmware: %s\n", ai_trim(bufall));
		}
		else if (strcmp(command, "ui_print") == 0) {
			char * str = strtok(NULL, "\n");
			
			if (str) {
				printf("%s\n", str);
				libaroma_listitem_text_add(controls->textitem, str, 0);
				libaroma_listitem_text_add(controls->textitem, "\n", 0);
				/*if (str[0] == '@') {
					char tmpbuf[256];
					snprintf(tmpbuf, 255, "<#selectbg_g><b>%s</b></#>", str + 1);
					actext_appendtxt(ai_buftxt, tmpbuf);
					fprintf(fpi, "%s\n", tmpbuf);
					char * t_trimmed = ai_trim(str + 1);
					snprintf(ai_progress_text, 63, "%s", t_trimmed);
					fprintf(fp, "%s\n", t_trimmed);
				}
				else {
					actext_appendtxt(ai_buftxt, str);
					
					fprintf(fpi, "%s\n", str);
					char * t_trimmed = ai_trim(str);
					snprintf(ai_progress_info, 100, "%s", t_trimmed);
					fprintf(fp, "	%s\n", t_trimmed);
				}*/
			}
		}
		else if (strcmp(command, "minzip:") == 0) {
			char * minzipcmd = ai_trim(strtok(NULL, "\""));
			
			if (strcmp(minzipcmd, "Extracted file") == 0) {
				char * filename = strtok(NULL, "\" \n");
				printf("Extract: %s\n", filename);
				libaroma_listitem_text_add(controls->textitem, "Extract: ", 0);
				libaroma_listitem_text_add(controls->textitem, filename, 0);
				libaroma_listitem_text_add(controls->textitem, "\n", 0);
			}
		}
		else {
			char * str = ai_trim(bufall);
			printf("random: %s\n", str);
		}
		
		// LOGS("Installer: Command(%s)\n", command);
		free(bufall);
	}
	
	//libaroma_listitem_text_add(controls->textitem, "Installer: Exited - Close Process Handler\n", 0);
	fclose(from_child);
	//-- Get Return Status
	int ai_return_status = 0;
	//libaroma_listitem_text_add(controls->textitem, "Installer: Wait For PID\n", 0);
	waitpid(controls->child_pid, &ai_return_status, 0);
	
	if (!WIFEXITED(ai_return_status) || WEXITSTATUS(ai_return_status) != 0) {
		snprintf(buffer, 1023, "Installer Error (Status %d)", WEXITSTATUS(ai_return_status));
	}
	else {
		snprintf(buffer, 1023, "Installer Sucessfull (Status %d)", WEXITSTATUS(ai_return_status));
	}
	printf("Install finished? :P\n");
	installation_finished=1;
	installation_inprogress=0;
	libaroma_ctl_list_del_item_internal(controls->cancelbtn, 0, 0); // delete cancel button after finish setup
	libaroma_listitem_text_add(controls->textitem, "Installation finished, you can reboot now.\nTo close, just press back.", 0);
	libaroma_ctl_list_del_item_internal(controls->rebootbtn, 0, 0); // delete reboot button after finish setup
	LIBAROMA_CTL_LIST_ITEMP reboot=libaroma_listitem_image(controls->rebootbtn, 1, 
								custitem_ex(controls->reboot_cv, "Reboot", 
								NULL, NULL, NULL, controls->rebootbtn->w, controls->rebootbtn->h, 
								CUSTITEM_CIRCLE_RIGHT|(settings()->text_italic?CUSTITEM_ITALIC:0)), controls->rebootbtn->h, 
								LIBAROMA_CTL_LIST_ITEM_RECEIVE_TOUCH, -1);
	libaroma_ctl_label_set_text(controls->titlelabel, "All done", 1);
	libaroma_ctl_label_set_text(controls->filelabel, "1 item installed", 1);
	libaroma_ctl_label_set_text(controls->countlabel, "", 1);
	libaroma_ctl_progress_type(controls->circleprogress, LIBAROMA_CTL_PROGRESS_CIRCULAR|LIBAROMA_CTL_PROGRESS_DETERMINATE);
	libaroma_ctl_progress_value(controls->progress, ai_progress_max);
	libaroma_window_invalidate(controls->progress->window, 1);
}
/*
void *old_threaded_install(void* ctls){
	if (installation_inprogress)
		return;
	installation_inprogress=1;
	if (debug) printf("Getting info\n");
	struct CONTROLS *controls = (struct CONTROLS*) ctls;
	printf("Started fake install!\n");
	libaroma_listitem_text_add(controls->textitem, "Lossing some time 1...\n", 0);
	libaroma_sleep(3*1000);
	if (!cancel_now)libaroma_listitem_text_add(controls->textitem, "Lossing some time 2...\n", 0);
	if (!cancel_now)libaroma_sleep(3*1000);
	if (!cancel_now)libaroma_listitem_text_add(controls->textitem, "How much time do you have to waste on this? lol\n", 0);
	if (!cancel_now)libaroma_sleep(3*1000);
	if (!cancel_now)libaroma_ctl_progress_type(controls->progress, LIBAROMA_CTL_PROGRESS_CIRCULAR|LIBAROMA_CTL_PROGRESS_DETERMINATE);
	if (!cancel_now)libaroma_ctl_progress_value(controls->progress, 2);
	if (cancel_now){
		libaroma_ctl_label_set_text(controls->titlelabel, "Installation cancelled.", 1);
		libaroma_ctl_label_set_text(controls->filelabel, "1 item not installed", 1);
		libaroma_ctl_label_set_text(controls->countlabel, "", 1);
		LIBAROMA_CTL_LIST_ITEMP reboot=libaroma_listitem_image(controls->rebootbtn, 1, 
									custitem_ex(controls->reboot_cv, "Reboot", 
									NULL, NULL, NULL, controls->rebootbtn->w, controls->rebootbtn->h, 
									CUSTITEM_CIRCLE_RIGHT), controls->rebootbtn->h, 
									LIBAROMA_CTL_LIST_ITEM_RECEIVE_TOUCH, -1);
		libaroma_ctl_list_del_item_internal(controls->rebootbtn, 0, 0);
		libaroma_listitem_text_add(controls->textitem, "Installation cancelled.", 0);
		libaroma_ctl_progress_type(controls->progress, LIBAROMA_CTL_PROGRESS_CIRCULAR|LIBAROMA_CTL_PROGRESS_DETERMINATE);
		installation_finished=1;
		return;
	}
	libaroma_ctl_list_del_item_internal(controls->cancelbtn, 0, 0); // delete cancel button after finish setup
	libaroma_listitem_text_add(controls->textitem, "Installation finished, you can reboot now.\n", 0);
	LIBAROMA_CTL_LIST_ITEMP reboot=libaroma_listitem_image(controls->rebootbtn, 1, 
								custitem_ex(controls->reboot_cv, "Reboot", 
								NULL, NULL, NULL, controls->rebootbtn->w, controls->rebootbtn->h, 
								CUSTITEM_CIRCLE_RIGHT), controls->rebootbtn->h, 
								LIBAROMA_CTL_LIST_ITEM_RECEIVE_TOUCH, -1);
	libaroma_ctl_label_set_text(controls->titlelabel, "All done", 1);
	libaroma_ctl_label_set_text(controls->filelabel, "1 item installed", 1);
	libaroma_ctl_label_set_text(controls->countlabel, "", 1);
	
	installation_finished=1;	
}
*/