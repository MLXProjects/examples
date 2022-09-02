/********************************************************************[libaroma]*
 * Copyright (C) 2011-2015 Ahmad Amarullah (http://amarullz.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *______________________________________________________________________________
 *
 * Filename    : main.c
 * Description : libaroma main test file
 *
 * + This is part of libaroma, an embedded ui toolkit.
 * + 04/02/15 - Author(s): Ahmad Amarullah
 *
 */
#ifndef __libaroma_libaroma_test_c__
#define __libaroma_libaroma_test_c__
#ifdef __cplusplus
extern "C" {
#endif
/* libaroma header */
#include <aroma.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>

/* debug */
char *debug_tag;
FILE *parent_pipe;

#define TEST_USE_DEBUG
#ifdef TEST_USE_DEBUG
#define RLOG(...) \
  fprintf(parent_pipe, debug_tag); \
  fprintf(parent_pipe, ": "); \
  fprintf(parent_pipe, __VA_ARGS__); \
  fprintf(parent_pipe, "\n");
#else
#define RLOG(...)
#endif

extern void full_test();

LIBAROMA_ZIP zip;
#ifndef _WIN32
pid_t parent_pid;
#endif

/* stream uri callback */
LIBAROMA_STREAMP stream_uri_cb(char * uri){
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

/*
 * Function    : init_libaroma
 * Return Value: void
 * Descriptions: init libaroma
 */
void init_libaroma(){

	/* set libaroma runtime configuration */
	libaroma_config()->fb_shm_name[0]=0;
	libaroma_start();

	/* clean display */
	//libaroma_canvas_blank(libaroma_fb()->canvas);
	//libaroma_sync();

	/* swap red and blue, enable if needed on device */
	//libaroma_fb_setrgb(0, 8, 16);

	/* load font - id=0 */
	libaroma_font(0,
		libaroma_stream(
			"res:///fonts/Roboto-Regular.ttf"
		)
	);
} /* End of init_libaroma */
#ifndef _WIN32
char *recovery_find(){
	char *recproc, *recpath;
	parent_pid = getppid();
	recproc = malloc(64);
	snprintf(recproc, 64, "/proc/%d/exe", parent_pid);
	recpath = malloc(512);
	readlink(recproc, recpath, 512);
	free(recproc);
	return recpath;
}

byte replace_recovery(char *recpath, char *recpath_orig, int recpath_len, char *updpath, char *zippath){
	RLOG("temporarily replacing recovery with test");
	int ret;
	ret = 0;

	char * mount_args[5];
	mount_args[0]="mount";
	mount_args[1]="-o";
	mount_args[2]="remount,rw";
	mount_args[3]="/";
	mount_args[4]=NULL;

	//first, remount root as rw to allow any changes.
	int status;
	if (fork() == 0) {
		execvp(mount_args[0], mount_args);
	}
	else {
		wait(&status);
		RLOG("mount exited with code %d", status);
		if (status!=0) return status;
	}

	//next, get recovery binary path (for temporary replacing)
	char *recpath_temp;
	recpath_temp=malloc(recpath_len+6);
	sprintf(recpath_temp, "%s-temp", recpath);

	//next, build the recovery replacer script
	char *args;
char *script =
"echo \"[SCRIPT STARTED]\" >> \"${zip_path}.log\" 2>&1\n\
# exec updater process and wait until it closes\n\
\"${upd_path}\" 0 0 \"${zip_path}\" \"${rec_path}\" >> \"${zip_path}.log\" 2>&1\n\
# revert recovery binary back to original\n\
rm -f \"${rec_path}\" >> \"${zip_path}.log\" 2>&1\n\
mv \"${rec_path}-original\" \"${rec_path}\" >> \"${zip_path}.log\" 2>&1\n\
echo \"[SCRIPT FINISHED]\" >> \"${zip_path}.log\" 2>&1\n";
	args = malloc(strlen(script)+128);
	snprintf(args, strlen(script)+128,
				"#!/sbin/sh\n"
				"zip_path=\"%s\"\n"
				"upd_path=\"%s\"\n"
				"rec_path=\"%s\"\n"
				"%s"
				, zippath, updpath, recpath, script);
	RLOG("writing script to %s", recpath_temp);

	//then write it into /<recoverypath>/recovery-temp
    FILE *fp = fopen(recpath_temp, "ab");
    if (fp != NULL)
    {
        fputs(args, fp);
        fclose(fp);
        RLOG("script written");
    }
    else {
		RLOG("failed to write script");
		goto exit;
	}

	char mode[] = "0777";
	int i;
	i = strtol(mode, 0, 8);
	int errno;
	errno = 0;
	if (errno = chmod(recpath_temp,i) < 0)
	{
		RLOG("error in chmod(%s, %s) - %d\n",
				recpath_temp, mode, errno);
		goto exit;
	}

    //swap recovery and script
	if (rename(recpath, recpath_orig)) {
		RLOG("rename original recovery failed");
		goto exit;
	} else RLOG("rename original recovery done");
	if (rename(recpath_temp, recpath)) {
		RLOG("rename script failed");
		rename(recpath, recpath_orig);
		goto exit;
	} else RLOG("rename script done");

	//kill the real recovery process
	free(recpath_temp);
	RLOG("let's kill the recovery");
	ret = kill(parent_pid, 9);

	//and finally return (this program should be re-executed by the restarted recovery
exit:
	if (ret!=9) free(recpath_temp);
	return 0;
}
#endif

/*
 * Function    : main
 * Return Value: int
 * Descriptions: main executable function
 */
int main(int argc, char **argv){

	libaroma_sdl_startup_size(480, 640);
	libaroma_sdl_window_title("Test App");

#ifdef LIBAROMA_GFX_MINUI
	libaroma_config()->gfx_first_backend = LIBAROMA_GFX_MINUI;
#endif
	//make things safer here
	libaroma_config()->runtime_monitor = LIBAROMA_START_SAFE;
	#ifdef _WIN32
	if (argc>=2)
		zip = libaroma_zip(argv[1]);
	if (zip==NULL)
		zip=libaroma_zip("./res.zip");
	libaroma_stream_set_uri_callback(stream_uri_cb);
	
	#else
	if (argc>=4){
		if (atoi(argv[2])!=0){
			debug_tag = "ui_print I/LIBAROMA()";
			parent_pipe = fdopen(atoi(argv[2]), "wb");

			//if device is DRM capable, do the recovery-replace method
			if (libaroma_file_exists("/dev/dri/card0")){
				char *recpath = recovery_find();
				int recpath_len=strlen(recpath);
				char *recpath_orig;
				recpath_orig=malloc(recpath_len+10);
				sprintf(recpath_orig, "%s-original", recpath);
				RLOG("testing if %s exists", recpath_orig);

				if (!libaroma_file_exists(recpath_orig)){
					RLOG("original file doesn't exist, patching and triggering recovery restart");
					int ret = replace_recovery(recpath, recpath_orig, recpath_len, argv[0], argv[3]);
					RLOG("replace_recovery returned %d", ret);
					free(recpath);
					free(recpath_orig);
					return 0;
				}
				else {
					RLOG("original file exists, skipping patch");
				}
			}
			else { //otherwise, just pause recovery
				libaroma_config()->runtime_monitor = LIBAROMA_START_MUTEPARENT;
			}
		}
		else {
			debug_tag = "I/LIBAROMA()";
			parent_pipe = stdout;
		}


		// enable this for bgr screen devices
		//libaroma_gfx_override_rgb(1, 0, 8, 16);

		/* load zip resource */
		RLOG("Loading zip from %s\n", argv[3]);
		zip=libaroma_zip(argv[3]);
		if (zip==NULL) {
			zip=libaroma_zip("/tmp/res.zip");
		}
		if (zip==NULL){
			RLOG("ZIP load failed\n");
		}
		else {
			printf("RLOGZIP load succeeded\n");
			/* init stream callback */
			libaroma_stream_set_uri_callback(stream_uri_cb);
		}
	}
	#endif
	init_libaroma();

	/* start full test */
	full_test();

	/* end libaroma process */
	libaroma_end();

	return 0;
} /* End of main */
#ifdef __cplusplus
}
#endif
#endif /* __libaroma_libaroma_test_c__ */
