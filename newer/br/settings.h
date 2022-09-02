/* default macros */
#define BR_LOG 3
#define BR_FUNCLOG
#define BR_APPNAME "BookReader"

/* settings struct */
typedef struct {
	byte slowmo;
	byte darkmode;
	byte scale;
	int anitime;
	int textsize;
	char *locale;
	char *version;
} BR_SETTINGS, * BR_SETTINGSP;

/* ui reload flag */
extern byte br_ui_needreload;

/* function prototypes */
extern BR_SETTINGSP br_settings();
extern byte br_settings_initfrag();
extern byte br_settings_updlocale();
extern void br_settings_load();
extern byte br_settings_save();
