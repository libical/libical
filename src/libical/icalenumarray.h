/*======================================================================
 FILE: icalenumarray.h

 CREATOR: Ken Murchison 24 Aug 2022 <murch@fastmailteam.com>

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)

 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 ======================================================================*/

#ifndef ICALENUMARRAY_H
#define ICALENUMARRAY_H

#include "libical_ical_export.h"
#include "icalarray.h"

#include <stdlib.h>

typedef icalarray icalenumarray;

typedef struct {
    int val;
    const char *xvalue;
} icalenumarray_element;

#define icalenumarray_new(increment_size) \
    icalarray_new(sizeof(icalenumarray_element), increment_size)

#define icalenumarray_element_at(array, position) \
    ((const icalenumarray_element *)icalarray_element_at(array, position))

#define icalenumarray_size(array) ((array)->num_elements)

LIBICAL_ICAL_EXPORT size_t icalenumarray_find(icalenumarray *array,
                                              const icalenumarray_element *needle);

LIBICAL_ICAL_EXPORT void icalenumarray_append(icalenumarray *array,
                                              const icalenumarray_element *elem);

LIBICAL_ICAL_EXPORT void icalenumarray_add(icalenumarray *array,
                                           const icalenumarray_element *add);

LIBICAL_ICAL_EXPORT void icalenumarray_remove_element_at(icalenumarray *array,
                                                         size_t position);
LIBICAL_ICAL_EXPORT void icalenumarray_remove(icalenumarray *array,
                                              const icalenumarray_element *del);

LIBICAL_ICAL_EXPORT void icalenumarray_free(icalenumarray *array);

LIBICAL_ICAL_EXPORT void icalenumarray_sort(icalenumarray *array);

LIBICAL_ICAL_EXPORT icalenumarray *icalenumarray_clone(icalenumarray *array);

#endif /* ICALENUMARRAY_H */
