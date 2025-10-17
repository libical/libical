/*======================================================================
 FILE: vcardproperty.c
 CREATOR: Ken Murchison 24 Aug 2022

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 ======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "vcardproperty_p.h"
#include "vcardcomponent.h"
#include "vcardparser.h"
#include "vcardvalue.h"
#include "icalerror.h"
#include "icalmemory.h"
#include "icalproperty.h"
#include "icalpvl.h"

#include <stdlib.h>

struct vcardproperty_impl {
    char id[5];
    vcardproperty_kind kind;
    char *x_name;
    char *group;
    icalpvl_list parameters;
    icalpvl_elem parameter_iterator;
    vcardvalue *value;
    vcardcomponent *parent;
};

void vcardproperty_add_parameters(vcardproperty *prop, va_list args)
{
    void *vp;

    while ((vp = va_arg(args, void *)) != 0) {
        if (vcardvalue_isa_value(vp) != 0) {
        } else if (vcardparameter_isa_parameter(vp) != 0) {
            vcardproperty_add_parameter((vcardproperty *)prop,
                                        (vcardparameter *)vp);
        } else {
            icalerror_set_errno(ICAL_BADARG_ERROR);
        }
    }
}

vcardproperty *vcardproperty_new_impl(vcardproperty_kind kind)
{
    vcardproperty *prop;

    if (!vcardproperty_kind_is_valid(kind)) {
        return NULL;
    }

    if ((prop = (vcardproperty *)icalmemory_new_buffer(sizeof(vcardproperty))) == 0) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return 0;
    }

    memset(prop, 0, sizeof(vcardproperty));

    strcpy(prop->id, "prop");

    prop->kind = kind;
    prop->parameters = icalpvl_newlist();

    return prop;
}

vcardproperty *vcardproperty_new(vcardproperty_kind kind)
{
    if (kind == VCARD_NO_PROPERTY) {
        return 0;
    }

    return (vcardproperty *)vcardproperty_new_impl(kind);
}

vcardproperty *vcardproperty_clone(const vcardproperty *old)
{
    vcardproperty *clone;
    icalpvl_elem p;

    icalerror_check_arg_rz((old != 0), "old");
    clone = vcardproperty_new_impl(old->kind);
    icalerror_check_arg_rz((clone != 0), "clone");

    if (old->value != 0) {
        clone->value = vcardvalue_clone(old->value);
    }

    if (old->x_name != 0) {
        clone->x_name = icalmemory_strdup(old->x_name);

        if (clone->x_name == 0) {
            vcardproperty_free(clone);
            icalerror_set_errno(ICAL_NEWFAILED_ERROR);
            return 0;
        }
    }

    for (p = icalpvl_head(old->parameters); p != 0; p = icalpvl_next(p)) {
        vcardparameter *param = vcardparameter_clone(icalpvl_data(p));

        if (param == 0) {
            vcardproperty_free(clone);
            icalerror_set_errno(ICAL_NEWFAILED_ERROR);
            return 0;
        }

        icalpvl_push(clone->parameters, param);
    }

    return clone;
}

vcardproperty *vcardproperty_new_from_string(const char *str)
{
    size_t buf_size = 1024;
    char *buf;
    char *buf_ptr;
    vcardproperty *prop;
    vcardcomponent *comp;
    int errors = 0;

    icalerror_check_arg_rz((str != 0), "str");

    buf = icalmemory_new_buffer(buf_size);
    buf_ptr = buf;

    /* Is this a HACK or a crafty reuse of code? */

    icalmemory_append_string(&buf, &buf_ptr, &buf_size, "BEGIN:VCARD\r\n");
    icalmemory_append_string(&buf, &buf_ptr, &buf_size, str);
    icalmemory_append_string(&buf, &buf_ptr, &buf_size, "\r\n");
    icalmemory_append_string(&buf, &buf_ptr, &buf_size, "END:VCARD\r\n");

    comp = vcardparser_parse_string(buf);

    if (comp == 0) {
        icalerror_set_errno(ICAL_PARSE_ERROR);
        icalmemory_free_buffer(buf);
        return 0;
    }

    errors = vcardcomponent_count_errors(comp);

    prop = vcardcomponent_get_first_property(comp, VCARD_ANY_PROPERTY);

    vcardcomponent_remove_property(comp, prop);

    vcardcomponent_free(comp);
    icalmemory_free_buffer(buf);

    if (errors > 0) {
        vcardproperty_free(prop);
        return 0;
    } else {
        return prop;
    }
}

