/*======================================================================
 FILE: icalproperty.c
 CREATOR: eric 28 April 1999

 (C) COPYRIGHT 2000, Eric Busboom <eric@civicknowledge.com>

 This library is free software; you can redistribute it and/or modify
 it under the terms of either:

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: https://www.gnu.org/licenses/lgpl-2.1.html

 Or:

    The Mozilla Public License Version 2.0. You may obtain a copy of
    the License at https://www.mozilla.org/MPL/

  The original code is icalproperty.c
======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "icalproperty_p.h"
#include "icalcomponent.h"
#include "icalerror.h"
#include "icalmemory.h"
#include "icalparser.h"
#include "icaltimezone.h"
#include "icalvalue.h"
#include "pvl.h"

#include <stdlib.h>

struct icalproperty_impl
{
    char id[5];
    icalproperty_kind kind;
    char *x_name;
    pvl_list parameters;
    pvl_elem parameter_iterator;
    icalvalue *value;
    icalcomponent *parent;
};

void icalproperty_add_parameters(icalproperty *prop, va_list args)
{
    void *vp;

    while ((vp = va_arg(args, void *)) != 0)
    {
        if (icalvalue_isa_value(vp) != 0) {
        } else if (icalparameter_isa_parameter(vp) != 0) {

            icalproperty_add_parameter((icalproperty *) prop, (icalparameter *) vp);
        } else {
            icalerror_set_errno(ICAL_BADARG_ERROR);
        }
    }
}

icalproperty *icalproperty_new_impl(icalproperty_kind kind)
{
    icalproperty *prop;

    if (!icalproperty_kind_is_valid(kind))
        return NULL;

    if ((prop = (icalproperty *) malloc(sizeof(icalproperty))) == 0) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return 0;
    }

    memset(prop, 0, sizeof(icalproperty));

    strcpy(prop->id, "prop");

    prop->kind = kind;
    prop->parameters = pvl_newlist();

    return prop;
}

icalproperty *icalproperty_new(icalproperty_kind kind)
{
    if (kind == ICAL_NO_PROPERTY) {
        return 0;
    }

    return (icalproperty *) icalproperty_new_impl(kind);
}

icalproperty *icalproperty_clone(const icalproperty *old)
{
    icalproperty *new;
    pvl_elem p;

    icalerror_check_arg_rz((old != 0), "old");
    new = icalproperty_new_impl(old->kind);
    icalerror_check_arg_rz((new != 0), "new");

    if (old->value != 0) {
        new->value = icalvalue_clone(old->value);
    }

    if (old->x_name != 0) {

        new->x_name = icalmemory_strdup(old->x_name);

        if (new->x_name == 0) {
            icalproperty_free(new);
            icalerror_set_errno(ICAL_NEWFAILED_ERROR);
            return 0;
        }
    }

    for (p = pvl_head(old->parameters); p != 0; p = pvl_next(p)) {
        icalparameter *param = icalparameter_clone(pvl_data(p));

        if (param == 0) {
            icalproperty_free(new);
            icalerror_set_errno(ICAL_NEWFAILED_ERROR);
            return 0;
        }

        pvl_push(new->parameters, param);
    }

    return new;
}

icalproperty *icalproperty_new_clone(icalproperty *old)
{
    return icalproperty_clone(old);
}

icalproperty *icalproperty_new_from_string(const char *str)
{
    size_t buf_size = 1024;
    char *buf;
    char *buf_ptr;
    icalproperty *prop;
    icalcomponent *comp;
    int errors = 0;

    icalerror_check_arg_rz((str != 0), "str");

    buf = icalmemory_new_buffer(buf_size);
    buf_ptr = buf;

    /* Is this a HACK or a crafty reuse of code? */

    icalmemory_append_string(&buf, &buf_ptr, &buf_size, "BEGIN:VCALENDAR\r\n");
    icalmemory_append_string(&buf, &buf_ptr, &buf_size, str);
    icalmemory_append_string(&buf, &buf_ptr, &buf_size, "\r\n");
    icalmemory_append_string(&buf, &buf_ptr, &buf_size, "END:VCALENDAR\r\n");

    comp = icalparser_parse_string(buf);

    if (comp == 0) {
        icalerror_set_errno(ICAL_PARSE_ERROR);
        free(buf);
        return 0;
    }

    errors = icalcomponent_count_errors(comp);

    prop = icalcomponent_get_first_property(comp, ICAL_ANY_PROPERTY);

    icalcomponent_remove_property(comp, prop);

    icalcomponent_free(comp);
    free(buf);

    if (errors > 0) {
        icalproperty_free(prop);
        return 0;
    } else {
        return prop;
    }
}

