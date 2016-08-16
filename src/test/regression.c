/*======================================================================
 FILE: regression.c
 CREATOR: eric 03 April 1999

 (C) COPYRIGHT 1999 Eric Busboom <eric@softwarestudio.org>
     http://www.softwarestudio.org

 The contents of this file are subject to the Mozilla Public License
 Version 1.0 (the "License"); you may not use this file except in
 compliance with the License. You may obtain a copy of the License at
 http://www.mozilla.org/MPL/

 Software distributed under the License is distributed on an "AS IS"
 basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 the License for the specific language governing rights and
 limitations under the License.

 The original author is Eric Busboom
 The original code is regression.c
======================================================================*/
//krazy:skip
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "regression.h"
#include "libical/ical.h"
#include "libicalss/icalss.h"
#include "libicalvcal/icalvcal.h"
#include "libicalvcal/vobject.h"
#include "libicalvcal/vcc.h"

#include <assert.h>
#include <stdlib.h>

/* For GNU libc, strcmp appears to be a macro, so using strcmp in
 assert results in incomprehansible assertion messages. This
 eliminates the problem */

int regrstrcmp(const char *a, const char *b)
{
    return strcmp(a, b);
}

/* This example creates and minipulates the ical object that appears
 * in rfc 2445, page 137 */
/*
static char str[] = "BEGIN:VCALENDAR\
PRODID:\"-//RDU Software//NONSGML HandCal//EN\"\
VERSION:2.0\
BEGIN:VTIMEZONE\
TZID:America/New_York\
BEGIN:STANDARD\
DTSTART:19981025T020000\
RDATE:19981025T020000\
TZOFFSETFROM:-0400\
TZOFFSETTO:-0500\
TZNAME:EST\
END:STANDARD\
BEGIN:DAYLIGHT\
DTSTART:19990404T020000\
RDATE:19990404T020000\
TZOFFSETFROM:-0500\
TZOFFSETTO:-0400\
TZNAME:EDT\
END:DAYLIGHT\
END:VTIMEZONE\
BEGIN:VEVENT\
DTSTAMP:19980309T231000Z\
UID:guid-1.host1.com\
ORGANIZER;ROLE=CHAIR:MAILTO:mrbig@host.com\
ATTENDEE;RSVP=TRUE;ROLE=REQ-PARTICIPANT;CUTYPE=GROUP:MAILTO:employee-A@host.com\
DESCRIPTION:Project XYZ Review Meeting\
CATEGORIES:MEETING\
CLASS:PUBLIC\
CREATED:19980309T130000Z\
SUMMARY:XYZ Project Review\
DTSTART;TZID=America/New_York:19980312T083000\
DTEND;TZID=America/New_York:19980312T093000\
LOCATION:1CP Conference Room 4350\
END:VEVENT\
BEGIN:BOOGA\
DTSTAMP:19980309T231000Z\
X-LIC-FOO:Booga\
DTSTOMP:19980309T231000Z\
UID:guid-1.host1.com\
END:BOOGA\
END:VCALENDAR";
*/

/* Return a list of all attendees who are required. */
/*
static char** get_required_attendees(icalcomponent* event)
{
    icalproperty* p;
    icalparameter* parameter;

    char **attendees;
    int max = 10;
    int c = 0;

    attendees = malloc(max * (sizeof (char *)));

    ok("event is non null", (event != 0));
    int_is("event is a VEVENT", icalcomponent_isa(event), ICAL_VEVENT_COMPONENT);

    for(
        p = icalcomponent_get_first_property(event,ICAL_ATTENDEE_PROPERTY);
        p != 0;
        p = icalcomponent_get_next_property(event,ICAL_ATTENDEE_PROPERTY)
        ) {

        parameter = icalproperty_get_first_parameter(p,ICAL_ROLE_PARAMETER);

        if ( icalparameter_get_role(parameter) == ICAL_ROLE_REQPARTICIPANT)
        {
            attendees[c++] = icalmemory_strdup(icalproperty_get_attendee(p));

            if (c >= max) {
                max *= 2;
                attendees = realloc(attendees, max * (sizeof (char *)));
            }
        }
    }

    return attendees;
}

*/

/* If an attendee has a PARTSTAT of NEEDSACTION or has no PARTSTAT
   parameter, change it to TENTATIVE. */
 /*
    static void update_attendees(icalcomponent* event)
    {
    icalproperty* p;
    icalparameter* parameter;

    assert(event != 0);
    assert(icalcomponent_isa(event) == ICAL_VEVENT_COMPONENT);

    for(
    p = icalcomponent_get_first_property(event,ICAL_ATTENDEE_PROPERTY);
    p != 0;
    p = icalcomponent_get_next_property(event,ICAL_ATTENDEE_PROPERTY)
    ) {

    parameter = icalproperty_get_first_parameter(p,ICAL_PARTSTAT_PARAMETER);

    if (parameter == 0) {

    icalproperty_add_parameter(
    p,
    icalparameter_new_partstat(ICAL_PARTSTAT_TENTATIVE)
    );

    } else if (icalparameter_get_partstat(parameter) == ICAL_PARTSTAT_NEEDSACTION) {

    icalproperty_remove_parameter_by_ref(p,parameter);

    icalparameter_free(parameter);

    icalproperty_add_parameter(
    p,
    icalparameter_new_partstat(ICAL_PARTSTAT_TENTATIVE)
    );
    }
    }
    }
  */

void test_values()
{
    icalvalue *v;
    icalvalue *copy;

    v = icalvalue_new_caladdress("cap://value/1");

    str_is("icalvalue_new_caladdress()", icalvalue_get_caladdress(v), "cap://value/1");

    icalvalue_set_caladdress(v, "cap://value/2");

    str_is("icalvalue_set_caladdress()", icalvalue_get_caladdress(v), "cap://value/2");

    str_is("icalvalue_as_ical_string()", icalvalue_as_ical_string(v), "cap://value/2");

    copy = icalvalue_new_clone(v);

    str_is("icalvalue_new_clone()", icalvalue_as_ical_string(copy), "cap://value/2");
    icalvalue_free(v);
    icalvalue_free(copy);

    v = icalvalue_new_boolean(1);
    int_is("icalvalue_new_boolean(1)", icalvalue_get_boolean(v), 1);
    icalvalue_set_boolean(v, 2);
    ok("icalvalue_set_boolean(2)", (2 == icalvalue_get_boolean(v)));
    str_is("icalvalue_as_ical_string()", icalvalue_as_ical_string(v), "2");

    copy = icalvalue_new_clone(v);
    str_is("icalvalue_new_clone()", icalvalue_as_ical_string(copy), "2");

    icalvalue_free(v);
    icalvalue_free(copy);

    v = icalvalue_new_x("test");
    str_is("icalvalue_new_x(test)", icalvalue_get_x(v), "test");
    icalvalue_set_x(v, "test2");
    str_is("icalvalue_set_x(test2)", icalvalue_get_x(v), "test2");
    str_is("icalvalue_as_ical_string()", icalvalue_as_ical_string(v), "test2");

    copy = icalvalue_new_clone(v);
    str_is("icalvalue_new_clone()", icalvalue_as_ical_string(copy), "test2");

    icalvalue_free(v);
    icalvalue_free(copy);

    v = icalvalue_new_date(icaltime_from_timet(1023404802, 0));
    str_is("icalvalue_new_date()", icalvalue_as_ical_string(v), "20020606T230642");
    icalvalue_set_date(v, icaltime_from_timet(1023404802 - 3600, 0));
    str_is("icalvalue_set_date()", icalvalue_as_ical_string(v), "20020606T220642");

    copy = icalvalue_new_clone(v);
    str_is("icalvalue_new_clone()", icalvalue_as_ical_string(v), "20020606T220642");

    icalvalue_free(v);
    icalvalue_free(copy);

    v = icalvalue_new(-1);

    ok("icalvalue_new(-1), Invalid type", (v == NULL));

    if (v != 0)
        icalvalue_free(v);

    ok("ICAL_BOOLEAN_VALUE", (ICAL_BOOLEAN_VALUE ==
                              icalparameter_value_to_value_kind(ICAL_VALUE_BOOLEAN)));
    ok("ICAL_UTCOFFSET_VALUE", (ICAL_UTCOFFSET_VALUE ==
                                icalparameter_value_to_value_kind(ICAL_VALUE_UTCOFFSET)));
    ok("ICAL_RECUR_VALUE", (ICAL_RECUR_VALUE ==
                            icalparameter_value_to_value_kind(ICAL_VALUE_RECUR)));
    ok("ICAL_CALADDRESS_VALUE", (ICAL_CALADDRESS_VALUE ==
                                 icalparameter_value_to_value_kind(ICAL_VALUE_CALADDRESS)));
    ok("ICAL_PERIOD_VALUE", (ICAL_PERIOD_VALUE ==
                             icalparameter_value_to_value_kind(ICAL_VALUE_PERIOD)));
    ok("ICAL_BINARY_VALUE", (ICAL_BINARY_VALUE ==
                             icalparameter_value_to_value_kind(ICAL_VALUE_BINARY)));
    ok("ICAL_TEXT_VALUE", (ICAL_TEXT_VALUE == icalparameter_value_to_value_kind(ICAL_VALUE_TEXT)));
    ok("ICAL_DURATION_VALUE", (ICAL_DURATION_VALUE ==
                               icalparameter_value_to_value_kind(ICAL_VALUE_DURATION)));
    ok("ICAL_INTEGER_VALUE",
       (ICAL_INTEGER_VALUE == icalparameter_value_to_value_kind(ICAL_VALUE_INTEGER)));

    ok("ICAL_URI_VALUE", (ICAL_URI_VALUE == icalparameter_value_to_value_kind(ICAL_VALUE_URI)));
    ok("ICAL_FLOAT_VALUE", (ICAL_FLOAT_VALUE ==
                            icalparameter_value_to_value_kind(ICAL_VALUE_FLOAT)));
    ok("ICAL_X_VALUE", (ICAL_X_VALUE == icalparameter_value_to_value_kind(ICAL_VALUE_X)));
    ok("ICAL_DATETIME_VALUE", (ICAL_DATETIME_VALUE ==
                               icalparameter_value_to_value_kind(ICAL_VALUE_DATETIME)));
    ok("ICAL_DATE_TIME", (ICAL_DATE_VALUE == icalparameter_value_to_value_kind(ICAL_VALUE_DATE)));

    /*    v = icalvalue_new_caladdress(0);

       printf("Bad string: %p\n",v);

       if (v!=0) icalvalue_free(v); */

    icalerror_set_error_state(ICAL_MALFORMEDDATA_ERROR, ICAL_ERROR_NONFATAL);
    v = icalvalue_new_from_string(ICAL_RECUR_VALUE, "D2 #0");
    ok("illegal recur value", (v == 0));

    v = icalvalue_new_from_string(ICAL_TRIGGER_VALUE, "Gonk");
    ok("illegal trigger value", (v == 0));

    v = icalvalue_new_from_string(ICAL_REQUESTSTATUS_VALUE, "Gonk");
    ok("illegal requeststatus value", (v == 0));

    icalerror_set_error_state(ICAL_MALFORMEDDATA_ERROR, ICAL_ERROR_DEFAULT);
}

void test_properties()
{
    icalproperty *prop;
    icalparameter *param;

    icalproperty *clone;
    char test_cn_str[128] = "";
    const char *test_cn_str_good = "A Common Name 1A Common Name 2A Common Name 3A Common Name 4";
    const char *test_ical_str_good =
        "COMMENT;CN=A Common Name 1;CN=A Common Name 2;CN=A Common Name 3;CN=A \r\n Common Name 4:Another Comment\r\n";

    prop = icalproperty_vanew_comment("Another Comment",
                                      icalparameter_new_cn("A Common Name 1"),
                                      icalparameter_new_cn("A Common Name 2"),
                                      icalparameter_new_cn("A Common Name 3"),
                                      icalparameter_new_cn("A Common Name 4"), (void *)0);

    for (param = icalproperty_get_first_parameter(prop, ICAL_ANY_PARAMETER);
         param != 0;
         param = icalproperty_get_next_parameter(prop, ICAL_ANY_PARAMETER)) {
        const char *str = icalparameter_get_cn(param);

        if (VERBOSE) {
            printf("Prop parameter: %s\n", icalparameter_get_cn(param));
        }
        strcat(test_cn_str, str);
    }
    str_is("fetching parameters", test_cn_str, test_cn_str_good);

    if (VERBOSE) {
        printf("Prop value: %s\n", icalproperty_get_comment(prop));
    }
    str_is("icalproperty_get_comment()", icalproperty_get_comment(prop), "Another Comment");

    if (VERBOSE) {
        printf("As iCAL string:\n %s\n", icalproperty_as_ical_string(prop));
    }
    str_is("icalproperty_as_ical_string()", icalproperty_as_ical_string(prop), test_ical_str_good);

    clone = icalproperty_new_clone(prop);

    if (VERBOSE) {
        printf("Clone:\n %s\n", icalproperty_as_ical_string(prop));
    }
    str_is("icalproperty_new_clone()", icalproperty_as_ical_string(prop), test_ical_str_good);

    icalproperty_free(clone);
    icalproperty_free(prop);

    prop = icalproperty_new(-1);

    ok("test icalproperty_new() with invalid type (-1)", (prop == NULL));

    if (prop != 0)
        icalproperty_free(prop);
}

void test_utf8()
{
    icalcomponent *comp;
    icalproperty *prop;
    const char *utf8text =
        "aáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaá";
    const char *test_ical_str_good =
        "DESCRIPTION:\r\n"
        " aáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaá\r\n"
        " óaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóaáóa\r\n"
        " áóaáóaáóaáóaáóaáóaáóaáóaáóaá\r\n";

    const char *item =
        "BEGIN:VCALENDAR\n"
        "PRODID:-//Ximian//NONSGML Evolution Calendar//EN\n"
        "VERSION:2.0\n"
        "METHOD:PUBLISH\n"
        "BEGIN:VEVENT\n"
        "SUMMARY:  áó aáóaáó aáä\n"
        "LOCATION:áóaáóaáóaä  áóaáóaáóaáóaáóaáóaáóaáóaáóaáó  aáóaáóaáóaáóaá \n"
        "END:VEVENT\n"
        "END:VCALENDAR\n";

    const char *agenda93 =
        "BEGIN:VCALENDAR\n"
        "VERSION:2.0\n"
        "CALSCALE:GREGORIAN\n"
        "PRODID:-//Henrik Levkowetz//ietf-agenda-ical 1.03//EN\n"
        "BEGIN:VEVENT\n"
        "DTSTART;TZID=Europe/Prague:20150721T190000\n"
        "DTEND;TZID=Europe/Prague:20150721T230000\n"
        "DTSTAMP:20150627T071730Z\n"
        "UID:ical-ietf-93-3ce43c64.1@tools.ietf.org\n"
        "SEQUENCE:197\n"
        "SUMMARY:IETF 93 Social Event at the Žofín Palace\n"
        "END:VEVENT\n"
        "END:VCALENDAR\n";

    prop = icalproperty_new_description(utf8text);

    str_is("icalproperty_as_ical_string()", icalproperty_as_ical_string(prop), test_ical_str_good);
    icalproperty_free(prop);

    /* test issue 116 */
    comp = icalcomponent_new_from_string(item);
    ok("parsed", (comp != NULL));
    str_is("location", icalcomponent_get_location(comp), "áóaáóaáóaä  áóaáóaáóaáóaáóaáóaáóaáóaáóaáó  aáóaáóaáóaáóaá");
    str_is("summary", icalcomponent_get_summary(comp), "áó aáóaáó aáä");
    icalcomponent_free(comp);

    /* test another report against issue 116 */
    comp = icalcomponent_new_from_string(agenda93);
    ok("parsed", (comp != NULL));
    str_is("summary", icalcomponent_get_summary(comp), "IETF 93 Social Event at the Žofín Palace");
    icalcomponent_free(comp);
}

void test_parameters()
{
    icalparameter *p;
    int i;
    int enums[] =
        { ICAL_CUTYPE_INDIVIDUAL, ICAL_CUTYPE_RESOURCE, ICAL_FBTYPE_BUSY, ICAL_PARTSTAT_NEEDSACTION,
  ICAL_ROLE_NONPARTICIPANT, ICAL_XLICCOMPARETYPE_LESSEQUAL, ICAL_XLICERRORTYPE_MIMEPARSEERROR, -1 };

    const char *str1 = "A Common Name";

    p = icalparameter_new_cn(str1);

    str_is("icalparameter_new_cn()", icalparameter_get_cn(p), str1);
    str_is("icalparameter_as_ical_string()", icalparameter_as_ical_string(p), "CN=A Common Name");

    icalparameter_free(p);

    p = icalparameter_new_from_string("PARTSTAT=ACCEPTED");
    ok("PARTSTAT_PARAMETER", (icalparameter_isa(p) == ICAL_PARTSTAT_PARAMETER));
    ok("PARTSTAT_ACCEPTED", (icalparameter_get_partstat(p) == ICAL_PARTSTAT_ACCEPTED));

    icalparameter_free(p);

    p = icalparameter_new_from_string("ROLE=CHAIR");

    ok("ROLE_PARAMETER", (icalparameter_isa(p) == ICAL_ROLE_PARAMETER));
    ok("ROLE_CHAIR", (icalparameter_get_role(p) == ICAL_ROLE_CHAIR));

    icalparameter_free(p);

    p = icalparameter_new_from_string("PARTSTAT=X-FOO");
    ok("PARTSTAT_PARAMETER", (icalparameter_isa(p) == ICAL_PARTSTAT_PARAMETER));
    ok("PARTSTAT_X", (icalparameter_get_partstat(p) == ICAL_PARTSTAT_X));

    icalparameter_free(p);

    p = icalparameter_new_from_string("X-PARAM=X-FOO");
    ok("X_PARAMETER", (icalparameter_isa(p) == ICAL_X_PARAMETER));

    icalparameter_free(p);

    for (i = 0; enums[i] != -1; i++) {
        if (VERBOSE) {
            printf("%s\n", icalparameter_enum_to_string(enums[i]));
        }
        ok("test parameter enums",
           (icalparameter_string_to_enum(icalparameter_enum_to_string(enums[i])) == enums[i]));
    }
}

const char *good_child =
    "BEGIN:VEVENT\r\n"
    "VERSION:2.0\r\n"
    "DESCRIPTION:This is an event\r\n"
    "COMMENT;CN=A Common Name 1;CN=A Common Name 2;CN=A Common Name 3;CN=A \r\n"
    " Common Name 4:Another Comment\r\n"
    "X-LIC-ERROR;X-LIC-ERRORTYPE=COMPONENT-PARSE-ERROR:This is only a test\r\n" "END:VEVENT\r\n";

