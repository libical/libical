#ifndef __ICALCAP_H__
#define __ICALCAP_H__

#include <ical.h>

/*
 * Opaque objects
 */
typedef struct _icalcap icalcap;
typedef struct _icalerror icalerror;
typedef struct _icalcap_message icalcap_message;

/*
 * Callback
 */
typedef int  (*icalcap_msg_handler)(const icalcap_message *msg);

/*
 * icalcap member functions
 */
void		icalcap_free(icalcap *cap);
int		icalcap_stop(icalcap *cap);
const char     *icalcap_get_username(const icalcap *cap);

/*
 * icalcap_message member functions
 */

struct _icalcap_message {
	icalcap			       *cap;
	int				type;

	icalcomponent		       *comp;
};

icalcap_message*icalcap_message_new(const icalcap *cap, const icalcomponent *comp);
icalcap_message*icalcap_message_new_reply(const icalcap_message *capmsg, const icalcomponent *comp);
void		icalcap_message_free(icalcap_message *capmsg);

int		icalcap_message_reply_error(const icalcap_message *orig, enum icalrequeststatus status,
			const char *msg, const char *debug);
int		icalcap_message_reply_component(const icalcap_message *orig, icalcomponent *comp);

int		icalcap_message_send(icalcap_message *capmsg);
icalcomponent  *icalcap_message_sync_send(icalcap_message *capmsg, int timeout);

#endif
