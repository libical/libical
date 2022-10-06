/*======================================================================
 FILE: icaltimezone.c
 CREATOR: Damon Chaplin 15 March 2001

 SPDX-FileCopyrightText: 2001, Damon Chaplin <damon@ximian.com>

 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

======================================================================*/
//krazy:excludeall=cpp

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "icaltimezone.h"
#include "icaltimezoneimpl.h"
#include "icalarray.h"
#include "icalerror.h"
#include "icalparser.h"
#include "icalmemory.h"
#include "icaltz-util.h"

#include <ctype.h>
#include <stddef.h>     /* for ptrdiff_t */
#include <stdlib.h>
#include <limits.h>

#if defined(HAVE_PTHREAD)
#include <pthread.h>
#if defined(PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP)
// It seems the same thread can attempt to lock builtin_mutex multiple times
// (at least when using builtin tzdata), so make it builtin_mutex recursive:
static pthread_mutex_t builtin_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
#else
static pthread_mutex_t builtin_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif
// To avoid use-after-free in multithreaded applications when accessing icaltimezone::changes
static pthread_mutex_t changes_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

#if defined(_WIN32)
#if !defined(_WIN32_WCE)
#include <mbstring.h>
#endif
#include <windows.h>
#endif

/** This is the toplevel directory where the timezone data is installed in. */
#define ZONEINFO_DIRECTORY      PACKAGE_DATA_DIR "/zoneinfo"

/** The prefix we use to uniquely identify TZIDs.
    It must begin and end with forward slashes.
 */
#define BUILTIN_TZID_PREFIX_LEN 256
#define BUILTIN_TZID_PREFIX     "/freeassociation.sourceforge.net/"

/* Known prefixes from the old versions of libical */
static const struct _compat_tzids {
    const char *tzid;
    int slashes;
} glob_compat_tzids[] = {
    { "/freeassociation.sourceforge.net/Tzfile/", 3 },
    { "/freeassociation.sourceforge.net/", 2 },
    { "/citadel.org/", 3 }, /* Full TZID for this can be: "/citadel.org/20190914_1/" */
    { NULL, -1 }
};

/* The prefix to be used for tzid's generated from system tzdata */
static ICAL_GLOBAL_VAR char s_ical_tzid_prefix[BUILTIN_TZID_PREFIX_LEN] = {0};

/** This is the filename of the file containing the city names and
    coordinates of all the builtin timezones. */
#define ZONES_TAB_FILENAME      "zones.tab"

/** This is the number of years of extra coverage we do when expanding
    the timezone changes. */
#define ICALTIMEZONE_EXTRA_COVERAGE     5

#if (SIZEOF_ICALTIME_T > 4)
/** Arbitrarily go up to 1000th anniversary of Gregorian calendar, since
    64-bit icaltime_t values get us up to the tm_year limit of 2+ billion years. */
#define ICALTIMEZONE_MAX_YEAR           2582
#else
/** This is the maximum year we will expand to, since 32-bit icaltime_t values
    only go up to the start of 2038. */
#define ICALTIMEZONE_MAX_YEAR           2037
#endif

typedef struct _icaltimezonechange icaltimezonechange;

struct _icaltimezonechange
{
    int utc_offset;
    /**< The offset to add to UTC to get local time, in seconds. */

    int prev_utc_offset;
    /**< The offset to add to UTC, before this change, in seconds. */

    int year;                   /**< Actual year, e.g. 2001. */
    int month;                  /**< 1 (Jan) to 12 (Dec). */
    int day;
    int hour;
    int minute;
    int second;
    /**< The time that the change came into effect, in UTC.
       Note that the prev_utc_offset applies to this local time,
       since we haven't changed to the new offset yet. */

    int is_daylight;
    /**< Whether this is STANDARD or DAYLIGHT time. */
};

/** An array of icaltimezones for the builtin timezones. */
static ICAL_GLOBAL_VAR icalarray *builtin_timezones = NULL;

/** This is the special UTC timezone, which isn't in builtin_timezones. */
static ICAL_GLOBAL_VAR icaltimezone utc_timezone = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static ICAL_GLOBAL_VAR char *zone_files_directory = NULL;

#if defined(USE_BUILTIN_TZDATA)
static ICAL_GLOBAL_VAR int use_builtin_tzdata = 1;
#else
static ICAL_GLOBAL_VAR int use_builtin_tzdata = 0;
#endif

static void icaltimezone_reset(icaltimezone *zone);
static void icaltimezone_expand_changes(icaltimezone *zone, int end_year);
static int icaltimezone_compare_change_fn(const void *elem1, const void *elem2);

static size_t icaltimezone_find_nearby_change(icaltimezone *zone, icaltimezonechange *change);

static void icaltimezone_adjust_change(icaltimezonechange *tt,
                                       int days, int hours, int minutes, int seconds);

static void icaltimezone_init(icaltimezone *zone);

/** @brief Gets the TZID, LOCATION/X-LIC-LOCATION, and TZNAME properties from
 * the VTIMEZONE component and places them in the icaltimezone.
 *
 * @returns 1 on success, or 0 if the TZID can't be found.
 */
static int icaltimezone_get_vtimezone_properties(icaltimezone *zone, icalcomponent *component)
#if defined(THREAD_SANITIZER)
__attribute__((no_sanitize("thread")))
#endif
;

static void icaltimezone_load_builtin_timezone(icaltimezone *zone)
#if defined(THREAD_SANITIZER)
__attribute__((no_sanitize("thread")))
#endif
;

static void icaltimezone_ensure_coverage(icaltimezone *zone, int end_year);

static void icaltimezone_init_builtin_timezones(void);

static void icaltimezone_parse_zone_tab(void);

static char *icaltimezone_load_get_line_fn(char *s, size_t size, void *data);

static void format_utc_offset(int utc_offset, char *buffer, size_t buffer_size);
static const char *get_zone_directory(void);

static void icaltimezone_builtin_lock(void)
{
#if defined(HAVE_PTHREAD)
    pthread_mutex_lock(&builtin_mutex);
#endif
}

static void icaltimezone_builtin_unlock(void)
{
#if defined(HAVE_PTHREAD)
    pthread_mutex_unlock(&builtin_mutex);
#endif
}

static void icaltimezone_changes_lock(void)
{
#if defined(HAVE_PTHREAD)
    pthread_mutex_lock(&changes_mutex);
#endif
}

static void icaltimezone_changes_unlock(void)
{
#if defined(HAVE_PTHREAD)
    pthread_mutex_unlock(&changes_mutex);
#endif
}

const char *icaltimezone_tzid_prefix(void)
{
    if (s_ical_tzid_prefix[0] == '\0') {
        strncpy(s_ical_tzid_prefix, BUILTIN_TZID_PREFIX, BUILTIN_TZID_PREFIX_LEN-1);
    }
    return s_ical_tzid_prefix;
}

icaltimezone *icaltimezone_new(void)
{
    icaltimezone *zone;

    zone = (icaltimezone *) icalmemory_new_buffer(sizeof(icaltimezone));
    if (!zone) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return NULL;
    }

    icaltimezone_init(zone);

    return zone;
}

icaltimezone *icaltimezone_copy(icaltimezone *originalzone)
{
    icaltimezone *zone;

    zone = (icaltimezone *) icalmemory_new_buffer(sizeof(icaltimezone));
    if (!zone) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return NULL;
    }

    memcpy(zone, originalzone, sizeof(icaltimezone));
    if (zone->tzid != NULL) {
        zone->tzid = icalmemory_strdup(zone->tzid);
    }
    if (zone->location != NULL) {
        zone->location = icalmemory_strdup(zone->location);
    }
    if (zone->tznames != NULL) {
        zone->tznames = icalmemory_strdup(zone->tznames);
    }

    icaltimezone_changes_lock();
    if (zone->changes != NULL) {
        zone->changes = icalarray_copy(zone->changes);
    }
    icaltimezone_changes_unlock();

    /* Let the caller set the component because then they will
       know to be careful not to free this reference twice. */
    zone->component = NULL;

    return zone;
}

void icaltimezone_free(icaltimezone *zone, int free_struct)
{
    icaltimezone_reset(zone);
    if (free_struct)
        icalmemory_free_buffer(zone);
}

/** @brief Resets the icaltimezone to the initial state, freeing most of the
 * fields.
 */
static void icaltimezone_reset(icaltimezone *zone)
{
    if (zone->tzid)
        icalmemory_free_buffer(zone->tzid);

    if (zone->location)
        icalmemory_free_buffer(zone->location);

    if (zone->tznames)
        icalmemory_free_buffer(zone->tznames);

    if (zone->component)
        icalcomponent_free(zone->component);

    //    icaltimezone_changes_lock();
    if (zone->changes) {
        icalarray_free(zone->changes);
        zone->changes = NULL;
    }
    //    icaltimezone_changes_unlock();

    icaltimezone_init(zone);
}

/** @brief Initializes an icaltimezone. */
static void icaltimezone_init(icaltimezone *zone)
{
    zone->tzid = NULL;
    zone->location = NULL;
    zone->tznames = NULL;
    zone->latitude = 0.0;
    zone->longitude = 0.0;
    zone->component = NULL;
    zone->builtin_timezone = NULL;
    zone->end_year = 0;
    zone->changes = NULL;
}

/** @brief Gets the TZID, LOCATION/X-LIC-LOCATION and TZNAME properties of
 * the VTIMEZONE component and stores them in the icaltimezone.
 *
 * @returns 1 on success, or 0 if the TZID can't be found.
 *
 * Note that it expects the zone to be initialized or reset - it doesn't free
 * any old values.
 */
static int icaltimezone_get_vtimezone_properties(icaltimezone *zone, icalcomponent *component)
{
    icalproperty *prop;
    const char *tzid;

    prop = icalcomponent_get_first_property(component, ICAL_TZID_PROPERTY);
    if (!prop)
        return 0;

    /* A VTIMEZONE MUST have a TZID, or a lot of our code won't work. */
    tzid = icalproperty_get_tzid(prop);
    if (!tzid) {
        return 0;
    }

    if (zone->tzid) {
        icalmemory_free_buffer(zone->tzid);
    }
    zone->tzid = icalmemory_strdup(tzid);

    if (zone->component) {
        icalcomponent_free(zone->component);
    }
    zone->component = component;

    if (zone->location) {
        icalmemory_free_buffer(zone->location);
    }
    zone->location = icaltimezone_get_location_from_vtimezone(component);

    if (zone->tznames) {
        icalmemory_free_buffer(zone->tznames);
    }
    zone->tznames = icaltimezone_get_tznames_from_vtimezone(component);

    return 1;
}

