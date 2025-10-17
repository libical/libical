/*======================================================================
 FILE: icalduration.h
 CREATOR: eric 26 Jan 2001

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 The Original Code is eric. The Initial Developer of the Original
 Code is Eric Busboom
======================================================================*/

#ifndef ICALDURATION_H
#define ICALDURATION_H

/**
 * @file icalduration.h
 * @brief Methods for working with durations in iCal
 */

#include "libical_ical_export.h"
#include "icaltime.h"

#include <stdbool.h>

/**
 * @brief A struct representing a duration
 *
 * Days and weeks are nominal; hours, minutes and seconds are exact
 */
struct icaldurationtype {
    int is_neg;
    unsigned int days;
    unsigned int weeks;
    unsigned int hours;
    unsigned int minutes;
    unsigned int seconds;
};

#define ICALDURATIONTYPE_INITIALIZER \
    {                                \
        0, 0, 0, 0, 0, 0}

/**
 * @brief Creates a new ::icaldurationtype from a duration in seconds.
 * @param seconds The duration in seconds
 * @return An ::icaldurationtype representing the duration @a t in seconds
 *
 * The number of seconds stored in the ::icaldurationtype structure is always a positive integer.
 * If the @p seconds argument is negative, then the `is_neg` structure member is set to 1 (true).
 *
 * Also note that the number of seconds is not in any way refactored into days, hours, or minutes.
 * This function simply assigns the ::icaldurationtype seconds structure member from the integer provided.
 *
 * @par Example
 * ```c
 * // create a new icaldurationtype with a duration of 60 seconds
 * struct icaldurationtype duration;
 * duration = icaldurationtype_from_seconds(60);
 * // create a new icaldurationtype with a duration of 60 seconds pointing back in time
 * duration = icaldurationtype_from_seconds(-60);
 * ```
 *
 * @since 4.0 previously known as icaldurationtype_from_int
 */
LIBICAL_ICAL_EXPORT struct icaldurationtype icaldurationtype_from_seconds(int seconds);

/**
 * @brief Creates a new ::icaldurationtype from a duration given as a string.
 * @param str The duration as a string
 * @return An ::icaldurationtype representing the duration @a str
 *
 * @par Error handling
 * When given bad input, it sets ::icalerrno to ::ICAL_MALFORMEDDATA_ERROR and
 * returns icaldurationtype_bad_duration().
 *
 * @par Usage
 * ```c
 * // create a new icaldurationtype
 * struct icaldurationtype duration;
 * duration = icaldurationtype_from_string("+PT05M");
 *
 * // verify that it's 5 minutes
 * assert(duration.minutes == 5);
 * ```
 */
LIBICAL_ICAL_EXPORT struct icaldurationtype icaldurationtype_from_string(const char *str);

/**
 * @brief Extracts the duration in integer seconds from an ::icaldurationtype.
 * @param duration A valid duration type.
 * @return An `int` representing the number of seconds in the duration.
 *
 * The number of seconds returned from the specified ::icaldurationtype can be a positive or negative integer
 * depending if the duration points forward or backward in time.
 *
 * Additionally, a ::icaldurationtype that has a non-zero days or weeks value is considered an error.
 * ie. only the ::icaldurationtype seconds, minutes and hours structure members are converted.
 *
 * @par Usage
 * ```c
 * // create icaldurationtype with given duration
 * struct icaldurationtype duration;
 * duration = icaldurationtype_from_seconds(3532342);
 *
 * // get the duration in seconds and verify it
 * assert(icaldurationtype_as_seconds(duration) == 3532342);
 * ```
 *
 * @since 4.0 previously known as icaldurationtype_as_int
 */
LIBICAL_ICAL_EXPORT int icaldurationtype_as_seconds(struct icaldurationtype duration);

/**
 * Converts an icaldurationtype into the iCal format as string.
 * @param d is the icaldurationtype to convert to iCal format
 * @return A string representing duration @p d in iCal format
 * @sa icaldurationtype_as_ical_string_r()
 *
 * @par Ownership
 * The string returned by this function is owned by the caller and needs to be
 * released with `icalmemory_free_buffer()` after it's no longer needed.
 *
 * @par Usage
 * ```c
 * // create new duration
 * struct icaldurationtype duration;
 * duration = icaldurationtype_from_seconds(3424224);
 *
 * // print as ical-formatted string
 * char *ical = icaldurationtype_as_ical_string(duration);
 * printf("%s\n", ical);
 *
 * // release string
 * icalmemory_free_buffer(ical);
 * ```
 */
LIBICAL_ICAL_EXPORT char *icaldurationtype_as_ical_string(struct icaldurationtype d);

