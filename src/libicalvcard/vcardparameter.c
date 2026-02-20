/*======================================================================
 FILE: vcardparameter.c
 CREATOR: Ken Murchison 24 Aug 2022 <murch@fastmailteam.com>

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 ======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "vcardparameter.h"
#include "vcardparameterimpl.h"
#include "icalerror.h"
#include "icalmemory.h"

#include <errno.h>
#include <stdlib.h>

LIBICAL_VCARD_EXPORT struct vcardparameter_impl *vcardparameter_new_impl(vcardparameter_kind kind)
{
    struct vcardparameter_impl *v;

    if ((v = (struct vcardparameter_impl *)icalmemory_new_buffer(sizeof(struct vcardparameter_impl))) == 0) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return 0;
    }

    memset(v, 0, sizeof(struct vcardparameter_impl));

    strcpy(v->id, "para");

    v->kind = kind;
    v->value_kind = vcardparameter_kind_value_kind(kind, &v->is_multivalued);

    return v;
}

vcardparameter *vcardparameter_new(vcardparameter_kind kind)
{
    struct vcardparameter_impl *v = vcardparameter_new_impl(kind);

    return (vcardparameter *)v;
}

void vcardparameter_free(vcardparameter *param)
{
    if (!param || param->parent != 0) {
        return;
    }

    if (param->string != 0) {
        icalmemory_free_buffer((void *)param->string);
    } else if (param->values != 0) {
        if (param->value_kind == VCARD_TEXT_VALUE) {
            vcardstrarray_free(param->values);
        } else {
            vcardenumarray_free(param->values);
        }
    } else if (param->structured != 0) {
        vcardstructured_unref(param->structured);
    }

    if (param->x_name != 0) {
        icalmemory_free_buffer((void *)param->x_name);
    }

    memset(param, 0, sizeof(vcardparameter));

    param->parent = 0;
    param->id[0] = 'X';
    icalmemory_free_buffer(param);
}

vcardparameter *vcardparameter_clone(const vcardparameter *old)
{
    struct vcardparameter_impl *clone;

    icalerror_check_arg_rz((old != 0), "param");

    clone = vcardparameter_new_impl(old->kind);

    if (clone == 0) {
        return 0;
    }

    memcpy(clone, old, sizeof(struct vcardparameter_impl));

    if (old->string != 0) {
        clone->string = icalmemory_strdup(old->string);
        if (clone->string == 0) {
            clone->parent = 0;
            vcardparameter_free(clone);
            return 0;
        }
    }

    if (old->x_name != 0) {
        clone->x_name = icalmemory_strdup(old->x_name);
        if (clone->x_name == 0) {
            clone->parent = 0;
            vcardparameter_free(clone);
            return 0;
        }
    }

    if (old->values != 0) {
        clone->values = old->value_kind == VCARD_TEXT_VALUE ? vcardstrarray_clone(old->values) : vcardenumarray_clone(old->values);
        if (clone->values == 0) {
            clone->parent = 0;
            vcardparameter_free(clone);
            return 0;
        }
    }

    return clone;
}

vcardparameter *vcardparameter_new_from_string(const char *str)
{
    char *eq;
    char *cpy;
    vcardparameter_kind kind;
    vcardparameter *param;

    icalerror_check_arg_rz(str != 0, "str");

    cpy = icalmemory_strdup(str);

    if (cpy == 0) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return 0;
    }

    eq = strchr(cpy, '=');

    if (eq == 0) {
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        icalmemory_free_buffer(cpy);
        return 0;
    }

    *eq = '\0';

    eq++;

    kind = vcardparameter_string_to_kind(cpy);

    if (kind == VCARD_NO_PARAMETER) {
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        icalmemory_free_buffer(cpy);
        return 0;
    }

    param = vcardparameter_new_from_value_string(kind, eq);

    if (kind == VCARD_X_PARAMETER) {
        vcardparameter_set_xname(param, cpy);
    } else if (kind == VCARD_IANA_PARAMETER) {
        vcardparameter_set_iana_name(param, cpy);
    }

    icalmemory_free_buffer(cpy);

    return param;
}

char *vcardparameter_as_vcard_string(vcardparameter *param)
{
    char *buf;

    buf = vcardparameter_as_vcard_string_r(param);
    icalmemory_add_tmp_buffer(buf);
    return buf;
}

/*
 * - param        = param-name "=" param-value
 * - param-name   = iana-token / x-token
 * - param-value  = paramtext /quoted-string
 * - paramtext    = *SAFE-CHAR
 * - quoted-string= DQUOTE *QSAFE-CHAR DQUOTE
 * - QSAFE-CHAR   = any character except CTLs and DQUOTE
 * - SAFE-CHAR    = any character except CTLs, DQUOTE. ";", ":", ","
 */
