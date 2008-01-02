#ifndef __ICALCAP_MESSAGE_IMPL_H__
#define __ICALCAP_MESSAGE_IMPL_H__

#ifdef WITH_RR

#include <ical.h>
#include <librr/rr.h>

#define	ICALCAP_MESSAGE_CMD	1
#define	ICALCAP_MESSAGE_REPLY	1

struct _icalcap_message_rr {
	const icalcap		       *cap;
	int				type;

	icalcomponent		       *comp;

	RRFrame			       *frame;
	RRMessage		       *msg;
};

icalcap_message*icalcap_message_new_rr(const icalcap *cap, const icalcomponent *comp);
icalcap_message*icalcap_message_new_reply_rr(const icalcap_message *capmsg, const icalcomponent *comp);
void		icalcap_message_free_rr(icalcap_message *capmsg);

int		icalcap_message_send_reply_rr(icalcap_message *capmsg);
icalcomponent  *icalcap_message_sync_send_rr(icalcap_message *capmsg, int timeout);

#else
#error "No implementation of icalcap found!"
#endif

#endif
