/*======================================================================
 FILE: icaltime.h
 CREATOR: eric 02 June 2000

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 The Original Code is eric. The Initial Developer of the Original
 Code is Eric Busboom
======================================================================*/

/**
 * @file icaltime.h
 * @brief Defines the data structure for representing date-times.
 *
 * struct icaltimetype is a pseudo-object that abstracts time handling.
 *
 * It can represent either a DATE or a DATE-TIME (floating, UTC or in a
 * given timezone), and it keeps track internally of its native timezone.
 *
 * The typical usage is to call the correct constructor specifying the
 * desired timezone. If this is not known until a later time, the
 * correct behavior is to specify a NULL timezone and call
 * icaltime_convert_to_zone() at a later time.
 *
 * There are several ways to create a new icaltimetype:
 *
 * - icaltime_null_time()
 * - icaltime_null_date()
 * - icaltime_current_time_with_zone()
 * - icaltime_today()
 * - icaltime_from_timet_with_zone(icaltime_t tm, int is_date,
 *                                 icaltimezone *zone)
 * - icaltime_from_day_of_year(int doy, int year)
 *
 * icaltimetype objects can be converted to different formats:
 *
 * - icaltime_as_timet(struct icaltimetype tt)
 * - icaltime_as_timet_with_zone(struct icaltimetype tt, icaltimezone *zone)
 * - icaltime_as_ical_string(struct icaltimetype tt)
 *
 * Accessor methods include:
 *
 * - icaltime_get_timezone(struct icaltimetype t)
 * - icaltime_get_tzid(struct icaltimetype t)
 * - icaltime_set_timezone(struct icaltimetype t, const icaltimezone *zone)
 * - icaltime_day_of_year(struct icaltimetype t)
 * - icaltime_day_of_week(struct icaltimetype t)
 * - icaltime_start_doy_week(struct icaltimetype t, int fdow)
 *
 * Query methods include:
 *
 * - icaltime_is_null_time(struct icaltimetype t)
 * - icaltime_is_valid_time(struct icaltimetype t)
 * - icaltime_is_date(struct icaltimetype t)
 * - icaltime_is_utc(struct icaltimetype t)
 *
 * Modify, compare and utility methods include:
 *
 * - icaltime_compare(struct icaltimetype a,struct icaltimetype b)
 * - icaltime_compare_date_only(struct icaltimetype a,  struct icaltimetype b)
 * - icaltime_adjust(struct icaltimetype *tt, int days, int hours,
 *                   int minutes, int seconds);
 * - icaltime_normalize(struct icaltimetype t);
 * - icaltime_convert_to_zone(const struct icaltimetype tt, icaltimezone *zone);
 */

#ifndef ICALTIME_H
#define ICALTIME_H

#include "libical_ical_export.h"

#include "icaltime_p.h"

#include <stdbool.h>
#include <time.h>

/// @cond PRIVATE
/* An opaque struct representing a timezone. We declare this here to avoid
   a circular dependency. */
#if !defined(ICALTIMEZONE_DEFINED)
#define ICALTIMEZONE_DEFINED
typedef struct _icaltimezone icaltimezone;
#endif
/// @endcond

/**
 * Structure to represent a date or date-time.
 */
struct icaltimetype {
    int year;   /**< Actual year, e.g. 2001. */
    int month;  /**< 1 (Jan) to 12 (Dec). */
    int day;    /**< 1-31, month dependent */
    int hour;   /**< 0-23 */
    int minute; /**< 0-59 */
    int second; /**< 0-59 */

    int is_date; /**< 1 -> interpret this as date. */

    int is_daylight; /**< 1 -> time is in daylight savings time. */

    const icaltimezone *zone; /**< timezone */
};

typedef struct icaltimetype icaltimetype;

/// @cond PRIVATE
#define ICALTIMETYPE_INITIALIZER {0, 0, 0, 0, 0, 0, 0, 0, 0}
/// @endcond

/**
 * Construct an icaltimetype with null time.
 *
 * @returns A null time icaltimetype, which indicates no time has been set.
 *This time represents the beginning of the epoch.
 */
LIBICAL_ICAL_EXPORT struct icaltimetype icaltime_null_time(void);

/**
 * Construct an icaltimetype with null date.
 *
 * @returns A null date icaltimetype, which indicates no time has been set.
 */
LIBICAL_ICAL_EXPORT struct icaltimetype icaltime_null_date(void);

/**
 * Construct an icaltimeype with the current time in the specified timezone.
 *
 * @param zone is the timezone to use
 *
 * @returns The current time in the given timezone, as an icaltimetype.
 */
LIBICAL_ICAL_EXPORT struct icaltimetype icaltime_current_time_with_zone(const icaltimezone *zone);

