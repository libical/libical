/*======================================================================
 FILE: vcard_test.c
 CREATOR: Ken Murchison 24 Aug 2022 <murch@fastmailteam.com>

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
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

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

void strip_errors(vcardcomponent *comp)
{
    vcardproperty *prop, *next;

    for (prop = vcardcomponent_get_first_property(comp, VCARD_XLICERROR_PROPERTY);
         prop; prop = next) {
        next = vcardcomponent_get_next_property(comp, VCARD_XLICERROR_PROPERTY);
        vcardcomponent_remove_property(comp, prop);
        vcardproperty_free(prop);
    }
}

static bool test_parse_file(const char *fname)
{
    FILE *fp;
    int fd, r;
#if defined(HAVE__FSTAT64)
    struct _stat64 sbuf;
#else
    struct stat sbuf;
#endif
    size_t filesize;
    void *data = NULL;
    vcardcomponent *card;
    //krazy:cond=insecurenet
    const char *want =
        "BEGIN:VCARD\r\n"
        "VERSION:4.0\r\n"
        "FN:Simon Perreault\r\n"
        "N:Perreault;Simon;;;ing. jr,M.Sc.\r\n"
        "BDAY:--0203\r\n"
        "BDAY:0000\r\n"
        "ANNIVERSARY;VALUE=TIMESTAMP:20090808T143000-0500\r\n"
        "GENDER:M;manly\r\n"
        "ADR;TYPE=WORK:;Suite D2-630;2875 Laurier;Quebec;QC;G1V 2M2;Canada\r\n"
        "TEL;VALUE=URI;TYPE=WORK,TEXT,VOICE,CELL,VIDEO,bar,foo:tel:\r\n"
        " +1-418-262-6501\r\n"
        "TEL;VALUE=URI:tel:+1-418-656-9254;ext=102\r\n"
        "EMAIL;TYPE=WORK:simon.perreault@viagenie.ca\r\n"
        "LANG;PREF=2:en\r\n"
        "LANG;PREF=1:fr\r\n"
        "TZ;VALUE=UTC-OFFSET:-05\r\n"
        "GEO;TYPE=WORK:geo:46.772673,-71.282945\r\n"
        "ORG;TYPE=WORK:Viagenie;Foo\r\n"
        "CATEGORIES:bar,foo\r\n"
        "NOTE;LANGUAGE=en;PID=1.0,3:Test vCard\r\n"
        "URL;TYPE=HOME:http://nomis80.org\r\n"
        "KEY;TYPE=WORK:http://www.viagenie.ca/simon.perreault/simon.asc\r\n"
        "X-LIC-ERROR;X-LIC-ERRORTYPE=RESTRICTION-CHECK:Failed restrictions for \r\n"
        " BDAY property. Expected zero or one instances of the property and got 2\r\n"
        "END:VCARD\r\n"
        "BEGIN:VCARD\r\n"
        "FN:Mickey Mouse\r\n"
        "X-LIC-ERROR;X-LIC-ERRORTYPE=RESTRICTION-CHECK:Failed restrictions for N \r\n"
        " property. Expected 1 instances of the property and got 0\r\n"
        "X-LIC-ERROR;X-LIC-ERRORTYPE=RESTRICTION-CHECK:Failed restrictions for \r\n"
        " VERSION property. Expected 1 instances of the property and got 0\r\n"
        "END:VCARD\r\n";
    //krazy:endcond=insecurenet
    fp = fopen(fname, "rb"); //on Windows, must open in binary mode
    if (fp == (FILE *)NULL) {
        fprintf(stderr, "Error: unable to open %s\n", fname);
        return false;
    }
    fd = fileno(fp);
    if (fstat(fd, &sbuf) != 0) {
        fprintf(stderr, "Error: unable to stat %s\n", fname);
        fclose(fp);
        return false;
    }
    filesize = (size_t)sbuf.st_size;
    data = malloc(filesize + 1);
    if (!data) {
        fprintf(stderr, "Error: unable to allocate memory\n");
        free(data);
        fclose(fp);
        return false;
    }
    memset(data, 0, filesize + 1);

    r = read(fd, data, filesize);
    fclose(fp);

    if (r < 0) {
        fprintf(stderr, "Error: Failed to read vCard\n");
        free(data);
        return false;
    }

    card = vcardparser_parse_string(data);
    free(data);

    if (card == NULL) {
        fprintf(stderr, "Error: Failed to parse vCard\n");
        return false;
    }

    vcardrestriction_check(card);
    vcardcomponent_normalize(card);
    assert_str_equals(want, vcardcomponent_as_vcard_string(card));
    vcardcomponent_free(card);

    return true;
}

static vcardcomponent *test_comp_vanew(void)
{
    vcardcomponent *card;
    const char *want =
        "BEGIN:VCARD\r\n"
        "VERSION:4.0\r\n"
        "X-LIC-ERROR;X-LIC-ERRORTYPE=RESTRICTION-CHECK:Failed restrictions for FN \r\n"
        " property. Expected one or more instances of the property and got 0\r\n"
        "END:VCARD\r\n";

    card = vcardcomponent_vanew(VCARD_VCARD_COMPONENT,
                                vcardproperty_new_version(VCARD_VERSION_40),
                                vcardproperty_new_kind(VCARD_KIND_INDIVIDUAL),
                                (void *)0);

    if (card == NULL) {
        fprintf(stderr, "Failed to create vCard\n");
        return 0;
    }

    vcardrestriction_check(card);
    vcardcomponent_normalize(card);
    assert_str_equals(want, vcardcomponent_as_vcard_string(card));
    strip_errors(card);

    return card;
}

static void test_add_props(vcardcomponent *card)
{
    const char *want =
        "BEGIN:VCARD\r\n"
        "VERSION:4.0\r\n"
        "FN:Mickey Mouse\r\n"
        "N:Mouse;Mickey;;;;;\r\n"
        "PHOTO:data:image/jpeg;base64,ABCDEF\r\n"
        "BDAY:--1118T03\r\n"
        "ADR:;;123 Main Street,Disney World;Orlando;FL;32836;USA;;;;;;;;;;;\r\n"
        "TEL;VALUE=URI:tel:+1-888-555-1212\r\n"
        "LANG;PREF=1:en\r\n"
        "LANG;PREF=2:fr\r\n"
        "TZ;VALUE=UTC-OFFSET:-0230\r\n"
        "GEO:geo:46.772673,-71.282945\r\n"
        "LOGO;MEDIATYPE=image/png:https://example.com/logo.png\r\n"
        "CATEGORIES:aaa,zzz\r\n"
        "group1.NOTE;LANGUAGE=en;PID=1,3;SORT-AS=bar,foo;TYPE=WORK:Test vCard\r\n"
        "UID;VALUE=TEXT:foo-bar\r\n"
        "END:VCARD\r\n";

    /* Create and add NOTE property */
    vcardstrarray *sa = vcardstrarray_new(10);
    vcardstrarray_append(sa, "1");
    vcardstrarray_append(sa, "2");
    vcardstrarray_append(sa, "3");
    vcardstrarray_add(sa, "3");
    vcardstrarray_remove_element_at(sa, 1);

    vcardproperty *prop =
        vcardproperty_vanew_note("Test vCard",
                                 vcardparameter_new_language("en"),
                                 vcardparameter_new_pid_list(sa),
                                 (void *)0);
    vcardcomponent_add_property(card, prop);
    vcardproperty_set_group(prop, "group1");

    vcardparameter *param = vcardparameter_new(VCARD_TYPE_PARAMETER);
    vcardenumarray_element e = {VCARD_TYPE_WORK, NULL};
    vcardparameter_add_type(param, &e);
    vcardproperty_add_parameter(prop, param);

    param = vcardparameter_new(VCARD_SORTAS_PARAMETER);
    vcardparameter_add_sortas(param, "foo");
    vcardparameter_add_sortas(param, "bar");
    vcardproperty_add_parameter(prop, param);

    /* Create and add FN property */
    prop = vcardproperty_new_fn("Mickey Mouse");
    vcardcomponent_add_property(card, prop);

    /* Create and add N property */
    vcardstructuredtype *name = vcardstructured_new(VCARD_NUM_N_FIELDS);
    sa = vcardstrarray_new(1);
    vcardstrarray_append(sa, "Mouse");
    vcardstructured_set_field_at(name, VCARD_N_FAMILY, sa);
    sa = vcardstrarray_new(1);
    vcardstrarray_append(sa, "Mickey");
    vcardstructured_set_field_at(name, VCARD_N_GIVEN, sa);
    prop = vcardproperty_new_n(name);
    vcardcomponent_add_property(card, prop);
    vcardstructured_unref(name);

    /* Create and add ADR property */
    vcardstructuredtype *address = vcardstructured_new(VCARD_NUM_ADR_FIELDS);
    sa = vcardstrarray_new(1);
    vcardstrarray_append(sa, "123 Main Street");
    vcardstrarray_append(sa, "Disney World");
    vcardstructured_set_field_at(address, VCARD_ADR_STREET, sa);
    sa = vcardstrarray_new(1);
    vcardstrarray_append(sa, "Orlando");
    vcardstructured_set_field_at(address, VCARD_ADR_LOCALITY, sa);
    sa = vcardstrarray_new(1);
    vcardstrarray_append(sa, "FL");
    vcardstructured_set_field_at(address, VCARD_ADR_REGION, sa);
    sa = vcardstrarray_new(1);
    vcardstrarray_append(sa, "32836");
    vcardstructured_set_field_at(address, VCARD_ADR_POSTAL_CODE, sa);
    sa = vcardstrarray_new(1);
    vcardstrarray_append(sa, "USA");
    vcardstructured_set_field_at(address, VCARD_ADR_COUNTRY, sa);
    prop = vcardproperty_new_adr(address);
    vcardcomponent_add_property(card, prop);
    vcardstructured_unref(address);

    /* Create and add CATEGORIES property */
    sa = vcardstrarray_new(2);
    vcardstrarray_append(sa, "zzz");
    vcardstrarray_append(sa, "aaa");
    prop = vcardproperty_new_categories(sa);
    vcardcomponent_add_property(card, prop);

    /* Create and add BDAY property */
    vcardtimetype t = vcardtime_null_date();
    t.month = 11;
    t.day = 18;
    t.hour = 3;
    prop = vcardproperty_new_bday(t);
    vcardcomponent_add_property(card, prop);

    /* Create and add TZ property */
    vcardtztype tz = {.utcoffset = -9000};
    prop = vcardproperty_new_tz(tz);
    vcardcomponent_add_property(card, prop);

    /* Create and add GEO property */
    vcardgeotype geo = {.uri = "geo:46.772673,-71.282945"};
    prop = vcardproperty_new_geo(geo);
    vcardcomponent_add_property(card, prop);

    /* Create and add PHOTO property */
    prop = vcardproperty_new_photo("data:image/jpeg;base64,ABCDEF");
    vcardcomponent_add_property(card, prop);

    /* Create and add LOGO property */
    prop = vcardproperty_vanew_logo("https://example.com/logo.png",
                                    vcardparameter_new_mediatype("image/png"),
                                    (void *)0);
    vcardcomponent_add_property(card, prop);

    /* Create and add UID property */
    prop = vcardproperty_vanew_uid("foo-bar",
                                   vcardparameter_new_value(VCARD_VALUE_TEXT),
                                   (void *)0);
    vcardcomponent_add_property(card, prop);

    /* Create and add TEL property */
    prop = vcardproperty_vanew_tel("tel:+1-888-555-1212",
                                   vcardparameter_new_value(VCARD_VALUE_URI),
                                   (void *)0);
    vcardcomponent_add_property(card, prop);

    /* Create and add LANG properties */
    prop = vcardproperty_vanew_lang("fr",
                                    vcardparameter_new_pref(2),
                                    (void *)0);
    vcardcomponent_add_property(card, prop);

    prop = vcardproperty_vanew_lang("en",
                                    vcardparameter_new_pref(1),
                                    (void *)0);
    vcardcomponent_add_property(card, prop);

    vcardrestriction_check(card);
    vcardcomponent_normalize(card);
    assert_str_equals(want, vcardcomponent_as_vcard_string(card));
}

