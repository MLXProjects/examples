#include <aroma.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>

typedef struct {
	int w;
	int h;
	int sz;
	char info[64];
} ASHMEM_CANVAS, * ASHMEM_CANVASP;

LIBAROMA_CONTROLP twrp_ctl;
LIBAROMA_CANVASP shmem_cv = NULL;
LIBAROMA_CANVASP ctl_cv = NULL;
bytep memfile=NULL;
FILE *evpipe=NULL;

pthread_t shmemcv_update_id;

byte pls_close=0;
byte stop_shmemcv_update=1;
byte shmem_alpha=0xFF;
byte allow_unsafe=0;

void shmemcv_update_thread(void *data){
	while (!pls_close){
		if (!stop_shmemcv_update){
			//if (allow_unsafe){
				libaroma_draw(ctl_cv, shmem_cv, 0, 0, 0);
				libaroma_ctl_image_update(twrp_ctl);
			//}
			//libaroma_fb_sync();
			libaroma_sleep(16);
		}
		else {
			if (pls_close){
				break;
			}
			libaroma_sleep(200);
		}
	}
	printf("update thread exited\n");
	libaroma_msg_post(LIBAROMA_MSG_EXIT, 0, 0, 0, 0, NULL);
	libaroma_sleep(100);
}

byte mlx_wm_msg_handler(LIBAROMA_WMP wm, LIBAROMA_MSGP msg){
	if (msg->msg==LIBAROMA_MSG_TOUCH){
		printf("Touch %i-%i state=%i, (%ix%i)\n", msg->msg, msg->key, msg->state, msg->x, msg->y);
		switch (msg->state){
			case 0:
				break;
			case 1:
				break;
			case 2:
				break;
		}
		return 0;
	}
	else if (msg->state==0){
		switch (msg->msg){
			case LIBAROMA_MSG_KEY_SELECT:
			case LIBAROMA_MSG_KEY_POWER:{
				//stop_shmemcv_update=1;
				//pls_close=1;
				//return LIBAROMA_WM_MSG_EXIT;
			} break;
			case LIBAROMA_MSG_KEY_VOLUP:{
				printf("vol up\n");
				if (!stop_shmemcv_update){
					printf("should exit updater thread\n");
					stop_shmemcv_update=1;
					pls_close=1;
					return 0;
				}
				stop_shmemcv_update=0;
			} break;
			case LIBAROMA_MSG_KEY_VOLDOWN:{
				printf("vol down\n");
				stop_shmemcv_update=1;
			} break;
			default:{
				printf("Message %i-%i state=%i, (%ix%i)\n", msg->msg, msg->key, msg->state, msg->x, msg->y);
			} break;
		}
	}
	return 0;
}