void test_components()
{
    icalcomponent *c;
    icalcomponent *child;

    c = icalcomponent_vanew(
            ICAL_VCALENDAR_COMPONENT,
            icalproperty_new_version("2.0"),
            icalproperty_new_prodid("-//RDU Software//NONSGML HandCal//EN"),
            icalproperty_vanew_comment("A Comment",
                icalparameter_new_cn("A Common Name 1"), (void *)0),
            icalcomponent_vanew(ICAL_VEVENT_COMPONENT,
                                icalproperty_new_version("2.0"),
                                icalproperty_new_description("This is an event"),
                                icalproperty_vanew_comment(
                                    "Another Comment",
                                    icalparameter_new_cn("A Common Name 1"),
                                    icalparameter_new_cn("A Common Name 2"),
                                    icalparameter_new_cn("A Common Name 3"),
                                    icalparameter_new_cn("A Common Name 4"),
                                    (void *)0),
                                icalproperty_vanew_xlicerror(
                                    "This is only a test",
                                    icalparameter_new_xlicerrortype(
                                        ICAL_XLICERRORTYPE_COMPONENTPARSEERROR),
                                    (void *)0),
                                0),
            0);

    if (VERBOSE)
        printf("Original Component:\n%s\n\n", icalcomponent_as_ical_string(c));

    child = icalcomponent_get_first_component(c, ICAL_VEVENT_COMPONENT);

    ok("test icalcomponent_get_first_component()", (child != NULL));

    if (VERBOSE)
        printf("Child Component:\n%s\n\n", icalcomponent_as_ical_string(child));

    str_is("test results of child component", icalcomponent_as_ical_string(child), good_child);

    icalcomponent_free(c);
}

void test_memory()
{
    size_t bufsize = 256;
    int i;
    char *p;

    char S1[] = "1) When in the Course of human events, ";
    char S2[] = "2) it becomes necessary for one people to dissolve the political bands which have connected them with another, ";
    char S3[] = "3) and to assume among the powers of the earth, ";
    char S4[] = "4) the separate and equal station to which the Laws of Nature and of Nature's God entitle them, ";
    char S5[] = "5) a decent respect to the opinions of mankind requires that they ";
    char S6[] = "6) should declare the causes which impel them to the separation. ";
    char S7[] = "7) We hold these truths to be self-evident, ";
    char S8[] = "8) that all men are created equal, ";

/*    char S9[] = "9) that they are endowed by their Creator with certain unalienable Rights, ";
    char S10[] = "10) that among these are Life, Liberty, and the pursuit of Happiness. ";
    char S11[] = "11) That to secure these rights, Governments are instituted among Men, ";
    char S12[] = "12) deriving their just powers from the consent of the governed. ";
*/

    char *f, *b1, *b2, *b3, *b4, *b5, *b6, *b7, *b8;

#define BUFSIZE 1024

    f = icalmemory_new_buffer(bufsize);
    p = f;
    b1 = icalmemory_tmp_buffer(BUFSIZE);
    strcpy(b1, S1);
    icalmemory_append_string(&f, &p, &bufsize, b1);

    b2 = icalmemory_tmp_buffer(BUFSIZE);
    strcpy(b2, S2);
    icalmemory_append_string(&f, &p, &bufsize, b2);

    b3 = icalmemory_tmp_buffer(BUFSIZE);
    strcpy(b3, S3);
    icalmemory_append_string(&f, &p, &bufsize, b3);

    b4 = icalmemory_tmp_buffer(BUFSIZE);
    strcpy(b4, S4);
    icalmemory_append_string(&f, &p, &bufsize, b4);

    b5 = icalmemory_tmp_buffer(BUFSIZE);
    strcpy(b5, S5);
    icalmemory_append_string(&f, &p, &bufsize, b5);

    b6 = icalmemory_tmp_buffer(BUFSIZE);
    strcpy(b6, S6);
    icalmemory_append_string(&f, &p, &bufsize, b6);

    b7 = icalmemory_tmp_buffer(BUFSIZE);
    strcpy(b7, S7);
    icalmemory_append_string(&f, &p, &bufsize, b7);

    b8 = icalmemory_tmp_buffer(BUFSIZE);
    strcpy(b8, S8);
    icalmemory_append_string(&f, &p, &bufsize, b8);

    if (VERBOSE) {
        printf("1: %p %s \n", b1, b1);
        printf("2: %p %s\n", b2, b2);
        printf("3: %p %s\n", b3, b3);
        printf("4: %p %s\n", b4, b4);
        printf("5: %p %s\n", b5, b5);
        printf("6: %p %s\n", b6, b6);
        printf("7: %p %s\n", b7, b7);
        printf("8: %p %s\n", b8, b8);

        printf("Final: %s\n", f);
        printf("Final buffer size: %lu\n", (unsigned long)bufsize);
    }

    ok("final buffer size == 806", (bufsize == 806));

    free(f);

    bufsize = 4;

    f = icalmemory_new_buffer(bufsize);

    memset(f, 0, bufsize);
    p = f;

    icalmemory_append_char(&f, &p, &bufsize, 'a');
    if (VERBOSE)
        printf("Char-by-Char buffer: %s\n", f);

    icalmemory_append_char(&f, &p, &bufsize, 'b');
    if (VERBOSE)
        printf("Char-by-Char buffer: %s\n", f);

    icalmemory_append_char(&f, &p, &bufsize, 'c');
    if (VERBOSE)
        printf("Char-by-Char buffer: %s\n", f);

    icalmemory_append_char(&f, &p, &bufsize, 'd');
    if (VERBOSE)
        printf("Char-by-Char buffer: %s\n", f);

    icalmemory_append_char(&f, &p, &bufsize, 'e');
    if (VERBOSE)
        printf("Char-by-Char buffer: %s\n", f);

    icalmemory_append_char(&f, &p, &bufsize, 'f');
    if (VERBOSE)
        printf("Char-by-Char buffer: %s\n", f);

    icalmemory_append_char(&f, &p, &bufsize, 'g');
    if (VERBOSE)
        printf("Char-by-Char buffer: %s\n", f);

    icalmemory_append_char(&f, &p, &bufsize, 'h');
    if (VERBOSE)
        printf("Char-by-Char buffer: %s\n", f);

    icalmemory_append_char(&f, &p, &bufsize, 'i');
    if (VERBOSE)
        printf("Char-by-Char buffer: %s\n", f);

    icalmemory_append_char(&f, &p, &bufsize, 'j');
    if (VERBOSE)
        printf("Char-by-Char buffer: %s\n", f);

    icalmemory_append_char(&f, &p, &bufsize, 'a');
    if (VERBOSE)
        printf("Char-by-Char buffer: %s\n", f);

    icalmemory_append_char(&f, &p, &bufsize, 'b');
    if (VERBOSE)
        printf("Char-by-Char buffer: %s\n", f);

    icalmemory_append_char(&f, &p, &bufsize, 'c');
    if (VERBOSE)
        printf("Char-by-Char buffer: %s\n", f);

    icalmemory_append_char(&f, &p, &bufsize, 'd');
    if (VERBOSE)
        printf("Char-by-Char buffer: %s\n", f);

    icalmemory_append_char(&f, &p, &bufsize, 'e');
    if (VERBOSE)
        printf("Char-by-Char buffer: %s\n", f);

    icalmemory_append_char(&f, &p, &bufsize, 'f');
    if (VERBOSE)
        printf("Char-by-Char buffer: %s\n", f);

    icalmemory_append_char(&f, &p, &bufsize, 'g');
    if (VERBOSE)
        printf("Char-by-Char buffer: %s\n", f);

    icalmemory_append_char(&f, &p, &bufsize, 'h');
    if (VERBOSE)
        printf("Char-by-Char buffer: %s\n", f);

    icalmemory_append_char(&f, &p, &bufsize, 'i');
    if (VERBOSE)
        printf("Char-by-Char buffer: %s\n", f);

    icalmemory_append_char(&f, &p, &bufsize, 'j');
    if (VERBOSE)
        printf("Char-by-Char buffer: %s\n", f);

    free(f);

    for (i = 0; i < 100; i++) {
        f = icalmemory_tmp_buffer(bufsize);

        assert(f != 0);

        memset(f, 0, bufsize);
        snprintf(f, bufsize, "%d", i);
    }
}

void test_dirset()
{
    icalcomponent *c;
    icalgauge *gauge;
    icalerrorenum error;
    icalcomponent *next, *itr;
    icalset *cluster;
    icalset *s, *s2;
    struct icalperiodtype rtime;
    int i;
    int count = 0;

    (void)mkdir("store", 0755);
    (void)mkdir("store-new", 0755);

    s = icaldirset_new("store");
    s2 = icaldirset_new("store-new");

    ok("opening 'store' dirset", s != NULL);
    ok("opening 'store-new' dirset", s2 != NULL);

    rtime.start = icaltime_from_timet(time(0), 0);

    cluster = icalfileset_new("clusterin.vcd");

    if (cluster == 0) {
        printf("Failed to create cluster: %s\n", icalerror_strerror(icalerrno));
    }
#define NUMCOMP 4

    /* Duplicate every component in the cluster NUMCOMP times */

    icalerror_clear_errno();

    for (i = 1; i < NUMCOMP + 1; i++) {

        /*rtime.start.month = i%12; */
        rtime.start.month = i;
        rtime.end = rtime.start;
        rtime.end.hour++;

        for (itr = icalfileset_get_first_component(cluster);
             itr != 0; itr = icalfileset_get_next_component(cluster)) {
            icalcomponent *clone;
            icalproperty *p;

            if (icalcomponent_isa(itr) != ICAL_VEVENT_COMPONENT) {
                continue;
            }

            /* Change the dtstart and dtend times in the component
               pointed to by Itr */

            clone = icalcomponent_new_clone(itr);
            assert(icalerrno == ICAL_NO_ERROR);
            assert(clone != 0);

            /* DTSTART */
            p = icalcomponent_get_first_property(clone, ICAL_DTSTART_PROPERTY);
            assert(icalerrno == ICAL_NO_ERROR);

            if (p == 0) {
                p = icalproperty_new_dtstart(rtime.start);
                icalcomponent_add_property(clone, p);
            } else {
                icalproperty_set_dtstart(p, rtime.start);
            }
            assert(icalerrno == ICAL_NO_ERROR);

            /* DTEND */
            p = icalcomponent_get_first_property(clone, ICAL_DTEND_PROPERTY);
            assert(icalerrno == ICAL_NO_ERROR);

            if (p == 0) {
                p = icalproperty_new_dtstart(rtime.end);
                icalcomponent_add_property(clone, p);
            } else {
                icalproperty_set_dtstart(p, rtime.end);
            }
            assert(icalerrno == ICAL_NO_ERROR);

            printf("\n----------\n%s\n---------\n", icalcomponent_as_ical_string(clone));
            error = icaldirset_add_component(s, clone);

            assert(error == ICAL_NO_ERROR);
            _unused(error);
        }
    }

    gauge = icalgauge_new_from_sql((char *)
								   "SELECT * FROM VEVENT WHERE "
								   "VEVENT.SUMMARY = 'Submit Income Taxes' OR "
								   "VEVENT.SUMMARY = 'Bastille Day Party'",
								   0);

    (void)icaldirset_select(s, gauge);

    for (c = icaldirset_get_first_component(s); c != 0; c = icaldirset_get_next_component(s)) {
        printf("Got one! (%d)\n", count++);

		printf("%s", icalcomponent_as_ical_string(c));
		if (icaldirset_add_component(s2, c) == 0) {
			printf("Failed to write!\n");
		}
		icalcomponent_free(c);
    }

    icalset_free(s2);

    for (c = icaldirset_get_first_component(s); c != 0; c = next) {

        next = icaldirset_get_next_component(s);

        /*icaldirset_remove_component(s,c); */
        printf("%s", icalcomponent_as_ical_string(c));
    }

    icalset_free(s);
    icalset_free(cluster);
}

void test_compare()
{
    icalvalue *v1, *v2;

    v1 = icalvalue_new_caladdress("cap://value/1");
    v2 = icalvalue_new_clone(v1);

    ok("compare value and clone", (icalvalue_compare(v1, v2) == ICAL_XLICCOMPARETYPE_EQUAL));

    icalvalue_free(v1);
    icalvalue_free(v2);

    v1 = icalvalue_new_caladdress("A");
    v2 = icalvalue_new_caladdress("B");

    ok("test compare of A and B results in LESS",
       (icalvalue_compare(v1, v2) == ICAL_XLICCOMPARETYPE_LESS));

    ok("test compare of B and A results in GREATER",
       (icalvalue_compare(v2, v1) == ICAL_XLICCOMPARETYPE_GREATER));

    icalvalue_free(v1);
    icalvalue_free(v2);

    v1 = icalvalue_new_caladdress("B");
    v2 = icalvalue_new_caladdress("A");

    ok("test compare of caladdress A and B results in GREATER",
       (icalvalue_compare(v1, v2) == ICAL_XLICCOMPARETYPE_GREATER));

    icalvalue_free(v1);
    icalvalue_free(v2);

    v1 = icalvalue_new_integer(5);
    v2 = icalvalue_new_integer(5);

    ok("test compare of 5 and 5 results in EQUAL",
       (icalvalue_compare(v1, v2) == ICAL_XLICCOMPARETYPE_EQUAL));

    icalvalue_free(v1);
    icalvalue_free(v2);

    v1 = icalvalue_new_integer(5);
    v2 = icalvalue_new_integer(10);

    ok("test compare of 5 and 10 results in LESS",
       (icalvalue_compare(v1, v2) == ICAL_XLICCOMPARETYPE_LESS));

    ok("test compare of 10 and 5 results in GREATER",
       (icalvalue_compare(v2, v1) == ICAL_XLICCOMPARETYPE_GREATER));

    icalvalue_free(v1);
    icalvalue_free(v2);
}

void test_restriction()
{
    icalcomponent *comp;
    struct icaltimetype atime = icaltime_from_timet(time(0), 0);
    int valid;

    struct icaldatetimeperiodtype rtime;

    rtime.period.start = icaltime_from_timet(time(0), 0);
    rtime.period.end = icaltime_from_timet(time(0), 0);
    rtime.period.end.hour++;
    rtime.time = icaltime_null_time();

    comp =
        icalcomponent_vanew(
            ICAL_VCALENDAR_COMPONENT,
            icalproperty_new_version("2.0"),
            icalproperty_new_prodid("-//RDU Software//NONSGML HandCal//EN"),
            icalproperty_new_method(ICAL_METHOD_REQUEST),
            icalcomponent_vanew(
                ICAL_VTIMEZONE_COMPONENT,
                icalproperty_new_tzid("America/New_York"),
                icalcomponent_vanew(ICAL_XDAYLIGHT_COMPONENT,
                                    icalproperty_new_dtstart(atime),
                                    icalproperty_new_rdate(rtime),
                                    icalproperty_new_tzoffsetfrom(-4),
                                    icalproperty_new_tzoffsetto(-5),
                                    icalproperty_new_tzname("EST"),
                                    (void *)0),
                icalcomponent_vanew(ICAL_XSTANDARD_COMPONENT,
                                    icalproperty_new_dtstart(atime),
                                    icalproperty_new_rdate(rtime),
                                    icalproperty_new_tzoffsetfrom(-5),
                                    icalproperty_new_tzoffsetto(-4),
                                    icalproperty_new_tzname("EST"),
                                    (void *)0),
                (void *)0),
            icalcomponent_vanew(
                ICAL_VEVENT_COMPONENT,
                icalproperty_new_dtstamp(atime),
                icalproperty_new_uid("guid-1.host1.com"),
                icalproperty_vanew_organizer("mrbig@host.com",
                                             icalparameter_new_role(ICAL_ROLE_CHAIR),
                                             (void *)0),
                icalproperty_vanew_attendee("employee-A@host.com",
                                            icalparameter_new_role(ICAL_ROLE_REQPARTICIPANT),
                                            icalparameter_new_rsvp(ICAL_RSVP_TRUE),
                                            icalparameter_new_cutype(ICAL_CUTYPE_GROUP),
                                            (void *)0),
                icalproperty_new_description("Project XYZ Review Meeting"),
                icalproperty_new_categories("MEETING"),
                icalproperty_new_class(ICAL_CLASS_PUBLIC),
                icalproperty_new_created(atime),
                icalproperty_new_summary("XYZ Project Review"),
                /*              icalproperty_new_dtstart(
                                atime,
                                icalparameter_new_tzid("America/New_York"),
                                0
                                ), */
                icalproperty_vanew_dtend(atime,
                                         icalparameter_new_tzid("America/New_York"),
                                         (void *)0),
                icalproperty_new_location("1CP Conference Room 4350"), (void *)0),
            (void *)0);

    valid = icalrestriction_check(comp);

    ok("icalrestriction_check() == 0", (valid == 0));

    (void)icalcomponent_as_ical_string(comp);

    icalcomponent_free(comp);
}

void test_calendar()
{
    icalcomponent *comp;
    icalset *c;
    icalset *s;
    icalcalendar *calendar;
    icalerrorenum error;
    struct icaltimetype atime = icaltime_from_timet(time(0), 0);

    (void)mkdir("calendar", 0755);
    (void)mkdir("calendar/booked", 0755);

    calendar = icalcalendar_new((char *)"calendar");

    comp =
        icalcomponent_vanew(
            ICAL_VCALENDAR_COMPONENT,
            icalcomponent_vanew(
                ICAL_VEVENT_COMPONENT,
                icalproperty_new_version("2.0"),
                icalproperty_new_description("This is an event"),
                icalproperty_new_dtstart(atime),
                icalproperty_vanew_comment(
                    "Another Comment",
                    icalparameter_new_cn("A Common Name 1"),
                    icalparameter_new_cn("A Common Name 2"),
                    icalparameter_new_cn("A Common Name 3"),
                    icalparameter_new_cn("A Common Name 4"),
                    (void *)0),
                icalproperty_vanew_xlicerror(
                    "This is only a test",
                    icalparameter_new_xlicerrortype(ICAL_XLICERRORTYPE_COMPONENTPARSEERROR),
                    (void *)0),
                (void *)0),
            (void *)0);

    s = icalcalendar_get_booked(calendar);

    error = icaldirset_add_component(s, comp);

    ok("Adding Component to dirset", (error == ICAL_NO_ERROR));

    c = icalcalendar_get_properties(calendar);

    error = icalfileset_add_component(c, icalcomponent_new_clone(comp));

    ok("Adding Clone Component to dirset", (error == ICAL_NO_ERROR));

    icalcalendar_free(calendar);

    ok("icalcalendar test", (1));
}

void test_increment(void);

