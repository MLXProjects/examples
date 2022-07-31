#ifndef __br_main_h__
	#error "please include main.h instead of this"
#endif
#ifndef __br_settings_h__
#define __br_settings_h__

typedef struct {
	byte pager_ani;
	word fragani_flags;
	byte dark_mode;
	word vnum_color;
	word saved_bg;
	word saved_fg;
	LIBAROMA_COLORSETP colorset;
	int font_id;
	int font_size;
	long anim_duration;
} BR_SETTINGS, * BR_SETTINGSP;

extern BR_SETTINGSP settings();
extern byte settings_init();
extern byte settings_load();
extern byte settings_save();
extern byte settings_mkdefaults();
extern byte settings_dark_mode(byte enable, byte update);
extern byte settings_lang_load(char *locale);

#endif /* __br_settings_h__ */