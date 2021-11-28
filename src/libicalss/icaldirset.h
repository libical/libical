/*======================================================================
 FILE: icaldirset.h
 CREATOR: eric 28 November 1999

 (C) COPYRIGHT 2000, Eric Busboom <eric@civicknowledge.com>

 This library is free software; you can redistribute it and/or modify
 it under the terms of either:

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: https://www.gnu.org/licenses/lgpl-2.1.html

 Or:

    The Mozilla Public License Version 2.0. You may obtain a copy of
    the License at https://www.mozilla.org/MPL/

 The Original Code is eric. The Initial Developer of the Original
 Code is Eric Busboom
======================================================================*/

/**
   @file   icaldirset.h

   @brief  icaldirset manages a database of ical components and offers
  interfaces for reading, writing and searching for components.

  icaldirset groups components in to clusters based on their DTSTAMP
  time -- all components that start in the same month are grouped
  together in a single file. All files in a sotre are kept in a single
  directory.

  The primary interfaces are icaldirset__get_first_component and
  icaldirset_get_next_component. These routine iterate through all of
  the components in the store, subject to the current gauge. A gauge
  is an icalcomponent that is tested against other components for a
  match. If a gauge has been set with icaldirset_select,
  icaldirset_first and icaldirset_next will only return components
  that match the gauge.

  The Store generated UIDs for all objects that are stored if they do
  not already have a UID. The UID is the name of the cluster (month &
  year as MMYYYY) plus a unique serial number. The serial number is
  stored as a property of the cluster.

*/

#ifndef ICALDIRSET_H
#define ICALDIRSET_H

#include "libical_icalss_export.h"
#include "icalset.h"

/* icaldirset Routines for storing, fetching, and searching for ical
 * objects in a database */

typedef struct icaldirset_impl icaldirset;

LIBICAL_ICALSS_EXPORT icalset *icaldirset_new(const char *path);

LIBICAL_ICALSS_EXPORT icalset *icaldirset_new_reader(const char *path);

LIBICAL_ICALSS_EXPORT icalset *icaldirset_new_writer(const char *path);

LIBICAL_ICALSS_EXPORT icalset *icaldirset_init(icalset *set, const char *dsn, void *options);

LIBICAL_ICALSS_EXPORT void icaldirset_free(icalset *set);

LIBICAL_ICALSS_EXPORT const char *icaldirset_path(icalset *set);

/* Marks the cluster as changed, so it will be written to disk when it
   is freed. Commit writes to disk immediately*/
LIBICAL_ICALSS_EXPORT void icaldirset_mark(icalset *set);

LIBICAL_ICALSS_EXPORT icalerrorenum icaldirset_commit(icalset *set);

/**
  This assumes that the top level component is a VCALENDAR, and there
   is an inner component of type VEVENT, VTODO or VJOURNAL. The inner
  component must have a DSTAMP property
*/
LIBICAL_ICALSS_EXPORT icalerrorenum icaldirset_add_component(icalset *store, icalcomponent *comp);
LIBICAL_ICALSS_EXPORT icalerrorenum icaldirset_remove_component(icalset *store,
                                                                icalcomponent *comp);

LIBICAL_ICALSS_EXPORT int icaldirset_count_components(icalset *store, icalcomponent_kind kind);

/* Restricts the component returned by icaldirset_first, _next to those
   that pass the gauge. _clear removes the gauge. */
LIBICAL_ICALSS_EXPORT icalerrorenum icaldirset_select(icalset *store, icalgauge *gauge);

LIBICAL_ICALSS_EXPORT void icaldirset_clear(icalset *store);

/* Gets a component by uid */
LIBICAL_ICALSS_EXPORT icalcomponent *icaldirset_fetch(icalset *store,
                                                      icalcomponent_kind kind, const char *uid);

LIBICAL_ICALSS_EXPORT int icaldirset_has_uid(icalset *store, const char *uid);

LIBICAL_ICALSS_EXPORT icalcomponent *icaldirset_fetch_match(icalset *set, icalcomponent *c);

/* Modifies components according to the MODIFY method of CAP. Works on
   the currently selected components. */
LIBICAL_ICALSS_EXPORT icalerrorenum icaldirset_modify(icalset *store,
                                                      icalcomponent *oldc, icalcomponent *newc);

/* Iterates through the components. If a gauge has been defined, these
   will skip over components that do not pass the gauge */

LIBICAL_ICALSS_EXPORT icalcomponent *icaldirset_get_current_component(icalset *store);

LIBICAL_ICALSS_EXPORT icalcomponent *icaldirset_get_first_component(icalset *store);

LIBICAL_ICALSS_EXPORT icalcomponent *icaldirset_get_next_component(icalset *store);

/* External iterator for thread safety */
LIBICAL_ICALSS_EXPORT icalsetiter icaldirset_begin_component(icalset *set,
                                                             icalcomponent_kind kind,
                                                             icalgauge *gauge, const char *tzid);

LIBICAL_ICALSS_EXPORT icalcomponent *icaldirsetiter_to_next(icalset *set, icalsetiter *i);

LIBICAL_ICALSS_EXPORT icalcomponent *icaldirsetiter_to_prior(icalset *set, icalsetiter *i);

typedef struct icaldirset_options
{
    int flags;            /**< flags corresponding to the open() system call O_RDWR, etc. */
} icaldirset_options;

#endif /* !ICALDIRSET_H */
