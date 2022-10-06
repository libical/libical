/*======================================================================
 FILE: icalrecur.c
 CREATOR: eric 16 May 2000

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>

 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

========================================================================*/

/**
  icalrecur.c
  @brief Implementation of routines for dealing with recurring time

  How this code works:

  Processing starts when the caller generates a new recurrence
  iterator via icalrecur_iterator_new(). This routine copies the
  recurrence rule into the iterator and extracts things like start and
  end dates. Then, it checks if the rule is legal, using some logic
  from RFC5545 and some logic that probably should be in RFC5545.

  If compiled with support for Non-Gregorian Recurrence Rules (RFC7529),
  icalrecur_iterator_new() verifies that the given RSCALE is supported
  and configures ICU4C to convert occurrences to/from non-Gregorian dates.

  Then, icalrecur_iterator_new() re-writes some of the BY*
  arrays. This involves ( via a call to setup_defaults() ) :

  1) For BY rule parts with no data ( ie BYSECOND was not specified )
  copy the corresponding time part from DTSTART into the BY array. (
  So impl->by_ptrs[BY_SECOND] will then have one element if is
  originally had none ) This only happens if the BY* rule part data
  would expand the number of occurrences in the occurrence set. This
  lets the code ignore DTSTART later on and still use it to get the
  time parts that were not specified in any other way.

  2) For the by rule part that are not the same interval as the
  frequency -- for HOURLY anything but BYHOUR, for instance -- copy the
  first data element from the rule part into the first occurrence. For
  example, for "INTERVAL=MONTHLY and BYHOUR=10,30", initialize the
  first time to be returned to have an hour of 10.

  Finally, for INTERVAL=YEARLY, the routine expands the rule to get
  all of the days specified in the rule. The code will do this for
  each new year, and this is the first expansion. This is a special
  case for the yearly interval; no other frequency gets expanded this
  way. The yearly interval is the most complex, so some special
  processing is required.

  After creating a new iterator, the caller will make successive calls
  to icalrecur_iterator_next() to get the next time specified by the
  rule. The main part of this routine is a switch on the frequency of
  the rule. Each different frequency is handled by a different
  routine.

  For example, next_hour handles the case of INTERVAL=HOURLY, and it
  is called by other routines to get the next hour. First, the routine
  tries to get the next minute part of a time with a call to
  next_minute(). If next_minute() returns 1, it has reached the end of
  its data, usually the last element of the BYMINUTE array. Then, if
  there is data in the BYHOUR array, the routine changes the hour to
  the next one in the array. If INTERVAL=HOURLY, the routine advances
  the hour by the interval.

  If the routine used the last hour in the BYHOUR array, and the
  INTERVAL=HOURLY, then the routine calls increment_monthday() to set
  the next month day. The increment_* routines may call higher routine
  to increment the month or year also.

  The code for INTERVAL=DAILY is handled by next_day(). First, the
  routine tries to get the next hour part of a time with a call to
  next_hour. If next_hour() returns 1, it has reached the end of its
  data, usually the last element of the BYHOUR array. This means that
  next_day() should increment the time to the next day. If FREQUENCY==DAILY,
  the routine increments the day by the interval; otherwise, it
  increments the day by 1.

  Next_day() differs from next_hour because it does not use the BYDAY
  array to select an appropriate day. Instead, it returns every day (
  incrementing by 1 if the frequency is not DAILY with INTERVAL!=1)
  Any days that are not specified in an non-empty BYDAY array are
  filtered out later.

  Generally, the flow of these routine is for a next_* call a next_*
  routine of a lower interval ( next_day calls next_hour) and then to
  possibly call an increment_* routine of an equal or higher
  interval. ( next_day calls increment_monthday() )

  When the call to the original next_* routine returns,
  icalrecur_iterator_next() will check the returned data against other
  BYrule parts to determine if is should be excluded by calling
  check_contracting_rules. Generally, a contracting rule is any with a
  larger time span than the interval. For instance, if
  INTERVAL=DAILY, BYMONTH is a contracting rule part.

  Check_contracting_rules() uses icalrecur_check_rulepart() to do its
  work. icalrecur_check_rulepart() uses expand_map[] to determine if a rule
  is contracting, and if it is, and if the BY rule part has some data,
  then the routine checks if the value of a component of the time is
  part of the byrule part. For instance, for "INTERVAL=DAILY;
  BYMONTH=6,10", icalrecur_check_rulepart() would check that the time value
  given to it has a month of either 6 or 10.

  Finally, icalrecur_iterator_next() does a few other checks on the
  time value, and if it passes, it returns the time.

  A note about the end_of_data flag. The flag indicates that the
  routine is at the end of its data -- the last BY rule if the routine
  is using by rules, or the last day of the week/month/year/etc if
  not.

  This flag is usually set early in a next_* routine and returned in
  the end. The way it is used allows the next_* routine to set the
  last time back to the first element in a BYxx rule, and then signal
  to the higher level routine to increment the next higher level. For
  instance. WITH FREQ=MONTHLY;BYDAY=TU,FR, After next_weekday_by_month
  runs though both TU and FR, it sets the week day back to TU and sets
  end_of_data to 1x. This signals next_month to increment the month.

 ======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "icalrecur.h"
#include "icalerror.h"
#include "icalmemory.h"
#include "icaltimezone.h"
#include "icalvalue.h"  /* for print_date[time]_to_string() */

#include <ctype.h>
#include <stddef.h>     /* For offsetof() macro */
#include <stdlib.h>

#if defined(HAVE_PTHREAD)
#include <pthread.h>
static pthread_mutex_t invalid_rrule_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

static ICAL_GLOBAL_VAR ical_invalid_rrule_handling invalidRruleHandling = ICAL_RRULE_TREAT_AS_ERROR;

#if defined(HAVE_LIBICU)
#include <unicode/ucal.h>
#include <unicode/ustring.h>
#if defined(HAVE_STDBOOL_H)
#include <stdbool.h>
#else
#define false 0
#define true 1
#endif
#else

/* The maximums below are based on Gregorian leap years */
#undef ICAL_BY_MONTH_SIZE
#undef ICAL_BY_WEEKNO_SIZE
#undef ICAL_BY_YEARDAY_SIZE
#define ICAL_BY_MONTH_SIZE      13      /* 1 to 12 */
#define ICAL_BY_WEEKNO_SIZE     54      /* 1 to 53 */
#define ICAL_BY_YEARDAY_SIZE    367     /* 1 to 366 */
#endif

#if (SIZEOF_ICALTIME_T > 4)
/** Arbitrarily go up to 1000th anniversary of Gregorian calendar, since
    64-bit icaltime_t values get us up to the tm_year limit of 2+ billion years. */
#define MAX_TIME_T_YEAR 2582
#else
/** This is the last year we will go up to, since 32-bit icaltime_t values
   only go up to the start of 2038. */
#define MAX_TIME_T_YEAR 2037
#endif

#define BYDAYIDX impl->by_indices[BY_DAY]
#define BYDAYPTR impl->by_ptrs[BY_DAY]

#define BYMONIDX impl->by_indices[BY_MONTH]
#define BYMONPTR impl->by_ptrs[BY_MONTH]

#define BYMDIDX impl->by_indices[BY_MONTH_DAY]
#define BYMDPTR impl->by_ptrs[BY_MONTH_DAY]

#define BYYDIDX impl->by_indices[BY_YEAR_DAY]
#define BYYDPTR impl->by_ptrs[BY_YEAR_DAY]

#define BYWEEKIDX impl->by_indices[BY_WEEK_NO]
#define BYWEEKPTR impl->by_ptrs[BY_WEEK_NO]

#define LEAP_MONTH 0x1000

int icalrecurrencetype_rscale_is_supported(void)
{
    return 1;
}

/****************** Enumeration Routines ******************/

static const struct freq_map
{
    icalrecurrencetype_frequency kind;
    const char str[9];
} freq_map[] = {
    {ICAL_SECONDLY_RECURRENCE, "SECONDLY"},
    {ICAL_MINUTELY_RECURRENCE, "MINUTELY"},
    {ICAL_HOURLY_RECURRENCE, "HOURLY"},
    {ICAL_DAILY_RECURRENCE, "DAILY"},
    {ICAL_WEEKLY_RECURRENCE, "WEEKLY"},
    {ICAL_MONTHLY_RECURRENCE, "MONTHLY"},
    {ICAL_YEARLY_RECURRENCE, "YEARLY"},
    {ICAL_NO_RECURRENCE, ""}
};

icalrecurrencetype_frequency icalrecur_string_to_freq(const char *str)
{
    int i;

    for (i = 0; freq_map[i].kind != ICAL_NO_RECURRENCE; i++) {
        if (strcasecmp(str, freq_map[i].str) == 0) {
            return freq_map[i].kind;
        }
    }
    return ICAL_NO_RECURRENCE;
}

const char *icalrecur_freq_to_string(icalrecurrencetype_frequency kind)
{
    int i;

    for (i = 0; freq_map[i].kind != ICAL_NO_RECURRENCE; i++) {
        if (freq_map[i].kind == kind) {
            return freq_map[i].str;
        }
    }
    return 0;
}

static const struct skip_map
{
    icalrecurrencetype_skip kind;
    const char str[9];
} skip_map[] = {
    {ICAL_SKIP_BACKWARD, "BACKWARD"},
    {ICAL_SKIP_FORWARD, "FORWARD"},
    {ICAL_SKIP_OMIT, "OMIT"},
    {ICAL_SKIP_UNDEFINED, ""}
};

icalrecurrencetype_skip icalrecur_string_to_skip(const char *str)
{
    int i;

    for (i = 0; skip_map[i].kind != ICAL_SKIP_UNDEFINED; i++) {
        if (strcasecmp(str, skip_map[i].str) == 0) {
            return skip_map[i].kind;
        }
    }
    return ICAL_SKIP_UNDEFINED;
}

const char *icalrecur_skip_to_string(icalrecurrencetype_skip kind)
{
    int i;

    for (i = 0; skip_map[i].kind != ICAL_SKIP_UNDEFINED; i++) {
        if (skip_map[i].kind == kind) {
            return skip_map[i].str;
        }
    }
    return 0;
}

static const struct wd_map
{
    icalrecurrencetype_weekday wd;
    const char str[3];
} wd_map[] = {
    {ICAL_SUNDAY_WEEKDAY, "SU"},
    {ICAL_MONDAY_WEEKDAY, "MO"},
    {ICAL_TUESDAY_WEEKDAY, "TU"},
    {ICAL_WEDNESDAY_WEEKDAY, "WE"},
    {ICAL_THURSDAY_WEEKDAY, "TH"},
    {ICAL_FRIDAY_WEEKDAY, "FR"},
    {ICAL_SATURDAY_WEEKDAY, "SA"},
    {ICAL_NO_WEEKDAY, ""}
};

const char *icalrecur_weekday_to_string(icalrecurrencetype_weekday kind)
{
    int i;

    for (i = 0; wd_map[i].wd != ICAL_NO_WEEKDAY; i++) {
        if (wd_map[i].wd == kind) {
            return wd_map[i].str;
        }
    }

    return 0;
}

icalrecurrencetype_weekday icalrecur_string_to_weekday(const char *str)
{
    int i;

    for (i = 0; wd_map[i].wd != ICAL_NO_WEEKDAY; i++) {
        if (strcasecmp(str, wd_map[i].str) == 0) {
            return wd_map[i].wd;
        }
    }

    return ICAL_NO_WEEKDAY;
}

/*********************** Rule parsing routines ************************/

struct icalrecur_parser
{
    const char *rule;
    char *copy;
    char *this_clause;
    char *next_clause;

    struct icalrecurrencetype rt;
};

enum byrule
{
    NO_CONTRACTION = -1,
    BY_MONTH       =  0,
    BY_WEEK_NO     =  1,
    BY_YEAR_DAY    =  2,
    BY_MONTH_DAY   =  3,
    BY_DAY         =  4,
    BY_HOUR        =  5,
    BY_MINUTE      =  6,
    BY_SECOND      =  7,
    BY_SET_POS     =  8
};

#define NUM_BY_PARTS  9

enum expand_table
{
    UNKNOWN  = 0,
    CONTRACT = 1,
    EXPAND   = 2,
    ILLEGAL  = 3
};

struct expand_split_map_struct
{
    icalrecurrencetype_frequency frequency;

    /* Elements of the 'map' array correspond to the BYxxx rules:
       Second,Minute,Hour,Day,Month Day,Year Day,Week No,Month,SetPos */

    short map[NUM_BY_PARTS];
};

/**
 * The split map indicates, for a particular interval, whether a BY_*
 * rule part expands the number of instances in the occurrence set or
 * contracts it. 1=> contract, 2=>expand, and 3 means the pairing is
 * not allowed.
 */

static const struct expand_split_map_struct expand_map[] = {
    /*                           M  W  YD MD D  h  m  s  P */
    {ICAL_SECONDLY_RECURRENCE, { 1, 3, 1, 1, 1, 1, 1, 1, 1 }},
    {ICAL_MINUTELY_RECURRENCE, { 1, 3, 1, 1, 1, 1, 1, 2, 1 }},
    {ICAL_HOURLY_RECURRENCE,   { 1, 3, 1, 1, 1, 1, 2, 2, 1 }},
    {ICAL_DAILY_RECURRENCE,    { 1, 3, 3, 1, 1, 2, 2, 2, 1 }},
    {ICAL_WEEKLY_RECURRENCE,   { 1, 3, 3, 3, 2, 2, 2, 2, 1 }},
    {ICAL_MONTHLY_RECURRENCE,  { 1, 3, 3, 2, 2, 2, 2, 2, 1 }},
    {ICAL_YEARLY_RECURRENCE,   { 2, 2, 2, 2, 2, 2, 2, 2, 1 }},
    {ICAL_NO_RECURRENCE,       { 0, 0, 0, 0, 0, 0, 0, 0, 0 }} //krazy:exclude=style
};

static const struct recur_map
{
    const char *str;
    size_t offset;
    int size;
    int min;
} recur_map[] = {
    { "BYMONTH",    offsetof(struct icalrecurrencetype, by_month),
      ICAL_BY_MONTH_SIZE,     1 },
    { "BYWEEKNO",   offsetof(struct icalrecurrencetype, by_week_no),
      ICAL_BY_WEEKNO_SIZE,   -1 },
    { "BYYEARDAY",  offsetof(struct icalrecurrencetype, by_year_day),
      ICAL_BY_YEARDAY_SIZE,  -1 },
    { "BYMONTHDAY", offsetof(struct icalrecurrencetype, by_month_day),
      ICAL_BY_MONTHDAY_SIZE, -1 },
    { "BYDAY",      offsetof(struct icalrecurrencetype, by_day),
      ICAL_BY_DAY_SIZE,       0 },
    { "BYHOUR",     offsetof(struct icalrecurrencetype, by_hour),
      ICAL_BY_HOUR_SIZE,      0 },
    { "BYMINUTE",   offsetof(struct icalrecurrencetype, by_minute),
      ICAL_BY_MINUTE_SIZE,    0 },
    { "BYSECOND",   offsetof(struct icalrecurrencetype, by_second),
      ICAL_BY_SECOND_SIZE,    0 },
    { "BYSETPOS",   offsetof(struct icalrecurrencetype, by_set_pos),
      ICAL_BY_SETPOS_SIZE,   -1 },
};

