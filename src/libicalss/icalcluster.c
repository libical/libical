/*======================================================================
 FILE: icalcluster.c
 CREATOR: acampi 13 March 2002

 SPDX-FileCopyrightText: 2002 Andrea Campi <a.campi@inet.it>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
======================================================================*/

/**
 *
 * icalcluster is an utility class design to manage clusters of
 * icalcomponents on behalf of an implementation of icalset. This is
 * done in order to split out common behavior different classes might
 * need.
 * The definition of what exactly a cluster will contain depends on the
 * icalset subclass. At the basic level, an icluster is just a tuple,
 * with anything as key and an icalcomponent as value.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "icalcluster.h"
#include "icalclusterimpl.h"

#include <stdlib.h>

static icalcluster *icalcluster_new_impl(void)
{
    struct icalcluster_impl *cluster;

    if ((cluster = (struct icalcluster_impl *)malloc(sizeof(struct icalcluster_impl))) == 0) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return 0;
    }

    memset(cluster, 0, sizeof(struct icalcluster_impl));
    strcpy(cluster->id, ICALCLUSTER_ID);

    return cluster;
}

icalcluster *icalcluster_new(const char *key, icalcomponent *data)
{
    struct icalcluster_impl *cluster = icalcluster_new_impl();

    if (!cluster) {
        return NULL;
    }
    assert(cluster->data == 0);

    cluster->key = strdup(key);
    cluster->changed = 0;
    cluster->data = 0;

    if (data != NULL) {
        if (icalcomponent_isa(data) != ICAL_XROOT_COMPONENT) {
            cluster->data = icalcomponent_new(ICAL_XROOT_COMPONENT);
            icalcomponent_add_component(cluster->data, data);
        } else {
            cluster->data = icalcomponent_clone(data);
        }
    } else {
        cluster->data = icalcomponent_new(ICAL_XROOT_COMPONENT);
    }

    return cluster;
}

/**
 * @brief Deep clone an icalcluster to a new one
 */

icalcluster *icalcluster_clone(const icalcluster *old)
{
    struct icalcluster_impl *old_impl = (struct icalcluster_impl *)old;
    struct icalcluster_impl *cluster = icalcluster_new_impl();

    if (cluster) {
        cluster->key = strdup(old_impl->key);
        cluster->data = icalcomponent_clone(old_impl->data);
        cluster->changed = 0;
    }

    return cluster;
}

void icalcluster_free(icalcluster *cluster)
{
    if (!cluster) {
        return;
    }

    if (cluster->key != 0) {
        free(cluster->key);
        cluster->key = 0;
    }

    if (cluster->data != 0) {
        icalcomponent_free(cluster->data);
        cluster->data = 0;
    }

    free(cluster);
}

const char *icalcluster_key(const icalcluster *cluster)
{
    icalerror_check_arg_rz((cluster != 0), "cluster");

    return cluster->key;
}

int icalcluster_is_changed(const icalcluster *cluster)
{
    icalerror_check_arg_rz((cluster != 0), "cluster");

    return cluster->changed;
}

void icalcluster_mark(icalcluster *cluster)
{
    icalerror_check_arg_rv((cluster != 0), "cluster");

    cluster->changed = 1;
}

void icalcluster_commit(icalcluster *cluster)
{
    icalerror_check_arg_rv((cluster != 0), "cluster");

    cluster->changed = 0;
}

icalcomponent *icalcluster_get_component(const icalcluster *cluster)
{
    icalerror_check_arg_rz((cluster != 0), "cluster");

    if (icalcomponent_isa(cluster->data) != ICAL_XROOT_COMPONENT) {
        char *obj;

        icalerror_warn("The top component is not an XROOT");
        obj = icalcomponent_as_ical_string_r(cluster->data);
        fprintf(stderr, "%s\n", obj);
        free(obj);
        abort();
    }

    return cluster->data;
}

icalerrorenum icalcluster_add_component(icalcluster *cluster, icalcomponent *child)
{
    icalerror_check_arg_re((cluster != 0), "cluster", ICAL_BADARG_ERROR);
    icalerror_check_arg_re((child != 0), "child", ICAL_BADARG_ERROR);

    icalcomponent_add_component(cluster->data, child);
    icalcluster_mark(cluster);

    return ICAL_NO_ERROR;
}

icalerrorenum icalcluster_remove_component(icalcluster *cluster, icalcomponent *child)
{
    icalerror_check_arg_re((cluster != 0), "cluster", ICAL_BADARG_ERROR);
    icalerror_check_arg_re((child != 0), "child", ICAL_BADARG_ERROR);

    icalcomponent_remove_component(cluster->data, child);
    icalcluster_mark(cluster);

    return ICAL_NO_ERROR;
}

int icalcluster_count_components(icalcluster *cluster, icalcomponent_kind kind)
{
    icalerror_check_arg_re((cluster != 0), "cluster", ICAL_BADARG_ERROR);

    return icalcomponent_count_components(cluster->data, kind);
}

/** @brief Iterate through components
 */
icalcomponent *icalcluster_get_current_component(icalcluster *cluster)
{
    icalerror_check_arg_rz((cluster != 0), "cluster");

    return icalcomponent_get_current_component(cluster->data);
}

icalcomponent *icalcluster_get_first_component(icalcluster *cluster)
{
    icalerror_check_arg_rz((cluster != 0), "cluster");

    return icalcomponent_get_first_component(cluster->data, ICAL_ANY_COMPONENT);
}

icalcomponent *icalcluster_get_next_component(icalcluster *cluster)
{
    icalerror_check_arg_rz((cluster != 0), "cluster");

    return icalcomponent_get_next_component(cluster->data, ICAL_ANY_COMPONENT);
}
