/*======================================================================
 FILE: icalrecur.h
 CREATOR: eric 20 March 2000

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
========================================================================*/

/**
@file icalrecur.h
@brief Routines for dealing with recurring time

How to use:

1) Get a rule and a start time from a component

@code
        icalproperty rrule;
        struct icalrecurrencetype *recur;
        struct icaltimetype dtstart;

        rrule = icalcomponent_get_first_property(comp,ICAL_RRULE_PROPERTY);
        recur = icalproperty_get_rrule(rrule);
        start = icalproperty_get_dtstart(dtstart);
@endcode

Or, just make them up:

@code
        recur = icalrecurrencetype_new_from_string("FREQ=YEARLY;BYDAY=SU,WE");
        dtstart = icaltime_from_string("19970101T123000")
@endcode

2) Create an iterator

@code
        icalrecur_iterator *ritr;
        ritr = icalrecur_iterator_new(recur, start);
@endcode

3) Iterator over the occurrences

@code
        struct icaltimetype next;
        next = icalrecur_iterator_next(ritr);
        while (!icaltime_is_null_time(next)){
                // Do something with next
                next = icalrecur_iterator_next(ritr);
        }
@endcode

Note that the time returned by icalrecur_iterator_next is in
whatever timezone that dtstart is in.

4) Deallocate a rule

@code
        icalrecurrencetype_unref(recur);
@endcode

The icalrecurrencetype object is reference counted. It will automatically be deallocated when the
reference count goes to zero.
*/

#ifndef ICALRECUR_H
#define ICALRECUR_H

#include "libical_ical_export.h"
#include "icalarray.h"
#include "icaltime.h"

#include <stdbool.h>

/*
 * Recurrence enumerations
 */

/**
 * Recurrence frequencies.
 *
 * @note These enums are used to index an array, so don't change the
 * order or the integers
 */
typedef enum icalrecurrencetype_frequency
{
    ICAL_SECONDLY_RECURRENCE = 0, /**< recurs on the second */
    ICAL_MINUTELY_RECURRENCE = 1, /**< recurs on the minute */
    ICAL_HOURLY_RECURRENCE = 2,   /**< recurs on the hour */
    ICAL_DAILY_RECURRENCE = 3,    /**< recurs daily */
    ICAL_WEEKLY_RECURRENCE = 4,   /**< recurs weekly */
    ICAL_MONTHLY_RECURRENCE = 5,  /**< recurs monthly */
    ICAL_YEARLY_RECURRENCE = 6,   /**< recurs yearly */
    ICAL_NO_RECURRENCE = 7        /**< does not recur */
} icalrecurrencetype_frequency;

/**
 * Weekly recurrences days.
 */
typedef enum icalrecurrencetype_weekday
{
    ICAL_NO_WEEKDAY,        /**< does not recur on a weekday */
    ICAL_SUNDAY_WEEKDAY,    /**< recurs on Sunday */
    ICAL_MONDAY_WEEKDAY,    /**< recurs on Monday */
    ICAL_TUESDAY_WEEKDAY,   /**< recurs on Tuesday */
    ICAL_WEDNESDAY_WEEKDAY, /**< recurs on Wednesday */
    ICAL_THURSDAY_WEEKDAY,  /**< recurs on Thursday */
    ICAL_FRIDAY_WEEKDAY,    /**< recurs on Friday */
    ICAL_SATURDAY_WEEKDAY   /**< recurs on Saturday */
} icalrecurrencetype_weekday;

/**
 * Recurrence skip types.
 */
typedef enum icalrecurrencetype_skip
{
    ICAL_SKIP_BACKWARD = 0, /**< skip backward to the previous recurrence */
    ICAL_SKIP_FORWARD,      /**< skip forward to the next recurrence */
    ICAL_SKIP_OMIT,         /**< omit this recurrence */
    ICAL_SKIP_UNDEFINED     /**< recurrence skip type is not defined */
} icalrecurrencetype_skip;

