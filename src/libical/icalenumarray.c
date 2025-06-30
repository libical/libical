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

size_t icalenumarray_find(icalenumarray *array,
                          const icalenumarray_element *needle)
{
    size_t i;

    for (i = 0; array && i < array->num_elements; i++) {
        icalenumarray_element *e = icalarray_element_at(array, i);
        if (!!e->xvalue == !!needle->xvalue &&
            ((e->xvalue && !strcmp(e->xvalue, needle->xvalue)) ||
             (!e->xvalue && (e->val == needle->val)))) {
            return i;
        }
    }

    return icalenumarray_size(array);
}

void icalenumarray_append(icalenumarray *array, const icalenumarray_element *elem)
{
    icalenumarray_element copy = {
        elem->val, elem->xvalue ? icalmemory_strdup(elem->xvalue) : NULL};

    icalarray_append(array, &copy);
}

void icalenumarray_add(icalenumarray *array, const icalenumarray_element *add)
{
    if (icalenumarray_find(array, add) >= icalenumarray_size(array))
        icalenumarray_append(array, add);
}

void icalenumarray_remove_element_at(icalenumarray *array,
                                     size_t position)
{
    if (position >= icalenumarray_size(array)) return;

    icalenumarray_element *del = icalarray_element_at(array, position);

    if (del->xvalue)
        icalmemory_free_buffer((char *)del->xvalue);
    icalarray_remove_element_at(array, position);
}

void icalenumarray_remove(icalenumarray *array, const icalenumarray_element *del)
{
    size_t position = icalenumarray_find(array, del);

    if (position < icalenumarray_size(array))
        icalenumarray_remove_element_at(array, position);
}

void icalenumarray_free(icalenumarray *array)
{

    for (size_t i = 0; i < icalenumarray_size(array); i++) {
        icalenumarray_element *del = icalarray_element_at(array, i);
        if (del->xvalue)
            icalmemory_free_buffer((char *)del->xvalue);
    }

    icalarray_free(array);
}

static int enumcmp(const icalenumarray_element *a, const icalenumarray_element *b)
{
    /* Sort X- values alphabetically, but last */
    if (a->xvalue) {
        if (b->xvalue)
            return strcmp(a->xvalue, b->xvalue);
        else
            return 1;
    } else if (b->xvalue) {
        return -1;
    } else {
        return a->val - b->val;
    }
}

void icalenumarray_sort(icalenumarray *array)
{
    icalarray_sort(array, (int (*)(const void *, const void *))&enumcmp);
}

icalenumarray *icalenumarray_clone(icalenumarray *array)
{
    icalenumarray *clone = icalenumarray_new(array->increment_size);
    size_t i;

    for (i = 0; i < array->num_elements; i++) {
        icalenumarray_append(clone, icalenumarray_element_at(array, i));
    }

    return clone;
}
