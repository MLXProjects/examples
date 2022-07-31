#include "asd.h"

int main(int argc, char ** argv){
	logi("Starting %s v1.0", PROG_NAME);
	
	if (!libaroma_start()) return;
	winmain();
	libaroma_end();
	return 0;
}