static const char *icalrecur_first_clause(struct icalrecur_parser *parser)
{
    char *idx;

    parser->this_clause = parser->copy;

    idx = strchr(parser->this_clause, ';');

    if (idx == 0) {
        parser->next_clause = 0;
        return 0;
    }

    *idx = 0;
    idx++;
    parser->next_clause = idx;

    return parser->this_clause;
}

static const char *icalrecur_next_clause(struct icalrecur_parser *parser)
{
    char *idx;

    parser->this_clause = parser->next_clause;

    if (parser->this_clause == 0) {
        return 0;
    }

    idx = strchr(parser->this_clause, ';');

    if (idx == 0) {
        parser->next_clause = 0;
    } else {

        *idx = 0;
        idx++;
        parser->next_clause = idx;
    }

    return parser->this_clause;
}

static void icalrecur_clause_name_and_value(struct icalrecur_parser *parser,
                                            char **name, char **value)
{
    char *idx;

    *name = parser->this_clause;

    idx = strchr(parser->this_clause, '=');

    if (idx == 0) {
        *name = 0;
        *value = 0;
        return;
    }

    *idx = 0;
    idx++;
    *value = idx;
}

/* returns < 0 if a parsing problem:
   -2 if an RSCALE rule is encountered yet we don't RSCALE support enabled
   -1 for all other parsing problems
*/
static int icalrecur_add_byrules(struct icalrecur_parser *parser, short *array,
                                 int min, int size, char *vals)
{
    char *t, *n;
    int i = 0;
    int v;
    int max = size - (min == 0);

    n = vals;

    while (n != 0) {

        if (i == size) {
            return -1;
        }

        t = n;

        n = strchr(t, ',');

        if (n != 0) {
            *n = 0;
            n++;
        }

        v = strtol(t, &t, 10);

        /* Sanity check value */
        if (v < 0) {
            if (min >= 0 || v <= -max) {
                return -1;
            }
        } else if (v > 0) {
            if (v >= max) {
                return -1;
            }
        } else if (min) {
            return -1;
        }

        if (*t) {
            /* Check for leap month suffix (RSCALE only) */
            if (array == parser->rt.by_month && strcmp(t, "L") == 0) {
                if (icalrecurrencetype_rscale_is_supported()) {
                    /* The "L" suffix in a BYMONTH recur-rule-part
                       is encoded by setting a high-order bit */
                    v |= LEAP_MONTH;
                } else {
                    return -2;
                }
            } else {
                return -1;
            }
        }

        array[i++] = (short)v;
        array[i] = ICAL_RECURRENCE_ARRAY_MAX;
    }

    return 0;
}

/*
 * Days in the BYDAY rule are expected by the code to be sorted, and while
 * this may be the common case, the RFC doesn't actually mandate it. This
 * function sorts the days taking into account the first day of week.
 */
static void sort_bydayrules(struct icalrecur_parser *parser)
{
    short *array;
    int week_start, one, two, i, j;

    array = parser->rt.by_day;
    week_start = parser->rt.week_start;

    for (i = 0;
         i < ICAL_BY_DAY_SIZE && array[i] != ICAL_RECURRENCE_ARRAY_MAX; i++) {
        for (j = 0; j < i; j++) {
            one = icalrecurrencetype_day_day_of_week(array[j]) - week_start;
            if (one < 0) {
                one += 7;
            }
            two = icalrecurrencetype_day_day_of_week(array[i]) - week_start;
            if (two < 0) {
                two += 7;
            }

            if (one > two) {
                short tmp = array[j];

                array[j] = array[i];
                array[i] = tmp;
            }
        }
    }
}

static int icalrecur_add_bydayrules(struct icalrecur_parser *parser,
                                    const char *vals)
{
    char *t, *n;
    int i = 0;
    short *array = parser->rt.by_day;
    char *vals_copy;

    vals_copy = icalmemory_strdup(vals);
    n = vals_copy;

    while (n != 0) {
        int sign = 1;
        signed char weekno;  /* note: Novell/Groupwise sends BYDAY=255SU,
                                so we fit in a signed char to get -1 SU for last Sun */
        icalrecurrencetype_weekday wd;

        if (i == ICAL_BY_DAY_SIZE) {
            icalmemory_free_buffer(vals_copy);
            return -1;
        }

        t = n;

        n = strchr(t, ',');

        if (n != 0) {
            *n = 0;
            n++;
        }

        /* Get Optional weekno */
        weekno = (signed char)strtol(t, &t, 10);
        if (weekno < 0) {
            weekno = -weekno;
            sign = -1;
        }

        /* Outlook/Exchange generate "BYDAY=MO, FR" and "BYDAY=2 TH".
         * Cope with that.
         */
        if (*t == ' ') {
            t++;
        }

        wd = icalrecur_string_to_weekday(t);

        /* Sanity check value */
        if (wd == ICAL_NO_WEEKDAY || weekno >= ICAL_BY_WEEKNO_SIZE) {
            icalmemory_free_buffer(vals_copy);
            return -1;
        }

        array[i++] = icalrecurrencetype_encode_day(wd, sign * weekno);
        array[i] = ICAL_RECURRENCE_ARRAY_MAX;
    }

    icalmemory_free_buffer(vals_copy);

    sort_bydayrules(parser);

    return 0;
}

struct icalrecurrencetype icalrecurrencetype_from_string(const char *str)
{
    struct icalrecur_parser parser;
    enum byrule byrule;

    memset(&parser, 0, sizeof(parser));
    icalrecurrencetype_clear(&parser.rt);

    icalerror_check_arg_re(str != 0, "str", parser.rt);

    /* Set up the parser struct */
    parser.rule = str;
    parser.copy = icalmemory_strdup(parser.rule);
    parser.this_clause = parser.copy;

    if (parser.copy == 0) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return parser.rt;
    }

    /* Loop through all of the clauses */
    for (icalrecur_first_clause(&parser);
         parser.this_clause != 0; icalrecur_next_clause(&parser)) {
        char *name, *value;
        int r = 0;

        icalrecur_clause_name_and_value(&parser, &name, &value);

        if (name == 0) {
            if (strlen(parser.this_clause) > 0) {
                r = -1;
            } else {
                /* Hit an empty name/value pair,
                   but we're also at the end of the string.
                   This was probably a trailing semicolon with no data
                   (e.g. "FREQ=WEEKLY;INTERVAL=1;BYDAY=MO;")
                */
                break;
            }
        } else if (strcasecmp(name, "FREQ") == 0) {
            if (parser.rt.freq != ICAL_NO_RECURRENCE) {
                /* Don't allow multiple FREQs */
                r = -1;
            } else {
                parser.rt.freq = icalrecur_string_to_freq(value);
                if (parser.rt.freq == ICAL_NO_RECURRENCE) {
                    r = -1;
                }
            }
        } else if (strcasecmp(name, "RSCALE") == 0) {
            if (parser.rt.rscale != NULL) {
                /* Don't allow multiple RSCALEs */
                r = -1;
            } else {
                parser.rt.rscale = icalmemory_strdup(value);
            }
        } else if (strcasecmp(name, "SKIP") == 0) {
            if (parser.rt.skip != ICAL_SKIP_OMIT) {
                /* Don't allow multiple SKIPs */
                r = -1;
            } else {
                parser.rt.skip = icalrecur_string_to_skip(value);
                if (parser.rt.skip == ICAL_SKIP_UNDEFINED) {
                    r = -1;
                }
            }
        } else if (strcasecmp(name, "COUNT") == 0) {
            if (parser.rt.count > 0 || !icaltime_is_null_time(parser.rt.until)) {
                /* Don't allow multiple COUNTs, or both COUNT and UNTIL */
                r = -1;
            } else {
                parser.rt.count = atoi(value);
                /* don't allow count to be less than 1 */
                if (parser.rt.count < 1) r = -1;
            }
        } else if (strcasecmp(name, "UNTIL") == 0) {
            if (parser.rt.count > 0 || !icaltime_is_null_time(parser.rt.until)) {
                /* Don't allow multiple COUNTs, or both COUNT and UNTIL */
                r = -1;
            } else {
                parser.rt.until = icaltime_from_string(value);
                if (icaltime_is_null_time(parser.rt.until)) r = -1;
            }
        } else if (strcasecmp(name, "INTERVAL") == 0) {
            if (parser.rt.interval > 1) {
                /* Don't allow multiple INTERVALs */
                r = -1;
            } else {
                parser.rt.interval = (short)atoi(value);
                /* don't allow an interval to be less than 1
                   (RFC specifies an interval must be a positive integer) */
                if (parser.rt.interval < 1) r = -1;
            }
        } else if (strcasecmp(name, "WKST") == 0) {
            if (parser.rt.week_start != ICAL_MONDAY_WEEKDAY) {
                /* Don't allow multiple WKSTs */
                r = -1;
            } else {
                parser.rt.week_start = icalrecur_string_to_weekday(value);
                if (parser.rt.week_start == ICAL_NO_WEEKDAY) {
                    r = -1;
                } else {
                    sort_bydayrules(&parser);
                }
            }
        } else if (strncasecmp(name, "BY", 2) == 0) {
            r = -1;

            for (byrule = 0; byrule < NUM_BY_PARTS; byrule++) {
                if (strcasecmp(name+2, recur_map[byrule].str+2) == 0) {
                    if (byrule == BY_DAY) {
                        r = icalrecur_add_bydayrules(&parser, value);
                    } else {
                        short *array = (short *)(recur_map[byrule].offset + (size_t) &parser.rt);
                        r = icalrecur_add_byrules(&parser, array,
                                                  recur_map[byrule].min,
                                                  recur_map[byrule].size,
                                                  value);
                    }
                    break;
                }
            }
        } else {
            r = -1;
        }

        if (r) {
            /* Note: silently ignore when we have a leap month, yet don't have RSCALE support.
               The magic value "-2" indicates when that happens.
            */
            if (r != -2) {
                icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
            }
            if (parser.rt.rscale) {
                icalmemory_free_buffer(parser.rt.rscale);
            }
            icalrecurrencetype_clear(&parser.rt);
            break;
        }
    }

    for (byrule = 0; byrule < NUM_BY_PARTS; byrule++) {
        short *array = (short *)(recur_map[byrule].offset + (size_t) &parser.rt);

        if (array[0] != ICAL_RECURRENCE_ARRAY_MAX &&
            expand_map[parser.rt.freq].map[byrule] == ILLEGAL) {
            ical_invalid_rrule_handling rruleHandlingSetting =
                ical_get_invalid_rrule_handling_setting();

            if (rruleHandlingSetting == ICAL_RRULE_TREAT_AS_ERROR) {
                icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
                if (parser.rt.rscale) {
                    icalmemory_free_buffer(parser.rt.rscale);
                }
                icalrecurrencetype_clear(&parser.rt);
                break;
            } else {
                array[0] = ICAL_RECURRENCE_ARRAY_MAX;
            }
        }
    }

    icalmemory_free_buffer(parser.copy);

    return parser.rt;
}

char *icalrecurrencetype_as_string(struct icalrecurrencetype *recur)
{
    char *buf;

    buf = icalrecurrencetype_as_string_r(recur);
    icalmemory_add_tmp_buffer(buf);
    return buf;
}

char *icalrecurrencetype_as_string_r(struct icalrecurrencetype *recur)
{
    char *str;
    char *str_p;
    size_t buf_sz = 200;
    char temp[20];
    int i, j;

    if (recur == 0 || recur->freq == ICAL_NO_RECURRENCE) {
        return 0;
    }

    str = (char *)icalmemory_new_buffer(buf_sz);
    str_p = str;

    if (recur->rscale != 0) {
        icalmemory_append_string(&str, &str_p, &buf_sz, "RSCALE=");
        icalmemory_append_string(&str, &str_p, &buf_sz, recur->rscale);

        /* Omit is the default, so no need to write that out */
        if (recur->skip != ICAL_SKIP_OMIT) {
            const char *skipstr = icalrecur_skip_to_string(recur->skip);
            icalmemory_append_string(&str, &str_p, &buf_sz, ";SKIP=");
            icalmemory_append_string(&str, &str_p, &buf_sz, skipstr);
        }
        icalmemory_append_char(&str, &str_p, &buf_sz, ';');
    }

    icalmemory_append_string(&str, &str_p, &buf_sz, "FREQ=");
    icalmemory_append_string(&str, &str_p, &buf_sz,
                             icalrecur_freq_to_string(recur->freq));

    /* 1 is the default, so no need to write that out */
    if (recur->interval != 1) {
        snprintf(temp, sizeof(temp), "%d", recur->interval);
        icalmemory_append_string(&str, &str_p, &buf_sz, ";INTERVAL=");
        icalmemory_append_string(&str, &str_p, &buf_sz, temp);
    }

    /* Monday is the default, so no need to write that out */
    if (recur->week_start != ICAL_MONDAY_WEEKDAY &&
        recur->week_start != ICAL_NO_WEEKDAY) {
        int dow = icalrecurrencetype_day_day_of_week(recur->week_start);
        const char *daystr = icalrecur_weekday_to_string(dow);
        icalmemory_append_string(&str, &str_p, &buf_sz, ";WKST=");
        icalmemory_append_string(&str, &str_p, &buf_sz, daystr);
    }

    for (j = 0; j < NUM_BY_PARTS; j++) {
        short *array = (short *)(recur_map[j].offset + (size_t) recur);
        int limit = recur_map[j].size - 1;

        /* Skip unused arrays */
        if (array[0] != ICAL_RECURRENCE_ARRAY_MAX) {

            icalmemory_append_char(&str, &str_p, &buf_sz, ';');
            icalmemory_append_string(&str, &str_p, &buf_sz, recur_map[j].str);
            icalmemory_append_char(&str, &str_p, &buf_sz, '=');

            for (i = 0; i < limit && array[i] != ICAL_RECURRENCE_ARRAY_MAX; i++) {
                if (j == BY_DAY) {
                    int pos = icalrecurrencetype_day_position(array[i]);
                    int dow = icalrecurrencetype_day_day_of_week(array[i]);
                    const char *daystr = icalrecur_weekday_to_string(dow);

                    if (pos == 0) {
                        icalmemory_append_string(&str, &str_p, &buf_sz, daystr);
                    } else {
                        snprintf(temp, sizeof(temp), "%d%s", pos, daystr);
                        icalmemory_append_string(&str, &str_p, &buf_sz, temp);
                    }

                } else if (j == BY_MONTH &&
                           icalrecurrencetype_month_is_leap(array[i])) {
                    snprintf(temp, sizeof(temp), "%dL",
                             icalrecurrencetype_month_month(array[i]));
                    icalmemory_append_string(&str, &str_p, &buf_sz, temp);
                } else {
                    snprintf(temp, sizeof(temp), "%d", array[i]);
                    icalmemory_append_string(&str, &str_p, &buf_sz, temp);
                }

                if ((i + 1) < limit &&
                    array[i + 1] != ICAL_RECURRENCE_ARRAY_MAX) {
                    icalmemory_append_char(&str, &str_p, &buf_sz, ',');
                }
            }
        }
    }

    if (recur->until.year != 0) {
        temp[0] = 0;
        if (recur->until.is_date) {
            print_date_to_string(temp, &(recur->until));
        } else {
            print_datetime_to_string(temp, &(recur->until));
        }

        icalmemory_append_string(&str, &str_p, &buf_sz, ";UNTIL=");
        icalmemory_append_string(&str, &str_p, &buf_sz, temp);
    }

    else if (recur->count != 0) {
        snprintf(temp, sizeof(temp), "%d", recur->count);
        icalmemory_append_string(&str, &str_p, &buf_sz, ";COUNT=");
        icalmemory_append_string(&str, &str_p, &buf_sz, temp);
    }

    return str;
}

