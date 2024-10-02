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
    /*  Comment out the following as it always triggers, even when parameter is non-zero
    icalerror_check_arg_rv((parameter==0),"parameter");*/

    if (param->parent != 0) {
        return;
    }

    if (param->string != 0) {
        icalmemory_free_buffer((void *)param->string);
    } else if (param->values != 0) {
        if (param->value_kind == VCARD_TEXT_VALUE)
            vcardstrarray_free(param->values);
        else
            vcardenumarray_free(param->values);
    } else if (param->structured != 0) {
        vcardstructured_free(param->structured);
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

    return clone;
}

vcardparameter *vcardparameter_new_clone(vcardparameter *old)
{
    return vcardparameter_clone(old);
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
 * Checks whether this character is allowed in a (Q)SAFE-CHAR
 *
 * QSAFE-CHAR   = WSP / %x21 / %x23-7E / NON-US-ASCII
 * ; any character except CTLs and DQUOTE
 * SAFE-CHAR    = WSP / %x21 / %x23-39 / %x3C-7E / NON-US-ASCII
 * ; any character except CTLs, DQUOTE. ";", ":", ","
 * WSP      = SPACE / HTAB
 * NON-US-ASCII       = %x80-F8
 * ; Use restricted by charset parameter
 * ; on outer MIME object (UTF-8 preferred)
 */
static int vcardparameter_is_safe_char(unsigned char character, int quoted)
{
    if (character == ' ' || character == '\t' || character == '!' ||
        (character >= 0x80 && character <= 0xF8)) {
        return 1;
    }

    if (quoted && character >= 0x23 && character <= 0x7e) {
        return 1;
    } else if (!quoted &&
               ((character >= 0x23 && character <= 0x39) ||
                (character >= 0x3c && character <= 0x7e))) {
        return 1;
    }

    return 0;
}

/**
 * Appends the parameter value to the buffer, encoding per RFC 6868
 * and filtering out those characters not permitted by the specifications
 *
 * paramtext    = *SAFE-CHAR
 * quoted-string= DQUOTE *QSAFE-CHAR DQUOTE
 */
static void vcardparameter_append_encoded_value(char **buf, char **buf_ptr,
                                                size_t *buf_size, const char *value)
{
    int qm = 0;
    const char *p;

    /* Encapsulate the property in quotes if necessary */
    if (!*value || strpbrk(value, ";:,") != 0) {
        icalmemory_append_char(buf, buf_ptr, buf_size, '"');
        qm = 1;
    }

    /* Copy the parameter value */
    for (p = value; *p; p++) {
        /* Encode unsafe characters per RFC6868, otherwise replace with SP */
        switch (*p) {
        case '\n':
            icalmemory_append_string(buf, buf_ptr, buf_size, "^n");
            break;

        case '^':
            icalmemory_append_string(buf, buf_ptr, buf_size, "^^");
            break;

        case '"':
            icalmemory_append_string(buf, buf_ptr, buf_size, "^'");
            break;

        default:
            if (vcardparameter_is_safe_char((unsigned char)*p, qm)) {
                icalmemory_append_char(buf, buf_ptr, buf_size, *p);
            } else {
                icalmemory_append_char(buf, buf_ptr, buf_size, ' ');
            }
            break;
        }
    }

    if (qm == 1) {
        icalmemory_append_char(buf, buf_ptr, buf_size, '"');
    }
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
    const char *kind_string;

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
        kind_string = vcardparameter_kind_to_string(param->kind);

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
        vcardparameter_append_encoded_value(&buf, &buf_ptr,
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

                vcardparameter_append_encoded_value(&buf, &buf_ptr,
                                                    &buf_size, str);
            } else {
                const vcardenumarray_element *elem =
                    vcardenumarray_element_at(param->values, i);
                if (elem->xvalue != 0) {
                    vcardparameter_append_encoded_value(&buf, &buf_ptr,
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

        vcardparameter_append_encoded_value(&buf, &buf_ptr, &buf_size, str);
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

vcardparameter_kind vcardparameter_isa(vcardparameter *parameter)
{
    if (parameter == 0) {
        return VCARD_NO_PARAMETER;
    }

    return parameter->kind;
}

int vcardparameter_isa_parameter(void *parameter)
{
    struct vcardparameter_impl *impl = (struct vcardparameter_impl *)parameter;

    if (parameter == 0) {
        return 0;
    }

    if (strcmp(impl->id, "para") == 0) {
        return 1;
    } else {
        return 0;
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

const char *vcardparameter_get_xname(vcardparameter *param)
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

const char *vcardparameter_get_xvalue(vcardparameter *param)
{
    icalerror_check_arg_rz((param != 0), "param");

    return param->string;
}

void vcardparameter_set_iana_value(vcardparameter *param, const char *v)
{
    vcardparameter_set_xvalue(param, v);
}

const char *vcardparameter_get_iana_value(vcardparameter *param)
{
    return vcardparameter_get_xvalue(param);
}

void vcardparameter_set_iana_name(vcardparameter *param, const char *v)
{
    vcardparameter_set_xname(param, v);
}

const char *vcardparameter_get_iana_name(vcardparameter *param)
{
    return vcardparameter_get_xname(param);
}

void vcardparameter_set_parent(vcardparameter *param, vcardproperty *property)
{
    icalerror_check_arg_rv((param != 0), "param");

    param->parent = property;
}

vcardproperty *vcardparameter_get_parent(vcardparameter *param)
{
    icalerror_check_arg_rz((param != 0), "param");

    return param->parent;
}

int vcardparameter_has_same_name(vcardparameter *param1, vcardparameter *param2)
{
    vcardparameter_kind kind1;
    vcardparameter_kind kind2;
    const char *name1;
    const char *name2;

    icalerror_check_arg_rz((param1 != 0), "param1");
    icalerror_check_arg_rz((param2 != 0), "param2");

    kind1 = vcardparameter_isa(param1);
    kind2 = vcardparameter_isa(param2);

    if (kind1 != kind2)
        return 0;

    if (kind1 == VCARD_X_PARAMETER) {
        name1 = vcardparameter_get_xname(param1);
        name2 = vcardparameter_get_xname(param2);
        if (strcasecmp(name1, name2) != 0) {
            return 0;
        }
    } else if (kind1 == VCARD_IANA_PARAMETER) {
        name1 = vcardparameter_get_iana_name(param1);
        name2 = vcardparameter_get_iana_name(param2);
        if (strcasecmp(name1, name2) != 0) {
            return 0;
        }
    }
    return 1;
}

int vcardparameter_is_multivalued(vcardparameter *param)
{
    icalerror_check_arg_rz((param != 0), "param");

    return param->is_multivalued;
}

int vcardparameter_is_structured(vcardparameter *param)
{
    icalerror_check_arg_rz((param != 0), "param");

    return (param->value_kind == VCARD_STRUCTURED_VALUE);
}

/* Everything below this line is machine generated. Do not edit. */
/* ALTREP */
