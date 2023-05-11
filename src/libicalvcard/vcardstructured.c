#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "icalerror.h"
#include "icalmemory.h"
#include "vcardstructured.h"

vcardstructuredtype *vcardstructured_new()
{
    vcardstructuredtype *s;

    s = (vcardstructuredtype *) icalmemory_new_buffer(sizeof(vcardstructuredtype));
    if (!s) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return NULL;
    }

    memset(s, 0, sizeof(vcardstructuredtype));

    return s;
}

char *vcardstructured_as_vcard_string_r(vcardstructuredtype *s)
{
    vcardstrarray *array;
    char *buf, *buf_ptr;
    size_t buf_size;
    unsigned i, num_fields;

    num_fields = s->num_fields;
    buf_size = num_fields * 25;  // arbitrary
    buf_ptr = buf = icalmemory_new_buffer(buf_size);

    for (i = 0; i < num_fields; i++) {
        array = s->field[i];
        if (i) icalmemory_append_char(&buf, &buf_ptr, &buf_size, ';');
        if (array)
            vcardstrarray_as_vcard_string_r(array, ',',
                                            &buf, &buf_ptr, &buf_size);
    }

    return buf;
}

void vcardstructured_free(vcardstructuredtype *s)
{
    unsigned i;

    for (i = 0; i < s->num_fields; i++)
        if (s->field[i]) vcardstrarray_free(s->field[i]);
    icalmemory_free_buffer((void *)s);
}
