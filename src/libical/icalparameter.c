/*======================================================================
 FILE: icalparameter.c
 CREATOR: eric 09 May 1999

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

  The original code is icalderivedparameters.{c,h}

 Contributions from:
    Graham Davison <g.m.davison@computer.org>
======================================================================*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "icalparameter.h"
#include "icalparameterimpl.h"
#include "icalerror.h"
#include "icalmemory.h"

#include <errno.h>
#include <stdlib.h>

LIBICAL_ICAL_EXPORT struct icalparameter_impl *icalparameter_new_impl(icalparameter_kind kind)
{
    struct icalparameter_impl *v;

    if ((v = (struct icalparameter_impl *)icalmemory_new_buffer(sizeof(struct icalparameter_impl))) == 0) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return 0;
    }

    memset(v, 0, sizeof(struct icalparameter_impl));

    strcpy(v->id, "para");

    v->kind = kind;
    v->value_kind = icalparameter_kind_value_kind(kind, &v->is_multivalued);

    return v;
}

icalparameter *icalparameter_new(icalparameter_kind kind)
{
    struct icalparameter_impl *v = icalparameter_new_impl(kind);

    return (icalparameter *)v;
}

void icalparameter_free(icalparameter *param)
{
    /*  Comment out the following as it always triggers, even when parameter is non-zero
    icalerror_check_arg_rv((parameter==0),"parameter");*/

    if (param->parent != 0) {
        return;
    }

    if (param->string != 0) {
        icalmemory_free_buffer((void *)param->string);
    } else if (param->values != 0) {
        if (param->value_kind == ICAL_TEXT_VALUE) {
            icalstrarray_free(param->values);
        } else {
            icalenumarray_free(param->values);
        }
    }

    icalmemory_free_buffer((void *)param->x_name);

    memset(param, 0, sizeof(icalparameter));

    param->parent = 0;
    param->id[0] = 'X';
    icalmemory_free_buffer(param);
}

icalparameter *icalparameter_clone(const icalparameter *old)
{
    struct icalparameter_impl *clone;

    icalerror_check_arg_rz((old != 0), "param");

    clone = icalparameter_new_impl(old->kind);

    if (clone == 0) {
        return 0;
    }

    memcpy(clone, old, sizeof(struct icalparameter_impl));

    if (old->string != 0) {
        clone->string = icalmemory_strdup(old->string);
        if (clone->string == 0) {
            clone->parent = 0;
            icalparameter_free(clone);
            return 0;
        }
    }

    if (old->x_name != 0) {
        clone->x_name = icalmemory_strdup(old->x_name);
        if (clone->x_name == 0) {
            clone->parent = 0;
            icalparameter_free(clone);
            return 0;
        }
    }

    if (old->values != 0) {
        clone->values = old->value_kind == ICAL_TEXT_VALUE ? icalstrarray_clone(old->values) : icalenumarray_clone(old->values);
        if (clone->values == 0) {
            clone->parent = 0;
            icalparameter_free(clone);
            return 0;
        }
    }

    return clone;
}

icalparameter *icalparameter_new_from_string(const char *str)
{
    char *eq;
    char *cpy;
    icalparameter_kind kind;
    icalparameter *param;

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

    kind = icalparameter_string_to_kind(cpy);

    if (kind == ICAL_NO_PARAMETER) {
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        icalmemory_free_buffer(cpy);
        return 0;
    }

    param = icalparameter_new_from_value_string(kind, eq);

    if (kind == ICAL_X_PARAMETER) {
        icalparameter_set_xname(param, cpy);
    } else if (kind == ICAL_IANA_PARAMETER) {
        icalparameter_set_iana_name(param, cpy);
    }

    icalmemory_free_buffer(cpy);

    return param;
}

