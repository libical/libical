/*======================================================================
 FILE: vcard_test_encode.c
 CREATOR: Robert Stepanek 24 Aug 2022 <rsto@fastmailteam.com>

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 ======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(NDEBUG)
#undef NDEBUG
#endif

#include "vcard.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#define assert_str_equals(want, have)                                                            \
    {                                                                                            \
        const char *_w = (want);                                                                 \
        const char *_h = (have);                                                                 \
        int _v = strcmp(_w, _h);                                                                 \
        if (_v) {                                                                                \
            fprintf(stderr, "line %d: string mismatch\n want=%s\n have=%s\n", __LINE__, _w, _h); \
            assert(0);                                                                           \
        }                                                                                        \
    }

static void test_prop_text(void)
{
    static const char *input =
        "BEGIN:VCARD\r\n"
        "VERSION:3.0\r\n"
        "FN:x\\,\\\\;\\;,;\\n\\N\r\n"
        "END:VCARD\r\n";

    vcardcomponent *card = vcardparser_parse_string(input);

    vcardproperty *prop =
        vcardcomponent_get_first_property(card, VCARD_FN_PROPERTY);
    assert_str_equals("x,\\;;,;\n\n", vcardproperty_get_fn(prop));
    assert_str_equals("FN:x\\,\\\\\\;\\;\\,\\;\\n\\n\r\n",
                      vcardproperty_as_vcard_string(prop));

    vcardcomponent_free(card);
}

static void test_prop_structured(void)
{
    static const char *input =
        "BEGIN:VCARD\r\n"
        "VERSION:3.0\r\n"
        "N:y,\\,\\\\;\\;,\\n\\N;;,\r\n"
        "END:VCARD\r\n";

    vcardcomponent *card = vcardparser_parse_string(input);

    vcardproperty *prop =
        vcardcomponent_get_first_property(card, VCARD_N_PROPERTY);
    vcardstructuredtype *n = vcardproperty_get_n(prop);
    assert(4 == vcardstructured_num_fields(n));
    assert(2 == vcardstructured_field_at(n, 0)->num_elements);
    assert_str_equals("y", vcardstrarray_element_at(vcardstructured_field_at(n, 0), 0));
    assert_str_equals(",\\", vcardstrarray_element_at(vcardstructured_field_at(n, 0), 1));
    assert(2 == vcardstructured_field_at(n, 1)->num_elements);
    assert_str_equals(";", vcardstrarray_element_at(vcardstructured_field_at(n, 1), 0));
    assert_str_equals("\n\n", vcardstrarray_element_at(vcardstructured_field_at(n, 1), 1));
    assert(0 == vcardstructured_field_at(n, 2)->num_elements);
    assert(2 == vcardstructured_field_at(n, 3)->num_elements);
    assert_str_equals("", vcardstrarray_element_at(vcardstructured_field_at(n, 3), 0));
    assert_str_equals("", vcardstrarray_element_at(vcardstructured_field_at(n, 3), 1));

    assert_str_equals("N:y,\\,\\\\;\\;,\\n\\n;;,\r\n",
                      vcardproperty_as_vcard_string(prop));

    vcardcomponent_free(card);
}

static void test_prop_x_structured(void)
{
    vcardstructuredtype *st = vcardstructured_new(2);
    vcardstrarray *field = vcardstrarray_new(1);
    vcardstrarray_add(field, "foo");
    vcardstructured_set_field_at(st, 0, field);

    field = vcardstrarray_new(1);
    vcardstrarray_add(field, "bar");
    vcardstructured_set_field_at(st, 1, field);

    vcardproperty *prop = vcardproperty_new(VCARD_X_PROPERTY);
    vcardproperty_set_x_name(prop, "X-FOO");
    vcardproperty_set_value(prop, vcardvalue_new_structured(st));
    vcardstructured_unref(st);

    assert_str_equals("X-FOO:foo;bar\r\n", vcardproperty_as_vcard_string(prop));
    vcardproperty_free(prop);
}

static void test_prop_multivalued(void)
{
    static const char *input =
        "BEGIN:VCARD\r\n"
        "VERSION:3.0\r\n"
        "CATEGORIES:\\,,\\N,;\\;\r\n" // separated by comma
        "ORG:\\,;\\;;x\r\n"           // separated by semicolon
        "END:VCARD\r\n";

    vcardcomponent *card = vcardparser_parse_string(input);

    vcardproperty *prop =
        vcardcomponent_get_first_property(card, VCARD_CATEGORIES_PROPERTY);
    vcardstrarray *categories = vcardproperty_get_categories(prop);
    assert(3 == categories->num_elements);
    assert_str_equals(",", vcardstrarray_element_at(categories, 0));
    assert_str_equals("\n", vcardstrarray_element_at(categories, 1));
    assert_str_equals(";;", vcardstrarray_element_at(categories, 2));
    assert_str_equals("CATEGORIES:\\,,\\n,\\;\\;\r\n",
                      vcardproperty_as_vcard_string(prop));

    prop = vcardcomponent_get_first_property(card, VCARD_ORG_PROPERTY);
    vcardstrarray *org = vcardproperty_get_org(prop);
    assert(3 == org->num_elements);
    assert_str_equals(",", vcardstrarray_element_at(org, 0));
    assert_str_equals(";", vcardstrarray_element_at(org, 1));
    assert_str_equals("x", vcardstrarray_element_at(org, 2));

    vcardcomponent_free(card);
}

static void test_prop_x(void)
{
    static const char *input =
        "BEGIN:VCARD\r\n"
        "VERSION:3.0\r\n"
        "X-PROP:foo\r\n"
        "X-PROP:foo;bar\r\n"
        "X-PROP:foo;bar\\;baz\r\n"
        "X-PROP:foo,bar\r\n"
        "X-PROP:foo\\,bar\r\n"
        "END:VCARD\r\n";

    vcardcomponent *card = vcardparser_parse_string(input);
    assert(card != NULL);

    vcardproperty *prop;
    vcardvalue *val;

    prop = vcardcomponent_get_first_property(card, VCARD_X_PROPERTY);
    val = vcardproperty_get_value(prop);
    assert(VCARD_X_VALUE == vcardvalue_isa(val));
    assert_str_equals("foo", vcardvalue_get_x(val));
    assert_str_equals("X-PROP:foo\r\n", vcardproperty_as_vcard_string(prop));

    prop = vcardcomponent_get_next_property(card, VCARD_X_PROPERTY);
    val = vcardproperty_get_value(prop);
    assert(VCARD_X_VALUE == vcardvalue_isa(val));
    assert_str_equals("foo;bar", vcardvalue_get_x(val));
    assert_str_equals("X-PROP:foo;bar\r\n", vcardproperty_as_vcard_string(prop));

    prop = vcardcomponent_get_next_property(card, VCARD_X_PROPERTY);
    val = vcardproperty_get_value(prop);
    assert(VCARD_X_VALUE == vcardvalue_isa(val));
    assert_str_equals("foo;bar\\;baz", vcardvalue_get_x(val));
    assert_str_equals("X-PROP:foo;bar\\;baz\r\n", vcardproperty_as_vcard_string(prop));

    prop = vcardcomponent_get_next_property(card, VCARD_X_PROPERTY);
    val = vcardproperty_get_value(prop);
    assert(VCARD_X_VALUE == vcardvalue_isa(val));
    assert_str_equals("foo,bar", vcardvalue_get_x(val));
    assert_str_equals("X-PROP:foo,bar\r\n", vcardproperty_as_vcard_string(prop));

    prop = vcardcomponent_get_next_property(card, VCARD_X_PROPERTY);
    val = vcardproperty_get_value(prop);
    assert(VCARD_X_VALUE == vcardvalue_isa(val));
    assert_str_equals("foo\\,bar", vcardvalue_get_x(val));
    assert_str_equals("X-PROP:foo\\,bar\r\n", vcardproperty_as_vcard_string(prop));

    assert(0 == vcardcomponent_get_next_property(card, VCARD_X_PROPERTY));

    vcardcomponent_free(card);
}

/* cppcheck-suppress constParameterCallback */
static vcardvalue_kind my_xprop_value_kind_func(const char *name, void *data)
{
    assert(data == (void *)0x1234);
    return !strcasecmp(name, "X-PROP-A") ? VCARD_TEXT_VALUE : VCARD_X_VALUE;
}

