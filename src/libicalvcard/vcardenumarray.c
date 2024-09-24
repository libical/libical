/*======================================================================
 FILE: vcardenumarray.c

 CREATOR: Ken Murchison 24 Aug 2022 <murch@fastmailteam.com>

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)

 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 ======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "vcardenumarray.h"
#include "icalmemory.h"

#include <string.h>

ssize_t vcardenumarray_find(vcardenumarray *array,
                            vcardenumarray_element *needle)
{
    size_t i;

    for (i = 0; array && i < array->num_elements; i++) {
        vcardenumarray_element *e = icalarray_element_at(array, i);
        if (!!e->xvalue == !!needle->xvalue &&
            ((e->xvalue && !strcmp(e->xvalue, needle->xvalue)) ||
             (!e->xvalue && (e->val == needle->val)))) {
            return (ssize_t)i;
        }
    }

    return -1;
}

void vcardenumarray_append(vcardenumarray *array, vcardenumarray_element *elem)
{
    vcardenumarray_element copy = {
        elem->val, elem->xvalue ? icalmemory_strdup(elem->xvalue) : NULL};

    icalarray_append(array, &copy);
}

void vcardenumarray_add(vcardenumarray *array, vcardenumarray_element *add)
{
    if (vcardenumarray_find(array, add) < 0)
        vcardenumarray_append(array, add);
}

void vcardenumarray_remove_element_at(vcardenumarray *array,
                                      ssize_t position)
{
    vcardenumarray_element *del = icalarray_element_at(array, (size_t)position);

    if (del->xvalue)
        icalmemory_free_buffer((char *)del->xvalue);
    icalarray_remove_element_at(array, (size_t)position);
}

void vcardenumarray_remove(vcardenumarray *array, vcardenumarray_element *del)
{
    ssize_t position = vcardenumarray_find(array, del);

    if (position >= 0)
        vcardenumarray_remove_element_at(array, position);
}

void vcardenumarray_free(vcardenumarray *array)
{
    while (array->num_elements)
        vcardenumarray_remove_element_at(array, (ssize_t)(array->num_elements - 1));
    icalarray_free(array);
}

static int enumcmp(const vcardenumarray_element *a, const vcardenumarray_element *b)
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

void vcardenumarray_sort(vcardenumarray *array)
{
    icalarray_sort(array, (int (*)(const void *, const void *))&enumcmp);
}
