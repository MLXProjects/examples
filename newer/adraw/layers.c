#include "common.h"

void adraw_cv_update(int x, int y, int w, int h, byte blend){
	switch (curtool){
		case TOOL_PAINT:{
			libaroma_draw(canvas, layer1, 0, 0, 0);
		} break;
		case TOOL_ERASER:{
			libaroma_draw_ex(canvas, layer2, x, y, x, y, w, h, 0, 0xFF);
		} break;
		case TOOL_CIRCLE:
		case TOOL_RECT:{
			libaroma_draw(canvas, layer1, 0, 0, 0);
			libaroma_draw_ex(canvas, layer2, x, y, x, y, w, h, 0, 0xFF);
		} break;
	}
	if (blend) libaroma_draw_ex(layer1, layer2, x, y, x, y, w, h, 0, 0xFF);
	libaroma_ctl_image_update(image);
}