void vcardproperty_free(vcardproperty *p)
{
    vcardparameter *param;

    icalerror_check_arg_rv((p != 0), "prop");

    if (p->parent != 0) {
        return;
    }

    if (p->value != 0) {
        vcardvalue_set_parent(p->value, 0);
        vcardvalue_free(p->value);
    }

    while ((param = icalpvl_pop(p->parameters)) != 0) {
        vcardparameter_free(param);
    }

    icalpvl_free(p->parameters);

    if (p->x_name != 0) {
        icalmemory_free_buffer(p->x_name);
    }

    if (p->group != 0) {
        icalmemory_free_buffer(p->group);
    }

    p->kind = VCARD_NO_PROPERTY;
    p->parameters = 0;
    p->parameter_iterator = 0;
    p->value = 0;
    p->x_name = 0;
    p->id[0] = 'X';

    icalmemory_free_buffer(p);
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
        if ((*pos & 128) == 0) {
            return pos;
        }

        /* utf8 escape byte */
        if ((*pos & 192) == 192) {
            return pos;
        }

        pos--;
    }

    /* Give up, just break at 74 chars (the 75th char is the space at
       the start of the line).  */

    return line_start + MAX_LINE_LEN - 1;
}

/** This splits the property into lines less than 75 octets long (as
 *  specified in RFC6350). It tries to split after a ';' if it can.
 *  It returns a tmp buffer.  NOTE: I'm not sure if it matters if we
 *  split a line in the middle of a UTF-8 character. It probably won't
 *  look nice in a text editor.
 */
