/*======================================================================
 FILE: vcardstructured.c
 CREATOR: Ken Murchison 24 Aug 2022

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 ======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "vcardstructuredimpl.h"
#include "vcardstructured.h"
#include "vcardvalue.h"
#include "icalerror.h"
#include "icalmemory.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static vcardstructuredtype *vcardstructured_alloc(void)
{
    struct vcardstructuredtype_impl *st;

    st = icalmemory_new_buffer(sizeof(struct vcardstructuredtype_impl));
    if (!st) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return NULL;
    }
    memset(st, 0, sizeof(struct vcardstructuredtype_impl));

    return st;
}

static void vcardstructured_free(vcardstructuredtype *st)
{
    for (size_t i = 0; i < st->num_fields; i++) {
        if (st->field[i]) {
            vcardstrarray_free(st->field[i]);
        }
    }
    icalmemory_free_buffer((void *)st);
}

vcardstructuredtype *vcardstructured_new(size_t num_fields)
{
    struct vcardstructuredtype_impl *st = vcardstructured_alloc();
    if (!st) {
        return st;
    }
    vcardstructured_ref(st);

    st->num_fields = num_fields;
    return st;
}

vcardstructuredtype *vcardstructured_new_from_string(const char *str)
{
    struct vcardstructuredtype_impl *st = vcardstructured_alloc();
    if (!st) {
        return st;
    }
    vcardstructured_ref(st);

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

void vcardstructured_ref(vcardstructuredtype *st)
{
    icalerror_check_arg_rv((st != NULL), "st");

    st->refcount++;
}

void vcardstructured_unref(vcardstructuredtype *st)
{
    icalerror_check_arg_rv((st != NULL), "st");
    icalerror_check_arg_rv((st->refcount > 0), "st->refcount > 0");

    st->refcount--;

    if (st->refcount != 0) {
        return;
    }

    vcardstructured_free(st);
}

vcardstructuredtype *vcardstructured_clone(const vcardstructuredtype *st)
{
    icalerror_check_arg_rz((st != NULL), "st");

    struct vcardstructuredtype_impl *clone_st = vcardstructured_alloc();
    if (!clone_st) {
        return NULL;
    }
    vcardstructured_ref(clone_st);

    clone_st->num_fields = st->num_fields;

    for (size_t i = 0; i < st->num_fields; i++) {
        clone_st->field[i] = vcardstrarray_clone(st->field[i]);
    }

    return clone_st;
}

size_t vcardstructured_num_fields(const vcardstructuredtype *st)
{
    icalerror_check_arg_rz((st != NULL), "st");
    return st->num_fields;
}

void vcardstructured_set_num_fields(vcardstructuredtype *st,
                                    size_t num_fields)
{
    icalerror_check_arg_rv((st != NULL), "st");

    for (size_t i = num_fields; i < st->num_fields; i++) {
        vcardstrarray_free(st->field[i]);
    }

    st->num_fields = num_fields;
}

vcardstrarray *vcardstructured_field_at(const vcardstructuredtype *st,
                                        size_t position)
{
    icalerror_check_arg_rz((st != NULL), "st");
    icalerror_check_arg_rz((st->num_fields > position), "position");

    return st->field[position];
}

void vcardstructured_set_field_at(vcardstructuredtype *st,
                                  size_t position,
                                  vcardstrarray *field)
{
    icalerror_check_arg_rv((st != NULL), "st");

    if (position >= st->num_fields) {
        vcardstructured_set_num_fields(st, position + 1);
    }

    vcardstrarray_free(st->field[position]);
    st->field[position] = field;
}
