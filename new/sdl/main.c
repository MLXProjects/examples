#include <aroma.h>
#define LOGI(...) printf("D/MLX(): "); printf(__VA_ARGS__); printf("\n")

int main(int argc, char **argv){
	byte ret=0;
	ret=libaroma_start();
	if (!ret) goto exit;
	int whalf=libaroma_fb()->w/2, hhalf=libaroma_fb()->h/2;
	LIBAROMA_CANVASP cv = libaroma_canvas(whalf, hhalf);
	if (!cv) goto end;
	libaroma_canvas_fillcolor(cv, RGB(FFFFFF));
	libaroma_draw(libaroma_fb()->canvas, cv, whalf/2, hhalf/2, 0);
	/*if (libaroma_fb_start_post()){
		libaroma_fb_post(cv, whalf/2, hhalf/2, 0, 0, whalf, hhalf);
		libaroma_fb_end_post();
	}*/
	libaroma_fb_sync_area(whalf/2, hhalf/2, whalf, hhalf);
	//libaroma_fb_sync_area(0, 0, libaroma_fb()->w, libaroma_fb()->h);
	LOGI("Wait for me");
	libaroma_sleep(2000);
end:
	ret=libaroma_end();
exit:
	return ret;
}