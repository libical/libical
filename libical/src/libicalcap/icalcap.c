#include "config.h"

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "icalcap.h"
#include "icalcap_impl.h"
#include "icalcap_session_impl.h"	/* FIXME */


void
icalcap_free(icalcap *cap) {

	if (cap == NULL)
		return;

	if (cap->username) {
		free(cap->username);
		cap->username = NULL;
	}

	if (cap->authname) {
		free(cap->authname);
		cap->authname = NULL;
	}

#ifdef WITH_RR
	icalcap_free_rr(cap);
#endif
}

int
icalcap_stop(icalcap *cap) {

#ifdef WITH_RR
	return icalcap_stop_rr(cap);
#else
	return 0;
#endif
}

const char *
icalcap_get_authname(const icalcap *cap) {

	if (cap == NULL || cap->authname == NULL)
		return NULL;

	return cap->authname;
}

int
icalcap_set_authname(icalcap *cap, const char *authname) {

	if (cap == NULL)
		return FALSE;

	cap->authname = strdup(authname);

	return TRUE;
}

const char *
icalcap_get_username(const icalcap *cap) {

	if (cap == NULL || cap->username == NULL)
		return NULL;

	return cap->username;
}

int
icalcap_set_username(icalcap *cap, const char *username) {

	if (cap == NULL)
		return FALSE;

	cap->username = strdup(username);

	return TRUE;
}


void *
icalcap_get_data(const icalcap *cap) {

#ifdef WITH_RR
	if (cap == NULL)
		return NULL;

	return cap->data;
#else
	return NULL;
#endif
}

int
icalcap_set_data(icalcap *cap, void *data) {

#ifdef WITH_RR
	if (cap == NULL || data == NULL)
		return FALSE;

	cap->data = data;
	return TRUE;
#else
	return FALSE;
#endif
}
