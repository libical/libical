#ifndef __ICALCAP_SERVER_H__
#define __ICALCAP_SERVER_H__

#include "icalcap.h"

typedef struct _icalcap_server icalcap_server;

typedef int  (*icalcap_auth_handler)(const char *auth, const char *user);
typedef int  (*icalcap_init_handler)(icalcap *cap, const char *piggyback);
typedef void (*icalcap_chanup_handler)(icalcap *cap);
typedef void (*icalcap_final_handler)(icalcap *cap);

icalcap_server *icalcap_server_new(icalcap_auth_handler	auth_cb,
				  icalcap_init_handler	init_cb,
				  icalcap_chanup_handler chanup_cb,
				  icalcap_msg_handler	msg_cb,
				  icalcap_final_handler	final_cb);

int		icalcap_server_listen(icalcap_server *cap,
				const char *hostname,
				const int port);

int		icalcap_server_run(const icalcap_server *cap);

int		icalcap_server_unlisten(icalcap_server *cap);
int		icalcap_server_shutdown(icalcap_server *cap);

#endif
