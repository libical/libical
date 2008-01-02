#include "config.h"

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "icalcap.h"
#include "icalcap_session.h"
#include "icalcap_server.h"

#include "icalcap_impl.h"
#include "icalcap_message_impl.h"
#include "icalcap_session_impl.h"
#include "icalcap_server_impl.h"

icalcomponent *icalcap_component_new_from_string(const char *data);

static int	default_msg_handler(RRCAP *cap, RRFrame *frame, GError **error);
static void	client_final_handler(RRCAP *cap);

/**
 * Implementation of functions in icalcap
 */
void
icalcap_free_rr(icalcap *cap) {

	g_return_if_fail(cap);

	if (cap->username)
		free(cap->username);

	if (cap->authname)
		free(cap->authname);

	g_free(cap);
}

/**
 * Implementation of functions in icalcap_session
 */
icalcap_session *
icalcap_session_new_rr(void) {

	icalcap_session *sess;
	GError  *error = NULL;

	/* Initialize roadrunner */
	if (!rr_init(0, NULL, &error))
		return 0;

	if ((sess = g_new0(icalcap_session, 1)) == NULL) {
		/* FIXME return an error */
		return NULL;
	}

	sess->cfg = rr_cap_config_new();

	/* Tell roadrunner which profiles we want to support */
	sess->profreg = rr_profile_registry_new();
	rr_profile_registry_add_profile(sess->profreg, RR_TYPE_CAP, NULL);
	rr_profile_registry_add_profile(sess->profreg, RR_TYPE_TLS, NULL);
  	rr_profile_registry_add_profile(sess->profreg, RR_TYPE_SASL_DIGEST_MD5, NULL);

	return sess;
}

int
icalcap_session_connect_rr(icalcap_session *sess, const char *hostname, const int port) {

	GError	       *error = NULL;

	if (sess == NULL) {
		/* FIXME return the error */
		return FALSE;
	}

	/* Create a connection object */
	sess->connection = rr_tcp_connection_new(sess->profreg, hostname, port, &error);
	if (sess->connection == NULL) {
		/* FIXME return the error */
		return FALSE;
	}

	return TRUE;
}

int
icalcap_session_login_rr(icalcap_session *sess, const char *username, const char *authname,
			const char *password) {

	/* assert cap != NULL */
	GError	*error = NULL;

	rr_sasl_set_username(sess->connection, username);
	rr_sasl_set_authname(sess->connection, authname);
	rr_sasl_set_password(sess->connection, password);

	/* FIXME */
	if (!rr_sasl_login(sess->connection, RR_TYPE_SASL_DIGEST_MD5, 
			    "foo.example.com", NULL, &error)) {
		/* FIXME return the error */
		return 0;
	}

	return 1;
}

icalcap *
icalcap_session_start_rr(const icalcap_session *sess, icalcap_msg_handler handler) {

	/* assert sess != NULL */
	icalcap	       *cap;
	RRCAP	       *channel;
	GError	       *error = NULL;

	if ((cap = g_new0(icalcap, 1)) == NULL) {
		/* FIXME return an error */
		return NULL;
	}

	if (handler != NULL) {
		rr_cap_config_set_msg_handler(sess->cfg, default_msg_handler, (void *)handler);
		/* FIXME rr_cap_config_set_final_handler(cfg, client_final_handler); */
	}

	if ((channel = rr_cap_client_start(sess->connection, sess->cfg, &error)) == NULL) {
		/* FIXME return the error */
		goto FAILED;
	}

	cap->chan = channel;
	channel->hl = cap;

	return cap;

FAILED:
	g_free(cap);
	return NULL;
}

int
icalcap_stop_rr(icalcap *cap) {

	/* assert cap != NULL */
	GError	*error = NULL;

	if (!rr_cap_close(cap->chan, &error)) {
		/* FIXME return the error */
		return 0;
	}

	cap->chan = NULL;
	g_free(cap);

	return 1;
}

int
icalcap_session_disconnect_rr(icalcap_session *sess) {

	/* assert cap != NULL */
	GError	*error = NULL;

	if (!rr_connection_disconnect(sess->connection, &error)) {
		/* FIXME return the error */
		return 0;
	}

	sess->connection = NULL;
	g_free(sess);

	if (!rr_exit(&error)) {
		/* FIXME return the error */
		return 0;
	}

	return 1;
}

/**
 * Implementation of functions in icalcap_server
 */

/**
 * If the user properly authenticated (via SASL), initialize the channel
 * credentials. Otherwise, refuse to open the channel.
 */