static void test_n_restriction(vcardcomponent *card)
{
    vcardproperty *prop;
    const char *want =
        "BEGIN:VCARD\r\n"
        "VERSION:3.0\r\n"
        "FN:Mickey Mouse\r\n"
        "PHOTO;ENCODING=B;TYPE=JPEG:ABCDEF\r\n"
        "BDAY;X-APPLE-OMIT-YEAR=1604:16041118T030000\r\n"
        "ADR:;;123 Main Street,Disney World;Orlando;FL;32836;USA;;;;;;;;;;;\r\n"
        "TEL:+1-888-555-1212\r\n"
        "LANG;PREF=1;TYPE=PREF:en\r\n"
        "LANG;PREF=2:fr\r\n"
        "TZ:-02:30\r\n"
        "GEO:46.772673;-71.282945\r\n"
        "LOGO;VALUE=URI;TYPE=PNG:https://example.com/logo.png\r\n"
        "CATEGORIES:aaa,zzz\r\n"
        "group1.NOTE;LANGUAGE=en;PID=1,3;SORT-AS=bar,foo;TYPE=WORK:Test vCard\r\n"
        "UID:foo-bar\r\n"
        "X-LIC-ERROR;X-LIC-ERRORTYPE=RESTRICTION-CHECK:Failed restrictions for N \r\n"
        " property. Expected 1 instances of the property and got 0\r\n"
        "END:VCARD\r\n";

    /* Remove N property */
    prop = vcardcomponent_get_first_property(card, VCARD_N_PROPERTY);
    vcardcomponent_remove_property(card, prop);
    vcardproperty_free(prop);

    vcardcomponent_transform(card, VCARD_VERSION_30);

    vcardrestriction_check(card);
    assert_str_equals(want, vcardcomponent_as_vcard_string(card));
    strip_errors(card);
}