void icalproperty_free(icalproperty *p)
{
    icalparameter *param;

    icalerror_check_arg_rv((p != 0), "prop");

    if (p->parent != 0) {
        return;
    }

    if (p->value != 0) {
        icalvalue_set_parent(p->value, 0);
        icalvalue_free(p->value);
    }

    while ((param = pvl_pop(p->parameters)) != 0) {
        icalparameter_free(param);
    }

    pvl_free(p->parameters);

    if (p->x_name != 0) {
        free(p->x_name);
    }

    p->kind = ICAL_NO_PROPERTY;
    p->parameters = 0;
    p->parameter_iterator = 0;
    p->value = 0;
    p->x_name = 0;
    p->id[0] = 'X';

    free(p);
}

/* This returns where the start of the next line should be. chars_left does
   not include the trailing '\0'. */
static const size_t MAX_LINE_LEN = 75;

static char *get_next_line_start(char *line_start, size_t chars_left)
{
    char *pos;

    /* If we have 74 chars or less left, we can output all of them.
       we return a pointer to the '\0' at the end of the string. */
    if (chars_left < MAX_LINE_LEN) {
        return line_start + chars_left;
    }

    /* Now we jump to the last possible character of the line, and step back
       trying to find a ';' ':' or ' '. If we find one, we return the character
       after it. */
    pos = line_start + MAX_LINE_LEN - 2;
    while (pos > line_start) {
        if (*pos == ';' || *pos == ':' || *pos == ' ') {
            return pos + 1;
        }
        pos--;
    }
    /* Now try to split on a UTF-8 boundary defined as a 7-bit
       value or as a byte with the two high-most bits set:
       11xxxxxx.  See https://czyborra.com/utf/ */

    pos = line_start + MAX_LINE_LEN - 1;
    while (pos > line_start) {
        /* plain ascii */
        if ((*pos & 128) == 0)
            return pos;

        /* utf8 escape byte */
        if ((*pos & 192) == 192)
            return pos;

        pos--;
    }

    /* Give up, just break at 74 chars (the 75th char is the space at
       the start of the line).  */

    return line_start + MAX_LINE_LEN - 1;
}

/** This splits the property into lines less than 75 octects long (as
 *  specified in RFC5545). It tries to split after a ';' if it can.
 *  It returns a tmp buffer.  NOTE: I'm not sure if it matters if we
 *  split a line in the middle of a UTF-8 character. It probably won't
 *  look nice in a text editor.
 */
static char *fold_property_line(char *text)
{
    size_t buf_size, len, chars_left;
    char *buf, *buf_ptr, *line_start, *next_line_start;
    int first_line;
    char ch;

    /* Start with a buffer twice the size of our property line, so we almost
       certainly won't overflow it. */
    len = strlen(text);
    buf_size = len * 2;
    buf = icalmemory_new_buffer(buf_size);
    buf_ptr = buf;

    /* Step through the text, finding each line to add to the output. */
    line_start = text;
    chars_left = len;
    first_line = 1;
    for (;;) {
        if (chars_left <= 0)
            break;

        /* This returns the first character for the next line. */
        next_line_start = get_next_line_start(line_start, chars_left);

        /* If this isn't the first line, we need to output a newline and space
           first. */
        if (!first_line) {
            icalmemory_append_string(&buf, &buf_ptr, &buf_size, "\r\n ");
        }
        first_line = 0;

        /* This adds the line to our tmp buffer. We temporarily place a '\0'
           in text, so we can copy the line in one go. */
        ch = *next_line_start;
        *next_line_start = '\0';
        icalmemory_append_string(&buf, &buf_ptr, &buf_size, line_start);
        *next_line_start = ch;

        /* Now we move on to the next line. */
        chars_left -= (next_line_start - line_start);
        line_start = next_line_start;
    }

    return buf;
}

