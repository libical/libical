/*======================================================================
 * FILE: icaltime_c.h
 *
 * SPDX-FileCopyrightText: 2025 Contributors to the libical project <git@github.com:libical/libical>
 * SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 *
 * The original code is icaltime.c by Eric Busboom
 *======================================================================*/

/*************************************************************************
 * WARNING: USE AT YOUR OWN RISK                                         *
 * These are library internal-only functions.                            *
 * Be warned that these functions can change at any time without notice. *
 *************************************************************************/

#include "icaltime_p.h"
#include "icaltimezoneimpl.h"

icaltime_span icaltime_span_new(struct icaltimetype dtstart, struct icaltimetype dtend, bool is_busy)
{
    icaltime_span span;

    span.is_busy = is_busy;

    span.start = icaltime_as_timet_with_zone(dtstart,
                                             dtstart.zone ? dtstart.zone : icaltimezone_get_utc_timezone());

    span.end = icaltime_as_timet_with_zone(dtend,
                                           dtend.zone ? dtend.zone : icaltimezone_get_utc_timezone());

    return span;
}

bool icaltime_span_overlaps(const icaltime_span *s1, const icaltime_span *s2)
{
    /* s1->start in s2 */
    if (s1->start > s2->start && s1->start < s2->end) {
        return true;
    }

    /* s1->end in s2 */
    if (s1->end > s2->start && s1->end < s2->end) {
        return true;
    }

    /* s2->start in s1 */
    if (s2->start > s1->start && s2->start < s1->end) {
        return true;
    }

    /* s2->end in s1 */
    if (s2->end > s1->start && s2->end < s1->end) {
        return true;
    }

    if (s1->start == s2->start && s1->end == s2->end) {
        return true;
    }

    return false;
}

bool icaltime_span_contains(const icaltime_span *s, const icaltime_span *container)
{
    if ((s->start >= container->start && s->start < container->end) &&
        (s->end <= container->end && s->end > container->start)) {
        return true;
    }

    return false;
}