/**
 * Recurrence byrule types.
 */
typedef enum icalrecurrencetype_byrule
{
    ICAL_BYRULE_NO_CONTRACTION = -1, /**< indicates the end of byrule */
    ICAL_BY_MONTH = 0,               /**< recur by months */
    ICAL_BY_WEEK_NO = 1,             /**< recur by weeks */
    ICAL_BY_YEAR_DAY = 2,            /**< recur by year days */
    ICAL_BY_MONTH_DAY = 3,           /**< recur by month days */
    ICAL_BY_DAY = 4,                 /**< recur by days */
    ICAL_BY_HOUR = 5,                /**< recur by hours */
    ICAL_BY_MINUTE = 6,              /**< recur by minutes */
    ICAL_BY_SECOND = 7,              /**< recur by seconds */
    ICAL_BY_SET_POS = 8,             /**< recur by a set position */

    ICAL_BY_NUM_PARTS = 9 /**< length of the bydata array */
} icalrecurrencetype_byrule;

/*
 * Recurrence enumerations conversion routines.
 */

/**
 * Converts a char string into a icalrecurrencetype_frequency.
 *
 * @param str a pointer to a char string containing the string representation of
 * an icalrecurrencetype_frequency.
 *
 * @return the icalrecurrencetype_frequency version of @p str.
 * If @p str is invalid then ::ICAL_NO_RECURRENCE is returned.
 *
 * @see icalrecur_freq_to_string
 */
LIBICAL_ICAL_EXPORT icalrecurrencetype_frequency icalrecur_string_to_freq(const char *str);

/**
 * Stringifies a icalrecurrencetype_frequency.
 *
 * @param kind a icalrecurrencetype_frequency to convert
 *
 * @return a pointer to char string version of @p kind.
 * If @p kind is invalid then NULL is returned.
 *
 * @see icalrecur_string_to_freq
 */
LIBICAL_ICAL_EXPORT const char *icalrecur_freq_to_string(icalrecurrencetype_frequency kind);

/**
 * Converts a char string into a icalrecurrencetype_skip.
 *
 * @param str a pointer t a char string containing the string representation of
 * an icalrecurrencetype_skip.
 *
 * @return the icalrecurrencetype_skip version of @p str.
 * If @p str is invalid then ::ICAL_SKIP_UNDEFINED is returned.
 *
 * @see icalrecur_skip_to_string
 */
LIBICAL_ICAL_EXPORT icalrecurrencetype_skip icalrecur_string_to_skip(const char *str);

/**
 * Stringifies a icalrecurrencetype_skip.
 *
 * @param kind a icalrecurrencetype_skip to convert
 *
 * @return a pointer to char string version of @p kind.
 * If @p kind is invalid then NULL is returned.
 *
 * @see icalrecur_string_to_skip
 */
LIBICAL_ICAL_EXPORT const char *icalrecur_skip_to_string(icalrecurrencetype_skip kind);

/**
 * Stringifies a icalrecurrencetype_weekday.
 *
 * @param kind a icalrecurrencetype_weekday to convert
 *
 * @return a pointer to char string version of @p kind.
 * If @p kind is invalid then NULL is returned.
 *
 * @see icalrecur_string_to_weekday
 */
LIBICAL_ICAL_EXPORT const char *icalrecur_weekday_to_string(icalrecurrencetype_weekday kind);

/**
 * Converts a char string into a icalrecurrencetype_weekday.
 *
 * @param str a pointer to a char string containing the string representation
 * of a icalrecurrencetype_weekday
 *
 * @return the icalrecurrencetype_weekday version of @p str.
 * If @p str is invalid then :: ICAL_NO_WEEKDAY is returned.
 *
 * @see icalrecur_weekday_to_string
 */
LIBICAL_ICAL_EXPORT icalrecurrencetype_weekday icalrecur_string_to_weekday(const char *str);

