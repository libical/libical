#ifndef __ICALCAP_SERVER_H__
#define __ICALCAP_SERVER_H__

#include "icalcap.h"

typedef struct _icalcap_server icalcap_server;

typedef int  (*icalcap_auth_handler)(const icalcap *cap, const char *piggyback);
typedef void (*icalcap_chanup_handler)(const icalcap *cap);

icalcap_server*icalcap_server_new(icalcap_auth_handler	auth_cb,
				  icalcap_chanup_handler chanup_cb,
				  icalcap_msg_handler	msg_cb);

int	icalcap_server_listen(icalcap_server *cap, const char *hostname, const int port);
int	icalcap_server_run(const icalcap_server *cap);
int	icalcap_server_shutdown(icalcap_server *cap);

#endif
