/*======================================================================
 FILE: vcard_test.c

 CREATOR: Ken Murchison 24 Aug 2022 <murch@fastmailteam.com>

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (http://fastmail.com)

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

#define assert_str_equals(want, have) \
{ \
    const char *_w = (want); \
    const char *_h = (have); \
    int _v = strcmp(_w, _h); \
    if (_v) { fprintf(stderr, "line %d: string mismatch\n want=%s\n have=%s\n", __LINE__, _w, _h); assert(0); } \
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

static void test_parse_file(const char *fname)
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
        "BDAY;VALUE=DATE:--0203\r\n"
        "BDAY;VALUE=DATE:--0203\r\n"
        "ANNIVERSARY;VALUE=TIMESTAMP:20090808T143000-0500\r\n"
        "GENDER:M;manly\r\n"
        "ADR;TYPE=WORK:;Suite D2-630;2875 Laurier;Quebec;QC;G1V 2M2;Canada\r\n"
        "TEL;VALUE=URI;TYPE=WORK,TEXT,VOICE,CELL,VIDEO,bar,foo:tel:\r\n"
        " +1-418-262-6501\r\n"
        "TEL;VALUE=URI:tel:+1-418-656-9254;ext=102\r\n"
        "EMAIL;TYPE=WORK:simon.perreault@viagenie.ca\r\n"
        "LANG;PREF=2:en\r\n"
        "LANG;PREF=1:fr\r\n"
        "TZ;VALUE=TEXT:-0500\r\n"
        "GEO;TYPE=WORK:geo:46.772673,-71.282945\r\n"
        "ORG;TYPE=WORK:Viagenie;Foo\r\n"
        "CATEGORIES:bar,foo\r\n"
        "NOTE;LANGUAGE=en;PID=1.0,3:Test vCard\r\n"
        "URL;TYPE=HOME:http://nomis80.org\r\n"
        "KEY;VALUE=URI;TYPE=WORK:http://www.viagenie.ca/simon.perreault/simon.asc\r\n"
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
        assert(0);
    }
    fd = fileno(fp);
    fstat(fd, &sbuf);
    filesize = sbuf.st_size; //to make fortify compile happy
    data = malloc(filesize+1);
    memset(data, 0, filesize+1);

    r = read(fd, data, filesize);
    fclose(fp);

    if (r < 0) {
        fprintf(stderr, "Failed to read vCard\n");
        free(data);
        assert(0);
    }

    card = vcardparser_parse_string(data);
    free(data);

    if (card == NULL) {
        fprintf(stderr, "Failed to parse vCard\n");
        assert(0);
    }

    vcardrestriction_check(card);
    vcardcomponent_normalize(card);
    assert_str_equals(want, vcardcomponent_as_vcard_string(card));
    vcardcomponent_free(card);
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
                                0);

    if (card == NULL) {
        fprintf(stderr, "Failed to create vCard\n");
        assert(0);
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
        "BDAY;VALUE=DATE:19281118\r\n"
        "ADR:;;123 Main Street,Disney World;Orlando;FL;32836;USA;;;;;;;;;;;\r\n"
        "TZ;VALUE=UTC-OFFSET:-0230\r\n"
        "CATEGORIES:aaa,zzz\r\n"
        "group1.NOTE;LANGUAGE=en;PID=1,3;SORT-AS=bar,foo;TYPE=WORK:Test vCard\r\n"
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
                                 vcardparameter_new_pid(sa),
                                 0);
    vcardcomponent_add_property(card, prop);
    vcardproperty_set_group(prop, "group1");

    vcardparameter *param = vcardparameter_new(VCARD_TYPE_PARAMETER);
    vcardenumarray_element e = { VCARD_TYPE_WORK, NULL };
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
    vcardstructuredtype name = { VCARD_NUM_N_FIELDS, { 0 } };
    sa = vcardstrarray_new(1);
    vcardstrarray_append(sa, "Mouse");
    name.field[VCARD_N_FAMILY] = sa;
    sa = vcardstrarray_new(1);
    vcardstrarray_append(sa, "Mickey");
    name.field[VCARD_N_GIVEN] = sa;
    prop = vcardproperty_new_n(&name);
    vcardcomponent_add_property(card, prop);

    /* Create and add ADR property */
    vcardstructuredtype address = { VCARD_NUM_ADR_FIELDS, { 0 } };
    sa = vcardstrarray_new(1);
    vcardstrarray_append(sa, "123 Main Street");
    vcardstrarray_append(sa, "Disney World");
    address.field[VCARD_ADR_STREET] = sa;
    sa = vcardstrarray_new(1);
    vcardstrarray_append(sa, "Orlando");
    address.field[VCARD_ADR_LOCALITY] = sa;
    sa = vcardstrarray_new(1);
    vcardstrarray_append(sa, "FL");
    address.field[VCARD_ADR_REGION] = sa;
    sa = vcardstrarray_new(1);
    vcardstrarray_append(sa, "32836");
    address.field[VCARD_ADR_POSTAL_CODE] = sa;
    sa = vcardstrarray_new(1);
    vcardstrarray_append(sa, "USA");
    address.field[VCARD_ADR_COUNTRY] = sa;
    prop = vcardproperty_new_adr(&address);
    vcardcomponent_add_property(card, prop);

    /* Create and add CATEGORIES property */
    sa = vcardstrarray_new(2);
    vcardstrarray_append(sa, "zzz");
    vcardstrarray_append(sa, "aaa");
    prop = vcardproperty_new_categories(sa);
    vcardcomponent_add_property(card, prop);

    /* Create and add BDAY property */
    vcardtimetype t = vcardtime_null_date();
    t.year = 1928;
    t.month = 11;
    t.day = 18;
    prop = vcardproperty_new_bday(t);
    vcardcomponent_add_property(card, prop);

    /* Create and add TZ property */
    vcardtztype tz = { .utcoffset = -9000 };
    prop = vcardproperty_new_tz(tz);
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
        "BDAY;VALUE=DATE:19281118\r\n"
        "ADR:;;123 Main Street,Disney World;Orlando;FL;32836;USA;;;;;;;;;;;\r\n"
        "TZ;VALUE=UTC-OFFSET:-0230\r\n"
        "CATEGORIES:aaa,zzz\r\n"
        "group1.NOTE;LANGUAGE=en;PID=1,3;SORT-AS=bar,foo;TYPE=WORK:Test vCard\r\n"
        "X-LIC-ERROR;X-LIC-ERRORTYPE=RESTRICTION-CHECK:Failed restrictions for N \r\n"
        " property. Expected 1 instances of the property and got 0\r\n"
        "END:VCARD\r\n";

    /* Change VERSION from 4.0 to 3.0 */
    prop = vcardcomponent_get_first_property(card, VCARD_VERSION_PROPERTY);
    vcardproperty_set_version(prop, VCARD_VERSION_30);

    /* Remove N property */
    prop = vcardcomponent_get_first_property(card, VCARD_N_PROPERTY);
    vcardcomponent_remove_property(card, prop);
    vcardproperty_free(prop);

    vcardrestriction_check(card);
    assert_str_equals(want, vcardcomponent_as_vcard_string(card));
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

    test_parse_file(file);
    card = test_comp_vanew();
    test_add_props(card);
    test_n_restriction(card);

    vcardcomponent_free(card);

    return 0;
}