static char *fold_property_line(char *text)
{
    size_t buf_size, len;
    ssize_t chars_left;
    char *buf, *buf_ptr, *line_start, *next_line_start;
    int first_line;

    /* Start with a buffer twice the size of our property line, so we almost
       certainly won't overflow it. */
    len = strlen(text);
    buf_size = len * 2;
    buf = icalmemory_new_buffer(buf_size);
    buf_ptr = buf;

    /* Step through the text, finding each line to add to the output. */
    line_start = text;
    chars_left = (ssize_t)len;
    first_line = 1;
    for (;;) {
        if (chars_left <= 0) {
            break;
        }

        /* This returns the first character for the next line. */
        next_line_start = get_next_line_start(line_start, (size_t)chars_left);

        /* If this isn't the first line, we need to output a newline and space
           first. */
        if (!first_line) {
            icalmemory_append_string(&buf, &buf_ptr, &buf_size, "\r\n ");
        }
        first_line = 0;

        /* This adds the line to our tmp buffer. We temporarily place a '\0'
           in text, so we can copy the line in one go. */
        char ch = *next_line_start;
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
static const char *vcardproperty_get_value_kind(vcardproperty *prop,
                                                const vcardvalue *value)
{
    const char *kind_string = NULL;
    if (!prop) {
        return kind_string;
    }
    vcardvalue_kind kind = VCARD_NO_VALUE;
    vcardparameter *val_param =
        vcardproperty_get_first_parameter(prop, VCARD_VALUE_PARAMETER);
    vcardproperty_version version = vcardcomponent_get_version(prop->parent);

    if (version == VCARD_VERSION_NONE) {
        version = VCARD_VERSION_30;
    }

    if (val_param) {
        kind = vcardparameter_value_to_value_kind(vcardparameter_get_value(val_param));
    }

    if (kind == VCARD_NO_VALUE && vcardproperty_isa(prop) != VCARD_X_PROPERTY) {
        kind = vcardvalue_isa(value);
    }

    /* Try to omit VERSION-dependent default VALUE kinds */
    switch (vcardproperty_isa(prop)) {
    case VCARD_GEO_PROPERTY:
        /* v3 default: VALUE=STRUCTURED
         * v4 default: VALUE=URL
         */
        if (kind != VCARD_X_VALUE) {
            kind = VCARD_NO_VALUE;
        }
        break;

    case VCARD_KEY_PROPERTY:
    case VCARD_LOGO_PROPERTY:
    case VCARD_PHOTO_PROPERTY:
    case VCARD_SOUND_PROPERTY:
        /* v3 default: VALUE=BINARY (ENCODING=b parameter)
         * v4 default: VALUE=URL
         */
        if (version == VCARD_VERSION_40 ||
            vcardproperty_get_first_parameter(prop, VCARD_ENCODING_PARAMETER)) {
            kind = VCARD_NO_VALUE;
        }
        break;

    case VCARD_ANNIVERSARY_PROPERTY:
    case VCARD_BDAY_PROPERTY:
    case VCARD_DEATHDATE_PROPERTY:
        /* TIME is v4 specific.
         * All other types are self-evident.
         */
        if (version == VCARD_VERSION_40 &&
            (kind == VCARD_TIME_VALUE || kind == VCARD_DATEANDORTIME_VALUE)) {
            kind = VCARD_NO_VALUE;
        } else if (kind == VCARD_DATE_VALUE || kind == VCARD_DATETIME_VALUE) {
            kind = VCARD_NO_VALUE;
        }
        break;

    case VCARD_TZ_PROPERTY:
        /* v3 default: VALUE=UTCOFFSET
         * v4 default: VALUE=TEXT
         */
        if (version == VCARD_VERSION_40) {
            if (kind == VCARD_TEXT_VALUE) {
                kind = VCARD_NO_VALUE;
            }
        } else if (kind == VCARD_UTCOFFSET_VALUE) {
            kind = VCARD_NO_VALUE;
        }
        break;

    case VCARD_UID_PROPERTY:
        /* v3 default: VALUE=TEXT
         * v4 default: VALUE=URI
         */
        if (version == VCARD_VERSION_40) {
            if (kind == VCARD_URI_VALUE) {
                kind = VCARD_NO_VALUE;
            }
        } else {
            kind = VCARD_NO_VALUE;
        }
        break;

    default:
        break;
    }

    if (kind != VCARD_NO_VALUE &&
        !vcardproperty_value_kind_is_default(prop->kind, kind)) {
        /* Not the default, so it must be specified */
        kind_string = vcardvalue_kind_to_string(kind);
    } else {
        /* Don't include the VALUE parameter at all */
    }

    return kind_string;
}

const char *vcardproperty_as_vcard_string(vcardproperty *prop)
{
    char *buf;

    buf = vcardproperty_as_vcard_string_r(prop);
    icalmemory_add_tmp_buffer(buf);
    return buf;
}

char *vcardproperty_as_vcard_string_r(vcardproperty *prop)
{
    vcardparameter *param;

    /* Create new buffer that we can append names, parameters and a
     * value to, and reallocate as needed.
     */

    const char *property_name = 0;
    size_t buf_size = 1024;
    char *buf;
    char *buf_ptr;
    const vcardvalue *value;
    char *out_buf;
    const char *kind_string = 0;
    const char newline[] = "\r\n";

    icalerror_check_arg_rz((prop != 0), "prop");

    buf = icalmemory_new_buffer(buf_size);
    buf_ptr = buf;

    /* Append property name */

    if (prop->kind == VCARD_X_PROPERTY && prop->x_name != 0) {
        property_name = prop->x_name;
    } else {
        property_name = vcardproperty_kind_to_string(prop->kind);
    }

    if (property_name == 0) {
        icalerror_warn("Got a property of an unknown kind.");
        icalmemory_free_buffer(buf);
        return 0;
    }

    if (prop->group != 0) {
        icalmemory_append_string(&buf, &buf_ptr, &buf_size, prop->group);
        icalmemory_append_string(&buf, &buf_ptr, &buf_size, ".");
    }

    icalmemory_append_string(&buf, &buf_ptr, &buf_size, property_name);

    value = vcardproperty_get_value(prop);
    kind_string = vcardproperty_get_value_kind(prop, value);
    if (kind_string != 0) {
        icalmemory_append_string(&buf, &buf_ptr, &buf_size, ";VALUE=");
        icalmemory_append_string(&buf, &buf_ptr, &buf_size, kind_string);
    }

    /* Append parameters */
    for (param = vcardproperty_get_first_parameter(prop, VCARD_ANY_PARAMETER);
         param != 0; param = vcardproperty_get_next_parameter(prop, VCARD_ANY_PARAMETER)) {
        vcardparameter_kind kind = vcardparameter_isa(param);

        kind_string = vcardparameter_as_vcard_string_r(param);

        if (kind_string == 0) {
            icalerror_warn("Got a parameter of unknown kind for the following property");

            icalerror_warn((property_name) ? property_name : "(NULL)");
            continue;
        }

        if (kind == VCARD_VALUE_PARAMETER) {
            icalmemory_free_buffer((char *)kind_string);
            continue;
        }

        icalmemory_append_string(&buf, &buf_ptr, &buf_size, ";");
        icalmemory_append_string(&buf, &buf_ptr, &buf_size, kind_string);
        icalmemory_free_buffer((char *)kind_string);
    }

    /* Append value */

    icalmemory_append_string(&buf, &buf_ptr, &buf_size, ":");

    if (value != 0) {
        char *str = vcardvalue_as_vcard_string_r(value);

        if (str != 0) {
            icalmemory_append_string(&buf, &buf_ptr, &buf_size, str);
        } else if (!icalproperty_get_allow_empty_properties()) {
            icalmemory_append_string(&buf, &buf_ptr, &buf_size, "ERROR: No Value");
        }
        icalmemory_free_buffer(str);
    } else if (!icalproperty_get_allow_empty_properties()) {
        icalmemory_append_string(&buf, &buf_ptr, &buf_size, "ERROR: No Value");
    }

    icalmemory_append_string(&buf, &buf_ptr, &buf_size, newline);

    /* We now use a function to fold the line properly every 75 characters.
       That function also adds the newline for us. */
    out_buf = fold_property_line(buf);

    icalmemory_free_buffer(buf);

    return out_buf;
}

vcardproperty_kind vcardproperty_isa(vcardproperty *p)
{
    if (p != 0) {
        return p->kind;
    }

    return VCARD_NO_PROPERTY;
}

bool vcardproperty_isa_property(void *property)
{
    const vcardproperty *impl = (vcardproperty *)property;

    icalerror_check_arg_rz((property != 0), "property");
    if (strcmp(impl->id, "prop") == 0) {
        return true;
    } else {
        return false;
    }
}

void vcardproperty_add_parameter(vcardproperty *p, vcardparameter *parameter)
{
    icalerror_check_arg_rv((p != 0), "prop");
    icalerror_check_arg_rv((parameter != 0), "parameter");

    icalpvl_push(p->parameters, parameter);
}

void vcardproperty_set_parameter(vcardproperty *prop, vcardparameter *parameter)
{
    vcardparameter_kind kind;

    icalerror_check_arg_rv((prop != 0), "prop");
    icalerror_check_arg_rv((parameter != 0), "parameter");

    kind = vcardparameter_isa(parameter);
    if (kind == VCARD_X_PARAMETER) {
        vcardproperty_remove_parameter_by_name(prop, vcardparameter_get_xname(parameter));
    } else if (kind == VCARD_IANA_PARAMETER) {
        vcardproperty_remove_parameter_by_name(prop, vcardparameter_get_iana_name(parameter));
    } else {
        vcardproperty_remove_parameter_by_kind(prop, kind);
    }

    vcardproperty_add_parameter(prop, parameter);
}

void vcardproperty_set_parameter_from_string(vcardproperty *prop,
                                             const char *name, const char *value)
{
    vcardparameter_kind kind;
    vcardparameter *param;

    icalerror_check_arg_rv((prop != 0), "prop");
    icalerror_check_arg_rv((name != 0), "name");
    icalerror_check_arg_rv((value != 0), "value");

    kind = vcardparameter_string_to_kind(name);

    if (kind == VCARD_NO_PARAMETER) {
        icalerror_set_errno(ICAL_BADARG_ERROR);
        return;
    }

    param = vcardparameter_new_from_value_string(kind, value);

    if (param == 0) {
        icalerror_set_errno(ICAL_BADARG_ERROR);
        return;
    }

    if (kind == VCARD_X_PARAMETER) {
        vcardparameter_set_xname(param, name);
    } else if (kind == VCARD_IANA_PARAMETER) {
        vcardparameter_set_iana_name(param, name);
    }

    vcardproperty_set_parameter(prop, param);
}

const char *vcardproperty_get_parameter_as_string(vcardproperty *prop, const char *name)
{
    char *buf;

    buf = vcardproperty_get_parameter_as_string_r(prop, name);
    icalmemory_add_tmp_buffer(buf);
    return buf;
}

char *vcardproperty_get_parameter_as_string_r(vcardproperty *prop, const char *name)
{
    vcardparameter_kind kind;
    vcardparameter *param;
    char *str;
    char *pv, *t;
    char *pvql;
    char *pvqr;

    icalerror_check_arg_rz((prop != 0), "prop");
    icalerror_check_arg_rz((name != 0), "name");

    kind = vcardparameter_string_to_kind(name);

    if (kind == VCARD_NO_PARAMETER) {
        /* icalenum_string_to_parameter_kind will set icalerrno */
        return 0;
    }

    for (param = vcardproperty_get_first_parameter(prop, kind);
         param != 0; param = vcardproperty_get_next_parameter(prop, kind)) {
        if (kind == VCARD_X_PARAMETER) {
            if (strcmp(vcardparameter_get_xname(param), name) == 0) {
                break;
            }
        } else if (kind == VCARD_IANA_PARAMETER) {
            if (strcmp(vcardparameter_get_iana_name(param), name) == 0) {
                break;
            }
        } else {
            break;
        }
    }

    if (param == 0) {
        return 0;
    }

    str = vcardparameter_as_vcard_string_r(param);

    t = strchr(str, '=');

    if (t == 0) {
        icalerror_set_errno(ICAL_INTERNAL_ERROR);
        icalmemory_free_buffer(str);
        return 0;
    }

    /* Strip the property name and the equal sign */
    pv = icalmemory_strdup(t + 1);
    icalmemory_free_buffer(str);

    /* Is the string quoted? */
    pvql = strchr(pv, '"');
    if (pvql == 0) {
        return (pv); /* No quotes?  Return it immediately. */
    }

    /* Strip everything up to the first quote */
    str = icalmemory_strdup(pvql + 1);
    icalmemory_free_buffer(pv);

    /* Search for the end quote */
    pvqr = strrchr(str, '"');
    if (pvqr == 0) {
        icalerror_set_errno(ICAL_INTERNAL_ERROR);
        icalmemory_free_buffer(str);
        return 0;
    }

    *pvqr = '\0';
    return str;
}

void vcardproperty_remove_parameter_by_kind(vcardproperty *prop, vcardparameter_kind kind)
{
    icalpvl_elem p;

    icalerror_check_arg_rv((prop != 0), "prop");

    for (p = icalpvl_head(prop->parameters); p != 0; p = icalpvl_next(p)) {
        vcardparameter *param = (vcardparameter *)icalpvl_data(p);

        if (vcardparameter_isa(param) == kind) {
            (void)icalpvl_remove(prop->parameters, p);
            vcardparameter_free(param);
            break;
        }
    }
}

void vcardproperty_remove_parameter_by_name(vcardproperty *prop, const char *name)
{
    icalpvl_elem p;

    icalerror_check_arg_rv((prop != 0), "prop");

    for (p = icalpvl_head(prop->parameters); p != 0; p = icalpvl_next(p)) {
        vcardparameter *param = (vcardparameter *)icalpvl_data(p);
        const char *kind_string;

        if (vcardparameter_isa(param) == VCARD_X_PARAMETER) {
            kind_string = vcardparameter_get_xname(param);
        } else if (vcardparameter_isa(param) == VCARD_IANA_PARAMETER) {
            kind_string = vcardparameter_get_iana_name(param);
        } else {
            kind_string = vcardparameter_kind_to_string(vcardparameter_isa(param));
        }

        if (!kind_string) {
            continue;
        }

        if (0 == strcmp(kind_string, name)) {
            (void)icalpvl_remove(prop->parameters, p);
            vcardparameter_free(param);
            break;
        }
    }
}

void vcardproperty_remove_parameter_by_ref(vcardproperty *prop, vcardparameter *parameter)
{
    icalpvl_elem p;

    icalerror_check_arg_rv((prop != 0), "prop");
    icalerror_check_arg_rv((parameter != 0), "parameter");

    for (p = icalpvl_head(prop->parameters); p != 0; p = icalpvl_next(p)) {
        vcardparameter *p_param = (vcardparameter *)icalpvl_data(p);

        if (vcardparameter_has_same_name(parameter, p_param)) {
            (void)icalpvl_remove(prop->parameters, p);
            vcardparameter_free(p_param);
            break;
        }
    }
}

int vcardproperty_count_parameters(const vcardproperty *prop)
{
    if (prop != 0) {
        return icalpvl_count(prop->parameters);
    }

    icalerror_set_errno(ICAL_USAGE_ERROR);
    return -1;
}

vcardparameter *vcardproperty_get_first_parameter(vcardproperty *p, vcardparameter_kind kind)
{
    icalerror_check_arg_rz((p != 0), "prop");

    p->parameter_iterator = icalpvl_head(p->parameters);

    if (p->parameter_iterator == 0) {
        return 0;
    }

    for (p->parameter_iterator = icalpvl_head(p->parameters);
         p->parameter_iterator != 0; p->parameter_iterator = icalpvl_next(p->parameter_iterator)) {
        vcardparameter *param = (vcardparameter *)icalpvl_data(p->parameter_iterator);

        if (vcardparameter_isa(param) == kind || kind == VCARD_ANY_PARAMETER) {
            return param;
        }
    }

    return 0;
}

vcardparameter *vcardproperty_get_next_parameter(vcardproperty *p, vcardparameter_kind kind)
{
    icalerror_check_arg_rz((p != 0), "prop");

    if (p->parameter_iterator == 0) {
        return 0;
    }

    for (p->parameter_iterator = icalpvl_next(p->parameter_iterator);
         p->parameter_iterator != 0; p->parameter_iterator = icalpvl_next(p->parameter_iterator)) {
        vcardparameter *param = (vcardparameter *)icalpvl_data(p->parameter_iterator);

        if (vcardparameter_isa(param) == kind || kind == VCARD_ANY_PARAMETER) {
            return param;
        }
    }

    return 0;
}

void vcardproperty_set_value(vcardproperty *p, vcardvalue *value)
{
    vcardvalue_kind kind;

    icalerror_check_arg_rv((p != 0), "prop");
    icalerror_check_arg_rv((value != 0), "value");

    if (p->value != 0) {
        vcardvalue_set_parent(p->value, 0);
        vcardvalue_free(p->value);
        p->value = 0;
    }

    p->value = value;

    vcardvalue_set_parent(value, p);

    kind = vcardvalue_isa(value);

    if (kind == VCARD_DATE_VALUE || kind == VCARD_DATETIME_VALUE) {
        vcardparameter *val_param;

        val_param = vcardproperty_get_first_parameter(p, VCARD_VALUE_PARAMETER);

        if (val_param &&
            vcardparameter_value_to_value_kind(vcardparameter_get_value(val_param)) != kind) {
            vcardproperty_remove_parameter_by_kind(p, VCARD_VALUE_PARAMETER);
        }
    }
}

void vcardproperty_set_value_from_string(vcardproperty *prop, const char *str, const char *type)
{
    vcardvalue *nval;
    vcardvalue_kind kind = VCARD_NO_VALUE;

    icalerror_check_arg_rv((prop != 0), "prop");
    icalerror_check_arg_rv((str != 0), "str");
    icalerror_check_arg_rv((type != 0), "type");

    if (strcmp(type, "NO") == 0) {
        /* Get the type from the value the property already has, if it exists */
        const vcardvalue *oval = vcardproperty_get_value(prop);
        if (oval != 0) {
            /* Use the existing value kind */
            kind = vcardvalue_isa(oval);
        } else {
            /* Use the default kind for the property */
            kind = vcardproperty_kind_to_value_kind(vcardproperty_isa(prop));
        }
    } else {
        /* Use the given kind string */
        kind = vcardvalue_string_to_kind(type);
    }

    if (kind == VCARD_NO_VALUE) {
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        return;
    }

    icalerror_clear_errno();
    nval = vcardvalue_new_from_string(kind, str);

    if (nval == 0) {
        /* vcardvalue_new_from_string sets errno */
        icalassert(icalerrno != ICAL_NO_ERROR);
        return;
    }

    vcardproperty_set_value(prop, nval);
}

vcardvalue *vcardproperty_get_value(const vcardproperty *prop)
{
    icalerror_check_arg_rz((prop != 0), "prop");

    return prop->value;
}

const char *vcardproperty_get_value_as_string(const vcardproperty *prop)
{
    char *buf;

    buf = vcardproperty_get_value_as_string_r(prop);
    icalmemory_add_tmp_buffer(buf);
    return buf;
}

char *vcardproperty_get_value_as_string_r(const vcardproperty *prop)
{
    const vcardvalue *value;

    icalerror_check_arg_rz((prop != 0), "prop");

    value = prop->value;

    return vcardvalue_as_vcard_string_r(value);
}

void vcardproperty_set_x_name(vcardproperty *prop, const char *name)
{
    icalerror_check_arg_rv((name != 0), "name");
    icalerror_check_arg_rv((prop != 0), "prop");

    if (prop->x_name != 0) {
        icalmemory_free_buffer(prop->x_name);
    }

    prop->x_name = icalmemory_strdup(name);

    if (prop->x_name == 0) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
    }
}

const char *vcardproperty_get_x_name(const vcardproperty *prop)
{
    icalerror_check_arg_rz((prop != 0), "prop");

    return prop->x_name;
}

void vcardproperty_set_group(vcardproperty *prop, const char *group)
{
    icalerror_check_arg_rv((group != 0), "group");
    icalerror_check_arg_rv((prop != 0), "prop");

    if (prop->group != 0) {
        icalmemory_free_buffer(prop->group);
    }

    prop->group = icalmemory_strdup(group);

    if (prop->group == 0) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
    }
}

