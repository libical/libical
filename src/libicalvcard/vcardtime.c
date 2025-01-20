/*======================================================================
 FILE: vcardtime.c

 CREATOR: Ken Murchison 24 Aug 2022

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)

 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 ======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "vcardtime.h"
#include "icalmemory.h"
#include "icaltime.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TIME_BUF_SIZE 21

vcardtimetype vcardtime_null_datetime(void)
{
    vcardtimetype t = {-1, -1, -1, -1, -1, -1, -1};

    return t;
}

vcardtimetype vcardtime_current_utc_time(void)
{
    time_t now = time(0);
    struct tm t;
    vcardtimetype tt;

    memset(&t, 0, sizeof(struct tm));
    if (!icalgmtime_r(&now, &t)) {
        return vcardtime_null_datetime();
    }

    tt.year = t.tm_year + 1900;
    tt.month = t.tm_mon + 1;
    tt.day = t.tm_mday;
    tt.hour = t.tm_hour;
    tt.minute = t.tm_min;
    tt.second = t.tm_sec;
    tt.utcoffset = 0;

    return tt;
}

bool vcardtime_is_time(const vcardtimetype t)
{
    return (t.year == -1 && t.month == -1 && t.day == -1);
}

bool vcardtime_is_date(const vcardtimetype t)
{
    return (t.hour == -1 && t.minute == -1 && t.second == -1);
}

bool vcardtime_is_null_datetime(const vcardtimetype t)
{
    return (vcardtime_is_time(t) && vcardtime_is_date(t));
}

bool vcardtime_is_datetime(const vcardtimetype t)
{
    return (t.day != -1 && t.hour != -1);
}

bool vcardtime_is_timestamp(const vcardtimetype t)
{
    return (t.year != -1 && t.month != -1 && t.day != -1 &&
            t.hour != -1 && t.minute != -1 && t.second != -1 &&
            t.utcoffset != -1);
}

bool vcardtime_is_utc(const vcardtimetype t)
{
    return (t.utcoffset == 0 && !vcardtime_is_date(t));
}

bool vcardtime_is_leap_year(const int year)
{
    if (year == -1) {
        return true;
    } else if (year <= 1752) {
        return (year % 4 == 0);
    } else {
        return ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
    }
}

static const int days_in_month[] =
    {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

bool vcardtime_is_valid_time(const struct vcardtimetype t)
{
    int days;

    if (t.year > 3000 || t.day == 0 ||
        t.hour > 23 || t.minute > 59 || t.second > 60 ||
        t.utcoffset < -720 || t.utcoffset > 840) {
        return false;
    }

    if (t.minute == -1 && t.hour != -1 && t.second != -1)
        return false;

    switch (t.month) {
    case 0:
        return false;

    case -1:
        if (t.year != -1 && t.day != -1)
            return false;

        days = 31;
        break;

    case 2:
        days = 28 + vcardtime_is_leap_year(t.year);
        break;

    default:
        if (t.month > 12)
            return false;

        days = days_in_month[t.month];
        break;
    }

    if (t.day > days)
        return false;

    return true;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
static int sprintf_date(const vcardtimetype t, unsigned flags,
                        char *buf, size_t size)
{
    /*
      date = year    [month  day]
           / year "-" month
           / "--"     month [day]
           / "--"      "-"   day
    */
    const char *fmt;

    if (!(flags & VCARDTIME_AS_V4)) {
        return snprintf(buf, size, "%04d%02d%02d",
                        t.year == -1 ? 0 : t.year,
                        t.month == -1 ? 1 : t.month,
                        t.day == -1 ? 1 : t.day);
    } else if (t.year != -1) {
        if (t.month == -1) {
            fmt = "%04d";
        } else if (t.day == -1) {
            fmt = "%04d-%02d";
        } else {
            fmt = "%04d%02d%02d";
        }

        /* cppcheck-suppress wrongPrintfScanfArgNum */
        return snprintf(buf, size, fmt, t.year, t.month, t.day);
    } else if (t.month != -1) {
        if (t.day == -1) {
            fmt = "--%02d";
        } else {
            fmt = "--%02d%02d";
        }

        /* cppcheck-suppress wrongPrintfScanfArgNum */
        return snprintf(buf, size, fmt, t.month, t.day);
    } else {
        return snprintf(buf, size, "---%02d", t.day);
    }
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
static int sprintf_time(const vcardtimetype t, unsigned flags,
                        char *buf, size_t size)
{
    /*
      time = ["T"] hour [minute [second]] [zone]
           / ["T"]  "-"  minute [second]
           / ["T"]  "-"   "-"    second
    */
    const char *fmt;
    int n;

    if (!(flags & VCARDTIME_BARE_TIME)) {
        strncat(buf, "T", size);
        buf++;
        size--;
    }

    if (!(flags & VCARDTIME_AS_V4)) {
        n = snprintf(buf, size, "%02d%02d%02d",
                     t.hour == -1 ? 0 : t.hour,
                     t.minute == -1 ? 0 : t.minute,
                     t.second == -1 ? 0 : t.second);
    } else if (t.hour != -1) {
        /* hour [minute [second]] [zone] */
        if (t.minute == -1) {
            /* hour */
            fmt = "%02d";
        } else if (t.second == -1) {
            /* hour minute */
            fmt = "%02d%02d";
        } else {
            /* hour minute second */
            fmt = "%02d%02d%02d";
        }

        /* cppcheck-suppress wrongPrintfScanfArgNum */
        n = snprintf(buf, size, fmt, t.hour, t.minute, t.second);
    } else if (t.minute != -1) {
        /* "-"  minute [second] */
        if (t.second == -1) {
            /* "-" minute */
            fmt = "-%02d";
        } else {
            /* "-" minute second */
            fmt = "-%02d%02d";
        }

        /* cppcheck-suppress wrongPrintfScanfArgNum */
        return snprintf(buf, size, fmt, t.minute, t.second);
    } else {
        /* "-" "-" second */
        return snprintf(buf, size, "--%02d", t.second);
    }

    if (t.utcoffset != -1) {
        /* zone = "Z" / ( sign hour minute ) */
        buf += n;
        size -= (size_t)n;

        if (t.utcoffset == 0) {
            strncpy(buf, "Z", size);
            n++;
        } else {
            n += snprintf(buf, size, "%+03d%02d",
                          t.utcoffset / 60, abs(t.utcoffset % 60));
        }
    }

    return n;
}
#pragma GCC diagnostic pop

