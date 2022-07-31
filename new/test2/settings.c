#ifndef __test_settings_c__
#define __test_settings_c__
#ifdef __cplusplus
extern "C" {
#endif
	
#include "common.h"

static TEST_CONFIG _test_config={0};

TEST_CONFIGP test_config(){
	return &_test_config;
}

byte settings_load(){
	//TODO: parse settings file
	return 0;
}

byte settings_init(){
	if (!settings_load()){
		_test_config.appbar_bg=RGB(009385);
		_test_config.appbar_fg=RGB(FFFFFF);
		_test_config.rgb_custom=NULL;
		_test_config.fragani_scale=0;
	}
	return 1;
}

#ifdef __cplusplus
}
#endif
#endif /* __test_settings_c__ */
