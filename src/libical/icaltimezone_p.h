/*
 * Authors :
 *  Chenthill Palanisamy <pchenthill@novell.com>
 *
 * SPDX-FileCopyrightText: 2007, Novell, Inc.
 * SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 */

#ifndef ICALTIMEZONE_P_H
#define ICALTIMEZONE_P_H

#include "libical_ical_export.h"
#include "icalcomponent.h"

#define ZONES_TAB_SYSTEM_FILENAME "zone.tab"

/**
 * Returns a pointer to a timezone icalcomponent corresponding to the specified location
 * (a file residing in the zoneinfo).
 *
 * @param location is a string containing the name of a location with a timezone file
 *        found under the zoneinfo data.
 *
 * @since 4.0 no longer publicly accessible
 */
LIBICAL_ICAL_NO_EXPORT icalcomponent *icaltimezone_fetch_timezone(const char *location);

#endif
