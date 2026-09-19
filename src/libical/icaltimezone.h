/*======================================================================
 FILE: icaltimezone.h
 CREATOR: Damon Chaplin 15 March 2001

 SPDX-FileCopyrightText: 2001, Damon Chaplin <damon@ximian.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
======================================================================*/

/**
 * @file icaltimezone.h
 * @brief Timezone handling routines
 */

#ifndef ICALTIMEZONE_H
#define ICALTIMEZONE_H

#include "libical_ical_export.h"
#include "icalcomponent.h"

#include <stdbool.h>
#include <stdio.h>

#if !defined(ICALTIMEZONE_DEFINED)
#define ICALTIMEZONE_DEFINED
/**
 * An opaque struct representing a timezone.
 * We declare this here to avoid a circular dependency.
 */
typedef struct _icaltimezone icaltimezone;
#endif

/*
 * Creating/Destroying individual icaltimezones.
 */

/**
 * Creates a new icaltimezone.
 * The data structure contains all null values and is essentially invalid.
 *
 * @return a pointer to the new invalid icaltimezone.
 */
LIBICAL_ICAL_EXPORT icaltimezone *icaltimezone_new(void);

/**
 * Deep clones an icaltimezone.
 *
 * Invalid icaltimezone data structures can also be cloned.
 *
 * @param originalzone is a pointer to an icaltimezone.
 *
 * @return a pointer to the cloned icaltimezone.
 */
LIBICAL_ICAL_EXPORT icaltimezone *icaltimezone_copy(const icaltimezone *originalzone);

/** @brief Frees all memory used for the icaltimezone.
 * @param zone The icaltimezone to be freed
 * @param free_struct Whether to free the icaltimezone struct as well
 */
LIBICAL_ICAL_EXPORT void icaltimezone_free(icaltimezone *zone, int free_struct);

/**
 * Sets the prefix to be used for tzid's generated from system tzdata.
 * Must be globally unique (such as a domain name owned by the developer
 * of the calling application), and begin and end with forward slashes.
 * The string must be less than 256 chars long.
 *
 * No error or sanity checking is performed in this function; the caller is
 * entirely responsible for using a proper tzid string.
 *
 * Do not change or de-allocate the string buffer after calling this.
 *
 * @param new_prefix a string (properly formatted and less then 256 characters long)
 * representing the tzid for the system tzdata. If not specified, the library default
 * "/freeassociation.sourceforge.net/" is used.
 */
LIBICAL_ICAL_EXPORT void icaltimezone_set_tzid_prefix(const char *new_prefix);

/**
 * Get the current setting of the tzid prefix.
 *
 * @return a pointer to a char string containing the current tzid prefix.
 */
LIBICAL_ICAL_EXPORT const char *icaltimezone_tzid_prefix(void);

/*
 * Accessing timezones.
 */

/** @brief Releases builtin timezone memory. */
LIBICAL_ICAL_EXPORT void icaltimezone_free_builtin_timezones(void);

/**
 * Gets an icalarray of icaltimezone structs, one for each builtin timezone.
 *
 * This will load and parse the zones.tab file to get the
 * timezone names and their coordinates. It will not load the
 * VTIMEZONE data for any timezones.
 *
 * @return a pointer to an icalarray containing the list of icaltimezone structs
 * associated with the buildtin timezones.
 */
LIBICAL_ICAL_EXPORT icalarray *icaltimezone_get_builtin_timezones(void);

/**
 * Gets a single builtin timezone, given its Olson location.
 *
 * @param location a pointer to a char string containing the Olson location to search
 *
 * @return a pointer to a builtin icaltimezone that matches the specified
 * Olson location.  NULL is returned if the location is empty or if the
 * search fails.
 */
LIBICAL_ICAL_EXPORT icaltimezone *icaltimezone_get_builtin_timezone(const char *location);

/**
 * Gets a single builtin timezone, given its offset from UTC.
 *
 * @param offset the offset from UTC to search
 * @param tzname a pointer to a char string containing a TZNAME to search.
 *
 * @return a pointer to a builtin icaltimezone that matches the specified
 * @p offset and @p tzname. NULL is returned if tzname is NULL or if the
 * search fails.
 */
