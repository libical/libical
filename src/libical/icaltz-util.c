/*
 * Authors :
 *  Chenthill Palanisamy <pchenthill@novell.com>
 *
 * SPDX-FileCopyrightText: 2007, Novell, Inc.
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 */
//krazy:excludeall=cpp

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "icaltz-util.h"
#include "icalerror.h"
#include "icaltimezone.h"
#include "icalmemory.h"

#include <stdlib.h>
#include <limits.h>

#if defined(sun) && defined(__SVR4)
#include <sys/types.h>
#include <sys/byteorder.h>
#else
#if defined(HAVE_BYTESWAP_H)
#include <byteswap.h>
#endif
#if defined(HAVE_ENDIAN_H)
#include <endian.h>
#else
#if defined(HAVE_SYS_ENDIAN_H)
#include <sys/endian.h>
#if defined(bswap32)
#define bswap_32 bswap32
#else
#define bswap_32 swap32
#endif
#endif
#endif
#endif

#if defined(__OpenBSD__) && !defined(bswap_32)
#define bswap_32 swap32
#endif

#if defined(_MSC_VER)
#if !defined(HAVE_BYTESWAP_H) && !defined(HAVE_SYS_ENDIAN_H) && !defined(HAVE_ENDIAN_H)
#define bswap_16(x) (((x) << 8) & 0xff00) | (((x) >> 8) & 0xff)

#define bswap_32(x) \
(((x) << 24) & 0xff000000) | \
(((x) << 8) & 0xff0000)    | \
(((x) >> 8) & 0xff00)      | \
(((x) >> 24) & 0xff)

#define bswap_64(x) \
((((x) & 0xff00000000000000ull) >> 56) | \
(((x) & 0x00ff000000000000ull) >> 40)  | \
(((x) & 0x0000ff0000000000ull) >> 24)  | \
(((x) & 0x000000ff00000000ull) >> 8)   | \
(((x) & 0x00000000ff000000ull) << 8)   | \
(((x) & 0x0000000000ff0000ull) << 24)  | \
(((x) & 0x000000000000ff00ull) << 40)  | \
(((x) & 0x00000000000000ffull) << 56))
#endif
#include <io.h>
#endif

#if defined(__APPLE__) || defined(__MINGW32__)
#define bswap_16(x) (((x) << 8) & 0xff00) | (((x) >> 8) & 0xff)
#define bswap_32 __builtin_bswap32
#define bswap_64 __builtin_bswap64
#endif

//@cond PRIVATE
typedef struct
{
    char magic[4];
    char version;
    char unused[15];
    char ttisgmtcnt[4];
    char ttisstdcnt[4];
    char leapcnt[4];
    char timecnt[4];
    char typecnt[4];
    char charcnt[4];
} tzinfo;

/* fullpath to the system zoneinfo directory (where zone.tab lives) */
static ICAL_GLOBAL_VAR char s_zoneinfopath[MAXPATHLEN] = {0};

/* A few well-known locations for system zoneinfo; can be overridden with TZDIR environment */
static const char *s_zoneinfo_search_paths[] = {
    "/usr/share/zoneinfo",
    "/usr/lib/zoneinfo",
    "/etc/zoneinfo",
    "/usr/share/lib/zoneinfo"
};

#define EFREAD(buf,size,num,fs) \
if (fread(buf, size, num, fs) < num  && ferror(fs)) {  \
    icalerror_set_errno(ICAL_FILE_ERROR);              \
    goto error;                                        \
}

typedef struct
{
    long int gmtoff;
    unsigned char isdst;
    unsigned int abbr;
    unsigned char isstd;
    unsigned char isgmt;
    char *zname;

} ttinfo;

typedef struct
{
    icaltime_t transition;
    long int change;
} leap;
//@endcond

