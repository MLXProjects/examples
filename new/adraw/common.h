#ifndef __adraw_common_h__
#define __adraw_common_h__
#include <aroma.h>
#include "types.h"
#include "funcs.h"

#define DEFAULT_PRICOLOR RGB(FF0000)
#define DEFAULT_SECCOLOR RGB(FFFFFF)

/* per-tool specific ids */
#define TOOL_RECT_FILL		160
#define TOOL_RECT_EMPTY		161

#define TOOL_CIRCLE_FILL	170
#define TOOL_CIRCLE_EMPTY	171

/* global variables */
extern LIBAROMA_ZIP zip;
extern word pri_cl, sec_cl;
extern byte sel_second;
extern enum tool curtool;
extern dword colors[28];
extern LIBAROMA_WINDOWP win;
extern LIBAROMA_CANVASP canvas, layer1, layer2;
extern LIBAROMA_CONTROLP image, alpha_label;
extern int eraser_size;
extern byte rect_type, circle_type;
extern byte color_alpha;

#endif //__adraw_common_h__