LIBICAL_ICAL_EXPORT icaltimezone *icaltimezone_get_builtin_timezone_from_offset(int offset,
                                                                                const char *tzname);

/**
 * Gets Returns a single builtin timezone, given its TZID.
 *
 * @param tzid a pointer to a char string containing the TZID to find
 *
 * @return a pointer to a builting icaltimezone for the specified TZID.
 */
LIBICAL_ICAL_EXPORT icaltimezone *icaltimezone_get_builtin_timezone_from_tzid(const char *tzid);

/**
 * Gets the UTC timezone.
 *
 * @return a pointer to an icaltimezone representing the UTC timezone.
 */
LIBICAL_ICAL_EXPORT icaltimezone *icaltimezone_get_utc_timezone(void);

/**
 * Gets the TZID of a timezone.
 *
 * @param zone a pointer to a valid icaltimezone
 *
 * @return a pointer to a char string containing the TZID for the
 * specified icaltimezone. NULL will be returned for floating timezones.
  */
LIBICAL_ICAL_EXPORT const char *icaltimezone_get_tzid(icaltimezone *zone);

/**
 * Gets the LOCATION of a timezone.
 *
 * @param zone a pointer to a valid icaltimezone
 *
 * @return a pointer to a char string containing the LOCATION for the
 * specified timezone. NULL will be returned for floating timezones.
 */
LIBICAL_ICAL_EXPORT const char *icaltimezone_get_location(const icaltimezone *zone);

/**
 * Gets the TZNAME properties used in the latest STANDARD and DAYLIGHT
 * components a a builtin timezone.
 *
 * If they are the same it will return just one, e.g. "LMT".
 * If they are different it will format them like "EST/EDT".
 *
 * @param zone is a pointer to a valid icaltimezone
 *
 * @return a pointer to a char string containing the TZNAME for the
 * specified icaltimezone. NULL will be returned for floating timezones.
*/
LIBICAL_ICAL_EXPORT const char *icaltimezone_get_tznames(icaltimezone *zone);

/**
 * Gets the latitude of a builtin timezone.
 *
 * @param zone a pointer to a valid icaltimezone
 *
 * @return the latitude of the specified icaltimezone as a double.
 */
LIBICAL_ICAL_EXPORT double icaltimezone_get_latitude(const icaltimezone *zone);

/**
 * Gets the longitude of a builtin timezone.
 *
 * @param zone a pointer to a valid icaltimezone
 *
 * @return the longitude of the specified icaltimezone as a double.
 */
LIBICAL_ICAL_EXPORT double icaltimezone_get_longitude(const icaltimezone *zone);

/**
 * Gets the VTIMEZONE component of a timezone.
 *
 * @param zone is a pointer to a valid icaltimezone.
 *
 * @return a pointer to the VTIMEZONE component of the specified icaltimezone.
 */
LIBICAL_ICAL_EXPORT icalcomponent *icaltimezone_get_component(icaltimezone *zone);

/**
 * Sets the VTIMEZONE component of an icaltimezone, initializing the
 * tzid, location & tzname fields.
 *
 * @param zone a pointer to a valid icaltimezone
 * @param comp is a pointer to a valid icalcomponent to use
 *
 * @returns true on success or false on failure, i.e.  no TZID was found.
 */
LIBICAL_ICAL_EXPORT bool icaltimezone_set_component(icaltimezone *zone, icalcomponent *comp);

/**
 * Get the timezone display name.
 *
 * We prefer to use the Olson city name, but fall back on the TZNAME, or finally
 * the TZID. We don't want to use "" as it may be wrongly interpreted as a
 * floating time. Do not free the returned string.
 *
 * @param zone a pointer to a valid icaltimezone
 *
 * @return a pointer to a char string containing the timezone display name.
 */
LIBICAL_ICAL_EXPORT const char *icaltimezone_get_display_name(icaltimezone *zone);

/**
 * Rotate a time from one timezone to another.
 *
 * @param tt is a the icaltimetype to rotate. If @p tt represents a date (ie. there is no associated time)
 * then no conversion is performed.
 * @param from_zone is the timezone to rotate from. Any timezone specified inside the icaltimetype is ignored.
 * If @p from_zone is NULL then no conversion is performed.
 * @param to_zone is the timezone to rotate to.  If @p to_zone is NULL then the time is rotated to UTC.
 *
 */

