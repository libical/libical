/*======================================================================
 FILE: vcardvalue.c
 CREATOR: Ken Murchison 24 Aug 2022

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 ======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "vcardvalue.h"
#include "vcardvalueimpl.h"
#include "vcardcomponent.h"
#include "vcardproperty.h"
#include "vcardtextlist.h"
#include "icalerror.h"
#include "icalmemory.h"

#include <ctype.h>
#include <locale.h>
#include <stdlib.h>

#define TMP_BUF_SIZE 1024

LIBICAL_VCARD_EXPORT struct vcardvalue_impl *vcardvalue_new_impl(vcardvalue_kind kind)
{
    struct vcardvalue_impl *v;

    if (!vcardvalue_kind_is_valid(kind)) {
        return NULL;
    }

    if ((v = (struct vcardvalue_impl *)icalmemory_new_buffer(sizeof(struct vcardvalue_impl))) == 0) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return 0;
    }

    strcpy(v->id, "val");

    v->kind = kind;
    v->size = 0;
    v->parent = 0;
    v->x_value = 0;
    memset(&(v->data), 0, sizeof(v->data));

    return v;
}

vcardvalue *vcardvalue_new(vcardvalue_kind kind)
{
    return (vcardvalue *)vcardvalue_new_impl(kind);
}

vcardvalue *vcardvalue_clone(const vcardvalue *old)
{
    struct vcardvalue_impl *clone;

    clone = vcardvalue_new_impl(old->kind);

    if (clone == 0) {
        return 0;
    }

    // id is a LIBICAL_VCARDVALUE_ID_LENGTH-char string (see vcardvalue_impl def)
    memset(clone->id, 0, LIBICAL_VCARDVALUE_ID_LENGTH);
    strncpy(clone->id, old->id, LIBICAL_VCARDVALUE_ID_LENGTH);
    clone->kind = old->kind;
    clone->size = old->size;

    switch (clone->kind) {
    case VCARD_TEXT_VALUE:
    case VCARD_URI_VALUE:
    case VCARD_LANGUAGETAG_VALUE: {
        if (old->data.v_string != 0) {
            clone->data.v_string = icalmemory_strdup(old->data.v_string);

            if (clone->data.v_string == 0) {
                clone->parent = 0;
                vcardvalue_free(clone);
                return 0;
            }
        }
        break;
    }
    case VCARD_X_VALUE: {
        if (old->x_value != 0) {
            clone->x_value = icalmemory_strdup(old->x_value);

            if (clone->x_value == 0) {
                clone->parent = 0;
                vcardvalue_free(clone);
                return 0;
            }
        }

        break;
    }

    case VCARD_STRUCTURED_VALUE:
        if (old->data.v_structured != 0) {
            clone->data.v_structured =
                vcardstructured_clone(old->data.v_structured);
        }
        break;

    default: {
        /* all of the other types are stored as values, not
               pointers, so we can just copy the whole structure. */

        clone->data = old->data;
    }
    }

    return clone;
}

char *vcardvalue_strdup_and_dequote_text(const char **str, const char *sep)
{
    const char *p;
    char *out = (char *)icalmemory_new_buffer(sizeof(char) * strlen(*str) + 1);
    char *pout;

    if (out == 0) {
        return 0;
    }

    pout = out;

    /* Stop the loop when encountering a terminator/separator in the source string
       or if a null has been written to the destination. This prevents
       reading past the end of the source string if the last character
       is a backslash. */
    for (p = *str; *p != 0; p++) {
        if (*p == '\\') {
            p++;
            switch (*p) {
            case 0: {
                p--; // step back to NUL to stop iteration
                *pout = '\0';
                break;
            }
            case 'n':
            case 'N': {
                *pout = '\n';
                break;
            }
            case 't':
            case 'T': {
                *pout = '\t';
                break;
            }
            case 'r':
            case 'R': {
                *pout = '\r';
                break;
            }
            case 'b':
            case 'B': {
                *pout = '\b';
                break;
            }
            case 'f':
            case 'F': {
                *pout = '\f';
                break;
            }
            case ';':
            case ',':
            case '"':
            case '\\': {
                *pout = *p;
                break;
            }
            default: {
                *pout = ' ';
            }
            }
        } else if (sep && strchr(sep, *p)) {
            break;
        } else {
            *pout = *p;
        }

        pout++;
    }

    *str = p;

    *pout = '\0';

    return out;
}

