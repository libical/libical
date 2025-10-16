/*======================================================================
 FILE: icalcluster.h
 CREATOR: acampi 13 March 2002

 SPDX-FileCopyrightText: 2002 Andrea Campi <a.campi@inet.it>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
======================================================================*/

#ifndef ICALCLUSTER_H
#define ICALCLUSTER_H

#include "libical_icalss_export.h"
#include "icalcomponent.h"
#include "icalerror.h"

typedef struct icalcluster_impl icalcluster;

/**
 * @brief Create a cluster with a key/value pair.
 *
 * @todo Always do a deep copy.
 */
LIBICAL_ICALSS_EXPORT icalcluster *icalcluster_new(const char *key, icalcomponent *data);

/**
 * Deeply clone an icalcluster.
 * Returns a pointer to the memory for the newly cloned icalcluster.
 * @since 4.0
*/
LIBICAL_ICALSS_EXPORT icalcluster *icalcluster_clone(const icalcluster *old);

LIBICAL_ICALSS_EXPORT void icalcluster_free(icalcluster *cluster);

LIBICAL_ICALSS_EXPORT const char *icalcluster_key(const icalcluster *cluster);

LIBICAL_ICALSS_EXPORT int icalcluster_is_changed(const icalcluster *cluster);

LIBICAL_ICALSS_EXPORT void icalcluster_mark(icalcluster *cluster);

LIBICAL_ICALSS_EXPORT void icalcluster_commit(icalcluster *cluster);

LIBICAL_ICALSS_EXPORT icalcomponent *icalcluster_get_component(const icalcluster *cluster);

LIBICAL_ICALSS_EXPORT int icalcluster_count_components(icalcluster *cluster,
                                                       icalcomponent_kind kind);

LIBICAL_ICALSS_EXPORT icalerrorenum icalcluster_add_component(icalcluster *cluster,
                                                              icalcomponent *child);

LIBICAL_ICALSS_EXPORT icalerrorenum icalcluster_remove_component(icalcluster *cluster,
                                                                 icalcomponent *child);

LIBICAL_ICALSS_EXPORT icalcomponent *icalcluster_get_current_component(icalcluster *cluster);

LIBICAL_ICALSS_EXPORT icalcomponent *icalcluster_get_first_component(icalcluster *cluster);

LIBICAL_ICALSS_EXPORT icalcomponent *icalcluster_get_next_component(icalcluster *cluster);

#endif /* !ICALCLUSTER_H */
