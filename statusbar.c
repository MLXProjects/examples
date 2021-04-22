#include <aroma.h>
#include <time.h>

#define SB_SHOWING		1
#define SB_ACTIVE		2
#define SB_CLOSING		3
#define SB_CLOSED		4

int STATUSBAR_HEIGHT = 24;

typedef struct{
	char *title;
	LIBAROMA_CANVASP canvas;
	LIBAROMA_CANVASP temp_canvas;
	LIBAROMA_CANVASP prev_screen;
	word override_color;
	word orig_override_color;
	byte animating;
	byte status;
	byte alpha;
	int y;
	LIBAROMA_WINDOWP notif_pulldown;
	int pulldown_y;
} STATUSBAR, *STATUSBARP;

static STATUSBAR _statusbar={0};

STATUSBARP statusbar(){
	return &_statusbar;
}

char *getcurtime(/*byte show_seconds*/){
	time_t s, val = 1;
    struct tm* current_time;
    s = time(NULL);
    current_time = localtime(&s);
	char curtime[16];
    snprintf(curtime, 16, "%02d:%02d:%02d",
           current_time->tm_hour,
           current_time->tm_min,
           /*(!show_seconds)?NULL:*/current_time->tm_sec);
	char *string = malloc(16);
    strcpy(string, curtime);
	//printf("STATUSBAR: Current time: %s\n", string);
    return string;
}

char *getcurbatt(){
	FILE *fp;
    char str[16] = "";
    char str2[16] = "%";
    char* filename = "/sys/class/power_supply/battery/capacity";

    fp = fopen(filename, "r");
    if (fp == NULL){
        printf("STATUSBAR: Could not open file %s",filename);
		char *string = malloc(16);
		strcpy(string, "lol");
		return string;
    }
	fgets(str, 16, fp);
    fclose(fp);
	char *string = malloc(16);
	const char nl[4] = "\n";
    strtok(str, nl);
    strcat(str, str2);
    strcpy(string, str);
	//printf("STATUSBAR: Battery level: %s", string);
	//snprintf(string, "%");
    return string;
}

int msleep(long msec){
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

	nanosleep(&ts, &ts);

    return 0;
}

byte statusbar_draw();

void statusbar_start_thread(){
	printf("STATUSBAR: Setting UI thread\n");
	libaroma_wm_set_ui_thread(statusbar_draw);
}

void statusbar_end_thread(){
	printf("STATUSBAR: Removing UI thread\n");
	libaroma_wm_set_ui_thread(NULL);
}

