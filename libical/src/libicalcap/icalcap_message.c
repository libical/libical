#include "config.h"

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "ical.h"
#include "icalcap.h"
#include "icalcap_message_impl.h"


/**
 * constructor
 *
 * Create a new icalcap_message from a component. This represents a command.
 */
icalcap_message *
icalcap_message_new(const icalcap *cap, const icalcomponent *comp) {

	icalcap_message *ret = NULL;

#ifdef WITH_RR
	ret = icalcap_message_new_rr(cap, comp);
#else
	ret = NULL;
#endif
	return ret;
}

/**
 * constructor
 *
 * Create a new icalcap_message from a component. This represents a reply.
 */
icalcap_message *
icalcap_message_new_reply(const icalcap_message *capmsg, const icalcomponent *comp) {

#ifdef WITH_RR
	return icalcap_message_new_reply_rr(capmsg, comp);
#else
	return NULL;
#endif
}

/**
 * destructor
 *
 * Delete icalcap_message
 */
void
icalcap_message_free(icalcap_message *capmsg) {

#ifdef WITH_RR
	icalcap_message_free_rr(capmsg);
#else
#endif
}

/**
 * Send the icalcap_message and wait for an answer.
 * The icalcap_message will be invalid after this call, and only valid call will be
 * icalcap_message_free().
 */
icalcomponent *
icalcap_message_sync_send(icalcap_message *capmsg, int timeout) {

	g_return_val_if_fail(capmsg, NULL);

#ifdef WITH_RR
	return icalcap_message_sync_send_rr(capmsg, timeout);
#else
	return NULL;
#endif
}

/**
 * Send the icalcap_message.
 * The icalcap_message will be invalid after this call, and only valid call will be
 * icalcap_message_free().
 */
int
icalcap_message_send(icalcap_message *capmsg) {

	int	rc = FALSE;
	char   *str;

	g_return_val_if_fail(capmsg, FALSE);

#ifdef WITH_RR
	rc = icalcap_message_send_reply_rr(capmsg);
#else
	rc = FALSE;
#endif

	return rc;
}

/**
 * Convenience method to send a component in reply to the given icalcap_message.
 * The icalcomponent is not modified in any way.
 *
 * FIXME should be const icalcomponent *
 */
int
icalcap_message_reply_component(const icalcap_message *orig, icalcomponent *in) {

	icalcap_message*reply;
	int		rc;

	reply = icalcap_message_new_reply(orig, in);
	if (reply == NULL)
		return FALSE;

	rc = icalcap_message_send(reply);
	icalcap_message_free(reply);

	return rc;
}

/* Only used by icalcap_message_reply_error */
static icalcomponent *new_reply_component(const icalcap_message *capmsg, const icalcomponent *comp);

/**
 * Convenience method to send an error in reply to the given icalcap_message.
 */
int
icalcap_message_reply_error(const icalcap_message *orig, enum icalrequeststatus status,
						const char *msg, const char *debug) {

	struct icalreqstattype	stat;
	icalcomponent	       *comp, *vreply;
	int			rc;

	/* Prepare the REQUEST-STATUS */
	stat = icalreqstattype_from_string(
		icalenum_reqstat_code(status));

	if (msg != NULL) {
		/* FIXME we used to do
			stat.desc = strdup(msg);
		   but this created a memory leak. Maybe the destructor for reqstat? */
		stat.desc = msg;
	} else {
		stat.desc = icalenum_reqstat_desc(status);
	}

	if (debug != NULL)
		stat.debug = debug;

	/* Prepare a VREPLY component */
	vreply = icalcomponent_vanew(
		ICAL_VREPLY_COMPONENT,
		icalproperty_new_requeststatus(stat),
		0);
	if (vreply == NULL) {
		/* FIXME */
		return FALSE;
	}

	comp = new_reply_component(orig, vreply);

	rc = icalcap_message_reply_component(orig, comp);

	icalcomponent_free(comp);
	icalcomponent_free(vreply);

	return rc;
}


/* internal use */

/* only used by new_reply_component */
static const char *
get_id(icalcomponent *comp) {

	icalproperty   *prop;
	icalparameter  *param;

	prop = icalcomponent_get_first_property(comp, ICAL_CMD_PROPERTY);
	if (prop == NULL)
		return NULL;

	param = icalproperty_get_first_parameter(prop, ICAL_ID_PARAMETER);
	if (param == NULL)
		return NULL;

	return icalparameter_get_id(param);
}

/* only used by icalcap_message_reply_error */
static icalcomponent *
new_reply_component(const icalcap_message *capmsg, const icalcomponent *comp) {

	icalcomponent  *clone, *cal, *root;
	icalproperty   *cmd;
	const char     *id;

	cmd = icalproperty_new_cmd(ICAL_CMD_REPLY);

	if (capmsg->comp != NULL) {
		id = get_id(capmsg->comp);

		if (id != NULL)
			icalproperty_add_parameter(cmd, icalparameter_new_id(id));
	}

	cal = icalcomponent_vanew(
		ICAL_VCALENDAR_COMPONENT,
		icalproperty_new_version("2.0"),
		icalproperty_new_prodid("-//I.Net spa//NONSGML//EN"),
		cmd,
		0);

	if (comp != NULL) {
		clone = icalcomponent_new_clone(comp);
		icalcomponent_add_component(cal, clone);
	}

	root = icalcomponent_vanew(
		ICAL_XROOT_COMPONENT,
		cal,
		0);
	return root;
}
