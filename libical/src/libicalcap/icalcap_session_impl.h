#ifndef __ICALCAP_SESSION_IMPL_H__
#define __ICALCAP_SESSION_IMPL_H__

#ifdef WITH_RR

#include <librr/rr.h>
#include <librrsasl/rr-sasl.h>
#include <librrtls/rr-tls.h>
#include <librrcap/rr-cap.h>

struct _icalcap_session {
	RRProfileRegistry      *profreg;
	RRConnection	       *connection;
	RRCAPConfig	       *cfg;
	icalcap_msg_handler	handler;

	char		       *username;
};

icalcap_session*icalcap_session_new_rr(void);
int		icalcap_session_connect_rr(icalcap_session *cap,
					const char *hostname,
					const int port);
int		icalcap_session_login_rr(icalcap_session *cap,
					const char *username,
					const char *authname,
					const char *password);
icalcap	       *icalcap_session_start_rr(const icalcap_session *cap,
					icalcap_msg_handler handler);
int		icalcap_session_disconnect_rr(icalcap_session *cap);

#else
#error "No implementation of icalcap found!"
#endif

#endif