char *vcardtime_as_vcard_string_r(const vcardtimetype t, unsigned flags)
{
    size_t size = TIME_BUF_SIZE;
    char *ptr, *buf;
    int n;

    ptr = buf = icalmemory_new_buffer(size);

    if (!(flags & VCARDTIME_AS_V4) || !vcardtime_is_time(t)) {
        n = sprintf_date(t, flags, ptr, size);
        ptr += n;
        size -= (size_t)n;
    }
    if (!vcardtime_is_date(t)) {
        (void)sprintf_time(t, flags, ptr, size);
    }

    return buf;
}

const char *vcardtime_as_vcard_string(const vcardtimetype t, unsigned flags)
{
    char *buf;

    buf = vcardtime_as_vcard_string_r(t, flags);
    icalmemory_add_tmp_buffer(buf);
    return buf;
}

#define num_digits(s) strspn(s, "0123456789")

static const char *sscanf_date(const char *str, vcardtimetype *t)
{
    /*
      date = year    [month  day]
           / year "-" month
           / "--"     month [day]
           / "--"      "-"   day
    */
    const char *month;
    size_t ndig;
    int nchar = 0;
    char *newstr;

    if (!str || !*str) {
        /* empty string */
        return NULL;
    } else if (!strncmp(str, "--", 2)) {
        month = str + 2;

        if (*month == '-') {
            ndig = num_digits(month + 1);

            if (ndig == 2) {
                sscanf(str, "---%2u%n", (unsigned *)&t->day, &nchar);
            }
        } else {
            ndig = num_digits(month);

            if (ndig == 4) {
                sscanf(str, "--%2u%2u%n",
                       (unsigned *)&t->month, (unsigned *)&t->day, &nchar);
            } else if (ndig == 2) {
                sscanf(str, "--%2u%n", (unsigned *)&t->month, &nchar);
            }
        }
    } else {
        ndig = num_digits(str);

        if (ndig == 8) {
            sscanf(str, "%4u%2u%2u%n",
                   (unsigned *)&t->year, (unsigned *)&t->month,
                   (unsigned *)&t->day, &nchar);
        } else if (ndig == 4) {
            month = str + 4;

            if (!*month) {
                sscanf(str, "%4u%n", (unsigned *)&t->year, &nchar);
            } else if (*month == '-') {
                ndig = num_digits(++month);

                if (ndig == 2) {
                    if (month[2] == '-') {
                        sscanf(str, "%4u-%2u-%2u%n",
                               (unsigned *)&t->year, (unsigned *)&t->month,
                               (unsigned *)&t->day, &nchar);
                    } else {
                        sscanf(str, "%4u-%2u%n",
                               (unsigned *)&t->year, (unsigned *)&t->month,
                               &nchar);
                    }
                }
            }
        }
    }

    if (!nchar) {
        /* invalid time */
        return NULL;
    }

    newstr = (char *)str + nchar;
    return newstr;
}

