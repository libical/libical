/*======================================================================
 FILE: icaltypes.h
 CREATOR: eric 20 March 1999

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
======================================================================*/

#ifndef ICALTYPES_H
#define ICALTYPES_H

#include "libical_ical_export.h"
#include "icalduration.h"
#include "icalenums.h"
#include "icalperiod.h"

#include <stdbool.h>

struct icaldatetimeperiodtype {
    struct icaltimetype time;
    struct icalperiodtype period;
};

#define ICAL_GEO_LEN 16
struct icalgeotype {
    char lat[ICAL_GEO_LEN];
    char lon[ICAL_GEO_LEN];
};

struct icaltriggertype {
    struct icaltimetype time;
    struct icaldurationtype duration;
};

LIBICAL_ICAL_EXPORT struct icaltriggertype icaltriggertype_from_seconds(const int reltime);

LIBICAL_ICAL_EXPORT struct icaltriggertype icaltriggertype_from_string(const char *str);

LIBICAL_ICAL_EXPORT bool icaltriggertype_is_null_trigger(struct icaltriggertype tr);

LIBICAL_ICAL_EXPORT bool icaltriggertype_is_bad_trigger(struct icaltriggertype tr);

/* struct icalreqstattype. This struct contains two string pointers.
The "desc" string is a pointer to a static table inside the library.
Don't try to free it. The "debug" string is owned by the icalvalue
and should not be freed manually.

BTW, you would get that original string from
*icalproperty_get_requeststatus() or icalvalue_get_text(), when
operating on the value of a request_status property. */

struct icalreqstattype {
    icalrequeststatus code;
    const char *desc;
    const char *debug;
};

LIBICAL_ICAL_EXPORT struct icalreqstattype icalreqstattype_from_string(const char *str);

LIBICAL_ICAL_EXPORT const char *icalreqstattype_as_string(struct icalreqstattype);

LIBICAL_ICAL_EXPORT char *icalreqstattype_as_string_r(struct icalreqstattype);

/* ical_unknown_token_handling :
 *    How should the ICAL library handle components, properties and parameters with
 *    unknown names?
 *    FIXME:  Currently only affects parameters.  Extend to components and properties.
 */
typedef enum ical_unknown_token_handling
{
    ICAL_ASSUME_IANA_TOKEN = 1,
    ICAL_DISCARD_TOKEN = 2,
    ICAL_TREAT_AS_ERROR = 3
} ical_unknown_token_handling;

LIBICAL_ICAL_EXPORT ical_unknown_token_handling ical_get_unknown_token_handling_setting(void);

LIBICAL_ICAL_EXPORT void ical_set_unknown_token_handling_setting(
    ical_unknown_token_handling newSetting);

#endif /* !ICALTYPES_H */