/* coverity[pass_by_value] */
void print_occur(struct icalrecurrencetype recur, struct icaltimetype start)
{
    struct icaltimetype next;
    icalrecur_iterator *ritr;

    time_t tt = icaltime_as_timet(start);

    printf("#### %s\n", icalrecurrencetype_as_string(&recur));
    printf("#### %s\n", ctime(&tt));

    ritr = icalrecur_iterator_new(recur, start);
    for (next = icalrecur_iterator_next(ritr);
         !icaltime_is_null_time(next);
         next = icalrecur_iterator_next(ritr)) {
        tt = icaltime_as_timet(next);
        printf("  %s", ctime(&tt));
    }

    icalrecur_iterator_free(ritr);
}

void test_recur()
{
    struct icalrecurrencetype rt;
    struct icaltimetype start;
    time_t array[25];
    int i;

    rt = icalrecurrencetype_from_string(
             "FREQ=MONTHLY;UNTIL=19971224T000000Z;INTERVAL=1;BYDAY=TU,2FR,3SA");
    start = icaltime_from_string("19970905T090000Z");

    if (VERBOSE)
        print_occur(rt, start);

    if (VERBOSE)
        printf("\n  Using icalrecur_expand_recurrence\n");

    icalrecur_expand_recurrence(
		(char *)"FREQ=MONTHLY;UNTIL=19971224T000000Z;INTERVAL=1;BYDAY=TU,2FR,3SA",
        icaltime_as_timet(start), 25, array);

    for (i = 0; i < 25 && array[i] != 0; i++) {
        if (VERBOSE)
            printf("  %s", ctime(&(array[i])));
    }

/*    test_increment();*/
}

void test_expand_recurrence()
{
    time_t arr[10];
    time_t now = 931057385;
    int i, numfound = 0;

    icalrecur_expand_recurrence((char *)"FREQ=MONTHLY;BYDAY=MO,WE", now, 5, arr);

    if (VERBOSE)
        printf("Start %s", ctime(&now));

    for (i = 0; i < 5; i++) {
        numfound++;
        if (VERBOSE)
            printf("i=%d  %s\n", i, ctime(&arr[i]));
    }
    int_is("Get an array of 5 items", numfound, 5);
}

enum byrule
{
    NO_CONTRACTION = -1,
    BY_SECOND = 0,
    BY_MINUTE = 1,
    BY_HOUR = 2,
    BY_DAY = 3,
    BY_MONTH_DAY = 4,
    BY_YEAR_DAY = 5,
    BY_WEEK_NO = 6,
    BY_MONTH = 7,
    BY_SET_POS
};

void icalrecurrencetype_test()
{
    icalvalue *v =
        icalvalue_new_from_string(
            ICAL_RECUR_VALUE,
            "FREQ=YEARLY;UNTIL=20060101T000000;INTERVAL=2;BYDAY=SU,WE;BYSECOND=15,30; BYMONTH=1,6,11");

    struct icalrecurrencetype r = icalvalue_get_recur(v);
    struct icaltimetype t = icaltime_from_timet(time(0), 0);
    struct icaltimetype next;
    time_t tt;

    struct icalrecur_iterator_impl *itr =
        (struct icalrecur_iterator_impl *)icalrecur_iterator_new(r, t);

    do {

        next = icalrecur_iterator_next(itr);
        tt = icaltime_as_timet(next);

        printf("%s", ctime(&tt));

    } while (!icaltime_is_null_time(next));

    icalvalue_free(v);

    icalrecur_iterator_free(itr);
}

/* From Federico Mena Quintero <federico@ximian.com>    */
void test_recur_parameter_bug()
{
    static const char test_icalcomp_str[] =
        "BEGIN:VEVENT\r\n"
        "RRULE;X-EVOLUTION-ENDDATE=20030209T081500:FREQ=DAILY;COUNT=10;INTERVAL=6\r\n"
        "END:VEVENT\r\n";

    icalcomponent *icalcomp;
    icalproperty *prop;
    struct icalrecurrencetype recur;
    int n_errors;
    char *str;

    icalcomp = icalparser_parse_string((char *)test_icalcomp_str);
    ok("icalparser_parse_string()", (icalcomp != NULL));
    assert(icalcomp != NULL);

    str = icalcomponent_as_ical_string(icalcomp);
    str_is("parsed matches original", str, (char *)test_icalcomp_str);
    if (VERBOSE)
        printf("%s\n\n", str);

    n_errors = icalcomponent_count_errors(icalcomp);
    int_is("no parse errors", n_errors, 0);

    if (n_errors) {
        icalproperty *p;
        for (p = icalcomponent_get_first_property(icalcomp, ICAL_XLICERROR_PROPERTY);
             p;
             p = icalcomponent_get_next_property(icalcomp, ICAL_XLICERROR_PROPERTY)) {
            const char *str;
            str = icalproperty_as_ical_string(p);
            fprintf(stderr, "error: %s\n", str);
        }
    }

    prop = icalcomponent_get_first_property(icalcomp, ICAL_RRULE_PROPERTY);
    ok("get RRULE property", (prop != NULL));
    assert(prop != NULL);

    recur = icalproperty_get_rrule(prop);

    if (VERBOSE)
        printf("%s\n", icalrecurrencetype_as_string(&recur));

    icalcomponent_free(icalcomp);
}

void test_duration()
{
    struct icaldurationtype d;

    d = icaldurationtype_from_string("PT8H30M");
    if (VERBOSE) {
        printf("%s\n", icaldurationtype_as_ical_string(d));
    }
    int_is("PT8H30M", icaldurationtype_as_int(d), 30600);

    d = icaldurationtype_from_string("-PT8H30M");
    if (VERBOSE) {
        printf("%s\n", icaldurationtype_as_ical_string(d));
    }
    int_is("-PT8H30M", icaldurationtype_as_int(d), -30600);

    d = icaldurationtype_from_string("PT10H10M10S");
    if (VERBOSE) {
        printf("%s\n", icaldurationtype_as_ical_string(d));
    }
    int_is("PT10H10M10S", icaldurationtype_as_int(d), 36610);

    d = icaldurationtype_from_string("P7W");
    if (VERBOSE) {
        printf("%s\n", icaldurationtype_as_ical_string(d));
    }
    int_is("P7W", icaldurationtype_as_int(d), 4233600);

    d = icaldurationtype_from_string("P2DT8H30M");
    if (VERBOSE) {
        printf("%s\n", icaldurationtype_as_ical_string(d));
    }
    int_is("P2DT8H30M", icaldurationtype_as_int(d), 203400);

    d = icaldurationtype_from_string("P2W1DT5H");
    if (VERBOSE) {
        printf("%s %d\n", icaldurationtype_as_ical_string(d), icaldurationtype_as_int(d));
    }
    int_is("P2W1DT5H", icaldurationtype_as_int(d), 1314000);

    icalerror_set_errors_are_fatal(0);

    /* Test conversion of bad input */

    d = icaldurationtype_from_int(1314000);
    if (VERBOSE) {
        printf("%s %d\n", icaldurationtype_as_ical_string(d), icaldurationtype_as_int(d));
    }
    str_is("1314000", icaldurationtype_as_ical_string(d), "P15DT5H");

    d = icaldurationtype_from_string("P-2DT8H30M");
    if (VERBOSE) {
        printf("%s\n", icaldurationtype_as_ical_string(d));
    }
    int_is("P-2DT8H30M", icaldurationtype_as_int(d), 0);

    d = icaldurationtype_from_string("P7W8H");
    if (VERBOSE) {
        printf("%s\n", icaldurationtype_as_ical_string(d));
    }
    int_is("P7W8H", icaldurationtype_as_int(d), 0);

    d = icaldurationtype_from_string("T10H");
    if (VERBOSE) {
        printf("%s\n", icaldurationtype_as_ical_string(d));
    }
    int_is("T10H", icaldurationtype_as_int(d), 0);

    icalerror_set_errors_are_fatal(1);

    d = icaldurationtype_from_int(4233600);
    if (VERBOSE) {
        printf("%s\n", icaldurationtype_as_ical_string(d));
    }
    str_is("P7W", icaldurationtype_as_ical_string(d), "P7W");

    d = icaldurationtype_from_int(4424400);
    if (VERBOSE) {
        printf("%s\n", icaldurationtype_as_ical_string(d));
    }
    str_is("P51DT5H", icaldurationtype_as_ical_string(d), "P51DT5H");
}

void test_period()
{
    struct icalperiodtype p;
    icalvalue *v;
    const char *str;

    str = "19971015T050000Z/PT8H30M";
    p = icalperiodtype_from_string(str);
    str_is(str, icalperiodtype_as_ical_string(p), str);

    str = "19971015T050000Z/19971015T060000Z";
    p = icalperiodtype_from_string(str);
    str_is(str, icalperiodtype_as_ical_string(p), str);

    str = "19970101T120000/PT3H";

    v = icalvalue_new_from_string(ICAL_PERIOD_VALUE, str);
    str_is(str, icalvalue_as_ical_string(v), str);

    icalvalue_free(v);
}

void test_strings()
{
    icalvalue *v;

    v = icalvalue_new_text("foo;bar;bats");
    if (VERBOSE)
        printf("%s\n", icalvalue_as_ical_string(v));

    str_is("test encoding of 'foo;bar;bats'", "foo\\;bar\\;bats", icalvalue_as_ical_string(v));

    icalvalue_free(v);

    v = icalvalue_new_text("foo\\;b\nar\\;ba\tts");
    if (VERBOSE)
        printf("%s\n", icalvalue_as_ical_string(v));

    str_is("test encoding of 'foo\\\\;b\\nar\\\\;ba\\tts'",
           "foo\\\\\\;b\\nar\\\\\\;ba	ts", icalvalue_as_ical_string(v));

    icalvalue_free(v);
}

#if defined(INVALID_TEST)
/* This test is invalid because parameters may not have control chars, such as '\n' */
void test_tzid_escape()
{
    icalparameter *tzid;
    icalproperty *prop;

    tzid = icalparameter_new_tzid("Timezone\nwith a newline");
    prop = icalproperty_new_dtstart(icaltime_from_day_of_year(26, 2009));
    icalproperty_add_parameter(prop, tzid);

    if (VERBOSE)
        printf("%s\n", icalproperty_as_ical_string(prop));

    str_is("test encoding of 'Timezone\\nwith a newline'",
           icalproperty_as_ical_string(prop),
           "DTSTART;VALUE=DATE,TZID=Timezone\\nwith a newline:20090126");

    icalproperty_free(prop);
}

#endif

void test_requeststat()
{
    icalcomponent *c;
    icalproperty *p;
    icalrequeststatus s;
    struct icalreqstattype st, st2;
    char temp[1024];

    static const char test_icalcomp_str[] =
        "BEGIN:VEVENT\n"
        "REQUEST-STATUS:2.1;Success but fallback taken  on one or more property  values.;booga\n"
        "END:VEVENT\n";

    s = icalenum_num_to_reqstat(2, 1);

    ok("icalenum_num_to_reqstat(2,1)", (s == ICAL_2_1_FALLBACK_STATUS));

    ok("icalenum_reqstat_major()", (icalenum_reqstat_major(s) == 2));
    ok("icalenum_reqstat_minor()", (icalenum_reqstat_minor(s) == 1));

    str_is("icalenum_reqstat_desc() -> 2.1", icalenum_reqstat_desc(s),
           "Success but fallback taken  on one or more property  values.");

    st.code = s;
    st.debug = "booga";
    st.desc = 0;

    str_is("icalreqstattype_as_string()",
           icalreqstattype_as_string(st),
           "2.1;Success but fallback taken  on one or more property  values.;booga");

    st.desc = " A non-standard description";

    str_is("icalreqstattype_as_string() w/ non standard description",
           icalreqstattype_as_string(st), "2.1; A non-standard description;booga");

    st.desc = 0;

    snprintf(temp, sizeof(temp), "%s\n", icalreqstattype_as_string(st));

    st2 = icalreqstattype_from_string(
              "2.1;Success but fallback taken  on one or more property  values.;booga");

    /*    printf("%d --  %d --  %s -- %s\n", */
    ok("icalenum_reqstat_major()", (icalenum_reqstat_major(st2.code) == 2));
    ok("icalenum_reqstat_minor()", (icalenum_reqstat_minor(st2.code) == 1));
    str_is("icalenum_reqstat_desc",
           icalenum_reqstat_desc(st2.code),
           "Success but fallback taken  on one or more property  values.");

    st2 = icalreqstattype_from_string(
              "2.1;Success but fallback taken  on one or more property  values.;booga");
    if (VERBOSE) {
        printf("%s\n", icalreqstattype_as_string(st2));
    }

    st2 = icalreqstattype_from_string(
              "2.1;Success but fallback taken  on one or more property  values.;");
    if (VERBOSE) {
        printf("%s\n", icalreqstattype_as_string(st2));
    }

    st2 = icalreqstattype_from_string(
              "2.1;Success but fallback taken  on one or more property  values.");
    if (VERBOSE) {
        printf("%s\n", icalreqstattype_as_string(st2));
    }

    st2 = icalreqstattype_from_string("2.1;");
    if (VERBOSE) {
        printf("%s\n", icalreqstattype_as_string(st2));
    }

    str_is("st2 test again",
           icalreqstattype_as_string(st2),
           "2.1;Success but fallback taken  on one or more property  values.");

    st2 = icalreqstattype_from_string("2.1");
    str_is("st2 test #3",
           icalreqstattype_as_string(st2),
           "2.1;Success but fallback taken  on one or more property  values.");

    c = icalparser_parse_string((char *)test_icalcomp_str);
    ok("icalparser_parse_string()", (c != NULL));
    if (!c) {
        exit(EXIT_FAILURE);
    }

    if (VERBOSE) {
        printf("%s", icalcomponent_as_ical_string(c));
    }
    p = icalcomponent_get_first_property(c, ICAL_REQUESTSTATUS_PROPERTY);

#if ADD_TESTS_REQUIRING_INVESTIGATION
    str_is("icalproperty_new_from_string()",
           icalproperty_as_ical_string(p),
           "REQUEST-STATUS:2.1;Success but fallback taken  on one or more property  \r\n values.;booga\r\n");
#endif
    icalerror_set_error_state(ICAL_MALFORMEDDATA_ERROR, ICAL_ERROR_NONFATAL);
    st2 = icalreqstattype_from_string("16.4");

    ok("test unknown code", (st2.code == ICAL_UNKNOWN_STATUS));

    st2 = icalreqstattype_from_string("1.");

    ok("test malformed code", (st2.code == ICAL_UNKNOWN_STATUS));
    icalerror_set_error_state(ICAL_MALFORMEDDATA_ERROR, ICAL_ERROR_DEFAULT);

    icalproperty_free(p);
}

void test_dtstart()
{
    struct icaltimetype tt, tt2;

    icalproperty *p;

    tt = icaltime_from_string("19970101");

    int_is("19970101 is a date", tt.is_date, 1);

    p = icalproperty_new_dtstart(tt);

    if (VERBOSE)
        printf("%s\n", icalvalue_kind_to_string(icalvalue_isa(icalproperty_get_value(p))));

    ok("ICAL_DATE_VALUE", (icalvalue_isa(icalproperty_get_value(p)) == ICAL_DATE_VALUE));

    tt2 = icalproperty_get_dtstart(p);
    int_is("converted date is date", tt2.is_date, 1);

    if (VERBOSE)
        printf("%s\n", icalproperty_as_ical_string(p));

    tt = icaltime_from_string("19970101T103000");

    int_is("19970101T103000 is not a date", tt.is_date, 0);

    icalproperty_free(p);

    p = icalproperty_new_dtstart(tt);

    if (VERBOSE) {
        printf("%s\n", icalvalue_kind_to_string(icalvalue_isa(icalproperty_get_value(p))));
    }
    ok("ICAL_DATETIME_VALUE", (icalvalue_isa(icalproperty_get_value(p)) == ICAL_DATETIME_VALUE));

    tt2 = icalproperty_get_dtstart(p);
    int_is("converted datetime is not date", tt2.is_date, 0);

    if (VERBOSE)
        printf("%s\n", icalproperty_as_ical_string(p));

    icalproperty_free(p);
}