/* Determine what VALUE parameter to include. The VALUE parameters
   are ignored in the normal parameter printing ( the block after
   this one, so we need to do it here */
static const char *icalproperty_get_value_kind(icalproperty *prop)
{
    const char *kind_string = NULL;
    icalvalue_kind kind = ICAL_NO_VALUE;
    icalparameter *val_param =
        icalproperty_get_first_parameter(prop, ICAL_VALUE_PARAMETER);

    if (val_param) {
        kind = icalparameter_value_to_value_kind(icalparameter_get_value(val_param));
    }

    if (kind == ICAL_NO_VALUE) {
        icalvalue *value = icalproperty_get_value(prop);

        kind = icalvalue_isa(value);

        if (kind == ICAL_ATTACH_VALUE) {
            icalattach *a = icalvalue_get_attach(value);

            kind = icalattach_get_is_url(a) ? ICAL_URI_VALUE : ICAL_BINARY_VALUE;
        }
    }

    if (kind != ICAL_NO_VALUE &&
        !icalproperty_value_kind_is_default(prop->kind, kind)) {
        /* Not the default, so it must be specified */
        kind_string = icalvalue_kind_to_string(kind);
    } else {
        /* Don't include the VALUE parameter at all */
    }

    return kind_string;
}

const char *icalproperty_as_ical_string(icalproperty *prop)
{
    char *buf;

    buf = icalproperty_as_ical_string_r(prop);
    icalmemory_add_tmp_buffer(buf);
    return buf;
}

char *icalproperty_as_ical_string_r(icalproperty *prop)
{
    icalparameter *param;

    /* Create new buffer that we can append names, parameters and a
     * value to, and reallocate as needed.
     */

    const char *property_name = 0;
    size_t buf_size = 1024;
    char *buf;
    char *buf_ptr;
    icalvalue *value;
    char *out_buf;
    const char *kind_string = 0;
    const char newline[] = "\r\n";

    icalerror_check_arg_rz((prop != 0), "prop");

    buf = icalmemory_new_buffer(buf_size);
    buf_ptr = buf;

    /* Append property name */

    if (prop->kind == ICAL_X_PROPERTY && prop->x_name != 0) {
        property_name = prop->x_name;
    } else {
        property_name = icalproperty_kind_to_string(prop->kind);
    }

    if (property_name == 0) {
        icalerror_warn("Got a property of an unknown kind.");
        icalmemory_free_buffer(buf);
        return 0;
    }

    icalmemory_append_string(&buf, &buf_ptr, &buf_size, property_name);

    kind_string = icalproperty_get_value_kind(prop);
    if (kind_string != 0) {
        icalmemory_append_string(&buf, &buf_ptr, &buf_size, ";VALUE=");
        icalmemory_append_string(&buf, &buf_ptr, &buf_size, kind_string);
    }

    /* Append parameters */
    for (param = icalproperty_get_first_parameter(prop, ICAL_ANY_PARAMETER);
         param != 0; param = icalproperty_get_next_parameter(prop, ICAL_ANY_PARAMETER)) {

        icalparameter_kind kind = icalparameter_isa(param);

        kind_string = icalparameter_as_ical_string_r(param);

        if (kind_string == 0) {
            icalerror_warn("Got a parameter of unknown kind for the following property");

            icalerror_warn((property_name) ? property_name : "(NULL)");
            continue;
        }

        if (kind == ICAL_VALUE_PARAMETER) {
            free((char *)kind_string);
            continue;
        }

        icalmemory_append_string(&buf, &buf_ptr, &buf_size, ";");
        icalmemory_append_string(&buf, &buf_ptr, &buf_size, kind_string);
        free((char *)kind_string);
    }

    /* Append value */

    icalmemory_append_string(&buf, &buf_ptr, &buf_size, ":");

    value = icalproperty_get_value(prop);

    if (value != 0) {
        char *str = icalvalue_as_ical_string_r(value);

        if (str != 0) {
            icalmemory_append_string(&buf, &buf_ptr, &buf_size, str);
#if ICAL_ALLOW_EMPTY_PROPERTIES == 0
        } else {
            icalmemory_append_string(&buf, &buf_ptr, &buf_size, "ERROR: No Value");
#endif
        }
        free(str);
    } else {
#if ICAL_ALLOW_EMPTY_PROPERTIES == 0
        icalmemory_append_string(&buf, &buf_ptr, &buf_size, "ERROR: No Value");
#endif
    }

    icalmemory_append_string(&buf, &buf_ptr, &buf_size, newline);

    /* We now use a function to fold the line properly every 75 characters.
       That function also adds the newline for us. */
    out_buf = fold_property_line(buf);

    icalmemory_free_buffer(buf);

    return out_buf;
}

