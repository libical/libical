#include <string.h>

#include "icalmemory.h"
#include "vcardstrarray.h"

ssize_t vcardstrarray_find(vcardstrarray *array,
                           const char *needle)
{
    size_t i;

    for (i = 0; i < array->num_elements; i++) {
        if (!strcmp(needle, icalarray_element_at(array, i))) return i;
    }

    return -1;
}

void vcardstrarray_append(vcardstrarray *array, const char *new)
{
    char *copy = icalmemory_strdup(new);

    icalarray_append(array, &copy);
}

void vcardstrarray_add(vcardstrarray *array, const char *add)
{
    if (vcardstrarray_find(array, add) < 0)
        vcardstrarray_append(array, add);
}

void vcardstrarray_remove_element_at(vcardstrarray *array, size_t position)
{
    char **del = icalarray_element_at(array, position);

    if (del && *del) icalmemory_free_buffer(*del);
    icalarray_remove_element_at(array, position);
}

void vcardstrarray_remove(vcardstrarray *array, const char *del)
{
    ssize_t position = vcardstrarray_find(array, del);

    if (position >= 0) vcardstrarray_remove_element_at(array, position);
}

void vcardstrarray_free(vcardstrarray *array)
{
    while (array->num_elements)
        vcardstrarray_remove_element_at(array, array->num_elements-1);
    icalarray_free(array);
}

static int strpcmp(const char **a, const char **b)
{
    return strcmp(*a, *b);
}

void vcardstrarray_sort(vcardstrarray *array)
{
    icalarray_sort(array, (int (*)(const void *, const void *)) &strpcmp);
}

