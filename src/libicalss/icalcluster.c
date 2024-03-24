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
    struct icalcluster_impl *impl;

    if ((impl = (struct icalcluster_impl *)malloc(sizeof(struct icalcluster_impl))) == 0) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return 0;
    }

    memset(impl, 0, sizeof(struct icalcluster_impl));
    strcpy(impl->id, ICALCLUSTER_ID);

    return impl;
}

icalcluster *icalcluster_new(const char *key, icalcomponent *data)
{
    struct icalcluster_impl *impl = icalcluster_new_impl();

    assert(impl->data == 0);

    impl->key = strdup(key);
    impl->changed = 0;
    impl->data = 0;

    if (data != NULL) {
        if (icalcomponent_isa(data) != ICAL_XROOT_COMPONENT) {
            impl->data = icalcomponent_new(ICAL_XROOT_COMPONENT);
            icalcomponent_add_component(impl->data, data);
        } else {
            impl->data = icalcomponent_clone(data);
        }
    } else {
        impl->data = icalcomponent_new(ICAL_XROOT_COMPONENT);
    }

    return impl;
}

/**
 * @brief Deep clone an icalcluster to a new one
 */

icalcluster *icalcluster_clone(const icalcluster *data)
{
    struct icalcluster_impl *old = (struct icalcluster_impl *)data;
    struct icalcluster_impl *impl = icalcluster_new_impl();

    impl->key = strdup(old->key);
    impl->data = icalcomponent_clone(old->data);
    impl->changed = 0;

    return impl;
}

icalcluster *icalcluster_new_clone(const icalcluster *data)
{
    return icalcluster_clone(data);
}

void icalcluster_free(icalcluster *impl)
{
    icalerror_check_arg_rv((impl != 0), "cluster");

    if (impl->key != 0) {
        free(impl->key);
        impl->key = 0;
    }

    if (impl->data != 0) {
        icalcomponent_free(impl->data);
        impl->data = 0;
    }

    free(impl);
}

const char *icalcluster_key(icalcluster *impl)
{
    icalerror_check_arg_rz((impl != 0), "cluster");

    return impl->key;
}

int icalcluster_is_changed(icalcluster *impl)
{
    icalerror_check_arg_rz((impl != 0), "cluster");

    return impl->changed;
}

void icalcluster_mark(icalcluster *impl)
{
    icalerror_check_arg_rv((impl != 0), "cluster");

    impl->changed = 1;
}

void icalcluster_commit(icalcluster *impl)
{
    icalerror_check_arg_rv((impl != 0), "cluster");

    impl->changed = 0;
}

icalcomponent *icalcluster_get_component(icalcluster *impl)
{
    icalerror_check_arg_rz((impl != 0), "cluster");

    if (icalcomponent_isa(impl->data) != ICAL_XROOT_COMPONENT) {
        char *obj;

        icalerror_warn("The top component is not an XROOT");
        obj = icalcomponent_as_ical_string_r(impl->data);
        fprintf(stderr, "%s\n", obj);
        free(obj);
        abort();
    }

    return impl->data;
}

icalerrorenum icalcluster_add_component(icalcluster *impl, icalcomponent *child)
{
    icalerror_check_arg_re((impl != 0), "cluster", ICAL_BADARG_ERROR);
    icalerror_check_arg_re((child != 0), "child", ICAL_BADARG_ERROR);

    icalcomponent_add_component(impl->data, child);
    icalcluster_mark(impl);

    return ICAL_NO_ERROR;
}

icalerrorenum icalcluster_remove_component(icalcluster *impl, icalcomponent *child)
{
    icalerror_check_arg_re((impl != 0), "cluster", ICAL_BADARG_ERROR);
    icalerror_check_arg_re((child != 0), "child", ICAL_BADARG_ERROR);

    icalcomponent_remove_component(impl->data, child);
    icalcluster_mark(impl);

    return ICAL_NO_ERROR;
}

int icalcluster_count_components(icalcluster *impl, icalcomponent_kind kind)
{
    icalerror_check_arg_re((impl != 0), "cluster", ICAL_BADARG_ERROR);

    return icalcomponent_count_components(impl->data, kind);
}

/** @brief Iterate through components
 */
icalcomponent *icalcluster_get_current_component(icalcluster *impl)
{
    icalerror_check_arg_rz((impl != 0), "cluster");

    return icalcomponent_get_current_component(impl->data);
}

icalcomponent *icalcluster_get_first_component(icalcluster *impl)
{
    icalerror_check_arg_rz((impl != 0), "cluster");

    return icalcomponent_get_first_component(impl->data, ICAL_ANY_COMPONENT);
}

icalcomponent *icalcluster_get_next_component(icalcluster *impl)
{
    icalerror_check_arg_rz((impl != 0), "cluster");

    return icalcomponent_get_next_component(impl->data, ICAL_ANY_COMPONENT);
}