/**
 * Recurrence type routines
 */

//@cond PRIVATE
/* See RFC 5545 Section 3.3.10, RECUR Value, and RFC 7529
 * for an explanation of the values and fields in struct icalrecurrencetype.
 *
 * The maximums below are based on lunisolar leap years (13 months)
 */
#define ICAL_BY_SECOND_SIZE 62                               /* 0 to 60 */
#define ICAL_BY_MINUTE_SIZE 61                               /* 0 to 59 */
#define ICAL_BY_HOUR_SIZE 25                                 /* 0 to 23 */
#define ICAL_BY_MONTH_SIZE 14                                /* 1 to 13 */
#define ICAL_BY_MONTHDAY_SIZE 32                             /* 1 to 31 */
#define ICAL_BY_WEEKNO_SIZE 56                               /* 1 to 55 */
#define ICAL_BY_YEARDAY_SIZE 386                             /* 1 to 385 */
#define ICAL_BY_SETPOS_SIZE ICAL_BY_YEARDAY_SIZE             /* 1 to N */
#define ICAL_BY_DAY_SIZE (7 * (ICAL_BY_WEEKNO_SIZE - 1) + 1) /* 1 to N */

typedef struct {
    short *data;
    short size;
} icalrecurrence_by_data;
//@endcond

/**
 * Data structure for holding digested recurrence rules.
 * @note ::until and ::count are mutually exclusive.
 */
struct icalrecurrencetype {
    /** Reference count */
    int refcount;

    /** the frequency of the recurrence */
    icalrecurrencetype_frequency freq;

    /** recur until this date-time, mutually exclusive with ::count */
    struct icaltimetype until;

    /** number of times to recur, mutually exclusive with ::until */
    int count;

    /** interval between recurrences */
    short interval;

    /** week day start, for weekday recurrences */
    icalrecurrencetype_weekday week_start;

    /**
     * Encoded value
     *
     * The 'day' element of the ICAL_BY_DAY array is encoded to allow
     * representation of both the day of the week ( Monday, Tuesday), but
     * also the Nth day of the week (first Tuesday of the month, last
     * Thursday of the year).
     *
     * These values are decoded by icalrecurrencetype_day_day_of_week() and
     * icalrecurrencetype_day_position().
     *
     * The 'month' element of the ICAL_BY_MONTH array is encoded to allow
     * representation of the "L" leap suffix (RFC 7529).
     *
     * These values are decoded by icalrecurrencetype_month_is_leap()
     * and icalrecurrencetype_month_month().
     */
    icalrecurrence_by_data by[ICAL_BY_NUM_PARTS];

    /** For RSCALE extension (RFC 7529) */
    char *rscale;

    /** recurrence skip (forward, backward, omit) */
    icalrecurrencetype_skip skip;
};

/**
 * Constructs a new instance of icalrecurrencetype.
 *
 * The new instance is initialized with a refcount of 1.
 *
 * @return A pointer to the new instance, or NULL if memory allocation failed.
 */
LIBICAL_ICAL_EXPORT struct icalrecurrencetype *icalrecurrencetype_new(void);

/**
 * Increases the icalrecurrencetype reference counter by 1.
 *
 * @param recur is a pointer to a valid icalrecurrencetype
 */
LIBICAL_ICAL_EXPORT void icalrecurrencetype_ref(struct icalrecurrencetype *recur);

/**
 * Decreases the icalrecurrencetype reference counter by 1.
 *
 * If the counter reaches 0, the instance  and all referenced memory
 * (i.e. rscale and 'by' arrays) are deallocated.
 *
 * @param recur a pointer to a valid icalrecurrencetype
 */
LIBICAL_ICAL_EXPORT void icalrecurrencetype_unref(struct icalrecurrencetype *recur);

/**
 * Return an array of RSCALE supported calendars.
 *
 * @return a pointer to an icalarray of a list of RSCALE supported calendars.
 * Currently returns list containing "GREGORIAN" only.
 */
