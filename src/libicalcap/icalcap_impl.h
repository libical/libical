#ifndef __ICALCAP_IMPL_H__
#define __ICALCAP_IMPL_H__

#include "icalcap_session.h"

#ifdef WITH_RR

#include <librr/rr.h>
#include <librrsasl/rr-sasl.h>
#include <librrtls/rr-tls.h>
#include <librrcap/rr-cap.h>

struct _icalcap {
	RRCAP	       *chan;
	const char     *username,
		       *authname;
};

struct _icalerror {
	GError *err;
};


void	icalcap_free_rr(icalcap *cap);

int	icalcap_stop_rr(icalcap *cap);

icalcomponent *icalcap_send_component_rr(const icalcap *cap, const icalcomponent *comp, int timeout);

#else
#error "No implementation of icalcap found!"
#endif

#endif
