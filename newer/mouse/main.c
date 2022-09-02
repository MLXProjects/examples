#include "common.h"

LIBAROMA_ZIP zip;

int main(int argc, char** argv){
	libaroma_sdl_startup_size(SDL_WIDTH, SDL_HEIGHT);
	libaroma_gfx_startup_dpi(160);
	libaroma_config()->sdl_mousemove=1;
	if (!libaroma_start()){
		printf("libaroma fail\n");
		return 0;
	}
	uimain();
	return libaroma_end();
}