byte mlx_asd(){
	char *shmem_path="/tmp/.mlx_shmem";
	/*if (libaroma_file_exists(shmem_path)){
		shmem_cv=libaroma_canvas_shmem(libaroma_fb()->w, libaroma_fb()->h, 0, shmem_path);
		libaroma_canvas_delete(shmem_cv);
	}*/
	shmem_cv=libaroma_canvas_shmem(libaroma_fb()->w, libaroma_fb()->h, 0, shmem_path);
	if (!shmem_cv) return 0;
	/*int childpipe[2];
	int ret=pipe(childpipe);
	if (ret<0){
		goto fail;
	}*/
	printf("drawing to screen\n");
	
	libaroma_wm_set_message_handler(&mlx_wm_msg_handler);
	/*
	int pipedone = mkfifo(myfifo, 0666);
	if (done!=0){
		printf("mkfifo failed %d - %s\n", errno, strerror(errno));
	}*/
	
	int maxw = libaroma_px(libaroma_fb()->w);
	int maxh = libaroma_px(libaroma_fb()->h);
	
	libaroma_font(0, libaroma_stream("file:///twres/fonts/RobotoCondensed-Regular.ttf"));
	
	LIBAROMA_WINDOWP win = libaroma_window(NULL, 0, 0, 0, 0);
	if (!win) goto fail;
	LIBAROMA_WINDOWP sb = libaroma_window_sidebar(win, 0);
	if (!sb) goto fail;
	LIBAROMA_CONTROLP pager = libaroma_ctl_pager(win, 1, 2, 0, 0, 0, 0);
	if (!pager) goto fail;
	LIBAROMA_WINDOWP pagwin=libaroma_ctl_pager_get_window(pager);
	libaroma_ctl_pager_set_animation(pager, LIBAROMA_CTL_PAGER_ANIMATION_STACKIN);
	LIBAROMA_CONTROLP frag = libaroma_ctl_fragment(pagwin, 4, maxw, 0, maxw, 0);
	if (!frag) goto fail;
	LIBAROMA_CONTROLP bar = libaroma_ctl_bar(win, 9, 0, 0, maxw, 56, "Libaroma test", RGB(009385), RGB(FFFFFF));
	libaroma_ctl_bar_set_icon(bar,NULL,0,LIBAROMA_CTL_BAR_ICON_DRAWER, 1);
	LIBAROMA_WINDOWP fragwin = libaroma_ctl_fragment_new_window(frag, 1);
	libaroma_ctl_fragment_set_active_window(frag, 1, 0, 0, 0, 0, 0, 0);
	ctl_cv = libaroma_canvas(libaroma_fb()->w, libaroma_fb()->h);
	twrp_ctl = libaroma_ctl_image_canvas_ex(fragwin, 2, ctl_cv, 0, 0, 0, 0, LIBAROMA_CTL_IMAGE_SHARED);
	libaroma_ctl_button(pagwin, 8, maxw*0.25, maxh*0.25, maxw*0.5, 64, "Close", LIBAROMA_CTL_BUTTON_RAISED|LIBAROMA_CTL_BUTTON_CIRCLE|LIBAROMA_CTL_BUTTON_COLORED, RGB(009385));
	libaroma_ctl_progress(pagwin, 3, 
					(maxw*0.5)-24, (libaroma_px(libaroma_fb()->h)*0.5)-24,
					48, 48,
					LIBAROMA_CTL_PROGRESS_INDETERMINATE|LIBAROMA_CTL_PROGRESS_CIRCULAR,
					1, 1);
	int thdone = pthread_create(&shmemcv_update_id, NULL, &shmemcv_update_thread, NULL);
	
	if (thdone!=0){
		printf("failed to create updater thread\n");
	}
	libaroma_window_anishow(win, 12, 400);
	LIBAROMA_MSG msg;
	dword command;
	byte cmd, param;
	word id;
	do {
		command	= libaroma_window_pool(win,&msg);
		cmd		= LIBAROMA_CMD(command);
		id		= LIBAROMA_CMD_ID(command);
		param	= LIBAROMA_CMD_PARAM(command);
		
		if (msg.msg==LIBAROMA_MSG_EXIT/* || msg.msg==LIBAROMA_MSG_KEY_SELECT || msg.msg==LIBAROMA_MSG_KEY_POWER*/){
			if (allow_unsafe){
				allow_unsafe=0;
				continue;
			}
			win->onpool=0;
			break;
		}
		else if (cmd==LIBAROMA_CMD_CLICK && id==8){
			stop_shmemcv_update=1;
			pls_close=1;
		}
	} while (win->onpool);
	printf("window onpool exited\n");
	stop_shmemcv_update=1;
	pls_close=1;
	libaroma_sleep(300);
	libaroma_window_aniclose(win, 12, 400);
	libaroma_window_free(win);
	printf("window freed\n");
	//libaroma_wm_client_start();
	
	/*while (!pls_close){
		libaroma_sleep(32);
	}
	
	libaroma_wm_client_stop();
	*/
	/*while (!stop_shmemcv_update){
		libaroma_sleep(16);
		libaroma_draw(libaroma_fb()->canvas, shmem_cv, 0, 0, 0);
		//libaroma_ctl_image_update(twrp_ctl);
		libaroma_fb_sync();
	}*/
	/* fork & exec */
	/*char buf;
	char *binary="/sbin/recovery";
    const char** args = (const char**)malloc(sizeof(char*) * 3);
    args[0] = binary;
    args[1] = "--mmap=/tmp/.mlx_shmem";
    args[2] = NULL;
	pid_t child;
	child=fork();
	if (child==-1){
		goto fail;
	}
	if (child==0){
        execv(binary, (char* const*)args);
		exit(0);
	}*/
	/*
	//kill(child, 9);
	/* close write-end */
	//close(childpipe[0]);
	/*
    char buffer[1024];
    evpipe = fdopen(childpipe[1], "w");
	if (!evpipe){
		printf("Failed to open pipe\n");
		kill(child, 9);
	}
    while (fgets(buffer, sizeof(buffer), evpipe) != NULL) {
        char* command = strtok(buffer, " \n");
        if (command == NULL) {
            continue;
        } else if (strcmp(command, "fb_update")==0){
			libaroma_draw(libaroma_fb()->canvas, shmem_cv, 0, 0, 0);
			libaroma_fb_sync();
		} else if (strcmp(command, "ui_print") == 0) {
            char* str = strtok(NULL, "\n");
            if (str) {
                printf("Server: client said - %s\n", str);
            } else {
                printf("\n");
            }
        }
    }
    fclose(evpipe);*/
	
	return 1;
fail:
	//if (win) libaroma_window_free(win);
	//kill(child, 9);
	return 0;
	
}

int main(int argc, char **argv){
	umask(0);
	printf("just do it\n");
	//libaroma_sleep(2000);
	int ret;
	ret=libaroma_start();
	if (!ret) return 0;
	ret=mlx_asd();
	//libaroma_canvas_delete(shmem_cv);
	libaroma_end();
	return ret;
}