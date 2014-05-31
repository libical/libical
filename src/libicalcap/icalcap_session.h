#ifndef __ICALCAP_SESSION_H__
#define __ICALCAP_SESSION_H__

#include "icalcap.h"

typedef struct _icalcap_session icalcap_session;

icalcap_session*icalcap_session_new(void);
int	icalcap_session_connect(icalcap_session *cap, const char *hostname, const int port);
int	icalcap_session_login(icalcap_session *cap, const char *username, const char *authname,
			const char *password);
icalcap*icalcap_session_start(const icalcap_session *cap, icalcap_msg_handler handler);
int	icalcap_session_disconnect(icalcap_session *cap);

#endif