/************************* occurrence iteration routines ******************/

/* Number of bits in an unsigned long */
#define BITS_PER_LONG      (8 * sizeof(unsigned long))

/* Number of longs in mask of n bits */
#define LONGS_PER_BITS(n)  ((n + BITS_PER_LONG -1 ) / BITS_PER_LONG)

#define ICAL_YEARDAYS_MASK_SIZE    (ICAL_BY_YEARDAY_SIZE + 7)
#define ICAL_YEARDAYS_MASK_OFFSET  4

struct icalrecur_iterator_impl
{
    struct icaltimetype dtstart;     /* copy of DTSTART: to fill in defaults */
    struct icalrecurrencetype rule;  /* copy of RRULE */

    struct icaltimetype rstart;      /* DTSTART in RSCALE  */
    struct icaltimetype istart;      /* Gregorian start time for iterator */
    struct icaltimetype iend;        /* Gregorian end time for iterator */
    struct icaltimetype last;        /* last time returned from iterator */
    int occurrence_no;               /* number of steps made on the iterator */

#if defined(HAVE_LIBICU)
    UCalendar *greg;                 /* Gregorian calendar */
    UCalendar *rscale;               /* RSCALE calendar    */
#endif

    struct icaltimetype period_start;  /* Start date of monthly/yearly period */

    /* days[] is a bitmask of year days.  A bit value of 1 marks an occurrence.
       The size of the bitmask is 7 + max days in year to accommodate full first
       and last weeks of the year: up to 3 days in previous year and
       up to 4 days in following year.  As a result, the days are offset by 4:
       bit 0 is day -3 (3rd last day of previous year) and bit 4 is day 1
       of the current year.  Days in the following year use higher day numbers,
       e.g. day 367 is day 1 or 2 of following year depending on whether the
       current year is a leap year.

       days_index is the day of year of the next occurrence,
       with a range of -3 to 4 + days in year.
    */
    unsigned long days[LONGS_PER_BITS(ICAL_YEARDAYS_MASK_SIZE)];
    short days_index;

    enum byrule byrule;
    short by_indices[NUM_BY_PARTS];
    short orig_data[NUM_BY_PARTS]; /**< 1 if there was data in the byrule */

    short *by_ptrs[NUM_BY_PARTS];  /**< Pointers into the by_* array elements of the rule */

};

static void daysmask_clearall(unsigned long mask[])
{
    memset(mask, 0,
           sizeof(unsigned long) * LONGS_PER_BITS(ICAL_YEARDAYS_MASK_SIZE));
}

static void daysmask_setbit(unsigned long mask[], short n, int v)
{
    n += ICAL_YEARDAYS_MASK_OFFSET;
    if (v) {
        mask[n / BITS_PER_LONG] |= (1UL << (n % BITS_PER_LONG));
    } else {
        mask[n / BITS_PER_LONG] &= ~(1UL << (n % BITS_PER_LONG));
    }
}

static unsigned long daysmask_getbit(unsigned long mask[], short n)
{
    n += ICAL_YEARDAYS_MASK_OFFSET;
    return (mask[n / BITS_PER_LONG] >> (n % BITS_PER_LONG)) & 1;
}

int icalrecur_iterator_sizeof_byarray(short *byarray)
{
    int array_itr;

    for (array_itr = 0;
         byarray[array_itr] != ICAL_RECURRENCE_ARRAY_MAX; array_itr++) {
    }

    return array_itr;
}

static int has_by_data(icalrecur_iterator *impl, enum byrule byrule)
{
    return (impl->orig_data[byrule] == 1);
}

static void setup_defaults(icalrecur_iterator *impl,
                           enum byrule byrule, int deftime)
{
    icalrecurrencetype_frequency freq = impl->rule.freq;

    if (expand_map[freq].map[byrule] == EXPAND) {

        /* Re-write the BY rule arrays with data from the DTSTART time so
           we don't have to explicitly deal with DTSTART */
        if (impl->by_ptrs[byrule][0] == ICAL_RECURRENCE_ARRAY_MAX) {
            impl->by_ptrs[byrule][0] = (short)deftime;
        }
    }
}

/** Calculate ISO weeks per year
   https://en.wikipedia.org/wiki/ISO_week_date#Weeks_per_year */
static int weeks_in_year(int year)
{
    /* Long years occur when year starts on Thu or leap year starts on Wed */
    int dow = icaltime_day_of_week(icaltime_from_day_of_year(1, year));
    int is_long = (dow == 5 || (dow == 4 && icaltime_is_leap_year(year)));

    return (52 + is_long);
}

/** Calculate the number of Gregorian months between 2 dates */
static int __greg_month_diff(icaltimetype a, icaltimetype b)
{
    return (12 * (b.year - a.year) + (b.month - a.month));
}

static void __get_start_time(icalrecur_iterator *impl, icaltimetype date,
                             int *hour, int *minute, int *second)
{
    icalrecurrencetype_frequency freq = impl->rule.freq;

    if (freq == ICAL_HOURLY_RECURRENCE) {
        *hour = date.hour;
    } else if (has_by_data(impl, BY_HOUR)) {
        *hour = impl->by_ptrs[BY_HOUR][0];
    } else {
        *hour = impl->rstart.hour;
    }

    if (freq == ICAL_MINUTELY_RECURRENCE) {
        *minute = date.minute;
    } else if (has_by_data(impl, BY_MINUTE)) {
        *minute = impl->by_ptrs[BY_MINUTE][0];
    } else {
        *minute = impl->rstart.minute;
    }

    if (freq == ICAL_SECONDLY_RECURRENCE) {
        *second = date.second;
    } else if (has_by_data(impl, BY_SECOND)) {
        *second = impl->by_ptrs[BY_SECOND][0];
    } else {
        *second = impl->rstart.second;
    }
}

static int __day_diff(icalrecur_iterator *impl, icaltimetype a, icaltimetype b);

#if defined(HAVE_LIBICU)
/*
 * Callbacks for recurrence rules with RSCALE support (using ICU)
 *
 * References:
 *   - tools.ietf.org/html/rfc7529
 *   - en.wikipedia.org/wiki/Intercalation_%28timekeeping%29
 *   - icu-project.org/apiref/icu4c/ucal_8h.html
 *   - cldr.unicode.org/development/development-process/design-proposals/chinese-calendar-support
 *   - cldr.unicode.org/development/development-process/design-proposals/islamic-calendar-types
 *
 * ICU Notes:
 *   - Months are 0-based
 *   - Leap months in Chinese and Hebrew calendars are handled differently
 */

icalarray *icalrecurrencetype_rscale_supported_calendars(void)
{
    UErrorCode status = U_ZERO_ERROR;
    UEnumeration *en;
    icalarray *calendars;
    const char *cal;

    calendars = icalarray_new(sizeof(const char **), 20);

    en = ucal_getKeywordValuesForLocale("calendar", NULL, false, &status);
    while ((cal = uenum_next(en, NULL, &status))) {
        cal = icalmemory_tmp_copy(cal);
        icalarray_append(calendars, &cal);
    }
    uenum_close(en);

    return calendars;
}

static void set_second(icalrecur_iterator *impl, int second)
{
    ucal_set(impl->rscale, UCAL_SECOND, (int32_t) second);
}

static void set_minute(icalrecur_iterator *impl, int minute)
{
    ucal_set(impl->rscale, UCAL_MINUTE, (int32_t) minute);
}

static void set_hour(icalrecur_iterator *impl, int hour)
{
    ucal_set(impl->rscale, UCAL_HOUR_OF_DAY, (int32_t) hour);
}

static void __set_month(icalrecur_iterator *impl, int month)
{
    int is_leap_month = icalrecurrencetype_month_is_leap(month);

    month = icalrecurrencetype_month_month(month) - 1;  /* UCal is 0-based */

    ucal_set(impl->rscale, UCAL_MONTH, (int32_t) month);
    if (is_leap_month)
        ucal_set(impl->rscale, UCAL_IS_LEAP_MONTH, 1);
}

static int set_month(icalrecur_iterator *impl, int month)
{
    UErrorCode status = U_ZERO_ERROR;
    int actual_month;

    __set_month(impl, month);

    ucal_set(impl->rscale, UCAL_DAY_OF_MONTH, (int32_t) 1);

    actual_month = 1 +  /* UCal is 0-based */
        (int)ucal_get(impl->rscale, UCAL_MONTH, &status);

    if (ucal_get(impl->rscale, UCAL_IS_LEAP_MONTH, &status)) {
        actual_month |= LEAP_MONTH;
    }

    if (actual_month != month) {
        switch (impl->rule.skip) {
        default:
            /* Should never get here! */

        case ICAL_SKIP_OMIT:
            /* Invalid month */
            return 0;

        case ICAL_SKIP_BACKWARD:
            /* Skip back to next valid month */
            ucal_add(impl->rscale, UCAL_MONTH, (int32_t) -1, &status);
            break;

        case ICAL_SKIP_FORWARD:
            /* UCal skips forward to valid month by default */
            break;
        }
    }

    return (1 +  /* UCal is 0-based */
            (int)ucal_get(impl->rscale, UCAL_MONTH, &status));
}

static int get_months_in_year(icalrecur_iterator *impl, int year)
{
    UErrorCode status = U_ZERO_ERROR;

    if (year) {
        ucal_set(impl->rscale, UCAL_YEAR, (int32_t) year);
    }

    return (1 +  /* UCal is 0-based */
            (int)ucal_getLimit(impl->rscale, UCAL_MONTH,
                               UCAL_ACTUAL_MAXIMUM, &status));
}

static int get_days_in_year(icalrecur_iterator *impl, int year)
{
    UErrorCode status = U_ZERO_ERROR;

    if (year) {
        ucal_set(impl->rscale, UCAL_YEAR, (int32_t) year);
    }

    return (int)ucal_getLimit(impl->rscale, UCAL_DAY_OF_YEAR,
                              UCAL_ACTUAL_MAXIMUM, &status);
}

static void set_day_of_year(icalrecur_iterator *impl, int doy)
{
    if (doy < 1) {
        doy += get_days_in_year(impl, 0);
    }

    ucal_set(impl->rscale, UCAL_DAY_OF_YEAR, (int32_t) doy);
}

static int get_start_of_week(icalrecur_iterator *impl)
{
    UErrorCode status = U_ZERO_ERROR;
    int doy, dow;

    doy = (int)ucal_get(impl->rscale, UCAL_DAY_OF_YEAR, &status);
    dow = (int)ucal_get(impl->rscale, UCAL_DAY_OF_WEEK, &status);
    dow -= impl->rule.week_start;
    if (dow < 0) {
        dow += 7;
    }

    return (doy - dow);
}

static int get_day_of_week(icalrecur_iterator *impl)
{
    UErrorCode status = U_ZERO_ERROR;

    return (int)ucal_get(impl->rscale, UCAL_DAY_OF_WEEK, &status);
}

static int get_week_number(icalrecur_iterator *impl, struct icaltimetype tt)
{
    UErrorCode status = U_ZERO_ERROR;
    UDate last_millis;
    int month, weekno;

    /* Save existing rscale date */
    last_millis = ucal_getMillis(impl->rscale, &status);

    month = icalrecurrencetype_month_month(tt.month) - 1;  /* UCal is 0-based */
    ucal_setDate(impl->rscale,
                 (int32_t) tt.year, (int32_t) month, (int32_t) tt.day, &status);
    if (icalrecurrencetype_month_is_leap(tt.month)) {
        ucal_set(impl->rscale, UCAL_IS_LEAP_MONTH, 1);
    }

    weekno = (int)ucal_get(impl->rscale, UCAL_WEEK_OF_YEAR, &status);

    /* Restore saved rscale date */
    ucal_setMillis(impl->rscale, last_millis, &status);

    return weekno;
}

static int get_days_in_month(icalrecur_iterator *impl, int month, int year)
{
    UErrorCode status = U_ZERO_ERROR;

    ucal_set(impl->rscale, UCAL_YEAR, (int32_t) year);

    if (!month) {
        month = impl->rstart.month;
    }
    __set_month(impl, month);

    return (int)ucal_getLimit(impl->rscale,
                              UCAL_DAY_OF_MONTH, UCAL_ACTUAL_MAXIMUM, &status);
}

static int get_day_of_year(icalrecur_iterator *impl,
                           int year, int month, int day, int *dow)
{
    UErrorCode status = U_ZERO_ERROR;

    ucal_set(impl->rscale, UCAL_YEAR, (int32_t) year);

    if (!month) {
        month = impl->rstart.month;
    }
    __set_month(impl, month);

    if (!day) {
        day = impl->rstart.day;
    } else if (day < 0) {
        day += 1 + (int)ucal_getLimit(impl->rscale, UCAL_DAY_OF_MONTH,
                                      UCAL_ACTUAL_MAXIMUM, &status);
    }
    ucal_set(impl->rscale, UCAL_DAY_OF_MONTH, (int32_t) day);

    if (dow) {
        *dow = (int)ucal_get(impl->rscale, UCAL_DAY_OF_WEEK, &status);
    }

    return (int)ucal_get(impl->rscale, UCAL_DAY_OF_YEAR, &status);
}

