/*======================================================================
 FILE: icalgauge.h
 CREATOR: eric 23 December 1999

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 The Original Code is eric. The Initial Developer of the Original
 Code is Eric Busboom
======================================================================*/

/**
 * @file icalgauge.h
 * @brief Routines implementing a filter for ical components
 */

#ifndef ICALGAUGE_H
#define ICALGAUGE_H

#include "libical_icalss_export.h"
#include "icalcomponent.h"

/// @cond PRIVATE
typedef struct icalgauge_impl icalgauge;
/// @endcond

/**
 * Construct a new icalguage from SQL with a specified expand value.
 *
 * @param sql is a pointer to a char string containing a valid SQL command
 * @param expand some integer to associate with this icalgauge
 *
 * @return a pointer to the newly construct icalgauge or NULL if the SQL parser failed.
 */
LIBICAL_ICALSS_EXPORT icalgauge *icalgauge_new_from_sql(const char *sql, int expand);

/**
 * Returns the expand value for the specified icalgauge.
 *
 * @param gauge is a pointer to a valid icalgauge.
 *
 * @return the expanded @p gauge integer value. or -1 if @p gauge is NULL.
 */
LIBICAL_ICALSS_EXPORT int icalgauge_get_expand(const icalgauge *gauge);

/**
 * Frees memory for the specified icalgauge.
 *
 * @param gauge a non-NULL pointer to valid icalgauge
 */
LIBICAL_ICALSS_EXPORT void icalgauge_free(icalgauge *gauge);

/**
 * (Debug) Prints gauge information to STDOUT.
 *
 * @param gauge is a pointer to a vaid icalgauge.
 */
LIBICAL_ICALSS_EXPORT void icalgauge_dump(icalgauge *gauge);

/**
 * Returns true if comp matches the gauge.
 *
 * @param g a pointer to a valid icalgauge
 * @param comp a pointer to a valid icalcomponent
 *
 * The component @p comp must be in canonical form, i.e. a VCALENDAR with
 * either a VEVENT, VTODO or VJOURNAL sub-component.
 *
 * @return true if there's a match; false otherwise.
 */
LIBICAL_ICALSS_EXPORT bool icalgauge_compare(icalgauge *g, icalcomponent *comp);

#endif /* ICALGAUGE_H */