char *vcardparameter_as_vcard_string_r(vcardparameter *param)
{
    size_t buf_size = 1024;
    char *buf;
    char *buf_ptr;

    icalerror_check_arg_rz((param != 0), "parameter");

    /* Create new buffer that we can append names, parameters and a
     * value to, and reallocate as needed.
     */

    buf = icalmemory_new_buffer(buf_size);
    buf_ptr = buf;

    if (param->kind == VCARD_X_PARAMETER) {
        icalmemory_append_string(&buf, &buf_ptr,
                                 &buf_size, vcardparameter_get_xname(param));
    } else if (param->kind == VCARD_IANA_PARAMETER) {
        icalmemory_append_string(&buf, &buf_ptr,
                                 &buf_size, vcardparameter_get_iana_name(param));
    } else {
        const char *kind_string = vcardparameter_kind_to_string(param->kind);

        if (param->kind == VCARD_NO_PARAMETER ||
            param->kind == VCARD_ANY_PARAMETER || kind_string == 0) {
            icalerror_set_errno(ICAL_BADARG_ERROR);
            icalmemory_free_buffer(buf);
            return 0;
        }

        /* Put the parameter name into the string */
        icalmemory_append_string(&buf, &buf_ptr, &buf_size, kind_string);
    }

    icalmemory_append_string(&buf, &buf_ptr, &buf_size, "=");

    if (param->string != 0) {
        icalmemory_append_encoded_string(&buf, &buf_ptr,
                                         &buf_size, param->string);
    } else if (param->data != 0) {
        char *intbuf = NULL;
        const char *str;

        if (param->value_kind == VCARD_INTEGER_VALUE) {
#define VCARD_INTEGER_LENGTH 21
            intbuf = icalmemory_tmp_buffer(VCARD_INTEGER_LENGTH);
            snprintf(intbuf, VCARD_INTEGER_LENGTH - 1, "%d", param->data);

            str = intbuf;
        } else {
            str = vcardparameter_enum_to_string(param->data);
        }

        icalmemory_append_string(&buf, &buf_ptr, &buf_size, str);

    } else if (param->values != 0) {
        size_t i;
        const char *sep = "";

        for (i = 0; i < param->values->num_elements; i++) {
            icalmemory_append_string(&buf, &buf_ptr, &buf_size, sep);

            if (param->value_kind == VCARD_TEXT_VALUE) {
                const char *str = vcardstrarray_element_at(param->values, i);

                icalmemory_append_encoded_string(&buf, &buf_ptr,
                                                 &buf_size, str);
            } else {
                const vcardenumarray_element *elem =
                    vcardenumarray_element_at(param->values, i);
                if (elem->xvalue != 0) {
                    icalmemory_append_encoded_string(&buf, &buf_ptr,
                                                     &buf_size, elem->xvalue);
                } else {
                    const char *str = vcardparameter_enum_to_string(elem->val);

                    icalmemory_append_string(&buf, &buf_ptr, &buf_size, str);
                }
            }
            sep = ",";
        }
    } else if (vcardparameter_is_structured(param)) {
        char *str = vcardstructured_as_vcard_string_r(param->structured, 1);

        icalmemory_append_encoded_string(&buf, &buf_ptr, &buf_size, str);
        icalmemory_free_buffer(str);
    } else if (vcardtime_is_valid_time(param->date)) {
        const char *str = vcardtime_as_vcard_string(param->date, 0);

        icalmemory_append_string(&buf, &buf_ptr, &buf_size, str);

    } else {
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        icalmemory_free_buffer(buf);
        return 0;
    }

    return buf;
}

