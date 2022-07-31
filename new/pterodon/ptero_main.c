#ifndef __ptero_main_c__
#define __ptero_main_c__
#ifdef __cplusplus
extern "C" {
#endif
#include "common.h"

static PTERODON _pterodon={0};

LIBAROMA_STREAMP stream_uri_cb(char * uri);

PTERODONP pterodon(){
	return &_pterodon;
}

/*
 * Function		: ptero_main
 * Parameters	: none
 * Return type	: byte
 * Description	: start main pterodon loop
 */
byte ptero_main(){
	pterodon()->running=1;
	printf("MLX: ************HASH TABLE************\n"
			"exit= %lu\n"
			"true= %lu\n"
			"false= %lu\n"
			"name= %lu\n"
			"value= %lu\n"
			"arrow= %lu\n"
			"drawer= %lu\n"
			"set= %lu\n"
			"exec= %lu\n"
			"show= %lu\n"
			"hide= %lu\n"
			"actionset= %lu\n"
			"sidebar= %lu\n"
			"appbar= %lu\n"
			"list= %lu\n"
			"check= %lu\n"
			"switch= %lu\n"
			"option= %lu\n"
			"caption= %lu\n"
			"divider= %lu\n"
			"menu= %lu\n"
			"image= %lu\n"
			"button= %lu\n"
			"fragment= %lu\n"
			"window= %lu\n"
			"pager= %lu\n"
			"tab_manager= %lu\n"
			"colorset= %lu\n"
			"if= %lu\n"
			"else= %lu\n"
			"elif= %lu\n"
			"equal_to= %lu\n"
			"greater_than= %lu\n"
			"smaller_than= %lu\n"
			"grater_or_equal_to= %lu\n"
			"smaller_or_equal_to= %lu\n"
			"different_to= %lu\n"
			"************END HASHES************\n", 
			libaroma_hash("exit"),
			libaroma_hash("true"),
			libaroma_hash("false"),
			libaroma_hash("name"),
			libaroma_hash("value"),
			libaroma_hash("arrow"),
			libaroma_hash("drawer"),
			libaroma_hash("set"),
			libaroma_hash("exec"),
			libaroma_hash("show"),
			libaroma_hash("hide"),
			libaroma_hash("actionset"),
			libaroma_hash("sidebar"),
			libaroma_hash("appbar"),
			libaroma_hash("list"),
			libaroma_hash("check"),
			libaroma_hash("switch"),
			libaroma_hash("option"),
			libaroma_hash("caption"),
			libaroma_hash("divider"),
			libaroma_hash("item"),
			libaroma_hash("image"),
			libaroma_hash("button"),
			libaroma_hash("fragment"),
			libaroma_hash("window"),
			libaroma_hash("pager"),
			libaroma_hash("tab_manager"),
			libaroma_hash("colorset"),
			libaroma_hash("if"),
			libaroma_hash("else"),
			libaroma_hash("elif"),
			libaroma_hash("equal_to"),
			libaroma_hash("greater_than"),
			libaroma_hash("smaller_than"),
			libaroma_hash("greater_or_equal_to"),
			libaroma_hash("smaller_or_equal_to"),
			libaroma_hash("different_to")
	);
	
	return ptero_gui_main();
}

/*
 * Function		: ptero_init
 * Parameters	: program arg count & array
 * Return type	: byte
 * Description	: initialize settings, aroma & resources
 */
byte ptero_init(int argc, char **argv){
	/* load zip to memory */
	global_zip = libaroma_zip(argv[(argc>3)?3:1]);
	if (!global_zip) return 0;
	/* load settings, layout, etc... */
	//ptero_settings_load();
	libaroma_stream_set_uri_callback(ptero_stream_uri_cb);
	//libaroma_sdl_startup_size(400, 900);
	libaroma_config_cursor_path("res:///touch.png");
	libaroma_config_force_cursor();
	/* init libaroma engine */
	if (!libaroma_start()) return 0;
	libaroma_wm_cursor_setvisible(0);
	libaroma_wm_cursor_setshadow(0);
	/* force ldpi - testing only */
	//libaroma_fb_setdpi(240);
	/* load font - id 0 */
	if (!libaroma_font(0, libaroma_stream_mzip(global_zip, "roboto.ttf"))) return 0;
	if (!ptero_layout_init()) {
		printf("MLX: %s\n", "failed to init layout");
		return 0;
	}
	return 1;
}

/*
 * Function		: ptero_exit
 * Parameters	: none
 * Return type	: byte
 * Description	: cleanup memory and close libaroma
 */
byte ptero_exit(){
	/* cleanup loaded files */
	//libaroma_xml_free(layout_xml);
	//libaroma_stream_close(layout_stream);
	/* free variable storage */
	LIBAROMA_SARRAYP _var_stor = pterodon()->var_stor;
	pterodon()->var_stor = NULL;
	libaroma_sarray_free(_var_stor);
	/* release resource zip */
	LIBAROMA_ZIP _zip = global_zip;
	global_zip=NULL;
	libaroma_zip_release(_zip);
	if (!libaroma_end()) return 0;
	return 1;
}

#ifdef __cplusplus
}
#endif
#endif /* __ptero_main_c__ */ 
 