char *icaltimezone_get_location_from_vtimezone(icalcomponent *component)
{
    icalproperty *prop;
    const char *location;
    const char *name;

    prop = icalcomponent_get_first_property(component, ICAL_LOCATION_PROPERTY);
    if (prop) {
        location = icalproperty_get_location(prop);
        if (location)
            return icalmemory_strdup(location);
    }

    prop = icalcomponent_get_first_property(component, ICAL_X_PROPERTY);
    while (prop) {
        name = icalproperty_get_x_name(prop);
        if (name && !strcasecmp(name, "X-LIC-LOCATION")) {
            location = icalproperty_get_x(prop);
            if (location)
                return icalmemory_strdup(location);
        }
        prop = icalcomponent_get_next_property(component, ICAL_X_PROPERTY);
    }

    return NULL;
}

char *icaltimezone_get_tznames_from_vtimezone(icalcomponent *component)
{
    icalcomponent *comp;
    icalcomponent_kind type;
    icalproperty *prop;
    struct icaltimetype dtstart;
    struct icaldatetimeperiodtype rdate;
    const char *current_tzname;
    const char *standard_tzname = NULL, *daylight_tzname = NULL;
    struct icaltimetype standard_max_date, daylight_max_date;
    struct icaltimetype current_max_date;

    standard_max_date = icaltime_null_time();
    daylight_max_date = icaltime_null_time();

    /* Step through the STANDARD & DAYLIGHT subcomponents. */
    comp = icalcomponent_get_first_component(component, ICAL_ANY_COMPONENT);
    while (comp) {
        type = icalcomponent_isa(comp);
        if (type == ICAL_XSTANDARD_COMPONENT || type == ICAL_XDAYLIGHT_COMPONENT) {
            current_max_date = icaltime_null_time();
            current_tzname = NULL;

            /* Step through the properties. We want to find the TZNAME, and
               the largest DTSTART or RDATE. */
            prop = icalcomponent_get_first_property(comp, ICAL_ANY_PROPERTY);
            while (prop) {
                switch (icalproperty_isa(prop)) {
                case ICAL_TZNAME_PROPERTY:
                    current_tzname = icalproperty_get_tzname(prop);
                    break;

                case ICAL_DTSTART_PROPERTY:
                    dtstart = icalproperty_get_dtstart(prop);
                    if (icaltime_compare(dtstart, current_max_date) > 0)
                        current_max_date = dtstart;

                    break;

                case ICAL_RDATE_PROPERTY:
                    rdate = icalproperty_get_rdate(prop);
                    if (icaltime_compare(rdate.time, current_max_date) > 0)
                        current_max_date = rdate.time;

                    break;

                default:
                    break;
                }

                prop = icalcomponent_get_next_property(comp, ICAL_ANY_PROPERTY);
            }

            if (current_tzname) {
                if (type == ICAL_XSTANDARD_COMPONENT) {
                    if (!standard_tzname ||
                        icaltime_compare(current_max_date, standard_max_date) > 0) {
                        standard_max_date = current_max_date;
                        standard_tzname = current_tzname;
                    }
                } else {
                    if (!daylight_tzname ||
                        icaltime_compare(current_max_date, daylight_max_date) > 0) {
                        daylight_max_date = current_max_date;
                        daylight_tzname = current_tzname;
                    }
                }
            }
        }

        comp = icalcomponent_get_next_component(component, ICAL_ANY_COMPONENT);
    }

    /* Outlook (2000) places "Standard Time" and "Daylight Time" in the TZNAME
       strings, which is totally useless. So we return NULL in that case. */
    if (standard_tzname && !strcmp(standard_tzname, "Standard Time"))
        return NULL;

    /* If both standard and daylight TZNAMEs were found, if they are the same
       we return just one, else we format them like "EST/EDT". */
    if (standard_tzname && daylight_tzname) {
        size_t standard_len, daylight_len;
        char *tznames;

        if (!strcmp(standard_tzname, daylight_tzname))
            return icalmemory_strdup(standard_tzname);

        standard_len = strlen(standard_tzname);
        daylight_len = strlen(daylight_tzname);
        tznames = icalmemory_new_buffer(standard_len + daylight_len + 2);
        strcpy(tznames, standard_tzname);
        tznames[standard_len] = '/';
        strcpy(tznames + standard_len + 1, daylight_tzname);
        return tznames;
    } else {
        const char *tznames;

        /* If either of the TZNAMEs was found just return that, else NULL. */
        tznames = standard_tzname ? standard_tzname : daylight_tzname;
        return tznames ? icalmemory_strdup(tznames) : NULL;
    }
}

static void icaltimezone_ensure_coverage(icaltimezone *zone, int end_year)
{
    /* When we expand timezone changes we always expand at least up to this
       year, plus ICALTIMEZONE_EXTRA_COVERAGE. */
    static ICAL_GLOBAL_VAR int icaltimezone_minimum_expansion_year = -1;

    int changes_end_year;

    icaltimezone_load_builtin_timezone(zone);

    if (icaltimezone_minimum_expansion_year == -1) {
        struct icaltimetype today = icaltime_today();

        icaltimezone_minimum_expansion_year = today.year;
    }

    changes_end_year = end_year;
    if (changes_end_year < icaltimezone_minimum_expansion_year)
        changes_end_year = icaltimezone_minimum_expansion_year;

    changes_end_year += ICALTIMEZONE_EXTRA_COVERAGE;

    if (changes_end_year > ICALTIMEZONE_MAX_YEAR)
        changes_end_year = ICALTIMEZONE_MAX_YEAR;

    if (!zone->changes || zone->end_year < end_year)
        icaltimezone_expand_changes(zone, changes_end_year);
}

/* Hold the icaltimezone_changes_lock(); before calling this function */
static void icaltimezone_expand_changes(icaltimezone *zone, int end_year)
{
    icalarray *changes;
    icalcomponent *comp;

#if 0
    printf("\nExpanding changes for: %s to year: %i\n", zone->tzid, end_year);
#endif

    changes = icalarray_new(sizeof(icaltimezonechange), 32);
    if (!changes)
        return;

    /* Scan the STANDARD and DAYLIGHT subcomponents. */
    comp = icalcomponent_get_first_component(zone->component, ICAL_ANY_COMPONENT);
    while (comp) {
        icaltimezone_expand_vtimezone(comp, end_year, changes);
        comp = icalcomponent_get_next_component(zone->component, ICAL_ANY_COMPONENT);
    }

    /* Sort the changes. We may have duplicates but I don't think it will
       matter. */
    icalarray_sort(changes, icaltimezone_compare_change_fn);

    if (zone->changes) {
        icalarray_free(zone->changes);
        zone->changes = 0;
    }

    zone->changes = changes;
    zone->end_year = end_year;
}

void icaltimezone_expand_vtimezone(icalcomponent *comp, int end_year, icalarray *changes)
{
    icaltimezonechange change;
    icalproperty *prop;
    struct icaltimetype dtstart, occ;
    struct icalrecurrencetype rrule;
    icalrecur_iterator *rrule_iterator;
    struct icaldatetimeperiodtype rdate;
    int found_dtstart = 0, found_tzoffsetto = 0, found_tzoffsetfrom = 0;
    int has_rdate = 0, has_rrule = 0;

    /* First we check if it is a STANDARD or DAYLIGHT component, and
       just return if it isn't. */
    if (icalcomponent_isa(comp) == ICAL_XSTANDARD_COMPONENT) {
        change.is_daylight = 0;
    } else if (icalcomponent_isa(comp) == ICAL_XDAYLIGHT_COMPONENT) {
        change.is_daylight = 1;
    } else {
        return;
    }

    /* Step through each of the properties to find the DTSTART,
       TZOFFSETFROM and TZOFFSETTO. We can't expand recurrences here
       since we need these properties before we can do that. */
    prop = icalcomponent_get_first_property(comp, ICAL_ANY_PROPERTY);
    while (prop) {
        switch (icalproperty_isa(prop)) {
        case ICAL_DTSTART_PROPERTY:
            dtstart = icalproperty_get_dtstart(prop);
            found_dtstart = 1;
            break;
        case ICAL_TZOFFSETTO_PROPERTY:
            change.utc_offset = icalproperty_get_tzoffsetto(prop);
            /*printf ("Found TZOFFSETTO: %i\n", change.utc_offset); */
            found_tzoffsetto = 1;
            break;
        case ICAL_TZOFFSETFROM_PROPERTY:
            change.prev_utc_offset = icalproperty_get_tzoffsetfrom(prop);
            /*printf ("Found TZOFFSETFROM: %i\n", change.prev_utc_offset); */
            found_tzoffsetfrom = 1;
            break;
        case ICAL_RDATE_PROPERTY:
            has_rdate = 1;
            break;
        case ICAL_RRULE_PROPERTY:
            has_rrule = 1;
            break;
        default:
            /* Just ignore any other properties. */
            break;
        }

        prop = icalcomponent_get_next_property(comp, ICAL_ANY_PROPERTY);
    }

    /* Microsoft Outlook for Mac (and possibly other versions) will create
       timezones without a tzoffsetfrom property if it's a timezone that
       doesn't change for DST. */
    if (found_tzoffsetto && !found_tzoffsetfrom) {
        change.prev_utc_offset = change.utc_offset;
        found_tzoffsetfrom = 1;
    }

    /* If we didn't find a DTSTART, TZOFFSETTO and TZOFFSETFROM we have to
       ignore the component. FIXME: Add an error property? */
    if (!found_dtstart || !found_tzoffsetto || !found_tzoffsetfrom)
        return;

#if 0
    printf("\n Expanding component DTSTART (Y/M/D): %i/%i/%i %i:%02i:%02i\n",
           dtstart.year, dtstart.month, dtstart.day, dtstart.hour, dtstart.minute, dtstart.second);
#endif

    /* If the STANDARD/DAYLIGHT component has no recurrence rule, we add
       a single change for the DTSTART. */
    if (!has_rrule) {
        change.year = dtstart.year;
        change.month = dtstart.month;
        change.day = dtstart.day;
        change.hour = dtstart.hour;
        change.minute = dtstart.minute;
        change.second = dtstart.second;

        /* Convert to UTC. */
        icaltimezone_adjust_change(&change, 0, 0, 0, -change.prev_utc_offset);

#if 0
        printf("  Appending single DTSTART (Y/M/D): %i/%02i/%02i %i:%02i:%02i\n",
               change.year, change.month, change.day, change.hour, change.minute, change.second);
#endif

        /* Add the change to the array. */
        icalarray_append(changes, &change);
    }

    /* The component has recurrence data, so we expand that now. */
    prop = icalcomponent_get_first_property(comp, ICAL_ANY_PROPERTY);
    while (prop && (has_rdate || has_rrule)) {
#if 0
        printf("Expanding property...\n");
#endif
        switch (icalproperty_isa(prop)) {
        case ICAL_RDATE_PROPERTY:
            rdate = icalproperty_get_rdate(prop);
            change.year = rdate.time.year;
            change.month = rdate.time.month;
            change.day = rdate.time.day;
            /* RDATEs with a DATE value inherit the time from
               the DTSTART. */
            if (icaltime_is_date(rdate.time)) {
                change.hour = dtstart.hour;
                change.minute = dtstart.minute;
                change.second = dtstart.second;
            } else {
                change.hour = rdate.time.hour;
                change.minute = rdate.time.minute;
                change.second = rdate.time.second;

                /* The spec was a bit vague about whether RDATEs were in local
                   time or UTC so we support both to be safe. So if it is in
                   UTC we have to add the UTC offset to get a local time. */
                if (!icaltime_is_utc(rdate.time))
                    icaltimezone_adjust_change(&change, 0, 0, 0, -change.prev_utc_offset);
            }

#if 0
            printf("  Appending RDATE element (Y/M/D): %i/%02i/%02i %i:%02i:%02i\n",
                   change.year, change.month, change.day,
                   change.hour, change.minute, change.second);
#endif

            icalarray_append(changes, &change);
            break;
        case ICAL_RRULE_PROPERTY:
            rrule = icalproperty_get_rrule(prop);

            /* If the rrule UNTIL value is set and is in UTC, we convert it to
               a local time, since the recurrence code has no way to convert
               it itself. */
            if (!icaltime_is_null_time(rrule.until) && icaltime_is_utc(rrule.until)) {
#if 0
                printf("  Found RRULE UNTIL in UTC.\n");
#endif

                /* To convert from UTC to a local time, we use the TZOFFSETFROM
                   since that is the offset from UTC that will be in effect
                   when each of the RRULE occurrences happens. */
                icaltime_adjust(&rrule.until, 0, 0, 0, change.prev_utc_offset);
                rrule.until.zone = NULL;
            }

            /* Add the dtstart to changes, otherwise some oddly-defined VTIMEZONE
               components can cause the first year to get skipped. */
            change.year = dtstart.year;
            change.month = dtstart.month;
            change.day = dtstart.day;
            change.hour = dtstart.hour;
            change.minute = dtstart.minute;
            change.second = dtstart.second;

#if 0
            printf("  Appending RRULE element (Y/M/D): %i/%02i/%02i %i:%02i:%02i\n",
                   change.year, change.month, change.day,
                   change.hour, change.minute, change.second);
#endif

            icaltimezone_adjust_change(&change, 0, 0, 0, -change.prev_utc_offset);

            icalarray_append(changes, &change);

            rrule_iterator = icalrecur_iterator_new(rrule, dtstart);
            for (; rrule_iterator;) {
                occ = icalrecur_iterator_next(rrule_iterator);
                /* Skip dtstart since we just added it */
                if (icaltime_compare(dtstart, occ) == 0) {
                    continue;
                }
                if (occ.year > end_year || icaltime_is_null_time(occ)) {
                    break;
                }
                change.year = occ.year;
                change.month = occ.month;
                change.day = occ.day;
                change.hour = occ.hour;
                change.minute = occ.minute;
                change.second = occ.second;

#if 0
                printf("  Appending RRULE element (Y/M/D): %i/%02i/%02i %i:%02i:%02i\n",
                       change.year, change.month, change.day,
                       change.hour, change.minute, change.second);
#endif

                icaltimezone_adjust_change(&change, 0, 0, 0, -change.prev_utc_offset);

                icalarray_append(changes, &change);
            }

            icalrecur_iterator_free(rrule_iterator);
            break;
        default:
            break;
        }

        prop = icalcomponent_get_next_property(comp, ICAL_ANY_PROPERTY);
    }
}

