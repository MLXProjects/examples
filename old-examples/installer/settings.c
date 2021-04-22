typedef struct{
	int show_seconds;
	int anim_speed;
	char *title;
	char *anim_type;
	char *anim_direction;
	byte title_italic;
	byte text_italic;
	byte console_italic;
	word back_color;
	word fore_color;
	word text_color;
	word console_back;
	word console_fore;
	word sb_text_color;
	word sb_override_color;
} SETTINGS, *SETTINGSP;

static SETTINGS _settings={0};

#define error_log(text) \
		printf("Could not get %s from settings! Using default\n", text);

SETTINGSP settings(){
	return &_settings;
}

char *prop_read(LIBAROMA_STREAMP stream, char *prop){
	printf("Reading prop %s\n", prop);	
	char *ret=libaroma_getprop(prop, stream, 0);
	printf("read finished: %s\n", ret);
	if (ret==NULL) return "null";
	return ret;
}

void config_read(LIBAROMA_ZIP zip){
	printf("Reading settings from aroma-config!\n");
	LIBAROMA_STREAMP stream=libaroma_stream("res:///aroma-config");
	
	// GUI SETTINGS LOADING
	_settings.anim_speed=atoi(prop_read(stream, "gui.anim_speed"));
	if (_settings.anim_speed<0){
		printf("Invalid animation speed! Using default: 300\n");
		_settings.anim_speed=300;
	}
	_settings.anim_type=prop_read(stream, "gui.anim_type");
	if (!strcmp(_settings.anim_type, "null") || !strcmp(_settings.anim_type, "default")){
		error_log("anim_type");
		_settings.anim_type="scale";
	}
	else if (!strcmp(_settings.anim_type, "slide") || !strcmp(_settings.anim_type, "page")){
		_settings.anim_direction=prop_read(stream, "gui.anim_direction");
		if (!strcmp(_settings.anim_direction, "null")){
			error_log("anim_direction");
			_settings.anim_direction="top";
		}
	}
	
	// WINDOW SETTINGS LOADING
	char *back_color=prop_read(stream, "win.back_color");
	if (!strcmp(back_color, "null") || !strcmp(back_color, "default")){
		error_log("back_color");
		back_color="#111111";
	}
	_settings.back_color=libaroma_rgb_from_string(back_color);
	char *fore_color=prop_read(stream, "win.fore_color");
	if (!strcmp(fore_color, "null") || !strcmp(fore_color, "default")){
		error_log("fore_color");
		fore_color="#ed1c24";
	}
	_settings.fore_color=libaroma_rgb_from_string(fore_color);
	char *text_color=prop_read(stream, "win.text_color");
	if (!strcmp(text_color, "null") || !strcmp(text_color, "default")){
		error_log("text_color");
		text_color=fore_color;
	}
	_settings.text_color=libaroma_rgb_from_string(text_color);
	char *sb_text_color=prop_read(stream, "sb.text_color");
	if (!strcmp(sb_text_color, "null") || !strcmp(sb_text_color, "default")){
		error_log("sb_text_color");
		sb_text_color="#ffffff";
	}
	_settings.sb_text_color=libaroma_rgb_from_string(sb_text_color);
	char *text_italic=prop_read(stream, "win.text_italic");
	if (!strcmp(text_italic, "yes")) _settings.text_italic=1;
	else _settings.text_italic=0;
	char *console_back=prop_read(stream, "win.console_back");
	if (!strcmp(console_back, "null") || !strcmp(console_back, "default")){
		error_log("console_back");
		console_back="#333333";
	}
	_settings.console_back=libaroma_rgb_from_string(console_back);
	char *console_fore=prop_read(stream, "win.console_fore");
	if (!strcmp(console_fore, "null") || !strcmp(console_fore, "default")){
		error_log("console_fore");
		console_fore="#ffffff";
	}
	_settings.console_fore=libaroma_rgb_from_string(console_fore);
	char *console_italic=prop_read(stream, "win.console_italic");
	if (!strcmp(console_italic, "yes")) _settings.console_italic=1;
	else _settings.console_italic=0;
	
	// STATUSBAR SETTINGS LOADING
	char *show_seconds=prop_read(stream, "sb.show_seconds");
	if (!strcmp(show_seconds, "no")){
		_settings.show_seconds=0;
	}
	else {
		_settings.show_seconds=1;
	}
	char *sb_override_color=prop_read(stream, "sb.override_color");
	byte sb_color_default=0;
	if (!strcmp(sb_override_color, "null") || !strcmp(sb_override_color, "none")){
		printf("Setting sb_override_color as default: fore_color\n");
		_settings.sb_override_color=_settings.fore_color;
		sb_color_default=1;
	}
	if (!sb_color_default) _settings.sb_override_color=libaroma_rgb_from_string(sb_override_color);
	_settings.title=prop_read(stream, "sb.title");
	if (!strcmp(_settings.title, "null")){
		_settings.title="";
	}
	char *title_italic=prop_read(stream, "sb.title_italic");
	if (!strcmp(title_italic, "yes")) _settings.title_italic=1;
	else _settings.title_italic=0;
	libaroma_stream_close(stream);
	stream=NULL;
	printf("Settings read! Values: \n"
			"back_color: %ld\n"
			"fore_color: %ld\n"
			"text_color: %ld\n"
			"console_back: %ld\n"
			"console_fore: %ld\n"
			"anim_speed: %d\n"
			"anim_type: %s\n"
			"anim_direction: %s\n", 
			_settings.back_color, _settings.fore_color, _settings.text_color, _settings.console_back, _settings.console_fore, _settings.anim_speed, _settings.anim_type, _settings.anim_direction);
			
	printf("STATUSBAR settings: \n"
			"title: %s\n"
			"title_italic: %d\n"
			"show_seconds: %d\n"
			"override_color: %ld\n",
			_settings.title, _settings.show_seconds, _settings.title_italic, _settings.sb_override_color 
			);
}