static void test_prop_x_value_kind(void)
{
    static const char *input =
        "BEGIN:VCARD\r\n"
        "VERSION:3.0\r\n"
        "X-PROP-A:foo,bar\\,baz\r\n"
        "X-PROP-B:foo,bar\\,baz\r\n"
        "END:VCARD\r\n";

    vcardcomponent *card;
    vcardproperty *prop;
    vcardvalue *val;

    /* Parse all both properties as X value */
    card = vcardparser_parse_string(input);
    prop = vcardcomponent_get_first_property(card, VCARD_X_PROPERTY);
    val = vcardproperty_get_value(prop);
    assert(VCARD_X_VALUE == vcardvalue_isa(val));
    assert_str_equals("foo,bar\\,baz", vcardvalue_get_x(val));
    assert_str_equals("X-PROP-A:foo,bar\\,baz\r\n", vcardproperty_as_vcard_string(prop));
    prop = vcardcomponent_get_next_property(card, VCARD_X_PROPERTY);
    val = vcardproperty_get_value(prop);
    assert(VCARD_X_VALUE == vcardvalue_isa(val));
    assert_str_equals("foo,bar\\,baz", vcardvalue_get_x(val));
    assert_str_equals("X-PROP-B:foo,bar\\,baz\r\n", vcardproperty_as_vcard_string(prop));
    vcardcomponent_free(card);

    /* Parse X-PROP-A property as TEXT value, others as X value */
    vcardparser_set_xprop_value_kind(my_xprop_value_kind_func, (void *)0x1234);

    card = vcardparser_parse_string(input);
    prop = vcardcomponent_get_first_property(card, VCARD_X_PROPERTY);
    val = vcardproperty_get_value(prop);
    assert(VCARD_TEXT_VALUE == vcardvalue_isa(val));
    assert_str_equals("foo,bar,baz", vcardvalue_get_text(val));
    assert_str_equals("X-PROP-A:foo\\,bar\\,baz\r\n", vcardproperty_as_vcard_string(prop));

    prop = vcardcomponent_get_next_property(card, VCARD_X_PROPERTY);
    val = vcardproperty_get_value(prop);
    assert(VCARD_X_VALUE == vcardvalue_isa(val));
    assert_str_equals("foo,bar\\,baz", vcardvalue_get_x(val));
    assert_str_equals("X-PROP-B:foo,bar\\,baz\r\n", vcardproperty_as_vcard_string(prop));
    vcardcomponent_free(card);
}

