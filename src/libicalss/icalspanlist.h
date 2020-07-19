/*======================================================================
 FILE: icalspanlist.h
 CREATOR: eric 21 Aug 2000

 (C) COPYRIGHT 2000, Eric Busboom <eric@civicknowledge.com>

 This library is free software; you can redistribute it and/or modify
 it under the terms of either:

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: https://www.gnu.org/licenses/lgpl-2.1.html

 Or:

    The Mozilla Public License Version 2.0. You may obtain a copy of
    the License at https://www.mozilla.org/MPL/
=========================================================================*/
#ifndef ICALSPANLIST_H
#define ICALSPANLIST_H

#include "libical_icalss_export.h"
#include "icalset.h"

/** @file icalspanlist.h
 *  @brief Code that supports collections of free/busy spans of time
 */

typedef struct icalspanlist_impl icalspanlist;

/** @brief Makes a free list from a set of VEVENT components.
 *
 *  @param set    A valid icalset containing VEVENTS
 *  @param start  The free list starts at this date/time
 *  @param end    The free list ends at this date/time
 *
 *  @return        A spanlist corresponding to the VEVENTS
 *
 * Given a set of components, a start time and an end time
 * return a spanlist that contains the free/busy times.
 * @p Start and @p end should be in UTC.
 */
LIBICAL_ICALSS_EXPORT icalspanlist *icalspanlist_new(icalset *set,
                                                     struct icaltimetype start,
                                                     struct icaltimetype end);

/** @brief Destructor.
 *  @param s A valid icalspanlist
 *
 *  Frees the memory associated with the spanlist.
 */
LIBICAL_ICALSS_EXPORT void icalspanlist_free(icalspanlist *spl);

/** @brief Finds the next free time span in a spanlist.
 *
 *  @param  sl     The spanlist to search.
 *  @param  t      The time to start looking.
 *
 *  Given a spanlist and a time, finds the next period of time
 *  that is free.
 */
LIBICAL_ICALSS_EXPORT struct icalperiodtype icalspanlist_next_free_time(icalspanlist *sl,
                                                                        struct icaltimetype t);

/** @brief (Debug) print out spanlist to STDOUT.
 *  @param sl A valid icalspanlist.
 */
LIBICAL_ICALSS_EXPORT void icalspanlist_dump(icalspanlist *s);

/** @brief Returns a VFREEBUSY component for a spanlist.
 *
 *   @param sl         A valid icalspanlist, from icalspanlist_new()
 *   @param organizer  The organizer specified as "MAILTO:user@domain"
 *   @param attendee   The attendee specified as "MAILTO:user@domain"
 *
 *   @return            A valid icalcomponent or NULL.
 *
 * This function returns a VFREEBUSY component for the given spanlist.
 * The start time is mapped to DTSTART, the end time to DTEND.
 * Each busy span is represented as a separate FREEBUSY entry.
 * An attendee parameter is required, and organizer parameter is
 * optional.
 */
LIBICAL_ICALSS_EXPORT icalcomponent *icalspanlist_as_vfreebusy(icalspanlist *sl,
                                                               const char *organizer,
                                                               const char *attendee);

/** @brief Returns an hour-by-hour array of free/busy times over a
 *         given period.
 *
 *  @param sl        A valid icalspanlist
 *  @param delta_t   The time slice to divide by, in seconds.  Default 3600.
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
LIBICAL_ICALSS_EXPORT int *icalspanlist_as_freebusy_matrix(icalspanlist *span, int delta_t);

/** @brief Constructs an icalspanlist from a VFREEBUSY component */
/** @brief Constructs an icalspanlist from the VFREEBUSY component of
 *         an icalcomponent.
 *
 *   @param   comp     A valid icalcomponent.
 *
 *   @return           A valid icalspanlist or NULL if no VFREEBUSY section.
 *
 */
LIBICAL_ICALSS_EXPORT icalspanlist *icalspanlist_from_vfreebusy(icalcomponent *comp);

#endif