LIBICAL_ICAL_EXPORT icalarray *icalrecurrencetype_rscale_supported_calendars(void);

/**
 * Creates a deep copy of the given icalrecurrencetype.
 *
 * The new instance is returned with a refcount of 1.
 *
 * @param r is a pointer to a valid icalrecurrencetype
 *
 * @return a pointer to the cloned memory.
 */
LIBICAL_ICAL_EXPORT struct icalrecurrencetype *icalrecurrencetype_clone(struct icalrecurrencetype *r);

/**
 * Resizes the buffer backing the 'by' array to the specified size, if different.
 *
 * Frees the buffer if the new size is 0.
 *
 * @param by is a pointer to valid icalrecurrence_by_data to use
 * @param size is the new size for the array
 *
 * @return true on success, false on failure.
 */
LIBICAL_ICAL_EXPORT bool icalrecur_resize_by(icalrecurrence_by_data *by, short size);

/*
 * Routines to decode the day values of the by[ICAL_BY_DAY] array
 */

/**
 * Decode the weekday from a "weekday:pos" form.
 *
 * @param day is the "weekday:pos" to decode (as created by icalrecurrencetype_encode_day())
 *
 * @returns The decoded day of the week from the "weekday:pos".
 * 1 is Monday, 2 is Tuesday, etc.
 *
 * @see icalrecurrencetype_encode_day and icalrecurrencetype_day_position().
 */
LIBICAL_ICAL_EXPORT enum icalrecurrencetype_weekday icalrecurrencetype_day_day_of_week(short day);

/**
 * Decodes a the position from a "weekday:pos" form.
 *
 * @param day is the "weekday:pos" to decode (as created by icalrecurrencetype_encode_day()).
 *
 * @return The position of the day in the week, where:
 * 0 == any of day of week. 1 == first, 2 = second, -2 == second to last, etc.
 * 0 means 'any' or 'every'.
 */
LIBICAL_ICAL_EXPORT int icalrecurrencetype_day_position(short day);

/**
 * Encode a weekday and position into a form which can be stored into a
 * `icalrecurrencetype::by[ICAL_BY_DAY]` array.
 *
 * The 'day' element of icalrecurrencetype_weekday is encoded to
 * allow representation of both the day of the week ( Monday, Tuesday),
 * but also the Nth day of the week (First tuesday of the month, last
 * thursday of the year) These routines decode the day values.
 *
 * The day's position in the period (Nth-ness) and the numerical
 * value of the day are encoded together as: pos*7 + dow.
 *
 * A position of 0 means 'any' or 'every'.
 *
 * Use icalrecurrencetype_day_day_of_week() and icalrecurrencetype_day_position()
 * to split the encoded value back into the parts.
 *
 * @param weekday is the weekday to use
 * @param position is the positiog to use
 *
 * @return the encoded "weekday:pos", which can later be decoded back using
 * icalrecurrencetype_day_day_of_week() and icalrecurrencetype_day_position().
 *
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT short icalrecurrencetype_encode_day(enum icalrecurrencetype_weekday weekday,
                                                        int position);

/**
 * Decode the is_leap from a month:leap_year form.
 *
 * @param month is the month:leap_year to decode (as created by icalrecurrencetype_encode_month())
 *
 * @return the is_leap portion of the month:leap_year
 *
 * @see icalrecurrencetype_encode_month, icalrecurrencetype_month_month
 */
LIBICAL_ICAL_EXPORT bool icalrecurrencetype_month_is_leap(short month);

/**
 * Decode the month from a month:leap_year form.
 *
 * @param month is the month:leap_year to decode (as created by icalrecurrencetype_encode_month())
 *
 * @return the month portion of the month:leap_year
 *
 * @see icalrecurrencetype_encode_month, icalrecurrencetype_month_is_leap
 */
LIBICAL_ICAL_EXPORT int icalrecurrencetype_month_month(short month);

