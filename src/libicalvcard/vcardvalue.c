/*======================================================================
 FILE: vcardvalue.c

======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "vcardvalue.h"
#include "vcardvalueimpl.h"
#include "icalerror.h"
#include "icalmemory.h"
//#include "icaltime.h"

#include <ctype.h>
#include <locale.h>
#include <stdlib.h>

#define TMP_BUF_SIZE 1024

LIBICAL_ICAL_EXPORT struct vcardvalue_impl *vcardvalue_new_impl(vcardvalue_kind kind)
{
    struct vcardvalue_impl *v;

    if (!vcardvalue_kind_is_valid(kind))
        return NULL;

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
    return (vcardvalue *) vcardvalue_new_impl(kind);
}

vcardvalue *vcardvalue_clone(const vcardvalue *old)
{
    struct vcardvalue_impl *new;

    new = vcardvalue_new_impl(old->kind);

    if (new == 0) {
        return 0;
    }

    strcpy(new->id, old->id);
    new->kind = old->kind;
    new->size = old->size;

    switch (new->kind) {
    case VCARD_TEXT_VALUE:
    case VCARD_URI_VALUE:
    case VCARD_LANGUAGETAG_VALUE:
        {
            if (old->data.v_string != 0) {
                new->data.v_string = icalmemory_strdup(old->data.v_string);

                if (new->data.v_string == 0) {
                    new->parent = 0;
                    vcardvalue_free(new);
                    return 0;
                }
            }
            break;
        }
    case VCARD_X_VALUE:
        {
            if (old->x_value != 0) {
                new->x_value = icalmemory_strdup(old->x_value);

                if (new->x_value == 0) {
                    new->parent = 0;
                    vcardvalue_free(new);
                    return 0;
                }
            }

            break;
        }

    default:
        {
            /* all of the other types are stored as values, not
               pointers, so we can just copy the whole structure. */

            new->data = old->data;
        }
    }

    return new;
}
#if 0
static char *icalmemory_strdup_and_dequote(const char *str)
{
    const char *p;
    char *out = (char *)icalmemory_new_buffer(sizeof(char) * strlen(str) + 1);
    char *pout;
    int wroteNull = 0;

    if (out == 0) {
        return 0;
    }

    pout = out;

    /* Stop the loop when encountering a terminator in the source string
       or if a null has been written to the destination. This prevents
       reading past the end of the source string if the last character
       is a backslash. */
    for (p = str; !wroteNull && *p != 0; p++) {

        if (*p == '\\') {
            p++;
            switch (*p) {
            case 0:
                {
                    wroteNull = 1;      //stops iteration so p isn't incremented past the end of str
                    *pout = '\0';
                    break;
                }
            case 'n':
            case 'N':
                {
                    *pout = '\n';
                    break;
                }
            case 't':
            case 'T':
                {
                    *pout = '\t';
                    break;
                }
            case 'r':
            case 'R':
                {
                    *pout = '\r';
                    break;
                }
            case 'b':
            case 'B':
                {
                    *pout = '\b';
                    break;
                }
            case 'f':
            case 'F':
                {
                    *pout = '\f';
                    break;
                }
            case ';':
            case ',':
            case '"':
            case '\\':
                {
                    *pout = *p;
                    break;
                }
            default:
                {
                    *pout = ' ';
                }
            }
        } else {
            *pout = *p;
        }

        pout++;
    }

    *pout = '\0';

    return out;
}

/*
 * Returns a quoted copy of a string
 * @todo This is not RFC5545 compliant.
 * The RFC only allows:
 * TSAFE-CHAR = %x20-21 / %x23-2B / %x2D-39 / %x3C-5B / %x5D-7E / NON-US-ASCII
 * As such, \t\r\b\f are not allowed, not even escaped
 */