byte statusbar_draw(){
	_statusbar.temp_canvas = libaroma_canvas(_statusbar.canvas->w, _statusbar.canvas->h);

	//if (_statusbar.status==SB_CLOSING || _statusbar.status==SB_SHOWING){
		libaroma_draw_ex(_statusbar.temp_canvas, _statusbar.prev_screen, 0, 0, 0, 0,
					 _statusbar.temp_canvas->w, _statusbar.temp_canvas->h, 0, 0xFF);
	//}

	if (_statusbar.override_color){
		libaroma_gradient_ex(_statusbar.temp_canvas, 0, _statusbar.y, _statusbar.canvas->w, _statusbar.canvas->h,
							 RGB(000000), _statusbar.override_color, (_statusbar.alpha==-1)?-1:0, 0, 0xFF, _statusbar.alpha);
	} else {
		word color;
		if (libaroma_wm_get_active_window() != NULL){
		byte hasAppbar = libaroma_wm_get_active_window()->appbar_bg!=NULL;
		color = hasAppbar?libaroma_wm_get_active_window()->appbar_bg:libaroma_colorget(NULL,
							libaroma_wm_get_active_window())->window_bg;
		}
		else {
			color = RGB(FFFFFF);
		}

		libaroma_gradient_ex(_statusbar.temp_canvas, 0, _statusbar.y, _statusbar.temp_canvas->w, _statusbar.temp_canvas->h,
							 RGB(000000), color, (_statusbar.alpha==-1)?-1:0, 0, 0xFF, _statusbar.alpha);
	}

	libaroma_draw_text(
		_statusbar.temp_canvas,
		getcurtime(),
		0, _statusbar.y, RGB(FFFFFF), _statusbar.temp_canvas->w,
		LIBAROMA_FONT(0,3)|
		LIBAROMA_TEXT_SINGLELINE|
		LIBAROMA_TEXT_LEFT,
		140);

	libaroma_draw_text(
		_statusbar.temp_canvas,
		_statusbar.title,
		0, _statusbar.y, RGB(FFFFFF), _statusbar.temp_canvas->w,
		LIBAROMA_FONT(0,3)|
		LIBAROMA_TEXT_SINGLELINE|
		LIBAROMA_TEXT_CENTER,
		140);
/*
	libaroma_draw_text(
		_statusbar.temp_canvas,
		getcurbatt(),
		0, _statusbar.y, RGB(FFFFFF), _statusbar.temp_canvas->w,
		LIBAROMA_FONT(0,3)|
		LIBAROMA_TEXT_SINGLELINE|
		LIBAROMA_TEXT_RIGHT,
		140);*/

	//printf("Drawing to canvas at y: %d with w: %d, h: %d\n", _statusbar.y, _statusbar.temp_canvas->w, _statusbar.temp_canvas->h);
	libaroma_draw_ex(_statusbar.canvas, _statusbar.temp_canvas, 0, 0/*_statusbar.y*/, 0, 0,
					 _statusbar.temp_canvas->w, _statusbar.temp_canvas->h, 0, 0xFF);
	//if (!_statusbar.animating) msleep(500);
	//printf("Alpha: %d\n", _statusbar.alpha);
	if (_statusbar.notif_pulldown && _statusbar.pulldown_y>0){
		printf("Drawing pulldown over statusbar at y=%d!\n", _statusbar.notif_pulldown->h-_statusbar.pulldown_y);
		libaroma_draw_ex(_statusbar.canvas, _statusbar.notif_pulldown->dc, 0, 0, 0,
				_statusbar.notif_pulldown->h-_statusbar.pulldown_y-_statusbar.canvas->h,
				_statusbar.canvas->w, _statusbar.canvas->h, 0, 0xFF);
	}
	else if (_statusbar.override_color != _statusbar.orig_override_color)
		_statusbar.override_color=_statusbar.orig_override_color;
}

void statusbar_animate(){
	if (libaroma_wm()->ui_thread==NULL) statusbar_start_thread();
	_statusbar.animating=1;
	while (_statusbar.animating){
		switch (_statusbar.status){
			case SB_SHOWING:
				{
					if (_statusbar.y<0){
						_statusbar.y++;
						msleep(5);
					}
					else {
						_statusbar.animating=0;
						_statusbar.status=SB_ACTIVE;
					}
				}
				break;
			case SB_CLOSING:
				{
					if (_statusbar.y>-_statusbar.canvas->h){
						_statusbar.y--;
						msleep(5);
					} else {
						printf("STATUSBAR: Closed\n");
						_statusbar.animating=0;
						_statusbar.status=SB_CLOSED;
					}
				}
				break;
		}
	}
}

void statusbar_init(word override_bg, char *title){
	printf("STATUSBAR: Init!\n");
	_statusbar.orig_override_color = override_bg;
	_statusbar.override_color=override_bg;
	_statusbar.prev_screen = libaroma_fb_snapshoot_canvas();
	printf("STATUSBAR: Creating canvas\n");
	_statusbar.canvas = libaroma_canvas_area(
		libaroma_fb()->canvas,
		0, 0, libaroma_fb()->w, STATUSBAR_HEIGHT);
	_statusbar.y = -(STATUSBAR_HEIGHT);
	_statusbar.animating = 1;
	_statusbar.status=SB_SHOWING;
	_statusbar.alpha = 0xFF;
	printf("STATUSBAR: Setting title: %s\n", title);
	if (title!=NULL) _statusbar.title=title;
	printf("STATUSBAR: Setting UI temporary thread\n");
	libaroma_wm_set_ui_thread(statusbar_draw);
	statusbar_animate();
	statusbar_end_thread();
}

void statusbar_close(){
	if (libaroma_wm()->ui_thread==NULL) statusbar_start_thread();
	printf("STATUSBAR: Closing!\n");
	_statusbar.animating=1;
	_statusbar.status=SB_CLOSING;
	statusbar_animate();
	//while (_statusbar.status == SB_CLOSING) { msleep(50); }
	libaroma_wm_set_ui_thread(NULL);
	printf("STATUSBAR: Closed, freeing resources\n");
	if (_statusbar.status == SB_CLOSED) {
		libaroma_canvas_free(_statusbar.canvas);
		libaroma_canvas_free(_statusbar.temp_canvas);
	printf("STATUSBAR: Freed\n");
	}
}