/*
 * Returns a quoted copy of a string
 * @todo This is not RFC5545 compliant.
 * RFC 6350 allows:
 *   TEXT-CHAR = "\\" / "\," / "\n" / WSP / NON-ASCII / %x21-2B / %x2D-5B / %x5D-7E
 * but we use the more restrictive (for ADR and N):
 *   component = "\\" / "\," / "\;" / "\n" / WSP / NON-ASCII /
                 %x21-2B / %x2D-3A / %x3C-5B / %x5D-7E
 * As such, \b, \f, \r are not allowed, not even escaped
 */
static char *vcardmemory_strdup_and_quote(char **str, char **str_p, size_t *buf_sz,
                                          const char *unquoted_str, bool is_param)
{
    const char *p;

    if (!*str) {
        *buf_sz = strlen(unquoted_str) + 1;

        *str_p = *str = (char *)icalmemory_new_buffer(*buf_sz);
    }

    /* cppcheck-suppress uninitvar */
    if (*str_p == 0) {
        return 0;
    }

    for (p = unquoted_str; *p != 0; p++) {
        switch (*p) {
        case '\b':
        case '\f':
        case '\r':
            /* ignore */
            break;

        case '\\':
        case ',':
        case ';':
            /* escape */
            icalmemory_append_char(str, str_p, buf_sz, '\\');
            icalmemory_append_char(str, str_p, buf_sz, *p);
            break;

        case '\n':
            /* If encoding a parameter value, embed literally
               (parameter encoding is done elsewhere), otherwise escape */
            icalmemory_append_string(str, str_p, buf_sz, is_param ? "\n" : "\\n");
            break;

        default:
            icalmemory_append_char(str, str_p, buf_sz, *p);
            break;
        }
    }

    /* Assume the last character is not a '\0' and add one. We could
       check *str_p != 0, but that would be an uninitialized memory
       read. */

    icalmemory_append_char(str, str_p, buf_sz, '\0');
    return *str;
}

/*
 * FIXME
 *
 * This is a bad API, as it forces callers to specify their own X type.
 * This function should take care of this by itself.
 */
static vcardvalue *vcardvalue_new_enum(vcardvalue_kind kind, int x_type, const char *str)
{
    int e = vcardproperty_kind_and_string_to_enum((int)kind, str);
    struct vcardvalue_impl *value;

    if (e != 0 && vcardproperty_enum_belongs_to_property(vcardproperty_value_kind_to_kind(kind), e)) {
        value = vcardvalue_new_impl(kind);
        value->data.v_enum = e;
    } else {
        /* Make it an X value */
        value = vcardvalue_new_impl(kind);
        value->data.v_enum = x_type;
        vcardvalue_set_x(value, str);
    }

    return value;
}

/**
 * Extracts a simple floating point number as a substring.
 * The decimal separator (if any) of the double has to be '.'
 * The code is locale *independent* and does *not* change the locale.
 * It should be thread safe.
 */
static bool simple_str_to_doublestr(const char *from, char *result, int result_len, char **to)
{
    const char *start = NULL;
    char *end = NULL, *cur = (char *)from;

    struct lconv *loc_data = localeconv();
    int i = 0, len;
    double dtest;

    /*sanity checks */
    if (!from || !result) {
        return true;
    }

    /*skip the white spaces at the beginning */
    while (*cur && isspace((int)*cur)) {
        cur++;
    }

    start = cur;
    /* copy the part that looks like a double into result.
     * during the copy, we give ourselves a chance to convert the '.'
     * into the decimal separator of the current locale.
     */
    while (*cur && (isdigit((int)*cur) || *cur == '.' || *cur == '+' || *cur == '-')) {
        ++cur;
    }
    end = cur;
    len = (int)(ptrdiff_t)(end - start);
    if (len + 1 >= result_len) {
        /* huh hoh, number is too big. truncate it */
        len = result_len - 1;
    }

    /* copy the float number string into result, and take
     * care to have the (optional) decimal separator be the one
     * of the current locale.
     */
    for (i = 0; i < len; ++i) {
        if (start[i] == '.' &&
            loc_data && loc_data->decimal_point && loc_data->decimal_point[0] && loc_data->decimal_point[0] != '.') {
            /*replace '.' by the digit separator of the current locale */
            result[i] = loc_data->decimal_point[0];
        } else {
            result[i] = start[i];
        }
    }
    if (to) {
        *to = end;
    }

    /* now try to convert to a floating point number, to check for validity only */
    if (sscanf(result, "%lf", &dtest) != 1) {
        return true;
    }
    return false;
}