/** @brief A function to compare 2 icaltimezonechange elements, used for
 * qsort().
 */
static int icaltimezone_compare_change_fn(const void *elem1, const void *elem2)
{
    const icaltimezonechange *change1, *change2;
    int retval;

    change1 = (const icaltimezonechange *)elem1;
    change2 = (const icaltimezonechange *)elem2;

    if (change1->year < change2->year) {
        retval = -1;
    } else if (change1->year > change2->year) {
        retval = 1;
    } else if (change1->month < change2->month) {
        retval = -1;
    } else if (change1->month > change2->month) {
        retval = 1;
    } else if (change1->day < change2->day) {
        retval = -1;
    } else if (change1->day > change2->day) {
        retval = 1;
    } else if (change1->hour < change2->hour) {
        retval = -1;
    } else if (change1->hour > change2->hour) {
        retval = 1;
    } else if (change1->minute < change2->minute) {
        retval = -1;
    } else if (change1->minute > change2->minute) {
        retval = 1;
    } else if (change1->second < change2->second) {
        retval = -1;
    } else if (change1->second > change2->second) {
        retval = 1;
    } else {
        retval = 0;
    }

    return retval;
}

void icaltimezone_convert_time(struct icaltimetype *tt,
                               icaltimezone *from_zone, icaltimezone *to_zone)
{
    int utc_offset, is_daylight;

    /* If the time is a DATE value or both timezones are the same, or we are
       converting a floating time, we don't need to do anything. */
    if (icaltime_is_date(*tt) || from_zone == to_zone || from_zone == NULL)
        return;

    /* Convert the time to UTC by getting the UTC offset and subtracting it. */
    utc_offset = icaltimezone_get_utc_offset(from_zone, tt, NULL);
    icaltime_adjust(tt, 0, 0, 0, -utc_offset);

    /* Now we convert the time to the new timezone by getting the UTC offset
       of our UTC time and adding it. */
    utc_offset = icaltimezone_get_utc_offset_of_utc_time(to_zone, tt, &is_daylight);
    tt->is_daylight = is_daylight;
    icaltime_adjust(tt, 0, 0, 0, utc_offset);
}

int icaltimezone_get_utc_offset(icaltimezone *zone, struct icaltimetype *tt, int *is_daylight)
{
    icaltimezonechange *zone_change, *prev_zone_change, tt_change, tmp_change;
    size_t change_num, change_num_to_use;
    int found_change;
    int step, utc_offset_change, cmp;
    int want_daylight;

    if (tt == NULL)
        return 0;

    if (is_daylight)
        *is_daylight = 0;

    /* For local times and UTC return 0. */
    if (zone == NULL || zone == &utc_timezone)
        return 0;

    /* Use the builtin icaltimezone if possible. */
    if (zone->builtin_timezone)
        zone = zone->builtin_timezone;

    icaltimezone_changes_lock();

    /* Make sure the changes array is expanded up to the given time. */
    icaltimezone_ensure_coverage(zone, tt->year);

    if (!zone->changes || zone->changes->num_elements == 0) {
        icaltimezone_changes_unlock();
        return 0;
    }

    /* Copy the time parts of the icaltimetype to an icaltimezonechange so we
       can use our comparison function on it. */
    tt_change.year = tt->year;
    tt_change.month = tt->month;
    tt_change.day = tt->day;
    tt_change.hour = tt->hour;
    tt_change.minute = tt->minute;
    tt_change.second = tt->second;

    /* This should find a change close to the time, either the change before
       it or the change after it. */
    change_num = icaltimezone_find_nearby_change(zone, &tt_change);

    /* Now move backwards or forwards to find the timezone change that applies
       to tt. It should only have to do 1 or 2 steps. */
    zone_change = icalarray_element_at(zone->changes, change_num);
    step = 1;
    found_change = 0;
    change_num_to_use = -1;
    for (;;) {
        /* Copy the change, so we can adjust it. */
        tmp_change = *zone_change;

        /* If the clock is going backward, check if it is in the region of time
           that is used twice. If it is, use the change with the daylight
           setting which matches tt, or use standard if we don't know. */
        if (tmp_change.utc_offset < tmp_change.prev_utc_offset) {
            /* If the time change is at 2:00AM local time and the clock is
               going back to 1:00AM we adjust the change to 1:00AM. We may
               have the wrong change but we'll figure that out later. */
            icaltimezone_adjust_change(&tmp_change, 0, 0, 0, tmp_change.utc_offset);
        } else {
            icaltimezone_adjust_change(&tmp_change, 0, 0, 0, tmp_change.prev_utc_offset);
        }

        cmp = icaltimezone_compare_change_fn(&tt_change, &tmp_change);

        /* If the given time is on or after this change, then this change may
           apply, but we continue as a later change may be the right one.
           If the given time is before this change, then if we have already
           found a change which applies we can use that, else we need to step
           backwards. */
        if (cmp >= 0) {
            found_change = 1;
            change_num_to_use = change_num;
        } else {
            step = -1;
        }

        /* If we are stepping backwards through the changes and we have found
           a change that applies, then we know this is the change to use so
           we exit the loop. */
        if (step == -1 && found_change == 1)
            break;

        /* If we go past the start of the changes array, then we have no data
           for this time so we return the prev UTC offset. */
        if (change_num == 0 && step < 0) {
            if (is_daylight) {
                *is_daylight = ! tmp_change.is_daylight;
            }

            icaltimezone_changes_unlock();

            return tmp_change.prev_utc_offset;
        }

        change_num += step;

        if (change_num >= zone->changes->num_elements)
            break;

        zone_change = icalarray_element_at(zone->changes, change_num);
    }

    /* If we didn't find a change to use, then we have a bug! */
    icalerror_assert(found_change == 1, "No applicable timezone change found");

    /* Now we just need to check if the time is in the overlapped region of
       time when clocks go back. */
    zone_change = icalarray_element_at(zone->changes, change_num_to_use);

    utc_offset_change = zone_change->utc_offset - zone_change->prev_utc_offset;
    if (utc_offset_change < 0 && change_num_to_use > 0) {
        tmp_change = *zone_change;
        icaltimezone_adjust_change(&tmp_change, 0, 0, 0, tmp_change.prev_utc_offset);

        if (icaltimezone_compare_change_fn(&tt_change, &tmp_change) < 0) {
            /* The time is in the overlapped region, so we may need to use
               either the current zone_change or the previous one. If the
               time has the is_daylight field set we use the matching change,
               else we use the change with standard time. */
            prev_zone_change = icalarray_element_at(zone->changes, change_num_to_use - 1);

            /* I was going to add an is_daylight flag to struct icaltimetype,
               but iCalendar doesn't let us distinguish between standard and
               daylight time anyway, so there's no point. So we just use the
               standard time instead. */
            want_daylight = (tt->is_daylight == 1) ? 1 : 0;

#if 0
            if (zone_change->is_daylight == prev_zone_change->is_daylight) {
                printf(" **** Same is_daylight setting\n");
            }
#endif

            if (zone_change->is_daylight != want_daylight &&
                prev_zone_change->is_daylight == want_daylight) {
                zone_change = prev_zone_change;
            }
        }
    }

    /* Now we know exactly which timezone change applies to the time, so
       we can return the UTC offset and whether it is a daylight time. */
    if (is_daylight) {
        *is_daylight = zone_change->is_daylight;
    }
    utc_offset_change = zone_change->utc_offset;

    icaltimezone_changes_unlock();

    return utc_offset_change;
}

