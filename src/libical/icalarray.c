/*======================================================================
 FILE: icalarray.c
 CREATOR: Damon Chaplin 07 March 2001

 SPDX-FileCopyrightText: 2001, Ximian, Inc.
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "icalarray.h"
#include "icalerror.h"
#include "icalmemory.h"
#include "qsort_gen.h"

#include <stdlib.h>
#include <string.h>

#ifndef ICALARRAY_DEFAULT_INCREMENT_SIZE
#define ICALARRAY_DEFAULT_INCREMENT_SIZE 4
#endif

static void icalarray_expand(icalarray *array, size_t space_needed);

icalarray *icalarray_new(size_t element_size, size_t increment_size)
{
    icalarray *array;

    array = (icalarray *)icalmemory_new_buffer(sizeof(icalarray));
    if (!array) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return NULL;
    }

    if (!increment_size) {
        increment_size = ICALARRAY_DEFAULT_INCREMENT_SIZE;
    }

    array->element_size = element_size;
    array->increment_size = increment_size;
    array->num_elements = 0;
    array->space_allocated = 0;
    array->chunks = NULL;

    return array;
}

static void *icalarray_alloc_chunk(const icalarray *array)
{
    void *chunk = icalmemory_new_buffer(array->element_size * array->increment_size);

    if (!chunk) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
    }
    return chunk;
}

icalarray *icalarray_copy(const icalarray *originalarray)
{
    icalarray *array = icalarray_new(originalarray->element_size, originalarray->increment_size);
    size_t chunks = originalarray->space_allocated / originalarray->increment_size;
    size_t chunk;

    if (!array) {
        return NULL;
    }

    array->chunks = icalmemory_new_buffer(chunks * sizeof(void *));
    if (array->chunks) {
        for (chunk = 0; chunk < chunks; chunk++) {
            array->chunks[chunk] = icalarray_alloc_chunk(array);
            if (array->chunks[chunk]) {
                memcpy(array->chunks[chunk], originalarray->chunks[chunk],
                       array->increment_size * array->element_size);

                array->space_allocated += array->increment_size;
            } else {
                icalerror_set_errno(ICAL_ALLOCATION_ERROR);
                icalarray_free(array);
                return NULL;
            }
        }

    } else {
        icalerror_set_errno(ICAL_ALLOCATION_ERROR);
        icalarray_free(array);
        return NULL;
    }

    array->num_elements = originalarray->num_elements;

    return array;
}

void icalarray_free(icalarray *array)
{
    if (array && array->chunks) {
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

void icalarray_set_element_at(icalarray *array, const void *element, size_t position)
{
    memcpy(icalarray_element_at(array, position), element, array->element_size);
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

struct _icalarray_sort_context {
    icalarray *array;
    int (*compare)(const void *, const void *);
};

static int icalarray_fcompare(const void *context, size_t i, size_t j)
{
    struct _icalarray_sort_context *sort_context = (struct _icalarray_sort_context *)context;
    void *pI = icalarray_element_at(sort_context->array, i);
    void *pJ = icalarray_element_at(sort_context->array, j);

    return sort_context->compare(pI, pJ);
}

static void icalarray_fswap(void *context, size_t i, size_t j)
{
    struct _icalarray_sort_context *sort_context = (struct _icalarray_sort_context *)context;
    void *pI = icalarray_element_at(sort_context->array, i);
    void *pJ = icalarray_element_at(sort_context->array, j);

    qsort_gen_memswap(pI, pJ, sort_context->array->element_size);
}

void icalarray_sort(icalarray *array, int (*compare)(const void *, const void *))
{
    struct _icalarray_sort_context sort_context;
    sort_context.array = array;
    sort_context.compare = compare;

    if (array->num_elements <= 1) {
        return;
    }

    qsort_gen(&sort_context, array->num_elements, icalarray_fcompare, icalarray_fswap);
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
