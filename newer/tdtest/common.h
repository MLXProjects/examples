#ifndef __common_h__
#define __common_h__

#include <aroma.h>
#include <stdio.h>
#include <stdlib.h>
#include <td/telegram/td_json_client.h>
#include <pthread.h>

#define loge(...) {printf("E/APP(): "); printf(__VA_ARGS__); printf("\n");}
#define logi(...) {printf("I/APP(): "); printf(__VA_ARGS__); printf("\n");}
#define logd(...) {if (main_debug){printf("D/APP(): "); printf(__VA_ARGS__); printf("\n");}}

typedef struct {
	double id;
	char *uname;
	char *fname;
	char *lname;
	char *phone;
	
	double last_time;
	byte isactive;
	
	char *photo_data;
} TD_USER, * TD_USERP;

typedef struct {
	double id;
	char *uname;
	char *title;

	byte issuper;

	int **members;
	int membern;
	char *photo_data;
} TD_GROUP, * TD_GROUPP;

extern int tg_running, tdclient_id;
extern byte onauth, onrequest;
extern char tdlib_version[];

extern TD_USERP *contacts;
extern int contactn;
extern byte contacts_mutex;
extern byte main_debug;

extern TD_USERP myuser();
extern void user_update(TD_USERP user, double id,
				char *fname, char *lname,
				char *uname, char *phone,
				byte isactive, double last_time
);
extern byte contacts_add(double id,
				char *fname,char *lname,
				char *uname, char *phone,
				byte isactive, double last_time);
extern TD_USERP contacts_find(double id);
extern unsigned long utils_djb2_hash(char *string);

#define mysleep(ms) usleep(ms*1000)

#endif //__common_h__