static void test_v3_to_v4(vcardcomponent *card)
{
    const char *want =
        "BEGIN:VCARD\r\n"
        "VERSION:4.0\r\n"
        "FN:Mickey Mouse\r\n"
        "PHOTO:data:image/jpeg;base64,ABCDEF\r\n"
        "BDAY:--1118T030000\r\n"
        "ADR:;;123 Main Street,Disney World;Orlando;FL;32836;USA;;;;;;;;;;;\r\n"
        "TEL:+1-888-555-1212\r\n"
        "LANG;PREF=1:en\r\n"
        "LANG;PREF=2:fr\r\n"
        "TZ;VALUE=UTC-OFFSET:-0230\r\n"
        "GEO:geo:46.772673,-71.282945\r\n"
        "LOGO;MEDIATYPE=image/png:https://example.com/logo.png\r\n"
        "CATEGORIES:aaa,zzz\r\n"
        "group1.NOTE;LANGUAGE=en;PID=1,3;SORT-AS=bar,foo;TYPE=WORK:Test vCard\r\n"
        "UID;VALUE=TEXT:foo-bar\r\n"
        "END:VCARD\r\n";

    vcardcomponent_transform(card, VCARD_VERSION_40);

    assert_str_equals(want, vcardcomponent_as_vcard_string(card));
}

