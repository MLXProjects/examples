#include "common.h"
#include "hashes.h"

char *phonenum_json, *verifkey_json;
byte onauth, onrequest;
pthread_t tg_update_tid;

void handleAuthStateUpdate(LIBAROMA_JSONP parsed_res);

void *tg_update_thread(void *cookie){
	const char *result;
	byte isanswer;
	while(tg_running){
		isanswer=0;
		//logd("[UPD ] waiting for result");
		result = td_receive(3.0);
		if (result != NULL) {
			// parse the result as a JSON object and process it as an incoming update or an answer to a previously sent request
			//logd("result: %s", result);
			LIBAROMA_JSONP parsed_res = libaroma_json(result);
			if (!parsed_res){
				loge("[UPD ] failed to parse result");
				continue;
			}
			LIBAROMA_JSONP extra=libaroma_json_object(parsed_res, "@extra");
			if (extra && strcmp(extra->valuestring, "")){ //if extra is valid and not empty, it's answer :D
				//logi("[UPD ] result is answer to previous request");
				//logi("[UPD ] result is answer to previous request: \n%s", result);
				isanswer=1;
			}
			LIBAROMA_JSONP type=libaroma_json_object(parsed_res, "@type");
			if (!type){
				loge("[UPD ] failed to parse type");
				continue;
			}
			//logd("[UPD ] parsed type=%s", libaroma_json_string(type), utils_djb2_hash(libaroma_json_string(type)));
			switch (utils_djb2_hash(type->valuestring)){
				case updateAuthorizationState:{
					handleAuthStateUpdate(parsed_res);
				} break;
				case updateOption:{
					LIBAROMA_JSONP optname=libaroma_json_object(parsed_res, "name");
					if (0==strcmp(optname->valuestring, "version")){
						LIBAROMA_JSONP optval=libaroma_json_object(parsed_res, "value");
						LIBAROMA_JSONP optval_type=libaroma_json_object(optval, "@type");
						if (0==strcmp(optval_type->valuestring, "optionValueString")){
							snprintf(tdlib_version, 16, "%s", libaroma_json_object(optval, "value")->valuestring);
						}
					}
				} break;
				/* group updates */
				case updateBasicGroup:
				case updateSupergroup:
				case updateSupergroupFullInfo:
					break;
				/* chat updates */
				case updateNewChat:{
					LIBAROMA_JSONP chat = libaroma_json_object(parsed_res, "chat");
					LIBAROMA_JSONP chat_type1 = libaroma_json_object(chat, "type");
					LIBAROMA_JSONP chat_type2 = libaroma_json_object(chat_type1, "@type");
					LIBAROMA_JSONP chat_title = libaroma_json_object(chat, "title");
					LIBAROMA_JSONP chat_id;
					if (0==strcmp(chat_type2->valuestring, "chatTypeSupergroup")){
						chat_id = libaroma_json_object(chat_type1, "supergroup_id");
					}
					else if (0==strcmp(chat_type2->valuestring, "chatTypeBasicGroup")){
						chat_id = libaroma_json_object(chat_type1, "basicgroup_id");
					}
					else if (0==strcmp(chat_type2->valuestring, "chatTypePrivate")){
						chat_id = libaroma_json_object(chat_type1, "user_id");
					}
					else {
						loge("update_new_chat called for unknown type %s", chat_type2->valuestring);
						logd("info: \n%s", result);
						break;
					}
					//logd("update_new_chat %s (%s - %1.0f)", chat_title->valuestring, chat_type2->valuestring, chat_id->valuedouble);
					//logd("%s: \n%s", type->valuestring, result);
					//tg_running=0;
				} break;
				case updateChatAction:
				case updateChatActionBar:
				case updateChatAvailableReactions:
				case updateChatFilters:
				case updateChatLastMessage:
				case updateChatMessageSender:
				case updateChatNotificationSettings:
				case updateChatReadOutbox:
				case updateChatReadInbox:
				case updateChatTheme:
				case updateChatThemes:
				case updateChatVideoChat:
				case updateUnreadChatCount:
					break;
				/* user updates */
				case hash_users:{
					LIBAROMA_JSONP users_count=libaroma_json_object(parsed_res, "total_count");
					LIBAROMA_JSONP users_array=libaroma_json_object(parsed_res, "user_ids");
					char *req_json[64];
					int i;
					double user_id;
					for (i=0; i<users_count->valuedouble; i++){
						LIBAROMA_JSONP user_id=libaroma_json_array(users_array, i);
						snprintf(req_json, 64, "{\"@type\": \"getUser\",\"user_id\":%1.0f}", user_id->valuedouble);
						td_send(tdclient_id, req_json);
						//logi("found id #%i with type %s (%1.0f)", i, utils_json_type(user_id), user_id->valuedouble);
					}
				} break;
				case hash_user:
				case updateUser:{
					LIBAROMA_JSONP user=libaroma_json_object(parsed_res, "user");
					if (!user) user=parsed_res; //allows to use same case for both updates
					byte user_iscontact = libaroma_json_is_true(libaroma_json_object(user, "is_contact"));
					if (user_iscontact){
						LIBAROMA_JSONP user_id	 =libaroma_json_object(user, "id");
						LIBAROMA_JSONP user_fname=libaroma_json_object(user, "first_name");
						LIBAROMA_JSONP user_lname=libaroma_json_object(user, "last_name");
						LIBAROMA_JSONP user_uname=libaroma_json_object(user, "username");
						LIBAROMA_JSONP user_phone=libaroma_json_object(user, "phone_number");
						LIBAROMA_JSONP user_status=libaroma_json_object(user, "status");
						LIBAROMA_JSONP status_type=libaroma_json_object(user_status, "@type");
						byte has_fname=(strcmp(user_fname->valuestring, ""));
						byte has_lname=(strcmp(user_lname->valuestring, ""));
						byte has_uname=(strcmp(user_uname->valuestring, ""));
						byte has_phone=(strcmp(user_phone->valuestring, ""));
						if ((user_id->valuedouble==myuser()->id) || 
							(isanswer && (0==strcmp(extra->valuestring, "mlx_getMe")))
						){
							user_update(myuser(), user_id->valuedouble, 
								has_fname?strdup(user_fname->valuestring):NULL, has_lname?strdup(user_lname->valuestring):NULL, 
								has_uname?strdup(user_uname->valuestring):NULL, has_phone?strdup(user_phone->valuestring):NULL,
								0, 0
							);
						}
						else if (!contacts_add(user_id->valuedouble, 
								has_fname?strdup(user_fname->valuestring):NULL, has_lname?strdup(user_lname->valuestring):NULL, 
								has_uname?strdup(user_uname->valuestring):NULL, has_phone?strdup(user_phone->valuestring):NULL,
								0, 0)
						){
							loge("failed to add contact %1.0f", user_id->valuedouble);
						}
						unsigned long status_hash=utils_djb2_hash(status_type->valuestring);
						if (status_hash!=userStatusOffline && status_hash!=userStatusRecently){
							logi("please bro, \n#define %s(%lu)", status_type->valuestring, utils_djb2_hash(status_type->valuestring));
						}
						/*TD_USERP real_user = contacts_find(user_id->valuedouble);
						if (real_user!=NULL){
							if (0==strcmp(status_type->valuestring, "userStatusOffline")){
								real_user->isactive=0;
								real_user->last_time=libaroma_json_object(user_status, "was_online")->valuedouble;
							}
						}*/
					}
				} break;
				case updateUserFullInfo:
				case updateUserStatus:
				/* message updates */
				case updateDeleteMessages:
				case updateNewMessage:
				case updateMessageInteractionInfo:
				/* misc updates */
				case updateDiceEmojis:
				case updateReactions:
				case updateHavePendingNotifications:
				case updateAnimationSearchParameters:
				case updateAttachmentMenuBots:
				case updateSelectedBackground:
				case updateFileDownloads:
				case updateScopeNotificationSettings:
				case updateConnectionState:
				/* ignore - already handled by handleAuthStateUpdate */
				case authorizationStateWaitTdlibParameters:
				case authorizationStateWaitEncryptionKey:
				case authorizationStateWaitPhoneNumber:
				case authorizationStateWaitCode:
					break;
				case tdlibRequestError:{
					if (isanswer){
						logi("last request (%s) failed:\n%s", extra->valuestring, result);
					}
					else {
						logi("last request failed:\n%s", result);
					}
				} break;
				case tdlibRequestOk:{
					if (isanswer){
						logi("last request (%s) succeded:\n%s", extra->valuestring, result);
					}
					else {
						logi("last request succeded:\n%s", result);
					}
				} break;
				default: {
					logd("[UPD ] please handle #define %s (%lu)", type->valuestring, utils_djb2_hash(type->valuestring));
				}
			}
			libaroma_json_free(parsed_res);
			//if (isanswer && onrequest) onrequest=0;
		}
	}
}