LIBICAL_ICAL_EXPORT void icaltimezone_convert_time(struct icaltimetype *tt,
                                                   icaltimezone *from_zone,
                                                   icaltimezone *to_zone);

/*
 * Getting offsets from UTC.
 */

/**
 * Calculates the UTC offset of a given local time in the given timezone.
 *
 * It is the number of seconds to add to UTC to get local time.
 *
 * @param zone is a pointer to a valid icaltimezone
 * @param tt is an icaltimetype with the local time
 * @param is_daylight is a pointer to an int which will be set to 1 (true)
 * on return if the time is in daylight-savings; in non-daylight-saving
 * it will be set to 0 (false).
 *
 * @return the UTC offset corresponding to the timezone change. For local times
 * and UTC a 0 offset is returned. Additionally, if @p tt is NULL or if the
 * @p tt year is too large a 0 offset is returned.
 */
LIBICAL_ICAL_EXPORT int icaltimezone_get_utc_offset(icaltimezone *zone,
                                                    const struct icaltimetype *tt, int *is_daylight);

/**
 * Calculates the UTC offset of a given UTC time in the given timezone.
 *
 * It is the number of seconds to add to UTC to get local time.
 *
 * @param zone is a pointer to a valid icaltimezone
 * @param tt is an icaltimetype with the UTC time
 * @param is_daylight is a pointer to an int which will be set to 1 (true)
 * on return if the time is in daylight-savings; in non-daylight-saving
 * it will be set to 0 (false).
 *
 * @return the UTC offset corresponding to the timezone change. For local times
 * and UTC a 0 offset is returned.
 */
LIBICAL_ICAL_EXPORT int icaltimezone_get_utc_offset_of_utc_time(icaltimezone *zone,
                                                                const struct icaltimetype *tt,
                                                                int *is_daylight);

/// @cond PRIVATE

/*
 * Handling arrays of timezones. For internal use.
 */
LIBICAL_ICAL_NO_EXPORT icalarray *icaltimezone_array_new(void);

LIBICAL_ICAL_NO_EXPORT void icaltimezone_array_append_from_vtimezone(icalarray *timezones,
                                                                     icalcomponent *child);

LIBICAL_ICAL_NO_EXPORT void icaltimezone_array_free(icalarray *timezones);
/// @endcond

/**
 * Applies a list of timezone changes from an array of changes until an end year.
 *
 * @param comp a pointer to a valid icalcomponent
 * @param end_year the end year
 * @param changes a pointer to an icalarray containing the changes to be applied
 *
 */
LIBICAL_ICAL_EXPORT void icaltimezone_expand_vtimezone(icalcomponent *comp,
                                                       int end_year,
                                                       icalarray *changes);

/**
 * Gets the LOCATION or X-LIC-LOCATION property from a VTIMEZONE.
 *
 * @param component is a pointer to a valid icalcomponent.
 *
 * @return a pointer to a char string containing the LOCATION or X-LIC-LOCATION
 * property for the specified icalcomponent.
 */
LIBICAL_ICAL_EXPORT char *icaltimezone_get_location_from_vtimezone(icalcomponent *component);

/**
 * Gets the TZNAMEs used for the last STANDARD & DAYLIGHT components in a VTIMEZONE.
 *
 * If both STANDARD and DAYLIGHT components use the same TZNAME, it
 * returns that. If they use different TZNAMEs, it formats them like
 * "EST/EDT". The returned string should be freed by the caller.
 *
 * @param component a pointer to a valid icalcomponent
 *
 * @return a pointer to a char string containing the TZNAMEs.
 */
LIBICAL_ICAL_EXPORT char *icaltimezone_get_tznames_from_vtimezone(icalcomponent *component);

