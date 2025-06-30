/*======================================================================
 FILE: vcardstrarray.c

 CREATOR: Ken Murchison 24 Aug 2022

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)

 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 ======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "vcardstrarray.h"
#include "icalmemory.h"

#include <string.h>

size_t vcardstrarray_find(vcardstrarray *array,
                          const char *needle)
{
    size_t i;

    for (i = 0; i < array->num_elements; i++) {
        if (!strcmp(needle, vcardstrarray_element_at(array, i))) {
            return i;
        }
    }

    return array->num_elements;
}

void vcardstrarray_append(vcardstrarray *array, const char *elem)
{
    char *copy = icalmemory_strdup(elem);

    icalarray_append(array, &copy);
}

void vcardstrarray_add(vcardstrarray *array, const char *add)
{
    if (vcardstrarray_find(array, add) >= vcardstrarray_size(array))
        vcardstrarray_append(array, add);
}

void vcardstrarray_remove_element_at(vcardstrarray *array, size_t position)
{
    if (position >= vcardstrarray_size(array))
        return;

    char **del = icalarray_element_at(array, position);

    if (del && *del)
        icalmemory_free_buffer(*del);
    icalarray_remove_element_at(array, position);
}

void vcardstrarray_remove(vcardstrarray *array, const char *del)
{
    size_t position = vcardstrarray_find(array, del);

    if (position < vcardstrarray_size(array))
        vcardstrarray_remove_element_at(array, position);
}

void vcardstrarray_free(vcardstrarray *array)
{
    for (size_t i = 0; i < vcardstrarray_size(array); i++) {
        char **del = icalarray_element_at(array, i);
        if (del && *del)
            icalmemory_free_buffer(*del);
    }

    icalarray_free(array);
}

static int strpcmp(const char **a, const char **b)
{
    return strcmp(*a, *b);
}

void vcardstrarray_sort(vcardstrarray *array)
{
    icalarray_sort(array, (int (*)(const void *, const void *))&strpcmp);
}

vcardstrarray *vcardstrarray_clone(vcardstrarray *array)
{
    vcardstrarray *clone = vcardstrarray_new(array->increment_size);
    size_t i;

    for (i = 0; i < array->num_elements; i++) {
        vcardstrarray_append(clone, vcardstrarray_element_at(array, i));
    }

    return clone;
}
