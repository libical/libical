/*======================================================================
 FILE: icaltypes.h
 CREATOR: eric 20 March 1999

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
======================================================================*/

/**
 * @file icaltypes.h
 * @brief Define internal types and provide functions to manipulate them.
 */

#ifndef ICALTYPES_H
#define ICALTYPES_H

#include "libical_ical_export.h"
#include "icalduration.h"
#include "icalenums.h"
#include "icalperiod.h"

#include <stdbool.h>

/**
 * A data structure to represent a time period.
 *
 * A time period should have either a date-time or a period, but not both.
 */
struct icaldatetimeperiodtype {
    struct icaltimetype time;     /**< the date-time of the time period */
    struct icalperiodtype period; /**< the period of the time period */
};

/** maximum number of chars in the lat/long string encoding */
#define ICAL_GEO_LEN 16
/**
 * A data structure to represent a GEO location.
 */
struct icalgeotype {
    char lat[ICAL_GEO_LEN]; /**< latitude, a double encoded as a string */
    char lon[ICAL_GEO_LEN]; /**< longitude, a double encoded as a string */
};

/**
 * A data structure to represent a time trigger.
 *
 * A trigger should have either a date-time or a duration, but not both.
 */
struct icaltriggertype {
    struct icaltimetype time;         /**< the date-time of the trigger */
    struct icaldurationtype duration; /**< the duration of the trigger */
};

/**
 * Construct an icaltriggertype from number of seconds.
 *
 * The resulting icaltriggertype will a have a corresponding duration setting
 * and a null time setting.
 *
 * @param reltime the number of seconds to use, may be negative or positive.
 *
 * @return an icaltriggertype with a duration of @p reltime.
 */
LIBICAL_ICAL_EXPORT struct icaltriggertype icaltriggertype_from_seconds(const int reltime);

/**
 * Construct an icaltriggertype from a char string representation.
 *
 * @param str a pointer to a char string containing a string representation of an icaltriggertype
 *
 * @return an icaltriggertype corresponding to @p str. If the string representation
 * is invalid, a null icaltriggertype is returned and the library internal error
 * is set to :ICAL_MALFORMEDDATA_ERROR.
 */
LIBICAL_ICAL_EXPORT struct icaltriggertype icaltriggertype_from_string(const char *str);

/**
 * Determine if the specified icaltriggertype is null.
 *
 * A null icaltriggertype has null icaltime or a null icalduration.
 *
 * @param tr is the icaltriggertype to use
 *
 * @return true if @p tr is null; false otherwise.
 * @see icaltriggertype_is_bad_trigger
 */
LIBICAL_ICAL_EXPORT bool icaltriggertype_is_null_trigger(struct icaltriggertype tr);

/**
 * Determine if the specified icaltriggertype is valid.
 *
 * @param tr is the icaltriggertype to use
 *
 * @return true if @p tr is valid; false otherwise.
 * @see icaltriggertype_is_null_trigger
 */
LIBICAL_ICAL_EXPORT bool icaltriggertype_is_bad_trigger(struct icaltriggertype tr);

/**
 * A datatype to represent a request status.
 *
 * This struct contains two string pointers:
 *  - "desc" string is a pointer to a static table inside the library.
 *    Don't try to free it.
 *  - "debug" string is owned by the icalvalue and should not be freed manually.
 *
 * @note get the original string from icalproperty_get_requeststatus() or
 * icalvalue_get_text() when operating on the value of a request_status property.
 */
struct icalreqstattype {
    icalrequeststatus code; /**< the status code "major.minor" */
    const char *desc;       /**< the request status description */
    const char *debug;      /**< the request status debug string */
};

/**
 * Create an icalreqstattype from a character string.
 *
 * @param str a pointer to a char string containing the string representation of an icalreqstattype
 *
 * @return the icalreqstattype representation of @p str.  An unknown icalreqstattype
 * (stat.code is ::ICAL_UNKNOWN_STATUS) is returned if @p str is invalid.
 */
LIBICAL_ICAL_EXPORT struct icalreqstattype icalreqstattype_from_string(const char *str);

/**
 * Represent the specified icalreqstattype as a string.
 *
 * @param stat the icalreqstattype to use
 *
 * @return a pointer to a char string containing the string representation of @p st
at.
 * NULL is returned only in case of a memory allocation failure.
 *
 * @see icalreqstattype_as_string_r
 */
LIBICAL_ICAL_EXPORT const char *icalreqstattype_as_string(struct icalreqstattype);

/**
 * Represent the specified icalreqstattype as a string.
 *
 * @param stat the icalreqstattype to use
 *
 * @return a pointer to a char string containing the string representation of @p stat.
 * NULL is returned only in case of a memory allocation failure.
 *
 * @see icalreqstattype_as_string
 */
LIBICAL_ICAL_EXPORT char *icalreqstattype_as_string_r(struct icalreqstattype stat);

/**
 * How the library should handle unknown parameters.
 */
typedef enum ical_unknown_token_handling
{
    /** assume IANA (currently unused) */
    ICAL_ASSUME_IANA_TOKEN = 1,
    /** discard and continue normally */
    ICAL_DISCARD_TOKEN = 2,
    /** treat as a error; the internal icalerror to :ICAL_MALFORMEDDATA_ERROR */
    ICAL_TREAT_AS_ERROR = 3
} ical_unknown_token_handling;

/**
 * Gets how the library handles unknown tokens.
 *
 * The default library setting is ::ICAL_TREAT_AS_ERROR.
 *
 * @return the library's current ical_unknown_token_handling setting.
 * @see ical_set_unknown_token_handling_setting
 */
LIBICAL_ICAL_EXPORT ical_unknown_token_handling ical_get_unknown_token_handling_setting(void);

/**
 * Sets how the library handles unknown tokens.
 *
 * @param newSetting is the ical_unknown_token_handling to use
 *
 * @see ical_get_unknown_token_handling_setting
 */
LIBICAL_ICAL_EXPORT void ical_set_unknown_token_handling_setting(
    ical_unknown_token_handling newSetting);

#endif /* !ICALTYPES_H */
