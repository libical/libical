/*======================================================================
 FILE: icalattach-leak.c

 Copyright (C) 2019 Red Hat, Inc. <www.redhat.com>

 This library is free software; you can redistribute it and/or modify
 it under the terms of either:

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: https://www.gnu.org/licenses/lgpl-2.1.html

 Or:

    The Mozilla Public License Version 2.0. You may obtain a copy of
    the License at https://www.mozilla.org/MPL/

 The Initial Developer of the Original Code is Milan Crha
======================================================================*/

/*
 * Program for testing ICalAttach memory handling.
 */

#include <stdio.h>

#define LIBICAL_GLIB_UNSTABLE_API
#include "libical-glib/libical-glib.h"
#undef LIBICAL_GLIB_UNSTABLE_API

static GSList *get_attachments(ICalComponent *comp)
{
    ICalProperty *prop;
    GSList *attaches = NULL;

    for (prop = i_cal_component_get_first_property(comp, I_CAL_ATTACH_PROPERTY);
         prop;
         g_object_unref(prop),
         prop = i_cal_component_get_next_property(comp, I_CAL_ATTACH_PROPERTY)) {
        attaches = g_slist_prepend(attaches, i_cal_property_get_attach(prop));
    }

    return attaches;
}

static void remove_all_attachments(ICalComponent *comp)
{
    GSList *to_remove = NULL, *link;
    ICalProperty *prop;

    for (prop = i_cal_component_get_first_property(comp, I_CAL_ATTACH_PROPERTY);
         prop;
         g_object_unref(prop),
         prop = i_cal_component_get_next_property(comp, I_CAL_ATTACH_PROPERTY)) {
        to_remove = g_slist_prepend(to_remove, g_object_ref(prop));
    }

    for (link = to_remove; link; link = g_slist_next(link)) {
        prop = link->data;

        i_cal_component_remove_property(comp, prop);
    }

    g_slist_free_full(to_remove, g_object_unref);
}

static void set_attachments(ICalComponent *comp, GSList *attaches)
{
    GSList *link;

    remove_all_attachments (comp);

    for (link = attaches; link; link = g_slist_next (link)) {
        ICalAttach *attach = link->data;

        i_cal_component_take_property(comp, i_cal_property_new_attach (attach));
    }
}

int main (void)
{
    ICalComponent *comp;
    GSList *attaches;

    comp = i_cal_component_new_from_string(
        "BEGIN:VEVENT\r\n"
        "UID:123\r\n"
        "ATTACH:file:///tmp/f1.txt\r\n"
        "ATTACH:file:///tmp/f2.txt\r\n"
        "END:VEVENT\r\n"
    );

    attaches = get_attachments(comp);
    printf("%s: 1st: has %d attachments\n", __FUNCTION__, g_slist_length(attaches));
    set_attachments(comp, attaches);
    g_slist_free_full(attaches, g_object_unref);

    attaches = get_attachments(comp);
    printf("%s: 2nd: has %d attachments\n", __FUNCTION__, g_slist_length(attaches));
    g_slist_free_full(attaches, g_object_unref);

    g_object_unref(comp);

    return 0;
}
