/*======================================================================
 FILE: icalstrarray.h

 CREATOR: Ken Murchison 24 Aug 2022

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)

 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 ======================================================================*/

#ifndef ICALSTRARRAY_H
#define ICALSTRARRAY_H

#include "libical_ical_export.h"
#include "icalarray.h"

#include <stdlib.h>

typedef icalarray icalstrarray;

#define icalstrarray_new(increment_size) \
    icalarray_new(sizeof(char *), increment_size)

#define icalstrarray_element_at(array, position) \
    *((const char **)icalarray_element_at(array, position))

#define icalstrarray_size(array) ((array)->num_elements)

LIBICAL_ICAL_EXPORT size_t icalstrarray_find(icalstrarray *array,
                                             const char *needle);

LIBICAL_ICAL_EXPORT void icalstrarray_append(icalstrarray *array,
                                             const char *elem);

LIBICAL_ICAL_EXPORT void icalstrarray_add(icalstrarray *array,
                                          const char *add);

LIBICAL_ICAL_EXPORT void icalstrarray_remove_element_at(icalstrarray *array,
                                                        size_t position);

LIBICAL_ICAL_EXPORT void icalstrarray_remove(icalstrarray *array,
                                             const char *del);

LIBICAL_ICAL_EXPORT void icalstrarray_free(icalstrarray *array);

LIBICAL_ICAL_EXPORT void icalstrarray_sort(icalstrarray *array);

LIBICAL_ICAL_EXPORT char *icalstrarray_as_ical_string_r(const icalstrarray *array,
                                                        const char sep);

LIBICAL_ICAL_EXPORT icalstrarray *icalstrarray_clone(icalstrarray *array);

#endif /* ICALSTRARRAY_H */