static void test_param_singlevalued(void)
{
    static const char *input =
        "BEGIN:VCARD\r\n"
        "VERSION:3.0\r\n"
        "X-PROP;TZ=^^^',^x^n:foo\r\n"
        "END:VCARD\r\n";

    vcardcomponent *card = vcardparser_parse_string(input);

    vcardproperty *prop = vcardcomponent_get_first_property(card,
                                                            VCARD_X_PROPERTY);
    vcardparameter *param = vcardproperty_get_first_parameter(prop,
                                                              VCARD_TZ_PARAMETER);

    assert_str_equals("^\",^x\n", vcardparameter_get_tz(param));
    // quote and escape "^x" to "^^x"
    assert_str_equals("TZ=\"^^^',^^x^n\"",
                      vcardparameter_as_vcard_string(param));

    vcardcomponent_free(card);
}

static void test_param_multivalued(void)
{
    static const char *input =
        "BEGIN:VCARD\r\n"
        "VERSION:3.0\r\n"
        "N;SORT-AS=^n^^^',^^:foo;bar,baz;;"
        "END:VCARD\r\n";

    vcardcomponent *card = vcardparser_parse_string(input);

    vcardproperty *prop = vcardcomponent_get_first_property(card,
                                                            VCARD_N_PROPERTY);
    vcardparameter *param = vcardproperty_get_first_parameter(prop,
                                                              VCARD_SORTAS_PARAMETER);
    vcardstrarray *sortas = vcardparameter_get_sortas(param);

    assert(2 == sortas->num_elements);
    assert_str_equals("\n^\"", vcardstrarray_element_at(sortas, 0));
    assert_str_equals("^", vcardstrarray_element_at(sortas, 1));
    assert_str_equals("SORT-AS=^n^^^',^^",
                      vcardparameter_as_vcard_string(param));

    vcardcomponent_free(card);
}

static void test_param_structured(void)
{
    static const char *input =
        "BEGIN:VCARD\r\n"
        "VERSION:4.0\r\n"
        "X-PROP;JSCOMPS=\";a;b,c;d\\,e\":foo\r\n"
        "X-PROP;JSCOMPS=a:foo\r\n"   // non-standard
        "X-PROP;JSCOMPS=a,b:foo\r\n" // non-standard
        "END:VCARD\r\n";

    vcardcomponent *card = vcardparser_parse_string(input);
    vcardproperty *prop;
    vcardparameter *param;
    vcardstructuredtype *jscomps;

    prop = vcardcomponent_get_first_property(card, VCARD_X_PROPERTY);
    param = vcardproperty_get_first_parameter(prop, VCARD_JSCOMPS_PARAMETER);
    jscomps = vcardparameter_get_jscomps(param);
    assert(4 == vcardstructured_num_fields(jscomps));
    assert(0 == vcardstrarray_size(vcardstructured_field_at(jscomps, 0)));
    assert(1 == vcardstrarray_size(vcardstructured_field_at(jscomps, 1)));
    assert_str_equals("a", vcardstrarray_element_at(vcardstructured_field_at(jscomps, 1), 0));
    assert(2 == vcardstrarray_size(vcardstructured_field_at(jscomps, 2)));
    assert_str_equals("b", vcardstrarray_element_at(vcardstructured_field_at(jscomps, 2), 0));
    assert_str_equals("c", vcardstrarray_element_at(vcardstructured_field_at(jscomps, 2), 1));
    assert(1 == vcardstrarray_size(vcardstructured_field_at(jscomps, 3)));
    assert_str_equals("d,e", vcardstrarray_element_at(vcardstructured_field_at(jscomps, 3), 0));
    assert_str_equals("foo", vcardproperty_get_value_as_string(prop));

    prop = vcardcomponent_get_next_property(card, VCARD_X_PROPERTY);
    param = vcardproperty_get_first_parameter(prop, VCARD_JSCOMPS_PARAMETER);
    jscomps = vcardparameter_get_jscomps(param);
    assert(1 == vcardstructured_num_fields(jscomps));
    assert(1 == vcardstrarray_size(vcardstructured_field_at(jscomps, 0)));
    assert_str_equals("a", vcardstrarray_element_at(vcardstructured_field_at(jscomps, 0), 0));
    assert_str_equals("foo", vcardproperty_get_value_as_string(prop));

    prop = vcardcomponent_get_next_property(card, VCARD_X_PROPERTY);
    param = vcardproperty_get_first_parameter(prop, VCARD_JSCOMPS_PARAMETER);
    jscomps = vcardparameter_get_jscomps(param);
    assert(1 == vcardstructured_num_fields(jscomps));
    assert(2 == vcardstrarray_size(vcardstructured_field_at(jscomps, 0)));
    assert_str_equals("a", vcardstrarray_element_at(vcardstructured_field_at(jscomps, 0), 0));
    assert_str_equals("b", vcardstrarray_element_at(vcardstructured_field_at(jscomps, 0), 1));
    assert_str_equals("foo", vcardproperty_get_value_as_string(prop));

    vcardcomponent_free(card);
}