static struct icaltimetype occurrence_as_icaltime(icalrecur_iterator *impl,
                                                  int normalize)
{
    struct icaltimetype tt = impl->dtstart;
    UErrorCode status = U_ZERO_ERROR;
    UCalendar *cal = impl->rscale;
    int is_leap_month = 0;

    if (normalize && (impl->rscale != impl->greg)) {
        /* Convert to Gregorian date */
        UDate millis = ucal_getMillis(impl->rscale, &status);

        ucal_setMillis(impl->greg, millis, &status);
        cal = impl->greg;
    } else {
        is_leap_month =
            (int)ucal_get(impl->rscale, UCAL_IS_LEAP_MONTH, &status);
    }

    tt.year = (int)ucal_get(cal, UCAL_YEAR, &status);
    tt.day = (int)ucal_get(cal, UCAL_DATE, &status);
    tt.month = 1 +  /* UCal is 0-based */
        (int)ucal_get(cal, UCAL_MONTH, &status);
    if (is_leap_month) {
        tt.month |= LEAP_MONTH;
    }

    if (!tt.is_date) {
        tt.hour = (int)ucal_get(cal, UCAL_HOUR_OF_DAY, &status);
        tt.minute = (int)ucal_get(cal, UCAL_MINUTE, &status);
        tt.second = (int)ucal_get(cal, UCAL_SECOND, &status);
    }

    return tt;
}

struct icaltimetype __icaltime_from_day_of_year(icalrecur_iterator *impl,
                                                int day, int year, int *weekno)
{
    ucal_set(impl->rscale, UCAL_YEAR, (int32_t) year);
    if (day < 0) {
        day += get_days_in_year(impl, 0) + 1;
    }

    ucal_set(impl->rscale, UCAL_DAY_OF_YEAR, (int32_t) day);

    if (weekno) {
        UErrorCode status = U_ZERO_ERROR;

        *weekno = (int)ucal_get(impl->rscale, UCAL_WEEK_OF_YEAR, &status);
    }

    return occurrence_as_icaltime(impl, 0);
}

static void increment_year(icalrecur_iterator *impl, int inc)
{
    UErrorCode status = U_ZERO_ERROR;

    ucal_add(impl->rscale, UCAL_YEAR, (int32_t) inc, &status);
}

static void __increment_month(icalrecur_iterator *impl, int inc)
{
    UErrorCode status = U_ZERO_ERROR;

    ucal_add(impl->rscale, UCAL_MONTH, (int32_t) inc, &status);
}

static void increment_monthday(icalrecur_iterator *impl, int inc)
{
    UErrorCode status = U_ZERO_ERROR;

    ucal_add(impl->rscale, UCAL_DAY_OF_MONTH, (int32_t) inc, &status);
}

static void increment_hour(icalrecur_iterator *impl, int inc)
{
    UErrorCode status = U_ZERO_ERROR;

    ucal_add(impl->rscale, UCAL_HOUR_OF_DAY, (int32_t) inc, &status);
}

static void increment_minute(icalrecur_iterator *impl, int inc)
{
    UErrorCode status = U_ZERO_ERROR;

    ucal_add(impl->rscale, UCAL_MINUTE, (int32_t) inc, &status);
}

static void increment_second(icalrecur_iterator *impl, int inc)
{
    UErrorCode status = U_ZERO_ERROR;

    ucal_add(impl->rscale, UCAL_SECOND, (int32_t) inc, &status);
}

static int validate_byrule(icalrecur_iterator *impl,
                           enum byrule byrule, UCalendarDateFields field,
                           short (*decode_val)(short *, unsigned),
                           unsigned int decode_flags)
{
    if (has_by_data(impl, byrule)) {
        UErrorCode status = U_ZERO_ERROR;
        short *by_ptr = impl->by_ptrs[byrule];
        short max =
            (short)ucal_getLimit(impl->rscale, field, UCAL_MAXIMUM, &status);
        short idx;

        for (idx = 0; by_ptr[idx] != ICAL_RECURRENCE_ARRAY_MAX; idx++) {
            short val = decode_val ?
                decode_val(&by_ptr[idx], decode_flags) : by_ptr[idx];

            if (abs(val) > max) return 0;
        }
    }

    return 1;
}

static short decode_month(short *month, unsigned int is_hebrew)
{
    if (is_hebrew && *month > 5) {  /* 5L == 0x1005 */
        /* Hebrew calendar:
           Translate RSCALE months to ICU (numbered 1-13, where 6 is leap).
           Hence, 5L maps to 6 and 6-12 map to 7-13. */
        *month = icalrecurrencetype_month_month(*month) + 1;
    }

    return icalrecurrencetype_month_month(*month) - 1;  /* UCal is 0-based */
}

static short decode_day(short *day, unsigned int flags)
{
    _unused(flags);

    return icalrecurrencetype_day_position(*day);
}

static int initialize_rscale(icalrecur_iterator *impl)
{
    struct icalrecurrencetype rule = impl->rule;
    struct icaltimetype dtstart = impl->dtstart;
    char locale[ULOC_KEYWORD_AND_VALUES_CAPACITY] = "";
    UErrorCode status = U_ZERO_ERROR;
    UChar *tzid = (UChar *) UCAL_UNKNOWN_ZONE_ID;
    short is_hebrew = 0;

    /* Convert the UTF8 timezoneid of dstart to ICU UChar. */
    const char *src = icaltimezone_get_location((icaltimezone *) dtstart.zone);
    if (!src) {
        const char *prefix = icaltimezone_tzid_prefix();

        src = icaltimezone_get_tzid((icaltimezone *) dtstart.zone);
        if (src && !strncmp(src, prefix, strlen(prefix))) {
            /* Skip past our prefix */
            src += strlen(prefix);
        }
    }
    if (src) {
        size_t len = (strlen(src) + 1) * U_SIZEOF_UCHAR;
        tzid = icalmemory_tmp_buffer(len);
        tzid = u_strFromUTF8Lenient(tzid, (int32_t)len, NULL, src, -1, &status);
        if (U_FAILURE(status)) {
            icalerror_set_errno(ICAL_INTERNAL_ERROR);
            return 0;
        }
    }

    /* Create locale for Gregorian calendar */
    (void)uloc_setKeywordValue("calendar", "gregorian",
                               locale, sizeof(locale), &status);

    /* Create Gregorian calendar and set to DTSTART */
    impl->greg = ucal_open(tzid, -1, locale, UCAL_DEFAULT, &status);
    if (impl->greg) {
        ucal_setDateTime(impl->greg,
                         (int32_t) dtstart.year,
                         (int32_t) (dtstart.month - 1), /* UCal is 0-based */
                         (int32_t) dtstart.day,
                         (int32_t) dtstart.hour,
                         (int32_t) dtstart.minute,
                         (int32_t) dtstart.second, &status);
    }
    if (!impl->greg || U_FAILURE(status)) {
        icalerror_set_errno(ICAL_INTERNAL_ERROR);
        return 0;
    }

    if (!rule.rscale) {
        /* Use Gregorian as RSCALE */
        impl->rscale = impl->greg;
    } else {
        UEnumeration *en;
        const char *cal;
        char *r;

        /* Lowercase the specified calendar */
        for (r = rule.rscale; *r; r++) {
            *r = tolower((int)*r);
        }

        /* Check if specified calendar is supported */
        en = ucal_getKeywordValuesForLocale("calendar", NULL, false, &status);
        while ((cal = uenum_next(en, NULL, &status))) {
            if (!strcmp(cal, rule.rscale)) {
                is_hebrew = !strcmp(rule.rscale, "hebrew");
                break;
            }
        }
        uenum_close(en);
        if (!cal) {
            icalerror_set_errno(ICAL_UNIMPLEMENTED_ERROR);
            return 0;
        }

        /* Create locale for RSCALE calendar */
        (void)uloc_setKeywordValue("calendar", rule.rscale,
                                   locale, sizeof(locale), &status);

        /* Create RSCALE calendar and set to DTSTART */
        impl->rscale = ucal_open(tzid, -1, locale, UCAL_DEFAULT, &status);
        if (impl->rscale) {
            UDate millis = ucal_getMillis(impl->greg, &status);

            ucal_setMillis(impl->rscale, millis, &status);
        }
        if (!impl->rscale || U_FAILURE(status)) {
            icalerror_set_errno(ICAL_INTERNAL_ERROR);
            return 0;
        }
    }

    /* Validate BY_* array values whose legal maximums differ based on RSCALE */
    if (!validate_byrule(impl, BY_MONTH, UCAL_MONTH,
                         &decode_month, (unsigned int)is_hebrew) ||
        !validate_byrule(impl, BY_DAY, UCAL_WEEK_OF_YEAR, &decode_day, 0) ||
        !validate_byrule(impl, BY_MONTH_DAY, UCAL_DAY_OF_MONTH, NULL, 0) ||
        !validate_byrule(impl, BY_YEAR_DAY, UCAL_DAY_OF_YEAR, NULL, 0) ||
        !validate_byrule(impl, BY_WEEK_NO, UCAL_WEEK_OF_YEAR, NULL, 0) ||
        !validate_byrule(impl, BY_SET_POS, UCAL_DAY_OF_YEAR, NULL, 0)) {
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        return 0;
    }

    /* Set iCalendar defaults */
    ucal_setAttribute(impl->rscale, UCAL_MINIMAL_DAYS_IN_FIRST_WEEK, 4);
    ucal_setAttribute(impl->rscale, UCAL_FIRST_DAY_OF_WEEK, rule.week_start);

    /* Get rstart (DTSTART in RSCALE) */
    impl->rstart = occurrence_as_icaltime(impl, 0);

    return 1;
}

/** Sets the Gregorian date and convert to RSCALE */
static void set_datetime(icalrecur_iterator *impl, icaltimetype date)
{
    UErrorCode status = U_ZERO_ERROR;

    impl->last.is_date = impl->rstart.is_date;
    impl->last.zone = impl->rstart.zone;

    if (impl->rstart.is_date) {
        ucal_setDate(impl->greg,
                     (int32_t) date.year,
                     (int32_t) (date.month - 1), /* UCal is 0-based */
                     (int32_t) date.day, &status);
    } else {
        int hour, minute, second;

        __get_start_time(impl, date, &hour, &minute, &second);

        ucal_setDateTime(impl->greg,
                         (int32_t) date.year,
                         (int32_t) (date.month - 1), /* UCal is 0-based */
                         (int32_t) date.day,
                         (int32_t) hour,
                         (int32_t) minute,
                         (int32_t) second,
                         &status);
    }

    if (impl->rscale != impl->greg) {
        UDate millis = ucal_getMillis(impl->greg, &status);
        ucal_setMillis(impl->rscale, millis, &status);
    }
}

/** Calculate the number of RSCALE months between 2 dates */
static int month_diff(icalrecur_iterator *impl, icaltimetype a, icaltimetype b)
{
    int diff;

    if (impl->rscale == impl->greg) {
        /* Use simple Gregorian math */
        diff = __greg_month_diff(a, b);
    } else if (a.year == b.year) {
        diff = b.month - a.month;
    } else {
        /* Count months in each year to account for leap months */
        UErrorCode status = U_ZERO_ERROR;
        UDate millis;
        int year = a.year;

        /* Save current date */
        millis = ucal_getMillis(impl->rscale, &status);

        set_day_of_year(impl, 1);
        diff = get_months_in_year(impl, year) - a.month;
        while (++year < b.year) diff += get_months_in_year(impl, year);
        diff += b.month;

        /* Restore date */
        ucal_setMillis(impl->rscale, millis, &status);
    }

    return diff;
}

/** Calculate the number of RSCALE days between 2 dates */
static int day_diff(icalrecur_iterator *impl, icaltimetype a, icaltimetype b)
{
    UErrorCode status = U_ZERO_ERROR;
    UDate millis;
    int diff;

    /* Save current date */
    millis = ucal_getMillis(impl->rscale, &status);

    set_day_of_year(impl, 1);

    diff = __day_diff(impl, a, b);

    /* Restore date */
    ucal_setMillis(impl->rscale, millis, &status);

    return diff;
}

static void reset_period_start(icalrecur_iterator *impl)
{
    struct icaltimetype start = impl->period_start;

    (void)get_day_of_year(impl, start.year, start.month, start.day, NULL);
}

#else /* !HAVE_LIBICU */

/*
 * Callbacks for recurrence rules without RSCALE (Gregorian only)
 */

icalarray *icalrecurrencetype_rscale_supported_calendars(void)
{
    icalarray *calendars = icalarray_new(sizeof(const char **), 1);
    const char *cal = "GREGORIAN";

    icalarray_append(calendars, &cal);

    return calendars;
}

static void set_second(icalrecur_iterator *impl, int second)
{
    impl->last.second = second;
}

static void set_minute(icalrecur_iterator *impl, int minute)
{
    impl->last.minute = minute;
}

static void set_hour(icalrecur_iterator *impl, int hour)
{
    impl->last.hour = hour;
}

static int set_month(icalrecur_iterator *impl, int month)
{
    return (impl->last.month = month);
}

#define get_months_in_year(impl, year)  (12)

static int get_days_in_year(icalrecur_iterator *impl, int year)
{
    _unused(impl);

    return icaltime_days_in_year(year);
}

static void set_day_of_year(icalrecur_iterator *impl, int doy)
{
    struct icaltimetype next;

    if (doy < 1) {
        doy += get_days_in_year(impl, impl->last.year);
    }

    next = icaltime_from_day_of_year(doy, impl->last.year);

    impl->last.day = next.day;
    impl->last.month = next.month;
    impl->last.year = next.year;
}

static int get_start_of_week(icalrecur_iterator *impl)
{
    return icaltime_start_doy_week(impl->last, impl->rule.week_start);
}

static int get_day_of_week(icalrecur_iterator *impl)
{
    return icaltime_day_of_week(impl->last);
}

/** Calculate ISO week number
   https://en.wikipedia.org/wiki/ISO_week_date#Calculation */
static int get_week_number(icalrecur_iterator *impl, struct icaltimetype tt)
{
    int dow, week;

    _unused(impl);

    /* Normalize day of week so that week_start day is 1 */
    dow = icaltime_day_of_week(tt) - (impl->rule.week_start - 1);
    if (dow <= 0)
        dow += 7;

    week = (icaltime_day_of_year(tt) - dow + 10) / 7;
    if (week < 1) {
        /* Last week of preceding year */
        week = weeks_in_year(tt.year - 1);
    } else if (week > weeks_in_year(tt.year)) {
        /* First week of following year */
        week = 1;
    }

    return week;
}

static int get_days_in_month(icalrecur_iterator *impl, int month, int year)
{
    _unused(impl);

    return icaltime_days_in_month(month, year);
}

static int get_day_of_year(icalrecur_iterator *impl,
                           int year, int month, int day, int *dow)
{
    struct icaltimetype t = impl->dtstart;

    t.is_date = 1;
    t.year = year;

    if (!month) {
        month = impl->dtstart.month;
    }
    t.month = month;

    if (!day) {
        day = impl->dtstart.day;
    }
    else if (day < 0) {
        day += icaltime_days_in_month(month, year) + 1;
    }
    t.day = day;

    if (dow) {
        *dow = icaltime_day_of_week(t);
    }

    return icaltime_day_of_year(t);
}

static struct icaltimetype occurrence_as_icaltime(icalrecur_iterator *impl,
                                                  int normalize)
{
    return (normalize ? icaltime_normalize(impl->last) : impl->last);
}

