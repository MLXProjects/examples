#ifndef __common_h__
#define __common_h__

#include <aroma.h>
#include <aromart.h>
#include <string.h>

/* debug */
#define TEST_USE_DEBUG "I/RT_TEST()"
#ifdef TEST_USE_DEBUG
#define TLOG(...) \
  fprintf(stdout, TEST_USE_DEBUG ": "); \
  fprintf(stdout, __VA_ARGS__); \
  fprintf(stdout, "\n");
#else
#define TLOG(...)
#endif

extern LIBAROMA_STREAMP stream_uri_cb(char * uri);
extern int app_run(LART_APP *app);
extern void sysui_handler();
extern void statusbar_ondraw(LIBAROMA_CANVASP cv,word fgcolor);

#endif /* __common_h__ */
