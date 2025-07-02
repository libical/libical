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
#include <stdint.h>

size_t icalstrarray_size(icalstrarray *array)
{
    if (!array)
        return 0;
    return array->num_elements;
}

const char *icalstrarray_element_at(icalstrarray *array, size_t position)
{
    if (position >= icalstrarray_size(array))
        return NULL;
    return *((const char **)icalarray_element_at(array, position));
}

size_t icalstrarray_find(icalstrarray *array,
                         const char *needle)
{
    if (!array || !needle)
        return icalstrarray_size(array);

    size_t i;

    for (i = 0; i < array->num_elements; i++) {
        const char *s = icalstrarray_element_at(array, i);
        if (s && !strcmp(needle, s)) {
            return i;
        }
    }

    return array->num_elements;
}

void icalstrarray_append(icalstrarray *array, const char *elem)
{
    if (!array || !elem)
        return;

    char *copy = icalmemory_strdup(elem);

    icalarray_append(array, &copy);
}

void icalstrarray_add(icalstrarray *array, const char *add)
{
    if (!array || !add)
        return;

    if (icalstrarray_find(array, add) >= icalstrarray_size(array))
        icalstrarray_append(array, add);
}

void icalstrarray_remove_element_at(icalstrarray *array, size_t position)
{
    if (position >= icalstrarray_size(array))
        return;

    char **del = icalarray_element_at(array, position);

    if (del && *del)
        icalmemory_free_buffer(*del);
    icalarray_remove_element_at(array, position);
}

void icalstrarray_remove(icalstrarray *array, const char *del)
{
    if (!array || !del)
        return;

    size_t j = 0;

    for (size_t i = 0; i < array->num_elements; i++) {
        char **elem = icalarray_element_at(array, i);
        if (strcmp(*elem, del)) {
            icalarray_set_element_at(array, elem, j++);
        } else {
            icalmemory_free_buffer(*elem);
        }
    }

    array->num_elements = j;
}

void icalstrarray_free(icalstrarray *array)
{
    if (!array)
        return;

    for (size_t i = 0; i < array->num_elements; i++) {
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

void icalstrarray_sort(icalstrarray *array)
{
    if (!array)
        return;
    icalarray_sort(array, (int (*)(const void *, const void *))&strpcmp);
}

icalstrarray *icalstrarray_clone(icalstrarray *array)
{
    if (!array)
        return NULL;

    icalstrarray *clone = icalstrarray_new(array->increment_size);
    size_t i;

    for (i = 0; i < array->num_elements; i++) {
        icalstrarray_append(clone, icalstrarray_element_at(array, i));
    }

    return clone;
}
