#include "../../libs/minzip/Zip.h"
byte debug=1;

typedef struct{
	char *path;
} FRAME, *FRAMEP;

typedef struct{
	byte playUntilComplete;
	int pause;
	int framecount;
	int count;
	FRAMEP frames[1024]
} PART, *PARTP;

typedef struct{
	int w;
	int h;
	int fps;
	int partcount;
	PARTP parts[64];
} ANIM, *ANIMP;

static ANIMP anim=NULL;
char* LastcharDel(char* name)
{
    int i = 0;
    while(name[i] != '\0')
    {
        i++;
         
    }
    name[i-1] = '\0';
    return name;
}
void bootanimation(LIBAROMA_ZIP zip){/*
	if (debug) printf("\n");
	*/
	if (debug) printf("Starting bootanimation function...\n");
	
	if (debug) printf("Bootanimation parsing file!\n");	
	char * buffer=libaroma_stream_to_string(libaroma_stream("res:///desc.txt"),0);
	if (buffer==NULL) {
		if (debug) printf("Cannot open desc.txt from zip!! Aborting...\n");
	}
	char * result = NULL;
	char *saveptr1;
	char * line = strtok_r(buffer, "\n", &saveptr1);
	if (debug) printf("desc.txt opened, let's prepare the structs\n");
	anim=(ANIMP) malloc(sizeof(ANIM));
	int pathcount=0;
	do {
		if (*line == '\0' || *line == '#') {
			continue;
		}
		
		int fps, width, height, count, pause;
        char path[256];
        char pathType;
        if (sscanf(line, "%d %d %d", &width, &height, &fps) == 3) {
            if (debug) printf("PARSER GOT SETTINGS w=%d, h=%d, fps=%d\n", width, height, fps);
            anim->w = libaroma_dp(width);
            anim->h = libaroma_dp(height);
            anim->fps = fps;
        }
        else if (sscanf(line, " %c %d %d %s", &pathType, &count, &pause, path) == 4) {
            PARTP part = (PARTP) malloc(sizeof(PART));
            part->playUntilComplete = (pathType == 'c');
            part->pause = pause;
			part->count = count;
			part->framecount=0;
            //animation.parts.add(part);
			anim->parts[pathcount]=part;
            if (debug) printf("PARSER GOT PART number %d, type=%d, count=%d, pause=%d, framecount=%d\n", 
					pathcount, anim->parts[pathcount]->playUntilComplete, anim->parts[pathcount]->count, 
					anim->parts[pathcount]->pause, anim->parts[pathcount]->framecount);
			pathcount++;
        }
	}
	while ((line = strtok_r(NULL, "\n", &saveptr1)));
	anim->partcount=pathcount;
	
	const ZipArchive * zipfile = (ZipArchive *) zip;
	int numEntries = zipfile->numEntries;
	//if (debug) printf("Number of entries inside zip file: %d\n", numEntries);
	
	if (debug) printf("Settings gathered: \n"
			"Animation with w %d and h %d at %dfps "
			"has %d parts\n", anim->w, anim->h, anim->fps, anim->partcount);
/*
	if (framecount>0){
		int j;
		for (j=0; j<framecount; j++){
			PARTP frame=anim->parts[j];
			if (debug) printf("Frame %d plays %d times and has a delay of %d parts after it\n", j, frame->count, frame->pause);
		}
	}
	*/
	size_t i;
	int curpart=-1;
	byte check_for_desc=1;
	int curframe=0;
	for (i=0; i<numEntries; i++){
		ZipEntry *entry = zipfile->pEntries + i;
		if (entry!=NULL){
			//if (debug) printf("Parsing file entry\n");
			const char *prefix="res:///";
			char *str=malloc(strlen(entry->fileName)-3+strlen(prefix));
			strcpy(str, prefix);
			strncat(str, entry->fileName, strlen(entry->fileName)-3);
			char *suffix="/";
			int str_len = strlen(str);
			int suffix_len = strlen(suffix);
			
			if (check_for_desc){
				if (0==strcmp(str, "res:///desc.txt")){
					check_for_desc=0;
					free(str);
					continue;
				}
			}
			if (0 == strcmp(str + (str_len-suffix_len), suffix)){
					curpart++;
					curframe=0;
					free(str);
					continue;
			}
            FRAMEP frame = (FRAMEP) malloc(sizeof(FRAME));
			frame->path=str;
			//if (debug) printf("Adding %s to part %d, part framecount is %d...\n", str, curpart, anim->parts[curpart]->framecount);
			if (debug) printf("anim->parts[%d]->frames[%d]->path=%s\n", curpart, curframe, str);
			anim->parts[curpart]->frames[curframe]=frame;
			anim->parts[curpart]->framecount++;
			curframe++;
		}
	}
	if (debug) printf("Bootanimation started to draw frames\n");
	drawframes(anim);
	if (debug) printf("Bootanimation finished\n");
	//while (1) {};
}

void drawframes(ANIMP anim){
	if (debug) printf("Creating window\n");
	LIBAROMA_WINDOWP win = libaroma_window(
		NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
	if (debug) printf("Creating canvas\n");
	LIBAROMA_CANVASP canvas=libaroma_canvas(libaroma_px(win->w), libaroma_px(win->h));
	if (debug) printf("Creating image control\n");
	LIBAROMA_CONTROLP imagectl = libaroma_ctl_image_canvas(win, 1, canvas, 0, 0, libaroma_px(win->w), libaroma_px(win->h));
	byte shown=0;
	int waitms=1000/anim->fps;//16;
	int curpart;
	if (debug) printf("Starting part/frame loop with %dms delay between frames...\n", waitms);
	for (curpart=0; curpart<anim->partcount; curpart++){
		PARTP part=anim->parts[curpart];
		if (part==NULL){
			if (debug) printf("PART GETTING FAILED!!\n");
			continue;
		}
		if (debug) printf("curpart: %d\n", curpart);
		int loopsPassed=0;
		int curframe;
		for (curframe=0; curframe<part->framecount; curframe++){
			if (debug) printf("anim->parts[%d]->frames[%d] from framecount %d\n", curpart, curframe, part->framecount);
			FRAMEP frame=part->frames[curframe];
			if (frame==NULL){
				if (debug) printf("FRAME GETTING FAILED!!\n");
				continue;
			}
			if (frame->path==NULL) {
				if (debug) printf("FRAME HAD NULL PATH!\n");
				continue;
			}
			if (debug) printf("Frame obtained at path %s\n", frame->path);
			LIBAROMA_CANVASP img=libaroma_image_uri(frame->path);
			if (img==NULL){
				if (debug) printf("path isn't a valid image\n");
				free(frame->path);
				frame->path=NULL;
				continue;
			}
			if (debug) printf("frame loaded\n");
			libaroma_canvas_blank(canvas);
			if (debug) printf("Canvas cleaned\n");
			libaroma_draw_scale_smooth(canvas, img, (canvas->w/2) - (anim->w/2), (canvas->h/2) - (anim->h/2), anim->w, anim->h,
								0, 0, img->w, img->h);
			if (debug) printf("Frame drawn\n");
			libaroma_ctl_image_set_canvas(imagectl, canvas, 1);
			if (debug) printf("Image shown\n");
			if (!shown) {
				if (debug) printf("Window shown\n");
				libaroma_wm_set_active_window(win);
				shown=1;
			}
			if (debug) printf("Frame draw finished, waiting %d ms...\n", waitms);
			libaroma_sleep(waitms);
			if (curframe==part->framecount-1 && part->count==0) curframe=0;
		}
	}
}