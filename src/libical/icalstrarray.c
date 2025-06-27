/*======================================================================
 FILE: icalstrarray.c

 CREATOR: Ken Murchison 24 Aug 2022

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)

 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 ======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "icalstrarray.h"
#include "icalmemory.h"

#include <string.h>

ssize_t icalstrarray_find(icalstrarray *array,
                           const char *needle)
{
    size_t i;

    for (i = 0; i < array->num_elements; i++) {
        if (!strcmp(needle, icalstrarray_element_at(array, i))) {
            return (ssize_t)i;
        }
    }

    return -1;
}

void icalstrarray_append(icalstrarray *array, const char *elem)
{
    char *copy = icalmemory_strdup(elem);

    icalarray_append(array, &copy);
}

void icalstrarray_add(icalstrarray *array, const char *add)
{
    if (icalstrarray_find(array, add) < 0)
        icalstrarray_append(array, add);
}

void icalstrarray_remove_element_at(icalstrarray *array, ssize_t position)
{
    char **del = icalarray_element_at(array, (size_t)position);

    if (del && *del)
        icalmemory_free_buffer(*del);
    icalarray_remove_element_at(array, (size_t)position);
}

void icalstrarray_remove(icalstrarray *array, const char *del)
{
    ssize_t position = icalstrarray_find(array, del);

    if (position >= 0)
        icalstrarray_remove_element_at(array, position);
}

void icalstrarray_free(icalstrarray *array)
{
    ssize_t i = (ssize_t)(array->num_elements - 1);
    while (i >= 0)
        icalstrarray_remove_element_at(array, i--);
    icalarray_free(array);
}

static int strpcmp(const char **a, const char **b)
{
    return strcmp(*a, *b);
}

void icalstrarray_sort(icalstrarray *array)
{
    icalarray_sort(array, (int (*)(const void *, const void *))&strpcmp);
}

icalstrarray *icalstrarray_clone(icalstrarray *array)
{
    icalstrarray *clone = icalstrarray_new(array->increment_size);
    size_t i;

    for (i = 0; i < array->num_elements; i++) {
        icalstrarray_append(clone, icalstrarray_element_at(array, i));
    }

    return clone;
}
