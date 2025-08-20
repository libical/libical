/*======================================================================
 FILE: icalcalendar.h
 CREATOR: eric 23 December 1999

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>

 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 The Original Code is eric. The Initial Developer of the Original
 Code is Eric Busboom
======================================================================*/

#ifndef ICALCALENDAR_H
#define ICALCALENDAR_H

#include "libical_icalss_export.h"
#include "icalset.h"

/** @file icalcalendar.h
 *
 * @brief Routines for storing calendar data in a file system.
 *
 * The calendar has two icaldirsets, one for incoming components and one for
 * booked components. It also has interfaces to access the free/busy list and a
 * list of calendar properties
 */

typedef struct icalcalendar_impl icalcalendar;

LIBICAL_ICALSS_EXPORT icalcalendar *icalcalendar_new(const char *dir);

LIBICAL_ICALSS_EXPORT void icalcalendar_free(icalcalendar *calendar);

LIBICAL_ICALSS_EXPORT int icalcalendar_lock(const icalcalendar *calendar);

LIBICAL_ICALSS_EXPORT int icalcalendar_unlock(const icalcalendar *calendar);

LIBICAL_ICALSS_EXPORT int icalcalendar_islocked(const icalcalendar *calendar);

LIBICAL_ICALSS_EXPORT int icalcalendar_ownlock(const icalcalendar *calendar);

LIBICAL_ICALSS_EXPORT icalset *icalcalendar_get_booked(icalcalendar *calendar);

LIBICAL_ICALSS_EXPORT icalset *icalcalendar_get_incoming(icalcalendar *calendar);

LIBICAL_ICALSS_EXPORT icalset *icalcalendar_get_properties(icalcalendar *calendar);

LIBICAL_ICALSS_EXPORT icalset *icalcalendar_get_freebusy(icalcalendar *calendar);

#endif /* !ICALCALENDAR_H */