static void test_ignore_backslash_at_eol(void)
{
    const char *str =
        "BEGIN:VCARD\r\n"
        "VERSION:3.0\r\n"
        "FN:foo\r\n"
        // unescaped backslash before CRLF
        "NOTE:test1\\\r\n"
        // unescaped backslash followed by line folds before CRLF
        "NOTE:test2\\\r\n\t\r\n \r\n"
        // escaped backslash before CRLF
        "NOTE:test3\\\\\r\n"
        // escaped backslash, separated by line folds before CRLF
        "NOTE:test4\\\r\n\t\r\n \\\r\n"
        // escaped newline, separated by line folds before CRLF
        "NOTE:test5\\\r\n\t\r\n n\r\n"
        "END:VCARD\r\n";

    vcardcomponent *vcard = vcardcomponent_new_from_string(str);
    assert(vcard);
    vcardproperty *prop;

    prop = vcardcomponent_get_first_property(vcard, VCARD_NOTE_PROPERTY);
    assert_str_equals("test1",
                      vcardvalue_as_vcard_string(vcardproperty_get_value(prop)));

    prop = vcardcomponent_get_next_property(vcard, VCARD_NOTE_PROPERTY);
    assert_str_equals("test2",
                      vcardvalue_as_vcard_string(vcardproperty_get_value(prop)));

    prop = vcardcomponent_get_next_property(vcard, VCARD_NOTE_PROPERTY);
    assert_str_equals("test3\\\\",
                      vcardvalue_as_vcard_string(vcardproperty_get_value(prop)));

    prop = vcardcomponent_get_next_property(vcard, VCARD_NOTE_PROPERTY);
    assert_str_equals("test4\\\\",
                      vcardvalue_as_vcard_string(vcardproperty_get_value(prop)));

    prop = vcardcomponent_get_next_property(vcard, VCARD_NOTE_PROPERTY);
    assert_str_equals("test5\\n",
                      vcardvalue_as_vcard_string(vcardproperty_get_value(prop)));

    vcardcomponent_free(vcard);
}

int main(int argc, const char **argv)
{
    vcardcomponent *card;
    const char *file;

    if (argc != 2) {
        file = TEST_DATADIR "/test.vcf";
    } else {
        file = argv[1];
    }

    if (!test_parse_file(file)) {
        return 1;
    }
    card = test_comp_vanew();
    if (!card) {
        return 1;
    }
    test_add_props(card);
    test_n_restriction(card);
    test_v3_to_v4(card);
    test_ignore_backslash_at_eol();

    vcardcomponent_free(card);

    return 0;
}
