#include <aroma.h>
#include "bootanimation.c"
#define USER_BOOTANIMATION_FILE "/sdcard/res.zip"
#define SYSTEM_BOOTANIMATION_FILE "/system/media/bootanimation.zip"
LIBAROMA_STREAMP stream_uri_cb(char * uri);

LIBAROMA_ZIP zip=NULL;
LIBAROMA_ZIP zip_open(
		const char * filename) {
			
	LIBAROMA_ZIP zip = malloc(sizeof(ZipArchive));
	MemMapping map;
    if (sysMapFile(filename, &map) != 0) {
        printf("libaroma_zip failed to map package %s\n", filename);
        return 3;
    }
	if (mzOpenZipArchive(map.addr, map.length, (ZipArchive *) zip) != 0) {
		printf("libaroma_zip mzOpenZipArchive error (%s)", filename);
		free(zip);
		return NULL;
	}
	return zip;
} /* End of libaroma_zip */


int main(){
	printf("ZIP path: /system/media/bootanimation.zip\n");
		//zip = zip_open("/system/media/bootanimation.zip");
	if (zip == NULL){
		printf("ZIP open failed!!\n");
		printf("Second ZIP path: %s\n", "/sdcard/res.zip");
		zip = zip_open("/sdcard/res.zip");
		if (zip==NULL){
			printf("ZIP open failed!! Exiting.\n");
			return 0;
		}
	}
	printf("Okay\n");
	libaroma_start();
	libaroma_stream_set_uri_callback(stream_uri_cb);
	
	bootanimation(zip);
	
	libaroma_end();
	return 0;
}

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
