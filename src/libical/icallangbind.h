/*======================================================================
 FILE: icallangbind.h
 CREATOR: eric 25 jan 2001

 SPDX-FileCopyrightText: 1999 Eric Busboom <eric@civicknowledge.com>

 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

======================================================================*/

#ifndef ICALLANGBIND_H
#define ICALLANGBIND_H

#include "libical_ical_export.h"
#include "icalcomponent.h"
#include "icalproperty.h"

LIBICAL_ICAL_EXPORT int *icallangbind_new_array(unsigned int size);

LIBICAL_ICAL_EXPORT void icallangbind_free_array(int *array);

LIBICAL_ICAL_EXPORT int icallangbind_access_array(int *array, int index);

LIBICAL_ICAL_EXPORT icalproperty *icallangbind_get_first_property(icalcomponent *c,
                                                                  const char *prop);

LIBICAL_ICAL_EXPORT icalproperty *icallangbind_get_next_property(icalcomponent *c,
                                                                 const char *prop);

LIBICAL_ICAL_EXPORT icalcomponent *icallangbind_get_first_component(icalcomponent *c,
                                                                    const char *comp);

LIBICAL_ICAL_EXPORT icalcomponent *icallangbind_get_next_component(icalcomponent *c,
                                                                   const char *comp);

LIBICAL_ICAL_EXPORT icalparameter *icallangbind_get_first_parameter(icalproperty *prop);

LIBICAL_ICAL_EXPORT icalparameter *icallangbind_get_next_parameter(icalproperty *prop);

LIBICAL_ICAL_EXPORT const char *icallangbind_property_eval_string(icalproperty *prop,
                                                                  const char *sep);

LIBICAL_ICAL_EXPORT char *icallangbind_property_eval_string_r(icalproperty *prop,
                                                              const char *sep);

LIBICAL_ICAL_EXPORT int icallangbind_string_to_open_flag(const char *str);

LIBICAL_ICAL_EXPORT const char *icallangbind_quote_as_ical(const char *str);

LIBICAL_ICAL_EXPORT char *icallangbind_quote_as_ical_r(const char *str);

#endif