/**
 * Encodes a month and leap year indicator into a form which can be stored
 * into a `icalrecurrencetype::by[ICAL_BY_MONTH]` array.
 *
 * The month element of the `by[ICAL_BY_MONTH]` array is encoded to allow
 * representation of the "L" leap suffix (RFC 7529).
 * The "L" suffix is encoded by setting a high-order bit.
 *
 * Use icalrecurrencetype_month_is_leap() and icalrecurrencetype_month_month()
 * to split the encoded value back into the parts.
 *
 * @param month is the month to use (1-12)
 * @param is_leap is a bool indicating if leap year is to be considered
 *
 * @return the encoded "month:leap_year", which can later be decoded back using
 * icalrecurrencetype_month_is_leap() and icalrecurrencetype_month_month().
 *
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT short icalrecurrencetype_encode_month(int month, bool is_leap);

/*
 * Recurrence rule parser
 */

/**
 * Create a new icalrecurrencetype from a string.
 *
 * @param str a pointer to a char string containing string version of the recurrence.
 *
 * @return a pointer to the new icalrecurrencetype or NULL if the string
 * contained an invalid representation.
 */
LIBICAL_ICAL_EXPORT struct icalrecurrencetype *icalrecurrencetype_new_from_string(const char *str);

/**
 * Represent an icalrecurrencetype as a string.
 *
 * @param recur a pointer to a valid icalrecurrencetype
 *
 * @return a pointer to a char string containing the string representation
 * of @p recur. Returns NULL if the conversion was unsuccessful.
 *
 * @see icalrecurrencetype_as_string_r
 */
LIBICAL_ICAL_EXPORT char *icalrecurrencetype_as_string(struct icalrecurrencetype *recur);

/**
 * Represent an icalrecurrencetype as a string.
 *
 * @param recur a pointer to a valid icalrecurrencetype
 *
 * @return a pointer to a char string containing the string representation
 * of @p recur. Returns NULL if the conversion was unsuccessful.
 *
 * @see icalrecurrencetype_as_string
 */
LIBICAL_ICAL_EXPORT char *icalrecurrencetype_as_string_r(struct icalrecurrencetype *recur);

/*
 * Recurrence iteration routines
 */

typedef struct icalrecur_iterator_impl icalrecur_iterator;

/**
 * Creates a new recurrence rule iterator starting at DTSTART.
 *
 * @param rule a pointer to a valid icalrecurrencetype
 * @param dtstart a valid icaltimetype to use for the DTSART
 *
 * @note The new iterator may keep a reference to the passed rule.
 * It must not be modified as long as the iterator is in use.
 *
 * @return a pointer to the new icalrecur_iterator.
 */
LIBICAL_ICAL_EXPORT icalrecur_iterator *icalrecur_iterator_new(struct icalrecurrencetype *rule,
                                                               struct icaltimetype dtstart);

/**
 * Sets the date-time at which the iterator will start,
 *
 * @note CAN NOT be used with RRULEs that contain COUNT.
 *
 * @param impl is a pointer to a valid icalrecur_iterator
 * @param start is the starting icaltimetype that must be between DTSTART and UNTIL.
 *
 * @return if the start was successfully set; false otherwise.
 *
 * @since 3.0
 */
LIBICAL_ICAL_EXPORT bool icalrecur_iterator_set_start(icalrecur_iterator *impl,
                                                      struct icaltimetype start);

/**
 * Sets the date-time at which the iterator will stop at the latest.
 *
 * @param impl is a pointer to a valid icalrecur_iterator
 * @param end is the ending icaltimetype
 *
 * Values equal to or greater than end will not be returned by the iterator.
 *
 * @return if the end was successfully set; false otherwise.
 */
LIBICAL_ICAL_EXPORT bool icalrecur_iterator_set_end(icalrecur_iterator *impl,
                                                    struct icaltimetype end);

