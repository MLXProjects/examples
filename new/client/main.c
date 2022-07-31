#include <aroma.h>
#include <sys/mman.h>

typedef struct {
	int w;
	int h;
	int pixel_size;
	byte isbgr;
	unsigned char* data;
} ASHMEM_CANVAS, * ASHMEM_CANVASP;

FILE *parent_pipe;
#define mlx_sendcmd(...) fprintf(parent_pipe, __VA_ARGS__); fprintf(parent_pipe, "\n"); fflush(parent_pipe)

ASHMEM_CANVASP ashmem_cv=NULL;
LIBAROMA_CANVASP shmem_cv=NULL;
bytep memfile=NULL;
static int shmem_fd=-1;
static int shmem_sz=0;

/* protos */
byte mlx_ashmem_fb_init(LIBAROMA_FBP me);

/* Framebuffer Driver */
void mlx_fb_release(LIBAROMA_FBP me){
	mlx_sendcmd("ui_print shmem unmap & close, bye");
	if (memfile!=NULL && memfile!=MAP_FAILED) munmap(memfile, shmem_sz);
	if (shmem_fd!=-1) close(shmem_fd);
	if (shmem_cv!=NULL) free(shmem_cv);
}
byte mlx_fb_startpost(LIBAROMA_FBP me){ return 1; }
void libaroma_btl32(int n, dwordp dst, const wordp src) {
	int i;
	for (i = 0; i < n; i++) {
		dst[i] = libaroma_rgb_to32(src[i]);
	}
}
byte mlx_fb_post(
	LIBAROMA_FBP me, wordp __restrict sc,
	int dx, int dy, int dw, int dh,
	int sx, int sy, int sw, int sh
	){
	//mlx_sendcmd("fb_update %d, %d (%dx%d) -> %d, %d (%dx%d)\n", sx, sy, sw, sh, dx, dy, dw, dh);
	/*int sstride = (shmem_cv->w - ashmem_cv->w) * 2;
	int dstride = ((shmem_cv->l) - (ashmem_cv->w * ashmem_cv->pixel_size));
	
	int dline = ashmem_cv->w+(dstride>>2);
	int sline = ashmem_cv->w+(sstride>>1);
	dwordp dst=(dwordp)ashmem_cv->data;
	wordp src=(wordp)shmem_cv->data;
	*/
	int i, j;
	int maxw=shmem_cv->w*ashmem_cv->pixel_size;
	bytep dest = (bytep)ashmem_cv->data;
#ifdef LIBAROMA_CONFIG_OPENMP
	#pragma omp parallel for
#endif
	for (i = 0; i < shmem_cv->h; i++) {
		for (j=0; j<maxw; j++){
			memset(dest, 0xFF, 1);
			//dst[(shmem_cv->w*i)+j]=libaroma_rgb_to32(src[(shmem_cv->h*i)+j]);
		}
	}
	mlx_sendcmd("fb_update\n");
	return 1;
}
byte mlx_fb_endpost(LIBAROMA_FBP me){
	return 1;
}