/**     @brief Convenience constructor.
 *
 * @returns The current day as an icaltimetype, with is_date set.
 */
LIBICAL_ICAL_EXPORT struct icaltimetype icaltime_today(void);

/**     @brief Constructor.
 *
 *      @param tm The time expressed as seconds past UNIX epoch
 *      @param is_date Boolean: 1 means we should treat tm as a DATE
 *      @param zone The timezone tm is in, NULL means to treat tm as a
 *              floating time
 *
 *      @return a new icaltime instance, initialized to the given time,
 *      optionally using the given timezone.
 *
 *      If the caller specifies the is_date param as TRUE, the returned
 *      object is of DATE type, otherwise the input is meant to be of
 *      DATE-TIME type.
 *      If the zone is not specified (NULL zone param) the time is taken
 *      to be floating, that is, valid in any timezone. Note that, in
 *      addition to the uses specified in [RFC5545], this can be used
 *      when doing simple math on couples of times.
 *      If the zone is specified (UTC or otherwise), it's stored in the
 *      object and it's used as the native timezone for this object.
 *      This means that the caller can convert this time to a different
 *      target timezone with no need to store the source timezone.
 *
 */
LIBICAL_ICAL_EXPORT struct icaltimetype icaltime_from_timet_with_zone(const icaltime_t tm,
                                                                      const bool is_date,
                                                                      const icaltimezone *zone);

/**
 * Creates a time from a ISO-8601-2004 string, per https://tools.ietf.org/html/rfc5545#section-3.3.5.
 *
 * @param str is a string containing a valid DATE-TIME per rfc5545 [ISO-8601-2004]
 *
 * The string format is based on the [ISO.8601.2004] spec in one of 3 legal forms:
 *
 *   - DATE WITH LOCAL TIME "19980118T230000"
 *   - DATE WITH UTC TIME "19980119T070000Z"
 *   - DATE WITH LOCAL TIME AND TIME ZONE REFERENCE "TZID=America/New_York:19980119T020000"
 *
 * That is to say, the basic format for time is T[hh][mm][ss] per ISO-8601-2004
 *
 * @return the icaltimetype representation of the specified date or date-time string.
 */
LIBICAL_ICAL_EXPORT struct icaltimetype icaltime_from_string(const char *str);

/**
 * Creates a new time, given a day of year and a year.
 *
 * @param doy in integer day of the year. Zero and negative numbers represent
          days of the previous year.
 * @param year the year
 *
 * @note that Jan 1 is day 1, not 0.
 * @note no sanity or error checking is done for the inputs; it's up to the
 *       caller to provide reasonable values.
 *
 * @return a new time, given the day of year and a year.
 */
LIBICAL_ICAL_EXPORT struct icaltimetype icaltime_from_day_of_year(const int doy, const int year);

/**
 * Compute the time as seconds past the UNIX epoch.
 *
 * This function probably won't do what you expect.  In particular, you should
 * only pass an icaltime in UTC, since no conversion is done.  Even in that case,
 * it's probably better to just use icaltime_as_timet_with_zone().
 *
 * @param tt is the icaltimetype
 *
 * The return value is defined for dates ranging from 1902-01-01 (incl.) up to 10000-01-01 (excl.)
 * if time_t has a size of 64 bit and up to 2038-01-18 (excl.) if it has a size of 32 bit.
 *
 * @return the time as seconds past the UNIX epoch.
 */
LIBICAL_ICAL_EXPORT icaltime_t icaltime_as_timet(const struct icaltimetype tt);

/**
 * Computes the time as seconds past the UNIX epoch, using the given timezone.
 *
 * This convenience method combines a call to icaltime_convert_to_zone()
 * with a call to icaltime_as_timet().
 *
 * If the input timezone is null, no conversion is done; that is, the
 * time is simply returned as icaltime_t in its native timezone.
 *
 * @param tt the time to convert
 * @param zone The timezone to use. NULL means no conversion.
 *
 * @return the time as seconds past the UNIX epoch, using the given timezone.
 */
LIBICAL_ICAL_EXPORT icaltime_t icaltime_as_timet_with_zone(const struct icaltimetype tt,
                                                           const icaltimezone *zone);

/**
 * Returns a string representation of the time, in RFC5545 format.
 *
 * @param tt an icaltimetype, presumably containing valid data.
 *
 * @note The string is owned by the caller and must be freed accordingly.
 * @note No sanity or error checking is performed on the input time @p tt.
 *
 * @return a string representation of the @p tt in RFC5545 format.
 *
 * @see icaltime_as_ical_string_r
 */
LIBICAL_ICAL_EXPORT const char *icaltime_as_ical_string(const struct icaltimetype tt);

