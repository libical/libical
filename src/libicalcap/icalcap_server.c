#include "config.h"

#include "icalcap.h"
#include "icalcap_server.h"
#include "icalcap_server_impl.h"


icalcap_server *
icalcap_server_new(icalcap_auth_handler auth_cb, icalcap_chanup_handler chanup_cb,
	icalcap_msg_handler msg_cb) {

#ifdef WITH_RR
	return icalcap_server_new_rr(auth_cb, chanup_cb, msg_cb);
#else
	return NULL;
#endif
}

int
icalcap_server_listen(icalcap_server *cap, const char *hostname, const int _port) {

	int port = _port;

	if (port <= 0)
		port = 1026;

#ifdef WITH_RR
	return icalcap_server_listen_rr(cap, hostname, port);
#else
	return 0;
#endif
}

int
icalcap_server_run(const icalcap_server *cap) {

#ifdef WITH_RR
	return icalcap_server_run_rr(cap);
#else
	return 0;
#endif
}

int
icalcap_server_shutdown(icalcap_server *cap) {

#ifdef WITH_RR
	return icalcap_server_shutdown_rr(cap);
#else
	return 0;
#endif
}
