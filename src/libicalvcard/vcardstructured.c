#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "icalerror.h"
#include "icalmemory.h"
#include "vcardstructured.h"

vcardstructuredtype *vcardstructured_new(void)
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

vcardstructuredtype *vcardstructured_from_string(const char *s)
{
    vcardstructuredtype *st = vcardstructured_new();
    size_t len = 0, alloc = 100;
    char *pos, *buf = icalmemory_new_buffer(alloc);
    vcardstrarray *field = vcardstrarray_new(2);

    st->field[st->num_fields++] = field;

    for (; *s; s++) {
        pos = buf + len;

        switch (*s) {
        case ',':
        case ';':
            /* end of value */
            icalmemory_append_char(&buf, &pos, &alloc, '\0');
            vcardstrarray_append(field, buf);
            len = 0;

            if (*s == ';') {
                /* end of field */
                field = vcardstrarray_new(2);
                st->field[st->num_fields++] = field;
            }
            break;

        default:
            icalmemory_append_char(&buf, &pos, &alloc, *s);
            len = pos - buf;
            break;
        }
    }

    /* end of value */
    icalmemory_append_char(&buf, &pos, &alloc, '\0');
    vcardstrarray_append(field, buf);

    icalmemory_free_buffer(buf);

    return st;
}

void vcardstructured_free(vcardstructuredtype *s)
{
    unsigned i;

    for (i = 0; i < s->num_fields; i++)
        if (s->field[i]) vcardstrarray_free(s->field[i]);
    icalmemory_free_buffer((void *)s);
}
