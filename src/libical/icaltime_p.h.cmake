/*======================================================================
 * FILE: icaltime_p.h
 *
 * SPDX-FileCopyrightText: 2025 Contributors to the libical project <git@github.com:libical/libical>
 * SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 *
 * The original code is icaltime.h by Eric Busboom
 *======================================================================*/

/*************************************************************************
 * WARNING: USE AT YOUR OWN RISK                                         *
 * These are library internal-only functions.                            *
 * Be warned that these functions can change at any time without notice. *
 *************************************************************************/

#ifndef ICALTIME_P_H
#define ICALTIME_P_H

/*
 * Time manipulation functions.
 */

#include "libical_ical_export.h"

#include <stdbool.h>
#include <time.h>

#define icaltime_t ${ICAL_ICALTIME_T_TYPE}

struct icaltime_span
{
    icaltime_t start;       /**< in UTC */
    icaltime_t end;         /**< in UTC */
    int is_busy;            /**< 1->busy time, 0-> free time */
};

typedef struct icaltime_span icaltime_span;

struct icaltimetype; // forwarding declaration

/**
 * Adds or subtracts a number of days, hours, minutes and seconds.
 *
 * Adds or subtracts a time from an icaltimetype. This time is given
 * as a number of days, hours, minutes and seconds.
 *
 * @note This function is exactly the same as
 * icaltimezone_adjust_change() except for the type of the first parameter.
 */
LIBICAL_ICAL_EXPORT void icaltime_adjust(struct icaltimetype *tt,
                                         const int days, const int hours,
                                         const int minutes, const int seconds);

/**
 *  @brief Builds an icaltimespan given a start time, end time and busy value.
 *
 *  @param dtstart   The beginning time of the span, can be a date-time
 *                   or just a date.
 *  @param dtend     The end time of the span.
 *  @param is_busy   A boolean value, false/true.
 *  @returns         A span using the supplied values. The times are specified in UTC.
 */
LIBICAL_ICAL_EXPORT struct icaltime_span icaltime_span_new(struct icaltimetype dtstart,
                                                           struct icaltimetype dtend, bool is_busy);

/** @brief Returns true if the two spans overlap.
 *
 *  @param s1         First span to test
 *  @param s2         Second span to test
 *  @return           boolean value
 *
 *  The result is calculated by testing if the start time of s1 is contained
 *  by the s2 span, or if the end time of s1 is contained by the s2 span.
 *
 *  Also returns true if the spans are equal.
 *
 *  Note, this will return false if the spans are adjacent.
 */
LIBICAL_ICAL_EXPORT bool icaltime_span_overlaps(const struct icaltime_span *s1, const struct icaltime_span *s2);

/** @brief Returns true if the span is totally within the containing
 *  span.
 *
 *  @param s          The span to test for.
 *  @param container  The span to test against.
 *  @return           boolean value.
 *
 */
LIBICAL_ICAL_EXPORT bool icaltime_span_contains(const struct icaltime_span *s, const struct icaltime_span *container);

#endif
