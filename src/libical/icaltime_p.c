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

void icaltime_adjust(struct icaltimetype *tt,
                     const int days, const int hours,
                     const int minutes, const int seconds)
{
    int second, minute, hour, day;
    int minutes_overflow, hours_overflow, days_overflow = 0, years_overflow;
    int days_in_month;

    /* If we are passed a date make sure to ignore hour minute and second */
    if (tt->is_date)
        goto IS_DATE;

    /* Add on the seconds. */
    second = tt->second + seconds;
    tt->second = second % 60;
    minutes_overflow = second / 60;
    if (tt->second < 0) {
        tt->second += 60;
        minutes_overflow--;
    }

    /* Add on the minutes. */
    minute = tt->minute + minutes + minutes_overflow;
    tt->minute = minute % 60;
    hours_overflow = minute / 60;
    if (tt->minute < 0) {
        tt->minute += 60;
        hours_overflow--;
    }

    /* Add on the hours. */
    hour = tt->hour + hours + hours_overflow;
    tt->hour = hour % 24;
    days_overflow = hour / 24;
    if (tt->hour < 0) {
        tt->hour += 24;
        days_overflow--;
    }

IS_DATE:
    /* Normalize the month. We do this before handling the day since we may
     *       need to know what month it is to get the number of days in it.
     *       Note that months are 1 to 12, so we have to be a bit careful. */
    if (tt->month >= 13) {
        years_overflow = (tt->month - 1) / 12;
        tt->year += years_overflow;
        tt->month -= years_overflow * 12;
    } else if (tt->month <= 0) {
        /* 0 to -11 is -1 year out, -12 to -23 is -2 years. */
        years_overflow = (tt->month / 12) - 1;
        tt->year += years_overflow;
        tt->month -= years_overflow * 12;
    }

    /* Add on the days. */
    day = tt->day + days + days_overflow;
    if (day > 0) {
        for (;;) {
            days_in_month = icaltime_days_in_month(tt->month, tt->year);
            if (day <= days_in_month)
                break;

            tt->month++;
            if (tt->month >= 13) {
                tt->year++;
                tt->month = 1;
            }

            day -= days_in_month;
        }
    } else {
        while (day <= 0) {
            if (tt->month == 1) {
                tt->year--;
                tt->month = 12;
            } else {
                tt->month--;
            }

            day += icaltime_days_in_month(tt->month, tt->year);
        }
    }
    tt->day = day;
}

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
    if (s1->start > s2->start && s1->start < s2->end)
        return true;

    /* s1->end in s2 */
    if (s1->end > s2->start && s1->end < s2->end)
        return true;

    /* s2->start in s1 */
    if (s2->start > s1->start && s2->start < s1->end)
        return true;

    /* s2->end in s1 */
    if (s2->end > s1->start && s2->end < s1->end)
        return true;

    if (s1->start == s2->start && s1->end == s2->end)
        return true;

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