int icaltimezone_get_utc_offset_of_utc_time(icaltimezone *zone,
                                            struct icaltimetype *tt, int *is_daylight)
{
    icaltimezonechange *zone_change, tt_change, tmp_change;
    size_t change_num, change_num_to_use;
    int found_change = 1;
    int step, utc_offset;

    if (is_daylight)
        *is_daylight = 0;

    /* For local times and UTC return 0. */
    if (zone == NULL || zone == &utc_timezone)
        return 0;

    /* Use the builtin icaltimezone if possible. */
    if (zone->builtin_timezone)
        zone = zone->builtin_timezone;

    icaltimezone_changes_lock();

    /* Make sure the changes array is expanded up to the given time. */
    icaltimezone_ensure_coverage(zone, tt->year);

    if (!zone->changes || zone->changes->num_elements == 0) {
        icaltimezone_changes_unlock();
        return 0;
    }

    /* Copy the time parts of the icaltimetype to an icaltimezonechange so we
       can use our comparison function on it. */
    tt_change.year = tt->year;
    tt_change.month = tt->month;
    tt_change.day = tt->day;
    tt_change.hour = tt->hour;
    tt_change.minute = tt->minute;
    tt_change.second = tt->second;

    /* This should find a change close to the time, either the change before
       it or the change after it. */
    change_num = icaltimezone_find_nearby_change(zone, &tt_change);

    /* Now move backwards or forwards to find the timezone change that applies
       to tt. It should only have to do 1 or 2 steps. */
    zone_change = icalarray_element_at(zone->changes, change_num);
    step = 1;
    found_change = 0;
    change_num_to_use = -1;
    for (;;) {
        /* Copy the change and adjust it to UTC. */
        tmp_change = *zone_change;

        /* If the given time is on or after this change, then this change may
           apply, but we continue as a later change may be the right one.
           If the given time is before this change, then if we have already
           found a change which applies we can use that, else we need to step
           backwards. */
        if (icaltimezone_compare_change_fn(&tt_change, &tmp_change) >= 0) {
            found_change = 1;
            change_num_to_use = change_num;
        } else {
            step = -1;
        }

        /* If we are stepping backwards through the changes and we have found
           a change that applies, then we know this is the change to use so
           we exit the loop. */
        if (step == -1 && found_change == 1)
            break;

        /* If we go past the start of the changes array, then we have no data
           for this time so we return the prev UTC offset. */
        if (change_num == 0 && step < 0) {
            if (is_daylight) {
                *is_daylight = ! tmp_change.is_daylight;
            }

            icaltimezone_changes_unlock();

            return tmp_change.prev_utc_offset;
        }

        change_num += step;

        if (change_num >= zone->changes->num_elements)
            break;

        zone_change = icalarray_element_at(zone->changes, change_num);
    }

    /* If we didn't find a change to use, then we have a bug! */
    icalerror_assert(found_change == 1, "No applicable timezone change found");

    /* Now we know exactly which timezone change applies to the time, so
       we can return the UTC offset and whether it is a daylight time. */
    zone_change = icalarray_element_at(zone->changes, change_num_to_use);
    if (is_daylight) {
        *is_daylight = zone_change->is_daylight;
    }
    utc_offset = zone_change->utc_offset;

    icaltimezone_changes_unlock();

    return utc_offset;
}

/** @brief Returns the index of a timezone change which is close to the time
 * given in change.
 *
 * Hold icaltimezone_changes_lock(); before calling this function.
*/
static size_t icaltimezone_find_nearby_change(icaltimezone *zone, icaltimezonechange * change)
{
    icaltimezonechange *zone_change;
    size_t lower, middle, upper;
    int cmp;

    /* Do a simple binary search. */
    lower = middle = 0;
    upper = zone->changes->num_elements;

    while (lower < upper) {
        middle = (lower + upper) / 2;
        zone_change = icalarray_element_at(zone->changes, middle);
        cmp = icaltimezone_compare_change_fn(change, zone_change);
        if (cmp == 0) {
            break;
        } else if (cmp < 0) {
            upper = middle;
        } else {
            lower = middle + 1;
        }
    }

    return middle;
}

/** @brief Adds (or subtracts) a time from a icaltimezonechange.
 *
 * NOTE: This function is exactly the same as icaltime_adjust() except
 * for the type of the first parameter.
 */