/**
 * Returns a string representation of the time, in RFC5545 format.
 *
 * @param tt an icaltimetype, presumably containing valid data.
 *
 * @note The string is owned by the caller and must be freed accordingly.
 * @note No sanity or error checking is performed on the input time @p tt.
 *
 * @return a string representation of the time, in RFC5545 format.
 *
 * @see icaltime_as_ical_string
 */
LIBICAL_ICAL_EXPORT char *icaltime_as_ical_string_r(const struct icaltimetype tt);

/**
 * Return the timezone for an icaltimetype.
 *
 * @param t is the icaltimetype
 *
 * @return a pointer to the @p t timezone.
 */
LIBICAL_ICAL_EXPORT const icaltimezone *icaltime_get_timezone(const struct icaltimetype t);

/**
 * Returns the tzid for an icaltimetype.
 *
 * @param t is the icaltimetype
 *
 * @return a string containing the tzid, or NULL for a floating time.
 */
LIBICAL_ICAL_EXPORT const char *icaltime_get_tzid(const struct icaltimetype t);

/**
 * Sets the timezone for an icaltimetype.
 *
 * @param t is the icaltimetype
 * @param zone is the timezone to use
 *
 * Forces the icaltime to be interpreted relative to another timezone.
 * The returned time represents the same time as @p t, but relative to
 * the new @p zone.
 *
 * For example, modifying July 20 1969, 8:17 PM UTC to the EDT time zone
 * would return a time representing July 20 1969, 8:17 PM EDT.
 *
 * If you need to do timezone conversion, applying offset adjustments,
 * then you should use icaltime_convert_to_zone instead.
 *
 * If @p t is of type `DATE`, its timezone is not modified and the returned
 * time is an exact copy of @p t.
 *
 * @return a icaltimetype computed from @p t with the timezone @p zone.
 */
LIBICAL_ICAL_EXPORT struct icaltimetype icaltime_set_timezone(struct icaltimetype *t,
                                                              const icaltimezone *zone);

/**
 * Computes the day of the year for the specified icaltimetype, counting
 * from 1 (Jan 1st).
 *
 * @param t is the icaltimetype
 *
 * @return the day of the year for @p t (1-366).
 */
LIBICAL_ICAL_EXPORT int icaltime_day_of_year(const struct icaltimetype t);

/**
 * Computes the day of the week for the specified icaltimetype.
 *
 * @param t is the icaltimetype
 *
 * @return the day of the week (Sunday is 1, and Saturday is 7) for the time @p t.
 */
LIBICAL_ICAL_EXPORT int icaltime_day_of_week(const struct icaltimetype t);

/**
 * Returns the day of the year for the first day of the week
 * that the given time is within.
 *
 * This uses the first day of the week that contains the given time,
 * which is a Sunday. It returns the day of the year for the resulting day.
 *
 * @param t is the icaltimetype
 * @param fdow is the first day of the week expressed as an integer (1-7)
 *
 * @return the day of the year for the Sunday of the week that the given @p t is within.
 */
LIBICAL_ICAL_EXPORT int icaltime_start_doy_week(const struct icaltimetype t, int fdow);

/**
 * Determine if the specified icaltimetype contain "null" time, where
 * "null" time means that all the contents are 0.
 *
 * @param t is the icaltimetype
 *
 * @return true if @p t represents "null" time; false otherwise.
 */
LIBICAL_ICAL_EXPORT bool icaltime_is_null_time(const struct icaltimetype t);

/**
 * Determine if the specified icaltimetype is a valid DATE.
 *
 * @param t is the icaltimetype
 *
 * This is usually the result of creating a new time type but not
 * clearing it, or setting one of the flags to an illegal value.
 *
 * @return true if contents of @p t are valid; false otherwise.
 */
LIBICAL_ICAL_EXPORT bool icaltime_is_valid_time(const struct icaltimetype t);

/**
 * Determine if the specified icaltimetype is a DATE.
 *
 * @param t is the icaltimetype
 *
 * @return true if the @p t is a DATE (rather than a DATE-TIME); false otherwise.
 */
LIBICAL_ICAL_EXPORT bool icaltime_is_date(const struct icaltimetype t);

/**
 * Determine if the specified icaltimetype is relative to the UTC zone.
 * @todo We should only check the zone.
 *
 * @param t is the icaltimetype
 *
 * @return true if @p t is relative to UTC zone; false otherwise.
 */
LIBICAL_ICAL_EXPORT bool icaltime_is_utc(const struct icaltimetype t);

