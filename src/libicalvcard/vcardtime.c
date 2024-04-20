#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "icalmemory.h"
#include "vcardtime.h"

vcardtimetype vcardtime_null_date(void)
{
    vcardtimetype t = { 0, 0, 0, -1, -1, -1, -1 };

    return t;
}

vcardtimetype vcardtime_null_time(void)
{
    vcardtimetype t = { -1, -1, -1, 0, 0, 0, -1 };

    return t;
}

vcardtimetype vcardtime_null_datetime(void)
{
    vcardtimetype t = { 0, 0, 0, 0, 0, 0, -1 };

    return t;
}

vcardtimetype vcardtime_current_utc_time(void)
{
    time_t now = time(0);
    struct tm *t = gmtime(&now);
    vcardtimetype tt = { t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                         t->tm_hour, t->tm_min, t->tm_sec, 0 };

    return tt;
}

int vcardtime_is_null_datetime(const vcardtimetype t)
{
    return (t.second + t.minute + t.hour + t.day + t.month + t.year == 0);
}

int vcardtime_is_time(const vcardtimetype t)
{
    return (t.year == -1 && t.month == -1 && t.day == -1);
}

int vcardtime_is_date(const vcardtimetype t)
{
    return (t.hour == -1 && t.minute == -1 && t.second == -1);
}

int vcardtime_is_datetime(const vcardtimetype t)
{
    return (t.day != -1 && t.hour != -1);
}

int vcardtime_is_timestamp(const vcardtimetype t)
{
    return (t.year != -1 && t.month != -1 && t.day != -1 &&
            t.hour != -1 && t.minute != -1 && t.second != -1);
}

int vcardtime_is_utc(const vcardtimetype t)
{
    return (t.utcoffset == 0 && !vcardtime_is_date(t));
}

int vcardtime_is_leap_year(const int year)
{
    if (year == -1) {
        return 1;
    } else if (year <= 1752) {
        return (year % 4 == 0);
    } else {
        return ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
    }
}