static vcardvalue *vcardvalue_new_from_string_with_error(vcardvalue_kind kind,
                                                         const char *str,
                                                         vcardproperty **error)
{
    struct vcardvalue_impl *value = 0;

    icalerror_check_arg_rz(str != 0, "str");

    if (error != 0) {
        *error = 0;
    }

    switch (kind) {
    case VCARD_BOOLEAN_VALUE: {
        if (!strcmp(str, "TRUE")) {
            value = vcardvalue_new_boolean(1);
        } else if (!strcmp(str, "FALSE")) {
            value = vcardvalue_new_boolean(0);
        } else if (error != 0) {
            char temp[TMP_BUF_SIZE];
            vcardparameter *errParam;

            snprintf(temp, sizeof(temp),
                     "Could not parse %s as a %s property",
                     str, vcardvalue_kind_to_string(kind));
            errParam = vcardparameter_new_xlicerrortype(VCARD_XLICERRORTYPE_VALUEPARSEERROR);
            *error = vcardproperty_vanew_xlicerror(temp, errParam, (void *)0);
        }
        break;
    }

    case VCARD_VERSION_VALUE:
        value = vcardvalue_new_enum(kind, (int)VCARD_VERSION_X, str);
        break;

    case VCARD_KIND_VALUE:
        value = vcardvalue_new_enum(kind, (int)VCARD_KIND_X, str);
        break;

    case VCARD_GRAMGENDER_VALUE:
        value = vcardvalue_new_enum(kind, (int)VCARD_GRAMGENDER_X, str);
        break;

    case VCARD_INTEGER_VALUE:
        value = vcardvalue_new_integer(atoi(str));
        break;

    case VCARD_FLOAT_VALUE:
        value = vcardvalue_new_float((float)atof(str));
        break;

    case VCARD_UTCOFFSET_VALUE: {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
        /* "+" / "-" hh [ [":"] mm ] */
        char sign[2] = "";
        unsigned hour, min = 0;
        int nchar = 0, len = (int)strlen(str);

        if (len > 3) {
            const char *fmt;

            if (str[3] == ':') {
                fmt = "%1[+-]%02u:%02u%n";
            } else {
                fmt = "%1[+-]%02u%02u%n";
            }

            if (3 != sscanf(str, fmt, sign, &hour, &min, &nchar)) {
                nchar = 0;
            }
        } else if (2 != sscanf(str, "%1[+-]%02u%n", sign, &hour, &nchar)) {
            nchar = 0;
        }
#pragma GCC diagnostic pop

        if (len && (len == nchar)) {
            int utcoffset = (int)(hour * 3600 + min * 60);

            if (*sign == '-') {
                utcoffset = -utcoffset;
            }
            value = vcardvalue_new_utcoffset(utcoffset);
        } else if (error != 0) {
            char temp[TMP_BUF_SIZE];
            vcardparameter *errParam;

            snprintf(temp, sizeof(temp),
                     "Could not parse %s as a %s property",
                     str, vcardvalue_kind_to_string(kind));
            errParam = vcardparameter_new_xlicerrortype(VCARD_XLICERRORTYPE_VALUEPARSEERROR);
            *error = vcardproperty_vanew_xlicerror(temp, errParam, (void *)0);
        }
        break;
    }

    case VCARD_TEXT_VALUE: {
        char *dequoted_str = vcardvalue_strdup_and_dequote_text(&str, NULL);

        value = vcardvalue_new_text(dequoted_str);
        icalmemory_free_buffer(dequoted_str);
        break;
    }

    case VCARD_TEXTLIST_VALUE: {
        value = vcardvalue_new_textlist(vcardtextlist_new_from_string(str, ','));
        break;
    }

    case VCARD_STRUCTURED_VALUE: {
        vcardstructuredtype *st = vcardstructured_new_from_string(str);
        value = vcardvalue_new_structured(st);
        vcardstructured_unref(st);
        break;
    }

    case VCARD_GEO_VALUE: {
        char *cur = NULL;
        struct vcardgeotype geo = {0};
        memset(geo.coords.lat, 0, VCARD_GEO_LEN);
        memset(geo.coords.lon, 0, VCARD_GEO_LEN);

        if (simple_str_to_doublestr(str, geo.coords.lat, VCARD_GEO_LEN, &cur)) {
            goto geo_parsing_error;
        }
        /* skip white spaces */
        while (cur && isspace((int)*cur)) {
            ++cur;
        }

        /*there is a ';' between the latitude and longitude parts */
        if (!cur || *cur != ';') {
            goto geo_parsing_error;
        }

        ++cur;

        /* skip white spaces */
        while (cur && isspace((int)*cur)) {
            ++cur;
        }

        if (simple_str_to_doublestr(cur, geo.coords.lon, VCARD_GEO_LEN, &cur)) {
            goto geo_parsing_error;
        }
        value = vcardvalue_new_geo(geo);
        break;

    geo_parsing_error:
        if (error != 0) {
            char temp[TMP_BUF_SIZE];
            snprintf(temp, sizeof(temp),
                     "Could not parse %s as a %s value",
                     str, vcardvalue_kind_to_string(kind));
            vcardparameter *errParam =
                vcardparameter_new_xlicerrortype(VCARD_XLICERRORTYPE_VALUEPARSEERROR);
            *error = vcardproperty_vanew_xlicerror(temp, errParam, (void *)0);
        }
    } break;

    case VCARD_URI_VALUE:
        value = vcardvalue_new_uri(str);
        break;

    case VCARD_DATE_VALUE:
    case VCARD_TIME_VALUE:
    case VCARD_DATETIME_VALUE:
    case VCARD_DATEANDORTIME_VALUE:
    case VCARD_TIMESTAMP_VALUE: {
        struct vcardtimetype tt;

        tt = vcardtime_from_string(str, kind == VCARD_TIME_VALUE);
        if (!vcardtime_is_null_datetime(tt)) {
            value = vcardvalue_new_impl(kind);
            value->data.v_time = tt;

            vcardvalue_reset_kind(value);
        }
        break;
    }

    case VCARD_LANGUAGETAG_VALUE:
        value = vcardvalue_new_languagetag(str);
        break;

    case VCARD_X_VALUE: {
        value = vcardvalue_new_x(str);
    } break;

    default: {
        char temp[TMP_BUF_SIZE];
        if (error != 0) {
            snprintf(temp, TMP_BUF_SIZE, "Unknown type for \'%s\'", str);
            vcardparameter *errParam = vcardparameter_new_xlicerrortype(VCARD_XLICERRORTYPE_VALUEPARSEERROR);
            *error = vcardproperty_vanew_xlicerror(temp, errParam, (void *)0);
        }

        snprintf(temp, TMP_BUF_SIZE,
                 "vcardvalue_new_from_string got an unknown value type (%s) for \'%s\'",
                 vcardvalue_kind_to_string(kind), str);
        icalerror_warn(temp);
        value = 0;
    }
    }

    if (error != 0 && *error == 0 && value == 0) {
        char temp[TMP_BUF_SIZE];
        vcardparameter *errParam;

        snprintf(temp, TMP_BUF_SIZE, "Failed to parse value: \'%s\'", str);

        /* coverity[resource_leak] */
        errParam = vcardparameter_new_xlicerrortype(VCARD_XLICERRORTYPE_VALUEPARSEERROR);
        *error = vcardproperty_vanew_xlicerror(temp, errParam, (void *)0);
    }

    return value;
}