void do_test_time(const char *zone)
{
    struct icaltimetype ictt, icttutc, icttzone, icttdayl,
        icttla, icttny, icttphoenix, icttlocal, icttnorm;
    time_t tt, tt2, tt_p200;
    time_t offset_tz;
    icalvalue *v;
    short day_of_week, start_day_of_week, day_of_year;
    icaltimezone *azone, *utczone;
    char msg[256];

    icalerror_set_errors_are_fatal(0);

    azone = icaltimezone_get_builtin_timezone(zone);
    utczone = icaltimezone_get_utc_timezone();

    /* Test new API */
    if (VERBOSE) {
        printf("\n---> From time_t \n");
    }

    tt = 1025127869;    /* stick with a constant... Wed, 26 Jun 2002 21:44:29 GMT */

    if (VERBOSE) {
        printf("Orig        : %s\n", ical_timet_string(tt));
    }
    if (VERBOSE) {
        printf("\nicaltime_from_timet(tt,0) (DEPRECATED)\n");
    }

    ictt = icaltime_from_timet(tt, 0);

    str_is("icaltime_from_timet(1025127869) as UTC", ictt_as_string(ictt),
           "2002-06-26 21:44:29 (floating)");

    ictt = icaltime_from_timet_with_zone(tt, 0, NULL);
    str_is("Floating time from time_t", ictt_as_string(ictt), "2002-06-26 21:44:29 (floating)");

    ictt = icaltime_from_timet_with_zone(tt, 0, azone);
#if ADD_TESTS_REQUIRING_INVESTIGATION
    ok("icaltime_from_timet_with_zone(tt,0,zone) as zone",
       strncmp(ictt_as_string(ictt), "2002-06-26 21:44:29", 19) == 0);
#endif

    ictt = icaltime_from_timet_with_zone(tt, 0, utczone);

    str_is("icaltime_from_timet_with_zone(tt,0,utc)", ictt_as_string(ictt),
           "2002-06-26 21:44:29 Z UTC");

    if (VERBOSE)
        printf("\n---> Convert from floating \n");

    ictt = icaltime_from_timet_with_zone(tt, 0, NULL);
    icttutc = icaltime_convert_to_zone(ictt, utczone);

    str_is("Convert from floating to UTC", ictt_as_string(icttutc), "2002-06-26 21:44:29 Z UTC");

    icttzone = icaltime_convert_to_zone(ictt, azone);
    ok("Convert from floating to zone",
       (strncmp(ictt_as_string(icttzone), "2002-06-26 21:44:29", 19) == 0));

    if (VERBOSE)
        printf("\n---> Convert from UTC \n");

    ictt = icaltime_from_timet_with_zone(tt, 0, utczone);
    icttutc = icaltime_convert_to_zone(ictt, utczone);

    str_is("Convert from UTC to UTC", ictt_as_string(icttutc), "2002-06-26 21:44:29 Z UTC");

    icttzone = icaltime_convert_to_zone(ictt, azone);

    ok("Convert from UTC to zone (test year/mon only..)",
       (strncmp(ictt_as_string(icttzone), "2002-06-26 21:44:29", 7) == 0));

    tt2 = icaltime_as_timet_with_zone(icttzone, icttzone.zone);

    if (VERBOSE) {
        printf("No conversion: %s\n", ical_timet_string(tt2));
    }
    ok("No conversion at all (test year/mon only)",
       (strncmp(ical_timet_string(tt2), "2002-06-26 21:44:29 Z", 7) == 0));

    ok("test time conversion routines", (tt == tt2));

    if (VERBOSE) {
        printf("\n---> Convert from zone \n");
    }
    ictt = icaltime_from_timet_with_zone(tt, 0, azone);
    icttzone = icaltime_convert_to_zone(ictt, azone);

    if (VERBOSE) {
        printf("To zone      : %s\n", ictt_as_string(icttzone));
    }
    icttutc = icaltime_convert_to_zone(ictt, utczone);

    if (VERBOSE) {
        printf("To UTC      : %s\n", ictt_as_string(icttutc));
    }
    tt2 = icaltime_as_timet(icttutc);

    if (VERBOSE) {
        printf("No conversion: %s\n", ical_timet_string(tt2));
    }
    tt2 = icaltime_as_timet_with_zone(icttutc, azone);

    if (VERBOSE) {
        printf("Back to zone  : %s\n", ical_timet_string(tt2));
    }
    ok("test time conversion, round 2", (tt == tt2));

    ictt = icaltime_from_string("20001103T183030Z");

    tt = icaltime_as_timet(ictt);

    ok("test icaltime -> time_t for 20001103T183030Z", (tt == 973276230));
    /* Fri Nov  3 10:30:30 PST 2000 in PST
       Fri Nov  3 18:30:30 PST 2000 in UTC */

    if (VERBOSE) {
        printf(" Normalize \n");
        printf("Orig (ical) : %s\n", ictt_as_string(ictt));
    }
    icttnorm = ictt;
    icttnorm.second -= 60 * 60 * 24 * 5;
    icttnorm = icaltime_normalize(icttnorm);

    if (VERBOSE) {
        printf("-5d in sec  : %s\n", ictt_as_string(icttnorm));
    }
    icttnorm.day += 60;
    icttnorm = icaltime_normalize(icttnorm);

    if (VERBOSE) {
        printf("+60 d       : %s\n", ictt_as_string(icttnorm));
    }

    /** add test case here.. **/

    if (VERBOSE) {
        printf("\n As time_t \n");
    }

    tt2 = icaltime_as_timet(ictt);

    if (VERBOSE) {
        printf("20001103T183030Z (timet): %s\n", ical_timet_string(tt2));
        printf("20001103T183030Z        : %s\n", ictt_as_string(ictt));
    }

    /** this test is bogus **/
    ok("test normalization", (tt2 == tt));

    icttlocal = icaltime_convert_to_zone(ictt, azone);
    tt2 = icaltime_as_timet(icttlocal);
    if (VERBOSE) {
        printf("20001103T183030  (timet): %s\n", ical_timet_string(tt2));
        printf("20001103T183030         : %s\n", ictt_as_string(icttlocal));
    }

    offset_tz = (time_t) (-icaltimezone_get_utc_offset_of_utc_time(azone, &ictt, 0));
    if (VERBOSE)
        printf("offset_tz               : %ld\n", (long)offset_tz);

    ok("test utc offset", (tt - tt2 == offset_tz));

    /* FIXME with the new API, it's not very useful */
    icttlocal = ictt;
    icaltimezone_convert_time(&icttlocal,
                              icaltimezone_get_utc_timezone(),
                              icaltimezone_get_builtin_timezone(zone));

    if (VERBOSE)
        printf("As local    : %s\n", ictt_as_string(icttlocal));

    if (VERBOSE)
        printf("\n Convert to and from lib c \n");

    if (VERBOSE)
        printf("System time is: %s\n", ical_timet_string(tt));

    v = icalvalue_new_datetime(ictt);

    if (VERBOSE)
        printf("System time from libical: %s\n", icalvalue_as_ical_string(v));

    icalvalue_free(v);

    tt2 = icaltime_as_timet(ictt);

    if (VERBOSE)
        printf("Converted back to libc: %s\n", ical_timet_string(tt2));

    if (VERBOSE)
        printf("\n Incrementing time  \n");

    icttnorm = ictt;

    icttnorm.year++;
    tt2 = icaltime_as_timet(icttnorm);
    if (VERBOSE)
        printf("Add a year: %s\n", ical_timet_string(tt2));

    icttnorm.month += 13;
    tt2 = icaltime_as_timet(icttnorm);
    if (VERBOSE)
        printf("Add 13 months: %s\n", ical_timet_string(tt2));

    icttnorm.second += 90;
    tt2 = icaltime_as_timet(icttnorm);
    if (VERBOSE)
        printf("Add 90 seconds: %s\n", ical_timet_string(tt2));

    if (VERBOSE)
        printf("\n Day Of week \n");

    day_of_week = icaltime_day_of_week(ictt);
    start_day_of_week = icaltime_start_doy_of_week(ictt);
    day_of_year = icaltime_day_of_year(ictt);

    snprintf(msg, sizeof(msg), "Testing day of week %d", day_of_week);
    int_is(msg, day_of_week, 6);

    snprintf(msg, sizeof(msg), "Testing day of year %d", day_of_year);
    int_is(msg, day_of_year, 308);

    snprintf(msg, sizeof(msg), "Week started on doy of %d", start_day_of_week);
    int_is(msg, start_day_of_week, 303);

    if (VERBOSE)
        printf("\n TimeZone Conversions \n");

/*
    icttla = ictt;
    icaltimezone_convert_time(&icttla,
                              icaltimezone_get_utc_timezone(),
                              lazone);
*/
    icttla = icaltime_convert_to_zone(ictt,
                                      icaltimezone_get_builtin_timezone("America/Los_Angeles"));

#if ADD_TESTS_BROKEN_BUILTIN_TZDATA
    int_is("Converted hour in America/Los_Angeles is 10", icttla.hour, 10);
#endif
    icttutc = icaltime_convert_to_zone(icttla, icaltimezone_get_utc_timezone());

#if ADD_TESTS_BROKEN_BUILTIN_TZDATA
    ok("America/Los_Angeles local time is 2000-11-03 10:30:30",
       (strncmp(ictt_as_string(icttla), "2000-11-03 10:30:30", 19) == 0));
#endif
    ok("Test conversion back to UTC", (icaltime_compare(icttutc, ictt) == 0));

    icttny = icaltime_convert_to_zone(ictt, icaltimezone_get_builtin_timezone("America/New_York"));

    icttphoenix = icaltime_convert_to_zone(ictt,
                                           icaltimezone_get_builtin_timezone("America/Phoenix"));

    if (VERBOSE) {
        printf("Orig (ctime): %s\n", ical_timet_string(tt));
        printf("Orig (ical) : %s\n", ictt_as_string(ictt));
        printf("UTC         : %s\n", ictt_as_string(icttutc));
        printf("Los Angeles : %s\n", ictt_as_string(icttla));
        printf("Phoenix     : %s\n", ictt_as_string(icttphoenix));
        printf("New York    : %s\n", ictt_as_string(icttny));
    }
    /** @todo Check results for Phoenix here?... **/

    /* Daylight savings test for New York */
    if (VERBOSE) {
        printf("\n Daylight Savings \n");

        printf("Orig (ctime): %s\n", ical_timet_string(tt));
        printf("Orig (ical) : %s\n", ictt_as_string(ictt));
        printf("NY          : %s\n", ictt_as_string(icttny));
    }
#if ADD_TESTS_BROKEN_BUILTIN_TZDATA
    ok("Converted time in zone America/New_York is 2000-11-03 13:30:30",
       (strncmp(ictt_as_string(icttny), "2000-11-03 13:30:30", 19) == 0));
#endif
    tt_p200 = tt + 200 * 24 * 60 * 60;  /* Add 200 days */

    icttdayl = icaltime_from_timet_with_zone(tt_p200, 0, icaltimezone_get_utc_timezone());
    icttny = icaltime_convert_to_zone(icttdayl,
                                      icaltimezone_get_builtin_timezone("America/New_York"));

    if (VERBOSE) {
        printf("Orig +200d  : %s\n", ical_timet_string(tt_p200));
        printf("NY+200D     : %s\n", ictt_as_string(icttny));
    }

    ok("Converted time +200d in zone America/New_York is 2001-05-22 14:30:30",
       (strncmp(ictt_as_string(icttny), "2001-05-22 14:30:30", 19) == 0));

    /* Daylight savings test for Los Angeles */

    icttla = icaltime_convert_to_zone(ictt,
                                      icaltimezone_get_builtin_timezone("America/Los_Angeles"));

    if (VERBOSE) {
        printf("\nOrig (ctime): %s\n", ical_timet_string(tt));
        printf("Orig (ical) : %s\n", ictt_as_string(ictt));
        printf("LA          : %s\n", ictt_as_string(icttla));
    }
#if ADD_TESTS_BROKEN_BUILTIN_TZDATA
    ok("Converted time in zone America/Los_Angeles is 2000-11-03 10:30:30",
       (strncmp(ictt_as_string(icttla), "2000-11-03 10:30:30", 19) == 0));
#endif
    icttla = icaltime_convert_to_zone(icttdayl,
                                      icaltimezone_get_builtin_timezone("America/Los_Angeles"));

    if (VERBOSE) {
        printf("Orig +200d  : %s\n", ical_timet_string(tt_p200));
        printf("LA+200D     : %s\n", ictt_as_string(icttla));
    }

    ok("Converted time +200d in zone America/Los_Angeles is 2001-05-22 11:30:30",
       (strncmp(ictt_as_string(icttla), "2001-05-22 11:30:30", 19) == 0));

    icalerror_set_errors_are_fatal(1);

    /* Do some checks for icaltime_compare */
    if (VERBOSE) {
        printf("\n icaltime_compare() tests \n");
    }

    ictt = icaltime_from_string("20001203T183030");
    icttutc = icaltime_convert_to_zone(ictt, utczone);
    icttny = icaltime_convert_to_zone(ictt,
                                      icaltimezone_get_builtin_timezone("America/New_York"));

    int_is("icaltime_compare(): same float and UTC",
           icaltime_compare(ictt, icttutc),
           0);

    int_is("icaltime_compare(): same float and NY",
           icaltime_compare(ictt, icttny),
           0);

    /* The offset is set as from UTC: see issue #231 */
    icttny.hour = icttny.hour - 5;
    int_is("icaltime_compare(): different float and NY",
           icaltime_compare(ictt, icttny),
           1);

    int_is("icaltime_compare(): same UTC and NY",
           icaltime_compare(icttutc, icttny),
           0);
}

void test_iterators()
{
    icalcomponent *c, *inner, *next;
    icalcompiter i;
    char vevent_list[64] = "";
    char remaining_list[64] = "";

    const char *vevent_list_good = "12347";
    const char *remaining_list_good = "568910";

    int nomore = 1;

    c = icalcomponent_vanew(ICAL_VCALENDAR_COMPONENT,
                            icalcomponent_vanew(ICAL_VEVENT_COMPONENT,
                                                icalproperty_new_version("1"), (void *)0),
                            icalcomponent_vanew(ICAL_VEVENT_COMPONENT,
                                                icalproperty_new_version("2"), (void *)0),
                            icalcomponent_vanew(ICAL_VEVENT_COMPONENT,
                                                icalproperty_new_version("3"), (void *)0),
                            icalcomponent_vanew(ICAL_VEVENT_COMPONENT,
                                                icalproperty_new_version("4"), (void *)0),
                            icalcomponent_vanew(ICAL_VTODO_COMPONENT,
                                                icalproperty_new_version("5"), (void *)0),
                            icalcomponent_vanew(ICAL_VJOURNAL_COMPONENT,
                                                icalproperty_new_version("6"), (void *)0),
                            icalcomponent_vanew(ICAL_VEVENT_COMPONENT,
                                                icalproperty_new_version("7"), (void *)0),
                            icalcomponent_vanew(ICAL_VJOURNAL_COMPONENT,
                                                icalproperty_new_version("8"), (void *)0),
                            icalcomponent_vanew(ICAL_VJOURNAL_COMPONENT,
                                                icalproperty_new_version("9"), (void *)0),
                            icalcomponent_vanew(ICAL_VJOURNAL_COMPONENT,
                                                icalproperty_new_version("10"), (void *)0),
                            (void *)0);

    /* List all of the VEVENTS */

    for (i = icalcomponent_begin_component(c, ICAL_VEVENT_COMPONENT);
         icalcompiter_deref(&i) != 0; icalcompiter_next(&i)) {

        icalcomponent *this = icalcompiter_deref(&i);

        icalproperty *p = icalcomponent_get_first_property(this,
                                                           ICAL_VERSION_PROPERTY);
        const char *s = icalproperty_get_version(p);

        strncat(vevent_list, s, sizeof(vevent_list) - strlen(vevent_list) - 1);
    }
    str_is("iterate through VEVENTS in a component", vevent_list, vevent_list_good);

    /* Delete all of the VEVENTS */
    /* reset iterator */
    (void)icalcomponent_get_first_component(c, ICAL_VEVENT_COMPONENT);

    while ((inner = icalcomponent_get_current_component(c)) != 0) {
        if (icalcomponent_isa(inner) == ICAL_VEVENT_COMPONENT) {
            icalcomponent_remove_component(c, inner);
            icalcomponent_free(inner);
        } else {
            (void)icalcomponent_get_next_component(c, ICAL_VEVENT_COMPONENT);
        }
    }

    /* List all remaining components */
    for (inner = icalcomponent_get_first_component(c, ICAL_ANY_COMPONENT);
         inner != 0; inner = icalcomponent_get_next_component(c, ICAL_ANY_COMPONENT)) {

        icalproperty *p = icalcomponent_get_first_property(inner, ICAL_VERSION_PROPERTY);

        const char *s = icalproperty_get_version(p);

        strcat(remaining_list, s);
    }

    str_is("iterate through remaining components", remaining_list, remaining_list_good);

    /* Remove all remaining components */
    for (inner = icalcomponent_get_first_component(c, ICAL_ANY_COMPONENT);
         inner != 0;
         inner = next) {

        icalcomponent *this;
        icalproperty *p;

        next = icalcomponent_get_next_component(c, ICAL_ANY_COMPONENT);

        p = icalcomponent_get_first_property(inner, ICAL_VERSION_PROPERTY);
        (void)icalproperty_get_version(p);

        icalcomponent_remove_component(c, inner);

        this = icalcomponent_get_current_component(c);

        if (this != 0) {
            p = icalcomponent_get_first_property(this, ICAL_VERSION_PROPERTY);
            (void)icalproperty_get_version(p);
        }

        icalcomponent_free(inner);
    }

    /* List all remaining components */
    for (inner = icalcomponent_get_first_component(c, ICAL_ANY_COMPONENT);
         inner != 0; inner = icalcomponent_get_next_component(c, ICAL_ANY_COMPONENT)) {

        icalproperty *p = icalcomponent_get_first_property(inner, ICAL_VERSION_PROPERTY);

        const char *s = icalproperty_get_version(p);

        if (s)
            nomore = 0;
    }

    ok("test if any components remain after deleting the rest", nomore == 1);

    icalcomponent_free(c);
}

void test_time()
{
    const char *zones[6] =
    {
        "America/Los_Angeles", "America/New_York", "Europe/London", "Asia/Shanghai", NULL
    };

    int i;

    do_test_time(0);

    for (i = 0; zones[i] != NULL; i++) {

        if (VERBOSE)
            printf(" ######### Timezone: %s ############\n", zones[i]);

        do_test_time(zones[i]);
    }
}

void test_icalset()
{
    icalcomponent *c;

    icalset *f = icalset_new_file("2446.ics");
    icalset *d = icalset_new_dir("outdir");

    assert(f != 0);
    assert(d != 0);

    for (c = icalset_get_first_component(f); c != 0; c = icalset_get_next_component(f)) {

        icalcomponent *clone;

        clone = icalcomponent_new_clone(c);

        (void)icalset_add_component(d, clone);

        printf(" class %d\n", icalclassify(c, 0, "user"));
    }

    icalset_free(f);
    icalset_free(d);
}

void test_overlaps()
{
    icalcomponent *cset, *c;
    icalset *set;
    time_t tm1 = 973378800;     /*Sat Nov  4 23:00:00 UTC 2000,
                                   Sat Nov  4 15:00:00 PST 2000 */
    time_t tm2 = 973382400;     /*Sat Nov  5 00:00:00 UTC 2000
                                   Sat Nov  4 16:00:00 PST 2000 */

    time_t hh = 1800;   /* one half hour */

    icalfileset_options options = { O_RDONLY, 0644, 0, NULL };
    set = icalset_new(ICAL_FILE_SET, TEST_DATADIR "/overlaps.ics", &options);

    c = icalcomponent_vanew(ICAL_VEVENT_COMPONENT,
                            icalproperty_vanew_dtstart(icaltime_from_timet(tm1 - hh, 0), (void *)0),
                            icalproperty_vanew_dtend(icaltime_from_timet(tm2 - hh, 0), (void *)0),
                            0);

    cset = icalclassify_find_overlaps(set, c);
#if ADD_TESTS_REQUIRING_INVESTIGATION
    ok("TODO find overlaps 1", (cset != NULL));
#endif

    if (VERBOSE && cset) {
        printf("%s\n", icalcomponent_as_ical_string(cset));
    }

    if (cset)
        icalcomponent_free(cset);

    if (c)
        icalcomponent_free(c);

    c = icalcomponent_vanew(
            ICAL_VEVENT_COMPONENT,
            icalproperty_vanew_dtstart(icaltime_from_timet(tm1 - hh, 0), (void *)0),
            icalproperty_vanew_dtend(icaltime_from_timet(tm2, 0), (void *)0),
            0);

    cset = icalclassify_find_overlaps(set, c);

#if ADD_TESTS_REQUIRING_INVESTIGATION
    ok("TODO find overlaps 1", cset != NULL);
#endif
    if (VERBOSE && cset)
        printf("%s\n", icalcomponent_as_ical_string(cset));

    if (cset)
        icalcomponent_free(cset);

    if (c)
        icalcomponent_free(c);

    c = icalcomponent_vanew(
            ICAL_VEVENT_COMPONENT,
            icalproperty_vanew_dtstart(icaltime_from_timet(tm1 + 5 * hh, 0), (void *)0),
            icalproperty_vanew_dtend(icaltime_from_timet(tm2 + 5 * hh, 0), (void *)0),
            0);

    cset = icalclassify_find_overlaps(set, c);
#if ADD_TESTS_REQUIRING_INVESTIGATION
    ok("TODO find overlaps 1", cset != NULL);
#endif
    if (VERBOSE && cset)
        printf("%s\n", icalcomponent_as_ical_string(cset));

    if (set)
        icalset_free(set);

    if (cset)
        icalcomponent_free(cset);

    if (c)
        icalcomponent_free(c);
}