static void icaltimezone_adjust_change(icaltimezonechange *tt,
                                       int days, int hours, int minutes, int seconds)
{
    int second, minute, hour, day;
    int minutes_overflow, hours_overflow, days_overflow;
    int days_in_month;

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

const char *icaltimezone_get_tzid(icaltimezone *zone)
{
    /* If this is a floating time, without a timezone, return NULL. */
    if (!zone)
        return NULL;

    icaltimezone_load_builtin_timezone(zone);

    return zone->tzid;
}

const char *icaltimezone_get_location(icaltimezone *zone)
{
    /* If this is a floating time, without a timezone, return NULL. */
    if (!zone)
        return NULL;

    /* Note that for builtin timezones this comes from zones.tab so we don't
       need to check the timezone is loaded here. */
    return zone->location;
}

const char *icaltimezone_get_tznames(icaltimezone *zone)
{
    /* If this is a floating time, without a timezone, return NULL. */
    if (!zone)
        return NULL;

    icaltimezone_load_builtin_timezone(zone);

    return zone->tznames;
}

double icaltimezone_get_latitude(icaltimezone *zone)
{
    /* If this is a floating time, without a timezone, return 0. */
    if (!zone)
        return 0.0;

    /* Note that for builtin timezones this comes from zones.tab so we don't
       need to check the timezone is loaded here. */
    return zone->latitude;
}

double icaltimezone_get_longitude(icaltimezone *zone)
{
    /* If this is a floating time, without a timezone, return 0. */
    if (!zone)
        return 0.0;

    /* Note that for builtin timezones this comes from zones.tab so we don't
       need to check the timezone is loaded here. */
    return zone->longitude;
}

icalcomponent *icaltimezone_get_component(icaltimezone *zone)
{
    /* If this is a floating time, without a timezone, return NULL. */
    if (!zone)
        return NULL;

    icaltimezone_load_builtin_timezone(zone);

    return zone->component;
}

int icaltimezone_set_component(icaltimezone *zone, icalcomponent *comp)
{
    icaltimezone_reset(zone);
    return icaltimezone_get_vtimezone_properties(zone, comp);
}

static const char *skip_slashes(const char *text, int n_slashes)
{
    const char *pp;
    int num_slashes = 0;

    if(!text)
        return NULL;

    for (pp = text; *pp; pp++) {
        if(*pp == '/') {
            num_slashes++;
            if(num_slashes == n_slashes)
                return pp + 1;
        }
    }

    return NULL;
}

const char *icaltimezone_get_display_name(icaltimezone *zone)
{
    const char *display_name;
    const char *tzid_prefix;

    display_name = icaltimezone_get_location(zone);
    if (!display_name) {
        display_name = icaltimezone_get_tznames(zone);
    }
    if (!display_name) {
        display_name = icaltimezone_get_tzid(zone);
        tzid_prefix = icaltimezone_tzid_prefix();
        /* Outlook will strip out X-LIC-LOCATION property and so all
           we get back in the iTIP replies is the TZID. So we see if
           this is one of our TZIDs and if so we jump to the city name
           at the end of it. */
        if (display_name &&
            !strncmp(display_name, tzid_prefix, strlen(tzid_prefix))) {
            /* Skip past our prefix */
            display_name += strlen(tzid_prefix);
        }
    }

    return display_name;
}

icalarray *icaltimezone_array_new(void)
{
    return icalarray_new(sizeof(icaltimezone), 16);
}

void icaltimezone_array_append_from_vtimezone(icalarray *timezones, icalcomponent *child)
{
    icaltimezone zone;

    icaltimezone_init(&zone);
    if (icaltimezone_get_vtimezone_properties(&zone, child))
        icalarray_append(timezones, &zone);
}

void icaltimezone_array_free(icalarray *timezones)
{
    icaltimezone *zone;
    size_t i;

    if (timezones) {
        for (i = 0; i < timezones->num_elements; i++) {
            zone = icalarray_element_at(timezones, i);
            icaltimezone_free(zone, 0);
        }

        icalarray_free(timezones);
    }
}

/*
 * BUILTIN TIMEZONE HANDLING
 */

icalarray *icaltimezone_get_builtin_timezones(void)
{
    if (!builtin_timezones)
        icaltimezone_init_builtin_timezones();

    return builtin_timezones;
}

void icaltimezone_free_builtin_timezones(void)
{
    icaltimezone_array_free(builtin_timezones);
    builtin_timezones = 0;
}

icaltimezone *icaltimezone_get_builtin_timezone(const char *location)
{
    icalcomponent *comp;
    icaltimezone *zone;
    size_t lower;
    const char *zone_location;

    if (!location || !location[0])
        return NULL;

    if (!builtin_timezones)
        icaltimezone_init_builtin_timezones();

    if (strcmp(location, "UTC") == 0 || strcmp(location, "GMT") == 0)
        return &utc_timezone;

#if 0
    /* Do a simple binary search. */
    lower = middle = 0;
    upper = builtin_timezones->num_elements;

    while (lower < upper) {
        middle = (lower + upper) / 2;
        zone = icalarray_element_at(builtin_timezones, middle);
        zone_location = icaltimezone_get_location(zone);
        cmp = strcmp(location, zone_location);
        if (cmp == 0) {
            return zone;
        } else if (cmp < 0) {
            upper = middle;
        } else {
            lower = middle + 1;
        }
    }
#endif

    /* The zones from the system are not stored in alphabetical order,
       so we just do a sequential search */
    for (lower = 0; lower < builtin_timezones->num_elements; lower++) {
        zone = icalarray_element_at(builtin_timezones, lower);
        zone_location = icaltimezone_get_location(zone);
        if (strcmp(location, zone_location) == 0)
            return zone;
    }

    /* Check whether file exists, but is not mentioned in zone.tab.
       It means it's a deprecated timezone, but still available. */
    comp = icaltzutil_fetch_timezone(location);
    if (comp) {
        icaltimezone tz;

        icaltimezone_init(&tz);
        if (icaltimezone_set_component(&tz, comp)) {
            icalarray_append(builtin_timezones, &tz);
            return icalarray_element_at(builtin_timezones, builtin_timezones->num_elements - 1);
        } else {
            icalcomponent_free(comp);
        }
    }

    return NULL;
}

static struct icaltimetype tm_to_icaltimetype(struct tm *tm)
{
    struct icaltimetype itt;

    memset(&itt, 0, sizeof(struct icaltimetype));

    /* cppcheck-suppress ctuuninitvar */
    itt.second = tm->tm_sec;
    itt.minute = tm->tm_min;
    itt.hour = tm->tm_hour;

    itt.day = tm->tm_mday;
    itt.month = tm->tm_mon + 1;
    itt.year = tm->tm_year + 1900;

    itt.is_date = 0;

    return itt;
}

static int get_offset(icaltimezone *zone)
{
    struct tm local;
    struct icaltimetype tt;
    int offset;
    const icaltime_t now = icaltime(NULL);

    if (!icalgmtime_r(&now, &local))
        return 0;

    tt = tm_to_icaltimetype(&local);
    offset = icaltimezone_get_utc_offset(zone, &tt, NULL);

    return offset;
}

icaltimezone *icaltimezone_get_builtin_timezone_from_offset(int offset, const char *tzname)
{
    icaltimezone *zone = NULL;
    size_t i, count;

    if (!builtin_timezones)
        icaltimezone_init_builtin_timezones();

    if (offset == 0)
        return &utc_timezone;

    if (!tzname)
        return NULL;

    count = builtin_timezones->num_elements;

    for (i = 0; i < count; i++) {
        int z_offset;

        zone = icalarray_element_at(builtin_timezones, i);
        icaltimezone_load_builtin_timezone(zone);

        z_offset = get_offset(zone);

        if (z_offset == offset && zone->tznames && !strcmp(tzname, zone->tznames))
            return zone;
    }

    return NULL;
}

icaltimezone *icaltimezone_get_builtin_timezone_from_tzid(const char *tzid)
{
    const char *p, *zone_tzid, *tzid_prefix;
    icaltimezone *zone;
    int compat = 0;

    if (!tzid || !tzid[0])
        return NULL;

    if (strcmp(tzid, "UTC") == 0 || strcmp(tzid, "GMT") == 0) {
        return icaltimezone_get_builtin_timezone(tzid);
    }

    tzid_prefix = icaltimezone_tzid_prefix();
    /* Check that the TZID starts with our unique prefix. */
    if (strncmp(tzid, tzid_prefix, strlen(tzid_prefix))) {
        int ii;

        for (ii = 0; glob_compat_tzids[ii].tzid; ii++) {
            if(strncmp(tzid, glob_compat_tzids[ii].tzid, strlen(glob_compat_tzids[ii].tzid)) == 0) {
                p = skip_slashes(tzid, glob_compat_tzids[ii].slashes);
                if(p) {
                    zone = icaltimezone_get_builtin_timezone(p);
                    /* Do not recheck the TZID matches exactly, it does not, because
                       fallbacking with the compatibility timezone prefix here. */
                    return zone;
                }
                break;
            }
        }

        return NULL;
    }

    /* Skip past our prefix */
    p = tzid + strlen(tzid_prefix);

    /* Special-case "/freeassociation.sourceforge.net/Tzfile/"
       because it shares prefix with BUILTIN_TZID_PREFIX */
    if (strcmp(tzid_prefix, BUILTIN_TZID_PREFIX) == 0 &&
        strncmp(p, "Tzfile/", 7) == 0) {
        p += 7;
        compat = 1;
    }

    /* Now we can use the function to get the builtin timezone from the
       location string. */
    zone = icaltimezone_get_builtin_timezone(p);
    if (!zone || compat)
        return zone;

#if defined(USE_BUILTIN_TZDATA)
    if (use_builtin_tzdata)
        return zone;
#endif

    /* Check that the builtin TZID matches exactly. We don't want to return
       a different version of the VTIMEZONE. */
    zone_tzid = icaltimezone_get_tzid(zone);
    if (!strcmp(zone_tzid, tzid)) {
        return zone;
    } else {
        return NULL;
    }
}

icaltimezone *icaltimezone_get_utc_timezone(void)
{
    if (!builtin_timezones)
        icaltimezone_init_builtin_timezones();

    return &utc_timezone;
}

/** @brief Initializes the builtin timezone data, i.e. the
 * builtin_timezones array and the special UTC timezone.
 *
 * It should be called before any code that uses the timezone functions.
 */
static void icaltimezone_init_builtin_timezones(void)
{
    /* Initialize the special UTC timezone. */
    utc_timezone.tzid = (char *)"UTC";

    icaltimezone_builtin_lock();
    if (!builtin_timezones) {
        icaltimezone_parse_zone_tab();
    }
    icaltimezone_builtin_unlock();
}

static int parse_coord(char *coord, int len, int *degrees, int *minutes, int *seconds)
{
    if (len == 5) {
        sscanf(coord + 1, "%2d%2d", degrees, minutes);
    } else if (len == 6) {
        sscanf(coord + 1, "%3d%2d", degrees, minutes);
    } else if (len == 7) {
        sscanf(coord + 1, "%2d%2d%2d", degrees, minutes, seconds);
    } else if (len == 8) {
        sscanf(coord + 1, "%3d%2d%2d", degrees, minutes, seconds);
    } else {
        icalerrprintf("Invalid coordinate: %s\n", coord);
        return 1;
    }

    if (coord[0] == '-')
        *degrees = -*degrees;

    return 0;
}

static int fetch_lat_long_from_string(const char *str,
                                      int *latitude_degrees, int *latitude_minutes,
                                      int *latitude_seconds,
                                      int *longitude_degrees, int *longitude_minutes,
                                      int *longitude_seconds,
                                      char *location)
{
    size_t len;
    char *sptr, *lat, *lon, *loc, *temp;

    /* We need to parse the latitude/longitude coordinates and location fields  */
    sptr = (char *)str;
    while ((*sptr != '\t') && (*sptr != '\0')) {
        sptr++;
    }
    temp = ++sptr;
    while (*sptr != '\t' && *sptr != '\0') {
        sptr++;
    }
    len = (ptrdiff_t) (sptr - temp);
    lat = (char *)icalmemory_new_buffer(len + 1);
    memset(lat, '\0', len + 1);
    strncpy(lat, temp, len);
    lat[len] = '\0';
    while ((*sptr != '\t') && (*sptr != '\0')) {
        sptr++;
    }
    loc = ++sptr;
    while (!isspace((int)(*sptr)) && (*sptr != '\0')) {
        sptr++;
    }
    len = (ptrdiff_t)(sptr - loc);
    strncpy(location, loc, len);
    location[len] = '\0';

#if defined(sun) && defined(__SVR4)
    /* Handle EET, MET and WET in zone_sun.tab. */
    if (!strcmp(location, "Europe/")) {
        while ((*sptr != '\t') && (*sptr != '\0')) {
            sptr++;
        }
        loc = ++sptr;
        while (!isspace(*sptr) && (*sptr != '\0')) {
            sptr++;
        }
        len = (ptrdiff_t)(sptr - loc);
        strncpy(location, loc, len);
        location[len] = '\0';
    }
#endif

    lon = lat + 1;
    while (*lon != '\0' && *lon != '+' && *lon != '-') {
        lon++;
    }

    if (parse_coord(lat, (int)(lon - lat),
                    latitude_degrees,
                    latitude_minutes,
                    latitude_seconds) == 1 ||
        parse_coord(lon, (int)strlen(lon),
                    longitude_degrees, longitude_minutes, longitude_seconds) == 1) {
        icalmemory_free_buffer(lat);
        return 1;
    }

    icalmemory_free_buffer(lat);

    return 0;
}

/** @brief Parses the zones.tab file containing the names and locations
 * of the builtin timezones.
 *
 * It creates the builtin_timezones array
 * which is an icalarray of icaltimezone structs. It only fills in the
 * location, latitude and longtude fields; the rest are left
 * blank. The VTIMEZONE component is loaded later if it is needed. The
 * timezones in the zones.tab file are sorted by their name, which is
 * useful for binary searches.
 */
static void icaltimezone_parse_zone_tab(void)
{
    const char *zonedir, *zonetab;
    char *filename;
    FILE *fp;
    char buf[1024];     /* Used to store each line of zones.tab as it is read. */
    char location[1024];        /* Stores the city name when parsing buf. */
    size_t filename_len;
    int latitude_degrees = 0, latitude_minutes = 0, latitude_seconds = 0;
    int longitude_degrees = 0, longitude_minutes = 0, longitude_seconds = 0;
    icaltimezone zone;

    icalerror_assert(builtin_timezones == NULL, "Parsing zones.tab file multiple times");

    builtin_timezones = icalarray_new(sizeof(icaltimezone), 1024);

    if (!use_builtin_tzdata) {
        zonedir = icaltzutil_get_zone_directory();
        zonetab = ZONES_TAB_SYSTEM_FILENAME;
    } else {
        zonedir = get_zone_directory();
        zonetab = ZONES_TAB_FILENAME;
    }

    filename_len = 0;
    if (zonedir) {
        filename_len = strlen(zonedir);
    }

    icalerror_assert(filename_len > 0, "Unable to locate a zoneinfo dir");
    if (filename_len == 0) {
        icalerror_set_errno(ICAL_INTERNAL_ERROR);\
        return;
    }

    filename_len += strlen(zonetab);
    filename_len += 2; /* for dir separator and final '\0' */

    filename = (char *)icalmemory_new_buffer(filename_len);
    if (!filename) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return;
    }
    snprintf(filename, filename_len, "%s/%s", zonedir, zonetab);

    fp = fopen(filename, "r");
    icalmemory_free_buffer(filename);
    icalerror_assert(fp, "Cannot open the zonetab file for reading");
    if (!fp) {
        icalerror_set_errno(ICAL_INTERNAL_ERROR);
        return;
    }

    while (fgets(buf, (int)sizeof(buf), fp)) {
        if (*buf == '#')
            continue;

        if (use_builtin_tzdata) {
            /* The format of each line is: "[ latitude longitude ] location". */
            if (buf[0] != '+' && buf[0] != '-') {
                latitude_degrees = longitude_degrees = 360;
                latitude_minutes = longitude_minutes = 0;
                latitude_seconds = longitude_seconds = 0;
                if (sscanf(buf, "%1000s", location) != 1) {     /*limit location to 1000chars */
                    /*increase as needed */
                    /*see location and buf declarations */
                    icalerrprintf("Invalid timezone description line: %s\n", buf);
                    continue;
                }
            } else if (sscanf(buf, "%4d%2d%2d %4d%2d%2d %1000s", /*limit location to 1000chars */
                              /*increase as needed */
                              /*see location and buf declarations */
                              &latitude_degrees, &latitude_minutes,
                              &latitude_seconds,
                              &longitude_degrees, &longitude_minutes,
                              &longitude_seconds, location) != 7) {
                icalerrprintf("Invalid timezone description line: %s\n", buf);
                continue;
            }
        } else {
            /* coverity[tainted_data] */
            if (fetch_lat_long_from_string(buf, &latitude_degrees, &latitude_minutes,
                                           &latitude_seconds,
                                           &longitude_degrees, &longitude_minutes,
                                           &longitude_seconds, location)) {
                icalerrprintf("Invalid timezone description line: %s\n", buf);
                continue;
            }
        }

        icaltimezone_init(&zone);
        zone.location = icalmemory_strdup(location);

        if (latitude_degrees >= 0) {
            zone.latitude =
                (double)latitude_degrees +
                (double)latitude_minutes / 60 +
                (double)latitude_seconds / 3600;
        } else {
            zone.latitude =
                (double)latitude_degrees -
                (double)latitude_minutes / 60 -
                (double)latitude_seconds / 3600;
        }

        if (longitude_degrees >= 0) {
            zone.longitude =
                (double)longitude_degrees +
                (double)longitude_minutes / 60 +
                (double)longitude_seconds / 3600;
        } else {
            zone.longitude =
                (double)longitude_degrees -
                (double)longitude_minutes / 60 -
                (double)longitude_seconds / 3600;
        }

        icalarray_append(builtin_timezones, &zone);

#if 0
        printf("Found zone: %s %f %f\n", location, zone.latitude, zone.longitude);
#endif
    }

    fclose(fp);
}