static void test_value_structured(void)
{
    vcardstructuredtype *stt;
    vcardstrarray *sa;
    vcardvalue *val;

    // Set structured value having both fields set.
    stt = vcardstructured_new(2);
    sa = vcardstrarray_new(1);
    vcardstrarray_add(sa, "foo");
    vcardstructured_set_field_at(stt, 0, sa);
    sa = vcardstrarray_new(1);
    vcardstrarray_add(sa, "bar");
    vcardstructured_set_field_at(stt, 1, sa);
    val = vcardvalue_new_structured(stt);
    assert_str_equals("foo;bar", vcardvalue_as_vcard_string(val));
    vcardstructured_unref(stt);
    vcardvalue_free(val);

    // Set structured value having only first field set.
    stt = vcardstructured_new(2);
    sa = vcardstrarray_new(1);
    vcardstrarray_add(sa, "foo");
    vcardstructured_set_field_at(stt, 0, sa);
    val = vcardvalue_new_structured(stt);
    assert_str_equals("foo;", vcardvalue_as_vcard_string(val));
    vcardstructured_unref(stt);
    vcardvalue_free(val);

    // Set structured value having only second field set.
    stt = vcardstructured_new(2);
    sa = vcardstrarray_new(1);
    vcardstrarray_add(sa, "bar");
    vcardstructured_set_field_at(stt, 1, sa);
    val = vcardvalue_new_structured(stt);
    assert_str_equals(";bar", vcardvalue_as_vcard_string(val));
    vcardstructured_unref(stt);
    vcardvalue_free(val);

    // Set structured value having empty field set.
    stt = vcardstructured_new(2);
    vcardstructured_set_field_at(stt, 0, vcardstrarray_new(1));
    vcardstructured_set_field_at(stt, 1, vcardstrarray_new(1));
    val = vcardvalue_new_structured(stt);
    assert_str_equals(";", vcardvalue_as_vcard_string(val));
    vcardstructured_unref(stt);
    vcardvalue_free(val);

    // Set structured value having no field set.
    stt = vcardstructured_new(2);
    val = vcardvalue_new_structured(stt);
    assert_str_equals(";", vcardvalue_as_vcard_string(val));
    vcardstructured_unref(stt);
    vcardvalue_free(val);
}

static void test_value_structured_from_string(void)
{
    vcardstructuredtype *stt;

    // Parse structured value having both fields set.
    stt = vcardstructured_new_from_string("foo;bar");
    assert(vcardstructured_num_fields(stt) == 2);
    assert(vcardstrarray_size(vcardstructured_field_at(stt, 0)) == 1);
    assert_str_equals("foo", vcardstrarray_element_at(vcardstructured_field_at(stt, 0), 0));
    assert(vcardstrarray_size(vcardstructured_field_at(stt, 1)) == 1);
    assert_str_equals("bar", vcardstrarray_element_at(vcardstructured_field_at(stt, 1), 0));
    vcardstructured_unref(stt);

    // Parse structured value having only first field set.
    stt = vcardstructured_new_from_string("foo;");
    assert(vcardstructured_num_fields(stt) == 2);
    assert(vcardstrarray_size(vcardstructured_field_at(stt, 0)) == 1);
    assert_str_equals("foo", vcardstrarray_element_at(vcardstructured_field_at(stt, 0), 0));
    assert(vcardstrarray_size(vcardstructured_field_at(stt, 1)) == 0);
    vcardstructured_unref(stt);

    // Parse structured value having only second field set.
    stt = vcardstructured_new_from_string(";foo");
    assert(vcardstructured_num_fields(stt) == 2);
    assert(vcardstrarray_size(vcardstructured_field_at(stt, 0)) == 0);
    assert(vcardstrarray_size(vcardstructured_field_at(stt, 1)) == 1);
    assert_str_equals("foo", vcardstrarray_element_at(vcardstructured_field_at(stt, 1), 0));
    vcardstructured_unref(stt);

    // Parse structured value having no field set.
    stt = vcardstructured_new_from_string(";");
    assert(vcardstructured_num_fields(stt) == 2);
    assert(vcardstrarray_size(vcardstructured_field_at(stt, 0)) == 0);
    assert(vcardstrarray_size(vcardstructured_field_at(stt, 1)) == 0);
    vcardstructured_unref(stt);

    // Parse structured value having just empty values.
    stt = vcardstructured_new_from_string(",;,");
    assert(vcardstructured_num_fields(stt) == 2);
    assert(vcardstrarray_size(vcardstructured_field_at(stt, 0)) == 2);
    assert_str_equals("", vcardstrarray_element_at(vcardstructured_field_at(stt, 0), 0));
    assert_str_equals("", vcardstrarray_element_at(vcardstructured_field_at(stt, 0), 1));
    assert(vcardstrarray_size(vcardstructured_field_at(stt, 1)) == 2);
    assert_str_equals("", vcardstrarray_element_at(vcardstructured_field_at(stt, 1), 0));
    assert_str_equals("", vcardstrarray_element_at(vcardstructured_field_at(stt, 1), 1));
    vcardstructured_unref(stt);

    // Parse structured value having non-empty and empty values.
    stt = vcardstructured_new_from_string(",foo;bar,");
    assert(vcardstructured_num_fields(stt) == 2);
    assert(vcardstrarray_size(vcardstructured_field_at(stt, 0)) == 2);
    assert_str_equals("", vcardstrarray_element_at(vcardstructured_field_at(stt, 0), 0));
    assert_str_equals("foo", vcardstrarray_element_at(vcardstructured_field_at(stt, 0), 1));
    assert(vcardstrarray_size(vcardstructured_field_at(stt, 1)) == 2);
    assert_str_equals("bar", vcardstrarray_element_at(vcardstructured_field_at(stt, 1), 0));
    assert_str_equals("", vcardstrarray_element_at(vcardstructured_field_at(stt, 1), 1));
    vcardstructured_unref(stt);
}