char *icalparameter_as_ical_string(icalparameter *param)
{
    char *buf;

    buf = icalparameter_as_ical_string_r(param);
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
char *icalparameter_as_ical_string_r(icalparameter *param)
{
    size_t buf_size = 1024;
    char *buf;
    char *buf_ptr;
    const char *kind_string;

    icalerror_check_arg_rz((param != 0), "parameter");

    /* Create new buffer that we can append names, parameters and a
     * value to, and reallocate as needed.
     */

    buf = icalmemory_new_buffer(buf_size);
    buf_ptr = buf;

    if (param->kind == ICAL_X_PARAMETER) {
        icalmemory_append_string(&buf, &buf_ptr, &buf_size, icalparameter_get_xname(param));
    } else if (param->kind == ICAL_IANA_PARAMETER) {
        icalmemory_append_string(&buf, &buf_ptr, &buf_size, icalparameter_get_iana_name(param));
    } else {
        kind_string = icalparameter_kind_to_string(param->kind);

        if (param->kind == ICAL_NO_PARAMETER ||
            param->kind == ICAL_ANY_PARAMETER || kind_string == 0) {
            icalerror_set_errno(ICAL_BADARG_ERROR);
            icalmemory_free_buffer(buf);
            return 0;
        }

        /* Put the parameter name into the string */
        icalmemory_append_string(&buf, &buf_ptr, &buf_size, kind_string);
    }

    icalmemory_append_string(&buf, &buf_ptr, &buf_size, "=");

    if (param->kind == ICAL_GAP_PARAMETER) {
        char *str = icaldurationtype_as_ical_string_r(param->duration);

        icalmemory_append_string(&buf, &buf_ptr, &buf_size, str);
        icalmemory_free_buffer(str);
    } else if (param->string != 0) {
        icalmemory_append_encoded_string(&buf, &buf_ptr, &buf_size, param->string);
    } else if (param->data != 0) {
        const char *str = icalparameter_enum_to_string(param->data);

        icalmemory_append_string(&buf, &buf_ptr, &buf_size, str);
    } else if (param->values != 0) {
        size_t i;
        const char *sep = "";

        for (i = 0; i < param->values->num_elements; i++) {
            icalmemory_append_string(&buf, &buf_ptr, &buf_size, sep);

            if (param->value_kind == ICAL_TEXT_VALUE) {
                const char *str = icalstrarray_element_at(param->values, i);

                icalmemory_append_encoded_string(&buf, &buf_ptr,
                                                 &buf_size, str);
            } else {
                const icalenumarray_element *elem =
                    icalenumarray_element_at(param->values, i);
                if (elem->xvalue != 0) {
                    icalmemory_append_encoded_string(&buf, &buf_ptr,
                                                     &buf_size, elem->xvalue);
                } else {
                    const char *str = icalparameter_enum_to_string(elem->val);

                    icalmemory_append_string(&buf, &buf_ptr, &buf_size, str);
                }
            }
            sep = ",";
        }
    } else {
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        icalmemory_free_buffer(buf);
        return 0;
    }

    return buf;
}

icalparameter_kind icalparameter_isa(const icalparameter *parameter)
{
    if (parameter == 0) {
        return ICAL_NO_PARAMETER;
    }

    return parameter->kind;
}

bool icalparameter_isa_parameter(void *parameter)
{
    const struct icalparameter_impl *impl = (struct icalparameter_impl *)parameter;

    if (parameter == 0) {
        return false;
    }

    if (strcmp(impl->id, "para") == 0) {
        return true;
    } else {
        return false;
    }
}

void icalparameter_set_xname(icalparameter *param, const char *v)
{
    icalerror_check_arg_rv((param != 0), "param");
    icalerror_check_arg_rv((v != 0), "v");

    icalmemory_free_buffer((void *)param->x_name);
    param->x_name = icalmemory_strdup(v);

    if (param->x_name == 0) {
        errno = ENOMEM;
    }
}

const char *icalparameter_get_xname(const icalparameter *param)
{
    icalerror_check_arg_rz((param != 0), "param");

    return param->x_name;
}

void icalparameter_set_xvalue(icalparameter *param, const char *v)
{
    icalerror_check_arg_rv((param != 0), "param");
    icalerror_check_arg_rv((v != 0), "v");

    icalmemory_free_buffer((void *)param->string);
    param->string = icalmemory_strdup(v);

    if (param->string == 0) {
        errno = ENOMEM;
    }
}

const char *icalparameter_get_xvalue(const icalparameter *param)
{
    icalerror_check_arg_rz((param != 0), "param");

    return param->string;
}

void icalparameter_set_iana_value(icalparameter *param, const char *v)
{
    icalparameter_set_xvalue(param, v);
}

const char *icalparameter_get_iana_value(const icalparameter *param)
{
    return icalparameter_get_xvalue(param);
}

void icalparameter_set_iana_name(icalparameter *param, const char *v)
{
    icalparameter_set_xname(param, v);
}

const char *icalparameter_get_iana_name(const icalparameter *param)
{
    return icalparameter_get_xname(param);
}

void icalparameter_set_parent(icalparameter *param, icalproperty *property)
{
    icalerror_check_arg_rv((param != 0), "param");

    param->parent = property;
}

icalproperty *icalparameter_get_parent(const icalparameter *param)
{
    icalerror_check_arg_rz((param != 0), "param");

    return param->parent;
}

bool icalparameter_has_same_name(const icalparameter *param1, const icalparameter *param2)
{
    icalparameter_kind kind1;
    icalparameter_kind kind2;
    const char *name1;
    const char *name2;

    icalerror_check_arg_rz((param1 != 0), "param1");
    icalerror_check_arg_rz((param2 != 0), "param2");

    kind1 = icalparameter_isa(param1);
    kind2 = icalparameter_isa(param2);

    if (kind1 != kind2) {
        return false;
    }

    if (kind1 == ICAL_X_PARAMETER) {
        name1 = icalparameter_get_xname(param1);
        name2 = icalparameter_get_xname(param2);
        if (strcasecmp(name1, name2) != 0) {
            return false;
        }
    } else if (kind1 == ICAL_IANA_PARAMETER) {
        name1 = icalparameter_get_iana_name(param1);
        name2 = icalparameter_get_iana_name(param2);
        if (strcasecmp(name1, name2) != 0) {
            return false;
        }
    }
    return true;
}

bool icalparameter_is_multivalued(const icalparameter *param)
{
    icalerror_check_arg_rz((param != 0), "param");

    return param->is_multivalued;
}

/** Decode parameter value per RFC6868 */
void icalparameter_decode_value(char *value)
{
    char *in, *out;

    for (in = out = value; *in; in++, out++) {
        int found_escaped_char = 0;

        if (*in == '^') {
            switch (*(in + 1)) {
            case 'n':
                *out = '\n';
                found_escaped_char = 1;
                break;
            case '^':
                *out = '^';
                found_escaped_char = 1;
                break;

            case '\'':
                *out = '"';
                found_escaped_char = 1;
                break;
            default:
                break;
            }
        }

        if (found_escaped_char) {
            ++in;
        } else {
            *out = *in;
        }
    }

    while (*out) {
        *out++ = '\0';
    }
}

/* Everything below this line is machine generated. Do not edit. */
/* ALTREP */
