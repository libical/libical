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
#include "vcardvalue.h"
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

vcardstructuredtype *vcardstructured_new_from_string(const char *str)
{
    vcardstructuredtype *st = vcardstructured_new();
    vcardstrarray *field = vcardstrarray_new(2);

    st->field[st->num_fields++] = field;

    do {
        char *dequoted_str = vcardvalue_strdup_and_dequote_text(&str, ",;");

        if (*str == ',' || field->num_elements || strlen(dequoted_str)) {
            vcardstrarray_append(field, dequoted_str);
        }
        icalmemory_free_buffer(dequoted_str);

        if (*str == ';') {
            /* end of field */
            field = vcardstrarray_new(2);
            st->field[st->num_fields++] = field;
        }

    } while (*str++ != '\0');

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