static void test_value_structured_escaped(void)
{
    vcardstructuredtype *stt = vcardstructured_new(2);
    vcardstrarray *sa;

    sa = vcardstrarray_new(1);
    vcardstrarray_add(sa, "foo,bar");
    vcardstrarray_add(sa, "baz;bam");
    vcardstructured_set_field_at(stt, 0, sa);

    sa = vcardstrarray_new(1);
    vcardstrarray_add(sa, "tux;");
    vcardstrarray_add(sa, "qux,");
    vcardstructured_set_field_at(stt, 1, sa);

    vcardvalue *val = vcardvalue_new_structured(stt);
    assert_str_equals("foo\\,bar,baz\\;bam;tux\\;,qux\\,", vcardvalue_as_vcard_string(val));
    vcardstructured_unref(stt);
    vcardvalue_free(val);
}

static void test_value_structured_from_string_escaped(void)
{
    vcardstructuredtype *stt;

    stt = vcardstructured_new_from_string("foo\\,bar,baz\\;bam;tux\\;,qux\\,");
    assert(vcardstructured_num_fields(stt) == 2);
    assert(vcardstrarray_size(vcardstructured_field_at(stt, 0)) == 2);
    assert(vcardstrarray_size(vcardstructured_field_at(stt, 1)) == 2);
    assert_str_equals("foo,bar", vcardstrarray_element_at(vcardstructured_field_at(stt, 0), 0));
    assert_str_equals("baz;bam", vcardstrarray_element_at(vcardstructured_field_at(stt, 0), 1));
    assert_str_equals("tux;", vcardstrarray_element_at(vcardstructured_field_at(stt, 1), 0));
    assert_str_equals("qux,", vcardstrarray_element_at(vcardstructured_field_at(stt, 1), 1));
    vcardstructured_unref(stt);

    stt = vcardstructured_new_from_string("foo,bar\\");
    assert(vcardstructured_num_fields(stt) == 1);
    assert(vcardstrarray_size(vcardstructured_field_at(stt, 0)) == 2);
    assert_str_equals("foo", vcardstrarray_element_at(vcardstructured_field_at(stt, 0), 0));
    assert_str_equals("bar", vcardstrarray_element_at(vcardstructured_field_at(stt, 0), 1));
    vcardstructured_unref(stt);
}

static void assert_folded_line(const char *line)
{
    size_t len = strlen(line);
    // line ends with CRLF
    assert(len >= 2 && line[len - 2] == '\r' && line[len - 1] == '\n');

    const char *eol = line + len - 2;
    // line isn't empty
    assert(eol != line);

    const char *p = line;
    while (p < eol) {
        // line must not contain stray LF character
        assert(p[0] != '\n');
        // assert folds
        if (p[0] == '\r') {
            // a fold must be followed by a content character
            assert(eol - p > 3 && p[1] == '\n' && (p[2] == ' ' || p[2] == '\t') && p[3] != '\r');
            p += 3;
        } else {
            p++;
        }
    }
}

static void test_line_folding(void)
{
#define TEST_LINE_FOLDING_PREAMBLE                       \
    "PHOTO;VALUE=uri;MEDIATYPE=application/octet-stream" \
    ":https://example.com/aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa/xxxxxxx"

    vcardproperty *prop;

    // regression test: also broken implementation produced valid output
    prop = vcardproperty_new_from_string(TEST_LINE_FOLDING_PREAMBLE);
    assert(prop != NULL);
    assert_folded_line(vcardproperty_as_vcard_string(prop));
    vcardproperty_free(prop);

    // broken implementation ended this with CR CR LF Space LF
    prop = vcardproperty_new_from_string(TEST_LINE_FOLDING_PREAMBLE "x");
    assert(prop != NULL);
    assert_folded_line(vcardproperty_as_vcard_string(prop));
    vcardproperty_free(prop);

    // broken implementation ended this with CR LF Space CR LF
    prop = vcardproperty_new_from_string(TEST_LINE_FOLDING_PREAMBLE "xx");
    assert(prop != NULL);
    assert_folded_line(vcardproperty_as_vcard_string(prop));
    vcardproperty_free(prop);

#undef TEST_LINE_FOLDING_PREAMBLE
}

static void assert_null_str(const char *str)
{
    if (str) {
        fprintf(stderr, "expected NULL, got string of length %zu: %s\n",
                strlen(str), str);
        assert(0);
    }
}

static void test_value_kind_mismatch_structured(void)
{
    /* The N and ADR values mimic a 32-byte vcardstructuredtype_impl, one byte
       short, so a mismatched getter reads its field pointer out of bounds. */
    static const char *input =
        "BEGIN:VCARD\r\n"
        "VERSION:4.0\r\n"
        "FN:x\r\n"
        "N;VALUE=URI:AAAAAAAABBBBBBBBCCCCCCCCBBBBBB\r\n"
        "ADR;VALUE=TEXT:AAAAAAAABBBBBBBBCCCCCCCCBBBBBB\r\n"
        "END:VCARD\r\n";

    vcardcomponent *card = vcardparser_parse_string(input);
    vcardproperty *prop;
    vcardstructuredtype *st;

    prop = vcardcomponent_get_first_property(card, VCARD_N_PROPERTY);
    assert(VCARD_URI_VALUE == vcardvalue_isa(vcardproperty_get_value(prop)));
    st = vcardproperty_get_n(prop);
    assert(NULL == vcardstructured_field_at(st, 0));
    assert(0 == vcardstructured_num_fields(st));
    assert(NULL == st);

    prop = vcardcomponent_get_first_property(card, VCARD_ADR_PROPERTY);
    assert(VCARD_TEXT_VALUE == vcardvalue_isa(vcardproperty_get_value(prop)));
    st = vcardproperty_get_adr(prop);
    vcardstructured_set_num_fields(st, 5);
    assert(0 == vcardstructured_num_fields(st));
    assert(NULL == st);

    vcardcomponent_transform(card, VCARD_VERSION_30);
    assert(NULL != vcardcomponent_as_vcard_string(card));

    vcardcomponent_free(card);
}