static int decode(const void *ptr)
{
#if defined(sun) && defined(__SVR4)
    if (sizeof(int) == 4) {
#if defined(_BIG_ENDIAN)
        return *(const int *)ptr;
#else
        return BSWAP_32(*(const int *)ptr);
#endif
#else
    if ((BYTE_ORDER == BIG_ENDIAN) && sizeof(int) == 4) {
        return *(const int *)ptr;
    } else if (BYTE_ORDER == LITTLE_ENDIAN && sizeof(int) == 4) {
        return (int)bswap_32(*(const unsigned int *)ptr);
#endif
    } else {
        const unsigned char *p = ptr;
        int result = *p & (1 << (CHAR_BIT - 1)) ? ~0 : 0;

        /* cppcheck-suppress shiftNegativeLHS */
        result = (result << 8) | *p++;
        result = (result << 8) | *p++;
        result = (result << 8) | *p++;
        result = (result << 8) | *p++;

        return result;
    }
}

static long long int decode64(const void *ptr)
{
#if defined(sun) && defined(__SVR4)
#if defined(_BIG_ENDIAN)
    return *(const long long int *)ptr;
#else
    return BSWAP_64(*(const long long int *)ptr);
#endif
#else
    if ((BYTE_ORDER == BIG_ENDIAN)) {
        return *(const long long int *)ptr;
    } else {
        return (const long long int)bswap_64(*(const unsigned long long int *)ptr);
    }
#endif
}

static char *zname_from_stridx(char *str, size_t idx)
{
    size_t i;
    size_t size;
    char *ret;

    i = idx;
    while (str[i] != '\0') {
        i++;
    }

    size = i - idx;
    str += idx;
    ret = (char *)icalmemory_new_buffer(size + 1);
    ret = strncpy(ret, str, size);
    ret[size] = '\0';

    return ret;
}

static void set_zoneinfopath(void)
{
    char file_path[MAXPATHLEN];
    const char *fname = ZONES_TAB_SYSTEM_FILENAME;
    size_t i, num_zi_search_paths;

    /* Search for the zone.tab file in the dir specified by the TZDIR environment */
    const char *env_tzdir = getenv("TZDIR");
    if (env_tzdir != NULL) {
        snprintf(file_path, MAXPATHLEN, "%s/%s", env_tzdir, fname);
        if (!access (file_path, F_OK|R_OK)) {
            strncpy(s_zoneinfopath, env_tzdir, MAXPATHLEN-1);
            return;
        }
    }

    /* Else, search for zone.tab in a list of well-known locations */
    num_zi_search_paths = sizeof(s_zoneinfo_search_paths) / sizeof(s_zoneinfo_search_paths[0]);
    for (i = 0; i < num_zi_search_paths; i++) {
        snprintf(file_path, MAXPATHLEN, "%s/%s", s_zoneinfo_search_paths[i], fname);
        if (!access(file_path, F_OK | R_OK)) {
            strncpy(s_zoneinfopath, s_zoneinfo_search_paths[i], MAXPATHLEN-1);
            break;
        }
    }
}

void icaltzutil_set_zone_directory(const char *zonepath)
{
    if ((zonepath == NULL) || (zonepath[0] == '\0')) {
        memset(s_zoneinfopath, 0, MAXPATHLEN);
    } else {
        strncpy(s_zoneinfopath, zonepath, MAXPATHLEN-1);
    }
}

const char *icaltzutil_get_zone_directory(void)
{
    if (s_zoneinfopath[0] == '\0') {
        set_zoneinfopath();
    }

    return s_zoneinfopath;
}

static int calculate_pos(icaltimetype icaltime)
{
   static const int r_pos[] = {1, 2, 3, -2, -1};
   int pos;

   pos = (icaltime.day - 1) / 7;

   /* Check if pos 3 is the last occurrence of the week day in the month */
   if (pos == 3 && ((icaltime.day + 7) > icaltime_days_in_month(icaltime.month, icaltime.year))) {
       pos = 4;
   }

   return r_pos[pos];
}

static char *parse_posix_zone(char *p, ttinfo *type)
{
    size_t size;

    /* Zone name */
    if (*p == '<') {
        /* Alphanumeric, '-', or '+' */
        size = strcspn(++p, ">");
    } else {
        /* Alpha ONLY */
        size = strcspn(p, "-+0123456789,\n");
    }

    type->zname = (char *) icalmemory_new_buffer(size + 1);
    strncpy(type->zname, p, size);
    type->zname[size] = '\0';
    p += size;

    if (*p == '>') {
        p++;
    }

    if (*p == ',') {
        return p;
    }

    /* Zone offset: hh[:mm[:ss]] */
    type->gmtoff = strtol(p, &p, 10) * -3600;  /* sign of offset is reversed */
    if (*p == ':') {
        type->gmtoff += strtol(++p, &p, 10) * 60;
    }
    if (*p == ':') {
        type->gmtoff += strtol(++p, &p, 10);
    }
    return p;
}

#define nth_weekday(week, day) (icalrecurrencetype_encode_day(day, week))

static char *parse_posix_rule(char *p,
                              struct icalrecurrencetype *recur, icaltimetype *t)
{
    int month = 0, monthday = 0, week = 0, day;

    /* Parse date */
    if (*p == 'J') {
        /* The Julian day n (1 <= n <= 365).
           Leap days shall not be counted. That is, in all years,
           including leap years, February 28 is day 59 and March 1 is day 60.
           It is impossible to refer explicitly to the occasional February 29.
        */
        day = strtol(++p, &p, 10);
    } else if (*p == 'M') {
        /* The d'th day (0 <= d <= 6)
           of week n of month m of the year (1 <= n <= 5, 1 <= m <= 12,
           where week 5 means "the last d day in month m"
           which may occur in either the fourth or the fifth week).
           Week 1 is the first week in which the d'th day occurs.
           Day zero is Sunday.
        */
        month = strtol(++p, &p, 10);
        week = strtol(++p, &p, 10);
        day = strtol(++p, &p, 10);
        if (week == 5) {
            week = -1;
        }
    } else {
        /* The zero-based Julian day (0 <= n <= 365).
           Leap days shall be counted, and it is possible to refer to February 29.

           Flag this by adding 1001 to the day.
        */
        day = strtol(++p, &p, 10) + 1001;
    }

    /* Parse time */
    *t = icaltime_null_time();
    t->hour = 2;  /* default is 02:00 */

    if (*p == '/') {
        t->hour = strtol(++p, &p, 10);
        if (*p == ':') t->minute = strtol(++p, &p, 10);
        if (*p == ':') t->second = strtol(++p, &p, 10);
    }

    /* Do adjustments for extended TZ strings */
    if (t->hour < 0 || t->hour > 23) {
        int days_adjust = t->hour / 24;

        t->hour %= 24;
        day += days_adjust;

        if (t->hour < 0) {
            t->hour += 24;
            day += 6;
        }
        if (month) {
            if (week == -1) {
                int days_in_month = icaltime_days_in_month(month, 1 /* non-leap */);

                monthday = days_in_month + days_adjust - 7;
            } else {
                monthday = 1 + (week - 1) * 7 + days_adjust;
            }
            week = 0;
        }
    }

    /* Create rule */
    icalrecurrencetype_clear(recur);
    recur->freq = ICAL_YEARLY_RECURRENCE;

    if (month) {
        recur->by_day[0] = nth_weekday(week, (day % 7) + 1);
        recur->by_month[0] = month;

        if (monthday) {
            unsigned i;
            for (i = 0; i < 7; i++) {
                recur->by_month_day[i] = monthday++;
            }
        }
    } else if (day > 1000) {
        recur->by_year_day[0] = day - 1000;
    } else {
        /* Convert day-of-non-leap-year into month/day */
        icaltimetype t = icaltime_from_day_of_year(day, 1 /* non-leap */);

        recur->by_month[0] = t.month;
        recur->by_month_day[0] = t.day;
    }

    return p;
}

struct zone_context {
    enum icalcomponent_kind kind;
    const char *name;
    long gmtoff_from;
    long gmtoff_to;

    icaltimetype time;
    icaltimetype prev_time;

    icalcomponent *rdate_comp;
    icalcomponent *rrule_comp;
    icalproperty *rrule_prop;
    short num_monthdays;
    struct icalrecurrencetype recur;
    struct icalrecurrencetype final_recur;
};

static void terminate_rrule(struct zone_context *zone)
{
    if (icaltime_compare(zone->time, zone->prev_time)) {
        // Multiple instances
        // Set UNTIL of the component's recurrence
        zone->recur.until = zone->time;
        icaltime_adjust(&zone->recur.until, 0, 0, 0, -zone->gmtoff_from);
        zone->recur.until.zone = icaltimezone_get_utc_timezone();

        // Remove BYMONTHDAY if BYDAY week != 0
        if (icalrecurrencetype_day_position(zone->recur.by_day[0])) {
            zone->recur.by_month_day[0] = ICAL_RECURRENCE_ARRAY_MAX;
        }

        icalproperty_set_rrule(zone->rrule_prop, zone->recur);

        zone->rdate_comp = zone->rrule_comp = NULL;
    } else {
        // Remove the RRULE from the component
        icalcomponent_remove_property(zone->rrule_comp, zone->rrule_prop);
        icalproperty_free(zone->rrule_prop);
    }
}

icalcomponent *icaltzutil_fetch_timezone(const char *location)
{
    tzinfo header;
    size_t i, num_trans, num_chars, num_leaps, num_isstd, num_isgmt;
    size_t num_types = 0;
    size_t size;
    int trans_size = 4;

    const char *zonedir;
    FILE *f = NULL;
    char *full_path = NULL;
    icaltime_t *transitions = NULL;
    char *r_trans = NULL, *temp;
    int *trans_idx = NULL;
    ttinfo *types = NULL;
    char *znames = NULL;
    leap *leaps = NULL;
    char *tzid = NULL;

    char footer[100], *tzstr = NULL;

    int idx, prev_idx;
    icalcomponent *tz_comp = NULL;
    icalproperty *icalprop;
    icaltimetype icaltime;

    struct zone_context standard =
        { ICAL_XSTANDARD_COMPONENT, NULL, LONG_MIN, LONG_MIN,
          ICALTIMETYPE_INITIALIZER, ICALTIMETYPE_INITIALIZER,
          NULL, NULL, NULL, 0,
          ICALRECURRENCETYPE_INITIALIZER, ICALRECURRENCETYPE_INITIALIZER
        };
    struct zone_context daylight =
        { ICAL_XDAYLIGHT_COMPONENT, NULL, LONG_MIN, LONG_MIN,
          ICALTIMETYPE_INITIALIZER, ICALTIMETYPE_INITIALIZER,
          NULL, NULL, NULL, 0,
          ICALRECURRENCETYPE_INITIALIZER, ICALRECURRENCETYPE_INITIALIZER
        };
    struct zone_context *zone;

    if (icaltimezone_get_builtin_tzdata()) {
        goto error;
    }

    zonedir = icaltzutil_get_zone_directory();
    if (!zonedir) {
        icalerror_set_errno(ICAL_FILE_ERROR);
        goto error;
    }

    size = strlen(zonedir) + strlen(location) + 2;
    full_path = (char *)icalmemory_new_buffer(size);
    if (full_path == NULL) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        goto error;
    }
    snprintf(full_path, size, "%s/%s", zonedir, location);
    if ((f = fopen(full_path, "rb")) == 0) {
        icalerror_set_errno(ICAL_FILE_ERROR);
        goto error;
    }

    /* read version 1 header */
    EFREAD(&header, 44, 1, f);
    if (memcmp(header.magic, "TZif", 4)) {
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        goto error;
    }
    switch (header.version) {
    case 0:
        break;
    case '2':
    case '3':
        if (sizeof(icaltime_t) == 8) {
            trans_size = 8;
        }
        break;
    default:
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        goto error;
    }

    num_isgmt = (size_t)decode(header.ttisgmtcnt);
    num_leaps = (size_t)decode(header.leapcnt);
    num_chars = (size_t)decode(header.charcnt);
    num_trans = (size_t)decode(header.timecnt);
    num_isstd = (size_t)decode(header.ttisstdcnt);
    num_types = (size_t)decode(header.typecnt);

    if (trans_size == 8) {
        size_t skip = num_trans * 5 + num_types * 6 +
            num_chars + num_leaps * 8 + num_isstd + num_isgmt;

        /* skip version 1 data block */
        if (fseek(f, (long)skip, SEEK_CUR) != 0) {
            icalerror_set_errno(ICAL_FILE_ERROR);
            goto error;
        }

        /* read version 2+ header */
        EFREAD(&header, 44, 1, f);
        if (memcmp(header.magic, "TZif", 4)) {
            icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
            goto error;
        }

        num_isgmt = (size_t)decode(header.ttisgmtcnt);
        num_leaps = (size_t)decode(header.leapcnt);
        num_chars = (size_t)decode(header.charcnt);
        num_trans = (size_t)decode(header.timecnt);
        num_isstd = (size_t)decode(header.ttisstdcnt);
        num_types = (size_t)decode(header.typecnt);
    }

    /* read data block */
    transitions = icalmemory_new_buffer((num_trans+1) * sizeof(icaltime_t));  // +1 for TZ string
    if (transitions == NULL) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        goto error;
    }
    r_trans = icalmemory_new_buffer(num_trans * (size_t)trans_size);
    if (r_trans == NULL) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        goto error;
    }
    trans_idx = icalmemory_new_buffer((num_trans+1) * sizeof(int));  // +1 for TZ string
    if (trans_idx == NULL) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        goto error;
    }
    if (num_trans == 0) {
        // Add one transition using time type 0 at 19011213T204552Z
        transitions[0] = (icaltime_t)INT_MIN;
        trans_idx[0] = 0;
        num_trans = 1;
    } else {
        EFREAD(r_trans, (size_t)trans_size, num_trans, f);
        temp = r_trans;
        for (i = 0; i < num_trans; i++) {
            trans_idx[i] = fgetc(f);
            if (trans_size == 8) {
                transitions[i] = (icaltime_t) decode64(r_trans);
            } else {
                transitions[i] = (icaltime_t) decode(r_trans);
            }
            r_trans += trans_size;
        }
        r_trans = temp;
    }

    types = icalmemory_new_buffer((num_types+2) * sizeof(ttinfo));  // +2 for TZ string
    if (types == NULL) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        goto error;
    }
    for (i = 0; i < num_types; i++) {
        unsigned char a[4];
        int c;

        EFREAD(a, 4, 1, f);
        c = fgetc(f);
        types[i].isdst = (unsigned char)c;
        if ((c = fgetc(f)) < 0) {
            break;
        }
        types[i].abbr = (unsigned int)c;
        types[i].gmtoff = decode(a);
    }

    znames = (char *)icalmemory_new_buffer(num_chars);
    if (znames == NULL) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        goto error;
    }
    EFREAD(znames, num_chars, 1, f);

    /* We got all the information which we need */

    leaps = icalmemory_new_buffer(num_leaps * sizeof(leap));
    if (leaps == NULL) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        goto error;
    }
    for (i = 0; i < num_leaps; i++) {
        char c[8];

        EFREAD(c, (size_t)trans_size, 1, f);
        if (trans_size == 8) {
            leaps[i].transition = (icaltime_t)decode64(c);
        } else {
            leaps[i].transition = (icaltime_t)decode(c);
        }

        EFREAD(c, 4, 1, f);
        leaps[i].change = decode(c);
    }

    for (i = 0; i < num_isstd; ++i) {
        int c = getc(f);
        types[i].isstd = c != 0;
    }

    while (i < num_types) {
        types[i++].isstd = 0;
    }

    for (i = 0; i < num_isgmt; ++i) {
        int c = getc(f);

        types[i].isgmt = c != 0;
    }

    while (i < num_types) {
        types[i++].isgmt = 0;
    }

    for (i = 0; i < num_types; i++) {
        /* coverity[tainted_data] */
        types[i].zname = zname_from_stridx(znames, types[i].abbr);
    }

    /* Read the footer */
    if (trans_size == 8 &&
        (footer[0] = (char)fgetc(f)) == '\n' &&
        fgets(footer+1, (int)sizeof(footer)-1, f) &&
        footer[strlen(footer)-1] == '\n') {
        tzstr = footer+1;
    }
    if (tzstr) {
        /* Parse the TZ string:
           stdoffset[dst[offset][,start[/time],end[/time]]]
        */
        ttinfo *std_type = &types[num_types++];
        ttinfo *dst_type = &types[num_types++];
        char *p = tzstr;

        /* Parse standard zone */
        p = parse_posix_zone(p, std_type);
        if (*p == '\n') {
            /* No DST, so ignore the TZ string */
            tzstr = NULL;
        } else {
            /* Parse DST zone */
            dst_type->isdst = 1;
            dst_type->gmtoff = std_type->gmtoff + 3600;  /* default is +1hr */
            p = parse_posix_zone(p, dst_type);

            if (*p != ',') {
                /* No rule, so ignore the TZ string */
                tzstr = NULL;
            } else {
                struct icaltimetype std_trans, dst_trans;

                /* Parse std->dst rule */
                p = parse_posix_rule(++p, /* skip ',' */
                                     &daylight.final_recur, &dst_trans);

                /* Parse dst->std rule */
                p = parse_posix_rule(++p, /* skip ',' */
                                     &standard.final_recur, &std_trans);

                if (*p != '\n') {
                    /* Trailing junk, so ignore the TZ string */
                    tzstr = NULL;
                } else {
                    struct icaltimetype last_trans =
                        icaltime_from_timet_with_zone(transitions[num_trans-1],
                                                      0, NULL);
                    icalrecur_iterator *iter;

                    if (types[trans_idx[num_trans-1]].isdst) {
                        /* Add next dst->std transition */
                        std_trans.year  = last_trans.year;
                        std_trans.month = last_trans.month;
                        std_trans.day   = last_trans.day;
                        iter = icalrecur_iterator_new(standard.final_recur,
                                                      std_trans);
                        std_trans = icalrecur_iterator_next(iter);
                        icaltime_adjust(&std_trans, 0, 0, 0, -dst_type->gmtoff);
                        transitions[num_trans] = icaltime_as_timet(std_trans);
                        trans_idx[num_trans++] = (int) num_types-2;
                        icalrecur_iterator_free(iter);
                    } else {
                        /* Add next std->dst transition */
                        dst_trans.year  = last_trans.year;
                        dst_trans.month = last_trans.month;
                        dst_trans.day   = last_trans.day;
                        iter = icalrecur_iterator_new(daylight.final_recur,
                                                      dst_trans);
                        dst_trans = icalrecur_iterator_next(iter);
                        icaltime_adjust(&dst_trans, 0, 0, 0, -std_type->gmtoff);
                        transitions[num_trans] = icaltime_as_timet(dst_trans);
                        trans_idx[num_trans++] = (int) num_types-1;
                        icalrecur_iterator_free(iter);
                    }
                }
            }
        }
    }

    /* Build the VTIMEZONE now */

    tz_comp = icalcomponent_new(ICAL_VTIMEZONE_COMPONENT);

    /* Add tzid property */
    size = strlen(icaltimezone_tzid_prefix()) + strlen(location) + 1;
    tzid = (char *)icalmemory_new_buffer(size);
    if (tzid == NULL) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        goto error;
    }
    snprintf(tzid, size, "%s%s", icaltimezone_tzid_prefix(), location);
    icalprop = icalproperty_new_tzid(tzid);
    icalcomponent_add_property(tz_comp, icalprop);

    icalprop = icalproperty_new_x(location);
    icalproperty_set_x_name(icalprop, "X-LIC-LOCATION");
    icalcomponent_add_property(tz_comp, icalprop);

    idx = 0;  // time type 0 is always time prior to first transition

    for (i = 0; i < num_trans; i++) {
        int by_day = 0;
        icaltime_t start;
        enum icalrecurrencetype_weekday dow = ICAL_NO_WEEKDAY;

        prev_idx = idx;
        idx = trans_idx[i];
        start = transitions[i] + types[prev_idx].gmtoff;
        icaltime = icaltime_from_timet_with_zone(start, 0, NULL);

        if (types[idx].isdst) {
            zone = &daylight;
        } else {
            zone = &standard;
        }

        // The last two transition times are DTSTART for the TZ string RRULEs
        if (tzstr && (i >= num_trans - 2)) {
            terminate_rrule(zone);
            zone->rrule_comp = NULL;
        } else {
            dow = icaltime_day_of_week(icaltime);
            by_day = nth_weekday(calculate_pos(icaltime), dow);
        }

        if (zone->rrule_comp) {
            int terminate = 0;
            int rdate = 0;

            // Check if the zone name or either of the offsets have changed
            if (types[prev_idx].gmtoff != zone->gmtoff_from ||
                types[idx].gmtoff      != zone->gmtoff_to   ||
                (types[idx].zname != NULL &&
                 strcmp(types[idx].zname, zone->name))) {

                zone->rdate_comp = NULL;
                terminate = 1;
            }
            // Check if most of the recurrence pattern is the same
            else if (icaltime.year   == zone->time.year + 1 &&
                     icaltime.month  == zone->time.month    &&
                     icaltime.hour   == zone->time.hour     &&
                     icaltime.minute == zone->time.minute   &&
                     icaltime.second == zone->time.second) {

                if (by_day == zone->recur.by_day[0]) {
                    // Same nth weekday of the month - continue
                } else if (dow == icalrecurrencetype_day_day_of_week(zone->recur.by_day[0])) {
                    // Same weekday in the month
                    if (icaltime.day >= zone->recur.by_month_day[0] + 7 ||
                        icaltime.day + 7 <= zone->recur.by_month_day[zone->num_monthdays-1]) {
                        // Don't allow two month days with the same weekday -
                        // possible RDATE
                        rdate = terminate = 1;
                    } else {
                        // Insert day of month into the array
                        int j;
                        for (j = 0; j < zone->num_monthdays; j++) {
                            if (icaltime.day <= zone->recur.by_month_day[j]) {
                                break;
                            }
                        }
                        if (icaltime.day < zone->recur.by_month_day[j]) {
                            memmove(&zone->recur.by_month_day[j+1],
                                    &zone->recur.by_month_day[j],
                                    (zone->num_monthdays - j) *
                                    sizeof(zone->recur.by_month_day[0]));
                            zone->recur.by_month_day[j] = icaltime.day;
                            zone->num_monthdays++;
                        }

                        // Remove week number from BYDAY
                        zone->recur.by_day[0] = nth_weekday(0, dow);
                    }
                } else if (icaltime.day == zone->recur.by_month_day[0]) {
                    // Same day of the month - remove BYDAY
                    zone->recur.by_day[0] = ICAL_RECURRENCE_ARRAY_MAX;
                } else {
                    // Different BYDAY and BYMONTHDAY - possible RDATE
                    rdate = terminate = 1;
                }
            } else {
                // Different recurrence pattern entirely - possible RDATE
                rdate = terminate = 1;
            }

            if (terminate) {
                // Terminate the current RRULE
                terminate_rrule(zone);

                if (rdate) {
                    if (zone->rdate_comp) {
                        // Add an RDATE to the previous component
                        // Remove the current RRULE component
                        struct icaldatetimeperiodtype dtp =
                            { zone->time,
                              ICALPERIODTYPE_INITIALIZER
                            };

                        icalprop = icalproperty_new_rdate(dtp);
                        icalcomponent_add_property(zone->rdate_comp, icalprop);

                        icalcomponent_remove_component(tz_comp, zone->rrule_comp);
                        icalcomponent_free(zone->rrule_comp);
                    } else {
                        zone->rdate_comp = zone->rrule_comp;
                    }
                }
                zone->rrule_comp = NULL;
            }
        }

        zone->prev_time = zone->time;
        zone->time = icaltime;
        zone->gmtoff_from = types[prev_idx].gmtoff;
        zone->gmtoff_to = types[idx].gmtoff;

        if (!zone->rrule_comp) {
            zone->name = types[idx].zname;
            zone->prev_time = icaltime;

            // Create a recurrence rule for the current set of changes
            icalrecurrencetype_clear(&zone->recur);
            zone->recur.freq = ICAL_YEARLY_RECURRENCE;
            zone->recur.by_day[0] = by_day;
            zone->recur.by_month[0] = icaltime.month;
            zone->recur.by_month_day[0] = icaltime.day;
            zone->num_monthdays = 1;
            zone->rrule_prop = icalproperty_new_rrule(zone->recur);

            zone->rrule_comp =
                icalcomponent_vanew(zone->kind,
                                    icalproperty_new_tzname(zone->name),
                                    icalproperty_new_tzoffsetfrom(zone->gmtoff_from),
                                    icalproperty_new_tzoffsetto(zone->gmtoff_to),
                                    icalproperty_new_dtstart(zone->time),
                                    zone->rrule_prop,
                                    (void *)0);
            icalcomponent_add_component(tz_comp, zone->rrule_comp);
        }
    }

    if (tzstr) {
        // Replace the last recurrence rules with those from the TZ string
        icalproperty_set_rrule(standard.rrule_prop, standard.final_recur);
        icalproperty_set_rrule(daylight.rrule_prop, daylight.final_recur);
    } else {
        // Terminate the last recurrence rules
        if (standard.rrule_comp) {
            terminate_rrule(&standard);
        }
        if (daylight.rrule_comp) {
            terminate_rrule(&daylight);
        }
    }

  error:
    if (f)
        fclose(f);

    if (full_path)
        icalmemory_free_buffer(full_path);

    if (transitions)
        icalmemory_free_buffer(transitions);

    if (r_trans)
        icalmemory_free_buffer(r_trans);

    if (trans_idx)
        icalmemory_free_buffer(trans_idx);

    if (types) {
        for (i = 0; i < num_types; i++) {
            if (types[i].zname) {
                icalmemory_free_buffer(types[i].zname);
            }
        }
        icalmemory_free_buffer(types);
    }

    if (znames)
        icalmemory_free_buffer(znames);

    if (leaps)
        icalmemory_free_buffer(leaps);

    if (tzid)
        icalmemory_free_buffer(tzid);

    return tz_comp;
}
