/*======================================================================
 FILE: vcardstrarray.h
 CREATOR: Ken Murchison 24 Aug 2022

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 ======================================================================*/

#ifndef VCARDSTRARRAY_H
#define VCARDSTRARRAY_H

#include "libical_vcard_export.h"
#include "icalstrarray.h"

typedef icalstrarray vcardstrarray;

#define vcardstrarray_new(increment_size) \
    icalstrarray_new(increment_size)

#define vcardstrarray_element_at(array, position) \
    icalstrarray_element_at(array, position)

#define vcardstrarray_size(array) \
    icalstrarray_size(array)

#define vcardstrarray_find(array, needle) \
    icalstrarray_find(array, needle)

#define vcardstrarray_append(array, elem) \
    icalstrarray_append(array, elem)

#define vcardstrarray_add(array, add) \
    icalstrarray_add(array, add)

#define vcardstrarray_remove_element_at(array, position) \
    icalstrarray_remove_element_at(array, position)

#define vcardstrarray_remove(array, del) \
    icalstrarray_remove(array, del)

#define vcardstrarray_free(array) \
    icalstrarray_free(array)

#define vcardstrarray_sort(array) \
    icalstrarray_sort(array)

#define vcardstrarray_clone(array) \
    icalstrarray_clone(array)

#endif /* VCARDSTRARRAY_H */