/**
 * Converts an icaldurationtype into the iCal format as string.
 * @param d is the icaldurationtype to convert to iCal format
 * @return A string representing duration @p d in iCal format
 * @sa icaldurationtype_as_ical_string()
 *
 * @par Ownership
 * The string returned by this function is owned by libical and must not be
 * released by the caller of the function.
 *
 * @par Usage
 * ```c
 * // create new duration
 * struct icaldurationtype duration;
 * duration = icaldurationtype_from_seconds(3424224);
 *
 * // print as ical-formatted string
 * printf("%s\n", icaldurationtype_as_ical_string(duration));
 * ```
 */
LIBICAL_ICAL_EXPORT char *icaldurationtype_as_ical_string_r(struct icaldurationtype d);

/**
 * @brief Creates a duration with zero length.
 * @return An ::icaldurationtype with a zero length
 * @sa icaldurationtype_is_null_duration()
 *
 * @par Usage
 * ```c
 * // create null duration
 * struct icaldurationtype duration;
 * duration = icaldurationtype_null_duration();
 *
 * // make sure it's zero length
 * assert(duration.days     == 0);
 * assert(duration.weeks    == 0);
 * assert(duration.hours    == 0);
 * assert(duration.minutes  == 0);
 * assert(duration.seconds  == 0);
 * assert(icalduration_is_null_duration(duration));
 * assert(icalduration_as_seconds(duration) == 0);
 * ```
 */
LIBICAL_ICAL_EXPORT struct icaldurationtype icaldurationtype_null_duration(void);

/**
 * @brief Creates a bad duration (used to indicate error).
 * @return A bad duration
 * @sa icaldurationtype_is_bad_duration()
 *
 * @par Usage
 * ```c
 * // create bad duration
 * struct icaldurationtype duration;
 * duration = icaldurationtype_bad_duration();
 *
 * // make sure it's bad
 * assert(icaldurationtype_is_bad_duration(duration));
 * ```
 */
LIBICAL_ICAL_EXPORT struct icaldurationtype icaldurationtype_bad_duration(void);

/**
 * @brief Checks if a duration is a null duration.
 * @param d The duration to check
 * @return true if the duration is a null duration, false otherwise
 * @sa icalduration_null_duration()
 *
 * @par Usage
 * ```
 * // make null duration
 * struct icaldurationtype duration;
 * duration = icaldurationtype_null_duration();
 *
 * // check null duration
 * assert(icaldurationtype_is_null_duration(duration));
 * ```
 */
LIBICAL_ICAL_EXPORT bool icaldurationtype_is_null_duration(struct icaldurationtype d);

/**
 * @brief Checks if a duration is a bad duration.
 * @param d The duration to check
 * @return true if the duration is a bad duration, false otherwise
 * @sa icalduration_bad_duration()
 *
 * @par Usage
 * ```
 * // make bad duration
 * struct icaldurationtype duration;
 * duration = icaldurationtype_bad_duration();
 *
 * // check bad duration
 * assert(icaldurationtype_is_bad_duration(duration));
 * ```
 */
LIBICAL_ICAL_EXPORT bool icaldurationtype_is_bad_duration(struct icaldurationtype d);

/**
 * @brief Extends a time duration.
 * @param t The time object to add the duration to
 * @param d The duration to add to the time object
 * @return The new ::icaltimetype which has been added the duration to
 *
 * @par Example
 * ```c
 * struct icaltimetype time;
 * struct icaldurationtype duration;
 *
 * // create time and duration objects
 * time = icaltime_today();
 * duration = icaldurationtype_from_seconds(60);
 *
 * // add the duration to the time object
 * time = icalduration_extend(time, duration);
 * ```
 *
 * @since 4.0 previously known as icaltime_add
 */
LIBICAL_ICAL_EXPORT struct icaltimetype icalduration_extend(struct icaltimetype t,
                                                            struct icaldurationtype d);

/**
 * @brief Creates a duration from two ::icaltimetype endpoints.
 * @param t1 The first point in time
 * @param t2 The second point in time
 * @return An ::icaldurationtype representing the duration elapsed between
 * @a t1 and @a t2
 *
 * @par Usage
 * ```c
 * struct icaltimetype t1 = icaltime_from_day_of_year(111, 2018);
 * struct icaltimetype t2 = icaltime_from_day_of_year(112, 2018);
 * struct icaldurationtype duration;
 *
 * // calculate duration between time points
 * duration = icalduration_from_times(t1, t2);
 * ```
 *
 * * @since 4.0 previously known as icaltime_subtract
 */
LIBICAL_ICAL_EXPORT struct icaldurationtype icalduration_from_times(struct icaltimetype t1,
                                                                    struct icaltimetype t2);

#endif /* !ICALDURATION_H */
