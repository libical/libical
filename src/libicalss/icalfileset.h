/*======================================================================
 FILE: icalfileset.h
 CREATOR: eric 23 December 1999

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>

 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 The Original Code is eric. The Initial Developer of the Original
 Code is Eric Busboom
======================================================================*/

#ifndef ICALFILESET_H
#define ICALFILESET_H

#include "libical_icalss_export.h"
#include "icalcluster.h"
#include "icalset.h"

typedef struct icalfileset_impl icalfileset;

LIBICAL_ICALSS_EXPORT icalset *icalfileset_new(const char *path);

LIBICAL_ICALSS_EXPORT icalset *icalfileset_new_reader(const char *path);

LIBICAL_ICALSS_EXPORT icalset *icalfileset_new_writer(const char *path);

LIBICAL_ICALSS_EXPORT icalset *icalfileset_init(icalset *set, const char *dsn, void *options);

LIBICAL_ICALSS_EXPORT icalcluster *icalfileset_produce_icalcluster(const char *path);

LIBICAL_ICALSS_EXPORT void icalfileset_free(icalset *cluster);

LIBICAL_ICALSS_EXPORT const char *icalfileset_path(icalset *cluster);

/* Mark the cluster as changed, so it will be written to disk when it
   is freed. Commit writes to disk immediately. */
LIBICAL_ICALSS_EXPORT void icalfileset_mark(icalset *set);

LIBICAL_ICALSS_EXPORT icalerrorenum icalfileset_commit(icalset *set);

LIBICAL_ICALSS_EXPORT icalerrorenum icalfileset_add_component(icalset *set, icalcomponent *child);

LIBICAL_ICALSS_EXPORT icalerrorenum icalfileset_remove_component(icalset *set,
                                                                 icalcomponent *child);

LIBICAL_ICALSS_EXPORT int icalfileset_count_components(icalset *set, icalcomponent_kind kind);

/**
 * Restricts the component returned by icalfileset_first, _next to those
 * that pass the gauge. _clear removes the gauge.
 */
LIBICAL_ICALSS_EXPORT icalerrorenum icalfileset_select(icalset *set, icalgauge *gauge);

/** @brief Clears the gauge **/
LIBICAL_ICALSS_EXPORT void icalfileset_clear(icalset *set);

/** @brief Gets and searches for a component by uid **/
LIBICAL_ICALSS_EXPORT icalcomponent *icalfileset_fetch(icalset *set,
                                                       icalcomponent_kind kind, const char *uid);

LIBICAL_ICALSS_EXPORT int icalfileset_has_uid(icalset *set, const char *uid);

LIBICAL_ICALSS_EXPORT icalcomponent *icalfileset_fetch_match(icalset *set, icalcomponent *c);

/**
 *  @brief Modifies components according to the MODIFY method of CAP.
 *
 *  Works on the currently selected components.
 */
LIBICAL_ICALSS_EXPORT icalerrorenum icalfileset_modify(icalset *set,
                                                       icalcomponent *oldcomp,
                                                       icalcomponent *newcomp);

/* Iterates through components. If a gauge has been defined, these
   will skip over components that do not pass the gauge */

LIBICAL_ICALSS_EXPORT icalcomponent *icalfileset_get_current_component(icalset *cluster);

LIBICAL_ICALSS_EXPORT icalcomponent *icalfileset_get_first_component(icalset *cluster);

LIBICAL_ICALSS_EXPORT icalcomponent *icalfileset_get_next_component(icalset *cluster);

/* External iterator for thread safety */
LIBICAL_ICALSS_EXPORT icalsetiter icalfileset_begin_component(icalset *set,
                                                              icalcomponent_kind kind,
                                                              icalgauge *gauge, const char *tzid);

LIBICAL_ICALSS_EXPORT icalcomponent *icalfilesetiter_to_next(icalset *set, icalsetiter *iter);

LIBICAL_ICALSS_EXPORT icalcomponent *icalfileset_form_a_matched_recurrence_component(icalsetiter *
                                                                                     itr);

/** Returns a reference to the internal component. **You probably should
   not be using this.** */

LIBICAL_ICALSS_EXPORT icalcomponent *icalfileset_get_component(icalset *cluster);

/**
 * @brief Options for opening an icalfileset.
 *
 * These options should be passed to the icalset_new() function
 */

typedef struct icalfileset_options
{
    int flags;                /**< flags for open() O_RDONLY, etc  */
    int mode;                 /**< file mode */
    int safe_saves;           /**< to lock or not */
    icalcluster *cluster;     /**< use this cluster to initialize data */
} icalfileset_options;

extern icalfileset_options icalfileset_options_default;

#endif /* !ICALFILESET_H */
