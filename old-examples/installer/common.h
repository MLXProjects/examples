#define FONT_LIGHT				0
#define FONT_LIGHT_ITALIC		1
#define FONT_MONOSPACE			2
#define FONT_MONOSPACE_ITALIC	3

#include "settings.c"

LIBAROMA_CANVASP custitem_ex(LIBAROMA_CANVASP icon, char *text, char *extra,
								word bg_color, word accent, int width, int height, word flags);
word main_bgcolor;
word main_accent;
					
int screen_issmall=1;
int debug=1;


#define CUSTITEM_ITALIC				0x20
#include "custitem.c"
#include "statusbar.c"
#include "../navbar.c"
#include "tools.c"
#include "lockscreen.c"

#include "install.c"