struct icaltimetype __icaltime_from_day_of_year(icalrecur_iterator *impl,
                                                int day, int year, int *weekno)
{
    struct icaltimetype tt;

    if (day < 0) {
        day += get_days_in_year(impl, year) + 1;
    }

    tt = icaltime_from_day_of_year(day, year);

    if (weekno) {
        *weekno = get_week_number(impl, tt);
    }
    return tt;
}

static void increment_year(icalrecur_iterator *impl, int inc)
{
    impl->last.year += inc;
}

static void __increment_month(icalrecur_iterator *impl, int inc)
{
    int years;

    impl->last.month += inc;

    /* Months are offset by one */
    impl->last.month--;

    years = impl->last.month / 12;

    impl->last.month = impl->last.month % 12;

    if (impl->last.month < 0) {
        impl->last.month = impl->last.month + 12;
        years--;
    }

    impl->last.month++;

    if (years != 0) {
        increment_year(impl, years);
    }
}

static void increment_monthday(icalrecur_iterator *impl, int inc)
{
    icaltime_adjust(&impl->last, inc, 0, 0, 0);
}

static void increment_hour(icalrecur_iterator *impl, int inc)
{
    icaltime_adjust(&impl->last, 0, inc, 0, 0);
}

static void increment_minute(icalrecur_iterator *impl, int inc)
{
    icaltime_adjust(&impl->last, 0, 0, inc, 0);
}

static void increment_second(icalrecur_iterator *impl, int inc)
{
    icaltime_adjust(&impl->last, 0, 0, 0, inc);
}

static int initialize_rscale(icalrecur_iterator *impl)
{
    if (impl->rule.rscale && strcasecmp(impl->rule.rscale, "GREGORIAN")) {
        icalerror_set_errno(ICAL_UNIMPLEMENTED_ERROR);
        return 0;
    }

    impl->rstart = impl->dtstart;

    return 1;
}

/** Sets the Gregorian date */
static void set_datetime(icalrecur_iterator *impl, icaltimetype date)
{
    impl->last.year = date.year;
    impl->last.month = date.month;
    impl->last.day = date.day;
    impl->last.is_date = impl->dtstart.is_date;
    impl->last.zone = impl->dtstart.zone;

    if (!impl->dtstart.is_date) {
        __get_start_time(impl, date, &impl->last.hour,
                         &impl->last.minute, &impl->last.second);
    }
}

/** Calculate the number of Gregorian months between 2 dates */
static int month_diff(icalrecur_iterator *impl, icaltimetype a, icaltimetype b)
{
    _unused(impl);

    return __greg_month_diff(a, b);
}

/** Calculate the number of Gregorian days between 2 dates */
static int day_diff(icalrecur_iterator *impl, icaltimetype a, icaltimetype b)
{
    return __day_diff(impl, a, b);
}

static void reset_period_start(icalrecur_iterator *impl)
{
    set_datetime(impl, impl->period_start);
}

#endif /* HAVE_LIBICU */

static int __iterator_set_start(icalrecur_iterator *impl, icaltimetype start);
static void increment_month(icalrecur_iterator *impl, int inc);
static int expand_month_days(icalrecur_iterator *impl, int year, int month);
static int expand_year_days(icalrecur_iterator *impl, int year);
static int next_yearday(icalrecur_iterator *impl,
                        void (*next_period)(icalrecur_iterator *, int));
static int prev_yearday(icalrecur_iterator *impl,
                        void (*next_period)(icalrecur_iterator *, int));

static void adjust_to_byday(icalrecur_iterator *impl)
{
    /* If there is BY_DAY data, then we need to move the initial
       time to the start of the BY_DAY data. That is if the
       start time is on a Wednesday, and the rule has
       BYDAY=MO,WE,FR, move the initial time back to
       monday. Otherwise, jumping to the next week ( jumping 7
       days ahead ) will skip over some occurrences in the
       second week. */

    /* This depends on impl->by_ptrs[BY_DAY] being correctly sorted by
     * day. This should probably be abstracted to make such assumption
     * more explicit. */
    short this_dow = (short)get_day_of_week(impl);
    short dow = (short)(impl->by_ptrs[BY_DAY][0] - this_dow);

    /* Normalize day of week around week start */
    if (dow != 0 && this_dow < (short)impl->rule.week_start)
        dow -= 7;

    if ((this_dow < impl->by_ptrs[BY_DAY][0] && dow >= 0) || dow < 0) {
        /* initial time is after first day of BY_DAY data */
        increment_monthday(impl, dow);
    }
}

icalrecur_iterator *icalrecur_iterator_new(struct icalrecurrencetype rule,
                                           struct icaltimetype dtstart)
{
    icalrecur_iterator *impl;
    icalrecurrencetype_frequency freq = rule.freq;
    enum byrule byrule;

    icalerror_clear_errno();

    if (freq == ICAL_NO_RECURRENCE) {
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        return 0;
    }

#define IN_RANGE(val, min, max) (val >= min && val <= max)

    /* Make sure that DTSTART is a sane value */
    if (!icaltime_is_valid_time(dtstart) ||
        !IN_RANGE(dtstart.year, 0, MAX_TIME_T_YEAR) ||
        !IN_RANGE(dtstart.month, 1, 12) ||
        !IN_RANGE(dtstart.day, 1,
                  icaltime_days_in_month(dtstart.month, dtstart.year)) ||
        (!dtstart.is_date && (!IN_RANGE(dtstart.hour, 0, 23) ||
                              !IN_RANGE(dtstart.minute, 0, 59) ||
                              !IN_RANGE(dtstart.second, 0, 59)))) {
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        return 0;
    }

    if (!(impl = (icalrecur_iterator *)icalmemory_new_buffer(sizeof(icalrecur_iterator)))) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return 0;
    }

    memset(impl, 0, sizeof(icalrecur_iterator));

    impl->dtstart = dtstart;
    impl->rule = rule;
    impl->iend = icaltime_null_time();

    /* Set up convenience pointers to make the code simpler. Allows
       us to iterate through all of the BY* arrays in the rule. */

    impl->by_ptrs[BY_MONTH] = impl->rule.by_month;
    impl->by_ptrs[BY_WEEK_NO] = impl->rule.by_week_no;
    impl->by_ptrs[BY_YEAR_DAY] = impl->rule.by_year_day;
    impl->by_ptrs[BY_MONTH_DAY] = impl->rule.by_month_day;
    impl->by_ptrs[BY_DAY] = impl->rule.by_day;
    impl->by_ptrs[BY_HOUR] = impl->rule.by_hour;
    impl->by_ptrs[BY_MINUTE] = impl->rule.by_minute;
    impl->by_ptrs[BY_SECOND] = impl->rule.by_second;
    impl->by_ptrs[BY_SET_POS] = impl->rule.by_set_pos;

    memset(impl->orig_data, 0, NUM_BY_PARTS * sizeof(short));

    /* Note which by rules had data in them when the iterator was
       created. We can't use the actual by_x arrays, because the
       empty ones will be given default values later in this
       routine. The orig_data array will be used later in has_by_data */

    impl->orig_data[BY_MONTH] =
      (short)(impl->rule.by_month[0] != ICAL_RECURRENCE_ARRAY_MAX);
    impl->orig_data[BY_WEEK_NO] =
      (short)(impl->rule.by_week_no[0] != ICAL_RECURRENCE_ARRAY_MAX);
    impl->orig_data[BY_YEAR_DAY] =
      (short)(impl->rule.by_year_day[0] != ICAL_RECURRENCE_ARRAY_MAX);
    impl->orig_data[BY_MONTH_DAY] =
      (short)(impl->rule.by_month_day[0] != ICAL_RECURRENCE_ARRAY_MAX);
    impl->orig_data[BY_DAY] =
      (short)(impl->rule.by_day[0] != ICAL_RECURRENCE_ARRAY_MAX);
    impl->orig_data[BY_HOUR] =
      (short)(impl->rule.by_hour[0] != ICAL_RECURRENCE_ARRAY_MAX);
    impl->orig_data[BY_MINUTE] =
      (short)(impl->rule.by_minute[0] != ICAL_RECURRENCE_ARRAY_MAX);
    impl->orig_data[BY_SECOND] =
      (short)(impl->rule.by_second[0] != ICAL_RECURRENCE_ARRAY_MAX);
    impl->orig_data[BY_SET_POS] =
      (short)(impl->rule.by_set_pos[0] != ICAL_RECURRENCE_ARRAY_MAX);

    /* Check if the recurrence rule is legal */

    for (byrule = 0; byrule < NUM_BY_PARTS; byrule++) {
        if (expand_map[freq].map[byrule] == ILLEGAL &&
            has_by_data(impl, byrule)) {
            ical_invalid_rrule_handling rruleHandlingSetting =
                ical_get_invalid_rrule_handling_setting();
            if (rruleHandlingSetting == ICAL_RRULE_IGNORE_INVALID) {
                impl->orig_data[byrule] = 0;
            } else {
                icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
                icalmemory_free_buffer(impl);
                return 0;
            }
        }
    }

    if (initialize_rscale(impl) == 0) {
        icalrecur_iterator_free(impl);
        return 0;
    }

    /* Set up defaults for BY_* arrays */
    setup_defaults(impl, BY_SECOND, impl->rstart.second);

    setup_defaults(impl, BY_MINUTE, impl->rstart.minute);

    setup_defaults(impl, BY_HOUR, impl->rstart.hour);

    setup_defaults(impl, BY_MONTH_DAY, impl->rstart.day);

    setup_defaults(impl, BY_MONTH, impl->rstart.month);

    if (!__iterator_set_start(impl, dtstart)) {
        icalrecur_iterator_free(impl);
        return 0;
    }

    return impl;
}

void icalrecur_iterator_free(icalrecur_iterator *i)
{
    icalerror_check_arg_rv((i != 0), "impl");

#if defined(HAVE_LIBICU)
    if (i->greg) {
        if (i->rscale && (i->rscale != i->greg)) {
            ucal_close(i->rscale);
        }

        ucal_close(i->greg);
    }
#endif

    icalmemory_free_buffer(i);
}

/** Calculate the number of days between 2 dates */
static int __day_diff(icalrecur_iterator *impl, icaltimetype a, icaltimetype b)
{
    int diff;

    if (a.year == b.year) {
        diff = get_day_of_year(impl, b.year, b.month, b.day, NULL) -
            get_day_of_year(impl, a.year, a.month, a.day, NULL);
    } else {
        /* Swap a and b if a is greater than b */
        int flipped = 0;
        int year;

        if (a.year > b.year) {
            icaltimetype temp = a;

            a = b;
            b = temp;
            flipped = 1;
        }

        /* Count days in each year to account for leap days/months */
        year = a.year;

        diff = get_days_in_year(impl, year) -
            get_day_of_year(impl, a.year, a.month, a.day, NULL);
        while (++year < b.year) diff += get_days_in_year(impl, year);
        diff += get_day_of_year(impl, b.year, b.month, b.day, NULL);

        if (flipped) {
            /* The difference is negative because a was greater than b */
            diff = -diff;
        }
    }

    return diff;
}

/** increment_month is different than the other increment_* routines --
   it figures out the interval for itself, and uses BYMONTH data if
   available. */
static void increment_month(icalrecur_iterator *impl, int inc)
{
    __increment_month(impl, inc);

    if (has_by_data(impl, BY_MONTH)) {
        struct icaltimetype this = occurrence_as_icaltime(impl, 0);

        while (this.year < 20000) {
            for (BYMONIDX = 0;
                 BYMONPTR[BYMONIDX] != ICAL_RECURRENCE_ARRAY_MAX; BYMONIDX++) {

                if (this.month == BYMONPTR[BYMONIDX]) return;
            }

            __increment_month(impl, inc);
            this = occurrence_as_icaltime(impl, 0);
        }
    }
}

#if 0
#include "ical.h"
void test_increment()
{
    icalrecur_iterator impl;

    impl.last = icaltime_from_string("20000101T000000Z");

    printf("Orig: %s\n", icaltime_as_ctime(impl.last));

    increment_second(&impl, 5);
    printf("+ 5 sec    : %s\n", icaltime_as_ctime(impl.last));

    increment_second(&impl, 355);
    printf("+ 355 sec  : %s\n", icaltime_as_ctime(impl.last));

    increment_minute(&impl, 5);
    printf("+ 5 min    : %s\n", icaltime_as_ctime(impl.last));

    increment_minute(&impl, 360);
    printf("+ 360 min  : %s\n", icaltime_as_ctime(impl.last));
    increment_hour(&impl, 5);
    printf("+ 5 hours  : %s\n", icaltime_as_ctime(impl.last));
    increment_hour(&impl, 43);
    printf("+ 43 hours : %s\n", icaltime_as_ctime(impl.last));
    increment_monthday(&impl, 3);
    printf("+ 3 days   : %s\n", icaltime_as_ctime(impl.last));
    increment_monthday(&impl, 600);
    printf("+ 600 days  : %s\n", icaltime_as_ctime(impl.last));
}

#endif

static int next_unit(icalrecur_iterator *impl,
                     int by_unit, icalrecurrencetype_frequency frequency,
                     int (*next_sub_unit)(icalrecur_iterator *),
                     void (*set_unit)(icalrecur_iterator *, int),
                     void (*increment_unit)(icalrecur_iterator *, int),
                     void (*increment_super_unit)(icalrecur_iterator *, int))
{
    int has_by_unit = (by_unit > NO_CONTRACTION) &&
        (impl->by_ptrs[by_unit][0] != ICAL_RECURRENCE_ARRAY_MAX);
    int this_frequency = (impl->rule.freq == frequency);

    int end_of_data = 0;

    icalassert(has_by_unit || this_frequency);

    if (next_sub_unit && next_sub_unit(impl) == 0) {
        return 0;
    }

    if (has_by_unit) {
        /* Ignore the frequency and use the byrule data */

        impl->by_indices[by_unit]++;

        if (impl->by_ptrs[by_unit][impl->by_indices[by_unit]] ==
            ICAL_RECURRENCE_ARRAY_MAX) {
            impl->by_indices[by_unit] = 0;

            end_of_data = 1;
        }

        set_unit(impl, impl->by_ptrs[by_unit][impl->by_indices[by_unit]]);

    } else if (!has_by_unit && this_frequency) {
        /* Compute the next value from the last time and the freq interval */
        increment_unit(impl, impl->rule.interval);
    }

    /* If we have gone through all of the units on the BY list, then we
       need to move to the next larger unit */

    if (has_by_unit && end_of_data && this_frequency) {
        increment_super_unit(impl, 1);
    }

    return end_of_data;
}

static int next_second(icalrecur_iterator *impl)
{
    return next_unit(impl, BY_SECOND, ICAL_SECONDLY_RECURRENCE, NULL,
                     &set_second, &increment_second, &increment_minute);
}

