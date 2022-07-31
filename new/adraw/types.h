#ifndef __adraw_types_h__
#define __adraw_types_h__

enum tool {
	TOOL_PEN,
	TOOL_PAINT,
	TOOL_SELECT,
	TOOL_CURSOR,
	TOOL_ERASER,
	TOOL_PICKER,
	TOOL_RECT,
	TOOL_CIRCLE,
	TOOL_LINE,
	TOOL_TEXT,
	TOOL_COLOR1,
	TOOL_COLOR2
};

typedef struct {
	int x;
	int y;
} ADRAW_POINT, *ADRAW_POINTP;

typedef struct {
	ADRAW_POINTP *items;
	int count;
	int max;
} ADRAW_STACK, *ADRAW_STACKP;

#endif //__adraw_types_h__