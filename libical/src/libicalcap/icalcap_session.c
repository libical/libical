#include "config.h"

#include "icalcap.h"
#include "icalcap_session.h"
#include "icalcap_session_impl.h"


icalcap_session *
icalcap_session_new(void) {

#ifdef WITH_RR
	return icalcap_session_new_rr();
#else
	return NULL;
#endif
}

int
icalcap_session_connect(icalcap_session *sess, const char *hostname, const int _port) {

	int port = _port;

	if (port <= 0)
		port = 1026;

#ifdef WITH_RR
	return icalcap_session_connect_rr(sess, hostname, port);
#else
	return 0;
#endif
}

int
icalcap_session_login(icalcap_session *sess, const char *username, const char *authname,
			const char *password) {

#ifdef WITH_RR
	return icalcap_session_login_rr(sess, username, authname, password);
#else
	return 0;
#endif
}

icalcap *
icalcap_session_start(const icalcap_session *sess, icalcap_msg_handler handler) {

#ifdef WITH_RR
	return icalcap_session_start_rr(sess, handler);
#else
	return 0;
#endif
}

int
icalcap_session_disconnect(icalcap_session *sess) {

#ifdef WITH_RR
	return icalcap_session_disconnect_rr(sess);
#else
	return 0;
#endif
}