static int next_minute(icalrecur_iterator *impl)
{
    return next_unit(impl, BY_MINUTE, ICAL_MINUTELY_RECURRENCE, &next_second,
                     &set_minute, &increment_minute, &increment_hour);
}

static int next_hour(icalrecur_iterator *impl)
{
    return next_unit(impl, BY_HOUR, ICAL_HOURLY_RECURRENCE, &next_minute,
                     &set_hour, &increment_hour, &increment_monthday);
}

static int next_day(icalrecur_iterator *impl)
{
    return next_unit(impl, NO_CONTRACTION, ICAL_DAILY_RECURRENCE, &next_hour,
                     NULL, &increment_monthday, NULL);
}

static int prev_unit(icalrecur_iterator *impl,
                     int by_unit, icalrecurrencetype_frequency frequency,
                     int (*prev_sub_unit)(icalrecur_iterator *),
                     void (*set_unit)(icalrecur_iterator *, int),
                     void (*increment_unit)(icalrecur_iterator *, int),
                     void (*increment_super_unit)(icalrecur_iterator *, int))
{
    int has_by_unit = (by_unit > NO_CONTRACTION) &&
        (impl->by_ptrs[by_unit][0] != ICAL_RECURRENCE_ARRAY_MAX);
    int this_frequency = (impl->rule.freq == frequency);

    int end_of_data = 0;

    icalassert(has_by_unit || this_frequency);

    if (prev_sub_unit && prev_sub_unit(impl) == 0) {
        return 0;
    }

    if (has_by_unit) {
        /* Ignore the frequency and use the byrule data */

        impl->by_indices[by_unit]--;

        if (impl->by_indices[by_unit] < 0) {
            impl->by_indices[by_unit] =
                icalrecur_iterator_sizeof_byarray(impl->by_ptrs[by_unit]) - 1;

            end_of_data = 1;
        }

        set_unit(impl, impl->by_ptrs[by_unit][impl->by_indices[by_unit]]);

    } else if (!has_by_unit && this_frequency) {
        /* Compute the next value from the last time and the freq interval */
        increment_unit(impl, -impl->rule.interval);
    }

    /* If we have gone through all of the units on the BY list, then we
       need to move to the next larger unit */

    if (has_by_unit && end_of_data && this_frequency) {
        increment_super_unit(impl, -1);
    }

    return end_of_data;
}

static int prev_second(icalrecur_iterator *impl)
{
    return prev_unit(impl, BY_SECOND, ICAL_SECONDLY_RECURRENCE, NULL,
                     &set_second, &increment_second, &increment_minute);
}

static int prev_minute(icalrecur_iterator *impl)
{
    return prev_unit(impl, BY_MINUTE, ICAL_MINUTELY_RECURRENCE, &prev_second,
                     &set_minute, &increment_minute, &increment_hour);
}

static int prev_hour(icalrecur_iterator *impl)
{
    return prev_unit(impl, BY_HOUR, ICAL_HOURLY_RECURRENCE, &prev_minute,
                     &set_hour, &increment_hour, &increment_monthday);
}

static int prev_day(icalrecur_iterator *impl)
{
    return prev_unit(impl, NO_CONTRACTION, ICAL_DAILY_RECURRENCE, &prev_hour,
                     NULL, &increment_monthday, NULL);
}

static int check_set_position(icalrecur_iterator *impl, int set_pos)
{
    int i;
    int found = 0;

    for (i = 0;
         i < ICAL_BY_SETPOS_SIZE &&
             impl->rule.by_set_pos[i] != ICAL_RECURRENCE_ARRAY_MAX;
         i++) {
        if (impl->rule.by_set_pos[i] == set_pos) {
            found = 1;
            break;
        }
    }
    return found;
}

/** Add each BYMONTHDAY to the year days bitmask */
static int expand_bymonth_days(icalrecur_iterator *impl, int year, int month)
{
    int i, set_pos_total = 0;
    int days_in_month = get_days_in_month(impl, month, year);

    for (i = 0; BYMDPTR[i] != ICAL_RECURRENCE_ARRAY_MAX; i++) {
        short doy = ICAL_BY_YEARDAY_SIZE, mday = BYMDPTR[i];
        int this_month = month;

        if (abs(mday) > days_in_month) {
            int days_in_year = get_days_in_year(impl, year);

            switch (impl->rule.skip) {
            default:
                /* Should never get here! */

            case ICAL_SKIP_OMIT:
                continue;

            case ICAL_SKIP_FORWARD:
                if (mday > 0) this_month++;  /* Next month */

                if (this_month > get_months_in_year(impl, year)) {
                    doy = days_in_year + 1;  /* First day of next year */
                } else {
                    mday = 1;                /* First day of month */
                }
                break;

            case ICAL_SKIP_BACKWARD:
                if (mday < 0) {
                    this_month--;  /* Prev month */
                }

                if (this_month == 0) {
                    doy = 0;       /* Last day of prev year */
                } else {
                    mday = -1;     /* Last day of month */
                }
                break;
            }
        }

        if (doy == ICAL_BY_YEARDAY_SIZE) {
            doy = get_day_of_year(impl, year, this_month, mday, NULL);
        }

        daysmask_setbit(impl->days, doy, 1);
        set_pos_total++;
        if (doy < impl->days_index) impl->days_index = doy;
    }

    return set_pos_total;
}

/** Expand the BYDAY rule part and apply it to the year days map. */
static int expand_by_day(icalrecur_iterator *impl, int year,
                         int doy_offset, int last_day,
                         int first_dow, int last_dow,
                         int is_limiting)
{
    /* Try to calculate each of the occurrences. */
    unsigned long bydays[LONGS_PER_BITS(ICAL_YEARDAYS_MASK_SIZE)];
    int i, set_pos_total = 0;
    short doy;

    daysmask_clearall(bydays);

    for (i = 0; BYDAYPTR[i] != ICAL_RECURRENCE_ARRAY_MAX; i++) {
        /* This is 1 (Sun) to 7 (Sat). */
        int dow = icalrecurrencetype_day_day_of_week(BYDAYPTR[i]);
        int pos = icalrecurrencetype_day_position(BYDAYPTR[i]);
        int first_matching_day, last_matching_day;
        int day, this_weekno;

        /* Calculate the first day in the period
           with the given weekday, and the last day. */
        first_matching_day = ((dow + 7 - first_dow) % 7) + 1;
        last_matching_day = last_day - ((last_dow + 7 - dow) % 7);

        if (pos == 0) {
            /* First instance of the weekday within the period.
               (Remaining instances added by loop below. */
            day = first_matching_day;

        } else if (pos > 0) {
            /* nth instance of the weekday within the period. */
            day = first_matching_day + (pos - 1) * 7;

            if (day > last_matching_day) {
                continue;
            }

        } else { /* pos < 0 */
            /* -nth instance of the weekday within the period. */
            day = last_matching_day + (pos + 1) * 7;

            if (day < first_matching_day) {
                continue;
            }
        }

        if (doy_offset < 0) {
            this_weekno = 1;
        } else {
            (void)__icaltime_from_day_of_year(impl, day + doy_offset, year,
                                              &this_weekno);
        }

        /* Add instance(s) of the weekday within the period */
        do {
            int valid = 0;

            if (has_by_data(impl, BY_WEEK_NO)) {
                /* Make sure our day falls in one of the BYWEEKNO */
                int nweeks = weeks_in_year(year);
                int j;

                for (j = 0; BYWEEKPTR[j] != ICAL_RECURRENCE_ARRAY_MAX; j++) {
                    int weekno = BYWEEKPTR[j];

                    if (weekno < 0) weekno += nweeks + 1;

                    if (weekno == this_weekno) {
                        valid = 1;
                        break;
                    }
                }
            } else {
                valid = 1;
            }

            if (valid) {
                daysmask_setbit(bydays, day + doy_offset, 1);
            }

        } while (!pos && ((day += 7) <= last_day) && ++this_weekno);
    }

    /* Apply bydays map to the year days bitmask */
    for (doy = doy_offset+1; doy <= doy_offset + last_day; doy++) {
        int valid;

        if (is_limiting) {
            /* "Filter" the year days bitmask with the bydays bitmask */
            valid = (int)(daysmask_getbit(impl->days, doy) &
                          daysmask_getbit(bydays, doy));
        } else {
            /* Add each BYDAY to the year days bitmask */
            valid = (int)daysmask_getbit(bydays, doy);
        }

        daysmask_setbit(impl->days, doy, valid);

        if (valid) {
            set_pos_total++;
            if (doy < impl->days_index) impl->days_index = doy;
        }
    }

    return set_pos_total;
}

/** "Filter" the year days bitmask with each BYSETPOS */
static void filter_bysetpos(icalrecur_iterator *impl, int pos_total,
                            int start_doy, int end_doy)
{
    int pos_count = 0;
    short doy;

    impl->days_index = ICAL_YEARDAYS_MASK_SIZE;

    for (doy = start_doy; doy <= end_doy; doy++) {
        if (daysmask_getbit(impl->days, doy)) {

            daysmask_setbit(impl->days, doy,
                            (check_set_position(impl, pos_count + 1) ||
                             check_set_position(impl, pos_count - pos_total)));

            if (daysmask_getbit(impl->days, doy) && doy < impl->days_index) {
                impl->days_index = doy;
            }
            pos_count++;
        }
    }
}

/** For INTERVAL=MONTHLY, set up the year days bitmask in the iterator to
   list all of the days of the current month that are specified in this
   rule. */
static int expand_month_days(icalrecur_iterator *impl, int year, int month)
{
    int doy_offset, days_in_month, first_dow, set_pos_total;

    daysmask_clearall(impl->days);

    /* We may end up skipping fwd/bwd a month during expansion.
       Mark our current start date so next_month() can increment from here */
    impl->period_start = occurrence_as_icaltime(impl, 0);

    doy_offset = get_day_of_year(impl, year, month, 1, &first_dow) - 1;
    days_in_month = get_days_in_month(impl, month, year);

    /* Add each BYMONTHDAY to the year days bitmask */
    set_pos_total = expand_bymonth_days(impl, year, month);

    if (has_by_data(impl, BY_DAY)) {
        /* Apply each BYDAY to the year days bitmask */
        int last_dow;

        impl->days_index = ICAL_YEARDAYS_MASK_SIZE;

        (void)get_day_of_year(impl, year, month, days_in_month, &last_dow);

        set_pos_total = expand_by_day(impl, year, doy_offset, days_in_month,
                                      first_dow, last_dow,
                                      has_by_data(impl, BY_MONTH_DAY));
    }

    if (has_by_data(impl, BY_SET_POS)) {
        /* "Filter" the year days bitmask with each BYSETPOS */
        filter_bysetpos(impl, set_pos_total,
                        doy_offset + 1, doy_offset + days_in_month);
    }

    return 0;
}

static void __next_month(icalrecur_iterator *impl, int inc)
{
    struct icaltimetype this;

    /* Increment to and expand the next month */
    increment_month(impl, inc);
    this = occurrence_as_icaltime(impl, 0);
    expand_month_days(impl, this.year, this.month);
}

static int next_month(icalrecur_iterator *impl)
{
    return next_yearday(impl, &__next_month);
}

static int prev_month(icalrecur_iterator *impl)
{
    return prev_yearday(impl, &__next_month);
}

static int next_weekday_by_week(icalrecur_iterator *impl)
{
    int end_of_data = 0;
    int start_of_week, dow;

    if (next_hour(impl) == 0) {
        return 0;
    }

    if (!has_by_data(impl, BY_DAY)) {
        return 1;
    }

    /* If we get here, we need to step to the next day */

    for (;;) {
        BYDAYIDX++;     /* Look at next elem in BYDAY array */

        /* Are we at the end of the BYDAY array? */
        if (BYDAYPTR[BYDAYIDX] == ICAL_RECURRENCE_ARRAY_MAX) {
            BYDAYIDX = 0;       /* Reset to 0 */
            end_of_data = 1;    /* Signal that we're at the end */
        }

        /* Add the day of week offset to the start of this week, and use
           that to get the next day */
        /* ignore position of dow ("4FR"), only use dow ("FR") */
        dow = icalrecurrencetype_day_day_of_week(BYDAYPTR[BYDAYIDX]);
        dow -= impl->rule.week_start;   /* Set Sunday to be 0 */
        if (dow < 0) {
            dow += 7;
        }

        start_of_week = get_start_of_week(impl);

        if (dow + start_of_week < 1) {
            /* The selected date is in the previous year. */
            if (!end_of_data) {
                continue;
            }

            increment_year(impl, -1);
        }

        set_day_of_year(impl, start_of_week + dow);

        return end_of_data;
    }
}

static int next_week(icalrecur_iterator *impl)
{
    int end_of_data = 0;

    /* Increment to the next week day,
       if there is data at a level less than a week */
    if (next_weekday_by_week(impl) == 0) {
        return 0;       /* Have not reached end of week yet */
    }

    /* If we get here, we have incremented through the entire week, and
       can increment to the next week */

    /* Jump to the next week */
    increment_monthday(impl, 7 * impl->rule.interval);

    return end_of_data;
}

static int prev_weekday_by_week(icalrecur_iterator *impl)
{
    int end_of_data = 0;
    int start_of_week, dow;

    if (prev_hour(impl) == 0) {
        return 0;
    }

    if (!has_by_data(impl, BY_DAY)) {
        return 1;
    }

    /* If we get here, we need to step to the previous day */

    BYDAYIDX--;     /* Look at previous elem in BYDAY array */

    /* Are we at the end of the BYDAY array? */
    if (BYDAYIDX < 0) {
        BYDAYIDX = icalrecur_iterator_sizeof_byarray(impl->by_ptrs[BY_DAY]) - 1;
        end_of_data = 1;    /* Signal that we're at the end */
    }

    /* Add the day of week offset to the start of this week, and use
       that to get the next day */
    /* ignore position of dow ("4FR"), only use dow ("FR") */
    dow = icalrecurrencetype_day_day_of_week(BYDAYPTR[BYDAYIDX]);
    dow -= impl->rule.week_start;   /* Set Sunday to be 0 */
    if (dow < 0) {
        dow += 7;
    }

    start_of_week = get_start_of_week(impl);

    if (dow + start_of_week < 1) {
        /* The selected date is in the previous year. */
        increment_year(impl, -1);
    }

    set_day_of_year(impl, start_of_week + dow);

    return end_of_data;
}

static int prev_week(icalrecur_iterator *impl)
{
    int end_of_data = 0;

    /* Decrement to the previous week day,
       if there is data at a level less than a week */
    if (prev_weekday_by_week(impl) == 0) {
        return 0;       /* Have not reached start of week yet */
    }

    /* If we get here, we have decremented through the entire week, and
       can decrement to the previous week */

    /* Jump to the previous week */
    increment_monthday(impl, 7 * -impl->rule.interval);

    return end_of_data;
}