static int
server_init_handler(RRCAP *chan, const gchar *piggyback, GError **error) {

	icalcap		       *cap;
	RRConnection	       *connection;

	icalcap_auth_handler	func;
	const gchar	       *username, *authname;
	int			rc;

	g_return_val_if_fail(chan, FALSE);
	g_return_val_if_fail(chan->cfg, FALSE);
	g_return_val_if_fail(chan->cfg->server_init_data, FALSE);

	connection = rr_channel_get_connection(RR_CHANNEL(chan));
	if ((username = rr_sasl_get_username(connection)) == NULL)
		return FALSE;
	if ((authname = rr_sasl_get_authname(connection)) == NULL)
		return FALSE;

	if ((cap = g_new0(icalcap, 1)) == NULL) {
		return FALSE;
	}

	cap->chan = chan;
	chan->hl = cap;

	cap->username = strdup(username);
	cap->authname = strdup(authname);

	func = (icalcap_auth_handler)chan->cfg->server_init_data;
	return func(cap, piggyback);
}

static void
server_confirmation_handler(RRCAP *chan) {

	icalcap_chanup_handler	func;

	g_return_if_fail(chan);
	g_return_if_fail(chan->cfg);
	g_return_if_fail(chan->cfg->server_confirmation_data);

	func = (icalcap_chanup_handler)chan->cfg->server_confirmation_data;

	func(chan->hl);
}

static gboolean
server_frame_handler(RRCAP *cap, RRFrame *frame, GError **error)
{
	if (frame->type == RR_FRAME_TYPE_MSG)
		/* FIXME */
		return default_msg_handler(cap, frame, error);
	else
		return FALSE;
}

static void
server_final_handler(RRCAP *cap)
{
	g_return_if_fail(cap);
	g_return_if_fail(RR_CAP(cap));

	if (cap->hl != NULL) {
		icalcap_free(cap->hl);
		cap->hl = NULL;
	}
}

/*
 * FIXME Do we want to pass argc and argv in?
 */
icalcap_server *
icalcap_server_new_rr(icalcap_auth_handler auth_cb, icalcap_chanup_handler chanup_cb,
								icalcap_msg_handler msg_cb) {

	icalcap_server *serv;
	GError	       *error = NULL;

	/* Initialize roadrunner */
	if (!rr_init(0, NULL, &error))
		return 0;

	if ((serv = g_new0(icalcap_server, 1)) == NULL) {
		/* FIXME return an error */
		return NULL;
	}
	serv->handler = msg_cb;

	/* This is somewhat hackish */
	serv->cfg = rr_cap_config_new();
	rr_cap_config_set_msg_handler(serv->cfg, NULL, (void *)msg_cb);
	rr_cap_config_set_frame_handler(serv->cfg, server_frame_handler);
	rr_cap_config_set_final_handler(serv->cfg, server_final_handler);
	rr_cap_config_set_server_init_handler(serv->cfg, server_init_handler, (void *)auth_cb);
	rr_cap_config_set_server_confirmation_handler(serv->cfg,
						server_confirmation_handler, (void *)chanup_cb);

	/* Tell roadrunner which profiles we want to support */
	serv->profreg = rr_profile_registry_new();
	rr_profile_registry_add_profile(serv->profreg, RR_TYPE_CAP, serv->cfg);
	rr_profile_registry_add_profile(serv->profreg, RR_TYPE_TLS, NULL);
  	rr_profile_registry_add_profile(serv->profreg, RR_TYPE_SASL_DIGEST_MD5, NULL);

	return serv;
}

int
icalcap_server_listen_rr(icalcap_server *serv, const char *hostname, const int port) {

	GError	       *error = NULL;
	g_return_val_if_fail(serv, FALSE);

	/* Create a listener object */
	serv->listener = rr_tcp_listener_new(serv->profreg, hostname, port, &error);
	if (serv->listener == NULL) {
		/* FIXME return the error */
		return FALSE;
	}

	return TRUE;
}

int
icalcap_server_run_rr(const icalcap_server *serv) {

	/* assert cap != NULL */
	GError	*error = NULL;

	if (!rr_wait_until_done(&error)) {
		/* FIXME return the error */
		return FALSE;
	}

	return TRUE;
}

int
icalcap_server_shutdown_rr(icalcap_server *serv) {

	/* assert cap != NULL */
	GError	*error = NULL;

	if (!rr_listener_shutdown(serv->listener, &error)) {
		/* FIXME return the error */
		return 0;
	}

	serv->listener = NULL;
	g_free(serv);

	return 1;
}

/*
 * icalcap_message.c
 */

/*
 * Internal constructor
 */
static struct _icalcap_message_rr *
_icalcap_message_new_from_component_rr(const icalcap *cap, int type, icalcomponent *comp) {

	struct _icalcap_message_rr	*ret;

	if ((ret = g_new0(struct _icalcap_message_rr, 1)) == NULL) {
		/* FIXME return an error */
		return NULL;
	}

	ret->cap	= cap;
	ret->type	= type;

	ret->comp	= comp;

	return ret;
}

static icalcap_message *
_icalcap_message_new_from_frame_rr(const icalcap *cap, int type, RRFrame *frame) {

	struct _icalcap_message_rr	*ret;

	if ((ret = g_new0(struct _icalcap_message_rr, 1)) == NULL) {
		/* FIXME return an error */
		return NULL;
	}

	ret->cap	= cap;
	ret->type	= type;
	ret->frame	= frame;

	ret->comp	= icalcap_component_new_from_string(ret->frame->payload);

	return (icalcap_message *)ret;
}