int main(int argc, char **argv){
	
	LIBAROMA_CANVASP app_cv=libaroma_canvas_shmem_open("/tmp/.mlx-shmem");
	if (!app_cv){
		printf("nope\n");
		return 1;
	}
	libaroma_png_save(app_cv, "/tmp/saved.png");
	return 0;
	if (argc<2){
		printf("Client: not today\n");
		return 1;
	}
	printf("Client: argc=%d\n"
			"path=%s"
			"mmap path=%s\n", argc, argv[0], argv[1]);
	libaroma_debug_set_output(NULL);
	parent_pipe = fdopen(atoi(argv[2]), "wb");
	if (parent_pipe==-1){
		printf("Client: failed to open pipe\n");
		return 1;
	}
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
	/*shmem_fd = atoi(argv[2]);
	if (shmem_fd<0){
		mlx_sendcmd("ui_print failed to get shmem (%d - %s)", shmem_fd, argv[2]);
		return 1;
	}
	mlx_sendcmd("ui_print shmem_fd=%d", shmem_fd);
	ASHMEM_CANVASP mem_cv;
	//mlx_sendcmd("ui_print shmem opened");
	// first-map shmem with struct size to retrieve info 
	mlx_sendcmd("ui_print mapping shmem with temporary size %d", sizeof(ASHMEM_CANVAS));
	memfile=(bytep) mmap(NULL, sizeof(ASHMEM_CANVAS), PROT_READ | PROT_WRITE, MAP_SHARED, shmem_fd, 0);
	if (memfile==MAP_FAILED){
		mlx_sendcmd("ui_print file mmap failed");
		close(shmem_fd);
		return 1;
	}
	mlx_sendcmd("ui_print shmem mapped (%p)", memfile);
	mem_cv=(ASHMEM_CANVASP) memfile;
	mlx_sendcmd("ui_print shmem casted (%p)", mem_cv);
	mlx_sendcmd("ui_print shmem info loaded (%dx%d - %dbpp)", mem_cv->w, mem_cv->h, mem_cv->pixel_size*8);
	shmem_sz=mem_cv->w*mem_cv->h*mem_cv->pixel_size;
	munmap(memfile, sizeof(ASHMEM_CANVAS));
	mlx_sendcmd("ui_print shmem remap with real size %d", shmem_sz);
	memfile=(bytep) mmap(NULL, shmem_sz, PROT_READ | PROT_WRITE, MAP_SHARED, shmem_fd, 0);
	if (memfile==MAP_FAILED){
		mlx_sendcmd("ui_print shmem remap failed");
		close(shmem_fd);
		return 1;
	}
	mlx_sendcmd("ui_print shmem remap done");
	ashmem_cv=(ASHMEM_CANVASP) memfile;*/
	//close(shmem_fd);
	//return 0;
	//libaroma_fb_set_initializer(mlx_ashmem_fb_init);
	mlx_sendcmd("gui_stop");
	if (!libaroma_start()){
		mlx_sendcmd("ui_print libaroma_start failed");
		mlx_sendcmd("gui_resume");
		munmap(memfile, shmem_sz);
		close(shmem_fd);
		return 1;
	}
	
	LIBAROMA_WINDOWP win = libaroma_window(NULL, 0, 0, 0, 0);
	if (!win) goto fail;
	libaroma_ctl_progress(win, 1, (win->width/2)-24, 24, 48, 48, LIBAROMA_CTL_PROGRESS_INDETERMINATE|LIBAROMA_CTL_PROGRESS_CIRCULAR, 0, 1);
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
		
		if (msg.msg==LIBAROMA_MSG_EXIT || msg.msg==LIBAROMA_MSG_KEY_SELECT || msg.msg==LIBAROMA_MSG_KEY_POWER){
			if (msg.state==0){
				win->onpool = 0;
				break;
			}
		}
	} while (win->onpool);
	libaroma_window_aniclose(win, 12, 400);
fail:
	libaroma_end();
	mlx_sendcmd("gui_resume");
	return 0;
}

byte mlx_ashmem_fb_init(LIBAROMA_FBP me){
	/*shmem_cv=(LIBAROMA_CANVASP)calloc(sizeof(LIBAROMA_CANVAS), 1);
	shmem_cv->w=shmem_cv->l=ashmem_cv->w;
	shmem_cv->h=ashmem_cv->h;
	shmem_cv->s=ashmem_cv->w*ashmem_cv->h;
	shmem_cv->data=(wordp)ashmem_cv->data;
	shmem_cv->alpha=NULL;
	shmem_cv->hicolor=NULL;
	shmem_cv->flags=0;*/
	shmem_cv=libaroma_canvas(ashmem_cv->w, ashmem_cv->h);
	if (!shmem_cv){
		mlx_sendcmd("ui_print canvas alloc failed\n");
		return 0;
	}
	me->internal			= NULL;
	me->release				= &mlx_fb_release;
	me->w					= shmem_cv->w;
	me->h					= shmem_cv->h;
	me->sz					= me->w*me->h;
	me->start_post			= &mlx_fb_startpost;
	me->end_post			= &mlx_fb_endpost;
	me->post				= &mlx_fb_post;
	me->snapshoot			= NULL;
	me->config				= NULL;
	me->dpi					= 160;
	me->double_buffer	 	= 1;
	me->internal_canvas 	= 1;
	me->canvas				= shmem_cv;
	return 1;
}