vcardvalue *vcardvalue_new_from_string(vcardvalue_kind kind, const char *str)
{
    return vcardvalue_new_from_string_with_error(kind, str, (vcardproperty **)0);
}

void vcardvalue_free(vcardvalue *v)
{
    icalerror_check_arg_rv((v != 0), "value");

    if (v->parent != 0) {
        return;
    }

    if (v->x_value != 0) {
        icalmemory_free_buffer(v->x_value);
    }

    switch (v->kind) {
    case VCARD_TEXT_VALUE:
    case VCARD_URI_VALUE:
    case VCARD_LANGUAGETAG_VALUE: {
        if (v->data.v_string != 0) {
            icalmemory_free_buffer((void *)v->data.v_string);
            v->data.v_string = 0;
        }
        break;
    }

    case VCARD_TEXTLIST_VALUE: {
        if (v->data.v_textlist != 0) {
            vcardstrarray_free(v->data.v_textlist);
        }
        v->data.v_textlist = 0;
        break;
    }

    case VCARD_STRUCTURED_VALUE: {
        vcardstructured_unref(v->data.v_structured);
        v->data.v_structured = 0;
        break;
    }

    case VCARD_GEO_VALUE: {
        v->data.v_geo.coords.lat[0] = '\0';
        v->data.v_geo.coords.lon[0] = '\0';
        break;
    }

    default: {
        /* Nothing to do */
    }
    }

    v->kind = VCARD_NO_VALUE;
    v->size = 0;
    v->parent = 0;
    memset(&(v->data), 0, sizeof(v->data));
    v->id[0] = 'X';
    icalmemory_free_buffer(v);
}

