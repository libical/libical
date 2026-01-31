/*======================================================================
 FILE: icalmessage.h
 CREATOR: eric 07 Nov 2000

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 =========================================================================*/

/**
 * @file icalmessage.h
 * @brief Defines functions for creating component reply messages.
 */

#ifndef ICALMESSAGE_H
#define ICALMESSAGE_H

#include "libical_icalss_export.h"
#include "icalcomponent.h"

/**
 * Constructs an icalcomponent for an accept reply.
 *
 * @param c is a pointer to a valid icalcomponent
 * @param user is a pointer to a char string containing a user from the attendee list
 * @param msg is a pointer to a char string containing the message
 *
 * @return a pointer to the newly constructed icalcomponent; or NULL if an error occurred
 * like the specified user was not included in the attendee list.
 */
LIBICAL_ICALSS_EXPORT icalcomponent *icalmessage_new_accept_reply(const icalcomponent *c,
                                                                  const char *user,
                                                                  const char *msg);

/**
 * Constructs a new icalcomponent for a decline reply for an existing icalcomponent.
 *
 * @param c is a pointer to a valid icalcomponent
 * @param user is a pointer to a char string containing a user from the attendee list
 * @param msg is a pointer to a char string containing the message
 *
 * @return a pointer to the newly constructed icalcomponent; or NULL if an error occurred
 * like the specified user was not included in the attendee list.
 */
LIBICAL_ICALSS_EXPORT icalcomponent *icalmessage_new_decline_reply(const icalcomponent *c,
                                                                   const char *user,
                                                                   const char *msg);

/**
 * Constructs a new icalcomponent for a counterproposal reply for an existing icalcomponent.
 *
 * @param oldc is a pointer to a valid icalcomponent for the current reply to modify
 * @param newc is a pointer to a valid icalcomponent for the newly modified version of @p oldc
 * @param user is a pointer to a char string containing a user from the attendee list
 * @param msg is a pointer to a char string containing the message
 *
 * @return a pointer to the newly constructed icalcomponent; or NULL if an error occurred
 * like the specified user was not included in the attendee list.
 */
LIBICAL_ICALSS_EXPORT icalcomponent *icalmessage_new_counterpropose_reply(const icalcomponent *oldc,
                                                                          const icalcomponent *newc,
                                                                          const char *user,
                                                                          const char *msg);
/**
 * Constructs a new icalcomponent for a delegate-to reply for an existing icalcomponent.
 *
 * @param c is a pointer to a valid icalcomponent
 * @param user is a pointer to a char string containing a user from the attendee list
 * @param delegatee is a pointer to a char string containing a delegatee, possibly not
 * already included in the attendee list.
 * @param msg is a pointer to a char string containing the message
 *
 * @return a pointer to the newly constructed icalcomponent; or NULL if an error occurred
 * like the specified user was not included in the attendee list.
 */
LIBICAL_ICALSS_EXPORT icalcomponent *icalmessage_new_delegate_reply(const icalcomponent *c,
                                                                    const char *user,
                                                                    const char *delegatee,
                                                                    const char *msg);

/**
 * Constructs a new icalcomponent for an error reply for an existing icalcomponent.
 *
 * @param c is a pointer to a valid icalcomponent
 * @param user is a pointer to a char string containing a user from the attendee list
 * @param msg is a pointer to a char string containing the error message
 * @param debug is a pointer to a char string to use the debug part of the icalreqstattype to use
 * @param code is a icalrequeststatus code part of the icalreqstattype to use
 *
 * @return a pointer to the newly constructed icalcomponent; or NULL if an error occurred
 * like the specified user was not included in the attendee list.
 */
LIBICAL_ICALSS_EXPORT icalcomponent *icalmessage_new_error_reply(const icalcomponent *c,
                                                                 const char *user,
                                                                 const char *msg,
                                                                 const char *debug,
                                                                 icalrequeststatus code);

#endif /* ICALMESSAGE_H */
