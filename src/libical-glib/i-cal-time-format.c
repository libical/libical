/*
 * Copyright 2021 Collabora, Ltd. (https://collabora.com)
 * Copyright 2021 Corentin Noël <corentin.noel@collabora.com>
 * Copyright 2009-2010 Christian Hergert <chris@dronelabs.com>
 * Copyright 2010 Thiago Santos <thiago.sousa.santos@collabora.com>
 * Copyright 2010 Emmanuele Bassi <ebassi@linux.intel.com>
 * Copyright 2010 Codethink Limited
 * Copyright 2018 Tomasz Miąsko
 *
 * This library is free software: you can redistribute it and/or modify it
 * under the terms of version 2.1. of the GNU Lesser General Public License
 * as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library. If not, see <https://www.gnu.org/licenses/>.
 *
 * Adapted from gdatetime.c in GLib
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "i-cal-time-format.h"
#include <libical-glib/i-cal-timezone.h>

/* langinfo.h in glibc 2.27 defines ALTMON_* only if _GNU_SOURCE is defined.  */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#include <math.h>
#include <stdlib.h>
#include <string.h>
#define GETTEXT_PACKAGE "glib20"
#include <glib/gi18n-lib.h>

#ifdef HAVE_LANGINFO_TIME
#include <langinfo.h>
#endif

#ifndef G_OS_WIN32
#include <sys/time.h>
#include <time.h>
#else
#if defined (_MSC_VER) && (_MSC_VER < 1800)
/* fallback implementation for isnan() on VS2012 and earlier */
#define isnan _isnan
#endif
#endif /* !G_OS_WIN32 */

/* Time conversion {{{1 */

#define UNIX_EPOCH_START     719163
#define INSTANT_TO_UNIX(instant) \
  ((instant)/USEC_PER_SECOND - UNIX_EPOCH_START * SEC_PER_DAY)
#define INSTANT_TO_UNIX_USECS(instant) \
  ((instant) - UNIX_EPOCH_START * SEC_PER_DAY * USEC_PER_SECOND)
#define UNIX_TO_INSTANT(unix) \
  (((gint64) (unix) + UNIX_EPOCH_START * SEC_PER_DAY) * USEC_PER_SECOND)
#define UNIX_USECS_TO_INSTANT(unix_usecs) \
  ((gint64) (unix_usecs) + UNIX_EPOCH_START * SEC_PER_DAY * USEC_PER_SECOND)
#define UNIX_TO_INSTANT_IS_VALID(unix) \
  ((gint64) (unix) <= INSTANT_TO_UNIX (G_MAXINT64))
#define UNIX_USECS_TO_INSTANT_IS_VALID(unix_usecs) \
  ((gint64) (unix_usecs) <= INSTANT_TO_UNIX_USECS (G_MAXINT64))

#define DAYS_IN_4YEARS    1461    /* days in 4 years */
#define DAYS_IN_100YEARS  36524   /* days in 100 years */
#define DAYS_IN_400YEARS  146097  /* days in 400 years  */

#define USEC_PER_SECOND      (G_GINT64_CONSTANT (1000000))
#define USEC_PER_MINUTE      (G_GINT64_CONSTANT (60000000))
#define USEC_PER_HOUR        (G_GINT64_CONSTANT (3600000000))
#define USEC_PER_MILLISECOND (G_GINT64_CONSTANT (1000))
#define USEC_PER_DAY         (G_GINT64_CONSTANT (86400000000))
#define SEC_PER_DAY          (G_GINT64_CONSTANT (86400))

#define SECS_PER_MINUTE (60)
#define SECS_PER_HOUR   (60 * SECS_PER_MINUTE)
#define SECS_PER_DAY    (24 * SECS_PER_HOUR)
#define SECS_PER_YEAR   (365 * SECS_PER_DAY)
#define SECS_PER_JULIAN (DAYS_PER_PERIOD * SECS_PER_DAY)

#define GREGORIAN_LEAP(y)    ((((y) % 4) == 0) && (!((((y) % 100) == 0) && (((y) % 400) != 0))))
#define JULIAN_YEAR(d)       ((d)->julian / 365.25)
#define DAYS_PER_PERIOD      (G_GINT64_CONSTANT (2914695))