bool vcardvalue_is_valid(const vcardvalue *value)
{
    if (value == 0) {
        return false;
    }

    return true;
}

static char *vcardvalue_boolean_as_vcard_string_r(const vcardvalue *value)
{
    int data;
    char *str;

    icalerror_check_arg_rz((value != 0), "value");
    str = (char *)icalmemory_new_buffer(6);

    data = vcardvalue_get_integer(value);

    strncpy(str, data ? "TRUE" : "FALSE", 6);

    return str;
}

#define MAX_INT_DIGITS 12 /* Enough for 2^32 + sign */

static char *vcardvalue_int_as_vcard_string_r(const vcardvalue *value)
{
    int data;
    char *str;

    icalerror_check_arg_rz((value != 0), "value");
    str = (char *)icalmemory_new_buffer(MAX_INT_DIGITS);

    data = vcardvalue_get_integer(value);

    snprintf(str, MAX_INT_DIGITS, "%d", data);

    return str;
}

static char *vcardvalue_utcoffset_as_vcard_string_r(const vcardvalue *value,
                                                    vcardproperty_version version)
{
    int data, h, m, s;
    char sign;
    char *str;
    size_t size = 10;
    const char *fmt;

    icalerror_check_arg_rz((value != 0), "value");

    str = (char *)icalmemory_new_buffer(size);
    data = vcardvalue_get_utcoffset(value);

    if (abs(data) == data) {
        sign = '+';
    } else {
        sign = '-';
    }

    h = data / 3600;
    m = (data - (h * 3600)) / 60;
    s = (data - (h * 3600) - (m * 60));

    h = MIN(abs(h), 23);
    m = MIN(abs(m), 59);
    s = MIN(abs(s), 59);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    if (s != 0) {
        if (version == VCARD_VERSION_40) {
            fmt = "%c%02d%02d%02d";
        } else {
            fmt = "%c%02d:%02d:%02d";
        }
    } else if (version == VCARD_VERSION_40) {
        if (m != 0) {
            fmt = "%c%02d%02d";
        } else {
            fmt = "%c%02d";
        }
    } else {
        fmt = "%c%02d:%02d";
    }

    snprintf(str, size, fmt, sign, h, m, s);
#pragma GCC diagnostic pop
    return str;
}

static char *vcardvalue_text_as_vcard_string_r(const vcardvalue *value)
{
    char *str = NULL;
    char *str_p = NULL;
    size_t buf_sz;

    return vcardmemory_strdup_and_quote(&str, &str_p, &buf_sz,
                                        value->data.v_string, 0);
}

static char *vcardvalue_string_as_vcard_string_r(const vcardvalue *value)
{
    const char *data;
    char *str = 0;

    icalerror_check_arg_rz((value != 0), "value");
    data = value->data.v_string;

    const size_t len_data = strlen(data) + 1;
    str = (char *)icalmemory_new_buffer(len_data);

    strncpy(str, data, len_data);
    str[len_data - 1] = '\0';

    return str;
}

