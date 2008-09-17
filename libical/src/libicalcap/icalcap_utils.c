/*-
 * $Id: icalcap_utils.c,v 1.3 2008-01-02 20:07:38 dothebart Exp $
 *
 * See the file LICENSE for redistribution information. 
 *
 * Copyright (c) 2002 Andrea Campi <a.campi@inet.it>
 */

#include "config.h"

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "icalcap.h"

#define	CONTENT_TYPE	"Content-Type: text/calendar"


icalcomponent *
icalcap_component_new_from_string(const char *data) {

	icalcomponent  *ret = NULL;
	char	       *mtype;

	/* FIXME split the check */
	if (strncmp(data, CONTENT_TYPE, strlen(CONTENT_TYPE))) {
		return NULL;
	}

	mtype = (char *)data+strlen(CONTENT_TYPE);

	ret = icalcomponent_new_from_string(mtype);
	if (ret == NULL) {
		return NULL;
	}

#ifdef DEBUG
	g_message("icalcap_component_new_from_string icalcomponent_new_from_string = %p", ret);
#endif

	/* FIXME
	 * Validate here: should check at least the version
	 */
	if (icalcomponent_isa(ret) != ICAL_VCALENDAR_COMPONENT &&
	    icalcomponent_isa(ret) != ICAL_XROOT_COMPONENT) {
		icalcomponent_free(ret);

		return NULL;
	}

	return ret;
}

#if 0
RRCAPCmdArgs *
msg_parse(RRCAP *cap, icalcomponent *comp) {

	icalproperty   *prop;
	icalparameter  *param;
	icalvalue      *value;
	RRCAPCmdArgs   *ret = g_new0(RRCAPCmdArgs, 1);
	char *str;

	ret->comp = comp;

	/* Find the command */
	if ((prop = icalcomponent_get_first_property(comp, ICAL_CMD_PROPERTY)) == NULL) {
		rr_cap_send_error(cap, NULL, ICAL_9_0_UNRECOGNIZED_COMMAND,
			"No CMD sent", NULL);
		goto FAILED;
	}
	if ((value = icalproperty_get_value(prop)) == NULL) {
		str = icalproperty_as_ical_string_r(prop);
		rr_cap_send_error(cap, NULL, ICAL_9_0_UNRECOGNIZED_COMMAND,
			"CMD has no value", str);
		free(str);
		goto FAILED;
	}
	ret->cmd = icalvalue_get_cmd(value);

	/* Look for params */

	/* ID */
	if ((param = icalproperty_get_first_parameter(prop,
						ICAL_ID_PARAMETER)) != NULL) {
		if ((ret->id = icalparameter_get_id(param)) == NULL) {
			str = icalproperty_as_ical_string_r(prop);
			rr_cap_send_error(cap, NULL,
				ICAL_9_0_UNRECOGNIZED_COMMAND,
				"ID param is garbled",
				str);
			free(str);
			goto FAILED;
		}
	}

	/* LATENCY */
	if ((param = icalproperty_get_first_parameter(prop,
						ICAL_LATENCY_PARAMETER)) != NULL) {
		const char *tmp;
		if ((tmp = icalparameter_get_latency(param)) == NULL) {
			str = icalproperty_as_ical_string_r(prop);
			rr_cap_send_error(cap, NULL,
				ICAL_9_0_UNRECOGNIZED_COMMAND,
				"LATENCY is garbled",
				str);
			free(str);
			goto FAILED;
		}

		ret->latency = atoi(tmp);
	}

	/* ACTION */
	if ((param = icalproperty_get_first_parameter(prop,
						ICAL_ACTIONPARAM_PARAMETER)) != NULL) {
		if ((ret->action = icalparameter_get_actionparam(param))
		    == NULL) {
			str = icalproperty_as_ical_string_r(prop);
			rr_cap_send_error(cap, NULL,
				ICAL_9_0_UNRECOGNIZED_COMMAND,
				"ACTION is garbled",
				str);
			free(str);
			goto FAILED;
		}
	}

	if ((ret->latency >= 0) ^ (ret->action != ICAL_ACTIONPARAM_NONE)) {
		str = icalproperty_as_ical_string_r(prop);
		rr_cap_send_error(cap, NULL, ICAL_9_0_UNRECOGNIZED_COMMAND,
			"LATENCY and ACTION must be both present",
			str);
		free(str);
		goto FAILED;
	}

	return ret;

FAILED:
	g_free(ret);
	return NULL;
}
#endif