void test_fblist()
{
    icalspanlist *sl, *new_sl;
    icalfileset_options options = { O_RDONLY, 0644, 0, NULL };
    icalset *set = icalset_new(ICAL_FILE_SET, TEST_DATADIR "/spanlist.ics", &options);
    struct icalperiodtype period;
    icalcomponent *comp;
    int *foo;
    int i;

    sl = icalspanlist_new(set,
                          icaltime_from_string("19980101T000000Z"),
                          icaltime_from_string("19980108T000000Z"));
    ok("create initial spanlist", (sl != NULL));

    ok("open ../test-data/spanlist.ics", (set != NULL));
    assert(set != NULL);

    if (VERBOSE) {
        printf("Restricted spanlist\n");
    }
    if (VERBOSE) {
        icalspanlist_dump(sl);
    }

    period = icalspanlist_next_free_time(sl, icaltime_from_string("19970801T120000"));

    str_is("Next Free time start 19970801T120000", icaltime_as_ical_string(period.start),
           "19970801T120000");
    str_is("Next Free time end   19980101T000000", icaltime_as_ical_string(period.end),
           "19980101T000000");

    period = icalspanlist_next_free_time(sl, period.end);

    str_is("Next Free time start 19980101T010000", icaltime_as_ical_string(period.start),
           "19980101T010000");
    str_is("Next Free time end   19980102T010000", icaltime_as_ical_string(period.end),
           "19980102T010000");

    if (VERBOSE) {
        printf("%s\n",
               icalcomponent_as_ical_string(icalspanlist_as_vfreebusy(
                   sl, "a@foo.com", "b@foo.com")));
    }

    foo = icalspanlist_as_freebusy_matrix(sl, 3600);

    for (i = 0; foo[i] != -1; i++);     /* find number entries */

    int_is("Calculating freebusy hourly matrix", i, (7 * 24));

    if (VERBOSE) {
        for (i = 0; foo[i] != -1; i++) {
            printf("%d", foo[i]);
            if ((i % 24) == 23)
                printf("\n");
        }
        printf("\n\n");
    }

    free(foo);

    foo = icalspanlist_as_freebusy_matrix(sl, 3600 * 24);

    ok("Calculating daily freebusy matrix", (foo != NULL));

	if (foo) {
        char out_str[80] = "";
        char *strp = out_str;

        for (i = 0; foo[i] != -1; i++) {
            snprintf(strp, 80, "%1d", foo[i]);
            strp++;
        }
        str_is("Checking freebusy validity", out_str, "1121110");

		if (VERBOSE) {
			for (i = 0; foo[i] != -1; i++) {
				printf("%d", foo[i]);
				if ((i % 7) == 6)
					printf("\n");
			}
			printf("\n\n");
		}
		free(foo);
	}

    icalspanlist_free(sl);

    if (VERBOSE)
        printf("Unrestricted spanlist\n");

    sl = icalspanlist_new(set, icaltime_from_string("19970324T120000Z"), icaltime_null_time());

    ok("add 19970324T120000Z to spanlist", (sl != NULL));

    if (VERBOSE) {
        printf("Restricted spanlist\n");
    }
    if (VERBOSE) {
        icalspanlist_dump(sl);
    }

    period = icalspanlist_next_free_time(sl, icaltime_from_string("19970801T120000Z"));

    str_is("Next Free time start 19980101T010000",
           icaltime_as_ical_string(period.start), "19980101T010000");

    str_is("Next Free time end   19980102T010000",
           icaltime_as_ical_string(period.end), "19980102T010000");

    comp = icalspanlist_as_vfreebusy(sl, "a@foo.com", "b@foo.com");

    ok("Calculating VFREEBUSY component", (comp != NULL));
    if (VERBOSE) {
        printf("%s\n", icalcomponent_as_ical_string(comp));
    }

    new_sl = icalspanlist_from_vfreebusy(comp);

    ok("Calculating spanlist from generated VFREEBUSY component", (new_sl != NULL));

    if (VERBOSE) {
        icalspanlist_dump(new_sl);
    }

    if (sl)
        icalspanlist_free(sl);

    if (new_sl)
        icalspanlist_free(new_sl);

    if (comp)
        icalcomponent_free(comp);

    icalset_free(set);
}

void test_convenience()
{
    icalcomponent *c;
    int duration;
    struct icaltimetype tt;

    c = icalcomponent_vanew(
            ICAL_VCALENDAR_COMPONENT,
            icalcomponent_vanew(
                ICAL_VEVENT_COMPONENT,
                icalproperty_new_dtstart(icaltime_from_string("19970801T120000")),
                icalproperty_new_dtend(icaltime_from_string("19970801T130000")),
                (void *)0),
            (void *)0);

    if (VERBOSE)
        printf("\n%s\n", icalcomponent_as_ical_string(c));

    duration = icaldurationtype_as_int(icalcomponent_get_duration(c)) / 60;

    str_is("Start is 1997-08-01 12:00:00 (floating)",
           ictt_as_string(icalcomponent_get_dtstart(c)), "1997-08-01 12:00:00 (floating)");
    str_is("End is 1997-08-01 13:00:00 (floating)",
           ictt_as_string(icalcomponent_get_dtend(c)), "1997-08-01 13:00:00 (floating)");
    ok("Duration is 60 m", (duration == 60));

    icalcomponent_free(c);

    c = icalcomponent_vanew(
            ICAL_VCALENDAR_COMPONENT,
            icalcomponent_vanew(
                ICAL_VEVENT_COMPONENT,
                icalproperty_new_dtstart(icaltime_from_string("19970801T120000Z")),
                icalproperty_new_duration(icaldurationtype_from_string("PT1H30M")),
                (void *)0),
            (void *)0);

    if (VERBOSE)
        printf("\n%s\n", icalcomponent_as_ical_string(c));

    duration = icaldurationtype_as_int(icalcomponent_get_duration(c)) / 60;

    str_is("Start is 1997-08-01 12:00:00 Z UTC",
           ictt_as_string(icalcomponent_get_dtstart(c)), "1997-08-01 12:00:00 Z UTC");
    str_is("End is 1997-08-01 13:30:00 Z UTC",
           ictt_as_string(icalcomponent_get_dtend(c)), "1997-08-01 13:30:00 Z UTC");
    ok("Duration is 90 m", (duration == 90));

    icalcomponent_free(c);

    icalerror_set_errors_are_fatal(0);

    c = icalcomponent_vanew(
            ICAL_VCALENDAR_COMPONENT,
            icalcomponent_vanew(
                ICAL_VEVENT_COMPONENT,
                icalproperty_new_dtstart(icaltime_from_string("19970801T120000")),
                icalproperty_new_dtend(icaltime_from_string("19970801T130000")),
                (void *)0),
            (void *)0);

    icalcomponent_set_duration(c, icaldurationtype_from_string("PT1H30M"));

    if (VERBOSE)
        printf("\n%s\n", icalcomponent_as_ical_string(c));

    duration = icaldurationtype_as_int(icalcomponent_get_duration(c)) / 60;

    str_is("Start is 1997-08-01 12:00:00 (floating)",
           ictt_as_string(icalcomponent_get_dtstart(c)), "1997-08-01 12:00:00 (floating)");
    str_is("End is 1997-08-01 13:00:00 (floating)",
           ictt_as_string(icalcomponent_get_dtend(c)), "1997-08-01 13:00:00 (floating)");
    ok("Duration is 60 m", (duration == 60));

    icalcomponent_free(c);

    c = icalcomponent_vanew(
            ICAL_VCALENDAR_COMPONENT,
            icalcomponent_vanew(
                ICAL_VEVENT_COMPONENT,
                icalproperty_new_dtstart(icaltime_from_string("19970801T120000Z")),
                icalproperty_new_duration(icaldurationtype_from_string("PT1H30M")),
                (void *)0),
            (void *)0);

    icalcomponent_set_dtend(c, icaltime_from_string("19970801T133000Z"));

    if (VERBOSE)
        printf("\n%s\n", icalcomponent_as_ical_string(c));

    duration = icaldurationtype_as_int(icalcomponent_get_duration(c)) / 60;

    ok("Start is 1997-08-01 12:00:00 Z UTC",
       (0 == strcmp("1997-08-01 12:00:00 Z UTC", ictt_as_string(icalcomponent_get_dtstart(c)))));
    ok("End is 1997-08-01 13:30:00 Z UTC",
       (0 == strcmp("1997-08-01 13:30:00 Z UTC", ictt_as_string(icalcomponent_get_dtend(c)))));
    ok("Duration is 90 m", (duration == 90));

    icalerror_set_errors_are_fatal(1);

    icalcomponent_free(c);

    c = icalcomponent_vanew(ICAL_VCALENDAR_COMPONENT,
                            icalcomponent_vanew(ICAL_VEVENT_COMPONENT, (void *)0), (void *)0);

    icalcomponent_set_dtstart(c, icaltime_from_string("19970801T120000Z"));
    icalcomponent_set_dtend(c, icaltime_from_string("19970801T133000Z"));

    if (VERBOSE)
        printf("\n%s\n", icalcomponent_as_ical_string(c));

    duration = icaldurationtype_as_int(icalcomponent_get_duration(c)) / 60;

    ok("Start is 1997-08-01 12:00:00 Z UTC",
       (0 == strcmp("1997-08-01 12:00:00 Z UTC", ictt_as_string(icalcomponent_get_dtstart(c)))));
    ok("End is 1997-08-01 13:30:00 Z UTC",
       (0 == strcmp("1997-08-01 13:30:00 Z UTC", ictt_as_string(icalcomponent_get_dtend(c)))));
    ok("Duration is 90 m", (duration == 90));

    icalcomponent_free(c);

    c = icalcomponent_vanew(ICAL_VCALENDAR_COMPONENT,
                            icalcomponent_vanew(ICAL_VEVENT_COMPONENT, (void *)0), (void *)0);

    icalcomponent_set_dtstart(c, icaltime_from_string("19970801T120000Z"));
    icalcomponent_set_duration(c, icaldurationtype_from_string("PT1H30M"));

    if (VERBOSE)
        printf("\n%s\n", icalcomponent_as_ical_string(c));

    duration = icaldurationtype_as_int(icalcomponent_get_duration(c)) / 60;

    ok("Start is 1997-08-01 12:00:00 Z UTC",
       (0 == strcmp("1997-08-01 12:00:00 Z UTC", ictt_as_string(icalcomponent_get_dtstart(c)))));
    ok("End is 1997-08-01 13:30:00 Z UTC",
       (0 == strcmp("1997-08-01 13:30:00 Z UTC", ictt_as_string(icalcomponent_get_dtend(c)))));
    ok("Duration is 90 m", (duration == 90));

    icalcomponent_free(c);

    c = icalcomponent_vanew(ICAL_VCALENDAR_COMPONENT,
                            icalcomponent_vanew(ICAL_VEVENT_COMPONENT, (void *)0), (void *)0);

    tt = icaltime_from_string("19970801T120000");
    (void)icaltime_set_timezone(&tt, icaltimezone_get_builtin_timezone("Europe/Rome"));
    icalcomponent_set_dtstart(c, tt);

    if (VERBOSE)
        printf("\n%s\n", icalcomponent_as_ical_string(c));

    icalcomponent_set_duration(c, icaldurationtype_from_string("PT1H30M"));
    duration = icaldurationtype_as_int(icalcomponent_get_duration(c)) / 60;

#if ADD_TESTS_BROKEN_BUILTIN_TZDATA
    ok("Start is 1997-08-01 12:00:00 Europe/Rome",
       (0 == strcmp("1997-08-01 12:00:00 /softwarestudio.org/Europe/Rome",
                    ictt_as_string(icalcomponent_get_dtstart(c)))));
    ok("End is 1997-08-01 13:30:00 Europe/Rome",
       (0 == strcmp("1997-08-01 13:30:00 /softwarestudio.org/Europe/Rome",
                    ictt_as_string(icalcomponent_get_dtend(c)))));
#endif
    ok("Duration is 90 m", (duration == 90));

    icalcomponent_free(c);
}

void test_time_parser()
{
    struct icaltimetype tt;

    icalerror_set_errors_are_fatal(0);

    tt = icaltime_from_string("19970101T1000");
    ok("19970101T1000 is null time", icaltime_is_null_time(tt));

    tt = icaltime_from_string("19970101X100000");
    ok("19970101X100000 is null time", icaltime_is_null_time(tt));

    tt = icaltime_from_string("19970101T100000");
    ok("19970101T100000 is valid", !icaltime_is_null_time(tt));

    if (VERBOSE)
        printf("%s\n", icaltime_as_ctime(tt));

    tt = icaltime_from_string("19970101T100000Z");

    ok("19970101T100000Z is valid", !icaltime_is_null_time(tt));
    if (VERBOSE)
        printf("%s\n", icaltime_as_ctime(tt));

    tt = icaltime_from_string("19970101");
    ok("19970101 is valid", (!icaltime_is_null_time(tt)));

    if (VERBOSE)
        printf("%s\n", icaltime_as_ctime(tt));

    icalerror_set_errors_are_fatal(1);
}

void test_recur_parser()
{
    struct icalrecurrencetype rt;
    const char *str;

    str =
        "FREQ=YEARLY;UNTIL=20000131T090000Z;BYDAY=-1TU,3WE,-4FR,SA,SU;BYYEARDAY=34,65,76,78;BYMONTH=1,2,3,4,8";
    rt = icalrecurrencetype_from_string(str);
    str_is(str, icalrecurrencetype_as_string(&rt), str);

    str = "FREQ=DAILY;COUNT=3;BYDAY=-1TU,3WE,-4FR,SA,SU;BYYEARDAY=34,65,76,78;BYMONTH=1,2,3,4,8";

    rt = icalrecurrencetype_from_string(str);
    str_is(str, icalrecurrencetype_as_string(&rt), str);
}

char *ical_strstr(const char *haystack, const char *needle)
{
    return strstr(haystack, needle);
}

void test_start_of_week()
{
    struct icaltimetype tt2;
    struct icaltimetype tt1 = icaltime_from_string("19900110");
    int dow, doy, start_dow;

    do {
        tt1 = icaltime_normalize(tt1);

        doy = icaltime_start_doy_of_week(tt1);
        dow = icaltime_day_of_week(tt1);

        tt2 = icaltime_from_day_of_year(doy, tt1.year);
        start_dow = icaltime_day_of_week(tt2);

        if (doy == 1) {
            char msg[128];

            snprintf(msg, sizeof(msg), "%s", ictt_as_string(tt1));
            int_is(msg, start_dow, 1);
        }

        if (start_dow != 1) {   /* Sunday is 1 */
            printf("failed: Start of week (%s) is not a Sunday \n for %s (doy=%d,dow=%d)\n",
                   ictt_as_string(tt2), ictt_as_string(tt1), dow, start_dow);
        }

        assert(start_dow == 1);

        tt1.day += 1;

    } while (tt1.year < 2010);
}

void test_doy()
{
    struct icaltimetype tt1, tt2;
    short doy, doy2;
    char msg[128];

    tt1 = icaltime_from_string("19900101");

    if (VERBOSE)
        printf("Test icaltime_day_of_year() agreement with mktime\n");

    do {
        struct tm stm;

        tt1 = icaltime_normalize(tt1);

        stm.tm_sec = tt1.second;
        stm.tm_min = tt1.minute;
        stm.tm_hour = tt1.hour;
        stm.tm_mday = tt1.day;
        stm.tm_mon = tt1.month - 1;
        stm.tm_year = tt1.year - 1900;
        stm.tm_isdst = -1;

        (void)mktime(&stm);

        doy = icaltime_day_of_year(tt1);

        doy2 = stm.tm_yday + 1;

        if (doy == 1) {
          /** show some test cases **/
            snprintf(msg, sizeof(msg), "Year %d - mktime() compare", tt1.year);
            int_is(msg, doy, doy2);
        }

        if (doy != doy2) {
            printf("Failed for %s (%d,%d)\n", ictt_as_string(tt1), doy, doy2);
        }
        assert(doy == doy2);

        tt1.day += 1;

    } while (tt1.year < 2010);

    if (VERBOSE)
        printf("\nTest icaltime_day_of_year() agreement with icaltime_from_day_of_year()\n");

    tt1 = icaltime_from_string("19900101");

    do {
        if (doy == 1) {
          /** show some test cases **/
            snprintf(msg, sizeof(msg), "Year %d - icaltime_day_of_year() compare", tt1.year);
            int_is(msg, doy, doy2);
        }

        doy = icaltime_day_of_year(tt1);
        tt2 = icaltime_from_day_of_year(doy, tt1.year);
        doy2 = icaltime_day_of_year(tt2);

        assert(doy2 == doy);
        assert(icaltime_compare(tt1, tt2) == 0);

        tt1.day += 1;
        tt1 = icaltime_normalize(tt1);

    } while (tt1.year < 2010);

    tt1 = icaltime_from_string("19950301");
    doy = icaltime_day_of_year(tt1);
    tt2 = icaltime_from_day_of_year(doy, 1995);
    if (VERBOSE) {
        printf("%d %s %s\n", doy, icaltime_as_ctime(tt1), icaltime_as_ctime(tt2));
    }
    ok("test 19950301", (tt2.day == 1 && tt2.month == 3));
    ok("day of year == 60", (doy == 60));

    tt1 = icaltime_from_string("19960301");
    doy = icaltime_day_of_year(tt1);
    tt2 = icaltime_from_day_of_year(doy, 1996);
    if (VERBOSE) {
        printf("%d %s %s\n", doy, icaltime_as_ctime(tt1), icaltime_as_ctime(tt2));
    }
    ok("test 19960301", (tt2.day == 1 && tt2.month == 3));
    ok("day of year == 61", (doy == 61));

    tt1 = icaltime_from_string("19970301");
    doy = icaltime_day_of_year(tt1);
    tt2 = icaltime_from_day_of_year(doy, 1997);
    if (VERBOSE) {
        printf("%d %s %s\n", doy, icaltime_as_ctime(tt1), icaltime_as_ctime(tt2));
    }
    ok("test 19970301", (tt2.day == 1 && tt2.month == 3));
    ok("day of year == 60", (doy == 60));
}

