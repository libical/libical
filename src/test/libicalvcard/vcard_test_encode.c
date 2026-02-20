/*======================================================================
 FILE: vcard_test_encode.c
 CREATOR: Robert Stepanek 24 Aug 2022 <rsto@fastmailteam.com>

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 ======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
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

static vcardvalue_kind my_xprop_value_kind_func(const char *name, void *data)
{
    (void)(data); // make CI happy, reporting unused parameter otherwise
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

    return 0;
}