static const guint16 days_in_months[2][13] =
{
  { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
  { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

static const guint16 days_in_year[2][13] =
{
  {  0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
  {  0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};

#ifdef HAVE_LANGINFO_TIME

#define GET_AMPM(d) ((i_cal_time_get_hour (d) < 12) ? \
                     nl_langinfo (AM_STR) : \
                     nl_langinfo (PM_STR))
#define GET_AMPM_IS_LOCALE TRUE

#define PREFERRED_DATE_TIME_FMT nl_langinfo (D_T_FMT)
#define PREFERRED_DATE_FMT nl_langinfo (D_FMT)
#define PREFERRED_TIME_FMT nl_langinfo (T_FMT)
#define PREFERRED_12HR_TIME_FMT nl_langinfo (T_FMT_AMPM)

static const gint weekday_item[2][7] =
{
  { ABDAY_1, ABDAY_2, ABDAY_3, ABDAY_4, ABDAY_5, ABDAY_6, ABDAY_7 },
  { DAY_1, DAY_2, DAY_3, DAY_4, DAY_5, DAY_6, DAY_7 }
};

static const gint month_item[2][12] =
{
  { ABMON_1, ABMON_2, ABMON_3, ABMON_4, ABMON_5, ABMON_6, ABMON_7, ABMON_8, ABMON_9, ABMON_10, ABMON_11, ABMON_12 },
  { MON_1, MON_2, MON_3, MON_4, MON_5, MON_6, MON_7, MON_8, MON_9, MON_10, MON_11, MON_12 },
};

#define WEEKDAY_ABBR(d) nl_langinfo (weekday_item[0][i_cal_time_day_of_week (d) - 1])
#define WEEKDAY_ABBR_IS_LOCALE TRUE
#define WEEKDAY_FULL(d) nl_langinfo (weekday_item[1][i_cal_time_day_of_week (d) - 1])
#define WEEKDAY_FULL_IS_LOCALE TRUE
#define MONTH_ABBR(d) nl_langinfo (month_item[0][i_cal_time_get_month (d) - 1])
#define MONTH_ABBR_IS_LOCALE TRUE
#define MONTH_FULL(d) nl_langinfo (month_item[1][i_cal_time_get_month (d) - 1])
#define MONTH_FULL_IS_LOCALE TRUE

#else

#define GET_AMPM(d)          (get_fallback_ampm (i_cal_time_get_hour (d)))
#define GET_AMPM_IS_LOCALE   FALSE

/* Translators: this is the preferred format for expressing the date and the time */
#define PREFERRED_DATE_TIME_FMT C_("GDateTime", "%a %b %e %H:%M:%S %Y")

/* Translators: this is the preferred format for expressing the date */
#define PREFERRED_DATE_FMT C_("GDateTime", "%m/%d/%y")

/* Translators: this is the preferred format for expressing the time */
#define PREFERRED_TIME_FMT C_("GDateTime", "%H:%M:%S")

/* Translators: this is the preferred format for expressing 12 hour time */
#define PREFERRED_12HR_TIME_FMT C_("GDateTime", "%I:%M:%S %p")

#define WEEKDAY_ABBR(d)       (get_weekday_name_abbr (i_cal_time_day_of_week (d)))
#define WEEKDAY_ABBR_IS_LOCALE FALSE
#define WEEKDAY_FULL(d)       (get_weekday_name (i_cal_time_day_of_week (d)))
#define WEEKDAY_FULL_IS_LOCALE FALSE
/* We don't yet know if nl_langinfo (MON_n) returns standalone or complete-date
 * format forms but if nl_langinfo (ALTMON_n) is not supported then we will
 * have to use MONTH_FULL as standalone.  The same if nl_langinfo () does not
 * exist at all.  MONTH_ABBR is similar: if nl_langinfo (_NL_ABALTMON_n) is not
 * supported then we will use MONTH_ABBR as standalone.
 */
#define MONTH_ABBR(d)         (get_month_name_abbr_standalone (i_cal_time_get_month (d)))
#define MONTH_ABBR_IS_LOCALE  FALSE
#define MONTH_FULL(d)         (get_month_name_standalone (i_cal_time_get_month (d)))
#define MONTH_FULL_IS_LOCALE  FALSE

static const gchar *
get_month_name_standalone (gint month)
{
  switch (month)
    {
    case 1:
      /* Translators: Some languages (Baltic, Slavic, Greek, and some more)
       * need different grammatical forms of month names depending on whether
       * they are standalone or in a complete date context, with the day
       * number.  Some other languages may prefer starting with uppercase when
       * they are standalone and with lowercase when they are in a complete
       * date context.  Here are full month names in a form appropriate when
       * they are used standalone.  If your system is Linux with the glibc
       * version 2.27 (released Feb 1, 2018) or newer or if it is from the BSD
       * family (which includes OS X) then you can refer to the date command
       * line utility and see what the command `date +%OB' produces.  Also in
       * the latest Linux the command `locale alt_mon' in your native locale
       * produces a complete list of month names almost ready to copy and
       * paste here.  Note that in most of the languages (western European,
       * non-European) there is no difference between the standalone and
       * complete date form.
       */
      return C_("full month name", "January");
    case 2:
      return C_("full month name", "February");
    case 3:
      return C_("full month name", "March");
    case 4:
      return C_("full month name", "April");
    case 5:
      return C_("full month name", "May");
    case 6:
      return C_("full month name", "June");
    case 7:
      return C_("full month name", "July");
    case 8:
      return C_("full month name", "August");
    case 9:
      return C_("full month name", "September");
    case 10:
      return C_("full month name", "October");
    case 11:
      return C_("full month name", "November");
    case 12:
      return C_("full month name", "December");

    default:
      g_warning ("Invalid month number %d", month);
    }

  return NULL;
}

static const gchar *
get_month_name_abbr_standalone (gint month)
{
  switch (month)
    {
    case 1:
      /* Translators: Some languages need different grammatical forms of
       * month names depending on whether they are standalone or in a complete
       * date context, with the day number.  Some may prefer starting with
       * uppercase when they are standalone and with lowercase when they are
       * in a full date context.  However, as these names are abbreviated
       * the grammatical difference is visible probably only in Belarusian
       * and Russian.  In other languages there is no difference between
       * the standalone and complete date form when they are abbreviated.
       * If your system is Linux with the glibc version 2.27 (released
       * Feb 1, 2018) or newer then you can refer to the date command line
       * utility and see what the command `date +%Ob' produces.  Also in
       * the latest Linux the command `locale ab_alt_mon' in your native
       * locale produces a complete list of month names almost ready to copy
       * and paste here.  Note that this feature is not yet supported by any
       * other platform.  Here are abbreviated month names in a form
       * appropriate when they are used standalone.
       */
      return C_("abbreviated month name", "Jan");
    case 2:
      return C_("abbreviated month name", "Feb");
    case 3:
      return C_("abbreviated month name", "Mar");
    case 4:
      return C_("abbreviated month name", "Apr");
    case 5:
      return C_("abbreviated month name", "May");
    case 6:
      return C_("abbreviated month name", "Jun");
    case 7:
      return C_("abbreviated month name", "Jul");
    case 8:
      return C_("abbreviated month name", "Aug");
    case 9:
      return C_("abbreviated month name", "Sep");
    case 10:
      return C_("abbreviated month name", "Oct");
    case 11:
      return C_("abbreviated month name", "Nov");
    case 12:
      return C_("abbreviated month name", "Dec");

    default:
      g_warning ("Invalid month number %d", month);
    }

  return NULL;
}

static const gchar *
get_weekday_name (gint day)
{
  switch (day)
    {
    case 1:
      return C_("full weekday name", "Sunday");
    case 2:
      return C_("full weekday name", "Monday");
    case 3:
      return C_("full weekday name", "Tuesday");
    case 4:
      return C_("full weekday name", "Wednesday");
    case 5:
      return C_("full weekday name", "Thursday");
    case 6:
      return C_("full weekday name", "Friday");
    case 7:
      return C_("full weekday name", "Saturday");

    default:
      g_warning ("Invalid week day number %d", day);
    }

  return NULL;
}

static const gchar *
get_weekday_name_abbr (gint day)
{
  switch (day)
    {
    case 1:
      return C_("abbreviated weekday name", "Sun");
    case 2:
      return C_("abbreviated weekday name", "Mon");
    case 3:
      return C_("abbreviated weekday name", "Tue");
    case 4:
      return C_("abbreviated weekday name", "Wed");
    case 5:
      return C_("abbreviated weekday name", "Thu");
    case 6:
      return C_("abbreviated weekday name", "Fri");
    case 7:
      return C_("abbreviated weekday name", "Sat");

    default:
      g_warning ("Invalid week day number %d", day);
    }

  return NULL;
}

#endif  /* HAVE_LANGINFO_TIME */

#ifdef HAVE_LANGINFO_ALTMON

/* If nl_langinfo () supports ALTMON_n then MON_n returns full date format
 * forms and ALTMON_n returns standalone forms.
 */

#define MONTH_FULL_WITH_DAY(d) MONTH_FULL(d)
#define MONTH_FULL_WITH_DAY_IS_LOCALE MONTH_FULL_IS_LOCALE

static const gint alt_month_item[12] =
{
  ALTMON_1, ALTMON_2, ALTMON_3, ALTMON_4, ALTMON_5, ALTMON_6,
  ALTMON_7, ALTMON_8, ALTMON_9, ALTMON_10, ALTMON_11, ALTMON_12
};

#define MONTH_FULL_STANDALONE(d) nl_langinfo (alt_month_item[i_cal_time_get_month (d) - 1])
#define MONTH_FULL_STANDALONE_IS_LOCALE TRUE

#else

/* If nl_langinfo () does not support ALTMON_n then either MON_n returns
 * standalone forms or nl_langinfo (MON_n) does not work so we have defined
 * it as standalone form.
 */

#define MONTH_FULL_STANDALONE(d) MONTH_FULL(d)
#define MONTH_FULL_STANDALONE_IS_LOCALE MONTH_FULL_IS_LOCALE
#define MONTH_FULL_WITH_DAY(d) (get_month_name_with_day (i_cal_time_get_month (d)))
#define MONTH_FULL_WITH_DAY_IS_LOCALE FALSE

static const gchar *
get_month_name_with_day (gint month)
{
  switch (month)
    {
    case 1:
      /* Translators: Some languages need different grammatical forms of
       * month names depending on whether they are standalone or in a full
       * date context, with the day number.  Some may prefer starting with
       * uppercase when they are standalone and with lowercase when they are
       * in a full date context.  Here are full month names in a form
       * appropriate when they are used in a full date context, with the
       * day number.  If your system is Linux with the glibc version 2.27
       * (released Feb 1, 2018) or newer or if it is from the BSD family
       * (which includes OS X) then you can refer to the date command line
       * utility and see what the command `date +%B' produces.  Also in
       * the latest Linux the command `locale mon' in your native locale
       * produces a complete list of month names almost ready to copy and
       * paste here.  In older Linux systems due to a bug the result is
       * incorrect in some languages.  Note that in most of the languages
       * (western European, non-European) there is no difference between the
       * standalone and complete date form.
       */
      return C_("full month name with day", "January");
    case 2:
      return C_("full month name with day", "February");
    case 3:
      return C_("full month name with day", "March");
    case 4:
      return C_("full month name with day", "April");
    case 5:
      return C_("full month name with day", "May");
    case 6:
      return C_("full month name with day", "June");
    case 7:
      return C_("full month name with day", "July");
    case 8:
      return C_("full month name with day", "August");
    case 9:
      return C_("full month name with day", "September");
    case 10:
      return C_("full month name with day", "October");
    case 11:
      return C_("full month name with day", "November");
    case 12:
      return C_("full month name with day", "December");

    default:
      g_warning ("Invalid month number %d", month);
    }

  return NULL;
}

#endif  /* HAVE_LANGINFO_ALTMON */

#ifdef HAVE_LANGINFO_ABALTMON

/* If nl_langinfo () supports _NL_ABALTMON_n then ABMON_n returns full
 * date format forms and _NL_ABALTMON_n returns standalone forms.
 */

#define MONTH_ABBR_WITH_DAY(d) MONTH_ABBR(d)
#define MONTH_ABBR_WITH_DAY_IS_LOCALE MONTH_ABBR_IS_LOCALE

static const gint ab_alt_month_item[12] =
{
  _NL_ABALTMON_1, _NL_ABALTMON_2, _NL_ABALTMON_3, _NL_ABALTMON_4,
  _NL_ABALTMON_5, _NL_ABALTMON_6, _NL_ABALTMON_7, _NL_ABALTMON_8,
  _NL_ABALTMON_9, _NL_ABALTMON_10, _NL_ABALTMON_11, _NL_ABALTMON_12
};

#define MONTH_ABBR_STANDALONE(d) nl_langinfo (ab_alt_month_item[i_cal_time_get_month (d) - 1])
#define MONTH_ABBR_STANDALONE_IS_LOCALE TRUE

#else

/* If nl_langinfo () does not support _NL_ABALTMON_n then either ABMON_n
 * returns standalone forms or nl_langinfo (ABMON_n) does not work so we
 * have defined it as standalone form. Now it's time to swap.
 */

#define MONTH_ABBR_STANDALONE(d) MONTH_ABBR(d)
#define MONTH_ABBR_STANDALONE_IS_LOCALE MONTH_ABBR_IS_LOCALE
#define MONTH_ABBR_WITH_DAY(d) (get_month_name_abbr_with_day (i_cal_time_get_month (d)))
#define MONTH_ABBR_WITH_DAY_IS_LOCALE FALSE

static const gchar *
get_month_name_abbr_with_day (gint month)
{
  switch (month)
    {
    case 1:
      /* Translators: Some languages need different grammatical forms of
       * month names depending on whether they are standalone or in a full
       * date context, with the day number.  Some may prefer starting with
       * uppercase when they are standalone and with lowercase when they are
       * in a full date context.  Here are abbreviated month names in a form
       * appropriate when they are used in a full date context, with the
       * day number.  However, as these names are abbreviated the grammatical
       * difference is visible probably only in Belarusian and Russian.
       * In other languages there is no difference between the standalone
       * and complete date form when they are abbreviated.  If your system
       * is Linux with the glibc version 2.27 (released Feb 1, 2018) or newer
       * then you can refer to the date command line utility and see what the
       * command `date +%b' produces.  Also in the latest Linux the command
       * `locale abmon' in your native locale produces a complete list of
       * month names almost ready to copy and paste here.  In other systems
       * due to a bug the result is incorrect in some languages.
       */
      return C_("abbreviated month name with day", "Jan");
    case 2:
      return C_("abbreviated month name with day", "Feb");
    case 3:
      return C_("abbreviated month name with day", "Mar");
    case 4:
      return C_("abbreviated month name with day", "Apr");
    case 5:
      return C_("abbreviated month name with day", "May");
    case 6:
      return C_("abbreviated month name with day", "Jun");
    case 7:
      return C_("abbreviated month name with day", "Jul");
    case 8:
      return C_("abbreviated month name with day", "Aug");
    case 9:
      return C_("abbreviated month name with day", "Sep");
    case 10:
      return C_("abbreviated month name with day", "Oct");
    case 11:
      return C_("abbreviated month name with day", "Nov");
    case 12:
      return C_("abbreviated month name with day", "Dec");

    default:
      g_warning ("Invalid month number %d", month);
    }

  return NULL;
}

#endif  /* HAVE_LANGINFO_ABALTMON */

static gint
i_cal_time_day_of_week_monday (const ICalTime *tt)
{
  gint weekday = i_cal_time_day_of_week (tt);
  if (weekday != 1)
    return weekday-1;

  return 7;
}

/*
 * i_cal_time_get_week_numbering_year:
 * @tt: an #ICalTime
 *
 * Returns the ISO 8601 week-numbering year in which the week containing
 * @tt falls.
 *
 * This function, taken together with i_cal_time_get_week_of_year() and
 * i_cal_time_get_day_of_week() can be used to determine the full ISO
 * week date on which @tt falls.
 *
 * This is usually equal to the normal Gregorian year (as returned by
 * i_cal_time_get_year()), except as detailed below:
 *
 * For Thursday, the week-numbering year is always equal to the usual
 * calendar year.  For other days, the number is such that every day
 * within a complete week (Monday to Sunday) is contained within the
 * same week-numbering year.
 *
 * For Monday, Tuesday and Wednesday occurring near the end of the year,
 * this may mean that the week-numbering year is one greater than the
 * calendar year (so that these days have the same week-numbering year
 * as the Thursday occurring early in the next year).
 *
 * For Friday, Saturday and Sunday occurring near the start of the year,
 * this may mean that the week-numbering year is one less than the
 * calendar year (so that these days have the same week-numbering year
 * as the Thursday occurring late in the previous year).
 *
 * An equivalent description is that the week-numbering year is equal to
 * the calendar year containing the majority of the days in the current
 * week (Monday to Sunday).
 *
 * Note that January 1 0001 in the proleptic Gregorian calendar is a
 * Monday, so this function never returns 0.
 *
 * Returns: the ISO 8601 week-numbering year for @tt
 **/
static gint
i_cal_time_get_week_numbering_year (const ICalTime *tt)
{
  gint year, month, day, weekday;

  i_cal_time_get_date (tt, &year, &month, &day);
  /* The calculations are easier with 1=monday … 7=sunday
   */
  weekday = i_cal_time_day_of_week_monday (tt);

  /* January 1, 2, 3 might be in the previous year if they occur after
   * Thursday.
   *
   *   Jan 1:  Friday, Saturday, Sunday    =>  day 1:  weekday 5, 6, 7
   *   Jan 2:  Saturday, Sunday            =>  day 2:  weekday 6, 7
   *   Jan 3:  Sunday                      =>  day 3:  weekday 7
   *
   * So we have a special case if (day - weekday) <= -4
   */
  if (month == 1 && (day - weekday) <= -4)
    return year - 1;

  /* December 29, 30, 31 might be in the next year if they occur before
   * Thursday.
   *
   *   Dec 31: Monday, Tuesday, Wednesday  =>  day 31: weekday 1, 2, 3
   *   Dec 30: Monday, Tuesday             =>  day 30: weekday 1, 2
   *   Dec 29: Monday                      =>  day 29: weekday 1
   *
   * So we have a special case if (day - weekday) >= 28
   */
  else if (month == 12 && (day - weekday) >= 28)
    return year + 1;

  else
    return year;
}

/*
 * i_cal_time_get_week_of_year:
 * @tt: a #ICalTime
 *
 * Returns the ISO 8601 week number for the week containing @datetime.
 * The ISO 8601 week number is the same for every day of the week (from
 * Moday through Sunday).  That can produce some unusual results
 * (described below).
 *
 * The first week of the year is week 1.  This is the week that contains
 * the first Thursday of the year.  Equivalently, this is the first week
 * that has more than 4 of its days falling within the calendar year.
 *
 * The value 0 is never returned by this function.  Days contained
 * within a year but occurring before the first ISO 8601 week of that
 * year are considered as being contained in the last week of the
 * previous year.  Similarly, the final days of a calendar year may be
 * considered as being part of the first ISO 8601 week of the next year
 * if 4 or more days of that week are contained within the new year.
 *
 * Returns: the ISO 8601 week number for @datetime.
 */
static gint
i_cal_time_get_week_of_year (const ICalTime *tt)
{
  gint a, b, c, d, e, f, g, n, s, month, day, year, weeknum;

  g_return_val_if_fail (tt != NULL, 0);

  i_cal_time_get_date (tt, &year, &month, &day);

  if (month <= 2)
    {
      a = i_cal_time_get_year (tt) - 1;
      b = (a / 4) - (a / 100) + (a / 400);
      c = ((a - 1) / 4) - ((a - 1) / 100) + ((a - 1) / 400);
      s = b - c;
      e = 0;
      f = day - 1 + (31 * (month - 1));
    }
  else
    {
      a = year;
      b = (a / 4) - (a / 100) + (a / 400);
      c = ((a - 1) / 4) - ((a - 1) / 100) + ((a - 1) / 400);
      s = b - c;
      e = s + 1;
      f = day + (((153 * (month - 3)) + 2) / 5) + 58 + s;
    }

  g = (a + b) % 7;
  d = (f + g - e) % 7;
  n = f + 3 - d;

  if (n < 0)
    weeknum = 53 - ((g - s) / 5);
  else if (n > 364 + s)
    weeknum = 1;
  else
    weeknum = (n / 7) + 1;

  return weeknum;
}

/* Format AM/PM indicator if the locale does not have a localized version. */
static const gchar *
get_fallback_ampm (gint hour)
{
  if (hour < 12)
    /* Translators: 'before midday' indicator */
    return C_("GDateTime", "AM");
  else
    /* Translators: 'after midday' indicator */
    return C_("GDateTime", "PM");
}

static inline gint
ymd_to_days (gint year,
             gint month,
             gint day)
{
  gint64 days;

  days = ((gint64) year - 1) * 365 + ((year - 1) / 4) - ((year - 1) / 100)
      + ((year - 1) / 400);

  days += days_in_year[0][month - 1];
  if (GREGORIAN_LEAP (year) && month > 2)
    day++;

  days += day;

  return days;
}

static gboolean
format_z (GString *outstr,
          gint     offset,
          guint    colons)
{
  gint hours;
  gint minutes;
  gint seconds;
  gchar sign = offset >= 0 ? '+' : '-';

  offset = ABS (offset);
  hours = offset / 3600;
  minutes = offset / 60 % 60;
  seconds = offset % 60;

  switch (colons)
    {
    case 0:
      g_string_append_printf (outstr, "%c%02d%02d",
                              sign,
                              hours,
                              minutes);
      break;

    case 1:
      g_string_append_printf (outstr, "%c%02d:%02d",
                              sign,
                              hours,
                              minutes);
      break;

    case 2:
      g_string_append_printf (outstr, "%c%02d:%02d:%02d",
                              sign,
                              hours,
                              minutes,
                              seconds);
      break;

    case 3:
      g_string_append_printf (outstr, "%c%02d", sign, hours);

      if (minutes != 0 || seconds != 0)
        {
          g_string_append_printf (outstr, ":%02d", minutes);

          if (seconds != 0)
            g_string_append_printf (outstr, ":%02d", seconds);
        }
      break;

    default:
      return FALSE;
    }

  return TRUE;
}

#ifdef HAVE_LANGINFO_OUTDIGIT
/* Initializes the array with UTF-8 encoded alternate digits suitable for use
 * in current locale. Returns NULL when current locale does not use alternate
 * digits or there was an error converting them to UTF-8.
 */
static const gchar * const *
initialize_alt_digits (void)
{
  guint i;
  gsize digit_len;
  gchar *digit;
  const gchar *locale_digit;
#define N_DIGITS 10
#define MAX_UTF8_ENCODING_LEN 4
  static gchar buffer[N_DIGITS * (MAX_UTF8_ENCODING_LEN + 1 /* null separator */)];
#undef N_DIGITS
#undef MAX_UTF8_ENCODING_LEN
  gchar *buffer_end = buffer;
  static const gchar *alt_digits[10];

  for (i = 0; i != 10; ++i)
    {
      locale_digit = nl_langinfo (_NL_CTYPE_OUTDIGIT0_MB + i);

      if (g_strcmp0 (locale_digit, "") == 0)
        return NULL;

      digit = _g_ctype_locale_to_utf8 (locale_digit, -1, NULL, &digit_len, NULL);
      if (digit == NULL)
        return NULL;

      g_assert (digit_len < (gsize) (buffer + sizeof (buffer) - buffer_end));

      alt_digits[i] = buffer_end;
      buffer_end = g_stpcpy (buffer_end, digit);
      /* skip trailing null byte */
      buffer_end += 1;

      g_free (digit);
    }

  return alt_digits;
}
#endif /* HAVE_LANGINFO_OUTDIGIT */

static void
format_number (GString     *str,
               gboolean     use_alt_digits,
               const gchar *pad,
               gint         width,
               guint32      number)
{
  const gchar *ascii_digits[10] = {
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"
  };
  const gchar * const *digits = ascii_digits;
  const gchar *tmp[10];
  gint i = 0;

  g_return_if_fail (width <= 10);

#ifdef HAVE_LANGINFO_OUTDIGIT
  if (use_alt_digits)
    {
      static const gchar * const *alt_digits = NULL;
      static gsize initialised;

      if G_UNLIKELY (g_once_init_enter (&initialised))
        {
          alt_digits = initialize_alt_digits ();

          if (alt_digits == NULL)
            alt_digits = ascii_digits;

          g_once_init_leave (&initialised, TRUE);
        }

      digits = alt_digits;
    }
#endif /* HAVE_LANGINFO_OUTDIGIT */

  do
    {
      tmp[i++] = digits[number % 10];
      number /= 10;
    }
  while (number);

  while (pad && i < width)
    tmp[i++] = *pad == '0' ? digits[0] : pad;

  /* should really be impossible */
  g_assert (i <= 10);

  while (i)
    g_string_append (str, tmp[--i]);
}

static gboolean
format_ampm (const ICalTime *tt,
             GString        *outstr,
             gboolean        locale_is_utf8,
             gboolean        uppercase)
{
  const gchar *ampm;
  gchar       *tmp = NULL, *ampm_dup;

  ampm = GET_AMPM (tt);

  if (!ampm || ampm[0] == '\0')
    ampm = get_fallback_ampm (i_cal_time_get_hour (tt));

  if (!locale_is_utf8 && GET_AMPM_IS_LOCALE)
    {
      /* This assumes that locale encoding can't have embedded NULs */
      ampm = tmp = g_locale_to_utf8 (ampm, -1, NULL, NULL, NULL);
      if (tmp == NULL)
        return FALSE;
    }
  if (uppercase)
    ampm_dup = g_utf8_strup (ampm, -1);
  else
    ampm_dup = g_utf8_strdown (ampm, -1);
  g_free (tmp);

  g_string_append (outstr, ampm_dup);
  g_free (ampm_dup);

  return TRUE;
}

static gboolean i_cal_time_format_utf8 (const ICalTime *tt,
					const gchar    *format,
					GString        *outstr,
					gboolean        locale_is_utf8);

/* i_cal_time_format() subroutine that takes a locale-encoded format
 * string and produces a UTF-8 encoded date/time string.
 */
static gboolean
i_cal_time_format_locale (const ICalTime *tt,
			  const gchar    *locale_format,
			  GString        *outstr,
			  gboolean        locale_is_utf8)
{
  gchar *utf8_format;
  gboolean success;

  if (locale_is_utf8)
    return i_cal_time_format_utf8 (tt, locale_format, outstr, locale_is_utf8);

  utf8_format = g_locale_to_utf8 (locale_format, -1, NULL, NULL, NULL);
  if (utf8_format == NULL)
    return FALSE;

  success = i_cal_time_format_utf8 (tt, utf8_format, outstr, locale_is_utf8);
  g_free (utf8_format);
  return success;
}

static inline gboolean
string_append (GString     *string,
               const gchar *s,
               gboolean     s_is_utf8)
{
  gchar *utf8;
  gsize  utf8_len;

  if (s_is_utf8)
    {
      g_string_append (string, s);
    }
  else
    {
      utf8 = g_locale_to_utf8 (s, -1, NULL, &utf8_len, NULL);
      if (utf8 == NULL)
        return FALSE;
      g_string_append_len (string, utf8, utf8_len);
      g_free (utf8);
    }

  return TRUE;
}

/* i_cal_time_format() subroutine that takes a UTF-8 encoded format
 * string and produces a UTF-8 encoded date/time string.
 */
static gboolean
i_cal_time_format_utf8 (const ICalTime *tt,
			const gchar    *utf8_format,
			GString        *outstr,
			gboolean        locale_is_utf8)
{
  guint     len;
  guint     colons;
  gunichar  c;
  gboolean  alt_digits = FALSE;
  gboolean  pad_set = FALSE;
  gboolean  name_is_utf8;
  const gchar *pad = "";
  const gchar *name;
  const gchar *tz;

  while (*utf8_format)
    {
      len = strcspn (utf8_format, "%");
      if (len)
        g_string_append_len (outstr, utf8_format, len);

      utf8_format += len;
      if (!*utf8_format)
	break;

      g_assert (*utf8_format == '%');
      utf8_format++;
      if (!*utf8_format)
	break;

      colons = 0;
      alt_digits = FALSE;
      pad_set = FALSE;

    next_mod:
      c = g_utf8_get_char (utf8_format);
      utf8_format = g_utf8_next_char (utf8_format);
      switch (c)
	{
	case 'a':
	  name = WEEKDAY_ABBR (tt);
          if (g_strcmp0 (name, "") == 0)
            return FALSE;

          name_is_utf8 = locale_is_utf8 || !WEEKDAY_ABBR_IS_LOCALE;

          if (!string_append (outstr, name, name_is_utf8))
            return FALSE;

	  break;
	case 'A':
	  name = WEEKDAY_FULL (tt);
          if (g_strcmp0 (name, "") == 0)
            return FALSE;

          name_is_utf8 = locale_is_utf8 || !WEEKDAY_FULL_IS_LOCALE;

          if (!string_append (outstr, name, name_is_utf8))
            return FALSE;

	  break;
	case 'b':
	  name = alt_digits ? MONTH_ABBR_STANDALONE (tt)
			    : MONTH_ABBR_WITH_DAY (tt);
          if (g_strcmp0 (name, "") == 0)
            return FALSE;

          name_is_utf8 = locale_is_utf8 ||
            ((alt_digits && !MONTH_ABBR_STANDALONE_IS_LOCALE) ||
             (!alt_digits && !MONTH_ABBR_WITH_DAY_IS_LOCALE));

          if (!string_append (outstr, name, name_is_utf8))
            return FALSE;

	  break;
	case 'B':
	  name = alt_digits ? MONTH_FULL_STANDALONE (tt)
			    : MONTH_FULL_WITH_DAY (tt);
          if (g_strcmp0 (name, "") == 0)
            return FALSE;

          name_is_utf8 = locale_is_utf8 ||
            ((alt_digits && !MONTH_FULL_STANDALONE_IS_LOCALE) ||
             (!alt_digits && !MONTH_FULL_WITH_DAY_IS_LOCALE));

          if (!string_append (outstr, name, name_is_utf8))
              return FALSE;

	  break;
	case 'c':
	  {
            if (g_strcmp0 (PREFERRED_DATE_TIME_FMT, "") == 0)
              return FALSE;
            if (!i_cal_time_format_locale (tt, PREFERRED_DATE_TIME_FMT,
                                           outstr, locale_is_utf8))
              return FALSE;
	  }
	  break;
	case 'C':
	  format_number (outstr, alt_digits, pad_set ? pad : "0", 2,
			 i_cal_time_get_year (tt) / 100);
	  break;
	case 'd':
	  format_number (outstr, alt_digits, pad_set ? pad : "0", 2,
			 i_cal_time_get_day (tt));
	  break;
	case 'e':
	  format_number (outstr, alt_digits, pad_set ? pad : " ", 2,
			 i_cal_time_get_day (tt));
	  break;
	case 'f':
	  // ICalTime doesn't support microsecond precision, return 0
	  g_string_append_printf (outstr, "%06" G_GUINT64_FORMAT, (guint64) 0);
	  break;
	case 'F':
	  g_string_append_printf (outstr, "%d-%02d-%02d",
				  i_cal_time_get_year (tt),
				  i_cal_time_get_month (tt),
				  i_cal_time_get_day (tt));
	  break;
	case 'g':
	  format_number (outstr, alt_digits, pad_set ? pad : "0", 2,
			 i_cal_time_get_week_numbering_year (tt) % 100);
	  break;
	case 'G':
	  format_number (outstr, alt_digits, pad_set ? pad : 0, 0,
			 i_cal_time_get_week_numbering_year (tt));
	  break;
	case 'h':
	  name = alt_digits ? MONTH_ABBR_STANDALONE (tt)
			    : MONTH_ABBR_WITH_DAY (tt);
          if (g_strcmp0 (name, "") == 0)
            return FALSE;

          name_is_utf8 = locale_is_utf8 ||
            ((alt_digits && !MONTH_ABBR_STANDALONE_IS_LOCALE) ||
             (!alt_digits && !MONTH_ABBR_WITH_DAY_IS_LOCALE));

          if (!string_append (outstr, name, name_is_utf8))
            return FALSE;

	  break;
	case 'H':
	  format_number (outstr, alt_digits, pad_set ? pad : "0", 2,
			 i_cal_time_get_hour (tt));
	  break;
	case 'I':
	  format_number (outstr, alt_digits, pad_set ? pad : "0", 2,
			 (i_cal_time_get_hour (tt) + 11) % 12 + 1);
	  break;
	case 'j':
	  format_number (outstr, alt_digits, pad_set ? pad : "0", 3,
			 i_cal_time_day_of_year (tt));
	  break;
	case 'k':
	  format_number (outstr, alt_digits, pad_set ? pad : " ", 2,
			 i_cal_time_get_hour (tt));
	  break;
	case 'l':
	  format_number (outstr, alt_digits, pad_set ? pad : " ", 2,
			 (i_cal_time_get_hour (tt) + 11) % 12 + 1);
	  break;
	case 'm':
	  format_number (outstr, alt_digits, pad_set ? pad : "0", 2,
			 i_cal_time_get_month (tt));
	  break;
	case 'M':
	  format_number (outstr, alt_digits, pad_set ? pad : "0", 2,
			 i_cal_time_get_minute (tt));
	  break;
	case 'n':
	  g_string_append_c (outstr, '\n');
	  break;
	case 'O':
	  alt_digits = TRUE;
	  goto next_mod;
	case 'p':
          if (!format_ampm (tt, outstr, locale_is_utf8, TRUE))
            return FALSE;
          break;
	case 'P':
          if (!format_ampm (tt, outstr, locale_is_utf8, FALSE))
            return FALSE;
	  break;
	case 'r':
	  {
            if (g_strcmp0 (PREFERRED_12HR_TIME_FMT, "") == 0)
              return FALSE;
	    if (!i_cal_time_format_locale (tt, PREFERRED_12HR_TIME_FMT,
					   outstr, locale_is_utf8))
	      return FALSE;
	  }
	  break;
	case 'R':
	  g_string_append_printf (outstr, "%02d:%02d",
				  i_cal_time_get_hour (tt),
				  i_cal_time_get_minute (tt));
	  break;
	case 's':
	  g_string_append_printf (outstr, "%" G_GINT64_FORMAT, (gint64)i_cal_time_as_timet (tt));
	  break;
	case 'S':
	  format_number (outstr, alt_digits, pad_set ? pad : "0", 2,
			 i_cal_time_get_second (tt));
	  break;
	case 't':
	  g_string_append_c (outstr, '\t');
	  break;
	case 'T':
	  g_string_append_printf (outstr, "%02d:%02d:%02d",
				  i_cal_time_get_hour (tt),
				  i_cal_time_get_minute (tt),
				  i_cal_time_get_second (tt));
	  break;
	case 'u':
	  format_number (outstr, alt_digits, 0, 0,
			 i_cal_time_day_of_week_monday (tt));
	  break;
	case 'V':
	  format_number (outstr, alt_digits, pad_set ? pad : "0", 2,
			 i_cal_time_get_week_of_year (tt));
	  break;
	case 'w':
	  format_number (outstr, alt_digits, 0, 0,
			 i_cal_time_day_of_week (tt) - 1);
	  break;
	case 'x':
	  {
            if (g_strcmp0 (PREFERRED_DATE_FMT, "") == 0)
              return FALSE;
	    if (!i_cal_time_format_locale (tt, PREFERRED_DATE_FMT,
					   outstr, locale_is_utf8))
	      return FALSE;
	  }
	  break;
	case 'X':
	  {
            if (g_strcmp0 (PREFERRED_TIME_FMT, "") == 0)
              return FALSE;
	    if (!i_cal_time_format_locale (tt, PREFERRED_TIME_FMT,
					   outstr, locale_is_utf8))
	      return FALSE;
	  }
	  break;
	case 'y':
	  format_number (outstr, alt_digits, pad_set ? pad : "0", 2,
			 i_cal_time_get_year (tt) % 100);
	  break;
	case 'Y':
	  format_number (outstr, alt_digits, 0, 0,
			 i_cal_time_get_year (tt));
	  break;
	case 'z':
	  {
	    gint offset;
	    offset = i_cal_timezone_get_utc_offset (i_cal_time_get_timezone (tt), (ICalTime *) tt, NULL);
	    if (!format_z (outstr, offset, colons))
	      return FALSE;
	  }
	  break;
	case 'Z':
	  tz = i_cal_timezone_get_tznames (i_cal_time_get_timezone (tt));
	  if (tz)
            g_string_append (outstr, tz);
	  else
            g_string_append (outstr, "UTC");
	  break;
	case '%':
	  g_string_append_c (outstr, '%');
	  break;
	case '-':
	  pad_set = TRUE;
	  pad = "";
	  goto next_mod;
	case '_':
	  pad_set = TRUE;
	  pad = " ";
	  goto next_mod;
	case '0':
	  pad_set = TRUE;
	  pad = "0";
	  goto next_mod;
	case ':':
	  /* Colons are only allowed before 'z' */
	  if (*utf8_format && *utf8_format != 'z' && *utf8_format != ':')
	    return FALSE;
	  colons++;
	  goto next_mod;
	default:
	  return FALSE;
	}
    }

  return TRUE;
}

/**
 * i_cal_time_format:
 * @tt: An #IcalTime
 * @format: a valid UTF-8 string, containing the format for the
 *          #IcalTime
 *
 * Creates a newly allocated string representing the requested @format.
 *
 * The format strings understood by this function are a subset of the
 * strftime() format language as specified by C99.  The \%D, \%U and \%W
 * conversions are not supported, nor is the 'E' modifier.  The GNU
 * extensions \%k, \%l, \%s and \%P are supported, however, as are the
 * '0', '_' and '-' modifiers. The Python extension \%f is also supported.
 *
 * In contrast to strftime(), this function always produces a UTF-8
 * string, regardless of the current locale.  Note that the rendering of
 * many formats is locale-dependent and may not match the strftime()
 * output exactly.
 *
 * The following format specifiers are supported:
 *
 * - \%a: the abbreviated weekday name according to the current locale
 * - \%A: the full weekday name according to the current locale
 * - \%b: the abbreviated month name according to the current locale
 * - \%B: the full month name according to the current locale
 * - \%c: the preferred date and time representation for the current locale
 * - \%C: the century number (year/100) as a 2-digit integer (00-99)
 * - \%d: the day of the month as a decimal number (range 01 to 31)
 * - \%e: the day of the month as a decimal number (range  1 to 31)
 * - \%F: equivalent to `%Y-%m-%d` (the ISO 8601 date format)
 * - \%g: the last two digits of the ISO 8601 week-based year as a
 *   decimal number (00-99). This works well with \%V and \%u.
 * - \%G: the ISO 8601 week-based year as a decimal number. This works
 *   well with \%V and \%u.
 * - \%h: equivalent to \%b
 * - \%H: the hour as a decimal number using a 24-hour clock (range 00 to 23)
 * - \%I: the hour as a decimal number using a 12-hour clock (range 01 to 12)
 * - \%j: the day of the year as a decimal number (range 001 to 366)
 * - \%k: the hour (24-hour clock) as a decimal number (range 0 to 23);
 *   single digits are preceded by a blank
 * - \%l: the hour (12-hour clock) as a decimal number (range 1 to 12);
 *   single digits are preceded by a blank
 * - \%m: the month as a decimal number (range 01 to 12)
 * - \%M: the minute as a decimal number (range 00 to 59)
 * - \%f: the microsecond as a decimal number (range 000000 to 999999)
 * - \%p: either "AM" or "PM" according to the given time value, or the
 *   corresponding  strings for the current locale.  Noon is treated as
 *   "PM" and midnight as "AM". Use of this format specifier is discouraged, as
 *   many locales have no concept of AM/PM formatting. Use \%c or \%X instead.
 * - \%P: like \%p but lowercase: "am" or "pm" or a corresponding string for
 *   the current locale. Use of this format specifier is discouraged, as
 *   many locales have no concept of AM/PM formatting. Use \%c or \%X instead.
 * - \%r: the time in a.m. or p.m. notation. Use of this format specifier is
 *   discouraged, as many locales have no concept of AM/PM formatting. Use \%c
 *   or \%X instead.
 * - \%R: the time in 24-hour notation (\%H:\%M)
 * - \%s: the number of seconds since the Epoch, that is, since 1970-01-01
 *   00:00:00 UTC
 * - \%S: the second as a decimal number (range 00 to 60)
 * - \%t: a tab character
 * - \%T: the time in 24-hour notation with seconds (\%H:\%M:\%S)
 * - \%u: the ISO 8601 standard day of the week as a decimal, range 1 to 7,
 *    Monday being 1. This works well with \%G and \%V.
 * - \%V: the ISO 8601 standard week number of the current year as a decimal
 *   number, range 01 to 53, where week 1 is the first week that has at
 *   least 4 days in the new year.
 *   This works well with \%G and \%u.
 * - \%w: the day of the week as a decimal, range 0 to 6, Sunday being 0.
 *   This is not the ISO 8601 standard format -- use \%u instead.
 * - \%x: the preferred date representation for the current locale without
 *   the time
 * - \%X: the preferred time representation for the current locale without
 *   the date
 * - \%y: the year as a decimal number without the century
 * - \%Y: the year as a decimal number including the century
 * - \%z: the time zone as an offset from UTC (+hhmm)
 * - \%:z: the time zone as an offset from UTC (+hh:mm).
 *   This is a gnulib strftime() extension.
 * - \%::z: the time zone as an offset from UTC (+hh:mm:ss). This is a
 *   gnulib strftime() extension.
 * - \%:::z: the time zone as an offset from UTC, with : to necessary
 *   precision (e.g., -04, +05:30). This is a gnulib strftime() extension.
 * - \%Z: the time zone or name or abbreviation
 * - \%\%: a literal \% character
 *
 * Some conversion specifications can be modified by preceding the
 * conversion specifier by one or more modifier characters. The
 * following modifiers are supported for many of the numeric
 * conversions:
 *
 * - O: Use alternative numeric symbols, if the current locale supports those.
 * - _: Pad a numeric result with spaces. This overrides the default padding
 *   for the specifier.
 * - -: Do not pad a numeric result. This overrides the default padding
 *   for the specifier.
 * - 0: Pad a numeric result with zeros. This overrides the default padding
 *   for the specifier.
 *
 * Additionally, when O is used with B, b, or h, it produces the alternative
 * form of a month name. The alternative form should be used when the month
 * name is used without a day number (e.g., standalone). It is required in
 * some languages (Baltic, Slavic, Greek, and more) due to their grammatical
 * rules. For other languages there is no difference. \%OB is a GNU and BSD
 * strftime() extension expected to be added to the future POSIX specification,
 * \%Ob and \%Oh are GNU strftime() extensions.
 *
 * Returns: (transfer full) (nullable): a newly allocated string formatted to
 *    the requested format or %NULL in the case that there was an error (such
 *    as a format specifier not being supported in the current locale). The
 *    string should be freed with g_free().
 *
 * Since: 3.0.12
 */
gchar *
i_cal_time_format (const ICalTime *tt,
                   const gchar *format)
{
  GString  *outstr;
  const gchar *charset;
  /* Avoid conversions from locale (for LC_TIME and not for LC_MESSAGES unless
   * specified otherwise) charset to UTF-8 if charset is compatible
   * with UTF-8 already. Check for UTF-8 and synonymous canonical names of
   * ASCII. */
  gboolean time_is_utf8_compatible = g_get_charset (&charset) ||
    g_strcmp0 ("ASCII", charset) == 0 ||
    g_strcmp0 ("ANSI_X3.4-1968", charset) == 0;

  g_return_val_if_fail (tt != NULL, NULL);
  g_return_val_if_fail (format != NULL, NULL);
  g_return_val_if_fail (g_utf8_validate (format, -1, NULL), NULL);

  outstr = g_string_sized_new (strlen (format) * 2);

  if (!i_cal_time_format_utf8 (tt, format, outstr,
                               time_is_utf8_compatible))
    {
      g_string_free (outstr, TRUE);
      return NULL;
    }

  return g_string_free (outstr, FALSE);
}
