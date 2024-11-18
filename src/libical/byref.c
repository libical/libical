// There are multiple languages that only support pointers but not structures directly as interop.
// This file functions as a mapper to oversee this.

/*======================================================================
 FILE: byref.c
 CREATOR: Siam.Smeets 18 Nov 24

 SPDX-FileCopyrightText: 2024, Siam Smeets <siam.smeets@dataaccess.eu>

 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

======================================================================*/

/**
 * @file byref.c
 * @brief A set of function wrappers to handle non-structured return type interop.
 *
 * Not all programming languages managed or not have support for c-structured return types.
 * To allow interop with these programming languages this file, over time, will contain wrapper functions,
 * That wrap the return type to a "byref" or pointer variable which is used as output.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "icalcomponent.h"
#include "icaltime.h"
#include "astime.h"
#include "icalerror.h"
#include "icalmemory.h"
#include "icaltimezone.h"

#include <ctype.h>
#include <stdlib.h>

/** @brief Convenience constructor (ret-wrapped)
 *
 * @returns The current time in the given timezone, as an icaltimetype, in the last parameter.
 */
LIBICAL_ICAL_EXPORT void icaltime_current_time_with_zone_ex(const icaltimezone *zone, struct icaltimetype *ret)
{
    *ret = icaltime_current_time_with_zone(zone);
}