void handleAuthStateUpdate(LIBAROMA_JSONP parsed_res){
	//logd("[AUTH] handling auth state update");
	LIBAROMA_JSONP authorization_state=libaroma_json_object(parsed_res, "authorization_state");
	LIBAROMA_JSONP auth_type=libaroma_json_object(authorization_state, "@type");
	switch (utils_djb2_hash(auth_type->valuestring)){
		case authorizationStateWaitTdlibParameters:{
			phonenum_json=NULL;
			verifkey_json=NULL;
			td_send(tdclient_id,
				"{"
					"\"@type\": \"setTdlibParameters\","
					"\"parameters\": { "
						"\"database_directory\": \"./tdlib\","
						"\"use_file_database\": true,"
						"\"use_chat_info_database\": false,"
						"\"use_message_database\": false,"
						"\"use_secret_chats\": false,"
						"\"api_id\": 2011767,"
						"\"api_hash\": \"0144de08f592724899c929dc73b4e92b\","
						"\"system_language_code\": \"en-US\","
						"\"device_model\": \"Build Server\","
						"\"system_version\": \"1.0\","
						"\"application_version\": \"0.1\","
						"\"enable_storage_optimizer\": true"
					"}"
				"}"
			);
			logd("[AUTH] tdlib params sent");
		} break;
		case authorizationStateWaitEncryptionKey:{
			td_send(tdclient_id,
				"{"
					"\"@type\": \"checkDatabaseEncryptionKey\","
					"\"encryption_key\":\"\""
				"}"
			);
			logd("[AUTH] empty encryption key sent");
		} break;
		case authorizationStateWaitPhoneNumber:{
			//onauth=1;
			char *line = NULL;
			size_t size;
			logd("[AUTH] asking for phone number: ");
			if (getline(&line, &size, stdin) == -1) {
				loge("[AUTH] invalid number");
			} else {
				phonenum_json=malloc(sizeof(char)*(59+size));
				sprintf(phonenum_json,
						"{"
							"\"@type\":\"setAuthenticationPhoneNumber\","
							"\"phone_number\":\"%s\""
						"}",
						line
				);
				free(line);
				td_send(tdclient_id, phonenum_json);
			}
		} break;
		case authorizationStateWaitCode:{
			if (phonenum_json!=NULL){
				free(phonenum_json);
				phonenum_json=NULL;
			}
			char *line = NULL;
			size_t size;
			logd("[AUTH] asking for verif code: ");
			if (getline(&line, &size, stdin) == -1) {
				loge("[AUTH] invalid number");
			} else {
				verifkey_json=malloc(sizeof(char)*(46+size));
				sprintf(verifkey_json, "{\"@type\": \"checkAuthenticationCode\",\"code\":\"%s\"}", line);
				free(line);
				td_send(tdclient_id, verifkey_json);
			}
		} break;
		case authorizationStateReady:{
			if (verifkey_json!=NULL){
				free(verifkey_json);
				verifkey_json=NULL;
			}
			logd("[AUTH] TDLib authorization done");
			/* refresh myuser & contacts at logon */
			td_send(tdclient_id, "{\"@type\":\"getMe\",\"@extra\":\"mlx_getMe\"}");
			td_send(tdclient_id, "{\"@type\":\"getContacts\",\"@extra\":\"mlx_getContacts\"}}");
			main_debug=0;
			onauth=0;
		} break;
		case authorizationStateClosing:{
			main_debug=1;
			logd("[AUTH] closing TDLib instance");
		} break;
		case authorizationStateClosed:{
			logd("[AUTH] TDLib instance closed");
			tg_running=0;
		} break;
		default: {
			logd("[AUTH] please handle \n#define %s(%lu)", auth_type->valuestring, utils_djb2_hash(auth_type->valuestring));
		} break;
	}
}