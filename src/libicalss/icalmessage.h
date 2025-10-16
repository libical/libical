/*======================================================================
 FILE: icalmessage.h
 CREATOR: eric 07 Nov 2000

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 =========================================================================*/

#ifndef ICALMESSAGE_H
#define ICALMESSAGE_H

#include "libical_icalss_export.h"
#include "icalcomponent.h"

LIBICAL_ICALSS_EXPORT icalcomponent *icalmessage_new_accept_reply(const icalcomponent *c,
                                                                  const char *user,
                                                                  const char *msg);

LIBICAL_ICALSS_EXPORT icalcomponent *icalmessage_new_decline_reply(const icalcomponent *c,
                                                                   const char *user,
                                                                   const char *msg);

/* New is modified version of old */
LIBICAL_ICALSS_EXPORT icalcomponent *icalmessage_new_counterpropose_reply(const icalcomponent *oldc,
                                                                          const icalcomponent *newc,
                                                                          const char *user,
                                                                          const char *msg);

LIBICAL_ICALSS_EXPORT icalcomponent *icalmessage_new_delegate_reply(const icalcomponent *c,
                                                                    const char *user,
                                                                    const char *delegatee,
                                                                    const char *msg);

LIBICAL_ICALSS_EXPORT icalcomponent *icalmessage_new_error_reply(const icalcomponent *c,
                                                                 const char *user,
                                                                 const char *msg,
                                                                 const char *debug,
                                                                 icalrequeststatus code);

#endif /* ICALMESSAGE_H */
