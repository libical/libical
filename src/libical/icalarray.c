/*======================================================================
 FILE: icalarray.c
 CREATOR: Damon Chaplin 07 March 2001

 (C) COPYRIGHT 2001, Ximian, Inc.

 This library is free software; you can redistribute it and/or modify
 it under the terms of either:

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.gnu.org/licenses/lgpl-2.1.html

 Or:

    The Mozilla Public License Version 2.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/
======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "icalarray.h"
#include "icalerror.h"
#include "icalmemory.h"

#include <stdlib.h>
#include <string.h>

static void icalarray_expand(icalarray *array, size_t space_needed);

icalarray *icalarray_new(size_t element_size, size_t increment_size)
{
    icalarray *array;

    array = (icalarray *) icalmemory_new_buffer(sizeof(icalarray));
    if (!array) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return NULL;
    }

    array->element_size = element_size;
    array->increment_size = increment_size;
    array->num_elements = 0;
    array->space_allocated = 0;
    array->chunks = NULL;

    return array;
}

static void *icalarray_alloc_chunk(icalarray *array)
{
    void *chunk = icalmemory_new_buffer(array->element_size * array->increment_size);

    if (!chunk) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
    }
    return chunk;
}

icalarray *icalarray_copy(icalarray *originalarray)
{
    icalarray *array = icalarray_new(originalarray->element_size, originalarray->increment_size);
    size_t chunks = originalarray->space_allocated / originalarray->increment_size;
    size_t chunk;

    if (!array) {
        return NULL;
    }

    array->num_elements = originalarray->num_elements;
    array->space_allocated = originalarray->space_allocated;

    array->chunks = icalmemory_new_buffer(chunks * sizeof(void *));
    if (array->chunks) {
        for (chunk = 0; chunk < chunks; chunk++) {
            array->chunks[chunk] = icalarray_alloc_chunk(array);
            if (array->chunks[chunk]) {
                memcpy(array->chunks[chunk], originalarray->chunks[chunk],
                       array->increment_size * array->element_size);
            }
            else {
                // a error has already been set. The best thing we can do to
                // make icalarray robust is to adjust the array size to what
                // we could allocate.
                if (array->num_elements > chunk)
                    array->num_elements = chunk;
                if (array->space_allocated > chunk * originalarray->increment_size)
                    array->space_allocated = chunk * originalarray->increment_size;
                break;
            }
        }

    } else {
        icalerror_set_errno(ICAL_ALLOCATION_ERROR);
    }

    return array;
}

void icalarray_free(icalarray *array)
{
    if (array->chunks) {
        size_t chunks = array->space_allocated / array->increment_size;
        size_t chunk;

        for (chunk = 0; chunk < chunks; chunk++) {
            icalmemory_free_buffer(array->chunks[chunk]);
        }
        icalmemory_free_buffer(array->chunks);
        array->chunks = 0;
    }
    icalmemory_free_buffer(array);
}

void icalarray_append(icalarray *array, const void *element)
{
    size_t pos;

    if (array->num_elements >= array->space_allocated) {
        icalarray_expand(array, 1);
        if (array->num_elements >= array->space_allocated) {
            /* expansion failed. Error has already been set. */
            return;
        }
    }

    pos = array->num_elements++;
    memcpy(icalarray_element_at(array, pos), element, array->element_size);
}

void *icalarray_element_at(icalarray *array, size_t position)
{
    size_t chunk = position / array->increment_size;
    size_t offset = position % array->increment_size;

    return (char *)(array->chunks[chunk]) + (offset * array->element_size);
}

void icalarray_remove_element_at(icalarray *array, size_t position)
{
    while (position < array->num_elements - 1) {
        memmove(icalarray_element_at(array, position),
                icalarray_element_at(array, position + 1), array->element_size);
        position++;
    }

    array->num_elements--;
}

void icalarray_sort(icalarray *array, int (*compare) (const void *, const void *))
{
    if (array->num_elements == 0) {
        return;
    }

    if (array->num_elements <= array->increment_size) {
        qsort(array->chunks[0], array->num_elements, array->element_size, compare);
    } else {
        size_t pos;
        void *tmp = icalmemory_new_buffer(array->num_elements * array->element_size);

        if (!tmp) {
            return;
        }
        for (pos = 0; pos < array->num_elements; pos++) {
            memcpy((char *)tmp + array->element_size * pos,
                   icalarray_element_at(array, pos), array->element_size);
        }

        qsort(tmp, array->num_elements, array->element_size, compare);

        for (pos = 0; pos < array->num_elements; pos++) {
            memcpy(icalarray_element_at(array, pos),
                   (char *)tmp + array->element_size * pos, array->element_size);
        }
        icalmemory_free_buffer(tmp);
    }
}

static void icalarray_expand(icalarray *array, size_t space_needed)
{
    size_t num_chunks = array->space_allocated / array->increment_size;
    size_t num_new_chunks;
    size_t c;
    void **new_chunks;

    num_new_chunks = (space_needed + array->increment_size - 1) / array->increment_size;
    if (!num_new_chunks) {
        num_new_chunks = 1;
    }

    new_chunks = icalmemory_new_buffer((num_chunks + num_new_chunks) * sizeof(void *));

    if (new_chunks) {
        if (array->chunks && num_chunks) {
            memcpy(new_chunks, array->chunks, num_chunks * sizeof(void *));
        }
        for (c = 0; c < num_new_chunks; c++) {
            new_chunks[c + num_chunks] = icalarray_alloc_chunk(array);
            if (!new_chunks[c + num_chunks]) {
                num_new_chunks = c;
                break;
            }
        }
        if (array->chunks) {
            icalmemory_free_buffer(array->chunks);
        }
        array->chunks = new_chunks;
        array->space_allocated = array->space_allocated + num_new_chunks * array->increment_size;
    } else {
        icalerror_set_errno(ICAL_ALLOCATION_ERROR);
    }
}
