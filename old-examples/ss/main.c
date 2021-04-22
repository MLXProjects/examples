#include <aroma.h>

int main(int argc, char ** argv){
	libaroma_start();
	
	while (1) { 
		libaroma_png_save(libaroma_fb()->canvas, "/tmp/asd.png");
		libaroma_sleep(5000); 
	}
	
	libaroma_end();
	
	return 0;
}