/*======================================================================
 FILE: vcardenumarray.h

 CREATOR: Ken Murchison 24 Aug 2022 <murch@fastmailteam.com>

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)

 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 ======================================================================*/

#ifndef VCARDENUMARRAY_H
#define VCARDENUMARRAY_H

#include "libical_vcard_export.h"
#include "icalarray.h"

#include <stdlib.h>

typedef icalarray vcardenumarray;

typedef struct {
    int val;
    const char *xvalue;
} vcardenumarray_element;

#define vcardenumarray_new(increment_size) \
    icalarray_new(sizeof(vcardenumarray_element), increment_size)

#define vcardenumarray_element_at(array, position) \
    ((const vcardenumarray_element *)icalarray_element_at(array, position))

#define vcardenumarray_size(array) ((array)->num_elements)

LIBICAL_VCARD_EXPORT ssize_t vcardenumarray_find(vcardenumarray *array,
                                                 const vcardenumarray_element *needle);

LIBICAL_VCARD_EXPORT void vcardenumarray_append(vcardenumarray *array,
                                                const vcardenumarray_element *elem);

LIBICAL_VCARD_EXPORT void vcardenumarray_add(vcardenumarray *array,
                                             const vcardenumarray_element *add);

LIBICAL_VCARD_EXPORT void vcardenumarray_remove_element_at(vcardenumarray *array,
                                                           ssize_t position);
LIBICAL_VCARD_EXPORT void vcardenumarray_remove(vcardenumarray *array,
                                                const vcardenumarray_element *del);

LIBICAL_VCARD_EXPORT void vcardenumarray_free(vcardenumarray *array);

LIBICAL_VCARD_EXPORT void vcardenumarray_sort(vcardenumarray *array);

#endif /* VCARDENUMARRAY_H */