void test_x()
{
    static const char test_icalcomp_str[] =
        "BEGIN:VEVENT\r\n"
        "RRULE\r\n"
        " ;X-EVOLUTION-ENDDATE=20030209T081500\r\n"
        " :FREQ=DAILY;COUNT=10;INTERVAL=6\r\n" "X-COMMENT;X-FOO=BAR: Booga\r\n" "END:VEVENT\r\n";

    icalcomponent *icalcomp;
    icalproperty *prop;
    struct icalrecurrencetype recur;
    int n_errors;

    icalcomp = icalparser_parse_string((char *)test_icalcomp_str);
    assert(icalcomp != NULL);

    if (VERBOSE)
        printf("%s\n\n", icalcomponent_as_ical_string(icalcomp));

    n_errors = icalcomponent_count_errors(icalcomp);
    int_is("icalparser_parse_string()", n_errors, 0);

    if (n_errors) {
      /** NOT USED **/
        icalproperty *p;

        for (p = icalcomponent_get_first_property(icalcomp, ICAL_XLICERROR_PROPERTY);
             p;
             p = icalcomponent_get_next_property(icalcomp, ICAL_XLICERROR_PROPERTY)) {
            const char *str;
            str = icalproperty_as_ical_string(p);
            fprintf(stderr, "error: %s\n", str);
        }
    }

    prop = icalcomponent_get_first_property(icalcomp, ICAL_RRULE_PROPERTY);
    ok("get RRULE property", (prop != NULL));
    assert(prop != NULL);

    recur = icalproperty_get_rrule(prop);

    if (VERBOSE)
        printf("%s\n", icalrecurrencetype_as_string(&recur));

    icalcomponent_free(icalcomp);
}

void test_gauge_sql()
{
    icalgauge *g;
    const char *str;

    str =
        "SELECT DTSTART,DTEND,COMMENT FROM VEVENT,VTODO WHERE VEVENT.SUMMARY = 'Bongoa' AND SEQUENCE < 5";

    g = icalgauge_new_from_sql((char *)str, 0);
    ok(str, (g != NULL));
    if (VERBOSE)
        icalgauge_dump(g);

    icalgauge_free(g);

    str =
        "SELECT * FROM VEVENT,VTODO WHERE VEVENT.SUMMARY = 'Bongoa' AND SEQUENCE < 5 OR METHOD != 'CREATE'";

    g = icalgauge_new_from_sql((char *)str, 0);
    ok(str, (g != NULL));
    if (VERBOSE)
        icalgauge_dump(g);

    icalgauge_free(g);

    str = "SELECT * FROM VEVENT WHERE SUMMARY == 'BA301'";

    g = icalgauge_new_from_sql((char *)str, 0);
    ok(str, (g != NULL));
    if (VERBOSE)
        icalgauge_dump(g);

    icalgauge_free(g);

    str = "SELECT * FROM VEVENT WHERE SUMMARY == 'BA301'";

    g = icalgauge_new_from_sql((char *)str, 0);
    ok(str, (g != NULL));
    if (VERBOSE)
        icalgauge_dump(g);

    icalgauge_free(g);

    str = "SELECT * FROM VEVENT WHERE LOCATION == '104 Forum'";

    g = icalgauge_new_from_sql((char *)str, 0);
    ok(str, (g != NULL));
    if (VERBOSE)
        icalgauge_dump(g);

    icalgauge_free(g);
}

void test_gauge_compare()
{
    icalgauge *g;
    icalcomponent *c;
    const char *str;

    /* Equality */

    c = icalcomponent_vanew(
            ICAL_VCALENDAR_COMPONENT,
            icalcomponent_vanew(
                ICAL_VEVENT_COMPONENT,
                icalproperty_new_dtstart(icaltime_from_string("20000101T000002")), 0),
            (void *)0);

	str = "SELECT * FROM VEVENT WHERE DTSTART = '20000101T000002'";
    g = icalgauge_new_from_sql((char *)str, 0);

    ok(str, (c != 0 && g != 0));
    assert(c != 0);
    assert(g != 0);

    int_is("compare", icalgauge_compare(g, c), 1);

    icalgauge_free(g);

	str = "SELECT * FROM VEVENT WHERE DTSTART = '20000101T000001'";
    g = icalgauge_new_from_sql((char *)str, 0);

    ok(str, (g != 0));

    assert(g != 0);
    int_is("compare", icalgauge_compare(g, c), 0);

    icalgauge_free(g);

	str = "SELECT * FROM VEVENT WHERE DTSTART != '20000101T000003'";
    g = icalgauge_new_from_sql((char *)str, 0);

    ok(str, (c != 0 && g != 0));

    assert(g != 0);
    int_is("compare", icalgauge_compare(g, c), 1);

    icalgauge_free(g);

    /* Less than */

	str = "SELECT * FROM VEVENT WHERE DTSTART < '20000101T000003'";
    g = icalgauge_new_from_sql((char *)str, 0);

    ok(str, (c != 0 && g != 0));

    int_is("compare", icalgauge_compare(g, c), 1);

    assert(g != 0);
    icalgauge_free(g);

	str = "SELECT * FROM VEVENT WHERE DTSTART < '20000101T000002'";
    g = icalgauge_new_from_sql((char *)str, 0);

    ok(str, (g != 0));

    assert(g != 0);
    int_is("compare", icalgauge_compare(g, c), 0);

    icalgauge_free(g);

    /* Greater than */

	str = "SELECT * FROM VEVENT WHERE DTSTART > '20000101T000001'";
    g = icalgauge_new_from_sql((char *)str, 0);

    ok(str, (g != 0));

    assert(g != 0);
    int_is("compare", icalgauge_compare(g, c), 1);

    icalgauge_free(g);

	str = "SELECT * FROM VEVENT WHERE DTSTART > '20000101T000002'";
    g = icalgauge_new_from_sql((char *)str, 0);

    ok(str, (g != 0));

    assert(g != 0);
    int_is("compare", icalgauge_compare(g, c), 0);

    icalgauge_free(g);

    /* Greater than or Equal to */

	str = "SELECT * FROM VEVENT WHERE DTSTART >= '20000101T000002'";
    g = icalgauge_new_from_sql((char *)str, 0);

    ok(str, (g != 0));

    int_is("compare", icalgauge_compare(g, c), 1);

    icalgauge_free(g);

	str = "SELECT * FROM VEVENT WHERE DTSTART >= '20000101T000003'";
    g = icalgauge_new_from_sql((char *)str, 0);

    ok(str, (g != 0));

    int_is("compare", icalgauge_compare(g, c), 0);

    icalgauge_free(g);

    /* Less than or Equal to */

	str = "SELECT * FROM VEVENT WHERE DTSTART <= '20000101T000002'";
    g = icalgauge_new_from_sql((char *)str, 0);

    ok(str, (g != 0));

    assert(g != 0);
    int_is("compare", icalgauge_compare(g, c), 1);

    icalgauge_free(g);

	str = "SELECT * FROM VEVENT WHERE DTSTART <= '20000101T000001'";
    g = icalgauge_new_from_sql((char *)str, 0);

    ok(str, (g != 0));

    int_is("compare", icalgauge_compare(g, c), 0);

    icalgauge_free(g);

    icalcomponent_free(c);

    /* Combinations */

    c = icalcomponent_vanew(
            ICAL_VCALENDAR_COMPONENT,
            icalcomponent_vanew(
                ICAL_VEVENT_COMPONENT,
                icalproperty_new_dtstart(icaltime_from_string("20000102T000000")), 0),
            (void *)0);

    str = "SELECT * FROM VEVENT WHERE DTSTART > '20000101T000000' and DTSTART < '20000103T000000'";

    g = icalgauge_new_from_sql((char *)str, 0);
    ok(str, (g != 0));
    int_is("compare", icalgauge_compare(g, c), 1);

    icalgauge_free(g);

    str = "SELECT * FROM VEVENT WHERE DTSTART > '20000101T000000' and DTSTART < '20000102T000000'";

    g = icalgauge_new_from_sql((char *)str, 0);
    ok(str, (g != 0));
    int_is("compare", icalgauge_compare(g, c), 0);

    icalgauge_free(g);

    str = "SELECT * FROM VEVENT WHERE DTSTART > '20000101T000000' or DTSTART < '20000102T000000'";

    g = icalgauge_new_from_sql((char *)str, 0);
    ok(str, (g != 0));
    int_is("compare", icalgauge_compare(g, c), 1);

    icalgauge_free(g);

    icalcomponent_free(c);

    /* Combinations, non-cannonical component */

    c = icalcomponent_vanew(ICAL_VEVENT_COMPONENT,
                            icalproperty_new_dtstart(icaltime_from_string("20000102T000000")),
                            (void *)0);

    str = "SELECT * FROM VEVENT WHERE DTSTART > '20000101T000000' and DTSTART < '20000103T000000'";

    g = icalgauge_new_from_sql((char *)str, 0);
    ok(str, (g != 0));
    int_is("compare", icalgauge_compare(g, c), 1);

    icalgauge_free(g);

    str = "SELECT * FROM VEVENT WHERE DTSTART > '20000101T000000' and DTSTART < '20000102T000000'";

    g = icalgauge_new_from_sql((char *)str, 0);
    ok(str, (g != 0));
    int_is("compare", icalgauge_compare(g, c), 0);

    icalgauge_free(g);

    str = "SELECT * FROM VEVENT WHERE DTSTART > '20000101T000000' or DTSTART < '20000102T000000'";

    g = icalgauge_new_from_sql((char *)str, 0);
    ok(str, (g != 0));
    int_is("compare", icalgauge_compare(g, c), 1);

    icalgauge_free(g);
    icalcomponent_free(c);

    /* Complex comparisons */

    c = icalcomponent_vanew(
            ICAL_VCALENDAR_COMPONENT,
            icalproperty_new_method(ICAL_METHOD_REQUEST),
            icalcomponent_vanew(
                ICAL_VEVENT_COMPONENT,
                icalproperty_new_dtstart(icaltime_from_string("20000101T000002")),
                icalproperty_new_comment("foo"),
                icalcomponent_vanew(
                    ICAL_VALARM_COMPONENT,
                    icalproperty_new_dtstart(icaltime_from_string("20000101T120000")),
                    (void *)0),
                (void *)0),
            (void *)0);

    str = "SELECT * FROM VEVENT WHERE VALARM.DTSTART = '20000101T120000'";

    g = icalgauge_new_from_sql((char *)str, 0);
    ok(str, (g != 0));
    int_is("compare", icalgauge_compare(g, c), 1);

    icalgauge_free(g);

    str = "SELECT * FROM VEVENT WHERE COMMENT = 'foo'";
    g = icalgauge_new_from_sql((char *)str, 0);
    ok(str, (g != 0));
    int_is("compare", icalgauge_compare(g, c), 1);

    icalgauge_free(g);

    str = "SELECT * FROM VEVENT WHERE COMMENT = 'foo' AND  VALARM.DTSTART = '20000101T120000'";
    g = icalgauge_new_from_sql((char *)str, 0);
    ok(str, (g != 0));
    int_is("compare", icalgauge_compare(g, c), 1);

    icalgauge_free(g);

    str = "SELECT * FROM VEVENT WHERE COMMENT = 'bar' AND  VALARM.DTSTART = '20000101T120000'";
    g = icalgauge_new_from_sql((char *)str, 0);
    ok(str, (g != 0));
    int_is("compare", icalgauge_compare(g, c), 0);

    icalgauge_free(g);

    str = "SELECT * FROM VEVENT WHERE COMMENT = 'bar' or  VALARM.DTSTART = '20000101T120000'";
    g = icalgauge_new_from_sql((char *)str, 0);
    ok(str, (g != 0));
    int_is("compare", icalgauge_compare(g, c), 1);

    icalgauge_free(g);

    icalcomponent_free(c);
}

icalcomponent *make_component(int i)
{
    icalcomponent *c;

    struct icaltimetype t = icaltime_from_string("20000101T120000Z");

    t.day += i;

    (void)icaltime_normalize(t);

    c = icalcomponent_vanew(ICAL_VCALENDAR_COMPONENT,
                            icalproperty_new_method(ICAL_METHOD_REQUEST),
                            icalcomponent_vanew(ICAL_VEVENT_COMPONENT,
                                                icalproperty_new_dtstart(t), (void *)0), (void *)0);

    assert(c != 0);

    return c;
}

void test_fileset()
{
#if defined(HAVE_UNLINK)
    icalset *fs;
    icalcomponent *c;
    int i;
    int comp_count = 0;
    const char *path = "test_fileset.ics";
    icalgauge *g =
        icalgauge_new_from_sql(
			(char *)
			"SELECT * FROM VEVENT "
            "WHERE DTSTART > '20000103T120000Z' AND "
            "DTSTART <= '20000106T120000Z'",
            0);

    ok("icalgauge_new_from_sql()", (g != NULL));

    unlink(path);

    fs = icalfileset_new(path);

    ok("icalfileset_new()", (fs != NULL));
    assert(fs != 0);

    for (i = 0; i != 10; i++) {
        c = make_component(i);
        (void)icalfileset_add_component(fs, c);
    }

    (void)icalfileset_commit(fs);

    icalset_free(fs);
    /** reopen fileset.ics **/
    fs = icalfileset_new(path);

    if (VERBOSE)
        printf("== No Selections \n");

    comp_count = 0;
    for (c = icalfileset_get_first_component(fs); c != 0; c = icalfileset_get_next_component(fs)) {
        struct icaltimetype t = icalcomponent_get_dtstart(c);

        comp_count++;
        if (VERBOSE)
            printf("%s\n", icaltime_as_ctime(t));
    }
    int_is("icalfileset get components", comp_count, 10);

    (void)icalfileset_select(fs, g);

    if (VERBOSE)
        printf("\n== DTSTART > '20000103T120000Z' AND DTSTART <= '20000106T120000Z' \n");

    comp_count = 0;
    for (c = icalfileset_get_first_component(fs); c != 0; c = icalfileset_get_next_component(fs)) {
        struct icaltimetype t = icalcomponent_get_dtstart(c);

        comp_count++;
        if (VERBOSE)
            printf("%s\n", icaltime_as_ctime(t));
    }
    int_is("icalfileset get components with gauge", comp_count, 3);

    icalset_free(fs);

    /*icalgauge_free(g); */
#endif
}

void microsleep(int us)
{       /*us is in microseconds */
#if defined(HAVE_NANOSLEEP)
    struct timespec ts;

    ts.tv_sec = 0;
    ts.tv_nsec = us * 1000;     /* convert from microseconds to nanoseconds */

    nanosleep(&ts, NULL);
#elif defined(HAVE_USLEEP)
    usleep(us);
#endif /*Windows Sleep is useless for microsleeping */
}

void test_file_locks()
{
#if defined(HAVE_WAITPID) && defined(HAVE_FORK) && defined(HAVE_UNLINK)
    pid_t pid;
    const char *path = "test_fileset_locktest.ics";
    icalset *fs;
    icalcomponent *c, *c2;
    struct icaldurationtype d;
    int i;
    int final, sec = 0;

    icalerror_clear_errno();

    unlink(path);

    fs = icalfileset_new(path);

    if (icalfileset_get_first_component(fs) == 0) {
        c = make_component(0);

        d = icaldurationtype_from_int(1);

        icalcomponent_set_duration(c, d);

        (void)icalfileset_add_component(fs, c);

        c2 = icalcomponent_new_clone(c);

        (void)icalfileset_add_component(fs, c2);

        (void)icalfileset_commit(fs);
    }

    icalset_free(fs);

    assert(icalerrno == ICAL_NO_ERROR);

    pid = fork();

    assert(pid >= 0);

    if (pid == 0) {
        /*child */
        int i;

        microsleep(rand() / (RAND_MAX / 100));

        for (i = 0; i < 50; i++) {
            fs = icalfileset_new(path);

            assert(fs != 0);

            c = icalfileset_get_first_component(fs);

            assert(c != 0);

            d = icalcomponent_get_duration(c);
            d = icaldurationtype_from_int(icaldurationtype_as_int(d) + 1);

            icalcomponent_set_duration(c, d);
            icalcomponent_set_summary(c, "Child");

            c2 = icalcomponent_new_clone(c);
            icalcomponent_set_summary(c2, "Child");
            (void)icalfileset_add_component(fs, c2);

            icalfileset_mark(fs);
            (void)icalfileset_commit(fs);

            icalset_free(fs);

            microsleep(rand() / (RAND_MAX / 20));
        }

        exit(0);

    } else {
        /* parent */
        int i;

        for (i = 0; i < 50; i++) {
            fs = icalfileset_new(path);

            assert(fs != 0);

            c = icalfileset_get_first_component(fs);

            assert(c != 0);

            d = icalcomponent_get_duration(c);
            d = icaldurationtype_from_int(icaldurationtype_as_int(d) + 1);

            icalcomponent_set_duration(c, d);
            icalcomponent_set_summary(c, "Parent");

            c2 = icalcomponent_new_clone(c);
            icalcomponent_set_summary(c2, "Parent");
            (void)icalfileset_add_component(fs, c2);

            icalfileset_mark(fs);
            (void)icalfileset_commit(fs);
            icalset_free(fs);

            putc('.', stdout);
            fflush(stdout);
        }
    }

    assert(waitpid(pid, 0, 0) == pid);

    fs = icalfileset_new(path);

    i = 1;

    c = icalfileset_get_first_component(fs);
    final = icaldurationtype_as_int(icalcomponent_get_duration(c));
    for (c = icalfileset_get_next_component(fs); c != 0; c = icalfileset_get_next_component(fs)) {
        struct icaldurationtype d = icalcomponent_get_duration(c);

        sec = icaldurationtype_as_int(d);

        /*printf("%d,%d ",i,sec); */
        assert(i == sec);
        i++;
    }

    printf("\nFinal: %d\n", final);

    assert(sec == final);
    _unused(sec);
#endif
}