icalproperty_kind icalproperty_isa(icalproperty *p)
{
    if (p != 0) {
        return p->kind;
    }

    return ICAL_NO_PROPERTY;
}

int icalproperty_isa_property(void *property)
{
    icalproperty *impl = (icalproperty *) property;

    icalerror_check_arg_rz((property != 0), "property");
    if (strcmp(impl->id, "prop") == 0) {
        return 1;
    } else {
        return 0;
    }
}

void icalproperty_add_parameter(icalproperty *p, icalparameter *parameter)
{
    icalerror_check_arg_rv((p != 0), "prop");
    icalerror_check_arg_rv((parameter != 0), "parameter");

    pvl_push(p->parameters, parameter);
}

void icalproperty_set_parameter(icalproperty *prop, icalparameter *parameter)
{
    icalparameter_kind kind;

    icalerror_check_arg_rv((prop != 0), "prop");
    icalerror_check_arg_rv((parameter != 0), "parameter");

    kind = icalparameter_isa(parameter);
    if (kind == ICAL_X_PARAMETER) {
        icalproperty_remove_parameter_by_name(prop, icalparameter_get_xname(parameter));
    } else if (kind == ICAL_IANA_PARAMETER) {
        icalproperty_remove_parameter_by_name(prop, icalparameter_get_iana_name(parameter));
    } else {
        icalproperty_remove_parameter_by_kind(prop, kind);
    }

    icalproperty_add_parameter(prop, parameter);
}

void icalproperty_set_parameter_from_string(icalproperty *prop,
                                            const char *name, const char *value)
{
    icalparameter_kind kind;
    icalparameter *param;

    icalerror_check_arg_rv((prop != 0), "prop");
    icalerror_check_arg_rv((name != 0), "name");
    icalerror_check_arg_rv((value != 0), "value");

    kind = icalparameter_string_to_kind(name);

    if (kind == ICAL_NO_PARAMETER) {
        icalerror_set_errno(ICAL_BADARG_ERROR);
        return;
    }

    param = icalparameter_new_from_value_string(kind, value);

    if (param == 0) {
        icalerror_set_errno(ICAL_BADARG_ERROR);
        return;
    }

    if (kind == ICAL_X_PARAMETER) {
        icalparameter_set_xname(param, name);
    } else if (kind == ICAL_IANA_PARAMETER) {
        icalparameter_set_iana_name(param, name);
    }

    icalproperty_set_parameter(prop, param);
}

const char *icalproperty_get_parameter_as_string(icalproperty *prop, const char *name)
{
    char *buf;

    buf = icalproperty_get_parameter_as_string_r(prop, name);
    icalmemory_add_tmp_buffer(buf);
    return buf;
}

