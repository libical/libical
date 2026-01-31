/*======================================================================
 FILE: icalspanlist.h
 CREATOR: eric 21 Aug 2000

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
=========================================================================*/
#ifndef ICALSPANLIST_H
#define ICALSPANLIST_H

#include "libical_icalss_export.h"
#include "icalset.h"

/**
 * @file icalspanlist.h
 * @brief Code that supports collections of free/busy spans of time
 */

/// @cond PRIVATE
struct icalpvl_list_t;
struct icalspanlist_impl {
    struct icalpvl_list_t *spans; /**< list of icaltime_span data **/
    struct icaltimetype start;    /**< start time of span **/
    struct icaltimetype end;      /**< end time of span **/
};
typedef struct icalspanlist_impl icalspanlist;
/// @endcond

/**
 * Makes a free list from a set of VEVENT components.
 *
 *  @param set a pointer to valid icalset containing VEVENTS
 *  @param start  the free list starts at this date/time
 *  @param end the free list ends at this date/time
 *
 *  @return a spanlist corresponding to the VEVENTS
 *
 * Given a set of components, a start time and an end time
 * return a spanlist that contains the free/busy times.
 * @p start and @p end should be in UTC.
 */
LIBICAL_ICALSS_EXPORT icalspanlist *icalspanlist_new(icalset *set,
                                                     struct icaltimetype start,
                                                     struct icaltimetype end);

/**
 * Destructor.
 *
 * @param sl A valid icalspanlist
 *
 *  Frees the memory associated with the spanlist.
 */
LIBICAL_ICALSS_EXPORT void icalspanlist_free(icalspanlist *sl);

/**
 * Finds the next free time span in a spanlist.
 *
 * @param  sl a pointer to a valid icalspanlist to search
 * @param  t the time to start looking.
 *
 * Given a spanlist and a time, finds the next period of free time.
 *
 * @return an icalperiodtype representing the free type period; if no free time is
 * available then an invalid icalperiodtype is returned.
 */
LIBICAL_ICALSS_EXPORT struct icalperiodtype icalspanlist_next_free_time(icalspanlist *sl,
                                                                        struct icaltimetype t);

/**
 * (Debug) print out spanlist to STDOUT.
 *
 *  @param sl a pointer to a valid icalspanlist.
 */
LIBICAL_ICALSS_EXPORT void icalspanlist_dump(icalspanlist *sl);

/**
 * Returns a VFREEBUSY component for a spanlist.
 *
 * @param sl a pointer to a valid icalspanlist, from icalspanlist_new()
 * @param organizer  the organizer specified as "MAILTO:user@domain"
 * @param attendee the attendee specified as "MAILTO:user@domain"
 *
 * @return a pointer to an icalcomponent, NULL if the conversion failed.
 *
 * This function returns a VFREEBUSY component for the given spanlist.
 * The start time is mapped to DTSTART, the end time to DTEND.
 * Each busy span is represented as a separate FREEBUSY entry.
 * An attendee parameter is required, and organizer parameter is optional.
 */
LIBICAL_ICALSS_EXPORT icalcomponent *icalspanlist_as_vfreebusy(icalspanlist *sl,
                                                               const char *organizer,
                                                               const char *attendee);

/**
 * Returns an hour-by-hour array of free/busy times over a given period.
 *
 *  @param spanlist a pointer to a valid icalspanlist
 *  @param delta_t  the time slice to divide by, in seconds.  Default 3600.
 *
 *  @return A pointer to an array of integers containing the number of
 *       busy events in each delta_t time period.  The final entry
 *       contains the value -1.
 *
 *  This calculation is somewhat tricky.  This is due to the fact that
 *  the time range contains the start time, but does not contain the
 *  end time.  To perform a proper calculation we subtract one second
 *  off the end times to get a true containing time.
 *
 *  Also note that if you supplying a spanlist that does not start or
 *  end on a time boundary divisible by delta_t you may get results
 *  that are not quite what you expect.
 */
LIBICAL_ICALSS_EXPORT int *icalspanlist_as_freebusy_matrix(icalspanlist *spanlist, int delta_t);

/**
 * Constructs an icalspanlist from the VFREEBUSY component of an icalcomponent.
 *
 *   @param comp a pointer to a valid icalcomponent
 *
 *   @return a pointer to an icalspanlist or NULL if no VFREEBUSY section.
 */
LIBICAL_ICALSS_EXPORT icalspanlist *icalspanlist_from_vfreebusy(icalcomponent *comp);

#endif