icalcap_message *
icalcap_message_new_rr(const icalcap *cap, const icalcomponent *comp) {

	struct _icalcap_message_rr	*ret;
	gchar				*str;

	if (comp == NULL) {
		/* FIXME return an error */
		return NULL;
	}

	ret = _icalcap_message_new_from_component_rr(cap, ICALCAP_MESSAGE_CMD, NULL);

	str = g_strdup_printf("%s\n\n%s",
		"Content-Type: text/calendar",
		icalcomponent_as_ical_string(comp));

	ret->msg	= rr_message_static_new(RR_FRAME_TYPE_MSG, str, strlen(str), TRUE);

	return (icalcap_message *)ret;
}

/*
 * This method and its implementation are critical. It has the responsibility for
 * serializing the component. The tricky part is that we have a options for encoding XROOT.
 */
icalcap_message *
icalcap_message_new_reply_rr(const icalcap_message *orig, const icalcomponent *comp) {

	struct _icalcap_message_rr     *in, *ret;
	icalcomponent		       *cc;
	GString			       *str;

	if ((in = (struct _icalcap_message_rr *)orig) == NULL) {
		/* FIXME return an error */
		return NULL;
	}

	ret = _icalcap_message_new_from_component_rr(in->cap, ICALCAP_MESSAGE_REPLY, NULL);

	/* FIXME */
	if (icalcomponent_isa(comp) != ICAL_XROOT_COMPONENT)
		return NULL;

	/* FIXME don't hardcode */
	str = g_string_new("Content-Type: text/calendar\n\n");

	for (cc = icalcomponent_get_first_component(comp,
					ICAL_VCALENDAR_COMPONENT);
	     cc != NULL;
	     cc = icalcomponent_get_next_component(comp,
					ICAL_VCALENDAR_COMPONENT)) {
		g_string_append(str, icalcomponent_as_ical_string(cc));
	}

	ret->msg	= rr_message_static_new(RR_FRAME_TYPE_RPY, str->str, strlen(str->str), TRUE);
	/* FIXME this should now be ok but if change the API we need to change */
	ret->msg->msgno = in->frame->msgno;

	return (icalcap_message *)ret;
}

void
icalcap_message_free_rr(icalcap_message *in) {

	struct _icalcap_message_rr *capmsg = (struct _icalcap_message_rr *)in;
	g_return_if_fail(capmsg);

	if (capmsg->comp != NULL) {
		icalcomponent_free(capmsg->comp);
		capmsg->comp = NULL;
	}

	if (capmsg->msg != NULL) {
		g_free(capmsg->msg);
	}

	g_free(capmsg);
}

int
icalcap_message_send_reply_rr(icalcap_message *in) {

	struct _icalcap_message_rr     *capmsg = (struct _icalcap_message_rr *)in;
	GError			       *error = NULL;
	int				rc;

	if ((rc = rr_channel_send_message(RR_CHANNEL(capmsg->cap->chan), capmsg->msg, &error)) == 0)
		g_message("error = %s", error->message);
	/* FIXME handle error */

	capmsg->msg = NULL;

	return rc;
}

icalcomponent *
icalcap_message_sync_send_rr(icalcap_message *in, int timeout) {

	struct _icalcap_message_rr     *capmsg = (struct _icalcap_message_rr *)in;
	icalcomponent		       *comp, *ret;

	gchar			       *str2;
	GError			       *error = NULL;
	int				rc;

	/* FIXME */
	rc = rr_cap_cmd(capmsg->cap->chan, capmsg->msg, 3 * timeout, &str2, &error);
	capmsg->msg = NULL;
	if (rc == 0) {
		g_message("error = %s", error->message);
		/* FIXME handle error */
		return 0;
	}

	comp = icalcap_component_new_from_string(str2);
	g_free(str2);
	if (ret == NULL)
		return NULL;

	return comp;
}

/*
 *
 */

/*
 * FIXME We assume we can safely create an icalcap_message derived object
 * without calls to the base object
 */
int
default_msg_handler(RRCAP *cap, RRFrame *frame, GError **error) {

	icalcap_msg_handler	func;
	icalcap_message        *msg;
	int			ret;

	g_return_val_if_fail(cap, FALSE);
	g_return_val_if_fail(cap->cfg, FALSE);
	g_return_val_if_fail(cap->cfg->msg_handler_data, FALSE);

	func = (icalcap_msg_handler)cap->cfg->msg_handler_data;

	msg = _icalcap_message_new_from_frame_rr(cap->hl, ICALCAP_MESSAGE_CMD, frame);
	if (msg == NULL) {
		/* FIXME */
		g_message("error");
		return FALSE;
	}

	ret = func(msg);
	icalcap_message_free(msg);

	return ret;
}