static void test_value_kind_mismatch_textlist(void)
{
    /* The CATEGORIES and NICKNAME values mimic a 40-byte struct _icalarray,
       stopping before its chunks pointer. */
    static const char *input =
        "BEGIN:VCARD\r\n"
        "VERSION:4.0\r\n"
        "FN:x\r\n"
        "CATEGORIES;VALUE=URI:AAAAAAAABBBBBBBBCCCCCCCCDDDDDD\r\n"
        "NICKNAME;VALUE=URI:AAAAAAAABBBBBBBBCCCCCCCCDDDDDD\r\n"
        "ORG;VALUE=DATE:19700101\r\n"
        "END:VCARD\r\n";

    vcardcomponent *card = vcardparser_parse_string(input);
    vcardproperty *prop;

    vcardcomponent_normalize(card);

    prop = vcardcomponent_get_first_property(card, VCARD_CATEGORIES_PROPERTY);
    assert(NULL == vcardproperty_get_categories(prop));
    prop = vcardcomponent_get_first_property(card, VCARD_NICKNAME_PROPERTY);
    assert(NULL == vcardproperty_get_nickname(prop));
    prop = vcardcomponent_get_first_property(card, VCARD_ORG_PROPERTY);
    assert(NULL == vcardproperty_get_org(prop));

    vcardcomponent_free(card);
}

static void test_value_kind_mismatch_time(void)
{
    static const char *input =
        "BEGIN:VCARD\r\n"
        "VERSION:4.0\r\n"
        "FN:x\r\n"
        "BDAY;VALUE=URI:urn:x:1\r\n"
        "REV;VALUE=URI:urn:x:2\r\n"
        "CREATED;VALUE=TEXT:not-a-timestamp\r\n"
        "END:VCARD\r\n";

    vcardcomponent *card = vcardparser_parse_string(input);
    vcardproperty *prop;

    vcardcomponent_transform(card, VCARD_VERSION_30);
    assert(NULL != vcardcomponent_as_vcard_string(card));
    vcardcomponent_free(card);

    card = vcardparser_parse_string(input);
    prop = vcardcomponent_get_first_property(card, VCARD_BDAY_PROPERTY);
    assert(vcardtime_is_null_datetime(vcardproperty_get_bday(prop)));
    prop = vcardcomponent_get_first_property(card, VCARD_REV_PROPERTY);
    assert(vcardtime_is_null_datetime(vcardproperty_get_rev(prop)));
    prop = vcardcomponent_get_first_property(card, VCARD_CREATED_PROPERTY);
    assert(vcardtime_is_null_datetime(vcardproperty_get_created(prop)));

    assert(1 == vcardrestriction_check(card));

    vcardcomponent_free(card);
}

static void test_value_kind_mismatch_string(void)
{
    static const char *input =
        "BEGIN:VCARD\r\n"
        "VERSION:4.0\r\n"
        "FN;VALUE=DATE:19700101\r\n"
        "UID;VALUE=DATE:19700101\r\n"
        "LANG;VALUE=DATE:19700101\r\n"
        "NOTE;VALUE=BOOLEAN:TRUE\r\n"
        "END:VCARD\r\n";

    vcardcomponent *card = vcardparser_parse_string(input);
    vcardproperty *prop;

    prop = vcardcomponent_get_first_property(card, VCARD_FN_PROPERTY);
    assert_null_str(vcardproperty_get_fn(prop));
    prop = vcardcomponent_get_first_property(card, VCARD_UID_PROPERTY);
    assert_null_str(vcardproperty_get_uid(prop));
    prop = vcardcomponent_get_first_property(card, VCARD_LANG_PROPERTY);
    assert_null_str(vcardproperty_get_lang(prop));
    prop = vcardcomponent_get_first_property(card, VCARD_NOTE_PROPERTY);
    assert_null_str(vcardproperty_get_note(prop));

    assert_null_str(vcardcomponent_get_uid(card));
    assert_null_str(vcardcomponent_get_fn(card));

    vcardcomponent_free(card);

    card = vcardparser_parse_string(
        "BEGIN:VCARD\r\n"
        "VERSION:3.0\r\n"
        "FN:x\r\n"
        "UID:probe-uid\r\n"
        "UID;VALUE=DATE:19700101\r\n"
        "END:VCARD\r\n");

    vcardcomponent_transform(card, VCARD_VERSION_30);
    assert(NULL != vcardcomponent_as_vcard_string(card));
    vcardcomponent_transform(card, VCARD_VERSION_40);
    assert(NULL != vcardcomponent_as_vcard_string(card));

    vcardcomponent_free(card);
}