vcardparameter_kind vcardparameter_isa(const vcardparameter *parameter)
{
    if (parameter == 0) {
        return VCARD_NO_PARAMETER;
    }

    return parameter->kind;
}

bool vcardparameter_isa_parameter(void *parameter)
{
    const struct vcardparameter_impl *impl = (struct vcardparameter_impl *)parameter;

    if (parameter == 0) {
        return false;
    }

    if (strcmp(impl->id, "para") == 0) {
        return true;
    } else {
        return false;
    }
}

void vcardparameter_set_xname(vcardparameter *param, const char *v)
{
    icalerror_check_arg_rv((param != 0), "param");
    icalerror_check_arg_rv((v != 0), "v");

    if (param->x_name != 0) {
        icalmemory_free_buffer((void *)param->x_name);
    }

    param->x_name = icalmemory_strdup(v);

    if (param->x_name == 0) {
        errno = ENOMEM;
    }
}

const char *vcardparameter_get_xname(const vcardparameter *param)
{
    icalerror_check_arg_rz((param != 0), "param");

    return param->x_name;
}

void vcardparameter_set_xvalue(vcardparameter *param, const char *v)
{
    icalerror_check_arg_rv((param != 0), "param");
    icalerror_check_arg_rv((v != 0), "v");

    if (param->string != 0) {
        icalmemory_free_buffer((void *)param->string);
    }

    param->string = icalmemory_strdup(v);

    if (param->string == 0) {
        errno = ENOMEM;
    }
}

const char *vcardparameter_get_xvalue(const vcardparameter *param)
{
    icalerror_check_arg_rz((param != 0), "param");

    return param->string;
}

void vcardparameter_set_iana_value(vcardparameter *param, const char *v)
{
    vcardparameter_set_xvalue(param, v);
}

const char *vcardparameter_get_iana_value(const vcardparameter *param)
{
    return vcardparameter_get_xvalue(param);
}

void vcardparameter_set_iana_name(vcardparameter *param, const char *v)
{
    vcardparameter_set_xname(param, v);
}

const char *vcardparameter_get_iana_name(const vcardparameter *param)
{
    return vcardparameter_get_xname(param);
}

void vcardparameter_set_parent(vcardparameter *param, vcardproperty *property)
{
    icalerror_check_arg_rv((param != 0), "param");

    param->parent = property;
}

vcardproperty *vcardparameter_get_parent(const vcardparameter *param)
{
    icalerror_check_arg_rz((param != 0), "param");

    return param->parent;
}

bool vcardparameter_has_same_name(const vcardparameter *param1, const vcardparameter *param2)
{
    vcardparameter_kind kind1;
    vcardparameter_kind kind2;
    const char *name1;
    const char *name2;

    icalerror_check_arg_rz((param1 != 0), "param1");
    icalerror_check_arg_rz((param2 != 0), "param2");

    kind1 = vcardparameter_isa(param1);
    kind2 = vcardparameter_isa(param2);

    if (kind1 != kind2) {
        return false;
    }

    if (kind1 == VCARD_X_PARAMETER) {
        name1 = vcardparameter_get_xname(param1);
        name2 = vcardparameter_get_xname(param2);
        if (strcasecmp(name1, name2) != 0) {
            return false;
        }
    } else if (kind1 == VCARD_IANA_PARAMETER) {
        name1 = vcardparameter_get_iana_name(param1);
        name2 = vcardparameter_get_iana_name(param2);
        if (strcasecmp(name1, name2) != 0) {
            return false;
        }
    }
    return true;
}

bool vcardparameter_is_multivalued(const vcardparameter *param)
{
    icalerror_check_arg_rz((param != 0), "param");

    return param->is_multivalued;
}

bool vcardparameter_is_structured(const vcardparameter *param)
{
    icalerror_check_arg_rz((param != 0), "param");

    return (param->value_kind == VCARD_STRUCTURED_VALUE);
}

/* Everything below this line is machine generated. Do not edit. */
/* ALTREP */