char *icalproperty_get_parameter_as_string_r(icalproperty *prop, const char *name)
{
    icalparameter_kind kind;
    icalparameter *param;
    char *str;
    char *pv, *t;
    char *pvql;
    char *pvqr;

    icalerror_check_arg_rz((prop != 0), "prop");
    icalerror_check_arg_rz((name != 0), "name");

    kind = icalparameter_string_to_kind(name);

    if (kind == ICAL_NO_PARAMETER) {
        /* icalenum_string_to_parameter_kind will set icalerrno */
        return 0;
    }

    for (param = icalproperty_get_first_parameter(prop, kind);
         param != 0; param = icalproperty_get_next_parameter(prop, kind)) {

        if (kind == ICAL_X_PARAMETER) {
            if (strcmp(icalparameter_get_xname(param), name) == 0) {
                break;
            }
        } else if (kind == ICAL_IANA_PARAMETER) {
            if (strcmp(icalparameter_get_iana_name(param), name) == 0) {
                break;
            }
        } else {
            break;
        }
    }

    if (param == 0) {
        return 0;
    }

    str = icalparameter_as_ical_string_r(param);

    t = strchr(str, '=');

    if (t == 0) {
        icalerror_set_errno(ICAL_INTERNAL_ERROR);
        free(str);
        return 0;
    }

    /* Strip the property name and the equal sign */
    pv = icalmemory_strdup(t + 1);
    free(str);

    /* Is the string quoted? */
    pvql = strchr(pv, '"');
    if (pvql == 0) {
        return (pv);    /* No quotes?  Return it immediately. */
    }

    /* Strip everything up to the first quote */
    str = icalmemory_strdup(pvql + 1);
    free(pv);

    /* Search for the end quote */
    pvqr = strrchr(str, '"');
    if (pvqr == 0) {
        icalerror_set_errno(ICAL_INTERNAL_ERROR);
        free(str);
        return 0;
    }

    *pvqr = '\0';
    return str;
}

void icalproperty_remove_parameter_by_kind(icalproperty *prop, icalparameter_kind kind)
{
    pvl_elem p;

    icalerror_check_arg_rv((prop != 0), "prop");

    for (p = pvl_head(prop->parameters); p != 0; p = pvl_next(p)) {
        icalparameter *param = (icalparameter *) pvl_data(p);

        if (icalparameter_isa(param) == kind) {
            (void)pvl_remove(prop->parameters, p);
            icalparameter_free(param);
            break;
        }
    }
}

void icalproperty_remove_parameter_by_name(icalproperty *prop, const char *name)
{
    pvl_elem p;

    icalerror_check_arg_rv((prop != 0), "prop");

    for (p = pvl_head(prop->parameters); p != 0; p = pvl_next(p)) {
        icalparameter *param = (icalparameter *) pvl_data(p);
        const char *kind_string;

        if (icalparameter_isa(param) == ICAL_X_PARAMETER) {
            kind_string = icalparameter_get_xname(param);
        } else if (icalparameter_isa(param) == ICAL_IANA_PARAMETER) {
            kind_string = icalparameter_get_iana_name(param);
        } else {
            kind_string = icalparameter_kind_to_string(icalparameter_isa(param));
        }

        if (!kind_string)
            continue;

        if (0 == strcmp(kind_string, name)) {
            (void)pvl_remove(prop->parameters, p);
            icalparameter_free(param);
            break;
        }
    }
}

void icalproperty_remove_parameter_by_ref(icalproperty *prop, icalparameter *parameter)
{
    pvl_elem p;

    icalerror_check_arg_rv((prop != 0), "prop");
    icalerror_check_arg_rv((parameter != 0), "parameter");

    for (p = pvl_head(prop->parameters); p != 0; p = pvl_next(p)) {
        icalparameter *p_param = (icalparameter *) pvl_data(p);

        if (icalparameter_has_same_name(parameter, p_param)) {
            (void)pvl_remove(prop->parameters, p);
            icalparameter_free(p_param);
            break;
        }
    }
}