static void test_value_kind_mismatch_scalar(void)
{
    static const char *input =
        "BEGIN:VCARD\r\n"
        "VERSION:4.0\r\n"
        "FN:x\r\n"
        "KIND;VALUE=URI:urn:x:1\r\n"
        "GRAMGENDER;VALUE=URI:urn:x:2\r\n"
        "GEO;VALUE=BOOLEAN:TRUE\r\n"
        "TZ;VALUE=DATE:19700101\r\n"
        "END:VCARD\r\n";

    vcardcomponent *card = vcardparser_parse_string(input);
    vcardproperty *prop;

    prop = vcardcomponent_get_first_property(card, VCARD_KIND_PROPERTY);
    assert(VCARD_KIND_NONE == vcardproperty_get_kind(prop));
    prop = vcardcomponent_get_first_property(card, VCARD_GRAMGENDER_PROPERTY);
    assert(VCARD_GRAMGENDER_NONE == vcardproperty_get_gramgender(prop));

    prop = vcardcomponent_get_first_property(card, VCARD_GEO_PROPERTY);
    vcardgeotype geo = vcardproperty_get_geo(prop);
    assert(NULL == geo.uri && '\0' == geo.coords.lat[0]);

    prop = vcardcomponent_get_first_property(card, VCARD_TZ_PROPERTY);
    vcardtztype tz = vcardproperty_get_tz(prop);
    assert(NULL == tz.tzid && NULL == tz.uri && 0 == tz.utcoffset);

    vcardcomponent_free(card);

    card = vcardparser_parse_string(
        "BEGIN:VCARD\r\n"
        "VERSION:3.0\r\n"
        "FN:x\r\n"
        "GEO;VALUE=BOOLEAN:TRUE\r\n"
        "TZ;VALUE=INTEGER:12345\r\n"
        "END:VCARD\r\n");

    vcardcomponent_transform(card, VCARD_VERSION_40);
    assert(NULL != vcardcomponent_as_vcard_string(card));

    vcardcomponent_free(card);
}

static void test_value_kind_mismatch_x(void)
{
    static const char *input =
        "BEGIN:VCARD\r\n"
        "VERSION:4.0\r\n"
        "FN:x\r\n"
        "X-PROP;VALUE=DATE:19700101\r\n"
        "END:VCARD\r\n";

    vcardcomponent *card = vcardparser_parse_string(input);
    vcardproperty *prop =
        vcardcomponent_get_first_property(card, VCARD_X_PROPERTY);
    vcardvalue *val = vcardproperty_get_value(prop);

    assert(VCARD_DATE_VALUE == vcardvalue_isa(val));
    assert_null_str(vcardvalue_get_x(val));
    assert_null_str(vcardvalue_get_text(val));
    assert(NULL == vcardvalue_get_textlist(val));

    vcardcomponent_free(card);
}

static void test_value_kind_enum_x(void)
{
    static const char *input =
        "BEGIN:VCARD\r\n"
        "VERSION:4.0\r\n"
        "FN:x\r\n"
        "KIND:x-my-kind\r\n"
        "END:VCARD\r\n";

    vcardcomponent *card;
    vcardproperty *prop;
    vcardvalue *val;

    /* An unrecognized KIND value keeps the token verbatim in x_value */
    card = vcardparser_parse_string(input);
    prop = vcardcomponent_get_first_property(card, VCARD_KIND_PROPERTY);
    val = vcardproperty_get_value(prop);

    assert(VCARD_KIND_VALUE == vcardvalue_isa(val));
    assert(VCARD_KIND_X == vcardvalue_get_kind(val));
    assert(VCARD_KIND_X == vcardproperty_get_kind(prop));
    assert(NULL != vcardvalue_get_x(val));
    assert_str_equals("x-my-kind", vcardvalue_get_x(val));
    assert_str_equals("KIND:x-my-kind\r\n", vcardproperty_as_vcard_string(prop));
    assert_str_equals(input, vcardcomponent_as_vcard_string(card));

    vcardcomponent_free(card);

    /* A recognized KIND value does not use x_value */
    val = vcardvalue_new_from_string(VCARD_KIND_VALUE, "individual");
    assert(VCARD_KIND_VALUE == vcardvalue_isa(val));
    assert(VCARD_KIND_INDIVIDUAL == vcardvalue_get_kind(val));
    assert_null_str(vcardvalue_get_x(val));
    assert_str_equals("INDIVIDUAL", vcardvalue_as_vcard_string(val));
    vcardvalue_free(val);

    /* vcardvalue_set_x() and _get_x() also apply to enum-kind values */
    val = vcardvalue_new_from_string(VCARD_KIND_VALUE, "x-my-kind");
    assert(NULL != vcardvalue_get_x(val));
    assert_str_equals("x-my-kind", vcardvalue_get_x(val));
    vcardvalue_set_x(val, "x-other-kind");
    assert_str_equals("x-other-kind", vcardvalue_get_x(val));
    assert_str_equals("x-other-kind", vcardvalue_as_vcard_string(val));
    vcardvalue_free(val);
}

static void test_value_kind_no_value(void)
{
    vcardproperty *prop = vcardproperty_new(VCARD_N_PROPERTY);

    assert(NULL == vcardproperty_get_value(prop));
    assert(NULL == vcardproperty_get_n(prop));
    assert(vcardtime_is_null_datetime(vcardproperty_get_bday(prop)));
    assert_null_str(vcardproperty_get_fn(prop));

    vcardproperty_free(prop);
}

