#ifndef VCARDENUMARRAY_H
#define VCARDENUMARRAY_H

#include <stdlib.h>

#include "libical_vcard_export.h"

#include "icalarray.h"

typedef icalarray vcardenumarray;

typedef struct {
    int val;
    const char *xvalue;
} vcardenumarray_element;

#define vcardenumarray_new(increment_size)                          \
    icalarray_new(sizeof(vcardenumarray_element), increment_size)

#define vcardenumarray_element_at(array, position)                      \
    ((const vcardenumarray_element *) icalarray_element_at(array, position))

#define vcardenumarray_size(array) ((array)->num_elements)

LIBICAL_VCARD_EXPORT ssize_t vcardenumarray_find(vcardenumarray *array,
                                                 vcardenumarray_element *needle);

LIBICAL_VCARD_EXPORT void vcardenumarray_append(vcardenumarray *array,
                                                vcardenumarray_element *new);

LIBICAL_VCARD_EXPORT void vcardenumarray_add(vcardenumarray *array,
                                             vcardenumarray_element *add);

LIBICAL_VCARD_EXPORT void vcardenumarray_remove_element_at(vcardenumarray *array,
                                                           size_t position);
LIBICAL_VCARD_EXPORT void vcardenumarray_remove(vcardenumarray *array,
                                                vcardenumarray_element *del);

LIBICAL_VCARD_EXPORT void vcardenumarray_free(vcardenumarray *array);

LIBICAL_VCARD_EXPORT void vcardenumarray_sort(vcardenumarray *array);

#endif /* VCARDENUMARRAY_H */