int icalproperty_count_parameters(const icalproperty *prop)
{
    if (prop != 0) {
        return pvl_count(prop->parameters);
    }

    icalerror_set_errno(ICAL_USAGE_ERROR);
    return -1;
}

icalparameter *icalproperty_get_first_parameter(icalproperty *p, icalparameter_kind kind)
{
    icalerror_check_arg_rz((p != 0), "prop");

    p->parameter_iterator = pvl_head(p->parameters);

    if (p->parameter_iterator == 0) {
        return 0;
    }

    for (p->parameter_iterator = pvl_head(p->parameters);
         p->parameter_iterator != 0; p->parameter_iterator = pvl_next(p->parameter_iterator)) {

        icalparameter *param = (icalparameter *) pvl_data(p->parameter_iterator);

        if (icalparameter_isa(param) == kind || kind == ICAL_ANY_PARAMETER) {
            return param;
        }
    }

    return 0;
}

icalparameter *icalproperty_get_next_parameter(icalproperty *p, icalparameter_kind kind)
{
    icalerror_check_arg_rz((p != 0), "prop");

    if (p->parameter_iterator == 0) {
        return 0;
    }

    for (p->parameter_iterator = pvl_next(p->parameter_iterator);
         p->parameter_iterator != 0; p->parameter_iterator = pvl_next(p->parameter_iterator)) {

        icalparameter *param = (icalparameter *) pvl_data(p->parameter_iterator);

        if (icalparameter_isa(param) == kind || kind == ICAL_ANY_PARAMETER) {
            return param;
        }
    }

    return 0;
}

void icalproperty_set_value(icalproperty *p, icalvalue *value)
{
    icalvalue_kind kind;

    icalerror_check_arg_rv((p != 0), "prop");
    icalerror_check_arg_rv((value != 0), "value");

    if (p->value != 0) {
        icalvalue_set_parent(p->value, 0);
        icalvalue_free(p->value);
        p->value = 0;
    }

    p->value = value;

    icalvalue_set_parent(value, p);

    kind = icalvalue_isa(value);
    if(kind == ICAL_DATE_VALUE || kind == ICAL_DATETIME_VALUE) {
        icalparameter *val_param;

        val_param = icalproperty_get_first_parameter(p, ICAL_VALUE_PARAMETER);

        if (val_param &&
            icalparameter_value_to_value_kind(icalparameter_get_value(val_param)) != kind) {
            icalproperty_remove_parameter_by_kind(p, ICAL_VALUE_PARAMETER);
        }
    }
}

void icalproperty_set_value_from_string(icalproperty *prop, const char *str, const char *type)
{
    icalvalue *oval, *nval;
    icalvalue_kind kind = ICAL_NO_VALUE;

    icalerror_check_arg_rv((prop != 0), "prop");
    icalerror_check_arg_rv((str != 0), "str");
    icalerror_check_arg_rv((type != 0), "type");

    if (strcmp(type, "NO") == 0) {
        /* Get the type from the value the property already has, if it exists */
        oval = icalproperty_get_value(prop);
        if (oval != 0) {
            /* Use the existing value kind */
            kind = icalvalue_isa(oval);
        } else {
            /* Use the default kind for the property */
            kind = icalproperty_kind_to_value_kind(icalproperty_isa(prop));
        }
    } else {
        /* Use the given kind string */
        kind = icalvalue_string_to_kind(type);
    }

    if (kind == ICAL_NO_VALUE) {
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        return;
    }

    icalerror_clear_errno();
    nval = icalvalue_new_from_string(kind, str);

    if (nval == 0) {
        /* icalvalue_new_from_string sets errno */
        assert(icalerrno != ICAL_NO_ERROR);
        return;
    }

    icalproperty_set_value(prop, nval);
}