/**
 * Sets the date-times over which the iterator will run,
 *
 * @param impl is a pointer to a valid icalrecur_iterator
 * @param to is the starting icaltimetype
 * @param from is the ending icaltimetype that must be between DSTART and UNTIL
 *
 * If @p to is null time, the forward iterator will return values
 * up to and including UNTIL (if present), otherwise up to the year 2582.
 *
 * if @p to is non-null time and later than @p from,
 * the forward iterator will return values up to and including 'to'.
 *
 * If @p to is non-null time and earlier than @p from,
 * the reverse iterator will be set to start at @p from
 * and will return values down to and including @p to.
 *
 * NOTE: CAN NOT be used with RRULEs that contain COUNT.
 *
 * @return true if the range was successfully set; false otherwise.
 *
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT bool icalrecur_iterator_set_range(icalrecur_iterator *impl,
                                                      struct icaltimetype from,
                                                      struct icaltimetype to);

/**
 * Gets the next occurrence from an icalrecur_iterator.
 *
 * @param impl a pointer to a valid icalrecur_iterator
 *
 * @return an icaltimetype for the next occurrence.
 *
 */
LIBICAL_ICAL_EXPORT struct icaltimetype icalrecur_iterator_next(icalrecur_iterator *);

/**
 * Gets the previous occurrence from an icalrecur_iterator.
 *
 * @param impl a pointer to a valid icalrecur_iterator
 *
 * @return an icaltimetype for the previous occurrence.
 *
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT struct icaltimetype icalrecur_iterator_prev(icalrecur_iterator *impl);

/**
 * Frees the specified icalrecur_iterator.
 *
 * @param impl a pointer to a valid icalrecur_iterator
 */
LIBICAL_ICAL_EXPORT void icalrecur_iterator_free(icalrecur_iterator *impl);

/**
 * Fills an array with the 'count' number of occurrences generated by the rrule.
 *
 * Specifically, this fills an array up with at most 'count' icaltime_t values,
 * each representing an occurrence time in seconds past the POSIX epoch.
 *
 * @note The times are returned in UTC, but the times are calculated in
 * local time. You will have to convert the results back into local time
 * before using them.
 *
 * @param rule a pointer to a char string containing the RRULE to use
 * @param start a valid starting date-time
 * @param count the length of the provided icalarray
 * @param array a pointer to an array of @p count icaltime_t values
 * resulting from expanding the specified RRULE @p rule.
 *
 * @return true if the @p rule could be converted to a string; false otherwise.
 *
 */
LIBICAL_ICAL_EXPORT bool icalrecur_expand_recurrence(const char *rule, icaltime_t start,
                                                     int count, icaltime_t *array);

/**
 * How the library handles  RRULEs with invalid BYxxx part combos.
 */
typedef enum ical_invalid_rrule_handling
{
    /** treat as a error; the internal icalerror to :ICAL_MALFORMEDDATA_ERROR */
    ICAL_RRULE_TREAT_AS_ERROR = 0,
    /** discard and continue normally */
    ICAL_RRULE_IGNORE_INVALID = 1
} ical_invalid_rrule_handling;

/**
 * Gets how the library should handle invalid RRULEs.
 *
 * The default library setting is ::ICAL_RRULE_TREAT_AS_ERROR.
 *
 * @return the library's current cal_invalid_rrule_handling setting.
 * @see ical_set_invalid_rrule_handling_setting
 */
LIBICAL_ICAL_EXPORT ical_invalid_rrule_handling ical_get_invalid_rrule_handling_setting(void);

/**
 * Sets how the library should handle invalid RRULEs.
 *
 * @param newSetting the ical_invalid_rrule_handling to use.
 * @see ical_get_invalid_rrule_handling_setting
 */
LIBICAL_ICAL_EXPORT void ical_set_invalid_rrule_handling_setting(
    ical_invalid_rrule_handling newSetting);

#endif /* ICALRECUR_H */
