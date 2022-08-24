#ifndef VCARDSTRARRAY_H
#define VCARDSTRARRAY_H

#include <stdlib.h>

#include "libical_vcard_export.h"

#include "icalarray.h"

typedef icalarray vcardstrarray;

#define vcardstrarray_new(increment_size)           \
    icalarray_new(sizeof(char *), increment_size)

#define vcardstrarray_element_at(array, position)               \
    *((const char **) icalarray_element_at(array, position))

LIBICAL_VCARD_EXPORT ssize_t vcardstrarray_find(vcardstrarray *array,
                                                const char *needle);

LIBICAL_VCARD_EXPORT void vcardstrarray_append(vcardstrarray *array,
                                               const char *new);

LIBICAL_VCARD_EXPORT void vcardstrarray_add(vcardstrarray *array,
                                            const char *add);

LIBICAL_VCARD_EXPORT void vcardstrarray_remove_element_at(vcardstrarray *array,
                                                          size_t position);

LIBICAL_VCARD_EXPORT void vcardstrarray_remove(vcardstrarray *array,
                                               const char *del);

LIBICAL_VCARD_EXPORT void vcardstrarray_free(vcardstrarray *array);

#endif /* VCARDSTRARRAY_H */