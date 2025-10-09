/*======================================================================
 FILE: icalgauge.h
 CREATOR: eric 23 December 1999

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 The Original Code is eric. The Initial Developer of the Original
 Code is Eric Busboom
======================================================================*/

#ifndef ICALGAUGE_H
#define ICALGAUGE_H

#include "libical_icalss_export.h"
#include "icalcomponent.h"

/** @file icalgauge.h
 *  @brief Routines implementing a filter for ical components
 */

typedef struct icalgauge_impl icalgauge;

LIBICAL_ICALSS_EXPORT icalgauge *icalgauge_new_from_sql(const char *sql, int expand);

/**
 * Returns the expand value for the specified icalgauge.
 * If @p gauge is NULL a value of -1 is returned.
 */
LIBICAL_ICALSS_EXPORT int icalgauge_get_expand(const icalgauge *gauge);

LIBICAL_ICALSS_EXPORT void icalgauge_free(icalgauge *gauge);

/** @brief Debug
 *
 * Prints gauge information to STDOUT.
 */
LIBICAL_ICALSS_EXPORT void icalgauge_dump(icalgauge *gauge);

/** @brief Returns true if comp matches the gauge.
 *
 * The component must be in
 * canonical form -- a VCALENDAR with one VEVENT, VTODO or VJOURNAL
 * sub component
 */
LIBICAL_ICALSS_EXPORT int icalgauge_compare(icalgauge *g, icalcomponent *comp);

#endif /* ICALGAUGE_H */
