#include "tools.h"

#ifdef __cplusplus
extern "C"{
#endif

LIBAROMA_ZIP zip;

int main(int argc, char ** argv){
	zip=libaroma_zip("/tmp/res.zip");
	libaroma_stream_set_uri_callback(stream_uri_callback);
	libaroma_start();
	libaroma_font(0, libaroma_stream("res:///roboto.ttf"));

	winmain();

	libaroma_end();

	return 0;
}

#ifdef __cplusplus
}
#endif