const char *vcardproperty_get_group(const vcardproperty *prop)
{
    icalerror_check_arg_rz((prop != 0), "prop");

    return prop->group;
}

const char *vcardproperty_get_property_name(const vcardproperty *prop)
{
    char *buf;

    buf = vcardproperty_get_property_name_r(prop);
    icalmemory_add_tmp_buffer(buf);
    return buf;
}

char *vcardproperty_get_property_name_r(const vcardproperty *prop)
{
    const char *property_name = 0;
    size_t buf_size = 256;
    char *buf;
    char *buf_ptr;

    icalerror_check_arg_rz((prop != 0), "prop");

    buf = icalmemory_new_buffer(buf_size);
    buf_ptr = buf;

    if (prop->kind == VCARD_X_PROPERTY && prop->x_name != 0) {
        property_name = prop->x_name;
    } else {
        property_name = vcardproperty_kind_to_string(prop->kind);
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

void vcardproperty_set_parent(vcardproperty *property, vcardcomponent *comp)
{
    icalerror_check_arg_rv((property != 0), "property");

    property->parent = comp;
}

vcardcomponent *vcardproperty_get_parent(const vcardproperty *property)
{
    icalerror_check_arg_rz((property != 0), "property");

    return property->parent;
}

static int param_compare(void *a, void *b)
{
    return strcmp(vcardparameter_as_vcard_string((vcardparameter *)a),
                  vcardparameter_as_vcard_string((vcardparameter *)b));
}

void vcardproperty_normalize(vcardproperty *prop)
{
    vcardproperty_kind prop_kind = vcardproperty_isa(prop);
    icalpvl_list sorted_params = icalpvl_newlist();
    vcardparameter *param;

    /* Normalize parameters into sorted list */
    while ((param = icalpvl_pop(prop->parameters)) != 0) {
        int remove = 0;

        /* Remove parameters having default values */
        switch (vcardparameter_isa(param)) {
        case VCARD_VALUE_PARAMETER:
            /* Skip VALUE parameters for default property value types */
            switch (prop_kind) {
            case VCARD_TEL_PROPERTY:
            case VCARD_TZ_PROPERTY:
            case VCARD_BIRTHPLACE_PROPERTY:
            case VCARD_DEATHPLACE_PROPERTY:
                if (vcardparameter_get_value(param) == VCARD_VALUE_TEXT) {
                    remove = 1;
                }
                break;

            case VCARD_RELATED_PROPERTY:
            case VCARD_UID_PROPERTY:
            case VCARD_KEY_PROPERTY:
                if (vcardparameter_get_value(param) == VCARD_VALUE_URI) {
                    remove = 1;
                }
                break;

            case VCARD_BDAY_PROPERTY:
            case VCARD_ANNIVERSARY_PROPERTY:
            case VCARD_DEATHDATE_PROPERTY:
                if (vcardparameter_get_value(param) == VCARD_VALUE_DATEANDORTIME) {
                    remove = 1;
                }
                break;

            default:
                break;
            }
            break;

        case VCARD_CALSCALE_PARAMETER:
            if (vcardparameter_get_calscale(param) == VCARD_CALSCALE_GREGORIAN) {
                remove = 1;
            }
            break;

        case VCARD_PREF_PARAMETER:
            if (vcardparameter_get_pref(param) >= 100) {
                remove = 1;
            }
            break;

        case VCARD_TYPE_PARAMETER:
            if (prop_kind == VCARD_TEL_PROPERTY) {
                /* Is it just TYPE=VOICE ? */
                vcardenumarray_element voice = {VCARD_TYPE_VOICE, NULL};

                if (vcardenumarray_find(vcardparameter_get_type(param),
                                        &voice) == 0) {
                    remove = 1;
                }
            }

            if (!remove) {
                vcardenumarray_sort(vcardparameter_get_type(param));
            }
            break;

        case VCARD_PID_PARAMETER:
            vcardstrarray_sort(vcardparameter_get_pid(param));
            break;

        case VCARD_SORTAS_PARAMETER:
            vcardstrarray_sort(vcardparameter_get_sortas(param));
            break;

        default:
            break;
        }

        if (remove) {
            vcardparameter_set_parent(param, 0); // MUST NOT have a parent to free
            vcardparameter_free(param);
        } else {
            icalpvl_insert_ordered(sorted_params, param_compare, param);
        }
    }

    icalpvl_free(prop->parameters);
    prop->parameters = sorted_params;

    switch (prop_kind) {
    case VCARD_CATEGORIES_PROPERTY:
        vcardstrarray_sort(vcardproperty_get_categories(prop));
        break;

    case VCARD_NICKNAME_PROPERTY:
        vcardstrarray_sort(vcardproperty_get_nickname(prop));
        break;

    default:
        break;
    }
}

void vcardproperty_add_type_parameter(vcardproperty *prop,
                                      vcardenumarray_element *type)
{
    vcardenumarray *types;
    vcardparameter *param =
        vcardproperty_get_first_parameter(prop, VCARD_TYPE_PARAMETER);

    if (param) {
        types = vcardparameter_get_type(param);
        vcardenumarray_add(types, type);
    } else {
        param = vcardparameter_new(VCARD_TYPE_PARAMETER);
        types = vcardenumarray_new(0);
        vcardenumarray_add(types, type);
        vcardparameter_set_type(param, types);
        vcardproperty_add_parameter(prop, param);
    }
}