void test_action()
{
    icalcomponent *c;
    icalproperty *p;
    char *str;

    static const char test_icalcomp_str[] =
        "BEGIN:VEVENT\r\n"
        "ACTION:EMAIL\r\n"
        "ACTION:PROCEDURE\r\n" "ACTION:AUDIO\r\n" "ACTION:FUBAR\r\n" "END:VEVENT\r\n";

    c = icalparser_parse_string((char *)test_icalcomp_str);

    ok("icalparser_parse_string(), ACTIONS", (c != NULL));
    assert(c != 0);

    str = icalcomponent_as_ical_string(c);
    str_is("icalcomponent_as_ical_string()", str, ((char *)test_icalcomp_str));
    if (VERBOSE)
        printf("%s\n\n", str);

    p = icalcomponent_get_first_property(c, ICAL_ACTION_PROPERTY);

    ok("ICAL_ACTION_EMAIL", (icalproperty_get_action(p) == ICAL_ACTION_EMAIL));

    p = icalcomponent_get_next_property(c, ICAL_ACTION_PROPERTY);

    ok("ICAL_ACTION_PROCEDURE", (icalproperty_get_action(p) == ICAL_ACTION_PROCEDURE));

    p = icalcomponent_get_next_property(c, ICAL_ACTION_PROPERTY);

    ok("ICAL_ACTION_AUDIO", (icalproperty_get_action(p) == ICAL_ACTION_AUDIO));

    p = icalcomponent_get_next_property(c, ICAL_ACTION_PROPERTY);

    ok("ICAL_ACTION_X", (icalproperty_get_action(p) == ICAL_ACTION_X));
    str_is("ICAL_ACTION -> FUBAR", icalvalue_get_x(icalproperty_get_value(p)), "FUBAR");
    icalcomponent_free(c);
}

void test_trigger()
{
    struct icaltriggertype tr;
    icalcomponent *c;
    icalproperty *p;
    const char *str;

    static const char test_icalcomp_str[] =
        "BEGIN:VEVENT\r\n"
        "TRIGGER;VALUE=DATE-TIME:19980403T120000\r\n"
        "TRIGGER:-PT15M\r\n"
        "TRIGGER;VALUE=DATE-TIME:19980403T120000\r\n"
        "TRIGGER;VALUE=DURATION:-PT15M\r\n" "END:VEVENT\r\n";

    c = icalparser_parse_string((char *)test_icalcomp_str);
    ok("icalparser_parse_string()", (c != NULL));
    assert(c != NULL);

    str_is("parsed triggers", icalcomponent_as_ical_string(c), (char *)test_icalcomp_str);

    for (p = icalcomponent_get_first_property(c, ICAL_TRIGGER_PROPERTY);
         p != 0; p = icalcomponent_get_next_property(c, ICAL_TRIGGER_PROPERTY)) {
        tr = icalproperty_get_trigger(p);

        if (!icaltime_is_null_time(tr.time)) {
            if (VERBOSE)
                printf("value=DATE-TIME:%s\n", icaltime_as_ical_string(tr.time));
        } else {
            if (VERBOSE)
                printf("value=DURATION:%s\n", icaldurationtype_as_ical_string(tr.duration));
        }
    }

    icalcomponent_free(c);

    /* Trigger, as a DATETIME */
    tr.duration = icaldurationtype_null_duration();
    tr.time = icaltime_from_string("19970101T120000");
    p = icalproperty_new_trigger(tr);
    str = icalproperty_as_ical_string(p);

    str_is("TRIGGER;VALUE=DATE-TIME:19970101T120000", str,
           "TRIGGER;VALUE=DATE-TIME:19970101T120000\r\n");
    icalproperty_free(p);

    /* TRIGGER, as a DURATION */
    tr.time = icaltime_null_time();
    tr.duration = icaldurationtype_from_string("P3DT3H50M45S");
    p = icalproperty_new_trigger(tr);
    str = icalproperty_as_ical_string(p);

    str_is("TRIGGER:P3DT3H50M45S", str, "TRIGGER:P3DT3H50M45S\r\n");
    icalproperty_free(p);

    /* TRIGGER, as a DATETIME, VALUE=DATETIME */
    tr.duration = icaldurationtype_null_duration();
    tr.time = icaltime_from_string("19970101T120000");
    p = icalproperty_new_trigger(tr);
    icalproperty_add_parameter(p, icalparameter_new_value(ICAL_VALUE_DATETIME));
    str = icalproperty_as_ical_string(p);

    str_is("TRIGGER;VALUE=DATE-TIME:19970101T120000", str,
           "TRIGGER;VALUE=DATE-TIME:19970101T120000\r\n");
    icalproperty_free(p);

    /*TRIGGER, as a DURATION, VALUE=DATETIME */
    tr.time = icaltime_null_time();
    tr.duration = icaldurationtype_from_string("P3DT3H50M45S");
    p = icalproperty_new_trigger(tr);
    icalproperty_add_parameter(p, icalparameter_new_value(ICAL_VALUE_DURATION));

    str = icalproperty_as_ical_string(p);

    str_is("TRIGGER;VALUE=DURATION:P3DT3H50M45S", str, "TRIGGER;VALUE=DURATION:P3DT3H50M45S\r\n");
    icalproperty_free(p);

    /* TRIGGER, as a DATETIME, VALUE=DURATION */
    tr.duration = icaldurationtype_null_duration();
    tr.time = icaltime_from_string("19970101T120000");
    p = icalproperty_new_trigger(tr);
    icalproperty_add_parameter(p, icalparameter_new_value(ICAL_VALUE_DATETIME));
    str = icalproperty_as_ical_string(p);

    str_is("TRIGGER;VALUE=DATE-TIME:19970101T120000", str,
           "TRIGGER;VALUE=DATE-TIME:19970101T120000\r\n");
    icalproperty_free(p);

    /*TRIGGER, as a DURATION, VALUE=DURATION */
    tr.time = icaltime_null_time();
    tr.duration = icaldurationtype_from_string("P3DT3H50M45S");
    p = icalproperty_new_trigger(tr);
    icalproperty_add_parameter(p, icalparameter_new_value(ICAL_VALUE_DURATION));

    str = icalproperty_as_ical_string(p);

    str_is("TRIGGER;VALUE=DURATION:P3DT3H50M45S", str, "TRIGGER;VALUE=DURATION:P3DT3H50M45S\r\n");
    icalproperty_free(p);

    /* TRIGGER, as a DATETIME, VALUE=BINARY  */
    tr.duration = icaldurationtype_null_duration();
    tr.time = icaltime_from_string("19970101T120000");
    p = icalproperty_new_trigger(tr);
    icalproperty_add_parameter(p, icalparameter_new_value(ICAL_VALUE_BINARY));
    str = icalproperty_as_ical_string(p);

    str_is("TRIGGER;VALUE=BINARY:19970101T120000", str, "TRIGGER;VALUE=BINARY:19970101T120000\r\n");
    icalproperty_free(p);

    /*TRIGGER, as a DURATION, VALUE=BINARY   */
    tr.time = icaltime_null_time();
    tr.duration = icaldurationtype_from_string("P3DT3H50M45S");
    p = icalproperty_new_trigger(tr);
    icalproperty_add_parameter(p, icalparameter_new_value(ICAL_VALUE_BINARY));

    str = icalproperty_as_ical_string(p);

    str_is("TRIGGER;VALUE=BINARY:P3DT3H50M45S", str, "TRIGGER;VALUE=BINARY:P3DT3H50M45S\r\n");
    icalproperty_free(p);
}

void test_rdate()
{
    struct icaldatetimeperiodtype dtp;
    icalproperty *p;
    const char *str;
    struct icalperiodtype period;

    period.start = icaltime_from_string("19970101T120000");
    period.end = icaltime_null_time();
    period.duration = icaldurationtype_from_string("PT3H10M15S");

    /* RDATE, as DATE-TIME */
    dtp.time = icaltime_from_string("19970101T120000");
    dtp.period = icalperiodtype_null_period();
    p = icalproperty_new_rdate(dtp);
    str = icalproperty_as_ical_string(p);

    str_is("RDATE as DATE-TIME", str, "RDATE:19970101T120000\r\n");
    icalproperty_free(p);

    /* RDATE, as PERIOD */
    dtp.time = icaltime_null_time();
    dtp.period = period;
    p = icalproperty_new_rdate(dtp);

    str = icalproperty_as_ical_string(p);
    str_is("RDATE, as PERIOD", str, "RDATE;VALUE=PERIOD:19970101T120000/PT3H10M15S\r\n");
    icalproperty_free(p);

    /* RDATE, as DATE-TIME, VALUE=DATE-TIME */
    dtp.time = icaltime_from_string("19970101T120000");
    dtp.period = icalperiodtype_null_period();
    p = icalproperty_new_rdate(dtp);
    icalproperty_add_parameter(p, icalparameter_new_value(ICAL_VALUE_DATETIME));
    str = icalproperty_as_ical_string(p);

    str_is("RDATE, as DATE-TIME, VALUE=DATE-TIME", str,
           "RDATE;VALUE=DATE-TIME:19970101T120000\r\n");
    icalproperty_free(p);

    /* RDATE, as PERIOD, VALUE=DATE-TIME */
    dtp.time = icaltime_null_time();
    dtp.period = period;
    p = icalproperty_new_rdate(dtp);
    icalproperty_add_parameter(p, icalparameter_new_value(ICAL_VALUE_PERIOD));
    str = icalproperty_as_ical_string(p);
    str_is("RDATE, as PERIOD, VALUE=DATE-TIME", str,
           "RDATE;VALUE=PERIOD:19970101T120000/PT3H10M15S\r\n");
    icalproperty_free(p);

    /* RDATE, as DATE-TIME, VALUE=PERIOD */
    dtp.time = icaltime_from_string("19970101T120000");
    dtp.period = icalperiodtype_null_period();
    p = icalproperty_new_rdate(dtp);
    icalproperty_add_parameter(p, icalparameter_new_value(ICAL_VALUE_DATETIME));
    str = icalproperty_as_ical_string(p);

    str_is("RDATE, as DATE-TIME, VALUE=PERIOD", str, "RDATE;VALUE=DATE-TIME:19970101T120000\r\n");
    icalproperty_free(p);

    /* RDATE, as PERIOD, VALUE=PERIOD */
    dtp.time = icaltime_null_time();
    dtp.period = period;
    p = icalproperty_new_rdate(dtp);
    icalproperty_add_parameter(p, icalparameter_new_value(ICAL_VALUE_PERIOD));
    str = icalproperty_as_ical_string(p);

    str_is("RDATE, as PERIOD, VALUE=PERIOD", str,
           "RDATE;VALUE=PERIOD:19970101T120000/PT3H10M15S\r\n");
    icalproperty_free(p);

    /* RDATE, as DATE-TIME, VALUE=BINARY */
    dtp.time = icaltime_from_string("19970101T120000");
    dtp.period = icalperiodtype_null_period();
    p = icalproperty_new_rdate(dtp);
    icalproperty_add_parameter(p, icalparameter_new_value(ICAL_VALUE_BINARY));
    str = icalproperty_as_ical_string(p);

    str_is("RDATE, as DATE-TIME, VALUE=BINARY", str, "RDATE;VALUE=BINARY:19970101T120000\r\n");
    icalproperty_free(p);

    /* RDATE, as PERIOD, VALUE=BINARY */
    dtp.time = icaltime_null_time();
    dtp.period = period;
    p = icalproperty_new_rdate(dtp);
    icalproperty_add_parameter(p, icalparameter_new_value(ICAL_VALUE_BINARY));
    str = icalproperty_as_ical_string(p);

    str_is("RDATE, as PERIOD, VALUE=BINARY", str,
           "RDATE;VALUE=BINARY:19970101T120000/PT3H10M15S\r\n");
    icalproperty_free(p);
}

void test_langbind()
{
    icalcomponent *c, *inner;
    icalproperty *p;
    char *test_str_parsed;
    static const char test_str[] =
        "BEGIN:VEVENT\n"
        "ATTENDEE;RSVP=TRUE;ROLE=REQ-PARTICIPANT;CUTYPE=GROUP:MAILTO:employee-A@host.com\n"
        "COMMENT: Comment that \n spans a line\n"
        "COMMENT: Comment with \"quotable\" \'characters\' and other \t bad magic \n things \f Yeah.\n"
        "DTSTART:19970101T120000\n"
        "DTSTART:19970101T120000Z\n"
        "DTSTART:19970101\n"
        "DURATION:P3DT4H25M\n"
        "FREEBUSY:19970101T120000/19970101T120000\n"
        "FREEBUSY:19970101T120000/P3DT4H25M\n" "END:VEVENT\n";

    static const char *test_str_parsed_good =
        "BEGIN:VEVENT\r\n"
        "ATTENDEE;RSVP=TRUE;ROLE=REQ-PARTICIPANT;CUTYPE=GROUP:MAILTO:\r\n"
        " employee-A@host.com\r\n"
        "COMMENT:Comment that spans a line\r\n"
        "COMMENT:Comment with \"quotable\" 'characters' and other 	 bad magic things \r\n"
        "  Yeah.\r\n"
        "DTSTART:19970101T120000\r\n"
        "DTSTART:19970101T120000Z\r\n"
        "DTSTART;VALUE=DATE:19970101\r\n"
        "DURATION:P3DT4H25M\r\n"
        "FREEBUSY:19970101T120000/19970101T120000\r\n"
        "FREEBUSY:19970101T120000/P3DT4H25M\r\n" "END:VEVENT\r\n";

    if (VERBOSE)
        printf("%s\n", test_str);

    c = icalparser_parse_string(test_str);

    ok("icalparser_parse_string()", (c != NULL));
    assert(c != NULL);

    test_str_parsed = icalcomponent_as_ical_string(c);

    str_is("parsed version with bad chars, etc", test_str_parsed, test_str_parsed_good);

    inner = icalcomponent_get_inner(c);

    for (p = icallangbind_get_first_property(inner, "ANY");
         p != 0; p = icallangbind_get_next_property(inner, "ANY")
        ) {

        const char *str = icallangbind_property_eval_string(p, (char *)":");

      /** TODO add tests **/
        if (VERBOSE)
            printf("%s\n", str);
    }

    p = icalcomponent_get_first_property(inner, ICAL_ATTENDEE_PROPERTY);

    icalproperty_set_parameter_from_string(p, "CUTYPE", "INDIVIDUAL");

    str_is("Set attendee parameter",
           icalproperty_as_ical_string(p),
           "ATTENDEE;RSVP=TRUE;ROLE=REQ-PARTICIPANT;CUTYPE=INDIVIDUAL:MAILTO:\r\n"
           " employee-A@host.com\r\n");

    icalproperty_set_value_from_string(p, "mary@foo.org", "TEXT");

    str_is("Set attendee parameter value",
           icalproperty_as_ical_string(p),
           "ATTENDEE;VALUE=TEXT;RSVP=TRUE;ROLE=REQ-PARTICIPANT;CUTYPE=INDIVIDUAL:\r\n"
           " mary@foo.org\r\n");

    icalcomponent_free(c);
}

void test_property_parse()
{
    icalcomponent *c;
    icalproperty *p;
    const char *str;

    static const char test_icalcomp_str[] =
        "BEGIN:VEVENT\n"
        "ATTENDEE;RSVP=TRUE;ROLE=REQ-PARTICIPANT;CUTYPE=GROUP:MAILTO:employee-A@host.com\n"
        "DTSTART:19970101T120000Z\n" "END:VEVENT\n";

    c = icalparser_parse_string((char *)test_icalcomp_str);
    ok("icalparser_parse_string()", (c != NULL));
    if (!c) {
        exit(EXIT_FAILURE);
    }

    p = icalcomponent_get_first_property(c, ICAL_ATTENDEE_PROPERTY);

    ok("icalproperty_from_string(), ATTENDEE", (p != 0));

    str = icalproperty_as_ical_string(p);
    if (VERBOSE)
        printf("%s\n", str);

    icalproperty_free(p);

    p = icalcomponent_get_first_property(c, ICAL_DTSTART_PROPERTY);

    ok("icalproperty_from_string(), simple DTSTART", (p != 0));

    str = icalproperty_as_ical_string(p);
    if (VERBOSE)
        printf("%s\n", str);

    icalproperty_free(p);
}

void test_value_parameter()
{
    icalcomponent *c;
    icalproperty *p;
    icalparameter *param;

    static const char test_icalcomp_str[] =
        "BEGIN:VEVENT\n"
        "DTSTART;VALUE=DATE-TIME:19971123T123000\n"
        "DTSTART;VALUE=DATE:19971123\n" "DTSTART;VALUE=FOO:19971123T123000\n" "END:VEVENT\n";

    c = icalparser_parse_string((char *)test_icalcomp_str);
    ok("icalparser_parse_string()", (c != NULL));
    if (!c) {
        exit(EXIT_FAILURE);
    }

    if (VERBOSE)
        printf("%s", icalcomponent_as_ical_string(c));

    p = icalcomponent_get_first_property(c, ICAL_DTSTART_PROPERTY);
    param = icalproperty_get_first_parameter(p, ICAL_VALUE_PARAMETER);

    ok("icalproperty_get_value()", (icalparameter_get_value(param) == ICAL_VALUE_DATETIME));

    p = icalcomponent_get_next_property(c, ICAL_DTSTART_PROPERTY);
    param = icalproperty_get_first_parameter(p, ICAL_VALUE_PARAMETER);
    ok("icalproperty_get_first_parameter()", (icalparameter_get_value(param) == ICAL_VALUE_DATE));

    icalcomponent_free(c);
}

void test_x_parameter()
{
    icalcomponent *c;
    icalproperty *p;

    static const char test_icalcomp_str[] =
        "BEGIN:VEVENT\n" "COMMENT;X-A=1;X-B=2:\\sThis is a note\n" "END:VEVENT\n";

    c = icalparser_parse_string((char *)test_icalcomp_str);
    ok("icalparser_parse_string()", (c != NULL));
    if (!c) {
        exit(EXIT_FAILURE);
    }

    if (VERBOSE)
        printf("%s", icalcomponent_as_ical_string(c));

    p = icalcomponent_get_first_property(c, ICAL_COMMENT_PROPERTY);
    icalproperty_set_parameter_from_string(p, "X-LIES", "no");
    icalproperty_set_parameter_from_string(p, "X-LAUGHS", "big");
    icalproperty_set_parameter_from_string(p, "X-TRUTH", "yes");
    icalproperty_set_parameter_from_string(p, "X-HUMOUR", "bad");

    if (VERBOSE)
        printf("%s\n", icalproperty_as_ical_string(p));

    str_is("COMMENT parses param", icalproperty_get_comment(p), " This is a note");

    str_is("Check X-LIES", icalproperty_get_parameter_as_string(p, "X-LIES"), "no");
    str_is("Check X-LAUGHS", icalproperty_get_parameter_as_string(p, "X-LAUGHS"), "big");
    str_is("Check X-TRUTH", icalproperty_get_parameter_as_string(p, "X-TRUTH"), "yes");
    str_is("Check X-HUMOUR", icalproperty_get_parameter_as_string(p, "X-HUMOUR"), "bad");

    icalcomponent_free(c);
}

