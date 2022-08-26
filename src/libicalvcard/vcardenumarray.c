#include <string.h>

#include "icalmemory.h"
#include "vcardenumarray.h"

ssize_t vcardenumarray_find(vcardenumarray *array,
                            vcardenumarray_element *needle)
{
    size_t i;

    for (i = 0; i < array->num_elements; i++) {
        vcardenumarray_element *e = icalarray_element_at(array, i);

        if (e->val == needle->val &&
            !!e->xvalue == !!needle->xvalue &&
            (!e->xvalue || !strcmp(e->xvalue, needle->xvalue))) {
            return i;
        }
    }

    return -1;
}

void vcardenumarray_append(vcardenumarray *array, vcardenumarray_element *new)
{
    vcardenumarray_element copy = {
        new->val, new->xvalue ? icalmemory_strdup(new->xvalue) : NULL
    };

    icalarray_append(array, &copy);
}

void vcardenumarray_add(vcardenumarray *array, vcardenumarray_element *add)
{
    if (vcardenumarray_find(array, add) < 0)
        vcardenumarray_append(array, add);
}

void vcardenumarray_remove_element_at(vcardenumarray *array,
                                      size_t position)
{
    vcardenumarray_element *del = icalarray_element_at(array, position);

    if (del->xvalue) icalmemory_free_buffer((char *) del->xvalue);
    icalarray_remove_element_at(array, position);
}

void vcardenumarray_remove(vcardenumarray *array, vcardenumarray_element *del)
{
    ssize_t position = vcardenumarray_find(array, del);

    if (position >= 0) vcardenumarray_remove_element_at(array, position);
}

void vcardenumarray_free(vcardenumarray *array)
{
    while (array->num_elements)
        vcardenumarray_remove_element_at(array, array->num_elements-1);
    icalarray_free(array);
}

static int enumcmp(const vcardenumarray_element *a, const vcardenumarray_element *b)
{
    /* Sort X- values alphabetically, but last */
    if (a->xvalue) {
        if (b->xvalue) return strcmp(a->xvalue, b->xvalue);
        else return 1;
    }
    else if (b->xvalue) {
        return -1;
    }
    else {
        return a->val - b->val;
    }
}

void vcardenumarray_sort(vcardenumarray *array)
{
    icalarray_sort(array, (int (*)(const void *, const void *)) &enumcmp);
}