/* For INTERVAL=YEARLY, set up the year days bitmask in the iterator to
   list all of the days of the current year that are specified in this
   rule. */
static int expand_year_days(icalrecur_iterator *impl, int year)
{
    int i;
    int set_pos_total = 0;
    short days_in_year = (short)get_days_in_year(impl, year);
    short doy;

    daysmask_clearall(impl->days);

    /* We may end up skipping fwd/bwd a year during expansion.
       Mark our current start date so next_year() can increment from here */
    impl->period_start = occurrence_as_icaltime(impl, 0);

    if (has_by_data(impl, BY_YEAR_DAY)) {
        /* We only support BYYEARDAY + BYDAY */
        if (has_by_data(impl, BY_WEEK_NO) ||
            has_by_data(impl, BY_MONTH) || has_by_data(impl, BY_MONTH_DAY)) {
            icalerror_set_errno(ICAL_UNIMPLEMENTED_ERROR);
            return 0;
        }

        /* Add each BYYEARDAY to the year days bitmask */
        for (i = 0; BYYDPTR[i] != ICAL_RECURRENCE_ARRAY_MAX; i++) {
            doy = BYYDPTR[i];

            if (abs(doy) > days_in_year) {
                switch (impl->rule.skip) {
                default:
                    /* Should never get here! */

                case ICAL_SKIP_OMIT:
                    /* Invalid day */
                    continue;

                case ICAL_SKIP_FORWARD:
                    if (doy < 0) {
                        doy = 1;                 /* First day of this year */
                    } else {
                        doy = days_in_year + 1;  /* First day of next year */
                    }
                    break;

                case ICAL_SKIP_BACKWARD:
                    if (doy < 0) {
                        doy = 0;                 /* Last day of prev year */
                    } else {
                        doy = days_in_year;      /* Last day of this year */
                    }
                    break;
                }
            } else if (doy < 0) {
                doy += days_in_year + 1;
            }

            daysmask_setbit(impl->days, doy, 1);
            set_pos_total++;
            if (doy < impl->days_index) impl->days_index = doy;
        }
    }
    else if (has_by_data(impl, BY_WEEK_NO)) {
        int weekno, start_doy;

        /* We only support BYWEEKNO + BYDAY */
        if (has_by_data(impl, BY_YEAR_DAY) ||
            has_by_data(impl, BY_MONTH_DAY) ||
            (has_by_data(impl, BY_MONTH) && !has_by_data(impl, BY_DAY))) {
            icalerror_set_errno(ICAL_UNIMPLEMENTED_ERROR);
            return 0;
        }

        /* BYWEEKNO + BYDAY handled below */
        if (!has_by_data(impl, BY_DAY)) {
            int nweeks = weeks_in_year(year);

            /* Calculate location of DTSTART day in weekno 1 */
            doy = get_day_of_year(impl, year,
                                  impl->dtstart.month, impl->dtstart.day, NULL);
            (void)__icaltime_from_day_of_year(impl, doy, year, &weekno);
            if (weekno > doy) doy += 7;
            start_doy = doy + get_start_of_week(impl);

            /* Add day of week in each BYWEEKNO to the year days bitmask */
            for (i = 0; BYWEEKPTR[i] != ICAL_RECURRENCE_ARRAY_MAX; i++) {
                weekno = BYWEEKPTR[i];

                if (weekno < 0) {
                    weekno += nweeks + 1;
                }

                doy = start_doy + 7 * (weekno - 1);

                daysmask_setbit(impl->days, doy, 1);
                set_pos_total++;
                if (doy < impl->days_index) impl->days_index = doy;
            }
        }
    } else {
        /* Add each BYMONTHDAY in each BYMONTH to the year days bitmask */
        for (i = 0; BYMONPTR[i] != ICAL_RECURRENCE_ARRAY_MAX; i++) {
            int month = set_month(impl, BYMONPTR[i]);

            if (month) set_pos_total += expand_bymonth_days(impl, year, month);
        }
    }

    if (has_by_data(impl, BY_DAY)) {
        /* Apply each BYDAY to the year days bitmask */
        int limiting =
            has_by_data(impl, BY_YEAR_DAY) || has_by_data(impl, BY_MONTH_DAY);
        int first_dow, last_dow;

        impl->days_index = ICAL_YEARDAYS_MASK_SIZE;
        set_pos_total = 0;

        if (has_by_data(impl, BY_MONTH)) {
            /* Numeric BYDAY are within each month */

            for (i = 0; BYMONPTR[i] != ICAL_RECURRENCE_ARRAY_MAX; i++) {
                short month = BYMONPTR[i];
                int doy_offset, days_in_month;

                /* Get offset within year & day of week of first day of month */
                doy_offset =
                    get_day_of_year(impl, year, month, 1, &first_dow) - 1;

                /* Get day of week of last day of month */
                days_in_month = get_days_in_month(impl, month, year);
                (void)get_day_of_year(impl, year,
                                      month, days_in_month, &last_dow);

                set_pos_total += expand_by_day(impl, year,
                                               doy_offset, days_in_month,
                                               first_dow, last_dow, limiting);
            }
        } else {
            /* Numeric BYDAY are within the year */
            short doy_offset = 0, last_day;

            if (has_by_data(impl, BY_WEEK_NO)) {
                int weekno;

                /* See which week contains Jan 1 */
                (void)__icaltime_from_day_of_year(impl, 1, year, &weekno);
                if (weekno > 1) {
                    /* Jan 1 is in last week of previous year - jump ahead */
                    doy_offset += 7;
                }

                /* Set start and end of ISO week-numbering year */
                doy_offset += get_start_of_week(impl) - 1;
                last_day = (7 * weeks_in_year(year)) - doy_offset - 1;

                first_dow = impl->rule.week_start;
                last_dow = (first_dow + 6) % 7;
            } else {
                /* Get day of week of first day of year */
                (void)get_day_of_year(impl, year, 1, 1, &first_dow);

                /* Get day of week of last day of year */
                set_day_of_year(impl, days_in_year);
                last_dow = get_day_of_week(impl);

                last_day = days_in_year;
            }

            set_pos_total += expand_by_day(impl, year, doy_offset, last_day,
                                           first_dow, last_dow, limiting);
        }
    }

    if (has_by_data(impl, BY_SET_POS)) {
        /* "Filter" the year days bitmask with each BYSETPOS */
        filter_bysetpos(impl, set_pos_total, 1, days_in_year);
    }

    return 0;
}

static void __next_year(icalrecur_iterator *impl, int inc)
{
    struct icaltimetype this;

    /* Increment to and expand the next year */
    increment_year(impl, inc);
    this = occurrence_as_icaltime(impl, 0);
    expand_year_days(impl, this.year);
}

static int next_year(icalrecur_iterator *impl)
{
    return next_yearday(impl, &__next_year);
}

static int prev_year(icalrecur_iterator *impl)
{
    return prev_yearday(impl, &__next_year);
}

static short daymask_find_next_bit(icalrecur_iterator *impl)
{
    unsigned long *days = impl->days;
    short days_index = impl->days_index + 1;
    unsigned long v;
    short startBitIndex;
    unsigned short wordIdx, maxWordIdx;

    if (days_index >= ICAL_YEARDAYS_MASK_SIZE)
        return ICAL_YEARDAYS_MASK_SIZE;

    // Prepare the first word, where searching might not start at the beginning
    startBitIndex = days_index + ICAL_YEARDAYS_MASK_OFFSET;
    wordIdx = (unsigned short)(startBitIndex / BITS_PER_LONG);
    v = days[wordIdx];
    v >>= startBitIndex % BITS_PER_LONG;

    if (!v) {
        // so the first word didn't contain any bits of interest.
        days_index += BITS_PER_LONG - startBitIndex % BITS_PER_LONG;

        // Are there more empty words following? Skip them.
        maxWordIdx = (unsigned short)(LONGS_PER_BITS(ICAL_YEARDAYS_MASK_SIZE)) - 1;
        while (days_index < ICAL_YEARDAYS_MASK_SIZE && wordIdx < maxWordIdx) {

            wordIdx++;
            v = days[wordIdx];

            if (v)
                break;

            days_index += BITS_PER_LONG;
        }
    }

    if (v) {
        // We found a word containing the next bit but don't know the exact
        // position yet. Do a b-search to find it.

        unsigned long mask;
        int maskSize = (int)(BITS_PER_LONG / 2);
        mask = (((unsigned long)1) << maskSize) - 1;

        while (maskSize) {
            if ((v & mask) == 0) {
                v >>= maskSize;
                days_index += maskSize;
            }
            maskSize /= 2;
            mask >>= maskSize;
        }
    }

    return days_index;
}

static short daymask_find_prev_bit(icalrecur_iterator *impl)
{
    unsigned long *days = impl->days;
    short days_index = impl->days_index - 1;
    unsigned long v;
    short startBitIndex;
    int wordIdx;

    if (days_index <= -ICAL_YEARDAYS_MASK_OFFSET)
        return -ICAL_YEARDAYS_MASK_OFFSET;

    // Prepare the first word, where searching might not start at the beginning
    startBitIndex = days_index + ICAL_YEARDAYS_MASK_OFFSET;
    wordIdx = (int)(startBitIndex / BITS_PER_LONG);
    v = days[wordIdx];
    v <<= BITS_PER_LONG - (startBitIndex % BITS_PER_LONG) - 1;

    if (!v) {
        // so the first word didn't contain any bits of interest.
        days_index -= (startBitIndex % BITS_PER_LONG) +1;

        // Are there more empty words leading? Skip them.
        while (days_index > -ICAL_YEARDAYS_MASK_OFFSET) {
            wordIdx--;
            v = days[wordIdx];

            if (v)
                break;

            days_index -= BITS_PER_LONG;
        }
    }

    if (v) {

        // We found a word containing the next bit but don't know the exact
        // position yet. Do a b-search to find it.

        unsigned long mask;
        int maskSize = (int)(BITS_PER_LONG / 2);
        mask = ((((unsigned long)1) << maskSize) - 1) << maskSize;

        while (maskSize) {

            if ((v & mask) == 0) {
                v <<= maskSize;
                days_index -= maskSize;
            }
            maskSize /= 2;
            mask <<= maskSize;
        }
    }

    return days_index;
}

static int next_yearday(icalrecur_iterator *impl,
                        void (*next_period)(icalrecur_iterator *, int))
{
    if (next_hour(impl) == 0) {
        return 0;
    }

    /* We may have skipped fwd/bwd a month/year with previous occurrence.
       Reset the period start date so we can increment properly */
    reset_period_start(impl);

    /* Find next year day that is set */
    impl->days_index = daymask_find_next_bit(impl);

    if (impl->days_index >= ICAL_YEARDAYS_MASK_SIZE) {

        for (;;) {
            /* Increment to and expand the next period */
            next_period(impl, impl->rule.interval);

            if (impl->days_index < ICAL_YEARDAYS_MASK_SIZE) {
                break;  /* break when a matching day is found */
            }
        }
    }

    if (impl->days_index < 1) {
        /* Day is in previous year */
        increment_year(impl, -1);
    }

    set_day_of_year(impl, impl->days_index);

    return 1;
}

static int prev_yearday(icalrecur_iterator *impl,
                        void (*next_period)(icalrecur_iterator *, int))
{
    if (prev_hour(impl) == 0) {
        return 0;
    }

    /* We may have skipped fwd/bwd a month/year with previous occurrence.
       Reset the period start date so we can decrement properly */
    reset_period_start(impl);

    /* Find previous year day that is set */
    impl->days_index = daymask_find_prev_bit(impl);

    while (impl->days_index <= -ICAL_YEARDAYS_MASK_OFFSET) {
        /* Decrement to and expand the previous period */
        next_period(impl, -impl->rule.interval);

        impl->days_index = ICAL_YEARDAYS_MASK_SIZE;
        impl->days_index = daymask_find_prev_bit(impl);
    }

    if (impl->days_index < 1) {
        /* Day is in previous year */
        increment_year(impl, -1);
    }

    set_day_of_year(impl, impl->days_index);

    return 1;
}

int icalrecur_check_rulepart(icalrecur_iterator *impl,
                             int v, enum byrule byrule)
{
    int itr;

    if (impl->by_ptrs[byrule][0] != ICAL_RECURRENCE_ARRAY_MAX) {
        for (itr = 0;
             impl->by_ptrs[byrule][itr] != ICAL_RECURRENCE_ARRAY_MAX; itr++) {
            if (impl->by_ptrs[byrule][itr] == v) {
                return 1;
            }
        }
    }

    return 0;
}

static int check_contract_restriction(icalrecur_iterator *impl,
                                      enum byrule byrule, int v)
{
    int pass = 0;
    int itr;
    icalrecurrencetype_frequency freq = impl->rule.freq;

    if (impl->by_ptrs[byrule][0] != ICAL_RECURRENCE_ARRAY_MAX &&
        expand_map[freq].map[byrule] == CONTRACT) {
        for (itr = 0;
             impl->by_ptrs[byrule][itr] != ICAL_RECURRENCE_ARRAY_MAX; itr++) {
            if (impl->by_ptrs[byrule][itr] == v) {
                pass = 1;
                break;
            }
        }

        return pass;
    } else {
        /* This is not a contracting byrule, or it has no data, so the
           test passes */
        return 1;
    }
}

static int check_contracting_rules(icalrecur_iterator *impl)
{
    struct icaltimetype last = occurrence_as_icaltime(impl, 0);
    int day_of_week;
    int week_no = get_week_number(impl, last);
    int year_day =
        get_day_of_year(impl, last.year, last.month, last.day, &day_of_week);

    if (check_contract_restriction(impl, BY_SECOND, last.second) &&
        check_contract_restriction(impl, BY_MINUTE, last.minute) &&
        check_contract_restriction(impl, BY_HOUR, last.hour) &&
        check_contract_restriction(impl, BY_DAY, day_of_week) &&
        check_contract_restriction(impl, BY_WEEK_NO, week_no) &&
        check_contract_restriction(impl, BY_MONTH_DAY, last.day) &&
        check_contract_restriction(impl, BY_MONTH, last.month) &&
        check_contract_restriction(impl, BY_YEAR_DAY, year_day)) {

        return 1;
    } else {
        return 0;
    }
}

struct icaltimetype icalrecur_iterator_next(icalrecur_iterator *impl)
{
    /* Quit if we reached COUNT or if last time is after the UNTIL time */
    if (!impl ||
        (impl->rule.count != 0 && impl->occurrence_no >= impl->rule.count) ||
        (!icaltime_is_null_time(impl->rule.until) &&
         icaltime_compare(impl->last, impl->rule.until) > 0)) {
        return icaltime_null_time();
    }

    /* If initial time is valid, return it */
    if ((impl->occurrence_no == 0) &&
        (icaltime_compare(impl->last, impl->istart) >= 0) &&
        check_contracting_rules(impl)) {

        impl->occurrence_no++;
        return impl->last;
    }