/**
 * Compares to icaltimetypes for equality.
 * This converts both times to the UTC timezone and compares them.
 *
 * @param a is the first icaltimetype to compare
 * @param b is the second icaltimetype to compare
 *
 * @return -1, 0, or 1 to indicate that a is less than b, a  equals b,
 * or a is greater than b, respectively.
 *
 * @see icaltime_compare_date_only, icaltime_compare_date_only_tz
 */
LIBICAL_ICAL_EXPORT int icaltime_compare(const struct icaltimetype a, const struct icaltimetype b);

/**
 * Like icaltime_compare, but only use the date parts.
 * This converts both times to the UTC timezone and compares their date
 * components.
 *
 * @param a is the first icaltimetype to compare
 * @param b is the second icaltimetype to compare
 *
 * @return -1, 0, or 1 to indicate that a is less than b, a  equals b,
 * or a is greater than b, respectively.
 *
 * @see icaltime_compare, icaltime_compare_date_only_tz
 */
LIBICAL_ICAL_EXPORT int icaltime_compare_date_only(const struct icaltimetype a,
                                                   const struct icaltimetype b);

/**
 * Like icaltime_compare, but only use the date parts; accepts timezone.
 * This converts both times to the given timezone and compares their date
 * components.
 *
 * @param a is the first icaltimetype to compare
 * @param b is the second icaltimetype to compare
 * @param tz is the timezone to use
 *
 * @return -1, 0, or 1 to indicate that a is less than b, a  equals b,
 * or a is greater than b, respectively.
 *
 * @see icaltime_compare, icaltime_compare_date_only
 */
LIBICAL_ICAL_EXPORT int icaltime_compare_date_only_tz(const struct icaltimetype a,
                                                      const struct icaltimetype b,
                                                      icaltimezone *tz);

/**
 * Adds or subtracts time from an icaltimetype.
 *
 * Adds or subtracts a time from an icaltimetype. This time is given
 * as a number of days, hours, minutes and seconds.
 *
 * @param tt is a pointer to a valid icaltimetype
 * @param days is the number of days adjustment
 * @param hours is the number of hours adjustment
 * @param minutes is the number of minutes adjustment
 * @param seconds is the number of seconds adjustment
 *
 * @note This function is identical to the static icaltimezone_adjust_change()
 * except for the type of the first parameter.
 */
LIBICAL_ICAL_EXPORT void icaltime_adjust(struct icaltimetype *tt,
                                         const int days, const int hours,
                                         const int minutes, const int seconds);

/**
 * Normalizes the icaltime, so all of the time components
 * are in their normal ranges.
 *
 * @param tt is the icaltimetype to normalize
 *
 * For instance, given a time with minutes=70, the minutes will be
 * reduced to 10 and the hour incremented. This allows the caller to
 * do arithmetic on times without worrying about overflow or underflow.
 *
 * @return an icaltimetype containing the normalized @p tt.
 */
LIBICAL_ICAL_EXPORT struct icaltimetype icaltime_normalize(const struct icaltimetype tt);

/**
 * Converts a time from its native timezone to a given timezone.
 *
 * @param tt is the icaltimetype to convert
 * @param zone is the timezone to use
 *
 * If @p tt is a date, the timezone is not converted and the returned
 * time is an exact copy of @p tt.
 *
 * If it's a floating time, the returned object
 * represents the same time relative to @p zone.
 * For example, if @p tt represents 9:30 AM floating and @p zone
 * is the GMT timezone, the returned object will represent 9:30 AM GMT.
 *
 * Otherwise, the time will be converted to @p zone, and its timezone
 * property updated to @p zone.
 *
 * For example, July 20 1969, 8:17 PM UTC would be converted to July 20
 * 1969, 4:17 PM EDT.
 *
 * @return an icaltimetype containing the timezone shifted @p tt.
 */
LIBICAL_ICAL_EXPORT struct icaltimetype icaltime_convert_to_zone(const struct icaltimetype tt,
                                                                 icaltimezone *zone);

/**
 * Returns the number of days in the given month.
 *
 * @param month is a month number (1-12)
 * @param year is the year (e.g. 2001)
 *
 * @return the number of days in the specified month.
 */
LIBICAL_ICAL_EXPORT int icaltime_days_in_month(const int month, const int year);

/**
 * Returns whether the specified year is a leap year.
 *
 * @param year is the year (e.g. 2001)
 *
 * @return true if the @p year is a leap year; false otherwise.
 */
LIBICAL_ICAL_EXPORT bool icaltime_is_leap_year(const int year);

/**
 * Returns the number of days in a specified year.
 *
 * @param year is year (e.g. 2001)
 *
 * @return the number of days in the specified year; ie, 365 unless
 * @p year is a leap year.
 */
LIBICAL_ICAL_EXPORT int icaltime_days_in_year(const int year);

#endif /* !ICALTIME_H */

/* kate: syntax C; */