static const int days_in_month[] =
    { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

int vcardtime_is_valid_time(const struct vcardtimetype t)
{
    int days;

    if (t.year > 3000 || t.day == 0 ||
        t.hour > 23  || t.minute > 59 || t.second > 60 ||
        t.utcoffset < -720 || t.utcoffset > 840) {
        return 0;
    }

    if (t.minute == -1 && t.hour != -1 && t.second != -1) return 0;

    switch (t.month) {
    case 0:
        return 0;

    case -1:
        if (t.year != 1 && t.day != 1) return 0;

        days = 31;
        break;

    case 2:
        days = 28 + vcardtime_is_leap_year(t.year);
        break;

    default:
        if (t.month > 12) return 0;

        days = days_in_month[t.month];
        break;
    }

    if (t.day > days) return 0;

    return 1;
}

static int sprintf_date(const vcardtimetype t, char *buf, size_t size)
{
    /* 3 zero-padded numeric arguments by position + NUL */
    char fmt[19] = "";

    if (t.year != -1) {
        strcat(fmt, "%04d");

        if (t.month != -1) {
            strcat(fmt, (t.day != -1) ? "%02d%02d" : "-%02d");
        }
    }
    else {
        strcat(fmt, "--");

        strcat(fmt, (t.month != -1) ? "%2$02d" : "-");

        if (t.day != -1) {
            strcat(fmt, "%3$02d");
        }
    }

    return snprintf(buf, size, fmt, t.year, t.month, t.day);
}

static int sprintf_time(const vcardtimetype t, int need_designator,
                      char *buf, size_t size)
{
    /* "T" + sign + 5 zero-padded numeric arguments by position + NUL */
    char fmt[33] = "";

    if (need_designator) {
        strcat(fmt, "T");
    }

    if (t.hour != -1) {
        strcat(fmt, "%02d");

        if (t.minute != -1) {
            strcat(fmt, (t.second != -1) ? "%02d%02d" : "%02d");
        }

        if (t.utcoffset != -1) {
            strcat(fmt, (t.utcoffset == 0) ? "Z" : "%4$+03d%5$02d");
        }
    }
    else {
        strcat(fmt, (t.minute != -1) ? "-%2$02d" : "--");

        if (t.second != -1) {
            strcat(fmt, "%3$02d");
        }
    }

    return snprintf(buf, size, fmt, t.hour, t.minute, t.second,
                    t.utcoffset / 60, abs(t.utcoffset % 60));
}

char *vcardtime_as_vcard_string_r(const vcardtimetype t,
                                  int need_time_designator)
{
    size_t size = 21;
    char *ptr, *buf;
    int n;

    ptr = buf = icalmemory_new_buffer(size);

    if (!vcardtime_is_time(t)) {
        n = sprintf_date(t, ptr, size);
        ptr += n;
        size -= n;

        need_time_designator = 1;
    }
    if (!vcardtime_is_date(t)) {
        n = sprintf_time(t, need_time_designator, ptr, size);
    }

    return buf;
}

const char *vcardtime_as_vcard_string(const vcardtimetype t,
                                      int need_time_designator)
{
    char *buf;

    buf = vcardtime_as_vcard_string_r(t, need_time_designator);
    icalmemory_add_tmp_buffer(buf);
    return buf;
}

#define num_digits(s) strspn(s, "0123456789")

static const char *sscanf_date(const char *str, vcardtimetype *t)
{
    char fmt[21] = "";  /* 4 numeric arguments by position + NUL */
    const char *month;
    size_t ndig;
    int nchar;

    if (!str || !*str) {
        /* empty string */
        return NULL;
    }
    else if (!strncmp(str, "--", 2)) {
        month = str + 2;

        if (*month == '-') {
            /* "--" "-" day */
            ndig = num_digits(month+1);

            if (ndig == 2) {
                strcpy(fmt, "---%3$2u");
            }
        }
        else {
            /* "--"  month [day] */
            ndig = num_digits(month);

            if (ndig == 4) {
                /* month day */
                strcpy(fmt, "--%2$2u%3$2u");
            }
            else if (ndig == 2) {
                /* month */
                strcpy(fmt, "--%2$2u");
            }
        }
    }
    else {
        /* year [ ("-" month) / (month day)] */
        ndig = num_digits(str);

        if (ndig == 8) {
            /* year month day */
            strcpy(fmt, "%1$4u%2$2u%3$2u");
        }
        else if (ndig == 4) {
            month = str + 4;

            if (!*month) {
                /* year */
                strcpy(fmt, "%1$4u");
            }
            else if (*month == '-') {
                /* year "-" month */
                ndig = num_digits(month+1);

                if (ndig == 2) {
                    strcpy(fmt, "%1$4u-%2$2u");
                }
            }
        }
    }

    if (!*fmt) {
        /* invalid time */
        return NULL;
    }

    strcat(fmt, "%4$n");
    sscanf(str, fmt, &t->year, &t->month, &t->day, &nchar);

    return (str += nchar);
}

static const char *sscanf_zone(const char *str, vcardtimetype *t)
{
    char fmt[16] = "";  /* 3 numeric arguments by position + NUL */
    int offset_h = 0, offset_m = 0, nchar;
    size_t ndig;

    if (!str || !*str) {
        /* empty string */
        return NULL;
    }
    else if (*str == 'Z') {
        /* zone = utc-designator */
        strcpy(fmt, "Z");
    }
    else if (strchr("+-", *str)) {
        /* zone = utc-offset = sign hour [minute] */
        ndig = num_digits(str+1);

        if (ndig == 4) {
            /* sign hour minute */
            strcpy(fmt, "%1$3d%2$2u");
        }
        else if (ndig == 2) {
            /* sign hour */
            strcpy(fmt, "%1$3d");
        }
    }

    if (!*fmt) {
        /* invalid zone */
        return NULL;
    }

    strcat(fmt, "%3$n");
    sscanf(str, fmt, &offset_h, &offset_m, &nchar);

    t->utcoffset = 60 * offset_h + ((offset_h < 0) ? -offset_m : offset_m);

    return (str += nchar);
}

static const char *sscanf_time(const char *str, vcardtimetype *t)
{
    char fmt[21] = "";  /* 4 numeric arguments by position + NUL */
    size_t ndig;
    int nchar;

    if (!str || !*str) {
        /* empty string */
        return NULL;
    }
    else if (*str == '-') {
        if (str[1] == '-') {
            /* "-" "-" second */
            ndig = num_digits(str+2);

            if (ndig == 2) {
                strcpy(fmt, "--%3$2u");
            }
        }
        else {
            /* "-"  minute [second] */
            ndig = num_digits(str+1);

            if (ndig == 4) {
                /* minute second */
                strcpy(fmt, "-%2$2u%3$2u");
            }
            else if (ndig == 2) {
                /* minute */
                strcpy(fmt, "-%2$2u");
            }
        }
    }
    else {
        /* hour [minute [second]] [zone] */
        ndig = num_digits(str);

        if (ndig == 6) {
            /* hour minute second */
            strcpy(fmt, "%1$2u%2$2u%3$2u");
        }
        else if (ndig == 4) {
            /* hour minute */
            strcpy(fmt, "%1$2u%2$2u");
        }
        else if (ndig == 2) {
            /* hour */
            strcpy(fmt, "%1$2u");
        }
    }

    if (!*fmt) {
        /* invalid time */
        return NULL;
    }

    strcat(fmt, "%4$n");
    sscanf(str, fmt, &t->hour, &t->minute, &t->second, &nchar);
    str += nchar;

    if (t->hour != -1 && *str) {
        str = sscanf_zone(str, t);
    }

    return str;
}

vcardtimetype vcardtime_from_string(const char *str, int is_bare_time)
{
    vcardtimetype t = { -1, -1, -1, -1, -1, -1, -1 };

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