    /* Iterate until we get the next valid time */
    do {
        switch (impl->rule.freq) {

        case ICAL_SECONDLY_RECURRENCE:
            next_second(impl);
            break;

        case ICAL_MINUTELY_RECURRENCE:
            next_minute(impl);
            break;

        case ICAL_HOURLY_RECURRENCE:
            next_hour(impl);
            break;

        case ICAL_DAILY_RECURRENCE:
            next_day(impl);
            break;

        case ICAL_WEEKLY_RECURRENCE:
            next_week(impl);
            break;

        case ICAL_MONTHLY_RECURRENCE:
            next_month(impl);
            break;

        case ICAL_YEARLY_RECURRENCE:
            next_year(impl);
            break;

        default:
            icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
            return icaltime_null_time();
        }

        impl->last = occurrence_as_icaltime(impl, 1);

        /* Ignore times that are after the MAX year,
           or the UNTIL time, or the end time */
        if (impl->last.year > MAX_TIME_T_YEAR ||
            (!icaltime_is_null_time(impl->rule.until) &&
             icaltime_compare(impl->last, impl->rule.until) > 0) ||
            (!icaltime_is_null_time(impl->iend) &&
             icaltime_compare(impl->last, impl->iend) >= 0)) {
            return icaltime_null_time();
        }

    } while (icaltime_compare(impl->last, impl->istart) < 0 ||
             !check_contracting_rules(impl));

    impl->occurrence_no++;

    return impl->last;
}

struct icaltimetype icalrecur_iterator_prev(icalrecur_iterator *impl)
{
    /* Quit if last time is before the DTSTART time */
    if (!impl || icaltime_compare(impl->last, impl->dtstart) < 0) {
        return icaltime_null_time();
    }

#if 0  //  Mostly for testing -- users probably don't want/expect this
    /* If last time is valid, return it */
    if (impl->rule.count != 0 && impl->occurrence_no == impl->rule.count &&
        (icaltime_is_null_time(impl->iend) ||
         icaltime_compare(impl->last, impl->iend) <= 0) &&
        check_contracting_rules(impl)) {

        impl->occurrence_no--;
        return impl->last;
    }
#endif

    /* Iterate until we get the next valid time */
    do {
        switch (impl->rule.freq) {

        case ICAL_SECONDLY_RECURRENCE:
            prev_second(impl);
            break;

        case ICAL_MINUTELY_RECURRENCE:
            prev_minute(impl);
            break;

        case ICAL_HOURLY_RECURRENCE:
            prev_hour(impl);
            break;

        case ICAL_DAILY_RECURRENCE:
            prev_day(impl);
            break;

        case ICAL_WEEKLY_RECURRENCE:
            prev_week(impl);
            break;

        case ICAL_MONTHLY_RECURRENCE:
            prev_month(impl);
            break;

        case ICAL_YEARLY_RECURRENCE:
            prev_year(impl);
            break;

        default:
            icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
            return icaltime_null_time();
        }

        impl->last = occurrence_as_icaltime(impl, 1);

        /* Ignore times that are before the DTSTART time */
        if (icaltime_compare(impl->last, impl->dtstart) < 0 ||
            (!icaltime_is_null_time(impl->istart) &&
             icaltime_compare(impl->last, impl->istart) < 0)) {
            return icaltime_null_time();
        }

    } while (impl->last.year > MAX_TIME_T_YEAR ||
             (!icaltime_is_null_time(impl->rule.until) &&
              icaltime_compare(impl->last, impl->rule.until) > 0) ||
             (!icaltime_is_null_time(impl->iend) &&
              icaltime_compare(impl->last, impl->iend) > 0) ||
             !check_contracting_rules(impl));

    impl->occurrence_no--;

    return impl->last;
}

static int __iterator_set_start(icalrecur_iterator *impl, icaltimetype start)
{
    icalrecurrencetype_frequency freq = impl->rule.freq;
    short interval = impl->rule.interval;
    int diff;

    impl->istart = start;
    impl->occurrence_no = 0;
    impl->days_index = ICAL_YEARDAYS_MASK_SIZE;

    /* Set Gregorian start date */
    set_datetime(impl, start);

    switch (freq) {
    case ICAL_YEARLY_RECURRENCE:
        /* For YEARLY rule, begin by setting up the year days array.
           The YEARLY rules work by expanding one year at a time. */

        if ((interval > 1) &&
            (diff = (impl->istart.year - impl->rstart.year) % interval)) {
            /* Specified start year doesn't match interval -
               bump start to first day of next year that matches interval */
            set_day_of_year(impl, 1);
            increment_year(impl, interval - diff);
        }

        /* Get (adjusted) start date as RSCALE date */
        start = occurrence_as_icaltime(impl, 0);

        /* Expand days array for (adjusted) start year -
           fail after hitting the year 20000 if no expanded days match */
        while (start.year < 20000) {
            expand_year_days(impl, start.year);
            if (icalerrno != ICAL_NO_ERROR) {
                icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
                return 0;
            }
            if (impl->days_index < ICAL_YEARDAYS_MASK_SIZE) {
                break;  /* break when a matching day is found */
            }
            increment_year(impl, interval);
            start = occurrence_as_icaltime(impl, 0);
        }

        /* Copy the first day into last */
        set_day_of_year(impl, impl->days_index);

        break;

    case ICAL_MONTHLY_RECURRENCE:
        /* For MONTHLY rule, begin by setting up the year days array.
           The MONTHLY rules work by expanding one month at a time. */

        if ((interval > 1) &&
            (diff = month_diff(impl, impl->rstart, impl->istart) % interval)) {
            /* Specified month doesn't match interval -
               bump start to first day of next month that matches interval */
            increment_monthday(impl, -impl->istart.day + 1);
            __increment_month(impl, interval - diff);
        }

        /* Get (adjusted) start date as RSCALE date */
        start = occurrence_as_icaltime(impl, 0);

        /* Expand days array for (adjusted) start month -
           fail after hitting the year 20000 if no expanded days match */
        while (start.year < 20000) {
            expand_month_days(impl, start.year, start.month);
            if (impl->days_index < ICAL_YEARDAYS_MASK_SIZE) {
                break;  /* break when a matching day is found */
            }
            increment_month(impl, impl->rule.interval);
            start = occurrence_as_icaltime(impl, 0);
        }

        /* Copy the first day into last */
        set_day_of_year(impl, impl->days_index);

        break;

    case ICAL_WEEKLY_RECURRENCE:
        if (impl->by_ptrs[BY_DAY][0] == ICAL_RECURRENCE_ARRAY_MAX) {
            /* Weekly recurrences with no BY_DAY data should occur on the
               same day of the week as the start time . */
            impl->by_ptrs[BY_DAY][0] = (short)get_day_of_week(impl);

        } else {
            adjust_to_byday(impl);

            /* If start == DTSTART, adjust rstart */
            if (icaltime_compare(start, impl->dtstart) == 0) {
                impl->rstart = occurrence_as_icaltime(impl, 0);
            }

            /* Get (adjusted) start date as RSCALE date */
            start = occurrence_as_icaltime(impl, 0);

            if ((interval > 1) &&
                (diff = (day_diff(impl, impl->rstart, start) + 6) / 7) % interval) {
                /* Specified week doesn't match interval -
                   bump start to next week that matches interval */
                increment_monthday(impl, 7 * (interval - diff));
            }
        }
        break;

    case ICAL_DAILY_RECURRENCE:
        if ((interval > 1) &&
            (diff = day_diff(impl, impl->rstart, impl->istart) % interval)) {
            /* Specified day doesn't match interval -
               bump start to next day that matches interval */
            increment_monthday(impl, interval - diff);
        }
        break;

    case ICAL_HOURLY_RECURRENCE:
        if ((interval > 1) &&
            (diff = abs(impl->istart.hour - impl->rstart.hour) % interval)) {
            /* Specified hour doesn't match interval -
               bump start to next hour that matches interval */
            increment_hour(impl, interval - diff);
        }
        break;

    case ICAL_MINUTELY_RECURRENCE:
        if ((interval > 1) &&
            (diff = abs(impl->istart.minute - impl->rstart.minute) % interval)) {
            /* Specified minute doesn't match interval -
               bump start to next minute that matches interval */
            increment_minute(impl, interval - diff);
        }
        break;

    case ICAL_SECONDLY_RECURRENCE:
        if ((interval > 1) &&
            (diff = abs(impl->istart.second - impl->rstart.second) % interval)) {
            /* Specified second doesn't match interval -
               bump start to next second that matches interval */
            increment_second(impl, interval - diff);
        }
        break;

    default:
        break;
    }

    /* Get start date as Gregorian date */
    impl->last = occurrence_as_icaltime(impl, 1);

    /* Fail if first instance exceeds MAX_TIME_T_YEAR */
    if (impl->last.year > MAX_TIME_T_YEAR) {
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        return 0;
    }

    return 1;
}

int icalrecur_iterator_set_start(icalrecur_iterator *impl,
                                 struct icaltimetype start)
{
    /* We can't adjust start date if we need to count occurrences */
    if (impl->rule.count > 0) {
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        return 0;
    }

    /* Convert start to same time zone as DTSTART */
    start = icaltime_convert_to_zone(start, (icaltimezone *) impl->dtstart.zone);

    if (icaltime_compare(start, impl->dtstart) < 0) {
        /* If start is before DTSTART, use DTSTART */
        start = impl->dtstart;
    }
    else if (!icaltime_is_null_time(impl->rule.until) &&
        icaltime_compare(start, impl->rule.until) > 0) {
        /* If start is after UNTIL, we're done */
        impl->last = start;
        return 1;
    }

    return __iterator_set_start(impl, start);
}

int icalrecur_iterator_set_end(icalrecur_iterator *impl,
                               struct icaltimetype end)
{
    /* Convert end to same time zone as DTSTART */
    end = icaltime_convert_to_zone(end, (icaltimezone *)impl->dtstart.zone);

    impl->iend = end;

    return 1;
}

int icalrecur_iterator_set_range(icalrecur_iterator *impl,
                                 struct icaltimetype from,
                                 struct icaltimetype to)
{
    if (impl->rule.count > 0 || icaltime_is_null_time(from)) {
        /* Can't set a range without 'from' or if we need to count occurrences */
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        return 0;
    }

    if (!icaltime_is_null_time(to) && icaltime_compare(to, from) < 0) {
        /* Setting up for the reverse iterator */
        const icaltimezone *zone = impl->dtstart.zone;

        /* Convert 'from' to same time zone as DTSTART */
        from = icaltime_convert_to_zone(from, (icaltimezone *) zone);

        if (icaltime_compare(from, impl->rule.until) > 0) {
            /* If 'from' is after UNTIL, use UNTIL */
            from = impl->rule.until;
        } else if (icaltime_compare(from, impl->dtstart) < 0) {
            /* If 'from' is before START, we're done */
            impl->last = from;
            return 1;
        }

        if (!__iterator_set_start(impl, from))
            return 0;

        /* __iterator_set_start() may back us up earlier than 'from'
           Iterate forward until we are later than 'from'.
        */
        while (icaltime_compare(impl->last, from) < 0) {
            (void)icalrecur_iterator_next(impl);
        }

        /* Convert 'to' to same time zone as DTSTART */
        to = icaltime_convert_to_zone(to, (icaltimezone *) zone);

        if (icaltime_compare(to, impl->dtstart) < 0) {
            /* If 'to' is before DTSTART, use DTSTART */
            to = impl->dtstart;
        }

        impl->istart = to;
        impl->iend = from;
        impl->days_index = 0;
    } else {
        if (!icalrecur_iterator_set_start(impl, from))
            return 0;

        icalrecur_iterator_set_end(impl, to);
    }

    return 1;
}

/************************** Type Routines **********************/

void icalrecurrencetype_clear(struct icalrecurrencetype *recur)
{
    memset(recur,
           ICAL_RECURRENCE_ARRAY_MAX_BYTE, sizeof(struct icalrecurrencetype));

    recur->week_start = ICAL_MONDAY_WEEKDAY;
    recur->freq = ICAL_NO_RECURRENCE;
    recur->interval = 1;
    recur->until = icaltime_null_time();
    recur->count = 0;
    recur->rscale = NULL;
    recur->skip = ICAL_SKIP_OMIT;
}

enum icalrecurrencetype_weekday icalrecurrencetype_day_day_of_week(short day)
{
    return abs(day) % 8;
}

int icalrecurrencetype_day_position(short day)
{
    int wd, pos;

    wd = icalrecurrencetype_day_day_of_week(day);

    pos = (abs(day) - wd) / 8 * ((day < 0) ? -1 : 1);

    return pos;
}

short icalrecurrencetype_encode_day(enum icalrecurrencetype_weekday weekday, int position)
{
    return (weekday + (8 * abs(position))) * ((position < 0) ? -1 : 1);
}

int icalrecurrencetype_month_is_leap(short month)
{
    return (month & LEAP_MONTH);
}

int icalrecurrencetype_month_month(short month)
{
    return (month & ~LEAP_MONTH);
}

short icalrecurrencetype_encode_month(int month, int is_leap)
{
    return month | (is_leap ? LEAP_MONTH : 0);
}

int icalrecur_expand_recurrence(const char *rule,
    icaltime_t start, int count, icaltime_t *array)
{
    struct icalrecurrencetype recur;
    icalrecur_iterator *ritr;
    icaltime_t tt;
    struct icaltimetype icstart, next;
    int i = 0;

    memset(array, 0, count * sizeof(icaltime_t));

    icstart = icaltime_from_timet_with_zone(start, 0, 0);

    recur = icalrecurrencetype_from_string(rule);
    ritr = icalrecur_iterator_new(recur, icstart);
    if (ritr) {
        for (next = icalrecur_iterator_next(ritr);
             !icaltime_is_null_time(next) && i < count;
             next = icalrecur_iterator_next(ritr)) {

            tt = icaltime_as_timet(next);

            if (tt >= start) {
                array[i++] = tt;
            }
        }
        icalrecur_iterator_free(ritr);
    }
    if(recur.rscale)
        icalmemory_free_buffer(recur.rscale);

    return 1;
}

ical_invalid_rrule_handling ical_get_invalid_rrule_handling_setting(void)
{
    ical_invalid_rrule_handling myHandling;

#if defined(HAVE_PTHREAD)
    pthread_mutex_lock(&invalid_rrule_mutex);
#endif

    myHandling = invalidRruleHandling;

#if defined(HAVE_PTHREAD)
    pthread_mutex_unlock(&invalid_rrule_mutex);
#endif

    return myHandling;
}

void ical_set_invalid_rrule_handling_setting(ical_invalid_rrule_handling newSetting)
{
#if defined(HAVE_PTHREAD)
    pthread_mutex_lock(&invalid_rrule_mutex);
#endif

    invalidRruleHandling = newSetting;

#if defined(HAVE_PTHREAD)
    pthread_mutex_unlock(&invalid_rrule_mutex);
#endif
}
