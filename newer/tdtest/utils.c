#include "common.h"
/*
void mlx_waitAnswer(char *extra){
	if (0==strcmp(, extra))
}*/

unsigned long utils_djb2_hash(char *str){
    unsigned long hash = 5381;
    int c;
    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}

const char *utils_json_type(LIBAROMA_JSONP parsed_res){
	switch (parsed_res->type){
		case cJSON_False:
			return "False";
		case cJSON_True:
			return "True";
		case cJSON_NULL:
			return "NULL";
		case cJSON_Number:
			return "Number";
		case cJSON_String:
			return "String";
		case cJSON_Array:
			return "Array";
		case cJSON_Object:
			return "Object";
		case cJSON_IsReference:
			return "IsReference";
	}
	return "Unknown";
}