static const char *sscanf_zone(const char *str, vcardtimetype *t)
{
    /*
      zone = "Z"
           / ( "+" / "-" ) hour [minute]
    */
    unsigned offset_h = 0, offset_m = 0;
    char sign[2] = "";
    size_t ndig;
    char *newstr;
    int nchar = 0;

    if (!str || !*str) {
        /* empty string */
        return NULL;
    } else if (*str == 'Z') {
        nchar = 1;
    } else if (strchr("+-", *str)) {
        ndig = num_digits(str + 1);

        if (ndig == 4) {
            sscanf(str, "%1[+-]%2u%2u%n", sign, &offset_h, &offset_m, &nchar);
        } else if (ndig == 2) {
            sscanf(str, "%1[+-]%2u%n", sign, &offset_h, &nchar);
        }
    }

    if (!nchar) {
        /* invalid zone */
        return NULL;
    }

    t->utcoffset = (int)(60 * offset_h + offset_m);
    if (*sign == '-')
        t->utcoffset = -t->utcoffset;

    newstr = (char *)str + nchar;
    return newstr;
}

static const char *sscanf_time(const char *str, vcardtimetype *t)
{
    /*
      time = hour [ ":" minute ":" second   [ "." secfrac ] ]
           / hour [     minute    [second]] [zone]
           /  "-"       minute    [second]
           /  "-"         "-"      second
    */
    unsigned secfrac;
    size_t ndig;
    int nchar = 0;

    if (!str || !*str) {
        /* empty string */
        return NULL;
    } else if (*str == '-') {
        if (str[1] == '-') {
            ndig = num_digits(str + 2);

            if (ndig == 2) {
                sscanf(str, "--%2u%n", (unsigned *)&t->second, &nchar);
            }
        } else {
            ndig = num_digits(str + 1);

            if (ndig == 4) {
                sscanf(str, "-%2u%2u%n",
                       (unsigned *)&t->minute, (unsigned *)&t->second, &nchar);
            } else if (ndig == 2) {
                sscanf(str, "-%2u%n", (unsigned *)&t->minute, &nchar);
                t->second = 0;
            }
        }
    } else {
        ndig = num_digits(str);

        if (ndig == 6) {
            sscanf(str, "%2u%2u%2u%n",
                   (unsigned *)&t->hour, (unsigned *)&t->minute,
                   (unsigned *)&t->second, &nchar);
        } else if (ndig == 4) {
            sscanf(str, "%2u%2u%n",
                   (unsigned *)&t->hour, (unsigned *)&t->minute, &nchar);
            t->second = 0;
        } else if (ndig == 2) {
            if (str[2] == ':') {
                if (str[8] == '.') {
                    sscanf(str, "%2u:%2u:%2u.%u%n",
                           (unsigned *)&t->hour, (unsigned *)&t->minute,
                           (unsigned *)&t->second, (unsigned *)&secfrac, &nchar);
                } else {
                    sscanf(str, "%2u:%2u:%2u%n",
                           (unsigned *)&t->hour, (unsigned *)&t->minute,
                           (unsigned *)&t->second, &nchar);
                }
            } else {
                sscanf(str, "%2u%n", (unsigned *)&t->hour, &nchar);
            }
        }
    }

    if (!nchar) {
        /* invalid time */
        return NULL;
    }

    str += nchar;

    if (t->hour != -1 && *str) {
        str = sscanf_zone(str, t);
    }

    return str;
}

vcardtimetype vcardtime_from_string(const char *str, int is_bare_time)
{
    vcardtimetype t = {-1, -1, -1, -1, -1, -1, -1};

    if (!is_bare_time && str && *str != 'T') {
        str = sscanf_date(str, &t);
    }
    if (str && *str) {
        if (*str == 'T') {
            str++;
        }
        str = sscanf_time(str, &t);
    }
#if 0
    /* XXX  Note that this won't parse standalone, undesignated time */
    if (str && *str != 'T') {
        str = sscanf_date(str, &t);
    }
    if (str && *str == 'T') {
        str = sscanf_time(++str, &t);
    }
#endif
    return (str && !*str) ? t : vcardtime_null_datetime();
}