icalvalue *icalproperty_get_value(const icalproperty *prop)
{
    icalerror_check_arg_rz((prop != 0), "prop");

    return prop->value;
}

const char *icalproperty_get_value_as_string(const icalproperty *prop)
{
    char *buf;

    buf = icalproperty_get_value_as_string_r(prop);
    icalmemory_add_tmp_buffer(buf);
    return buf;
}

char *icalproperty_get_value_as_string_r(const icalproperty *prop)
{
    icalvalue *value;

    icalerror_check_arg_rz((prop != 0), "prop");

    value = prop->value;

    return icalvalue_as_ical_string_r(value);
}

void icalproperty_set_x_name(icalproperty *prop, const char *name)
{
    icalerror_check_arg_rv((name != 0), "name");
    icalerror_check_arg_rv((prop != 0), "prop");

    if (prop->x_name != 0) {
        free(prop->x_name);
    }

    prop->x_name = icalmemory_strdup(name);

    if (prop->x_name == 0) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
    }
}

const char *icalproperty_get_x_name(icalproperty *prop)
{
    icalerror_check_arg_rz((prop != 0), "prop");

    return prop->x_name;
}

const char *icalproperty_get_property_name(const icalproperty *prop)
{
    char *buf;

    buf = icalproperty_get_property_name_r(prop);
    icalmemory_add_tmp_buffer(buf);
    return buf;
}

char *icalproperty_get_property_name_r(const icalproperty *prop)
{
    const char *property_name = 0;
    size_t buf_size = 256;
    char *buf;
    char *buf_ptr;

    icalerror_check_arg_rz((prop != 0), "prop");

    buf = icalmemory_new_buffer(buf_size);
    buf_ptr = buf;

    if (prop->kind == ICAL_X_PROPERTY && prop->x_name != 0) {
        property_name = prop->x_name;
    } else {
        property_name = icalproperty_kind_to_string(prop->kind);
    }

    if (property_name == 0) {
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        icalmemory_free_buffer(buf);
        return 0;

    } else {
        /* _append_string will automatically grow the buffer if
           property_name is longer than the initial buffer size */
        icalmemory_append_string(&buf, &buf_ptr, &buf_size, property_name);
    }

    return buf;
}

void icalproperty_set_parent(icalproperty *property, icalcomponent *component)
{
    icalerror_check_arg_rv((property != 0), "property");

    property->parent = component;
}

icalcomponent *icalproperty_get_parent(const icalproperty *property)
{
    icalerror_check_arg_rz((property != 0), "property");

    return property->parent;
}

static int param_compare(void *a, void *b)
{
    /* XXX  Need to sort values for multi-valued parameters (e.g. MEMBER) */
    return strcmp(icalparameter_as_ical_string((icalparameter *) a),
                  icalparameter_as_ical_string((icalparameter *) b));
}

