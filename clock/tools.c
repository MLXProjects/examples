#include "tools.h"

#ifdef __cplusplus
extern "C"{
#endif
clock_t benchmarker;

void clock_init_ex(LIBAROMA_WINDOWP win, int x){
	LIBAROMA_CONTROLP clock = libaroma_ctl_clock(win, 1, x, (libaroma_fb()->h-libaroma_fb()->w)/2,
			libaroma_fb()->w, libaroma_fb()->w, NULL);
	libaroma_ctl_clock_set_color(clock, LIBAROMA_CTL_CLOCK_COLOR_HOUR, RGB(FF0000));
	libaroma_ctl_clock_set_color(clock, LIBAROMA_CTL_CLOCK_COLOR_MINUTE, RGB(FF00FF));
	libaroma_ctl_clock_set_color(clock, LIBAROMA_CTL_CLOCK_COLOR_SECOND, RGB(FFFF00));
	libaroma_ctl_clock_set_color(clock, LIBAROMA_CTL_CLOCK_COLOR_CENTER, RGB(FFFFFF));
	libaroma_ctl_clock_disable_border(clock);
	LIBAROMA_CANVASP bg=libaroma_image_uri("res:///clock.png");
	libaroma_ctl_clock_set_bg(clock, bg);
	libaroma_canvas_free(bg);
}

/*
 * Function		: insidecontrol
 * Return Value	: int
 * Descriptions	: check if given coords are inside the bounds of the given control
 */
int insidecontrol(LIBAROMA_CONTROLP ctl, int x, int y, int pager_offset){
	if (ctl == NULL)
		return 0;
	if (((x >= (ctl->x-pager_offset)) &&
		 (y >= ctl->y)) &&
		((x <= (ctl->x-pager_offset) + ctl->w) &&
		 (y <= ctl->y + ctl->h)))
		return 1;
	return 0;
}

int get_brightness(){
	ALOG("Trying to get brightness");
	FILE *fp;
	fp = fopen("/sys/class/backlight/panel/brightness", "r");
	if (fp==NULL)
		fp = fopen("/sys/class/leds/lcd-backlight/brightness", "r");
		if (fp==NULL) return;
	char str[16];
	char line[16];
	if (fgets(str, 16, fp) == NULL){
		free(str);
		return -1;
	}
	int value;
	sscanf(str, "%d", &value);

	fclose(fp);
	printf("Returning current brightness %d\n", value);
	return value;
}

void slide_brightness(int x, int offset){
	int value=140;
	value=x-offset+26;
	if (value>255) value=255;
	if (value<0) value=0;
	//value=value-52;
	printf("New brightness should be %d\n", value);
	set_brightness(value);
}

void set_brightness(int value){
	FILE *fp;
	fp = fopen("/sys/class/backlight/panel/brightness", "w");
	if (fp==NULL)
		fp = fopen("/sys/class/leds/lcd-backlight/brightness", "w");
		if (fp==NULL) return;
	fprintf(fp, "%d", value);
	fclose(fp);
}

word rand_color(){
	byte r = (byte)((rand() %
           (255 - 1 + 1)) + 1);
	byte g = (byte)((rand() %
           (255 - 1 + 1)) + 1);
	byte b = (byte)((rand() %
           (255 - 1 + 1)) + 1);
	return libaroma_rgb(r, g, b);
}

double c_benchmark(char *func){
	if (0==strcmp(func, "")){ //start time benchmark
		benchmarker=clock();
	}
	else if (benchmarker!=NULL) { //output time benchmark result
		ALOG("benchmarker: function %s() took %f seconds until now", func, ((double)(clock() - benchmarker))/CLOCKS_PER_SEC);
	}
	return ((double)(clock() - benchmarker))/CLOCKS_PER_SEC;
}

LIBAROMA_STREAMP stream_uri_callback(char * uri){
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

int MAX(int val1, int val2){
	return (val1>val2)?val1:val2;
}

#ifdef __cplusplus
}
#endif