void icaltimezone_release_zone_tab(void)
{
    size_t i;
    icalarray *mybuiltin_timezones = builtin_timezones;

    if (builtin_timezones == NULL)
        return;

    builtin_timezones = NULL;
    for (i = 0; i < mybuiltin_timezones->num_elements; i++) {
        icalmemory_free_buffer(((icaltimezone *) icalarray_element_at(mybuiltin_timezones, i))->location);
    }
    icalarray_free(mybuiltin_timezones);
}

/** @brief Loads the builtin VTIMEZONE data for the given timezone. */
static void icaltimezone_load_builtin_timezone(icaltimezone *zone)
{
    icalcomponent *comp = 0, *subcomp;

    /* Prevent blocking on mutex lock caused by recursive calls */
    if (zone->component)
        return;

    icaltimezone_builtin_lock();

    /* Try again, maybe it had been set by other thread while waiting for the lock */
    if (zone->component) {
        icaltimezone_builtin_unlock();
        return;
    }

    /* If the location isn't set, it isn't a builtin timezone. */
    if (!zone->location || !zone->location[0]) {
        icaltimezone_builtin_unlock();
        return;
    }

    if (use_builtin_tzdata) {
        char *filename;
        size_t filename_len;
        FILE *fp;
        icalparser *parser;

        filename_len = strlen(get_zone_directory()) + strlen(zone->location) + 6;

        filename = (char *)icalmemory_new_buffer(filename_len);
        if (!filename) {
            icalerror_set_errno(ICAL_NEWFAILED_ERROR);
            goto out;
        }

        snprintf(filename, filename_len, "%s/%s.ics", get_zone_directory(), zone->location);

        fp = fopen(filename, "r");
        icalmemory_free_buffer(filename);
        if (!fp) {
            icalerror_set_errno(ICAL_FILE_ERROR);
            goto out;
        }

        /* ##### B.# Sun, 11 Nov 2001 04:04:29 +1100
           this is where the MALFORMEDDATA error is being set, after the call to 'icalparser_parse'
           icalerrprintf("** WARNING ** %s: %d %s\n",
                   __FILE__, __LINE__, icalerror_strerror(icalerrno));
         */

        parser = icalparser_new();
        icalparser_set_gen_data(parser, fp);
        comp = icalparser_parse(parser, icaltimezone_load_get_line_fn);
        icalparser_free(parser);
        fclose(fp);

        /* Find the VTIMEZONE component inside the VCALENDAR. There should be 1. */
        subcomp = icalcomponent_get_first_component(comp, ICAL_VTIMEZONE_COMPONENT);

        if (subcomp) {
            icalproperty *prop;

            /* Ensure expected TZID */
            prop = icalcomponent_get_first_property(subcomp, ICAL_TZID_PROPERTY);
            if(prop) {
                char *new_tzid;
                size_t new_tzid_len;
                const char *tzid_prefix = icaltimezone_tzid_prefix();

                new_tzid_len = strlen(tzid_prefix) + strlen(zone->location) + 1;
                new_tzid = (char *)icalmemory_new_buffer(sizeof(char) * (new_tzid_len + 1));
                if(new_tzid) {
                    snprintf(new_tzid, new_tzid_len, "%s%s", tzid_prefix, zone->location);
                    icalproperty_set_tzid(prop, new_tzid);
                    icalmemory_free_buffer(new_tzid);
                } else {
                    icalerror_set_errno(ICAL_NEWFAILED_ERROR);
                }
            }

            /* Ensure expected Location - it's for cases where one VTIMEZONE is shared
               between different locations (like Pacific/Midway is Pacific/Pago_Pago).
               This updates the properties, thus when the component is converted to
               the string and back to the component the Location will still match. */
            prop = icalcomponent_get_first_property(subcomp, ICAL_LOCATION_PROPERTY);
            if (prop)
                icalproperty_set_location(prop, zone->location);

            for (prop = icalcomponent_get_first_property(subcomp, ICAL_X_PROPERTY);
                 prop;
                 prop = icalcomponent_get_next_property(subcomp, ICAL_X_PROPERTY)) {
                const char *name;

                name = icalproperty_get_x_name(prop);
                if (name && !strcasecmp(name, "X-LIC-LOCATION")) {
                    icalproperty_set_x(prop, zone->location);
                    break;
                }
            }
        }
    } else {
        subcomp = icaltzutil_fetch_timezone(zone->location);
    }

    if (!subcomp) {
        icalerror_set_errno(ICAL_PARSE_ERROR);
        goto out;
    }

    icaltimezone_get_vtimezone_properties(zone, subcomp);

    if (use_builtin_tzdata) {
        icalcomponent_remove_component(comp, subcomp);
        icalcomponent_free(comp);
    }

  out:
    icaltimezone_builtin_unlock();
    return;
}

/** @brief Callback used from icalparser_parse() */
static char *icaltimezone_load_get_line_fn(char *s, size_t size, void *data)
{
    return fgets(s, (int)size, (FILE *) data);
}

/*
 * DEBUGGING
 */

int icaltimezone_dump_changes(icaltimezone *zone, int max_year, FILE *fp)
{
    static const char months[][4] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    icaltimezonechange *zone_change;
    size_t change_num;
    char buffer[8];

    /* Make sure the changes array is expanded up to the given time. */
    icaltimezone_ensure_coverage(zone, max_year);

#if 0
    printf("Num changes: %i\n", zone->changes->num_elements);
#endif

    icaltimezone_changes_lock();

    for (change_num = 0; change_num < zone->changes->num_elements; change_num++) {
        zone_change = icalarray_element_at(zone->changes, change_num);

        if (zone_change->year > max_year)
            break;

        fprintf(fp, "%s\t%2i %s %04i\t%2i:%02i:%02i",
                zone->location,
                zone_change->day, months[zone_change->month - 1],
                zone_change->year, zone_change->hour, zone_change->minute, zone_change->second);

        /* Wall Clock Time offset from UTC. */
        format_utc_offset(zone_change->utc_offset, buffer, sizeof(buffer));
        fprintf(fp, "\t%s", buffer);

        fprintf(fp, "\n");
    }

    icaltimezone_changes_unlock();

    return 1;
}

/** @brief Formats a UTC offset as "+HHMM" or "+HHMMSS".
 *
 * @p buffer should have space for 8 characters. */
static void format_utc_offset(int utc_offset, char *buffer, size_t buffer_size)
{
    const char *sign = "+";
    int hours, minutes, seconds;

    if (utc_offset < 0) {
        utc_offset = -utc_offset;
        sign = "-";
    }

    hours = utc_offset / 3600;
    minutes = (utc_offset % 3600) / 60;
    seconds = utc_offset % 60;

    /* Sanity check. Standard timezone offsets shouldn't be much more than 12
       hours, and daylight saving shouldn't change it by more than a few hours.
       (The maximum offset is 15 hours 56 minutes at present.) */
    if (hours < 0 || hours >= 24 || minutes < 0 || minutes >= 60 || seconds < 0 || seconds >= 60) {
        icalerrprintf("Warning: Strange timezone offset: H:%i M:%i S:%i\n",
                hours, minutes, seconds);
    }

    if (seconds == 0) {
        snprintf(buffer, buffer_size, "%s%02i%02i", sign, hours, minutes);
    } else {
        snprintf(buffer, buffer_size, "%s%02i%02i%02i", sign, hours, minutes, seconds);
    }
}