static char *icalmemory_strdup_and_quote(const vcardvalue *value, const char *unquoted_str)
{
    char *str;
    char *str_p;
    const char *p;
    size_t buf_sz;

    buf_sz = strlen(unquoted_str) + 1;

    str_p = str = (char *)icalmemory_new_buffer(buf_sz);

    if (str_p == 0) {
        return 0;
    }

    for (p = unquoted_str; *p != 0; p++) {

        switch (*p) {
        case '\n':{
                icalmemory_append_string(&str, &str_p, &buf_sz, "\\n");
                break;
            }

/*issue74: \t is not escaped, but embedded literally.*/
        case '\t':{
                icalmemory_append_string(&str, &str_p, &buf_sz, "\t");
                break;
            }

/*issue74: \r, \b and \f are not whitespace and are trashed.*/
        case '\r':{
                /*icalmemory_append_string(&str,&str_p,&buf_sz,"\\r"); */
                break;
            }
        case '\b':{
                /*icalmemory_append_string(&str,&str_p,&buf_sz,"\\b"); */
                break;
            }
        case '\f':{
                /*icalmemory_append_string(&str,&str_p,&buf_sz,"\\f"); */
                break;
            }

        case ';':
        case ',':
            /* unescaped COMMA is allowed in CATEGORIES property as its
               considered a list delimiter here, see:
               https://tools.ietf.org/html/rfc5545#section-3.8.1.2 */
            if ((icalproperty_isa(value->parent) == VCARD_CATEGORIES_PROPERTY) ||
                (icalproperty_isa(value->parent) == VCARD_RESOURCES_PROPERTY) ||
                (icalproperty_isa(value->parent) == VCARD_POLLPROPERTIES_PROPERTY) ||
                (icalproperty_isa(value->parent) == VCARD_LOCATIONTYPE_PROPERTY)) {
                icalmemory_append_char(&str, &str_p, &buf_sz, *p);
                break;
            }
            /* falls through */
/*issue74, we don't escape double quotes
        case '"':
*/
        case '\\':{
                icalmemory_append_char(&str, &str_p, &buf_sz, '\\');
                icalmemory_append_char(&str, &str_p, &buf_sz, *p);
                break;
            }

        default:{
                icalmemory_append_char(&str, &str_p, &buf_sz, *p);
            }
        }
    }

    /* Assume the last character is not a '\0' and add one. We could
       check *str_p != 0, but that would be an uninitialized memory
       read. */

    icalmemory_append_char(&str, &str_p, &buf_sz, '\0');
    return str;
}

/*
 * FIXME
 *
 * This is a bad API, as it forces callers to specify their own X type.
 * This function should take care of this by itself.
 */
