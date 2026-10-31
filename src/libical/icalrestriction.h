/*======================================================================
  FILE: icalrestriction.h
  CREATOR: eric 24 April 1999

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 Contributions from:
    Graham Davison (g.m.davison@computer.org)
======================================================================*/

#ifndef ICALRESTRICTION_H
#define ICALRESTRICTION_H

/**
 * @file icalrestriction.h
 * @brief Functions to check if an icalcomponent meets the restrictions
 *  imposed by the standard.
 */

#include "libical_ical_export.h"
#include "icalcomponent.h"
#include "icalproperty.h"

/**
 * @brief The kinds of icalrestrictions there are
 *
 * These must stay in this order for icalrestriction_compare to work
 */
typedef enum icalrestriction_kind
{
    /** No restriction. */
    ICAL_RESTRICTION_NONE = 0, /* 0 */

    /** Zero. */
    ICAL_RESTRICTION_ZERO, /* 1 */

    /** One. */
    ICAL_RESTRICTION_ONE, /* 2 */

    /** Zero or more. */
    ICAL_RESTRICTION_ZEROPLUS, /* 3 */

    /** One or more. */
    ICAL_RESTRICTION_ONEPLUS, /* 4 */

    /** Zero or one. */
    ICAL_RESTRICTION_ZEROORONE, /* 5 */

    /** Zero or one, exclusive with another property. */
    ICAL_RESTRICTION_ONEEXCLUSIVE, /* 6 */

    /** Zero or one, mutual with another property. */
    ICAL_RESTRICTION_ONEMUTUAL, /* 7 */

    /** Unknown. */
    ICAL_RESTRICTION_UNKNOWN /* 8 */
} icalrestriction_kind;

/**
 * @brief Checks if the given @a count is in accordance with the given
 *  restriction, @a restr.
 * @param restr The restriction to apply to the @a count
 * @param count The amount present that is to be checked against the restriction
 * @return true if the restriction is met, false if not
 *
 * @par Example
 * ```c
 * assert(icalrestriction_compare(ICALRESTRICTION_ONEPLUS, 5) == true);
 * assert(icalrestriction_compare(ICALRESTRICTION_NONE,    3) == false);
 * ```
 */
LIBICAL_ICAL_EXPORT bool icalrestriction_compare(icalrestriction_kind restr, int count);

/**
 * @brief Checks if a given `VCALENDAR` meets all the restrictions imposed by
 *  the standard.
 * @param comp The `VCALENDAR` component to check
 * @return true if the restrictions are met, false if not
 *
 * @par Error handling
 * Returns false and sets ::icalerrno if `NULL` is passed as @a comp, or if the
 * component is not a `VCALENDAR`.
 *
 * @par Example
 * ```c
 * icalcomponent *component = // ...
 *
 * // check component
 * assert(icalrestriction_check(component) == true);
 * ```
 */
LIBICAL_ICAL_EXPORT bool icalrestriction_check(icalcomponent *comp);

#endif /* !ICALRESTRICTION_H */
