#ifndef __ptero_theme_c__
#define __ptero_theme_c__
#ifdef __cplusplus
extern "C" {
#endif
#include "common.h"

/*
 * Function		: ptero_theme_parse
 * Parameters	: theme tag
 * Return type	: byte
 * Description	: parse theme tag
 */
/*byte ptero_theme_parse(LIBAROMA_XML theme_tag){
	if (!theme_tag) return 0;
	
	return 1;
}*/

/*
 * Function		: ptero_colorset_parse
 * Parameters	: colorset tag
 * Return type	: byte
 * Description	: parse colorset tag
 */
byte ptero_colorset_parse(LIBAROMA_XML colorset_tag) {
	if (!colorset_tag) return 0;
	LIBAROMA_COLORSETP gui_colorset = /*malloc(sizeof(LIBAROMA_COLORSET));*/libaroma_colorget(NULL, pterodon()->winmain);
	char *attr, *color;
	int i=0;
	//byte check_for_active=1;
	/* loop through attributes to set colors */
	for (i=0, attr=colorset_tag->attr[0], color=colorset_tag->attr[1]; attr && color; attr=colorset_tag->attr[i], color=colorset_tag->attr[i+1], i+=2) {
		//printf("MLX: attr=%s, color=%s\n", attr, color);
		/*if (check_for_active && (0==strcmp(attr, "active") && (0==strcmp(color, "true")))){
			check_for_active=0;
			pterodon()->colorset_original = libaroma_wm()->colorset;
			pterodon()->colorset = gui_colorset();
			libaroma_wm()->colorset = pterodon()->colorset;
		}*/
		if (0==strcmp(attr, "primary"))
			gui_colorset->primary = libaroma_rgb_from_string(color);
		else if (0==strcmp(attr, "primary_text"))
			gui_colorset->primary_text = libaroma_rgb_from_string(color);
		else if (0==strcmp(attr, "primary_icon"))
			gui_colorset->primary_icon = libaroma_rgb_from_string(color);
		else if (0==strcmp(attr, "primary_light"))
			gui_colorset->primary_light = libaroma_rgb_from_string(color);
		else if (0==strcmp(attr, "accent"))
			gui_colorset->accent = libaroma_rgb_from_string(color);
		else if (0==strcmp(attr, "window_bg"))
			gui_colorset->window_bg = libaroma_rgb_from_string(color);
		else if (0==strcmp(attr, "window_text"))
			gui_colorset->window_text = libaroma_rgb_from_string(color);
		else if (0==strcmp(attr, "control_bg"))
			gui_colorset->control_bg = libaroma_rgb_from_string(color);
		else if (0==strcmp(attr, "control_primary_text"))
			gui_colorset->control_primary_text = libaroma_rgb_from_string(color);
		else if (0==strcmp(attr, "control_secondary_text"))
			gui_colorset->control_primary_text = libaroma_rgb_from_string(color);
		else if (0==strcmp(attr, "divider"))
			gui_colorset->divider = libaroma_rgb_from_string(color);
		else if (0==strcmp(attr, "dialog_bg"))
			gui_colorset->dialog_bg = libaroma_rgb_from_string(color);
		else if (0==strcmp(attr, "dialog_primary_text"))
			gui_colorset->dialog_primary_text = libaroma_rgb_from_string(color);
		else if (0==strcmp(attr, "dialog_secondary_text"))
			gui_colorset->dialog_secondary_text = libaroma_rgb_from_string(color);
		else if (0==strcmp(attr, "fill_icons")) {
			pterodon()->global_fill_icons = libaroma_rgb_from_string(color);
		}
	}
	/*do {
		attr = colorset_tag->attr[i];
		if (!attr) break;
		color = colorset_tag->attr[i+1];
		
		i+=2;
	} while (attr);*/
	return 1;
}/*

byte ptero_theme_init(char *name){
	if (!name) return 0;
	if (ptero_theme_exists(name)) {
		printf("MLX: %s theme already exists\n", name);
		return 1;
	}
	PTERO_THEMEP theme = calloc(1, sizeof(PTERO_THEME));
	theme->colorset = calloc(1, sizeof(LIBAROMA_COLORSET));
	theme->aliases = libaroma_sarrayp(NULL);
}*/

#ifdef __cplusplus
}
#endif
#endif /* __ptero_theme_c__ */ 
