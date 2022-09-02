#include "common.h"

int main(int argc, char **argv){
	int ret;
	TLOG("Test startup");

	//libaroma_config()->runtime_monitor = LIBAROMA_START_SAFE;

	//custom rgb for bgr devices
	libaroma_gfx_override_rgb(1, 0, 8, 16);

	//start libaroma runtime loop
	ret = lart_start(argv, app_run, sysui_handler, statusbar_ondraw);

	TLOG("Test finished");
	return ret;
}
