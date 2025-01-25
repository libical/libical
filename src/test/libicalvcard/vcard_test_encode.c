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

#define assert_str_equals(want, have)                                                                                  \
    {                                                                                                                  \
        const char *_w = (want);                                                                                       \
        const char *_h = (have);                                                                                       \
        int _v = strcmp(_w, _h);                                                                                       \
        if (_v) {                                                                                                      \
            fprintf(stderr, "line %d: string mismatch\n want=%s\n have=%s\n", __LINE__, _w, _h);                       \
            assert(0);                                                                                                 \
        }                                                                                                              \
    }

static void test_prop_text(void)
{
    static const char *input = "BEGIN:VCARD\r\n"
                               "VERSION:3.0\r\n"
                               "FN:x\\,\\\\;\\;,;\\n\\N\r\n"
                               "END:VCARD\r\n";

    vcardcomponent *card = vcardparser_parse_string(input);

    vcardproperty *prop = vcardcomponent_get_first_property(card, VCARD_FN_PROPERTY);
    assert_str_equals("x,\\;;,;\n\n", vcardproperty_get_fn(prop));
    assert_str_equals("FN:x\\,\\\\\\;\\;\\,\\;\\n\\n\r\n", vcardproperty_as_vcard_string(prop));

    vcardcomponent_free(card);
}

static void test_prop_structured(void)
{
    static const char *input = "BEGIN:VCARD\r\n"
                               "VERSION:3.0\r\n"
                               "N:y,\\,\\\\;\\;,\\n\\N\r\n"
                               "END:VCARD\r\n";

    vcardcomponent *card = vcardparser_parse_string(input);

    vcardproperty *prop = vcardcomponent_get_first_property(card, VCARD_N_PROPERTY);
    vcardstructuredtype *n = vcardproperty_get_n(prop);
    assert(2 == n->num_fields);
    assert(2 == n->field[0]->num_elements);
    assert_str_equals("y", vcardstrarray_element_at(n->field[0], 0));
    assert_str_equals(",\\", vcardstrarray_element_at(n->field[0], 1));
    assert(2 == n->field[1]->num_elements);
    assert_str_equals(";", vcardstrarray_element_at(n->field[1], 0));
    assert_str_equals("\n\n", vcardstrarray_element_at(n->field[1], 1));
    assert_str_equals("N:y,\\,\\\\;\\;,\\n\\n\r\n", vcardproperty_as_vcard_string(prop));

    vcardcomponent_free(card);
}

static void test_prop_multivalued(void)
{
    static const char *input = "BEGIN:VCARD\r\n"
                               "VERSION:3.0\r\n"
                               "CATEGORIES:\\,,\\N,;\\;\r\n" // separated by comma
                               "ORG:\\,;\\;;x\r\n"           // separated by semicolon
                               "END:VCARD\r\n";

    vcardcomponent *card = vcardparser_parse_string(input);

    vcardproperty *prop = vcardcomponent_get_first_property(card, VCARD_CATEGORIES_PROPERTY);
    vcardstrarray *categories = vcardproperty_get_categories(prop);
    assert(3 == categories->num_elements);
    assert_str_equals(",", vcardstrarray_element_at(categories, 0));
    assert_str_equals("\n", vcardstrarray_element_at(categories, 1));
    assert_str_equals(";;", vcardstrarray_element_at(categories, 2));
    assert_str_equals("CATEGORIES:\\,,\\n,\\;\\;\r\n", vcardproperty_as_vcard_string(prop));

    prop = vcardcomponent_get_first_property(card, VCARD_ORG_PROPERTY);
    vcardstrarray *org = vcardproperty_get_org(prop);
    assert(3 == org->num_elements);
    assert_str_equals(",", vcardstrarray_element_at(org, 0));
    assert_str_equals(";", vcardstrarray_element_at(org, 1));
    assert_str_equals("x", vcardstrarray_element_at(org, 2));

    vcardcomponent_free(card);
}

static void test_param_singlevalued(void)
{
    static const char *input = "BEGIN:VCARD\r\n"
                               "VERSION:3.0\r\n"
                               "X-PROP;TZ=^^^',^x^n:foo\r\n"
                               "END:VCARD\r\n";

    vcardcomponent *card = vcardparser_parse_string(input);

    vcardproperty *prop = vcardcomponent_get_first_property(card, VCARD_X_PROPERTY);
    vcardparameter *param = vcardproperty_get_first_parameter(prop, VCARD_TZ_PARAMETER);

    assert_str_equals("^\",^x\n", vcardparameter_get_tz(param));
    // quote and escape "^x" to "^^x"
    assert_str_equals("TZ=\"^^^',^^x^n\"", vcardparameter_as_vcard_string(param));

    vcardcomponent_free(card);
}

static void test_param_multivalued(void)
{
    static const char *input = "BEGIN:VCARD\r\n"
                               "VERSION:3.0\r\n"
                               "N;SORT-AS=^n^^^',^^:foo;bar,baz;;"
                               "END:VCARD\r\n";

    vcardcomponent *card = vcardparser_parse_string(input);

    vcardproperty *prop = vcardcomponent_get_first_property(card, VCARD_N_PROPERTY);
    vcardparameter *param = vcardproperty_get_first_parameter(prop, VCARD_SORTAS_PARAMETER);
    vcardstrarray *sortas = vcardparameter_get_sortas(param);

    assert(2 == sortas->num_elements);
    assert_str_equals("\n^\"", vcardstrarray_element_at(sortas, 0));
    assert_str_equals("^", vcardstrarray_element_at(sortas, 1));
    assert_str_equals("SORT-AS=^n^^^',^^", vcardparameter_as_vcard_string(param));

    vcardcomponent_free(card);
}

int main(int argc, char **argv)
{
    _unused(argc);
    _unused(argv);

    test_prop_text();
    test_prop_structured();
    test_prop_multivalued();

    test_param_singlevalued();
    test_param_multivalued();

    return 0;
}
