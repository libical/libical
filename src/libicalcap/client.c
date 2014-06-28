/*-
 * $Id: client.c,v 1.3 2008-01-02 20:07:33 dothebart Exp $
 *
 * See the file LICENSE for redistribution information. 
 * If you have not received a copy of the license, please contact CodeFactory
 * by email at info@codefactory.se, or on the web at http://www.codefactory.se/
 * You may also write to: CodeFactory AB, SE-903 47, Umeå, Sweden.
 *
 * Copyright (c) 2002 Jonas Borgström <jonas@codefactory.se>
 * Copyright (c) 2002 Daniel Lundin   <daniel@codefactory.se>
 * Copyright (c) 2002 CodeFactory AB.  All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>

#include <librr/rr.h>

#include "icalcap.h"
#include "icalcap_session.h"

#define CLIENT_CAPABILITY \
"Content-Type: text/html\n\n\
BEGIN:VCALENDAR\n\
VERSION:2.0\n\
PRODIR:rrcap-client\n\
CMD:REPLY\n\
BEGIN:VREPLY\n\
CAP-VERSION:1.0\n\
PRODID:rrcap-client\n\
QUERY-LEVEL:CAL-QL-NONE\n\
CAR-LEVEL:CAR-FULL-NONE\n\
DATE-MAX:99991231T235959Z\n\
DATE-MIN:00000101T000000Z\n\
MAX-COMPONENT-SIZE:0\n\
COMPONENTS:VCALENDAR,VTODO,VJOURNAL,VEVENT,VCAR,\n\
 VALARM,VFREEBUSY,VTIMEZONE,STANDARD,DAYLIGHT,VREPLY\n\
ITIP-VERSION:2447\n\
RECUR-ACCEPTED:TRUE\n\
RECUR-EXPAND:TRUE\n\
RECUR-LIMIT:0\n\
STORES-EXPANDED:FALSE\n\
END:VREPLY\n\
END:VCALENDAR\n"


icalcomponent *
icalcap_send_cmd(const icalcap *cap, const gchar *cmd, const gchar *id, int timeout) {

	icalcap_message*capmsg;
	icalcomponent  *comp, *ret = NULL;
	icalproperty   *prop;

	prop = icalproperty_vanew_cmd(
		icalproperty_string_to_enum(cmd),
		icalparameter_new_id(id),
		0);

	if (timeout > 0) {
		char buf[16];

		snprintf(buf, 16, "%d", timeout);
		icalproperty_add_parameter(prop,
			icalparameter_new_latency(buf));

		icalproperty_add_parameter(prop,
			icalparameter_new_actionparam(ICAL_ACTIONPARAM_ABORT));
	}

	comp = icalcomponent_vanew(
		ICAL_VCALENDAR_COMPONENT,
		icalproperty_new_version("2.0"),
		icalproperty_new_prodid("-//I.Net spa//NONSGML//EN"),
		prop,
		0);

	capmsg = icalcap_message_new(cap, comp);
	ret = icalcap_message_sync_send(capmsg, timeout);
	icalcap_message_free(capmsg);
	icalcomponent_free(comp);

	return ret;
}

icalcomponent *
icalcap_new_reply_component(const char *id, const icalcomponent *comp) {

	if (comp == NULL)
		return NULL;

	return icalcomponent_vanew(
		ICAL_VCALENDAR_COMPONENT,
		icalproperty_new_version("2.0"),
		icalproperty_new_prodid("-//I.Net spa//NONSGML//EN"),
		icalproperty_vanew_cmd(
			ICAL_CMD_REPLY,
/* 			icalparameter_new_id(id), */
			0),
		comp,
		0);
}


static int
msg_handler(const icalcap_message *capmsg) {

	icalcomponent *reply;
	char *temp;

	temp = icalcomponent_as_ical_string_r(capmsg->comp);
	g_message("Got: %s", temp);
	free(temp);

	/* FIXME Check it's a GET-CAPABILITY */

	reply = icalcap_new_reply_component(NULL, capmsg->comp);
	if (reply == NULL) {
		return FALSE;
	}

	icalcomponent_add_property(reply,
		icalproperty_new_prodid("client"));

	icalcomponent_free(reply);
	return TRUE;
}

int
main (gint argc, gchar **argv)
{
	icalcap_session*conn;
	icalcap	       *cap;
	icalcomponent  *comp;

	int		i, n;
	int		verbose = 0;

	if ((conn = icalcap_session_new()) == NULL) {
		fprintf(stderr, "Init failed\n");
		exit(0);
	}

	if (!icalcap_session_connect(conn, "gundam.inet.it", 0)) {
		fprintf(stderr, "Connect failed\n");
		exit(0);
	}

	if (!icalcap_session_login(conn, "user@example.com", "user@example.com", "password")) {
		fprintf(stderr, "Login failed\n");
		exit(0);
	}

	if ((cap = icalcap_session_start(conn, msg_handler)) == NULL) {
		fprintf(stderr, "Start failed\n");
		exit(0);
	}

	if (argc > 1 && *argv[1] == '1')
		n = 100;
	else
		n = 1;

	for (i=0; i<n; i++) {
		g_print("Test 1: %d\n", i);

		if ((comp = icalcap_send_cmd(cap, "GET-CAPABILITY", "zero",15))
		    == NULL) {
			fprintf(stderr, "Send failed\n");
			exit(0);
		}

		if (verbose) {
			char *obj = icalcomponent_as_ical_string_r(comp);
			g_print("Got %s\n", obj);
			g_free(obj);
		}

		if (comp)
			icalcomponent_free(comp);
	}

	if (!icalcap_stop(cap)) {
		fprintf(stderr, "Stop failed\n");
		exit(0);
	}

	if (!icalcap_session_disconnect(conn)) {
		fprintf(stderr, "Disconnect failed\n");
		exit(0);
	}

	return 0;
}