static void _vcardstrarray_as_vcard_string_r(char **str, char **str_p, size_t *buf_sz,
                                             vcardstrarray *array, const char sep,
                                             bool is_param)
{
    if (!vcardstrarray_size(array)) {
        (void)vcardmemory_strdup_and_quote(str, str_p, buf_sz, "", is_param);
        return;
    }

    size_t i;

    for (i = 0; i < vcardstrarray_size(array); i++) {
        if (i) {
            *str_p -= 1; // backup to \0
            icalmemory_append_char(str, str_p, buf_sz, sep);
        }

        (void)vcardmemory_strdup_and_quote(str, str_p, buf_sz,
                                           vcardstrarray_element_at(array, i), is_param);
    }
}

char *vcardstrarray_as_vcard_string_r(const vcardstrarray *array, const char sep)
{
    char *buf = NULL;
    char *buf_ptr = NULL;
    size_t buf_size;

    _vcardstrarray_as_vcard_string_r(&buf, &buf_ptr, &buf_size,
                                     (vcardstrarray *)array, sep, 0);

    return buf;
}

char *vcardstructured_as_vcard_string_r(const vcardstructuredtype *st, bool is_param)
{
    char *buf;
    char *buf_ptr;
    size_t buf_size = 1;
    size_t i;

    buf_ptr = buf = (char *)icalmemory_new_buffer(buf_size);

    for (i = 0; i < vcardstructured_num_fields(st); i++) {
        vcardstrarray *array = vcardstructured_field_at(st, i);

        if (i) {
            if (buf_ptr > buf) {
                buf_ptr -= 1; // backup to \0
            }
            icalmemory_append_char(&buf, &buf_ptr, &buf_size, ';');
        }

        if (array) {
            _vcardstrarray_as_vcard_string_r(&buf, &buf_ptr, &buf_size,
                                             array, ',', is_param);
        } else {
            icalmemory_append_char(&buf, &buf_ptr, &buf_size, '\0');
        }
    }

    return buf;
}

static char *vcardvalue_textlist_as_vcard_string_r(const vcardvalue *value,
                                                   const char sep)
{
    icalerror_check_arg_rz((value != 0), "value");

    return vcardstrarray_as_vcard_string_r(value->data.v_textlist, sep);
}

static char *vcardvalue_structured_as_vcard_string_r(const vcardvalue *value)
{
    icalerror_check_arg_rz((value != 0), "value");

    return vcardstructured_as_vcard_string_r(value->data.v_structured, 0);
}

static char *vcardvalue_float_as_vcard_string_r(const vcardvalue *value)
{
    float data;
    char *str;
    char *old_locale;

    icalerror_check_arg_rz((value != 0), "value");
    data = vcardvalue_get_float(value);

    /* bypass current locale in order to make
       sure snprintf uses a '.' as a separator
       set locate to 'C' and keep old locale */
    old_locale = icalmemory_strdup(setlocale(LC_NUMERIC, NULL));
    (void)setlocale(LC_NUMERIC, "C");

    str = (char *)icalmemory_new_buffer(40);

    snprintf(str, 40, "%f", data);

    /* restore saved locale */
    (void)setlocale(LC_NUMERIC, old_locale);
    icalmemory_free_buffer(old_locale);

    return str;
}

static char *vcardvalue_geo_as_vcard_string_r(const vcardvalue *value)
{
    icalerror_check_arg_rz((value != 0), "value");
    size_t max_len = 2 * VCARD_GEO_LEN;
    char *str = (char *)icalmemory_new_buffer(max_len);
    snprintf(str, max_len, "%s;%s",
             value->data.v_geo.coords.lat, value->data.v_geo.coords.lon);
    str[max_len - 1] = '\0';
    return str;
}

const char *vcardvalue_as_vcard_string(const vcardvalue *value)
{
    char *buf;

    buf = vcardvalue_as_vcard_string_r(value);
    icalmemory_add_tmp_buffer(buf);
    return buf;
}

