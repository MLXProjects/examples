#include "common.h"

dword colors[28] = {
	(0x0),
	(0x808080),
	(0x8F0000),
	(0x81800A),
	(0x00821A),
	(0x008282),
	(0x00017F),
	(0x8E007D),
	(0x818041),
	(0x004141),
	(0x0082FF),
	(0x004180),
	(0x3E00FD),
	(0x8C3C00),
	(0xFFFFFF),
	(0xC0C0C0),
	(0xFF0000),
	(0xFFFF1F),
	(0x00FF3E),
	(0x00FFFF),
	(0x0004FD),
	(0xFF00FA),
	(0xFFFF83),
	(0x00FF8C),
	(0x06FFFF),
	(0x7C80FE),
	(0xFF0075),
	(0xFF782D)
};

void adraw_paint(int x, int y, word newcolor){
	word oldcolor = *(layer1->data + ((layer1->w*y)+x));
	if (oldcolor == newcolor) return;
	logi("painting area at %d, %d", x, y);
	ADRAW_STACKP stack = adraw_stack(layer1->w*layer1->h);
	if (!stack){
		loge("cannot alloc paint stack");
		return;
	}
	wordp dest = layer1->data;
	int x1, w=layer1->w, h=layer1->h;
	byte spanAbove, spanBelow;
	/* initialize the stack's first item */
	adraw_stack_push(stack, x, y);
	/* pop stack and get coords each time they're added */
	while(adraw_stack_pop(stack, &x, &y))
	{
		x1 = x;
		while(x1 >= 0 && dest[y * w + x1] == oldcolor) x1--;
		x1++;
		spanAbove = spanBelow = 0;
		while(x1 < w && dest[y * w + x1] == oldcolor)
		{
			dest[y * w + x1] = newcolor;
			if(!spanAbove && y > 0 && dest[(y - 1) * w + x1] == oldcolor)
			{
				adraw_stack_push(stack, x1, y - 1);
				spanAbove = 1;
			}
			else if(spanAbove && y > 0 && dest[(y - 1) * w + x1] != oldcolor)
			{
				spanAbove = 0;
			}
			if(!spanBelow && y < h - 1 && dest[(y + 1) * w + x1] == oldcolor)
			{
				adraw_stack_push(stack, x1, y + 1);
				spanBelow = 1;
			}
			else if(spanBelow && y < h - 1 && dest[(y + 1) * w + x1] != oldcolor)
			{
				spanBelow = 0;
			}
			x1++;
		}
	}
	adraw_stack_free(stack);
}

LIBAROMA_CANVASP adraw_toolicon(byte tool_id){
	switch (tool_id){
		case TOOL_PEN:
			return libaroma_image_uri("res:///icons/pen.svg");
		case TOOL_PAINT:
			return libaroma_image_uri("res:///icons/paint.svg");
		case TOOL_SELECT:
			return libaroma_image_uri("res:///icons/select.svg");
		case TOOL_CURSOR:
			return libaroma_image_uri("res:///icons/cursor.svg");
		case TOOL_ERASER:
			return libaroma_image_uri("res:///icons/eraser.svg");
		case TOOL_PICKER:
			return libaroma_image_uri("res:///icons/picker.svg");
		case TOOL_RECT:
			return libaroma_image_uri("res:///icons/rect.svg");
		case TOOL_CIRCLE:
			return libaroma_image_uri("res:///icons/circle.svg");
		case TOOL_LINE:
			return libaroma_image_uri("res:///icons/line.svg");
		case TOOL_TEXT:
			return libaroma_image_uri("res:///icons/text.svg");
		default:
			return NULL;
	}
}

/* stream uri callback */
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