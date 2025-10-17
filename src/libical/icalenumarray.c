/*======================================================================
 FILE: icalenumarray.c
 CREATOR: Ken Murchison 24 Aug 2022 <murch@fastmailteam.com>

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 ======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "icalenumarray.h"
#include "icalmemory.h"

#include <string.h>

size_t icalenumarray_size(const icalenumarray *array)
{
    if (!array) {
        return 0;
    }
    return array->num_elements;
}

const icalenumarray_element *icalenumarray_element_at(icalenumarray *array, size_t position)
{
    if (position >= icalenumarray_size(array)) {
        return NULL;
    }
    return icalarray_element_at(array, position);
}

static int enumcmp(const icalenumarray_element *a, const icalenumarray_element *b)
{
    /* Sort X-values alphabetically, but last */
    if (!a->xvalue && b->xvalue) {
        return -1;
    } else if (a->xvalue && !b->xvalue) {
        return 1;
    }

    if (a->val < b->val) {
        return -1;
    } else if (a->val > b->val) {
        return 1;
    }

    if (a->xvalue) {
        return strcmp(a->xvalue, b->xvalue);
    } else {
        return 0;
    }
}

size_t icalenumarray_find(icalenumarray *array,
                          const icalenumarray_element *needle)
{
    if (!array || !needle) {
        return icalenumarray_size(array);
    }

    size_t i;

    for (i = 0; i < array->num_elements; i++) {
        const icalenumarray_element *e = icalarray_element_at(array, i);
        if (!enumcmp(e, needle)) {
            return i;
        }
    }

    return icalenumarray_size(array);
}

void icalenumarray_append(icalenumarray *array, const icalenumarray_element *elem)
{
    if (!array || !elem) {
        return;
    }

    icalenumarray_element copy = {
        elem->val, elem->xvalue ? icalmemory_strdup(elem->xvalue) : NULL};

    icalarray_append(array, &copy);
}

void icalenumarray_add(icalenumarray *array, const icalenumarray_element *elem)
{
    if (!array || !elem) {
        return;
    }

    if (icalenumarray_find(array, elem) >= icalenumarray_size(array)) {
        icalenumarray_append(array, elem);
    }
}

void icalenumarray_remove_element_at(icalenumarray *array,
                                     size_t position)
{
    if (position >= icalenumarray_size(array)) {
        return;
    }

    icalenumarray_element *del = icalarray_element_at(array, position);

    icalmemory_free_buffer((char *)del->xvalue);
    icalarray_remove_element_at(array, position);
}

void icalenumarray_remove(icalenumarray *array, const icalenumarray_element *del)
{
    if (!array || !del) {
        return;
    }

    size_t j = 0;

    for (size_t i = 0; i < array->num_elements; i++) {
        const icalenumarray_element *elem = icalarray_element_at(array, i);
        if (enumcmp(elem, del)) {
            icalarray_set_element_at(array, elem, j++);
        } else {
            icalmemory_free_buffer((char *)elem->xvalue);
        }
    }

    array->num_elements = j;
}

void icalenumarray_free(icalenumarray *array)
{
    if (!array) {
        return;
    }

    for (size_t i = 0; i < icalenumarray_size(array); i++) {
        icalenumarray_element *del = icalarray_element_at(array, i);
        icalmemory_free_buffer((char *)del->xvalue);
    }

    icalarray_free(array);
}

void icalenumarray_sort(icalenumarray *array)
{
    if (!array) {
        return;
    }

    icalarray_sort(array, (int (*)(const void *, const void *))&enumcmp);
}

icalenumarray *icalenumarray_clone(icalenumarray *array)
{
    if (!array) {
        return NULL;
    }

    icalenumarray *clone = icalenumarray_new(array->increment_size);
    size_t i;

    for (i = 0; i < array->num_elements; i++) {
        icalenumarray_append(clone, icalenumarray_element_at(array, i));
    }

    return clone;
}
