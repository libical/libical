/*======================================================================
 FILE: icalduration.c
 CREATOR: eric 02 June 2000

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 The Original Code is eric. The Initial Developer of the Original
 Code is Eric Busboom
======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "icalduration.h"
#include "icalerror.h"
#include "icalmemory.h"
#include "icaltime.h"
#include "icaltimezone.h"

/* From Seth Alves, <alves@hungry.com>   */
struct icaldurationtype icaldurationtype_from_int(int t)
{
    struct icaldurationtype dur;

    dur = icaldurationtype_null_duration();

    if (t < 0) {
        dur.is_neg = 1;
        t = -t;
    }
    dur.seconds = (unsigned int)t;

    return dur;
}

struct icaldurationtype icaldurationtype_from_string(const char *str)
{
    int i;
    int begin_flag = 0;
    int time_flag = 0;
    int date_flag = 0;
    int digits = -1;
    int scan_size = -1;
    int size = (int)strlen(str);
    char p;
    struct icaldurationtype d;

    memset(&d, 0, sizeof(struct icaldurationtype));

    for (i = 0; i != size; i++) {
        p = str[i];

        switch (p) {
        case '+': {
            if (i != 0 || begin_flag == 1) {
                goto error;
            }
            break;
        }
        case '-': {
            if (i != 0 || begin_flag == 1) {
                goto error;
            }
            d.is_neg = 1;
            break;
        }

        case 'P': {
            if (i != 0 && i != 1) {
                goto error;
            }
            begin_flag = 1;
            break;
        }

        case 'T': {
            time_flag = 1;
            break;
        }

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': {
            /* HACK.
                   Skip any more digits if the last one read has not been assigned */
            if (digits != -1) {
                break;
            }

            if (begin_flag == 0) {
                goto error;
            }
            /* Get all of the digits, not one at a time */
            scan_size = sscanf(&str[i], "%10d", &digits); /*limit to 10digits.
                                                                  increase as needed */
            if (scan_size == 0) {
                goto error;
            }
            break;
        }

        case 'H': {
            if (time_flag == 0 || d.hours != 0 || digits == -1) {
                goto error;
            }
            d.hours = (unsigned int)digits;
            digits = -1;
            break;
        }
        case 'M': {
            if (time_flag == 0 || d.minutes != 0 || digits == -1) {
                goto error;
            }
            d.minutes = (unsigned int)digits;
            digits = -1;
            break;
        }
        case 'S': {
            if (time_flag == 0 || d.seconds != 0 || digits == -1) {
                goto error;
            }
            d.seconds = (unsigned int)digits;
            digits = -1;
            break;
        }
        case 'W': {
            if (time_flag == 1 || date_flag == 1 || d.weeks != 0 || digits == -1) {
                goto error;
            }
            d.weeks = (unsigned int)digits;
            digits = -1;
            break;
        }
        case 'D': {
            if (time_flag == 1 || d.days != 0 || digits == -1) {
                goto error;
            }
            date_flag = 1;
            d.days = (unsigned int)digits;
            digits = -1;
            break;
        }
        default: {
            goto error;
        }
        }
    }

    return d;

error:
    icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
    return icaldurationtype_bad_duration();
}

static void append_duration_segment(char **buf, char **buf_ptr, size_t *buf_size,
                                    const char *sep, unsigned int value)
{
    char temp[1024];

    snprintf(temp, sizeof(temp), "%u", value);

    icalmemory_append_string(buf, buf_ptr, buf_size, temp);
    icalmemory_append_string(buf, buf_ptr, buf_size, sep);
}

char *icaldurationtype_as_ical_string(struct icaldurationtype d)
{
    char *buf;

    buf = icaldurationtype_as_ical_string_r(d);
    icalmemory_add_tmp_buffer(buf);
    return buf;
}

char *icaldurationtype_as_ical_string_r(struct icaldurationtype d)
{
    char *buf;
    size_t buf_size = 256;
    char *buf_ptr = 0;

    buf = (char *)icalmemory_new_buffer(buf_size);
    buf_ptr = buf;

    if (d.weeks == 0 &&
        d.days == 0 &&
        d.hours == 0 &&
        d.minutes == 0 &&
        d.seconds == 0) {
        icalmemory_append_string(&buf, &buf_ptr, &buf_size, "PT0S");
    } else {
        if (d.is_neg == 1) {
            icalmemory_append_char(&buf, &buf_ptr, &buf_size, '-');
        }

        icalmemory_append_char(&buf, &buf_ptr, &buf_size, 'P');

        if (d.weeks != 0) {
            append_duration_segment(&buf, &buf_ptr, &buf_size, "W", d.weeks);
        }

        if (d.days != 0) {
            append_duration_segment(&buf, &buf_ptr, &buf_size, "D", d.days);
        }

        if (d.hours != 0 || d.minutes != 0 || d.seconds != 0) {
            icalmemory_append_string(&buf, &buf_ptr, &buf_size, "T");

            if (d.hours != 0) {
                append_duration_segment(&buf, &buf_ptr, &buf_size, "H", d.hours);
            }
            if (d.minutes != 0) {
                append_duration_segment(&buf, &buf_ptr, &buf_size, "M", d.minutes);
            }
            if (d.seconds != 0) {
                append_duration_segment(&buf, &buf_ptr, &buf_size, "S", d.seconds);
            }
        }
    }

    return buf;
}

