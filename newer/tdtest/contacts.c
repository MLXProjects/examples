#include "common.h"

static TD_USER _myuser={0};

TD_USERP *contacts;
int contactn;

TD_USERP myuser(){
	return &_myuser;
}

void user_update(TD_USERP user, double id,
				char *fname, char *lname,
				char *uname, char *phone,
				byte isactive, double last_time
){
	if (user->id!=id)
		user->id=id;
	if (user->fname){
		if (strcmp(user->fname, fname)){
			free(user->fname);
			user->fname=fname;
		}
	}
	else user->fname=fname;
	if (user->lname){
		if (strcmp(user->lname, lname)){
			free(user->lname);
			user->lname=lname;
		}
	}
	else user->lname=lname;
	if (user->uname){
		if (strcmp(user->uname, uname)){
			free(user->uname);
			user->uname=uname;
		}
	}
	else user->uname=uname;
	if (user->phone){
		if (strcmp(user->phone, phone)){
			free(user->phone);
			user->phone=phone;
		}
	}
	else user->phone=phone;
	if (user->isactive!=isactive)
		user->isactive=isactive;
	if (user->last_time!=last_time)
		user->last_time=last_time;
}

byte contacts_add(double id,
					char *fname,char *lname,
					char *uname, char *phone,
					byte isactive, double last_time){
	TD_USERP contact;
	if ((contact=contacts_find(id))!=NULL){
		user_update(contact, id, fname, lname, uname, phone, isactive, last_time);
		return 1;
	}
	logd("[MISC] adding id %d (%s %s - @%s, %s)", id, fname, lname, uname, phone);
	contact = calloc(sizeof(TD_USER), 1);
	if (!contact){
		loge("[MISC] failed to calloc contact %d", id);
		return 0;
	}
	contact->id=id;
	contact->fname=fname;
	contact->lname=lname;
	contact->uname=uname;
	contact->phone=phone;
	contact->isactive=isactive;
	contact->last_time=last_time;
	TD_USERP *tmp_contacts = realloc(contacts, sizeof(TD_USERP)*contactn+1);
	if (!tmp_contacts){
		loge("[MISC] cannot realloc contact list");
		free(contact);
		return 0;
	}
	tmp_contacts[contactn]=contact;
	contacts=tmp_contacts;
	contactn+=1;
	return 1;
}

TD_USERP contacts_find(double id){
	int i;
	for (i=contactn; i>0; i--){
		TD_USERP try_contact = contacts[i-1];
		if (try_contact->id==id){
			return try_contact;
		}
	}
	return NULL;
}