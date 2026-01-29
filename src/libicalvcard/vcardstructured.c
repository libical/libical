/*======================================================================
 FILE: vcardstructured.c
 CREATOR: Ken Murchison 24 Aug 2022

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 ======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "vcardstructured.h"
#include "icalerror.h"
#include "icalmemory.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

vcardstructuredtype *vcardstructured_new(void)
{
    vcardstructuredtype *s;

    s = (vcardstructuredtype *)icalmemory_new_buffer(sizeof(vcardstructuredtype));
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
    ptrdiff_t len = 0;
    size_t alloc = 100;
    char *pos, *buf = icalmemory_new_buffer(alloc);
    vcardstrarray *field = vcardstrarray_new(2);

    st->field[st->num_fields++] = field;

    for (; *s; s++) {
        pos = buf + len;

        switch (*s) {
        case '\\':
            if (s[1]) {
                icalmemory_append_char(&buf, &pos, &alloc, s[1]);
                len = (ptrdiff_t)(pos - buf);
                s++;
            }
            break;

        case ',':
        case ';':
            /* end of value */
            if (len || *s == ',' || vcardstrarray_size(field)) {
                icalmemory_append_char(&buf, &pos, &alloc, '\0');
                vcardstrarray_append(field, buf);
            }
            len = 0;

            if (*s == ';') {
                /* end of field */
                field = vcardstrarray_new(2);
                st->field[st->num_fields++] = field;
            }
            break;

        default:
            icalmemory_append_char(&buf, &pos, &alloc, *s);
            len = (ptrdiff_t)(pos - buf);
            break;
        }
    }

    /* end of value */
    pos = buf + len;
    if (len || vcardstrarray_size(field)) {
        icalmemory_append_char(&buf, &pos, &alloc, '\0');
        vcardstrarray_append(field, buf);
    }

    icalmemory_free_buffer(buf);

    return st;
}

void vcardstructured_free(vcardstructuredtype *s)
{
    unsigned i;

    for (i = 0; i < s->num_fields; i++) {
        if (s->field[i]) {
            vcardstrarray_free(s->field[i]);
        }
    }
    icalmemory_free_buffer((void *)s);
}
