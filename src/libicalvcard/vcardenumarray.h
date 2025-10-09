/*======================================================================
 FILE: vcardenumarray.h
 CREATOR: Ken Murchison 24 Aug 2022 <murch@fastmailteam.com>

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 ======================================================================*/

#ifndef VCARDENUMARRAY_H
#define VCARDENUMARRAY_H

#include "libical_vcard_export.h"
#include "icalenumarray.h"

typedef icalenumarray vcardenumarray;
typedef icalenumarray_element vcardenumarray_element;

#define vcardenumarray_new(increment_size) \
    icalenumarray_new(increment_size)

#define vcardenumarray_element_at(array, position) \
    icalenumarray_element_at(array, position)

#define vcardenumarray_size(array) \
    icalenumarray_size(array)

#define vcardenumarray_find(array, needle) \
    icalenumarray_find(array, needle)

#define vcardenumarray_append(array, elem) \
    icalenumarray_append(array, elem)

#define vcardenumarray_add(array, add) \
    icalenumarray_add(array, add)

#define vcardenumarray_remove_element_at(array, position) \
    icalenumarray_remove_element_at(array, position)

#define vcardenumarray_remove(array, del) \
    icalenumarray_remove(array, del)

#define vcardenumarray_free(array) \
    icalenumarray_free(array)

#define vcardenumarray_sort(array) \
    icalenumarray_sort(array)

#define vcardenumarray_clone(array) \
    icalenumarray_clone(array)

#endif /* VCARDENUMARRAY_H */