char *vcardvalue_as_vcard_string_r(const vcardvalue *value)
{
    vcardproperty_version version = VCARD_VERSION_NONE;
    bool is_structured;
    unsigned flags = 0;

    if (value == 0) {
        return 0;
    }

    if (value->parent) {
        vcardcomponent *comp = vcardproperty_get_parent(value->parent);
        if (comp) {
            version = vcardcomponent_get_version(comp);
        }
    }

    if (version == VCARD_VERSION_NONE) {
        version = VCARD_VERSION_30;
    }

    is_structured = vcardproperty_is_structured(vcardproperty_isa(value->parent));

    switch (value->kind) {
    case VCARD_BOOLEAN_VALUE:
        return vcardvalue_boolean_as_vcard_string_r(value);

    case VCARD_INTEGER_VALUE:
        return vcardvalue_int_as_vcard_string_r(value);

    case VCARD_UTCOFFSET_VALUE:
        return vcardvalue_utcoffset_as_vcard_string_r(value, version);

    case VCARD_TEXT_VALUE:
        return vcardvalue_text_as_vcard_string_r(value);

    case VCARD_TEXTLIST_VALUE:
        return vcardvalue_textlist_as_vcard_string_r(value,
                                                     is_structured ? ';' : ',');

    case VCARD_STRUCTURED_VALUE:
        return vcardvalue_structured_as_vcard_string_r(value);

    case VCARD_GEO_VALUE:
        return vcardvalue_geo_as_vcard_string_r(value);

    case VCARD_URI_VALUE:
    case VCARD_LANGUAGETAG_VALUE:
        return vcardvalue_string_as_vcard_string_r(value);

    case VCARD_TIME_VALUE:
        flags |= VCARDTIME_BARE_TIME;
        _fallthrough();

    case VCARD_DATE_VALUE:
        _fallthrough();

    case VCARD_DATETIME_VALUE:
        _fallthrough();

    case VCARD_DATEANDORTIME_VALUE:
        _fallthrough();

    case VCARD_TIMESTAMP_VALUE:
        if (version == VCARD_VERSION_40) {
            flags |= VCARDTIME_AS_V4;
        }

        return vcardtime_as_vcard_string_r(value->data.v_time, flags);

    case VCARD_FLOAT_VALUE:
        return vcardvalue_float_as_vcard_string_r(value);

    case VCARD_KIND_VALUE:
    case VCARD_VERSION_VALUE:
    case VCARD_GRAMGENDER_VALUE:
        if (value->x_value != 0) {
            return icalmemory_strdup(value->x_value);
        }

        return vcardproperty_enum_to_string_r(value->data.v_enum);

    case VCARD_X_VALUE:
        if (value->x_value != 0) {
            return icalmemory_strdup(value->x_value);
        }
        _fallthrough();

    case VCARD_NO_VALUE:
        _fallthrough();

    default: {
        return 0;
    }
    }
}

vcardvalue_kind vcardvalue_isa(const vcardvalue *value)
{
    if (value == 0) {
        return VCARD_NO_VALUE;
    }

    return value->kind;
}

bool vcardvalue_isa_value(void *value)
{
    const struct vcardvalue_impl *impl = (struct vcardvalue_impl *)value;

    icalerror_check_arg_rz((value != 0), "value");

    if (strcmp(impl->id, "val") == 0) {
        return true;
    } else {
        return false;
    }
}

/** Examine the value and possibly change the kind to agree with the
 *  value
 */

void vcardvalue_reset_kind(vcardvalue *value)
{
    switch (value->kind) {
    case VCARD_DATE_VALUE:
    case VCARD_TIME_VALUE:
    case VCARD_DATETIME_VALUE:
    case VCARD_DATEANDORTIME_VALUE:
    case VCARD_TIMESTAMP_VALUE:
        if (vcardtime_is_timestamp(value->data.v_time)) {
            value->kind = VCARD_TIMESTAMP_VALUE;
        } else if (vcardtime_is_time(value->data.v_time)) {
            value->kind = VCARD_TIME_VALUE;
        } else if (vcardtime_is_date(value->data.v_time)) {
            value->kind = VCARD_DATE_VALUE;
        } else {
            value->kind = VCARD_DATETIME_VALUE;
        }
        break;

    default:
        break;
    }
}

void vcardvalue_set_parent(vcardvalue *value, vcardproperty *property)
{
    icalerror_check_arg_rv((value != 0), "value");

    value->parent = property;
}

vcardproperty *vcardvalue_get_parent(const vcardvalue *value)
{
    return value->parent;
}

/* The remaining interfaces are 'new', 'set' and 'get' for each of the value
   types */
