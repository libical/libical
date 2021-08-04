/*
 * Copyright (C) 2015 William Yu <williamyu@gnome.org>
 *
 * This library is free software: you can redistribute it and/or modify it
 * under the terms of version 2.1. of the GNU Lesser General Public License
 * as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library. If not, see <https://www.gnu.org/licenses/>.
 */

#if !defined (__LIBICAL_GLIB_H_INSIDE__) && !defined (LIBICAL_GLIB_COMPILATION)
#error "Only <libical-glib/libical-glib.h> can be included directly."
#endif

#ifndef I_CAL_OBJECT_H
#define I_CAL_OBJECT_H

#include <stdio.h>
#include <glib.h>
#include <glib-object.h>
#include <libical/ical.h>
#include <libical/libical_ical_export.h>

#define I_CAL_TYPE_OBJECT \
    (i_cal_object_get_type ())
LIBICAL_ICAL_EXPORT
G_DECLARE_DERIVABLE_TYPE(ICalObject, i_cal_object, I_CAL, OBJECT, GObject)

G_BEGIN_DECLS
/**
 * ICalObject:
 *
 * This is an ICalObject instance struct.
 */

/**
 * ICalObjectClass:
 *
 * This is an ICalObject class struct.
 */

struct _ICalObjectClass
{
    /*< private > */
    GObjectClass parent_class;
};

LIBICAL_ICAL_EXPORT gpointer i_cal_object_construct(GType object_type,
                                                    gpointer native,
                                                    GDestroyNotify native_destroy_func,
                                                    gboolean is_global_memory, GObject *owner);

LIBICAL_ICAL_EXPORT gpointer i_cal_object_get_native(ICalObject *iobject);

LIBICAL_ICAL_EXPORT gpointer i_cal_object_steal_native(ICalObject *iobject);

LIBICAL_ICAL_EXPORT gboolean i_cal_object_get_is_global_memory(ICalObject *iobject);

LIBICAL_ICAL_EXPORT GDestroyNotify i_cal_object_get_native_destroy_func(ICalObject *iobject);

LIBICAL_ICAL_EXPORT void i_cal_object_set_native_destroy_func(ICalObject *iobject,
                                                              GDestroyNotify native_destroy_func);

LIBICAL_ICAL_EXPORT void i_cal_object_set_owner(ICalObject *iobject, GObject *owner);

LIBICAL_ICAL_EXPORT GObject *i_cal_object_ref_owner(ICalObject *iobject);

LIBICAL_ICAL_EXPORT void i_cal_object_remove_owner(ICalObject *iobject);

LIBICAL_ICAL_EXPORT void i_cal_object_add_depender(ICalObject *iobject, GObject *depender);

LIBICAL_ICAL_EXPORT void i_cal_object_remove_depender(ICalObject *iobject, GObject *depender);

LIBICAL_ICAL_EXPORT void i_cal_object_free_global_objects(void);

G_END_DECLS
#endif /* I_CAL_OBJECT_H */