void test_x_property()
{
    icalcomponent *c;
    icalproperty *p;

    static const char test_icalcomp_str[] =
        "BEGIN:VEVENT\n" "X-LIC-PROPERTY:\\sThis is a note\n" "END:VEVENT\n";

    c = icalparser_parse_string((char *)test_icalcomp_str);
    ok("icalparser_parse_string()", (c != NULL));
    if (!c) {
        exit(EXIT_FAILURE);
    }

    if (VERBOSE)
        printf("%s", icalcomponent_as_ical_string(c));

    p = icalcomponent_get_first_property(c, ICAL_X_PROPERTY);
    ok("x-property is correct kind", (icalproperty_isa(p) == ICAL_X_PROPERTY));
    str_is("icalproperty_get_x_name() works", icalproperty_get_x_name(p), "X-LIC-PROPERTY");
    str_is("icalproperty_get_x() works", icalproperty_get_x(p), " This is a note");

    icalcomponent_free(c);
}

void test_utcoffset()
{
    icalcomponent *c;

    static const char test_icalcomp_str[] =
        "BEGIN:VTIMEZONE\n" "TZOFFSETFROM:-001608\n" "END:VTIMEZONE\n";

    c = icalparser_parse_string((char *)test_icalcomp_str);
    ok("parse TZOFFSETFROM:-001608", (c != NULL));

    if (VERBOSE && c)
        printf("%s", icalcomponent_as_ical_string(c));

    if (c)
        icalcomponent_free(c);
}

void test_attach()
{
    icalcomponent *c;

    static const char test_icalcomp_str[] =
        "BEGIN:VEVENT\n"
        "ATTACH:CID:jsmith.part3.960817T083000.xyzMain@host1.com\n"
        "ATTACH:FMTTYPE=application/postscript;ftp://xyzCorp.com/pub/reports/r-960812.ps\n"
        "END:VEVENT\n";

    c = icalparser_parse_string((char *)test_icalcomp_str);
    ok("parse simple attachment", (c != NULL));

    if (VERBOSE && c)
        printf("%s", icalcomponent_as_ical_string(c));

    if (c)
        icalcomponent_free(c);
}

void test_attach_caldav()
{
    icalcomponent *c;
    icalproperty *p;
    icalparameter *param;

    static const char test_icalcomp_str_caldav[] =
        "BEGIN:VEVENT\n"
        "ATTACH;MANAGED-ID=e474a36f22;FMTTYPE=image/jpeg;SIZE=281639;FILENAME=2doimage.jpg:https://www.someurl.com/somefile.jpg\n"
        "END:VEVENT\n";

    c = icalparser_parse_string((char *)test_icalcomp_str_caldav);
    ok("parse caldav attachment", (c != NULL));

    if (VERBOSE && c)
        printf("%s", icalcomponent_as_ical_string(c));

    p = icalcomponent_get_first_property(c, ICAL_ATTACH_PROPERTY);
    ok("property is correct kind (attach)", (icalproperty_isa(p) == ICAL_ATTACH_PROPERTY));

    param = icalproperty_get_first_parameter(p, ICAL_ANY_PARAMETER);
    if (VERBOSE && param) {
        printf("MANAGED-ID = %s\n", icalparameter_get_managedid(param));
    }
    str_is("managed-id", icalparameter_get_managedid(param), "e474a36f22");

    param = icalproperty_get_next_parameter(p, ICAL_ANY_PARAMETER);
    if (VERBOSE && param) {
        printf("FMTTYPE = %s\n", icalparameter_get_fmttype(param));
    }
    str_is("fmttype", icalparameter_get_fmttype(param), "image/jpeg");

    param = icalproperty_get_next_parameter(p, ICAL_ANY_PARAMETER);
    if (VERBOSE && param) {
        printf("SIZE = %s\n", icalparameter_get_size(param));
    }
    str_is("size", icalparameter_get_size(param), "281639");

    param = icalproperty_get_next_parameter(p, ICAL_ANY_PARAMETER);
    if (VERBOSE && param) {
        printf("FILENAME = %s\n", icalparameter_get_filename(param));
    }
    str_is("filename", icalparameter_get_filename(param), "2doimage.jpg");

    if (VERBOSE) {
        printf("URI = %s\n", icalattach_get_url(icalproperty_get_attach(p)));
    }
    str_is("attach url", icalattach_get_url(icalproperty_get_attach(p)),
           "https://www.someurl.com/somefile.jpg");

    if (p)
        icalproperty_free(p);

    if (c)
        icalcomponent_free(c);
}

void test_attach_url()
{
    static const char test_icalcomp_str_attachwithurl[] =
        "BEGIN:VALARM\r\n" "ATTACH:foofile\r\n" "END:VALARM\r\n";

    icalattach *attach = icalattach_new_from_url("foofile");
    icalcomponent *ac = icalcomponent_new(ICAL_VALARM_COMPONENT);
    icalproperty *ap = icalproperty_new_attach(attach);

    icalcomponent_add_property(ac, ap);
    if (VERBOSE) {
        printf("%s\n", icalcomponent_as_ical_string(ac));
    }
    str_is("attach url", icalattach_get_url(attach), "foofile");
    str_is("attach with url", icalcomponent_as_ical_string(ac), test_icalcomp_str_attachwithurl);
    icalproperty_free(ap);
    icalcomponent_free(ac);
}

void test_attach_data()
{
    static const char test_icalcomp_str_attachwithdata[] =
        "BEGIN:VALARM\r\n" "ATTACH;VALUE=BINARY:foofile\r\n" "END:VALARM\r\n";

    icalattach *attach = icalattach_new_from_data("foofile", NULL, 0);
    icalcomponent *ac = icalcomponent_new(ICAL_VALARM_COMPONENT);
    icalproperty *ap = icalproperty_new_attach(attach);

    icalcomponent_add_property(ac, ap);
    if (VERBOSE) {
        printf("%s\n", icalcomponent_as_ical_string(ac));
    }
    str_is("attach data", (const char *) icalattach_get_data(attach), "foofile");
    str_is("attach with data", icalcomponent_as_ical_string(ac), test_icalcomp_str_attachwithdata);
    icalproperty_free(ap);
    icalcomponent_free(ac);
}

void test_vcal(void)
{
    VObject *vcal;
    icalcomponent *comp;
    const char *file = TEST_DATADIR "/user-cal.vcf";

    vcal = Parse_MIME_FromFileName((char *)file);

    ok("Parsing " TEST_DATADIR "/user-cal.vcf", (vcal != 0));

	if (vcal) {
		comp = icalvcal_convert(vcal);

		ok("Converting to ical component", (comp != 0));

		if (VERBOSE && comp)
			printf("%s\n", icalcomponent_as_ical_string(comp));

		if (comp)
			icalcomponent_free(comp);

        deleteVObject(vcal);
	}
}

/*
 * Test to see if recurrences are excluded in certain situations
 * See r961 for more information
 */
void test_recurrenceexcluded(void)
{
    char funTime[2048];
    icalcomponent *calendar = NULL;
    icalcomponent *event = NULL;
    struct icaltimetype dtstart;
    struct icaltimetype recurtime;

    funTime[0] = '\0';
    strcat(funTime, "BEGIN:VCALENDAR\n");
    strcat(funTime, "VERSION:2.0\n");
    strcat(funTime, "BEGIN:VTIMEZONE\n");
    strcat(funTime, "TZID:/mozilla.org/20071231_1/Europe/London\n");
    strcat(funTime, "X-LIC-LOCATION:Europe/London\n");
    strcat(funTime, "BEGIN:DAYLIGHT\n");
    strcat(funTime, "TZOFFSETFROM:+0000\n");
    strcat(funTime, "TZOFFSETTO:+0100\n");
    strcat(funTime, "TZNAME:BST\n");
    strcat(funTime, "DTSTART:19700328T230000\n");
    strcat(funTime, "RRULE:FREQ=YEARLY;INTERVAL=1;BYDAY=-1SU;BYMONTH=3\n");
    strcat(funTime, "END:DAYLIGHT\n");
    strcat(funTime, "BEGIN:STANDARD\n");
    strcat(funTime, "TZOFFSETFROM:+0100\n");
    strcat(funTime, "TZOFFSETTO:+0000\n");
    strcat(funTime, "TZNAME:GMT\n");
    strcat(funTime, "DTSTART:19701025T000000\n");
    strcat(funTime, "RRULE:FREQ=YEARLY;INTERVAL=1;BYDAY=-1SU;BYMONTH=10\n");
    strcat(funTime, "END:STANDARD\n");
    strcat(funTime, "END:VTIMEZONE\n");
    strcat(funTime, "BEGIN:VEVENT\n");
    strcat(funTime, "DTSTAMP:20080805T174443Z\n");
    strcat(funTime, "UID:5fb6ccb8-9646-45ab-8c95-8d15e9de1280\n");
    strcat(funTime, "SUMMARY:Exclude test\n");
    strcat(funTime, "EXDATE;TZID=/mozilla.org/20071231_1/Europe/London:20080818T190000\n");
    strcat(funTime, "EXDATE:20080819T180000Z\n");
    strcat(funTime, "RRULE:FREQ=DAILY;COUNT=12;INTERVAL=1;BYDAY=MO,TU,WE,TH,FR\n");
    strcat(funTime, "DTSTART;TZID=/mozilla.org/20071231_1/Europe/London:20080811T190000\n");
    strcat(funTime, "DTEND;TZID=/mozilla.org/20071231_1/Europe/London:20080811T200000\n");
    strcat(funTime, "END:VEVENT\n");
    strcat(funTime, "END:VCALENDAR\n");
    calendar = icalparser_parse_string(funTime);
    dtstart = icalcomponent_get_dtstart(calendar);
    event = icalcomponent_get_first_component(calendar, ICAL_VEVENT_COMPONENT);
    recurtime = icaltime_from_string("20080818T180000Z");
    ok("Recurrence is excluded as per r961",
       icalproperty_recurrence_is_excluded(event, &dtstart, &recurtime));
    recurtime = icaltime_from_string("20080819T180000Z");
    ok("Recurrence is excluded for UTC EXDATE",
       icalproperty_recurrence_is_excluded(event, &dtstart, &recurtime));
}

void test_bad_dtstart_in_timezone(void)
{
    icaltimezone *myTZ = NULL;
    icalcomponent *vtimezone = NULL;
    char *str = NULL;

    myTZ = icaltimezone_get_builtin_timezone("Europe/Zurich");
    vtimezone = icaltimezone_get_component(myTZ);
    str = icalcomponent_as_ical_string(vtimezone);

    if (VERBOSE)
        printf("%s\n", str);

    ok("bad-dtstart-in-timezone.patch r960", (strstr(str, "DTSTART:20371025T030000") != NULL));
    ok("bad-dtstart-in-timezone.patch r960", (strstr(str, "DTSTART:20371025T030000") != NULL));
}

void test_icalcomponent_new_from_string(void)
{
    const char *item =
        "BEGIN:VCALENDAR\n"
        "PRODID:-//Ximian//NONSGML Evolution Calendar//EN\n"
        "VERSION:2.0\n"
        "BEGIN:VEVENT\n"
        "SUMMARY:phone meeting\n"
        "DTEND:20060406T163000Z\n"
        "DTSTART:20060406T160000Z\n"
        "UID:1234567890@dummy\n"
        "DTSTAMP:20110824T104144Z\n"
        "LAST-MODIFIED:20110824T104144Z\n"
        "CREATED:20060409T213201\n"
        "LOCATION:my office\n"
        "DESCRIPTION:let's talk\n"
        "CLASS:PUBLIC\n" "TRANSP:OPAQUE\n" "SEQUENCE:1\n" "END:VEVENT\n" "END:VCALENDAR\n";
    // must succeed and not leak memory...
    icalcomponent *comp = icalcomponent_new_from_string(item);

    ok("parsed", (comp != NULL));
    icalcomponent_free(comp);
}

void test_comma_in_quoted_value(void)
{
    icalcomponent *c;
    icalproperty *p;

    static const char test_icalcomp_str[] =
        "BEGIN:VEVENT\n" "X-TEST;VALUE=URI:\"geo:10.123456,-70.123456\"\n" "END:VEVENT\n";

    c = icalparser_parse_string((char *)test_icalcomp_str);
    ok("icalparser_parse_string()", (c != NULL));
    if (!c) {
        exit(EXIT_FAILURE);
    }

    if (VERBOSE)
        printf("%s", icalcomponent_as_ical_string(c));

    p = icalcomponent_get_first_property(c, ICAL_X_PROPERTY);
    ok("x-property is correct kind", (icalproperty_isa(p) == ICAL_X_PROPERTY));
    str_is("icalproperty_get_x_name() works", icalproperty_get_x_name(p), "X-TEST");
    str_is("icalproperty_get_value_as_string() works",
           icalproperty_get_value_as_string(p), "\"geo:10.123456,-70.123456\"");

    icalcomponent_free(c);
}

int main(int argc, char *argv[])
{
#if !defined(HAVE_UNISTD_H)
    extern char *optarg;
    extern int optopt;
#endif
#if defined(HAVE_GETOPT)
    int errflg = 0;
    int c;
#endif
/*    char* program_name = strrchr(argv[0],'/'); */
    int do_test = 0;
    int do_header = 0;
    int failed_count = 0;

    set_zone_directory((char *)"../../zoneinfo");
    icaltimezone_set_tzid_prefix("/softwarestudio.org/");

    test_start(0);

#if defined(HAVE_GETOPT)
    while ((c = getopt(argc, argv, "lvq")) != -1) {
        switch (c) {
        case 'v':
        {
            VERBOSE = 1;
            break;
        }
        case 'q':
        {
            QUIET = 1;
            break;
        }
        case 'l':
        {
            do_header = 1;
			break;
        }
        case '?':
        {
            errflg++;
        }
        }
    }

    if (optind < argc) {
        do_test = atoi(argv[argc - 1]);
    }
#else
    if (argc > 1)
        do_test = atoi(argv[1]);

#endif

    test_run("Test time parser functions", test_time_parser, do_test, do_header);
    test_run("Test time", test_time, do_test, do_header);
    test_run("Test day of Year", test_doy, do_test, do_header);
    test_run("Test duration", test_duration, do_test, do_header);
    test_run("Test period", test_period, do_test, do_header);
    test_run("Test DTSTART", test_dtstart, do_test, do_header);
    test_run("Test day of year of week start", test_start_of_week, do_test, do_header);
    test_run("Test recur parser", test_recur_parser, do_test, do_header);
    test_run("Test recur", test_recur, do_test, do_header);
    test_run("Test Recurring Events File", test_recur_file, do_test, do_header);
    test_run("Test parameter bug", test_recur_parameter_bug, do_test, do_header);
    test_run("Test Array Expansion", test_expand_recurrence, do_test, do_header);
    test_run("Test Free/Busy lists", test_fblist, do_test, do_header);
    test_run("Test Overlaps", test_overlaps, do_test, do_header);

    test_run("Test Span", test_icalcomponent_get_span, do_test, do_header);
    test_run("Test Gauge SQL", test_gauge_sql, do_test, do_header);
    test_run("Test Gauge Compare", test_gauge_compare, do_test, do_header);
    test_run("Test File Set", test_fileset, do_test, do_header);
    test_run("Test File Set (Extended)", test_fileset_extended, do_test, do_header);
    test_run("Test Dir Set", test_dirset, do_test, do_header);
    test_run("Test Dir Set (Extended)", test_dirset_extended, do_test, do_header);

/* test_file_locks is slow but should work ok -- uncomment to test it */
/*    test_run("Test File Locks", test_file_locks, do_test, do_header);*/
    test_run("Test X Props and Params", test_x, do_test, do_header);
    test_run("Test Trigger", test_trigger, do_test, do_header);
    test_run("Test Restriction", test_restriction, do_test, do_header);
    test_run("Test RDATE", test_rdate, do_test, do_header);
    test_run("Test language binding", test_langbind, do_test, do_header);
    test_run("Test property parser", test_property_parse, do_test, do_header);
    test_run("Test Action", test_action, do_test, do_header);
    test_run("Test Value Parameter", test_value_parameter, do_test, do_header);
    test_run("Test X property", test_x_property, do_test, do_header);
    test_run("Test X parameter", test_x_parameter, do_test, do_header);
    test_run("Test request status", test_requeststat, do_test, do_header);
    test_run("Test UTC-OFFSET", test_utcoffset, do_test, do_header);
    test_run("Test Values", test_values, do_test, do_header);
    test_run("Test Parameters", test_parameters, do_test, do_header);
    test_run("Test Properties", test_properties, do_test, do_header);
    test_run("Test Components", test_components, do_test, do_header);
    test_run("Test Convenience", test_convenience, do_test, do_header);
    test_run("Test classify ", test_classify, do_test, do_header);
    test_run("Test Iterators", test_iterators, do_test, do_header);
    test_run("Test strings", test_strings, do_test, do_header);
#if defined(INVALID_TEST)
    test_run("Test TZID escaping", test_tzid_escape, do_test, do_header);
#endif
    test_run("Test Compare", test_compare, do_test, do_header);
    test_run("Create Simple Component", create_simple_component, do_test, do_header);
    test_run("Create Components", create_new_component, do_test, do_header);
    test_run("Create Components with vaargs", create_new_component_with_va_args, do_test,
             do_header);
    test_run("Test Memory", test_memory, do_test, do_header);
    test_run("Test Attachment", test_attach, do_test, do_header);
    test_run("Test CalDAV Attachment", test_attach_caldav, do_test, do_header);
    test_run("Test Attachment with URL", test_attach_url, do_test, do_header);
    test_run("Test Attachment with data", test_attach_data, do_test, do_header);
    test_run("Test icalcalendar", test_calendar, do_test, do_header);
    test_run("Test Dirset", test_dirset, do_test, do_header);
    test_run("Test vCal to iCal conversion", test_vcal, do_test, do_header);
    test_run("Test UTF-8 Handling", test_utf8, do_test, do_header);
    test_run("Test exclusion of recurrences as per r961", test_recurrenceexcluded, do_test,
             do_header);
#if ADD_TESTS_REQUIRING_INVESTIGATION
    test_run("Test bad dtstart in timezone as per r960", test_bad_dtstart_in_timezone, do_test,
             do_header);
#endif
    test_run("Test icalcomponent_new_from_string()", test_icalcomponent_new_from_string, do_test,
             do_header);
    test_run("Test comma in quoted value of x property", test_comma_in_quoted_value, do_test,
             do_header);

    /** OPTIONAL TESTS go here... **/

#if defined(WITH_CXX_BINDINGS)
    test_run("Test C++ API", test_cxx, do_test, do_header);
#endif

#if defined(WITH_BDB)
    test_run("Test BDB Set", test_bdbset, do_test, do_header);
#endif

    icaltimezone_free_builtin_timezones();
    icalmemory_free_ring();
    free_zone_directory();

    failed_count = test_end();

    /* return a non-zero exit code if a test failed */
    return failed_count > 0;
}

/* make sure to keep tabs in our test strings */
/* Local Variables:    */
/* mode: c             */
/* indent-tabs-mode: t */
/* End:                */
