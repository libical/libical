/*
 * Vzic - a program to convert Olson timezone database files into VZTIMEZONE
 * files compatible with the iCalendar specification (RFC2445).
 *
 * SPDX-FileCopyrightText: 2000-2001 Ximian, Inc.
 * SPDX-FileCopyrightText: 2003, Damon Chaplin <damon@ximian.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Author: Damon Chaplin <damon@gnome.org>
 *
 */

#ifndef VZIC_H
#define VZIC_H

#include <glib.h>

/* uncomment, to enable debug prints */
/* #define VZIC_DEBUG_PRINT 1 */

/*
 * Global command-line options.
 */

/* By default we output Outlook-compatible output. If --pure is used we output
   pure output, with no changes to be compatible with Outlook. */
extern gboolean VzicPureOutput;
extern gboolean VzicWithArtifacts;

extern gboolean VzicDumpOutput;
extern gboolean VzicDumpChanges;
extern gboolean VzicDumpZoneNamesAndCoords;
extern gboolean VzicDumpZoneTranslatableStrings;
extern gboolean VzicNoRRules;
extern gboolean VzicNoRDates;
extern char *VzicUrlPrefix;
extern const char *VzicOutputDir;

extern GList *VzicTimeZoneNames;

/* The minimum & maximum years we can use. */
#define YEAR_MINIMUM G_MININT
#define YEAR_MAXIMUM G_MAXINT

/* The maximum size of any complete pathname. */
#define PATHNAME_BUFFER_SIZE 1024

/* Defines if to generate symbolic links or VTIMEZONEs for zone aliases */
#ifndef CREATE_SYMLINK
#define CREATE_SYMLINK 0
#endif

/* Defines if timezone aliases at top-level ("e.g. EST5EDT")
 * should be ignored. */
#ifndef IGNORE_TOP_LEVEL_LINK
#define IGNORE_TOP_LEVEL_LINK 0
#endif

/* Days can be expressed either as a simple month day number, 1-31, or a rule
   such as the last Sunday, or the first Monday on or after the 8th. */
typedef enum
{
    DAY_SIMPLE,
    DAY_WEEKDAY_ON_OR_AFTER,
    DAY_WEEKDAY_ON_OR_BEFORE,
    DAY_LAST_WEEKDAY
} DayCode;

/* Times can be given either as universal time (UTC), local standard time
   (without daylight-saving adjustments) or wall clock time (local standard
   time plus daylight-saving adjustments, i.e. what you would see on a clock
   on the wall!). */
typedef enum
{
    TIME_WALL,
    TIME_STANDARD,
    TIME_UNIVERSAL
} TimeCode;

/* This represents one timezone, e.g. "Africa/Algiers".
   It contains the timezone name, and an array of ZoneLineData structs which
   hold data from each Zone line, including the continuation lines. */
typedef struct _ZoneData ZoneData;
struct _ZoneData {
    char *zone_name;

    /* An array of ZoneLineData, one for each Zone & Zone continuation line
     read in. */
    GArray *zone_line_data;
};

typedef struct _ZoneLineData ZoneLineData;
struct _ZoneLineData {
    /* The amount of time to add to UTC to get local standard time for the
     current time range, in seconds. */
    int stdoff_seconds;

    /* Either rules is set to the name of a set of rules, or rules is NULL and
     save is set to the time to add to local standard time to get wall time, in
     seconds. If save is 0 as well, then standard time always applies. */
    char *rules;
    int save_seconds;

    /* The format to use for the abbreviated timezone name, e.g. WE%sT.
     The %s is replaced by variable part of the name. (See the letter_s field
     in the RuleData struct below). */
    char *format;

    /* TRUE if an UNTIL time is given. */
    gboolean until_set;

    /* The UNTIL year, e.g. 2000. */
    int until_year;

    /* The UNTIL month 0 (Jan) to 11 (Dec). */
    int until_month;

    /* The UNTIL day, either a simple month day number, 1-31, or a rule such as
     the last Sunday, or the first Monday on or after the 8th. */
    DayCode until_day_code;
    int until_day_number;  /* 1 to 31. */
    int until_day_weekday; /* 0 (Sun) to 6 (Sat). */

    /* The UNTIL time, in seconds from midnight. The code specifies whether the
     time is a wall clock time, local standard time, or universal time. */
    int until_time_seconds;
    TimeCode until_time_code;
};

typedef struct _RuleData RuleData;
struct _RuleData {
    /* The first year that the rule applies to, e.g. 1996.
     Can also be YEAR_MINIMUM. */
    int from_year;

    /* The last year that the rule applies to, e.g. 1996.
     Can also be YEAR_MAXIMUM. */
    int to_year;

    /* A string used to only match certain years between from and to.
     The rule only applies to the years which match. If type is NULL the rule
     applies to all years between from and to.
     zic uses an external program called yearistype to check the string.
     Currently it is not used in the Olson database. */
    char *type;

    /* The month of the rule 0 (Jan) to 11 (Dec). */
    int in_month;

    /* The day, either a simple month day number, 1-31, or a rule such as
     the last Sunday, or the first Monday on or after the 8th. */
    DayCode on_day_code;
    int on_day_number;
    int on_day_weekday; /* 0 (Sun) to 6 (Sat). */

    /* The time, in seconds from midnight. The code specifies whether the
     time is a wall clock time, local standard time, or universal time. */
    int at_time_seconds;
    TimeCode at_time_code;

    /* The amount of time to add to local standard time when the rule is in
     effect, in seconds. If this is not 0 then it must be a daylight-saving
     time. */
    int save_seconds;

    /* The letter(s) to use as the variable part in the abbreviated timezone
     name. If this is NULL then no variable part is used. (See the format field
     in the ZoneLineData struct above.) */
    char *letter_s;

    /* This is set to TRUE if this element is a shallow copy of another one,
     in which case we don't free any of the fields. */
    gboolean is_shallow_copy;
};

typedef struct _ZoneDescription ZoneDescription;
struct _ZoneDescription {
    /* 2-letter ISO 3166 country code. */
    char country_code[2];

    /* latitude and longitude in degrees, minutes & seconds. The degrees value
     holds the sign of the entire latitude/longitude. */
    int latitude[3];
    int longitude[3];

    char *comment;
};

#endif /* VZIC_H */