/**
 * Truncate a VTIMEZONE component to the given start and end times.
 *
 * If either time is null, then no truncation will occur at that point.
 * If either time is non-null, then it MUST be specified as UTC.
 * If the start time is non-null and ms_compatible is false,
 * then the DTSTART of RRULEs will be adjusted to occur after the start time.
 *
 * @param vtz is a pointer to a valid VTIMEZONE icalcomponent
 * @param start is the starting icaltimetype
 * @param end is the ending icaltimetype
 * @param ms_compatible is a flag indicating if the truncation should be
 * compatible with Microsoft Outlook/Exchange (which doesn't appear to
 * like truncating the frontend of RRULEs).
 *
 * @since 3.0.6
 */
LIBICAL_ICAL_EXPORT void icaltimezone_truncate_vtimezone(icalcomponent *vtz,
                                                         icaltimetype start,
                                                         icaltimetype end,
                                                         bool ms_compatible);

/*
 * @par Handling the default location the timezone files
 */

/**
 * Gets the fullpath to the system zoneinfo directory (where zone.tab lives).
 * The returned value points to static memory inside the library and should
 * not try to be freed.
 *
 * If the TZDIR variable appears in the environment, it will be searched first
 * for zone.tab.  If zone.tab is not located in TZDIR (or if TZDIR is not in the
 * environment), then a list of well-known paths where the system zone.tab
 * typically is installed is searched.
 *
 * @return a pointer to a char string containing the system zoneinfo directory name.
 *
 * @since 4.0 previously known as icaltzutil_get_zone_directory
 */
LIBICAL_ICAL_EXPORT const char *icaltimezone_get_system_zone_directory(void);

/**
 * Sets the fullpath to the system zoneinfo directory (zone.tab must reside in there).
 * @param zonepath const character string containing the fullpath to the zoneinfo directory.
 *
 * The internal zoneinfo path can be cleared if @p zonepath is empty or NULL.
 * @since 4.0 previously known as icaltzutil_set_zone_directory
 */
LIBICAL_ICAL_EXPORT void icaltimezone_set_system_zone_directory(const char *zonepath);

/**
 * Gets the directory to look for the zonefiles, either system or builtin.
 *
 * @return a pointer to a char string containing the zonefile directory name.
 */
LIBICAL_ICAL_EXPORT const char *icaltimezone_get_zone_directory(void);

/**
 * Sets the directory to look for the zonefiles, either system or builtin.
 *
 * The memory allocated by this function should be freed using icaltimezone_set_zone_directory().
 *
 * @param path is a char string containing the directory path for zoneinfo data.
 */
LIBICAL_ICAL_EXPORT void icaltimezone_set_zone_directory(const char *path);

/**
 * Frees the zonefile directory.
 *
 * Frees the memory allocated by the icaltimezone_set_zone_directory()
 */
LIBICAL_ICAL_EXPORT void icaltimezone_free_zone_directory(void);

/**
 * Set the library to use builtin timezone data.
 *
 * @param set if true, configures the library to use builtin timezone data;
 * otherwise, the system timezone data is used.
 */
LIBICAL_ICAL_EXPORT void icaltimezone_set_builtin_tzdata(bool set);

/**
 * Get if the library is using builtin timezone data.
 *
 * @return true if the builtin timezone data is being used; false otherwise.
 */
LIBICAL_ICAL_EXPORT bool icaltimezone_get_builtin_tzdata(void);

/*
 * Debugging Output.
 */

/**
 * Outputs a list of timezone changes for the given timezone to the
 * given file, up to the maximum year given.
 *
 * We compare this output with the output from 'vzic --dump-changes' to
 * make sure that we are consistent. (vzic is the Olson timezone
 * database to VTIMEZONE converter.)
 *
 * The output format is:
 *
 *      Zone-Name [tab] Date [tab] Time [tab] UTC-Offset
 *
 * The Date and Time fields specify the time change in UTC.
 *
 * The UTC Offset is for local (wall-clock) time. It is the amount of time
 * to add to UTC to get local time.
 *
 * @param zone is a pointer to a valid icaltimezone to use
 * @param max_year is the maximum year to process
 * @param fp is a pointer to FILE for a file that is opened for writing
 *
 * @return true. always.
 */
LIBICAL_ICAL_EXPORT bool icaltimezone_dump_changes(icaltimezone *zone, int max_year, FILE *fp);

#endif /* ICALTIMEZONE_H */
