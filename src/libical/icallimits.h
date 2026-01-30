/**
 *  SPDX-FileCopyrightText: Allen Winter <winter@kde.org>
 *  SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 */

#ifndef LIBICAL_LIMITS_H
#define LIBICAL_LIMITS_H

#include "libical_ical_export.h"

#include <stddef.h>

/**
 * The types of limits
 */
typedef enum icallimits_kind
{
    /** Maximum number of parse failures allowed in a calendar file before processing halts */
    ICAL_LIMIT_PARSE_FAILURES,
    /** Maximum number of parse characters to search in a calendar component for the next parameter or property */
    ICAL_LIMIT_PARSE_SEARCH,
    /** Maximum number of parse failure messages inserted into the output (by `insert_error()`) */
    ICAL_LIMIT_PARSE_FAILURE_ERROR_MESSAGES,
    /** Maximum number of properties allowed in a calendar component */
    ICAL_LIMIT_PROPERTIES,
    /** Maximum number of parameters allowed for a property */
    ICAL_LIMIT_PARAMETERS,
    /** Maximum number of characters for a value */
    ICAL_LIMIT_VALUE_CHARS,
    /** Maximum number of values allowed for multi-valued properties */
    ICAL_LIMIT_PROPERTY_VALUES,
    /** Maximum number of times to search for the next recurrence before giving up */
    ICAL_LIMIT_RECURRENCE_SEARCH,
    /** Maximum number of times to search for the next recurrence before time is considered standing still */
    ICAL_LIMIT_RECURRENCE_TIME_STANDING_STILL,
    /** Maximum number of times to search vtimezone rrules for an occurrence before the specified end year */
    ICAL_LIMIT_RRULE_SEARCH,
} icallimits_kind;

/**
 * Gets the value for the corresponding internal library limit.
 *
 * @param kind is the icallimits_kind of limit to get.
 *
 * @returns The current value of the specified @p kind"
 */
LIBICAL_ICAL_EXPORT size_t icallimit_get(icallimits_kind kind);

/**
 * Sets the value for the corresponding internal library limit.
 *
 * @param kind is the icallimits_kind of limit to set.
 * @param limit is the limit value. Use SIZE_MAX (i.e. maximum value of size_t) to set an unlimited value.
 */
LIBICAL_ICAL_EXPORT void icallimit_set(icallimits_kind kind, size_t limit);

#endif