static vcardvalue *vcardvalue_new_enum(vcardvalue_kind kind, int x_type, const char *str)
{
    int e = icalproperty_kind_and_string_to_enum(kind, str);
    struct vcardvalue_impl *value;

    if (e != 0 && icalproperty_enum_belongs_to_property(icalproperty_value_kind_to_kind(kind), e)) {

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
static int simple_str_to_doublestr(const char *from, char *result, int result_len, char **to)
{
    char *start = NULL, *end = NULL, *cur = (char *)from;

#if !defined(HAVE_GETNUMBERFORMAT)
    struct lconv *loc_data = localeconv();
#endif
    int i = 0;
    double dtest;

    /*sanity checks */
    if (!from || !result) {
        return 1;
    }

    /*skip the white spaces at the beginning */
    while (*cur && isspace((int)*cur))
        cur++;

    start = cur;
    /* copy the part that looks like a double into result.
     * during the copy, we give ourselves a chance to convert the '.'
     * into the decimal separator of the current locale.
     */
    while (*cur && (isdigit((int)*cur) || *cur == '.' || *cur == '+' || *cur == '-')) {
        ++cur;
    }
    end = cur;
    if (end - start + 1 > result_len) {
        /*huh hoh, number is too big. getting out */
        return 1;
    }

    /* copy the float number string into tmp_buf, and take
     * care to have the (optional) decimal separator be the one
     * of the current locale.
     */
#if !defined(HAVE_GETNUMBERFORMAT)
    for (i = 0; i < end - start; ++i) {
        if (start[i] == '.' && loc_data && loc_data->decimal_point && loc_data->decimal_point[0]
            && loc_data->decimal_point[0] != '.') {
            /*replace '.' by the digit separator of the current locale */
            result[i] = loc_data->decimal_point[0];
        } else {
            result[i] = start[i];
        }
    }
#else
    GetNumberFormat(LOCALE_SYSTEM_DEFAULT, 0, start, NULL, result, result_len);
#endif
    if (to) {
        *to = end;
    }

    /* now try to convert to a floating point number, to check for validity only */
    if (sscanf(result, "%lf", &dtest) != 1) {
        return 1;
    }
    return 0;
}

static void free_vcardvalue_attach_data(char *data, void *user_data)
{
    _unused(user_data);
    free(data);
}

static vcardvalue *vcardvalue_new_from_string_with_error(vcardvalue_kind kind,
                                                       const char *str, icalproperty ** error)
{
    struct vcardvalue_impl *value = 0;

    icalerror_check_arg_rz(str != 0, "str");

    if (error != 0) {
        *error = 0;
    }

    switch (kind) {

    case VCARD_ATTACH_VALUE:
        {
            icalattach *attach;

            attach = icalattach_new_from_url(str);
            if (!attach)
                break;

            value = vcardvalue_new_attach(attach);
            icalattach_unref(attach);
            break;
        }

    case VCARD_BINARY_VALUE:
        {
            icalattach *attach;

            attach = icalattach_new_from_data(strdup(str), free_vcardvalue_attach_data, 0);
            if (!attach)
              break;

            value = vcardvalue_new_attach(attach);

            icalattach_unref(attach);
            break;
        }
    case VCARD_BOOLEAN_VALUE:
        {
            if (!strcmp(str, "TRUE")) {
                value = vcardvalue_new_boolean(1);
            } else if (!strcmp(str, "FALSE")) {
                value = vcardvalue_new_boolean(0);
            } else if (error != 0) {
                char temp[TMP_BUF_SIZE];
                icalparameter *errParam;

                snprintf(temp, sizeof(temp),
                         "Could not parse %s as a %s property",
                         str, vcardvalue_kind_to_string(kind));
                errParam = icalparameter_new_xlicerrortype(VCARD_XLICERRORTYPE_VALUEPARSEERROR);
                *error = icalproperty_vanew_xlicerror(temp, errParam, 0);
                icalparameter_free(errParam);
            }
            break;
        }

    case VCARD_TRANSP_VALUE:
        value = vcardvalue_new_enum(kind, (int)VCARD_TRANSP_X, str);
        break;
    case VCARD_METHOD_VALUE:
        value = vcardvalue_new_enum(kind, (int)VCARD_METHOD_X, str);
        break;
    case VCARD_STATUS_VALUE:
        value = vcardvalue_new_enum(kind, (int)VCARD_STATUS_X, str);
        break;
    case VCARD_ACTION_VALUE:
        value = vcardvalue_new_enum(kind, (int)VCARD_ACTION_X, str);
        break;

    case VCARD_QUERY_VALUE:
        value = vcardvalue_new_query(str);
        break;

    case VCARD_CLASS_VALUE:
        value = vcardvalue_new_enum(kind, (int)VCARD_CLASS_X, str);
        break;
    case VCARD_CMD_VALUE:
        value = vcardvalue_new_enum(kind, VCARD_CMD_X, str);
        break;
    case VCARD_QUERYLEVEL_VALUE:
        value = vcardvalue_new_enum(kind, VCARD_QUERYLEVEL_X, str);
        break;
    case VCARD_CARLEVEL_VALUE:
        value = vcardvalue_new_enum(kind, VCARD_CARLEVEL_X, str);
        break;
    case VCARD_BUSYTYPE_VALUE:
        value = vcardvalue_new_enum(kind, VCARD_BUSYTYPE_X, str);
        break;
    case VCARD_POLLMODE_VALUE:
        value = vcardvalue_new_enum(kind, VCARD_POLLMODE_X, str);
        break;
    case VCARD_POLLCOMPLETION_VALUE:
        value = vcardvalue_new_enum(kind, VCARD_POLLCOMPLETION_X, str);
        break;

    case VCARD_INTEGER_VALUE:
        value = vcardvalue_new_integer(atoi(str));
        break;

    case VCARD_FLOAT_VALUE:
        value = vcardvalue_new_float((float)atof(str));
        break;

    case VCARD_UTCOFFSET_VALUE:
        {
            int t, utcoffset, hours, minutes, seconds;

            /* treat the UTCOFSET string as a decimal number, disassemble its digits
               and reconstruct it as sections */
            t = strtol(str, 0, 10);
            /* add phantom seconds field */
            if (strlen(str) < 7) {
                t *= 100;
            }
            hours = (t / 10000);
            minutes = (t - hours * 10000) / 100;
            seconds = (t - hours * 10000 - minutes * 100);
            utcoffset = hours * 3600 + minutes * 60 + seconds;

            value = vcardvalue_new_utcoffset(utcoffset);

            break;
        }

    case VCARD_TEXT_VALUE:
        {
            char *dequoted_str = icalmemory_strdup_and_dequote(str);

            value = vcardvalue_new_text(dequoted_str);
            icalmemory_free_buffer(dequoted_str);
            break;
        }

    case VCARD_STRING_VALUE:
        value = vcardvalue_new_string(str);
        break;

    case VCARD_VCARDADDRESS_VALUE:
        value = vcardvalue_new_caladdress(str);
        break;

    case VCARD_URI_VALUE:
        value = vcardvalue_new_uri(str);
        break;

    case VCARD_GEO_VALUE:
        {
            char *cur = NULL;
            struct icalgeotype geo;
            memset(geo.lat, 0, VCARD_GEO_LEN);
            memset(geo.lon, 0, VCARD_GEO_LEN);

            if (simple_str_to_doublestr(str, geo.lat, VCARD_GEO_LEN, &cur)) {
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

            if (simple_str_to_doublestr(cur, geo.lon, VCARD_GEO_LEN, &cur)) {
                goto geo_parsing_error;
            }
            value = vcardvalue_new_geo(geo);
            break;

          geo_parsing_error:
            if (error != 0) {
                char temp[TMP_BUF_SIZE];
                icalparameter *errParam;

                snprintf(temp, sizeof(temp),
                         "Could not parse %s as a %s property",
                         str, vcardvalue_kind_to_string(kind));
                errParam = icalparameter_new_xlicerrortype(VCARD_XLICERRORTYPE_VALUEPARSEERROR);
                *error = icalproperty_vanew_xlicerror(temp, errParam, 0);
                icalparameter_free(errParam);
            }
        }
        break;

    case VCARD_RECUR_VALUE:
        {
            struct icalrecurrencetype rt;

            rt = icalrecurrencetype_from_string(str);
            if (rt.freq != VCARD_NO_RECURRENCE) {
                value = vcardvalue_new_recur(rt);
            }
            icalmemory_free_buffer(rt.rscale);
            break;
        }

    case VCARD_DATE_VALUE:
    case VCARD_DATETIME_VALUE:
        {
            struct icaltimetype tt;

            tt = icaltime_from_string(str);
            if (!icaltime_is_null_time(tt)) {
                value = vcardvalue_new_impl(kind);
                value->data.v_time = tt;

                vcardvalue_reset_kind(value);
            }
            break;
        }

    case VCARD_DATETIMEPERIOD_VALUE:
        {
            struct icaltimetype tt;
            struct icalperiodtype p;

            tt = icaltime_from_string(str);

            if (!icaltime_is_null_time(tt)) {
                value = vcardvalue_new_datetime(tt);
                break;
            }

            p = icalperiodtype_from_string(str);
            if (!icalperiodtype_is_null_period(p)) {
                value = vcardvalue_new_period(p);
            }

            break;
        }

    case VCARD_DURATION_VALUE:
        {
            struct icaldurationtype dur = icaldurationtype_from_string(str);

            if (!icaldurationtype_is_bad_duration(dur)) {       /* failed to parse */
                value = vcardvalue_new_duration(dur);
            }

            break;
        }

    case VCARD_PERIOD_VALUE:
        {
            struct icalperiodtype p;

            p = icalperiodtype_from_string(str);

            if (!icalperiodtype_is_null_period(p)) {
                value = vcardvalue_new_period(p);
            }
            break;
        }

    case VCARD_TRIGGER_VALUE:
        {
            struct icaltriggertype tr = icaltriggertype_from_string(str);

            if (!icaltriggertype_is_bad_trigger(tr)) {
                value = vcardvalue_new_trigger(tr);
            }
            break;
        }

    case VCARD_REQUESTSTATUS_VALUE:
        {
            struct icalreqstattype rst = icalreqstattype_from_string(str);

            if (rst.code != VCARD_UNKNOWN_STATUS) {
                value = vcardvalue_new_requeststatus(rst);
            }
            break;
        }

    case VCARD_X_VALUE:
        {
            char *dequoted_str = icalmemory_strdup_and_dequote(str);

            value = vcardvalue_new_x(dequoted_str);
            icalmemory_free_buffer(dequoted_str);
        }
        break;

    default:
        {
            char temp[TMP_BUF_SIZE];
            icalparameter *errParam;

            if (error != 0) {

                snprintf(temp, TMP_BUF_SIZE, "Unknown type for \'%s\'", str);

                errParam = icalparameter_new_xlicerrortype(VCARD_XLICERRORTYPE_VALUEPARSEERROR);
                *error = icalproperty_vanew_xlicerror(temp, errParam, 0);
                icalparameter_free(errParam);
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
        icalparameter *errParam;

        snprintf(temp, TMP_BUF_SIZE, "Failed to parse value: \'%s\'", str);

        errParam = icalparameter_new_xlicerrortype(VCARD_XLICERRORTYPE_VALUEPARSEERROR);
        *error = icalproperty_vanew_xlicerror(temp, errParam, 0);
        icalparameter_free(errParam);
    }

    return value;
}

vcardvalue *vcardvalue_new_from_string(vcardvalue_kind kind, const char *str)
{
    return vcardvalue_new_from_string_with_error(kind, str, (icalproperty **) 0);
}
#endif
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
    case VCARD_LANGUAGETAG_VALUE:
        {
            if (v->data.v_string != 0) {
                icalmemory_free_buffer((void *)v->data.v_string);
                v->data.v_string = 0;
            }
            break;
        }

    default:
        {
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
#if 0
int vcardvalue_is_valid(const vcardvalue *value)
{
    if (value == 0) {
        return 0;
    }

    return 1;
}

static char *vcardvalue_binary_as_ical_string_r(const vcardvalue *value)
{
    char *str;

    icalerror_check_arg_rz((value != 0), "value");

    str = (char *)icalmemory_new_buffer(60);
    snprintf(str, 60, "vcardvalue_binary_as_ical_string is not implemented yet");

    return str;
}

static char *vcardvalue_boolean_as_ical_string_r(const vcardvalue *value)
{
    int data;
    char *str;

    icalerror_check_arg_rz((value != 0), "value");
    str = (char *)icalmemory_new_buffer(6);

    data = vcardvalue_get_integer(value);

    strcpy(str, data ? "TRUE" : "FALSE");

    return str;
}

#define MAX_INT_DIGITS 12       /* Enough for 2^32 + sign */

static char *vcardvalue_int_as_ical_string_r(const vcardvalue *value)
{
    int data;
    char *str;

    icalerror_check_arg_rz((value != 0), "value");
    str = (char *)icalmemory_new_buffer(MAX_INT_DIGITS);

    data = vcardvalue_get_integer(value);

    snprintf(str, MAX_INT_DIGITS, "%d", data);

    return str;
}

static char *vcardvalue_utcoffset_as_ical_string_r(const vcardvalue *value)
{
    int data, h, m, s;
    char sign;
    char *str;

    icalerror_check_arg_rz((value != 0), "value");

    str = (char *)icalmemory_new_buffer(9);
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
    if (s != 0) {
        snprintf(str, 9, "%c%02d%02d%02d", sign, h, m, s);
    } else {
        snprintf(str, 9, "%c%02d%02d", sign, h, m);
    }

    return str;
}

static char *vcardvalue_string_as_ical_string_r(const vcardvalue *value)
{
    const char *data;
    char *str = 0;

    icalerror_check_arg_rz((value != 0), "value");
    data = value->data.v_string;

    str = (char *)icalmemory_new_buffer(strlen(data) + 1);

    strcpy(str, data);

    return str;
}

static char *vcardvalue_recur_as_ical_string_r(const vcardvalue *value)
{
    struct icalrecurrencetype *recur = value->data.v_recur;

    return icalrecurrencetype_as_string_r(recur);
}

static char *vcardvalue_text_as_ical_string_r(const vcardvalue *value)
{
    return icalmemory_strdup_and_quote(value, value->data.v_string);
}

static char *vcardvalue_attach_as_ical_string_r(const vcardvalue *value)
{
    icalattach *a;
    char *str;

    icalerror_check_arg_rz((value != 0), "value");

    a = vcardvalue_get_attach(value);

    if (icalattach_get_is_url(a)) {
        const char *url;

        url = icalattach_get_url(a);
        str = icalmemory_new_buffer(strlen(url) + 1);
        strcpy(str, url);
        return str;
    } else {
        const char *data = 0;

        data = (const char *)icalattach_get_data(a);
        str = icalmemory_new_buffer(strlen(data) + 1);
        strcpy(str, data);
        return str;
    }
}

static char *vcardvalue_duration_as_ical_string_r(const vcardvalue *value)
{
    struct icaldurationtype data;

    icalerror_check_arg_rz((value != 0), "value");
    data = vcardvalue_get_duration(value);

    return icaldurationtype_as_ical_string_r(data);
}

static void print_time_to_string(char *str, const struct icaltimetype *data)
{       /* this function is a candidate for a library-wide external function
           except it isn't used any place outside of vcardvalue.c.
           see print_date_to_string() and print_datetime_to_string in vcardvalue.h */
    char temp[20];

    str[0] = '\0';

    if (data != 0) {
        if (icaltime_is_utc(*data)) {
            snprintf(temp, sizeof(temp), "%02d%02d%02dZ", data->hour, data->minute, data->second);
            strncat(str, temp, 7);
        } else {
            snprintf(temp, sizeof(temp), "%02d%02d%02d", data->hour, data->minute, data->second);
            strncat(str, temp, 6);
        }
    }
}

void print_date_to_string(char *str, const struct icaltimetype *data)
{
    char temp[20];

    str[0] = '\0';

    if (data != 0) {
        snprintf(temp, sizeof(temp), "%04d%02d%02d", data->year, data->month, data->day);
        strncat(str, temp, 8);
    }
}

static char *vcardvalue_date_as_ical_string_r(const vcardvalue *value)
{
    struct icaltimetype data;
    char *str;

    icalerror_check_arg_rz((value != 0), "value");
    data = vcardvalue_get_date(value);

    str = (char *)icalmemory_new_buffer(9);

    str[0] = '\0';
    print_date_to_string(str, &data);

    return str;
}

void print_datetime_to_string(char *str, const struct icaltimetype *data)
{
    char temp[20];

    str[0] = '\0';

    if (data != 0) {
        print_date_to_string(str, data);
        if (!data->is_date) {
            strncat(str, "T", 19);
            temp[0] = '\0';
            print_time_to_string(temp, data);
            strncat(str, temp, 19);
        }
    }
}

static char *vcardvalue_datetime_as_ical_string_r(const vcardvalue *value)
{
    struct icaltimetype data;
    char *str;
    vcardvalue_kind kind = vcardvalue_isa(value);

    icalerror_check_arg_rz((value != 0), "value");

    if (!(kind == VCARD_DATE_VALUE || kind == VCARD_DATETIME_VALUE)) {
        icalerror_set_errno(VCARD_BADARG_ERROR);
        return 0;
    }

    data = vcardvalue_get_datetime(value);

    str = (char *)icalmemory_new_buffer(20);

    str[0] = 0;
    print_datetime_to_string(str, &data);

    return str;
}

static char *vcardvalue_float_as_ical_string_r(const vcardvalue *value)
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

static char *vcardvalue_geo_as_ical_string_r(const vcardvalue *value)
{
    struct icalgeotype data;
    char *str;
    char *old_locale;

    icalerror_check_arg_rz((value != 0), "value");

    data = vcardvalue_get_geo(value);

    /* bypass current locale in order to make
     * sure snprintf uses a '.' as a separator
     * set locate to 'C' and keep old locale */
    old_locale = icalmemory_strdup(setlocale(LC_NUMERIC, NULL));
    (void)setlocale(LC_NUMERIC, "C");

    str = (char *)icalmemory_new_buffer(80);

    snprintf(str, 80, "%s;%s", data.lat, data.lon);

    /* restore saved locale */
    (void)setlocale(LC_NUMERIC, old_locale);
    icalmemory_free_buffer(old_locale);

    return str;
}

static char *vcardvalue_datetimeperiod_as_ical_string_r(const vcardvalue *value)
{
    struct icaldatetimeperiodtype dtp = vcardvalue_get_datetimeperiod(value);

    icalerror_check_arg_rz((value != 0), "value");

    if (!icaltime_is_null_time(dtp.time)) {
        return icaltime_as_ical_string_r(dtp.time);
    } else {
        return icalperiodtype_as_ical_string_r(dtp.period);
    }
}

static char *vcardvalue_period_as_ical_string_r(const vcardvalue *value)
{
    struct icalperiodtype data;

    icalerror_check_arg_rz((value != 0), "value");
    data = vcardvalue_get_period(value);

    return icalperiodtype_as_ical_string_r(data);
}

static char *vcardvalue_trigger_as_ical_string_r(const vcardvalue *value)
{
    struct icaltriggertype data;

    icalerror_check_arg_rz((value != 0), "value");
    data = vcardvalue_get_trigger(value);

    if (!icaltime_is_null_time(data.time)) {
        return icaltime_as_ical_string_r(data.time);
    } else {
        return icaldurationtype_as_ical_string_r(data.duration);
    }
}

const char *vcardvalue_as_ical_string(const vcardvalue *value)
{
    char *buf;

    buf = vcardvalue_as_ical_string_r(value);
    icalmemory_add_tmp_buffer(buf);
    return buf;
}

char *vcardvalue_as_ical_string_r(const vcardvalue *value)
{
    if (value == 0) {
        return 0;
    }

    switch (value->kind) {

    case VCARD_ATTACH_VALUE:
        return vcardvalue_attach_as_ical_string_r(value);

    case VCARD_BINARY_VALUE:
        return vcardvalue_binary_as_ical_string_r(value);

    case VCARD_BOOLEAN_VALUE:
        return vcardvalue_boolean_as_ical_string_r(value);

    case VCARD_INTEGER_VALUE:
        return vcardvalue_int_as_ical_string_r(value);

    case VCARD_UTCOFFSET_VALUE:
        return vcardvalue_utcoffset_as_ical_string_r(value);

    case VCARD_TEXT_VALUE:
        return vcardvalue_text_as_ical_string_r(value);

    case VCARD_QUERY_VALUE:
        return vcardvalue_string_as_ical_string_r(value);

    case VCARD_STRING_VALUE:
    case VCARD_URI_VALUE:
    case VCARD_VCARDADDRESS_VALUE:
        return vcardvalue_string_as_ical_string_r(value);

    case VCARD_DATE_VALUE:
        return vcardvalue_date_as_ical_string_r(value);
    case VCARD_DATETIME_VALUE:
        return vcardvalue_datetime_as_ical_string_r(value);
    case VCARD_DURATION_VALUE:
        return vcardvalue_duration_as_ical_string_r(value);

    case VCARD_PERIOD_VALUE:
        return vcardvalue_period_as_ical_string_r(value);
    case VCARD_DATETIMEPERIOD_VALUE:
        return vcardvalue_datetimeperiod_as_ical_string_r(value);

    case VCARD_FLOAT_VALUE:
        return vcardvalue_float_as_ical_string_r(value);

    case VCARD_GEO_VALUE:
        return vcardvalue_geo_as_ical_string_r(value);

    case VCARD_RECUR_VALUE:
        return vcardvalue_recur_as_ical_string_r(value);

    case VCARD_TRIGGER_VALUE:
        return vcardvalue_trigger_as_ical_string_r(value);

    case VCARD_REQUESTSTATUS_VALUE:
        return icalreqstattype_as_string_r(value->data.v_requeststatus);

    case VCARD_ACTION_VALUE:
    case VCARD_CMD_VALUE:
    case VCARD_QUERYLEVEL_VALUE:
    case VCARD_CARLEVEL_VALUE:
    case VCARD_METHOD_VALUE:
    case VCARD_STATUS_VALUE:
    case VCARD_TRANSP_VALUE:
    case VCARD_CLASS_VALUE:
    case VCARD_BUSYTYPE_VALUE:
    case VCARD_POLLMODE_VALUE:
    case VCARD_POLLCOMPLETION_VALUE:
        if (value->x_value != 0) {
            return icalmemory_strdup(value->x_value);
        }

        return icalproperty_enum_to_string_r(value->data.v_enum);

    case VCARD_X_VALUE:
        if (value->x_value != 0)
            return icalmemory_strdup_and_quote(value, value->x_value);

        /* FALLTHRU */

    case VCARD_NO_VALUE:
    default:
        {
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

int vcardvalue_isa_value(void *value)
{
    struct vcardvalue_impl *impl = (struct vcardvalue_impl *)value;

    icalerror_check_arg_rz((value != 0), "value");

    if (strcmp(impl->id, "val") == 0) {
        return 1;
    } else {
        return 0;
    }
}

static int vcardvalue_is_time(const vcardvalue *a)
{
    vcardvalue_kind kind = vcardvalue_isa(a);

    if (kind == VCARD_DATETIME_VALUE || kind == VCARD_DATE_VALUE) {
        return 1;
    }

    return 0;
}

/*
 * In case of error, this function returns 0. This is partly bogus, as 0 is
 * not part of the returned enum.
 * FIXME We should probably add an error value to the enum.
 */
icalparameter_xliccomparetype vcardvalue_compare(const vcardvalue *a, const vcardvalue *b)
{
    icalerror_check_arg_rz((a != 0), "a");
    icalerror_check_arg_rz((b != 0), "b");

    /* Not the same type; they can only be unequal */
    if (!(vcardvalue_is_time(a) && vcardvalue_is_time(b)) && vcardvalue_isa(a) != vcardvalue_isa(b)) {
        return VCARD_XLICCOMPARETYPE_NOTEQUAL;
    }

    switch (vcardvalue_isa(a)) {

    case VCARD_ATTACH_VALUE:
        {
            if (icalattach_get_is_url(a->data.v_attach) &&
                icalattach_get_is_url(b->data.v_attach)) {
                if (strcasecmp(icalattach_get_url(a->data.v_attach),
                               icalattach_get_url(b->data.v_attach)) == 0) {
                    return VCARD_XLICCOMPARETYPE_EQUAL;
                } else {
                    return VCARD_XLICCOMPARETYPE_NOTEQUAL;
                }
            } else {
                if (a->data.v_attach == b->data.v_attach) {
                    return VCARD_XLICCOMPARETYPE_EQUAL;
                } else {
                    return VCARD_XLICCOMPARETYPE_NOTEQUAL;
                }
            }
        }
    case VCARD_BINARY_VALUE:
        {
            if (a->data.v_attach == b->data.v_attach) {
                return VCARD_XLICCOMPARETYPE_EQUAL;
            } else {
                return VCARD_XLICCOMPARETYPE_NOTEQUAL;
            }
        }

    case VCARD_BOOLEAN_VALUE:
        {
            if (vcardvalue_get_boolean(a) == vcardvalue_get_boolean(b)) {
                return VCARD_XLICCOMPARETYPE_EQUAL;
            } else {
                return VCARD_XLICCOMPARETYPE_NOTEQUAL;
            }
        }

    case VCARD_FLOAT_VALUE:
        {
            if (a->data.v_float > b->data.v_float) {
                return VCARD_XLICCOMPARETYPE_GREATER;
            } else if (a->data.v_float < b->data.v_float) {
                return VCARD_XLICCOMPARETYPE_LESS;
            } else {
                return VCARD_XLICCOMPARETYPE_EQUAL;
            }
        }

    case VCARD_INTEGER_VALUE:
    case VCARD_UTCOFFSET_VALUE:
        {
            if (a->data.v_int > b->data.v_int) {
                return VCARD_XLICCOMPARETYPE_GREATER;
            } else if (a->data.v_int < b->data.v_int) {
                return VCARD_XLICCOMPARETYPE_LESS;
            } else {
                return VCARD_XLICCOMPARETYPE_EQUAL;
            }
        }

    case VCARD_DURATION_VALUE:
        {
            int dur_a = icaldurationtype_as_int(a->data.v_duration);
            int dur_b = icaldurationtype_as_int(b->data.v_duration);

            if (dur_a > dur_b) {
                return VCARD_XLICCOMPARETYPE_GREATER;
            } else if (dur_a < dur_b) {
                return VCARD_XLICCOMPARETYPE_LESS;
            } else {
                return VCARD_XLICCOMPARETYPE_EQUAL;
            }
        }

    case VCARD_TEXT_VALUE:
    case VCARD_URI_VALUE:
    case VCARD_VCARDADDRESS_VALUE:
    case VCARD_TRIGGER_VALUE:
    case VCARD_DATE_VALUE:
    case VCARD_DATETIME_VALUE:
    case VCARD_DATETIMEPERIOD_VALUE:
    case VCARD_QUERY_VALUE:
    case VCARD_RECUR_VALUE:
        {
            int r;
            char *temp1, *temp2;

            temp1 = vcardvalue_as_ical_string_r(a);
            temp2 = vcardvalue_as_ical_string_r(b);
            r = strcmp(temp1, temp2);
            icalmemory_free_buffer(temp1);
            icalmemory_free_buffer(temp2);

            if (r > 0) {
                return VCARD_XLICCOMPARETYPE_GREATER;
            } else if (r < 0) {
                return VCARD_XLICCOMPARETYPE_LESS;
            } else {
                return VCARD_XLICCOMPARETYPE_EQUAL;
            }
        }

    case VCARD_METHOD_VALUE:
        {
            if (vcardvalue_get_method(a) == vcardvalue_get_method(b)) {
                return VCARD_XLICCOMPARETYPE_EQUAL;
            } else {
                return VCARD_XLICCOMPARETYPE_NOTEQUAL;
            }
        }

    case VCARD_STATUS_VALUE:
        {
            if (vcardvalue_get_status(a) == vcardvalue_get_status(b)) {
                return VCARD_XLICCOMPARETYPE_EQUAL;
            } else {
                return VCARD_XLICCOMPARETYPE_NOTEQUAL;
            }
        }

    case VCARD_TRANSP_VALUE:
        {
            if (vcardvalue_get_transp(a) == vcardvalue_get_transp(b)) {
                return VCARD_XLICCOMPARETYPE_EQUAL;
            } else {
                return VCARD_XLICCOMPARETYPE_NOTEQUAL;
            }
        }

    case VCARD_ACTION_VALUE:
        {
            if (vcardvalue_get_action(a) == vcardvalue_get_action(b)) {
                return VCARD_XLICCOMPARETYPE_EQUAL;
            } else {
                return VCARD_XLICCOMPARETYPE_NOTEQUAL;
            }
        }

    case VCARD_PERIOD_VALUE:
    case VCARD_GEO_VALUE:
    case VCARD_NO_VALUE:
    default:
        {
            icalerror_warn("Comparison not implemented for value type");
            return 0;
        }
    }
}
#endif
/** Examine the value and possibly change the kind to agree with the
 *  value
 */

void vcardvalue_reset_kind(vcardvalue *value)
{
    if ((value->kind == VCARD_DATETIME_VALUE || value->kind == VCARD_DATE_VALUE) &&
1) {
//        !icaltime_is_null_time(value->data.v_time)) {

//        if (icaltime_is_date(value->data.v_time)) {
if (1) {
            value->kind = VCARD_DATE_VALUE;
        } else {
            value->kind = VCARD_DATETIME_VALUE;
        }
    }
}
#if 0
void vcardvalue_set_parent(vcardvalue *value, icalproperty *property)
{
    icalerror_check_arg_rv((value != 0), "value");

    value->parent = property;
}

icalproperty *vcardvalue_get_parent(vcardvalue *value)
{
    return value->parent;
}

int vcardvalue_encode_ical_string(const char *szText, char *szEncText, int nMaxBufferLen)
{
    char *ptr;
    vcardvalue *value = 0;

    if ((szText == 0) || (szEncText == 0))
        return 0;

    value = vcardvalue_new_from_string(VCARD_STRING_VALUE, szText);

    if (value == 0)
        return 0;

    ptr = vcardvalue_text_as_ical_string_r(value);
    if (ptr == 0)
        return 0;

    if ((int)strlen(ptr) >= nMaxBufferLen) {
        vcardvalue_free(value);
        icalmemory_free_buffer(ptr);
        return 0;
    }

    strcpy(szEncText, ptr);
    icalmemory_free_buffer(ptr);

    vcardvalue_free((vcardvalue *) value);

    return 1;
}

int vcardvalue_decode_ical_string(const char *szText, char *szDecText, int nMaxBufferLen)
{
    char *str, *str_p;
    const char *p;
    size_t buf_sz;

    if ((szText == 0) || (szDecText == 0))
        return 0;

    buf_sz = strlen(szText) + 1;
    str_p = str = (char *)icalmemory_new_buffer(buf_sz);

    if (str_p == 0) {
        return 0;
    }

    for (p = szText; *p != 0; p++) {
        if (*p == '\\') {
            icalmemory_append_char(&str, &str_p, &buf_sz, *(p + 1));
            p++;
        } else {
            icalmemory_append_char(&str, &str_p, &buf_sz, *p);
        }

        if (str_p - str > nMaxBufferLen)
            break;
    }

    icalmemory_append_char(&str, &str_p, &buf_sz, '\0');

    if ((int)strlen(str) >= nMaxBufferLen) {
        icalmemory_free_buffer(str);
        return 0;
    }

    strcpy(szDecText, str);

    icalmemory_free_buffer(str);
    return 1;
}
#endif
/* The remaining interfaces are 'new', 'set' and 'get' for each of the value
   types */