static const char *get_zone_directory(void)
{
#if !defined(_WIN32)
    return zone_files_directory == NULL ? ZONEINFO_DIRECTORY : zone_files_directory;
#else
    wchar_t wbuffer[1000];

#if !defined(_WIN32_WCE)
    char buffer[1000], zoneinfodir[1000], dirname[1000];
#else
    wchar_t zoneinfodir[1000], dirname[1000];
#endif
    int used_default;
    static ICAL_GLOBAL_VAR char *cache = NULL;

#if !defined(_WIN32_WCE)
    unsigned char *dirslash, *zislash, *zislashp1;
#else
    wchar_t *dirslash, *zislash;
#endif
    struct stat st;

    if (zone_files_directory)
        return zone_files_directory;

    if (cache)
        return cache;

    /* Get the filename of the application */
    if (!GetModuleFileNameW(NULL, wbuffer, sizeof(wbuffer) / sizeof(wbuffer[0])))
        return ZONEINFO_DIRECTORY;

/*wince supports only unicode*/
#if !defined(_WIN32_WCE)
    /* Convert to system codepage */
    if (!WideCharToMultiByte(CP_ACP, 0, wbuffer, -1, buffer, sizeof(buffer),
                             NULL, &used_default) || used_default) {
        /* Failed, try 8.3 format */
        if (!GetShortPathNameW(wbuffer, wbuffer,
                               sizeof(wbuffer) / sizeof(wbuffer[0])) ||
            !WideCharToMultiByte(CP_ACP, 0, wbuffer, -1, buffer, sizeof(buffer),
                                 NULL, &used_default) || used_default) {
            return ZONEINFO_DIRECTORY;
        }
    }
#endif
    /* Look for the zoneinfo directory somewhere in the path where
     * the app is installed. If the path to the app is
     *
     *      C:\opt\evo-2.6\bin\evolution-2.6.exe
     *
     * and the compile-time ZONEINFO_DIRECTORY is
     *
     *      C:/devel/target/evo/share/evolution-data-server-1.6/zoneinfo,
     *
     * we check the pathnames:
     *
     *      C:\opt\evo-2.6/devel/target/evo/share/evolution-data-server-1.6/zoneinfo
     *      C:\opt\evo-2.6/target/evo/share/evolution-data-server-1.6/zoneinfo
     *      C:\opt\evo-2.6/evo/share/evolution-data-server-1.6/zoneinfo
     *      C:\opt\evo-2.6/share/evolution-data-server-1.6/zoneinfo         <===
     *      C:\opt\evo-2.6/evolution-data-server-1.6/zoneinfo
     *      C:\opt\evo-2.6/zoneinfo
     *      C:\opt/devel/target/evo/share/evolution-data-server-1.6/zoneinfo
     *      C:\opt/target/evo/share/evolution-data-server-1.6/zoneinfo
     *      C:\opt/evo/share/evolution-data-server-1.6/zoneinfo
     *      C:\opt/share/evolution-data-server-1.6/zoneinfo
     *      C:\opt/evolution-data-server-1.6/zoneinfo
     *      C:\opt/zoneinfo
     *      C:/devel/target/evo/share/evolution-data-server-1.6/zoneinfo
     *      C:/target/evo/share/evolution-data-server-1.6/zoneinfo
     *      C:/evo/share/evolution-data-server-1.6/zoneinfo
     *      C:/share/evolution-data-server-1.6/zoneinfo
     *      C:/evolution-data-server-1.6/zoneinfo
     *      C:/zoneinfo
     *
     * In Evolution's case, we would get a match already at the
     * fourth pathname check.
     */

    /* Strip away basename of app .exe first */
#if !defined(_WIN32_WCE)
    dirslash = _mbsrchr((unsigned char *)buffer, '\\');
#else
    dirslash = wcsrchr(wbuffer, L'\\');
#endif
    if (dirslash) {
#if !defined(_WIN32_WCE)
        *dirslash = '\0';
#else
        *dirslash = L'\0';
#endif
    }

#if defined(_WIN32_WCE)
    while ((dirslash = wcsrchr(wbuffer, '\\'))) {
        /* Strip one more directory from app .exe location */
        *dirslash = L'\0';

        MultiByteToWideChar(CP_ACP, 0, ZONEINFO_DIRECTORY, -1, zoneinfodir, 1000);

        while ((zislash = wcschr(zoneinfodir, L'/'))) {
            *zislash = L'.';
            wcscpy(dirname, wbuffer);
            wcscat(dirname, "/");
            wcscat(dirname, zislash + 1);
            if (stat(dirname, &st) == 0 && S_ISDIR(st.st_mode)) {
                cache = wce_wctomb(dirname);
                return cache;
            }
        }
    }
#else
    while ((dirslash = _mbsrchr((unsigned char *)buffer, '\\'))) {
        /* Strip one more directory from app .exe location */
        *dirslash = '\0';

        strcpy(zoneinfodir, ZONEINFO_DIRECTORY);
        while ((zislash = _mbschr((unsigned char *)zoneinfodir, '/'))) {
            *zislash = '.';
            strcpy(dirname, buffer);
            strcat(dirname, "/");
            zislashp1 = zislash + 1;
            strcat(dirname, (char *)zislashp1);
            if (stat(dirname, &st) == 0 && S_ISDIR(st.st_mode)) {
                cache = icalmemory_strdup(dirname);
                return cache;
            }
        }
    }
#endif
    return ZONEINFO_DIRECTORY;
#endif
}

void set_zone_directory(const char *path)
{
    if (zone_files_directory)
        free_zone_directory();

    zone_files_directory = icalmemory_new_buffer(strlen(path) + 1);

    if (zone_files_directory != NULL)
        strcpy(zone_files_directory, path);
}

void free_zone_directory(void)
{
    if (zone_files_directory != NULL) {
        icalmemory_free_buffer(zone_files_directory);
        zone_files_directory = NULL;
    }
}

void icaltimezone_set_tzid_prefix(const char *new_prefix)
{
    if (new_prefix) {
        strncpy(s_ical_tzid_prefix, new_prefix, BUILTIN_TZID_PREFIX_LEN-1);
    }
}

void icaltimezone_set_builtin_tzdata(int set)
{
    use_builtin_tzdata = set;
}

int icaltimezone_get_builtin_tzdata(void)
{
    return use_builtin_tzdata;
}

struct observance {
    const char *name;
    icaltimetype onset;
    int offset_from;
    int offset_to;
};

static void check_tombstone(struct observance *tombstone,
                            struct observance *obs)
{
    if (icaltime_compare(obs->onset, tombstone->onset) > 0) {
        /* onset is closer to cutoff than existing tombstone */
        tombstone->name = icalmemory_tmp_copy(obs->name);
        tombstone->offset_from = tombstone->offset_to = obs->offset_to;
        tombstone->onset = obs->onset;
    }
}

struct rdate {
    icalproperty *prop;
    struct icaldatetimeperiodtype date;
};

static int rdate_compare(const void *rdate1, const void *rdate2)
{
    return icaltime_compare(((struct rdate *) rdate1)->date.time,
                            ((struct rdate *) rdate2)->date.time);
}

