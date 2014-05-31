#ifndef __ICALCAP_SERVER_IMPL_H__
#define __ICALCAP_SERVER_IMPL_H__

#ifdef WITH_RR

#include <librr/rr.h>
#include <librrsasl/rr-sasl.h>
#include <librrtls/rr-tls.h>
#include <librrcap/rr-cap.h>

struct _icalcap_server {
	RRProfileRegistry      *profreg;
	RRListener	       *listener;
	RRCAPConfig	       *cfg;
	icalcap_msg_handler	handler;
};

icalcap_server *icalcap_server_new_rr(icalcap_auth_handler auth_cb,
					icalcap_chanup_handler chanup_cb,
					icalcap_msg_handler msg_cb);
int		icalcap_server_listen_rr(icalcap_server *cap,
					const char *hostname,
					const int port);
int		icalcap_server_run_rr(const icalcap_server *cap);
int		icalcap_server_shutdown_rr(icalcap_server *cap);

#else
#error "No implementation of icalcap found!"
#endif

#endif
