/*======================================================================
 FILE: vcardstrarray.h

 CREATOR: Ken Murchison 24 Aug 2022

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)

 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 ======================================================================*/

#ifndef VCARDSTRARRAY_H
#define VCARDSTRARRAY_H

#include "libical_vcard_export.h"
#include "icalarray.h"

#include <stdlib.h>

typedef icalarray vcardstrarray;

#define vcardstrarray_new(increment_size) \
    icalarray_new(sizeof(char *), increment_size)

#define vcardstrarray_element_at(array, position) \
    *((const char **)icalarray_element_at(array, position))

#define vcardstrarray_size(array) ((array)->num_elements)

LIBICAL_VCARD_EXPORT size_t vcardstrarray_find(vcardstrarray *array,
                                               const char *needle);

LIBICAL_VCARD_EXPORT void vcardstrarray_append(vcardstrarray *array,
                                               const char *elem);

LIBICAL_VCARD_EXPORT void vcardstrarray_add(vcardstrarray *array,
                                            const char *add);

LIBICAL_VCARD_EXPORT void vcardstrarray_remove_element_at(vcardstrarray *array,
                                                          size_t position);

LIBICAL_VCARD_EXPORT void vcardstrarray_remove(vcardstrarray *array,
                                               const char *del);

LIBICAL_VCARD_EXPORT void vcardstrarray_free(vcardstrarray *array);

LIBICAL_VCARD_EXPORT void vcardstrarray_sort(vcardstrarray *array);

LIBICAL_VCARD_EXPORT char *vcardstrarray_as_vcard_string_r(const vcardstrarray *array,
                                                          const char sep);

LIBICAL_VCARD_EXPORT vcardstrarray *vcardstrarray_clone(vcardstrarray *array);

#endif /* VCARDSTRARRAY_H */