void icaltimezone_truncate_vtimezone(icalcomponent *vtz,
                                     icaltimetype start, icaltimetype end,
                                     int ms_compatible)
{
    icalcomponent *comp, *nextc, *tomb_std = NULL, *tomb_day = NULL;
    icalproperty *prop, *proleptic_prop = NULL;
    struct observance tombstone;
    unsigned need_tomb = (unsigned)!icaltime_is_null_time(start);
    unsigned need_tzuntil = (unsigned)!icaltime_is_null_time(end);

    if (!need_tomb && !need_tzuntil) {
        /* Nothing to do */
        return;
    }

    /* See if we have a proleptic tzname in VTIMEZONE */
    for (prop = icalcomponent_get_first_property(vtz, ICAL_X_PROPERTY);
         prop;
         prop = icalcomponent_get_next_property(vtz, ICAL_X_PROPERTY)) {
        if (!strcmp("X-PROLEPTIC-TZNAME", icalproperty_get_x_name(prop))) {
            proleptic_prop = prop;
            break;
        }
    }

    memset(&tombstone, 0, sizeof(struct observance));
    tombstone.name = icalmemory_tmp_copy(proleptic_prop ?
                                         icalproperty_get_x(proleptic_prop) :
                                         "LMT");
    if (!proleptic_prop ||
        !icalproperty_get_parameter_as_string(proleptic_prop, "X-NO-BIG-BANG")) {
      tombstone.onset.year = -1;
    }

    /* Process each VTMEZONE STANDARD/DAYLIGHT subcomponent */
    for (comp = icalcomponent_get_first_component(vtz, ICAL_ANY_COMPONENT);
         comp; comp = nextc) {
        icalproperty *dtstart_prop = NULL, *rrule_prop = NULL;
        icalarray *rdates = icalarray_new(sizeof(struct rdate), 10);
        icaltimetype dtstart;
        struct observance obs;
        size_t n;
        unsigned trunc_dtstart = 0;
        int r;

        nextc = icalcomponent_get_next_component(vtz, ICAL_ANY_COMPONENT);

        memset(&obs, 0, sizeof(struct observance));
        obs.offset_from = obs.offset_to = INT_MAX;
        obs.onset.is_daylight =
            (icalcomponent_isa(comp) == ICAL_XDAYLIGHT_COMPONENT);

        /* Grab the properties that we require to expand recurrences */
        for (prop = icalcomponent_get_first_property(comp, ICAL_ANY_PROPERTY);
             prop;
             prop = icalcomponent_get_next_property(comp, ICAL_ANY_PROPERTY)) {

            switch (icalproperty_isa(prop)) {
            case ICAL_TZNAME_PROPERTY:
                obs.name = icalproperty_get_tzname(prop);
                break;

            case ICAL_DTSTART_PROPERTY:
                dtstart_prop = prop;
                obs.onset = dtstart = icalproperty_get_dtstart(prop);
                break;

            case ICAL_TZOFFSETFROM_PROPERTY:
                obs.offset_from = icalproperty_get_tzoffsetfrom(prop);
                break;

            case ICAL_TZOFFSETTO_PROPERTY:
                obs.offset_to = icalproperty_get_tzoffsetto(prop);
                break;

            case ICAL_RRULE_PROPERTY:
                rrule_prop = prop;
                break;

            case ICAL_RDATE_PROPERTY: {
                struct icaldatetimeperiodtype dtp = icalproperty_get_rdate(prop);
                struct rdate rdate;

                rdate.prop = prop;
                rdate.date.time = dtp.time;
                rdate.date.period = dtp.period;

                icalarray_append(rdates, &rdate);
                break;
            }

            default:
                /* ignore all other properties */
                break;
            }
        }

        /* We MUST have DTSTART, TZNAME, TZOFFSETFROM, and TZOFFSETTO */
        if (!dtstart_prop || !obs.name ||
            obs.offset_from == INT_MAX || obs.offset_to == INT_MAX) {
            icalarray_free(rdates);
            continue;
        }

        /* Adjust DTSTART observance to UTC */
        icaltime_adjust(&obs.onset, 0, 0, 0, -obs.offset_from);
        (void)icaltime_set_timezone(&obs.onset, icaltimezone_get_utc_timezone());

        /* Check DTSTART vs window close */
        if (need_tzuntil && icaltime_compare(obs.onset, end) >= 0) {
            /* All observances occur on/after window close - remove component */
            icalcomponent_remove_component(vtz, comp);
            icalcomponent_free(comp);

            /* Nothing else to do */
            icalarray_free(rdates);
            continue;
        }

        /* Check DTSTART vs window open */
        r = icaltime_compare(obs.onset, start);
        if (r < 0) {
            /* DTSTART is prior to our window open - check it vs tombstone */
            if (need_tomb) {
                check_tombstone(&tombstone, &obs);
            }

            /* Adjust it */
            trunc_dtstart = 1;
        } else if (r == 0) {
            /* DTSTART is on/after our window open */
            need_tomb = 0;
        }

        if (rrule_prop) {
            struct icalrecurrencetype rrule = icalproperty_get_rrule(rrule_prop);
            unsigned eternal = (unsigned)icaltime_is_null_time(rrule.until);
            icalrecur_iterator *ritr = NULL;
            unsigned trunc_until = 0;

            /* Check RRULE duration */
            if (!eternal && icaltime_compare(rrule.until, start) < 0) {
                /* RRULE ends prior to our window open -
                   check UNTIL vs tombstone */
                obs.onset = rrule.until;
                if (need_tomb) {
                    check_tombstone(&tombstone, &obs);
                }

                /* Remove RRULE */
                icalcomponent_remove_property(comp, rrule_prop);
                icalproperty_free(rrule_prop);
            } else {
                /* RRULE ends on/after our window open */
                if (need_tzuntil &&
                    (eternal || icaltime_compare(rrule.until, end) >= 0)) {
                    /* RRULE ends after our window close - need to adjust it */
                    trunc_until = 1;
                }

                if (!eternal) {
                    /* Adjust UNTIL to local time (for iterator) */
                    icaltime_adjust(&rrule.until, 0, 0, 0, obs.offset_from);
                    (void)icaltime_set_timezone(&rrule.until, NULL);
                }

                ritr = icalrecur_iterator_new(rrule, dtstart);

                if (trunc_dtstart) {
                    /* Bump RRULE start to 1 year prior to our window open */
                    icaltimetype newstart = dtstart;
                    newstart.year  = start.year - 1;
                    newstart.month = start.month;
                    newstart.day   = start.day;
                    (void)icaltime_normalize(newstart);
                    icalrecur_iterator_set_start(ritr, newstart);
                }
            }

            /* Process any RRULE observances within our window */
            if (ritr) {
                icaltimetype recur, prev_onset;

                while (!icaltime_is_null_time(recur = icalrecur_iterator_next(ritr))) {
                    unsigned ydiff;

                    obs.onset = recur;

                    /* Adjust observance to UTC */
                    icaltime_adjust(&obs.onset, 0, 0, 0, -obs.offset_from);
                    (void)icaltime_set_timezone(&obs.onset,
                                                icaltimezone_get_utc_timezone());

                    if (trunc_until && icaltime_compare(obs.onset, end) >= 0) {
                        /* Observance is on/after window close */

                        /* Check if DSTART is within 1yr of prev onset */
                        ydiff = (unsigned)(prev_onset.year - dtstart.year);
                        if (ydiff <= 1) {
                            /* Remove RRULE */
                            icalcomponent_remove_property(comp, rrule_prop);
                            icalproperty_free(rrule_prop);

                            if (ydiff) {
                                /* Add previous onset as RDATE */
                                struct icaldatetimeperiodtype rdate;
                                rdate.time = prev_onset;
                                rdate.period = icalperiodtype_null_period();

                                prop = icalproperty_new_rdate(rdate);
                                icalcomponent_add_property(comp, prop);
                            }
                        } else {
                            /* Set UNTIL to previous onset */
                            rrule.until = prev_onset;
                            icalproperty_set_rrule(rrule_prop, rrule);
                        }

                        /* We're done */
                        break;
                    }

                    /* Check observance vs our window open */
                    r = icaltime_compare(obs.onset, start);
                    if (r < 0) {
                        /* Observance is prior to our window open -
                           check it vs tombstone */
                        if (ms_compatible) {
                            /* XXX  We don't want to move DTSTART of the RRULE
                               as Outlook/Exchange doesn't appear to like
                               truncating the frontend of RRULEs */
                            need_tomb = 0;
                            trunc_dtstart = 0;
                            if (proleptic_prop) {
                                icalcomponent_remove_property(vtz,
                                                              proleptic_prop);
                                icalproperty_free(proleptic_prop);
                                proleptic_prop = NULL;
                            }
                        }
                        if (need_tomb) {
                            check_tombstone(&tombstone, &obs);
                        }
                    } else {
                        /* Observance is on/after our window open */
                        if (r == 0) need_tomb = 0;

                        if (trunc_dtstart) {
                            /* Make this observance the new DTSTART */
                            icalproperty_set_dtstart(dtstart_prop, recur);
                            dtstart = obs.onset;
                            trunc_dtstart = 0;

                            /* Check if new DSTART is within 1yr of UNTIL */
                            ydiff = (unsigned)(rrule.until.year - recur.year);
                            if (!trunc_until && ydiff <= 1) {
                                /* Remove RRULE */
                                icalcomponent_remove_property(comp, rrule_prop);
                                icalproperty_free(rrule_prop);

                                if (ydiff) {
                                    /* Add UNTIL as RDATE */
                                    struct icaldatetimeperiodtype rdate;
                                    rdate.time = rrule.until;
                                    rdate.period = icalperiodtype_null_period();

                                    prop = icalproperty_new_rdate(rdate);
                                    icalcomponent_add_property(comp, prop);
                                }
                            }
                        }

                        if (!trunc_until) {
                            /* We're done */
                            break;
                        }

                        /* Check if observance is outside 1yr of window close */
                        ydiff = (unsigned)(end.year - recur.year);
                        if (ydiff > 1) {
                            /* Bump RRULE to restart at 1 year prior to our window close */
                            icaltimetype newstart = recur;
                            newstart.year  = end.year - 1;
                            newstart.month = end.month;
                            newstart.day   = end.day;
                            (void)icaltime_normalize(newstart);
                            icalrecur_iterator_set_start(ritr, newstart);
                        }
                    }
                    prev_onset = obs.onset;
                }
                icalrecur_iterator_free(ritr);
            }
        }

        /* Sort the RDATEs by onset */
        icalarray_sort(rdates, &rdate_compare);

        /* Check RDATEs */
        for (n = 0; n < rdates->num_elements; n++) {
            struct rdate *rdate = icalarray_element_at(rdates, n);

            /* RDATEs with a DATE value inherit the time from the DTSTART. */
            if (icaltime_is_date(rdate->date.time)) {
                rdate->date.time.hour   = dtstart.hour;
                rdate->date.time.minute = dtstart.minute;
                rdate->date.time.second = dtstart.second;
            }

            if (n == 0 && icaltime_compare(rdate->date.time, dtstart) == 0) {
                /* RDATE is same as DTSTART - remove it */
                icalcomponent_remove_property(comp, rdate->prop);
                icalproperty_free(rdate->prop);
                continue;
            }

            obs.onset = rdate->date.time;

            /* Adjust observance to UTC */
            icaltime_adjust(&obs.onset, 0, 0, 0, -obs.offset_from);
            (void)icaltime_set_timezone(&obs.onset, icaltimezone_get_utc_timezone());

            if (need_tzuntil && icaltime_compare(obs.onset, end) >= 0) {
                /* RDATE is after our window close - remove it */
                icalcomponent_remove_property(comp, rdate->prop);
                icalproperty_free(rdate->prop);

                continue;
            }

            r = icaltime_compare(obs.onset, start);
            if (r < 0) {
                /* RDATE is prior to window open - check it vs tombstone */
                if (need_tomb) {
                    check_tombstone(&tombstone, &obs);
                }

                /* Remove it */
                icalcomponent_remove_property(comp, rdate->prop);
                icalproperty_free(rdate->prop);
            } else {
                /* RDATE is on/after our window open */
                if (r == 0) need_tomb = 0;

                if (trunc_dtstart) {
                    /* Make this RDATE the new DTSTART */
                    icalproperty_set_dtstart(dtstart_prop,
                                             rdate->date.time);
                    trunc_dtstart = 0;

                    icalcomponent_remove_property(comp, rdate->prop);
                    icalproperty_free(rdate->prop);
                }
            }
        }
        icalarray_free(rdates);

        /* Final check */
        if (trunc_dtstart) {
            /* All observances in comp occur prior to window open, remove it
               unless we haven't saved a tombstone comp of this type yet */
            if (icalcomponent_isa(comp) == ICAL_XDAYLIGHT_COMPONENT) {
                if (!tomb_day) {
                    tomb_day = comp;
                    comp = NULL;
                }
            }
            else if (!tomb_std) {
                tomb_std = comp;
                comp = NULL;
            }

            if (comp) {
                icalcomponent_remove_component(vtz, comp);
                icalcomponent_free(comp);
            }
        }
    }

    if (need_tomb && !icaltime_is_null_time(tombstone.onset)) {
        /* Need to add tombstone component/observance starting at window open
           as long as its not prior to start of TZ data */
        icalcomponent *tomb;
        icalproperty *prop, *nextp;

        /* Determine which tombstone component we need */
        if (tombstone.onset.is_daylight) {
            tomb = tomb_day;
            tomb_day = NULL;
        } else {
            tomb = tomb_std;
            tomb_std = NULL;
        }

        /* Set property values on our tombstone */
        for (prop = icalcomponent_get_first_property(tomb, ICAL_ANY_PROPERTY);
             prop; prop = nextp) {

            nextp = icalcomponent_get_next_property(tomb, ICAL_ANY_PROPERTY);

            switch (icalproperty_isa(prop)) {
            case ICAL_TZNAME_PROPERTY:
                icalproperty_set_tzname(prop, tombstone.name);
                break;
            case ICAL_TZOFFSETFROM_PROPERTY:
                icalproperty_set_tzoffsetfrom(prop, tombstone.offset_from);
                break;
            case ICAL_TZOFFSETTO_PROPERTY:
                icalproperty_set_tzoffsetto(prop, tombstone.offset_to);
                break;
            case ICAL_DTSTART_PROPERTY:
                /* Adjust window open to local time */
                icaltime_adjust(&start, 0, 0, 0, tombstone.offset_from);
                (void)icaltime_set_timezone(&start, NULL);

                icalproperty_set_dtstart(prop, start);
                break;
            default:
                icalcomponent_remove_property(tomb, prop);
                icalproperty_free(prop);
                break;
            }
        }

        /* Remove X-PROLEPTIC-TZNAME as it no longer applies */
        if (proleptic_prop) {
            icalcomponent_remove_property(vtz, proleptic_prop);
            icalproperty_free(proleptic_prop);
        }
    }

    /* Remove any unused tombstone components */
    if (tomb_std) {
        icalcomponent_remove_component(vtz, tomb_std);
        icalcomponent_free(tomb_std);
    }
    if (tomb_day) {
        icalcomponent_remove_component(vtz, tomb_day);
        icalcomponent_free(tomb_day);
    }

    if (need_tzuntil) {
        /* Add TZUNTIL to VTIMEZONE */
        prop = icalcomponent_get_first_property(vtz, ICAL_TZUNTIL_PROPERTY);

        if (prop) {
            icalproperty_set_tzuntil(prop, end);
        } else {
            icalcomponent_add_property(vtz, icalproperty_new_tzuntil(end));
        }
    }
}