static void test_value_kind_compatible(void)
{
    static const char *v3 =
        "BEGIN:VCARD\r\n"
        "VERSION:3.0\r\n"
        "FN:x\r\n"
        "UID:foo-bar\r\n"
        "PHOTO;ENCODING=b:QUFB\r\n"
        "TZ:-0500\r\n"
        "GEO:1.5;2.5\r\n"
        "BDAY:19700101\r\n"
        "REV:19700101T000000Z\r\n"
        "END:VCARD\r\n";
    static const char *v4 =
        "BEGIN:VCARD\r\n"
        "VERSION:4.0\r\n"
        "FN:x\r\n"
        "UID:urn:uuid:1\r\n"
        "TEL;VALUE=URI:tel:+1-418-656-9254\r\n"
        "ANNIVERSARY;VALUE=TIMESTAMP:20090808T143000-0500\r\n"
        "BDAY:19700101\r\n"
        "TZ:Europe/Berlin\r\n"
        "GEO:geo:1.5,2.5\r\n"
        "END:VCARD\r\n";

    vcardcomponent *card;
    vcardproperty *prop;
    vcardtimetype t;

    card = vcardparser_parse_string(v3);

    prop = vcardcomponent_get_first_property(card, VCARD_UID_PROPERTY);
    assert(VCARD_TEXT_VALUE == vcardvalue_isa(vcardproperty_get_value(prop)));
    assert_str_equals("foo-bar", vcardproperty_get_uid(prop));

    prop = vcardcomponent_get_first_property(card, VCARD_PHOTO_PROPERTY);
    assert(VCARD_TEXT_VALUE == vcardvalue_isa(vcardproperty_get_value(prop)));
    assert_str_equals("QUFB", vcardproperty_get_photo(prop));

    prop = vcardcomponent_get_first_property(card, VCARD_TZ_PROPERTY);
    assert(VCARD_UTCOFFSET_VALUE == vcardvalue_isa(vcardproperty_get_value(prop)));
    assert(-18000 == vcardproperty_get_tz(prop).utcoffset);

    prop = vcardcomponent_get_first_property(card, VCARD_GEO_PROPERTY);
    assert(VCARD_GEO_VALUE == vcardvalue_isa(vcardproperty_get_value(prop)));
    vcardgeotype geo = vcardproperty_get_geo(prop);
    assert_str_equals("1.5", geo.coords.lat);

    prop = vcardcomponent_get_first_property(card, VCARD_BDAY_PROPERTY);
    assert(VCARD_DATE_VALUE == vcardvalue_isa(vcardproperty_get_value(prop)));
    t = vcardproperty_get_bday(prop);
    assert(1970 == t.year && 1 == t.month && 1 == t.day);

    prop = vcardcomponent_get_first_property(card, VCARD_REV_PROPERTY);
    assert(VCARD_TIMESTAMP_VALUE == vcardvalue_isa(vcardproperty_get_value(prop)));
    assert(1970 == vcardproperty_get_rev(prop).year);

    assert_str_equals("foo-bar", vcardcomponent_get_uid(card));
    assert_str_equals("x", vcardcomponent_get_fn(card));

    vcardcomponent_free(card);

    card = vcardparser_parse_string(v4);

    prop = vcardcomponent_get_first_property(card, VCARD_UID_PROPERTY);
    assert(VCARD_URI_VALUE == vcardvalue_isa(vcardproperty_get_value(prop)));
    assert_str_equals("urn:uuid:1", vcardproperty_get_uid(prop));

    prop = vcardcomponent_get_first_property(card, VCARD_TEL_PROPERTY);
    assert(VCARD_URI_VALUE == vcardvalue_isa(vcardproperty_get_value(prop)));
    assert_str_equals("tel:+1-418-656-9254", vcardproperty_get_tel(prop));

    prop = vcardcomponent_get_first_property(card, VCARD_ANNIVERSARY_PROPERTY);
    assert(VCARD_TIMESTAMP_VALUE == vcardvalue_isa(vcardproperty_get_value(prop)));
    t = vcardproperty_get_anniversary(prop);
    assert(2009 == t.year && 8 == t.month && 8 == t.day);

    prop = vcardcomponent_get_first_property(card, VCARD_TZ_PROPERTY);
    assert(VCARD_TEXT_VALUE == vcardvalue_isa(vcardproperty_get_value(prop)));
    assert_str_equals("Europe/Berlin", vcardproperty_get_tz(prop).tzid);

    prop = vcardcomponent_get_first_property(card, VCARD_GEO_PROPERTY);
    assert(VCARD_URI_VALUE == vcardvalue_isa(vcardproperty_get_value(prop)));
    assert_str_equals("geo:1.5,2.5", vcardproperty_get_geo(prop).uri);

    vcardcomponent_free(card);
}

int main(int argc, char **argv)
{
    _unused(argc);
    _unused(argv);

    test_prop_text();
    test_prop_structured();
    test_prop_multivalued();
    test_prop_x();
    test_prop_x_value_kind();
    test_prop_x_structured();

    test_param_singlevalued();
    test_param_multivalued();
    test_param_structured();

    test_value_structured();
    test_value_structured_from_string();
    test_value_structured_escaped();
    test_value_structured_from_string_escaped();

    test_line_folding();

    test_value_kind_mismatch_structured();
    test_value_kind_mismatch_textlist();
    test_value_kind_mismatch_time();
    test_value_kind_mismatch_string();
    test_value_kind_mismatch_scalar();
    test_value_kind_mismatch_x();
    test_value_kind_enum_x();
    test_value_kind_no_value();
    test_value_kind_compatible();

    return 0;
}