void icalproperty_normalize(icalproperty *prop)
{
    icalproperty_kind prop_kind = icalproperty_isa(prop);
    pvl_list sorted_params = pvl_newlist();
    icalparameter *param;

    /* Normalize parameters into sorted list */
    while ((param = pvl_pop(prop->parameters)) != 0) {
        int remove = 0;

        /* Remove parameters having default values */
        switch (icalparameter_isa(param)) {
        case ICAL_VALUE_PARAMETER:
            /* Skip VALUE parameters for default property value types */
            switch (prop_kind) {
            case ICAL_ATTACH_PROPERTY:
                if (icalparameter_get_value(param) == ICAL_VALUE_URI) {
                    remove = 1;
                }
                break;

            case ICAL_DTEND_PROPERTY:
            case ICAL_XDTEND_PROPERTY:
            case ICAL_DUE_PROPERTY:
            case ICAL_DTSTART_PROPERTY:
            case ICAL_XDTSTART_PROPERTY:
            case ICAL_EXDATE_PROPERTY:
            case ICAL_RDATE_PROPERTY:
            case ICAL_RECURRENCEID_PROPERTY:
                if (icalparameter_get_value(param) == ICAL_VALUE_DATETIME) {
                    remove = 1;
                }
                break;

            case ICAL_DURATION_PROPERTY:
            case ICAL_XDURATION_PROPERTY:
                if (icalparameter_get_value(param) == ICAL_VALUE_DURATION) {
                    remove = 1;
                }
                break;

            default:
                break;
            }
            break;

        case ICAL_CUTYPE_PARAMETER:
            if (icalparameter_get_cutype(param) == ICAL_CUTYPE_INDIVIDUAL) {
                remove = 1;
            }
            break;

        case ICAL_ENCODING_PARAMETER:
            if (icalparameter_get_encoding(param) == ICAL_ENCODING_8BIT) {
                remove = 1;
            }
            break;

        case ICAL_FBTYPE_PARAMETER:
            if (icalparameter_get_fbtype(param) == ICAL_FBTYPE_BUSY) {
                remove = 1;
            }
            break;

        case ICAL_PARTSTAT_PARAMETER:
            if (icalparameter_get_partstat(param) == ICAL_PARTSTAT_NEEDSACTION) {
                remove = 1;
            }
            break;

        case ICAL_RELATED_PARAMETER:
            if (icalparameter_get_related(param) == ICAL_RELATED_START) {
                remove = 1;
            }
            break;

        case ICAL_RELTYPE_PARAMETER:
            if (icalparameter_get_reltype(param) == ICAL_RELTYPE_PARENT) {
                remove = 1;
            }
            break;

        case ICAL_ROLE_PARAMETER:
            if (icalparameter_get_role(param) == ICAL_ROLE_REQPARTICIPANT) {
                remove = 1;
            }
            break;

        case ICAL_RSVP_PARAMETER:
            if (icalparameter_get_rsvp(param) == ICAL_RSVP_FALSE) {
                remove = 1;
            }
            break;

        case ICAL_SCHEDULEAGENT_PARAMETER:
            if (icalparameter_get_scheduleagent(param) == ICAL_SCHEDULEAGENT_SERVER) {
                remove = 1;
            }
            break;

        default:
            break;
        }

        if (remove) {
            icalparameter_set_parent(param, 0); // MUST NOT have a parent to free
            icalparameter_free(param);
        } else {
            pvl_insert_ordered(sorted_params, param_compare, param);
        }
    }

    pvl_free(prop->parameters);
    prop->parameters = sorted_params;
}

/**     @brief Gets a DATE or DATE-TIME property as an icaltime
 *
 *      If the property is a DATE-TIME with a TZID parameter and a
 *      corresponding VTIMEZONE is present in the component, the
 *      returned component will already be in the correct timezone;
 *      otherwise the caller is responsible for converting it.
 *
 *      The @a comp can be NULL, in which case the parent of the @a prop
 *      is used to find the corresponding time zone.
 */
struct icaltimetype icalproperty_get_datetime_with_component(icalproperty *prop,
                                                             icalcomponent *comp)
{
    icalcomponent *c;
    icalparameter *param;
    struct icaltimetype ret;

    ret = icalvalue_get_datetime(icalproperty_get_value(prop));

    if (icaltime_is_utc(ret))
        return ret;

    if ((param = icalproperty_get_first_parameter(prop, ICAL_TZID_PARAMETER)) != NULL) {
        const char *tzid = icalparameter_get_tzid(param);
        icaltimezone *tz = NULL;

        if (!comp)
            comp = icalproperty_get_parent (prop);

        for (c = comp; c != NULL; c = icalcomponent_get_parent(c)) {
            tz = icalcomponent_get_timezone(c, tzid);
            if (tz != NULL)
                break;
        }

        if (tz == NULL)
            tz = icaltimezone_get_builtin_timezone_from_tzid(tzid);

        if (tz == NULL)
            tz = icaltimezone_get_builtin_timezone(tzid);

        if (tz != NULL)
            ret = icaltime_set_timezone(&ret, tz);
    }

    return ret;
}