int icaldurationtype_as_int(struct icaldurationtype dur)
{
    if (dur.days != 0 || dur.weeks != 0) {
        /* The length of a day is position-dependent */
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        return 0;
    }
    return (int)(((int)dur.seconds +
                  60 * ((int)dur.minutes +
                        60 * ((int)dur.hours))) *
                 (dur.is_neg == 1 ? -1 : 1));
}

struct icaldurationtype icaldurationtype_null_duration(void)
{
    struct icaldurationtype d;

    memset(&d, 0, sizeof(struct icaldurationtype));

    return d;
}

bool icaldurationtype_is_null_duration(struct icaldurationtype d)
{
    struct icaldurationtype n = icaldurationtype_null_duration();
    return memcmp(&d, &n, sizeof(struct icaldurationtype)) ? false : true;
}

/* In icalvalue_new_from_string_with_error, we should not call
   icaldurationtype_is_null_duration() to see if there is an error
   condition. Null duration is perfectly valid for an alarm.
   We cannot depend on the caller to check icalerrno either,
   following the philosophy of unix errno. we set the is_neg
   to -1 to indicate that this is a bad duration.
*/
struct icaldurationtype icaldurationtype_bad_duration(void)
{
    struct icaldurationtype d;

    memset(&d, 0, sizeof(struct icaldurationtype));
    d.is_neg = -1;
    return d;
}

bool icaldurationtype_is_bad_duration(struct icaldurationtype d)
{
    return (d.is_neg == -1);
}

struct icaltimetype icaltime_add(struct icaltimetype t, struct icaldurationtype d)
{
    struct icaltimetype t_days;
    if (t.is_date &&
        (d.seconds != 0 ||
         d.minutes != 0 ||
         d.hours != 0)) {
        /* We can't add time durations to dates */
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        return icaltime_null_time();
    }

    /* Days are nominal (not exact) and position-dependent */
    if (!d.is_neg) {
        t.day += (int)d.days;
        t.day += (int)(d.weeks * 7);
    } else {
        t.day -= (int)d.days;
        t.day -= (int)(d.weeks * 7);
    }

    t = icaltime_normalize(t);
    t_days = t;

    if (!d.is_neg) {
        t.second += (int)d.seconds;
        t.minute += (int)d.minutes;
        t.hour += (int)d.hours;
    } else {
        t.second -= (int)d.seconds;
        t.minute -= (int)d.minutes;
        t.hour -= (int)d.hours;
    }

    t = icaltime_normalize(t);
    /* RFC 5545 states:
     * In the case of discontinuities in the time scale, such
     * as the change from standard time to daylight time and back, the
     * computation of the exact duration requires the subtraction or
     * addition of the change of duration of the discontinuity.
     */
    icaltime_adjust(&t, 0, 0, 0,
                    icaltimezone_get_utc_offset((icaltimezone *)t.zone, &t, &t.is_daylight) -
                        icaltimezone_get_utc_offset((icaltimezone *)t.zone, &t_days, &t_days.is_daylight));

    return t;
}

struct icaldurationtype icaltime_subtract(struct icaltimetype t1, struct icaltimetype t2)
{
    if ((!t1.is_date && t2.is_date) ||
        (t1.is_date && !t2.is_date)) {
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        return icaldurationtype_bad_duration();
    }

    struct icaldurationtype ret;
    if (t1.is_date) {
        icaltime_t t1t = icaltime_as_timet(t1);
        icaltime_t t2t = icaltime_as_timet(t2);
        ret = icaldurationtype_null_duration();
        ret.days = ((unsigned int)(t1t - t2t)) / (60 * 60 * 24);
    } else {
        icaltime_t t1t = icaltime_as_timet_with_zone(t1, t1.zone);
        icaltime_t t2t = icaltime_as_timet_with_zone(t2, t2.zone);

        ret = icaldurationtype_from_int((int)(t1t - t2t));
    }
    return ret;
}
