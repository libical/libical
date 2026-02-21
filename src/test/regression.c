/*======================================================================
 FILE: regression.c
 CREATOR: eric 03 April 1999

 SPDX-FileCopyrightText: 1999 Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 The original author is Eric Busboom
======================================================================*/

//Don't care about insecureAPI.strcpy issues in this test program
//NOLINTBEGIN(clang-analyzer-security.insecureAPI.strcpy)

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(NDEBUG)
#undef NDEBUG
#endif

#include "regression.h"
#include "test-malloc.h"
#include "libical/ical.h"
#include "libical/icaldate_p.h"
#include "libicalss/icalss.h"
#include "libicalvcal/icalvcal.h"
#include "libicalvcal/vobject.h"
#include "libicalvcal/vcc.h"

#include <assert.h>
#include <stdlib.h>

#define TESTS_TZID_PREFIX "/softwarestudio.org/tests/"

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

/* Returns a list of all attendees who are required. */
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

void test_values(void)
{
    icalvalue *v;
    icalvalue *copy;

    v = icalvalue_new_caladdress("cap://value/1");

    str_is("icalvalue_new_caladdress()", icalvalue_get_caladdress(v), "cap://value/1");

    icalvalue_set_caladdress(v, "cap://value/2");

    str_is("icalvalue_set_caladdress()", icalvalue_get_caladdress(v), "cap://value/2");

    str_is("icalvalue_as_ical_string()", icalvalue_as_ical_string(v), "cap://value/2");

    copy = icalvalue_clone(v);

    str_is("icalvalue_clone()", icalvalue_as_ical_string(copy), "cap://value/2");
    icalvalue_free(v);
    icalvalue_free(copy);

    v = icalvalue_new_boolean(0);
    int_is("icalvalue_new_boolean(0)", icalvalue_get_boolean(v), 0);
    str_is("icalvalue_as_ical_string()", icalvalue_as_ical_string(v), "FALSE");
    icalvalue_set_boolean(v, 2);
    ok("icalvalue_set_boolean(2)", (2 == icalvalue_get_boolean(v)));
    str_is("icalvalue_as_ical_string()", icalvalue_as_ical_string(v), "TRUE");

    copy = icalvalue_clone(v);
    str_is("icalvalue_clone()", icalvalue_as_ical_string(copy), "TRUE");

    icalvalue_free(v);
    icalvalue_free(copy);

    v = icalvalue_new_x("test");
    str_is("icalvalue_new_x(test)", icalvalue_get_x(v), "test");
    icalvalue_set_x(v, "test2");
    str_is("icalvalue_set_x(test2)", icalvalue_get_x(v), "test2");
    str_is("icalvalue_as_ical_string()", icalvalue_as_ical_string(v), "test2");

    copy = icalvalue_clone(v);
    str_is("icalvalue_clone()", icalvalue_as_ical_string(copy), "test2");

    icalvalue_free(v);
    icalvalue_free(copy);

    v = icalvalue_new_datetime(icaltime_from_timet_with_zone(1023404802, 0, NULL));
    str_is("icalvalue_new_datetime()", icalvalue_as_ical_string(v), "20020606T230642");
    icalvalue_set_datetime(v, icaltime_from_timet_with_zone(1023404802 - 3600, 0, NULL));
    str_is("icalvalue_set_datetime()", icalvalue_as_ical_string(v), "20020606T220642");

    copy = icalvalue_clone(v);
    str_is("icalvalue_clone()", icalvalue_as_ical_string(v), "20020606T220642");

    icalvalue_free(v);
    icalvalue_free(copy);

    v = icalvalue_new((icalvalue_kind)-1); //NOLINT (purposefully, for testing)

    ok("icalvalue_new(-1), Invalid type", (v == NULL));

    if (v != 0) {
        icalvalue_free(v);
    }

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

void test_properties(void)
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
                                      icalparameter_new_cn("A Common Name 4"),
                                      (void *)0);

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

    clone = icalproperty_clone(prop);

    if (VERBOSE) {
        printf("Clone:\n %s\n", icalproperty_as_ical_string(prop));
    }
    str_is("icalproperty_clone()", icalproperty_as_ical_string(prop), test_ical_str_good);

    icalproperty_free(clone);
    icalproperty_free(prop);

    prop = icalproperty_new((icalproperty_kind)-1); //NOLINT (purposefully, for testing)

    ok("test icalproperty_new() with invalid type (-1)", (prop == NULL));

    if (prop != 0) {
        icalproperty_free(prop);
    }
}

void test_utf8(void)
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

    const char *utf8_marker =
        "\xEF\xBB\xBF"
        "BEGIN:VCALENDAR\n"
        "PRODID:-//Ximian//NONSGML Evolution Calendar//EN\n"
        "VERSION:2.0\n"
        "METHOD:PUBLISH\n"
        "BEGIN:VEVENT\n"
        "SUMMARY:  áó aáóaáó aáä\n"
        "LOCATION:áóaáóaáóaä  áóaáóaáóaáóaáóaáóaáóaáóaáóaáó  aáóaáóaáóaáóaá \n"
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

    /* test a string with UTF-8 marker at the beginning */
    comp = icalcomponent_new_from_string(utf8_marker);
    ok("parsed", (comp != NULL));
    ok("kind", (icalcomponent_isa(comp) == ICAL_VCALENDAR_COMPONENT));
    str_is("location", icalcomponent_get_location(comp), "áóaáóaáóaä  áóaáóaáóaáóaáóaáóaáóaáóaáóaáó  aáóaáóaáóaáóaá");
    str_is("summary", icalcomponent_get_summary(comp), "áó aáóaáó aáä");
    icalcomponent_free(comp);
}

void test_icaltime_compare_utc_zone(void)
{
    struct icaltimetype a;
    struct icaltimetype b;

    /* test comparing a UTC and floating time */
    a = icaltime_from_string("20170130T103000Z");
    b = icaltime_from_string("20170130T103000");
    int_is("a == b", icaltime_compare(a, b), 0);

    /* test comparing times across time zones (where UTC is specified by zone) */
    a = icaltime_from_string("20170130T103000Z");
    b = icaltime_from_string("20170130T103000");
    b.zone = icaltimezone_get_builtin_timezone("America/Los_Angeles");
    int_is("a < b", icaltime_compare(a, b), -1);
}

void test_icaltime_normalize(void)
{
    icalcomponent *comp = icalcomponent_new_vpatch();
    ok("icaltime_normalize new vpatch", (comp != 0));
    struct icaltimetype t = icalcomponent_get_dtstamp(comp);
    ok("icaltime_normalize dtstamp time is valid", icaltime_is_valid_time(t));
    if (VERBOSE) {
        printf("Time: %d-%d-%d %d:%d:%d (is_valid: %d)\n", t.year, t.month, t.day, t.hour, t.minute, t.second, icaltime_is_valid_time(t));
    }

    struct icaltimetype norm = icaltime_normalize(t);
    ok("icaltime_normalize normalize normalized dtstamp time is valid", icaltime_is_valid_time(norm));
    if (VERBOSE) {
        printf("Normalized Time: %d-%d-%d %d:%d:%d (is_valid: %d)\n", norm.year, norm.month, norm.day, norm.hour, norm.minute, norm.second, icaltime_is_valid_time(norm));
    }
    icalcomponent_free(comp);
}

void test_parameters(void)
{
    icalparameter *p;
    int i;
    int enums[] =
        {ICAL_CUTYPE_INDIVIDUAL, ICAL_CUTYPE_RESOURCE, ICAL_FBTYPE_BUSY, ICAL_PARTSTAT_NEEDSACTION,
         ICAL_ROLE_NONPARTICIPANT, ICAL_XLICCOMPARETYPE_LESSEQUAL, ICAL_XLICERRORTYPE_MIMEPARSEERROR, -1};

    const char *str1 = "A Common Name";

    p = icalparameter_new_cn(str1);

    str_is("icalparameter_new_cn()", icalparameter_get_cn(p), str1);
    str_is("icalparameter_as_ical_string()", icalparameter_as_ical_string(p), "CN=A Common Name");

    icalparameter_free(p);

    p = icalparameter_new_cn("");

    str_is("icalparameter_new_cn()", icalparameter_get_cn(p), "");
    str_is("icalparameter_as_ical_string()", icalparameter_as_ical_string(p), "CN=\"\"");

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
    "X-LIC-ERROR;X-LIC-ERRORTYPE=COMPONENT-PARSE-ERROR:This is only a test\r\n"
    "END:VEVENT\r\n";

const char *bad_child =
    "BEGIN;\r\n"
    "TZNAME:\\r\n"
    "TZNAME:\\r\n"
    "TZNAME:\\r\n"
    "TZNAME:\\r\n"
    "TZNAME:\\r\n"
    "TZNAME:\\r\n"
    "TZNAME:\\r\n";

void test_components(void)
{
    icalcomponent *c;
    icalcomponent *child;
    int estate;

    c = icalcomponent_vanew(
        ICAL_VCALENDAR_COMPONENT,
        icalproperty_new_version("2.0"),
        icalproperty_new_prodid("-//RDU Software//NONSGML HandCal//EN"),
        icalproperty_vanew_comment("A Comment",
                                   icalparameter_new_cn("A Common Name 1"),
                                   (void *)0),
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
                                icalparameter_new_xlicerrortype(ICAL_XLICERRORTYPE_COMPONENTPARSEERROR),
                                (void *)0),
                            (void *)0),
        (void *)0);

    if (VERBOSE) {
        printf("Original Component:\n%s\n\n", icalcomponent_as_ical_string(c));
    }

    child = icalcomponent_get_first_component(c, ICAL_VEVENT_COMPONENT);

    ok("test icalcomponent_get_first_component()", (child != NULL));

    if (VERBOSE) {
        printf("Child Component:\n%s\n\n", icalcomponent_as_ical_string(child));
    }

    str_is("test results of child component", icalcomponent_as_ical_string(child), good_child);
    icalcomponent_free(c);

    estate = icalerror_get_errors_are_fatal();
    icalerror_set_errors_are_fatal(false);
    c = icalcomponent_new_from_string(bad_child);
    ok("parse failed as expected", (c == NULL));
    icalcomponent_free(c);
    icalerror_set_errors_are_fatal(estate);
}

static void test_component_foreach_callback(const icalcomponent *comp, const struct icaltime_span *span, void *data)
{
    int *foundExpected;
    _unused(comp);
    _unused(span);

    foundExpected = (int *)data;
    (*foundExpected)++;
}

int test_component_foreach_parameterized(int startOffsSec, int endOffsSec, int expectedFoundInstances)
{
    const char *calStr =
        "BEGIN:VCALENDAR\n"
        "BEGIN:VEVENT\n"
        "DTSTART;20180220T020000Z\n"
        "DURATION:PT1H\n"
        "RRULE:FREQ=WEEKLY;BYDAY=TU\n"
        "END:VEVENT\n"
        "END:VCALENDAR\n";

    icalcomponent *calendar = icalparser_parse_string(calStr);
    icalcomponent *event = icalcomponent_get_first_component(calendar, ICAL_VEVENT_COMPONENT);

    struct icaltimetype dtstart = icaltime_from_string("20180220T020000Z");
    struct icaltimetype it_start = dtstart;
    struct icaltimetype it_end = dtstart;
    int foundCnt = 0;

    icaltime_adjust(&it_start, 0, 0, 0, startOffsSec);
    icaltime_adjust(&it_end, 0, 0, 0, endOffsSec);

    icalcomponent_foreach_recurrence(event, it_start, it_end, test_component_foreach_callback, &foundCnt);
    icalcomponent_free(calendar);

    ok("icalcomponent_foreach_recurrence yielded the expected number of instances.", foundCnt == expectedFoundInstances);

    return 0;
}

void test_component_foreach_start_as_date_variant(const char *calStr, int expect_found)
{
    icalcomponent *calendar = icalparser_parse_string(calStr);
    icalcomponent *event = icalcomponent_get_first_component(calendar, ICAL_VEVENT_COMPONENT);

    // 2024-03-24 00:00 UTC to 2024-03-30 00:00 UTC
    struct icaltimetype t_start = icaltime_from_string("20240324");
    struct icaltimetype t_end = icaltime_from_string("20240330");

    int found = 0;
    icalcomponent_foreach_recurrence(event, t_start, t_end, test_component_foreach_callback, &found);
    int_is("Occurs", found, expect_found);

    icalcomponent_free(calendar);
}

// reproduces #833
void test_component_foreach_start_as_date(void)
{
    test_component_foreach_start_as_date_variant(
        "BEGIN:VCALENDAR\n"
        "BEGIN:VEVENT\n"
        "DTSTART:20240323T010000Z\n"
        "DTEND:20240324T000100Z\n"
        "RRULE:FREQ=WEEKLY\n"
        "END:VEVENT\n"
        "END:VCALENDAR\n",
        1);

    test_component_foreach_start_as_date_variant(
        "BEGIN:VCALENDAR\n"
        "BEGIN:VEVENT\n"
        "DTSTART:20240323T010000\n"
        "DTEND:20240324T000100\n"
        "RRULE:FREQ=WEEKLY\n"
        "END:VEVENT\n"
        "END:VCALENDAR\n",
        1);

    test_component_foreach_start_as_date_variant(
        "BEGIN:VCALENDAR\n"
        "BEGIN:VEVENT\n"
        "DTSTART:20240323\n"
        "DTEND:20240324\n"
        "RRULE:FREQ=WEEKLY\n"
        "END:VEVENT\n"
        "END:VCALENDAR\n",
        0);

    test_component_foreach_start_as_date_variant(
        "BEGIN:VCALENDAR\n"
        "BEGIN:VEVENT\n"
        "DTSTART:20240323\n"
        "DTEND:20240325\n"
        "RRULE:FREQ=WEEKLY\n"
        "END:VEVENT\n"
        "END:VCALENDAR\n",
        1);
}

void test_component_foreach(void)
{
    const char *calStr =
        "BEGIN:VCALENDAR\n"
        "BEGIN:VEVENT\n"
        /* 14th is NOT a tuesday, so it doesn't conform to the rule. */
        "DTSTART;20180214T020000Z\n"
        "DURATION:PT4H\n"
        "RRULE:FREQ=WEEKLY;COUNT=2;BYDAY=TU\n"
        "END:VEVENT\n"
        "END:VCALENDAR\n";

    icalcomponent *calendar = icalparser_parse_string(calStr);
    icalcomponent *event = icalcomponent_get_first_component(calendar, ICAL_VEVENT_COMPONENT);

    struct icaltimetype t_start = icaltime_from_string("20180219T000000Z");
    struct icaltimetype t_end = icaltime_from_string("20180221T000000Z");

    int foundExpectedCnt = 0;
    int i;

    /* we expect exactly one callback, namely on tu, 20th. */
    icalcomponent_foreach_recurrence(event, t_start, t_end, test_component_foreach_callback, &foundExpectedCnt);

    icalcomponent_free(calendar);

    ok("Exactly one instance was returned as expected in a VEVENT where DTSTART doesn't conform to the contained RRULE.", foundExpectedCnt == 1);

    calStr =
        "BEGIN:VCALENDAR\n"
        "BEGIN:VEVENT\n"
        /* Now DTSTART conforms to the RRULE. */
        "DTSTART;20180220T020000Z\n"
        "DURATION:PT4H\n"
        "RRULE:FREQ=WEEKLY;COUNT=2;BYDAY=TU\n"
        "END:VEVENT\n"
        "END:VCALENDAR\n";

    calendar = icalparser_parse_string(calStr);
    event = icalcomponent_get_first_component(calendar, ICAL_VEVENT_COMPONENT);

    t_start = icaltime_from_string("20180219T000000Z");
    t_end = icaltime_from_string("20180221T000000Z");

    foundExpectedCnt = 0;
    /* we expect exactly one callback, namely on tu, 20th. */
    icalcomponent_foreach_recurrence(event, t_start, t_end, test_component_foreach_callback, &foundExpectedCnt);

    icalcomponent_free(calendar);

    ok("Exactly one instance was returned as expected in a VEVENT where DTSTART *does* conform to the contained RRULE.", foundExpectedCnt == 1);

    /* No RRULE in this VEVENT */
    calStr =
        "BEGIN:VCALENDAR\n"
        "BEGIN:VEVENT\n"
        /* Now DTSTART conforms to the RRULE. */
        "DTSTART;20180220T020000Z\n"
        "DURATION:PT4H\n"
        "END:VEVENT\n"
        "END:VCALENDAR\n";

    calendar = icalparser_parse_string(calStr);
    event = icalcomponent_get_first_component(calendar, ICAL_VEVENT_COMPONENT);

    t_start = icaltime_from_string("20180219T000000Z");
    t_end = icaltime_from_string("20180221T000000Z");

    foundExpectedCnt = 0;
    icalcomponent_foreach_recurrence(event, t_start, t_end, test_component_foreach_callback, &foundExpectedCnt);
    ok("Exactly one instance was returned for an event without a RRULE.", foundExpectedCnt == 1);

    t_start = icaltime_from_string("20180221T000000Z");
    t_end = icaltime_from_string("20180223T000000Z");

    foundExpectedCnt = 0;
    icalcomponent_foreach_recurrence(event, t_start, t_end, test_component_foreach_callback, &foundExpectedCnt);
    ok("No instance was returned for an event without a RRULE where DTSTART lies outside the iterator limits.", foundExpectedCnt == 0);

    icalcomponent_free(calendar);

    calStr =
        "BEGIN:VCALENDAR\n"
        "BEGIN:VEVENT\n"
        "DTSTART;20180220T020000Z\n"
        "DURATION:PT4H\n"
        "RDATE;20180221T020000Z,20180222T020000Z\n"
        "RDATE;VALUE=PERIOD:20180223T020000Z/20180223T030000Z,\n"
        " 20180224T020000Z/PT2H\n"
        "END:VEVENT\n"
        "END:VCALENDAR\n";

    calendar = icalparser_parse_string(calStr);
    event = icalcomponent_get_first_component(calendar, ICAL_VEVENT_COMPONENT);

    t_start = icaltime_from_string("20180219T000000Z");
    t_end = icaltime_from_string("20180225T000000Z");

    foundExpectedCnt = 0;
    icalcomponent_foreach_recurrence(event, t_start, t_end, test_component_foreach_callback, &foundExpectedCnt);
    ok("Exactly five instances were returned for an event with RDATEs.", foundExpectedCnt == 5);

    icalcomponent_free(calendar);

    calStr =
        "BEGIN:VCALENDAR\n"
        "BEGIN:VEVENT\n"
        "DTSTART;20180220\n"
        "DURATION:P1D\n"
        "RDATE;VALUE=DATE:20180221,20180222\n"
        "END:VEVENT\n"
        "END:VCALENDAR\n";

    calendar = icalparser_parse_string(calStr);
    event = icalcomponent_get_first_component(calendar, ICAL_VEVENT_COMPONENT);

    t_start = icaltime_from_string("20180219T000000Z");
    t_end = icaltime_from_string("20180226T000000Z");

    foundExpectedCnt = 0;
    icalcomponent_foreach_recurrence(event, t_start, t_end, test_component_foreach_callback, &foundExpectedCnt);
    ok("Exactly three instances were returned for an event with RDATEs.", foundExpectedCnt == 3);

    icalcomponent_free(calendar);

    calStr =
        "BEGIN:VCALENDAR\n"
        "BEGIN:VEVENT\n"
        "DTSTART;20020402T114500Z\n"
        "DTEND;20020402T124500Z\n"
        "RRULE:FREQ=WEEKLY;INTERVAL=1;COUNT=6;BYDAY=TU,WE\n"
        "RDATE;20020404T114500Z,20020403T014500Z,20020403T014500Z,\n"
        " 20020403T114500Z,20020402T114500Z\n"
        "END:VEVENT\n"
        "END:VCALENDAR\n";

    calendar = icalparser_parse_string(calStr);
    event = icalcomponent_get_first_component(calendar, ICAL_VEVENT_COMPONENT);

    t_start = icaltime_from_string("20020402T114500Z");
    t_end = icaltime_from_string("20020502T114500Z");

    foundExpectedCnt = 0;
    icalcomponent_foreach_recurrence(event, t_start, t_end, test_component_foreach_callback, &foundExpectedCnt);
    printf("foundExpectedCnt: %d\n", foundExpectedCnt);
    ok("Exactly eight instances were returned for an event with RRULE and duplicate RDATEs.", foundExpectedCnt == 8);

    icalcomponent_free(calendar);

    for (i = 0; i < 3; i++) {
        /* Add one week with every run, so the first run will address the
        first recurrence instance, the second run the second instance,
        etc.
        */
        int offs = i * 7 * 24 * 3600;

        test_component_foreach_parameterized(offs + -1, offs + -1, 0);
        test_component_foreach_parameterized(offs + -1, offs + 0, 0);
        test_component_foreach_parameterized(offs + -1, offs + 1, 1);
        test_component_foreach_parameterized(offs + 0, offs + 0, 0);
        test_component_foreach_parameterized(offs + 0, offs + 1, 1);
        test_component_foreach_parameterized(offs + 1, offs + 1, 1);
        test_component_foreach_parameterized(offs + 0, offs + 3600, 1);
        test_component_foreach_parameterized(offs + 3600 - 1, offs + 3600, 1);
        test_component_foreach_parameterized(offs + 3600 - 1, offs + 3600 + 1, 1);
        test_component_foreach_parameterized(offs + 3600, offs + 3600, 0);
        test_component_foreach_parameterized(offs + 3600, offs + 3600 + 1, 0);
        test_component_foreach_parameterized(offs + 3600 + 1, offs + 3600 + 1, 0);
    }
}

typedef struct {
    size_t found;
    icalarray *arr;
} foreach_arr_t;

static void test_component_foreach_dtend_callback(const icalcomponent *comp, const struct icaltime_span *span, void *data)
{
    foreach_arr_t *a = (foreach_arr_t *)data;
    _unused(comp);
    _unused(span);

    assert(a->found < a->arr->num_elements);
    const icaltime_t *dtend = (icaltime_t *)icalarray_element_at(a->arr, a->found);
    ok("DTEND matches", *dtend == span->end);

    a->found++;
}

void test_component_foreach_dtend_daily(int count, const char *dtstart_str, const char *duration_str, const char **dtend_strs)
{
    if (count <= 0) {
        return;
    }
    icalcomponent *comp = icalcomponent_new(ICAL_VEVENT_COMPONENT);

    icaltimetype dtstartt = icaltime_from_string(dtstart_str), dtendt;
    icaltime_t dtend;
    dtstartt.zone = icaltimezone_get_builtin_timezone("America/New_York");
    icalcomponent_set_dtstart(comp, dtstartt);

    struct icalrecurrencetype *rrule = icalrecurrencetype_new_from_string("FREQ=DAILY");
    rrule->count = count;
    icalproperty *rrule_prop = icalproperty_new_rrule(rrule);
    icalcomponent_add_property(comp, rrule_prop);

    struct icaldurationtype dur = icaldurationtype_from_string(duration_str);
    icalcomponent_set_duration(comp, dur);

    foreach_arr_t a;
    a.found = 0;
    a.arr = icalarray_new(sizeof(dtend), 1);

    for (int i = 0; i < count; i++) {
        dtendt = icaltime_from_string(dtend_strs[i]);
        dtendt.zone = icaltimezone_get_builtin_timezone("America/New_York");
        dtend = icaltime_as_timet_with_zone(dtendt, dtendt.zone);
        icalarray_append(a.arr, &dtend);
    }

    icalcomponent_foreach_recurrence(comp, dtstartt, dtendt, test_component_foreach_dtend_callback, &a);

    icalcomponent_free(comp);
    icalarray_free(a.arr);
    icalrecurrencetype_unref(rrule);
}

void test_component_foreach_dtend_nominal(void)
{
    const char *dtends[] = {
        "20251101T220000",
        "20251102T220000",
        "20251103T220000",
    };
    test_component_foreach_dtend_daily(3, "20251031T220000", "P1D", dtends);
}

void test_component_foreach_dtend_exact(void)
{
    const char *dtends[] = {
        "20251101T220000",
        "20251102T210000",
        "20251103T220000",
    };
    test_component_foreach_dtend_daily(3, "20251031T220000", "PT24H", dtends);
}

void test_recur_iterator_set_start(void)
{
    icaltimetype start = icaltime_from_string("20150526");
    struct icalrecurrencetype *recurrence = icalrecurrencetype_new_from_string("FREQ=WEEKLY");
    icalrecur_iterator *iterator = icalrecur_iterator_new(recurrence, start);

    icaltimetype next = icalrecur_iterator_next(iterator);

    ok("Next recurrence iterator result should be of type date", next.is_date == 1);
    icalrecur_iterator_free(iterator);
    icalrecurrencetype_unref(recurrence);
}

void test_recur_iterator_on_jan_1(void)
{
    icaltimetype start = icaltime_from_string("20190101");
    struct icalrecurrencetype *recurrence = icalrecurrencetype_new_from_string("FREQ=WEEKLY;WKST=SU;INTERVAL=2;BYDAY=MO,TU,WE,TH,FR");
    icalrecur_iterator *iterator = icalrecur_iterator_new(recurrence, start);

    icaltimetype next = icalrecur_iterator_next(iterator);
    ok("Next recurrence iterator result should be January 1", next.year == 2019 && next.month == 1 && next.day == 1);

    next = icalrecur_iterator_next(iterator);
    ok("Next recurrence iterator result should be January 2", next.year == 2019 && next.month == 1 && next.day == 2);
    icalrecur_iterator_free(iterator);
    icalrecurrencetype_unref(recurrence);
}

void test_memory(void)
{
    size_t bufsize = 256;
    int i;
    char *p;

    const char S1[] = "1) When in the Course of human events, ";
    const char S2[] = "2) it becomes necessary for one people to dissolve the political bands which have connected them with another, ";
    const char S3[] = "3) and to assume among the powers of the earth, ";
    const char S4[] = "4) the separate and equal station to which the Laws of Nature and of Nature's God entitle them, ";
    const char S5[] = "5) a decent respect to the opinions of mankind requires that they ";
    const char S6[] = "6) should declare the causes which impel them to the separation. ";
    const char S7[] = "7) We hold these truths to be self-evident, ";
    const char S8[] = "8) that all men are created equal, ";

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

    icalmemory_free_buffer(f);

    bufsize = 4;

    f = icalmemory_new_buffer(bufsize);

    memset(f, 0, bufsize);
    p = f;

    icalmemory_append_char(&f, &p, &bufsize, 'a');
    if (VERBOSE) {
        printf("Char-by-Char buffer: %s\n", f);
    }

    icalmemory_append_char(&f, &p, &bufsize, 'b');
    if (VERBOSE) {
        printf("Char-by-Char buffer: %s\n", f);
    }

    icalmemory_append_char(&f, &p, &bufsize, 'c');
    if (VERBOSE) {
        printf("Char-by-Char buffer: %s\n", f);
    }

    icalmemory_append_char(&f, &p, &bufsize, 'd');
    if (VERBOSE) {
        printf("Char-by-Char buffer: %s\n", f);
    }

    icalmemory_append_char(&f, &p, &bufsize, 'e');
    if (VERBOSE) {
        printf("Char-by-Char buffer: %s\n", f);
    }

    icalmemory_append_char(&f, &p, &bufsize, 'f');
    if (VERBOSE) {
        printf("Char-by-Char buffer: %s\n", f);
    }

    icalmemory_append_char(&f, &p, &bufsize, 'g');
    if (VERBOSE) {
        printf("Char-by-Char buffer: %s\n", f);
    }

    icalmemory_append_char(&f, &p, &bufsize, 'h');
    if (VERBOSE) {
        printf("Char-by-Char buffer: %s\n", f);
    }

    icalmemory_append_char(&f, &p, &bufsize, 'i');
    if (VERBOSE) {
        printf("Char-by-Char buffer: %s\n", f);
    }

    icalmemory_append_char(&f, &p, &bufsize, 'j');
    if (VERBOSE) {
        printf("Char-by-Char buffer: %s\n", f);
    }

    icalmemory_append_char(&f, &p, &bufsize, 'a');
    if (VERBOSE) {
        printf("Char-by-Char buffer: %s\n", f);
    }

    icalmemory_append_char(&f, &p, &bufsize, 'b');
    if (VERBOSE) {
        printf("Char-by-Char buffer: %s\n", f);
    }

    icalmemory_append_char(&f, &p, &bufsize, 'c');
    if (VERBOSE) {
        printf("Char-by-Char buffer: %s\n", f);
    }

    icalmemory_append_char(&f, &p, &bufsize, 'd');
    if (VERBOSE) {
        printf("Char-by-Char buffer: %s\n", f);
    }

    icalmemory_append_char(&f, &p, &bufsize, 'e');
    if (VERBOSE) {
        printf("Char-by-Char buffer: %s\n", f);
    }

    icalmemory_append_char(&f, &p, &bufsize, 'f');
    if (VERBOSE) {
        printf("Char-by-Char buffer: %s\n", f);
    }

    icalmemory_append_char(&f, &p, &bufsize, 'g');
    if (VERBOSE) {
        printf("Char-by-Char buffer: %s\n", f);
    }

    icalmemory_append_char(&f, &p, &bufsize, 'h');
    if (VERBOSE) {
        printf("Char-by-Char buffer: %s\n", f);
    }

    icalmemory_append_char(&f, &p, &bufsize, 'i');
    if (VERBOSE) {
        printf("Char-by-Char buffer: %s\n", f);
    }

    icalmemory_append_char(&f, &p, &bufsize, 'j');
    if (VERBOSE) {
        printf("Char-by-Char buffer: %s\n", f);
    }

    icalmemory_free_buffer(f);

    for (i = 0; i < 100; i++) {
        f = icalmemory_tmp_buffer(bufsize);

        assert(f != 0);

        memset(f, 0, bufsize);
        snprintf(f, bufsize, "%d", i);
    }
}

void test_dirset(void)
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

    rtime.start = icaltime_from_timet_with_zone(time(0), 0, NULL);

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

            clone = icalcomponent_clone(itr);
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

    gauge = icalgauge_new_from_sql("SELECT * FROM VEVENT WHERE "
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

void test_compare(void)
{
    icalvalue *v1, *v2;

    v1 = icalvalue_new_caladdress("cap://value/1");
    v2 = icalvalue_clone(v1);

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

void test_restriction(void)
{
    icalcomponent *comp;
    struct icaltimetype atime = icaltime_from_timet_with_zone(time(0), 0, NULL);
    int valid;

    struct icaldatetimeperiodtype rtime;

    rtime.period.start = icaltime_from_timet_with_zone(time(0), 0, NULL);
    rtime.period.end = icaltime_from_timet_with_zone(time(0), 0, NULL);
    rtime.period.end.hour++;
    rtime.period.duration = icaldurationtype_null_duration();
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
                icalproperty_vanew_dtend(atime,
                                         icalparameter_new_tzid("America/New_York"),
                                         (void *)0),
                icalproperty_new_location("1CP Conference Room 4350"),
                icalcomponent_vanew(ICAL_VALARM_COMPONENT,
                                    icalproperty_new_action(ICAL_ACTION_EMAIL),
                                    icalproperty_new_repeat(0),
                                    icalcomponent_vanew(ICAL_VLOCATION_COMPONENT,
                                                        (void *)0),
                                    (void *)0),
                (void *)0),
            (void *)0);

    valid = icalrestriction_check(comp);
    (void)icalcomponent_as_ical_string(comp);
    icalcomponent_free(comp);
    ok("icalrestriction_check() == 0", (valid == 0));
}

void test_calendar(void)
{
    icalcomponent *comp;
    icalset *c;
    icalset *s;
    icalcalendar *calendar;
    icalerrorenum error;
    struct icaltimetype atime = icaltime_from_timet_with_zone(time(0), 0, NULL);

    (void)mkdir("calendar", 0755);
    (void)mkdir("calendar/booked", 0755);

    calendar = icalcalendar_new("calendar");

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

    error = icalfileset_add_component(c, icalcomponent_clone(comp));

    ok("Adding Clone Component to dirset", (error == ICAL_NO_ERROR));

    icalcalendar_free(calendar);

    ok("icalcalendar test", (1));
}

void test_increment(void);

void print_occur(struct icalrecurrencetype *recur, struct icaltimetype start)
{
    struct icaltimetype next;
    icalrecur_iterator *ritr;

    icaltime_t tt = icaltime_as_timet(start);

    printf("#### %s\n", icalrecurrencetype_as_string(recur));
    printf("#### %s\n", icalctime(&tt));

    ritr = icalrecur_iterator_new(recur, start);
    for (next = icalrecur_iterator_next(ritr);
         !icaltime_is_null_time(next);
         next = icalrecur_iterator_next(ritr)) {
        tt = icaltime_as_timet(next);
        printf("  %s", icalctime(&tt));
    }

    icalrecur_iterator_free(ritr);
}

void test_recur(void)
{
    struct icalrecurrencetype *rt;
    struct icaltimetype start;
    icaltime_t array[25];
    int i;

    rt = icalrecurrencetype_new_from_string(
        "FREQ=MONTHLY;UNTIL=19971224T000000Z;INTERVAL=1;BYDAY=TU,2FR,3SA");
    start = icaltime_from_string("19970905T090000Z");

    if (VERBOSE) {
        print_occur(rt, start);
    }

    if (VERBOSE) {
        printf("\n  Using icalrecur_expand_recurrence\n");
    }

    icalrecur_expand_recurrence(
        "FREQ=MONTHLY;UNTIL=19971224T000000Z;INTERVAL=1;BYDAY=TU,2FR,3SA",
        icaltime_as_timet(start), 25, array);

    for (i = 0; i < 25 && array[i] != 0; i++) {
        if (VERBOSE) {
            printf("  %s", icalctime(&(array[i])));
        }
    }

    icalrecurrencetype_unref(rt);
    /*    test_increment();*/
}

void test_recur_encode_by_day(void)
{
    struct icalrecurrencetype *rt;
    int ii;

    rt = icalrecurrencetype_new_from_string("FREQ=WEEKLY;BYDAY=WE");
    ok("Is weekly recurrence", (rt->freq == ICAL_WEEKLY_RECURRENCE));
    ok("The by[ICAL_BY_DAY] size equals 1", (rt->by[ICAL_BY_DAY].size == 1));
    ok("The day of week is Wednesday", (icalrecurrencetype_day_day_of_week(rt->by[ICAL_BY_DAY].data[0]) == ICAL_WEDNESDAY_WEEKDAY));
    ok("The position is 0", (icalrecurrencetype_day_position(rt->by[ICAL_BY_DAY].data[0]) == 0));
    ok("Encoded value matches", (icalrecurrencetype_encode_day(ICAL_WEDNESDAY_WEEKDAY, 0) == rt->by[ICAL_BY_DAY].data[0]));
    icalrecurrencetype_unref(rt);

    rt = icalrecurrencetype_new_from_string("FREQ=MONTHLY;BYDAY=2FR");
    ok("Is monthly recurrence", (rt->freq == ICAL_MONTHLY_RECURRENCE));
    ok("The by[ICAL_BY_DAY] size equals 1", (rt->by[ICAL_BY_DAY].size == 1));
    ok("The day of week is Friday", (icalrecurrencetype_day_day_of_week(rt->by[ICAL_BY_DAY].data[0]) == ICAL_FRIDAY_WEEKDAY));
    ok("The position is 2", (icalrecurrencetype_day_position(rt->by[ICAL_BY_DAY].data[0]) == 2));
    ok("Encoded value matches", (icalrecurrencetype_encode_day(ICAL_FRIDAY_WEEKDAY, 2) == rt->by[ICAL_BY_DAY].data[0]));
    icalrecurrencetype_unref(rt);

    rt = icalrecurrencetype_new_from_string("FREQ=YEARLY;BYDAY=-3MO");
    ok("Is yearly recurrence", (rt->freq == ICAL_YEARLY_RECURRENCE));
    ok("The by[ICAL_BY_DAY] size equals 1", (rt->by[ICAL_BY_DAY].size == 1));
    ok("The day of week is Monday", (icalrecurrencetype_day_day_of_week(rt->by[ICAL_BY_DAY].data[0]) == ICAL_MONDAY_WEEKDAY));
    ok("The position is -3", (icalrecurrencetype_day_position(rt->by[ICAL_BY_DAY].data[0]) == -3));
    ok("Encoded value matches", (icalrecurrencetype_encode_day(ICAL_MONDAY_WEEKDAY, -3) == rt->by[ICAL_BY_DAY].data[0]));
    icalrecurrencetype_unref(rt);

    for (ii = -5; ii <= 5; ii++) {
        icalrecurrencetype_weekday wd;

        for (wd = ICAL_SUNDAY_WEEKDAY; wd <= ICAL_SATURDAY_WEEKDAY; wd++) {
            short encoded;

            if (VERBOSE) {
                printf("  Trying weekday %d and position %d\n", wd, ii);
            }

            encoded = icalrecurrencetype_encode_day(wd, ii);

            ok("Decoded day of week matches", (icalrecurrencetype_day_day_of_week(encoded) == wd));
            ok("Decoded position matches", (icalrecurrencetype_day_position(encoded) == ii));
        }
    }
}

void test_recur_encode_by_month(void)
{
    struct icalrecurrencetype *rt;
    int ii, jj;

    rt = icalrecurrencetype_new_from_string("FREQ=WEEKLY;BYMONTH=2");
    ok("Is weekly recurrence", (rt->freq == ICAL_WEEKLY_RECURRENCE));
    ok("The by[ICAL_BY_MONTH] size equals 1", (rt->by[ICAL_BY_MONTH].size == 1));
    ok("The month is 2", (icalrecurrencetype_month_month(rt->by[ICAL_BY_MONTH].data[0]) == 2));
    ok("Is not leap month", (icalrecurrencetype_month_is_leap(rt->by[ICAL_BY_MONTH].data[0]) == 0));
    ok("Encoded value matches", (icalrecurrencetype_encode_month(2, 0) == rt->by[ICAL_BY_MONTH].data[0]));
    icalrecurrencetype_unref(rt);

    rt = icalrecurrencetype_new_from_string("FREQ=MONTHLY;BYMONTH=3L");
    if (rt != NULL) {
        ok("Is monthly recurrence", (rt->freq == ICAL_MONTHLY_RECURRENCE));
        ok("The by[ICAL_BY_MONTH] size equals 1", (rt->by[ICAL_BY_MONTH].size == 1));
        ok("The month is 3", (icalrecurrencetype_month_month(rt->by[ICAL_BY_MONTH].data[0]) == 3));
        ok("Is leap month", (icalrecurrencetype_month_is_leap(rt->by[ICAL_BY_MONTH].data[0]) != 0));
        ok("Encoded value matches", (icalrecurrencetype_encode_month(3, 1) == rt->by[ICAL_BY_MONTH].data[0]));
        icalrecurrencetype_unref(rt);
    }

    for (ii = 0; ii <= 1; ii++) {
        for (jj = 1; jj <= 12; jj++) {
            short encoded;

            if (VERBOSE) {
                printf("  Trying month %d as %sleap\n", jj, ii ? "" : "not ");
            }

            encoded = icalrecurrencetype_encode_month(jj, ii);

            ok("Decoded month matches", (icalrecurrencetype_month_month(encoded) == jj));
            ok("Decoded is-leap matches", ((icalrecurrencetype_month_is_leap(encoded) ? 1 : 0) == ii));
        }
    }
}

void test_expand_recurrence(void)
{
    icaltime_t arr[10];
    icaltime_t now = 931057385;
    int i, numfound = 0;

    icalrecur_expand_recurrence("FREQ=MONTHLY;BYDAY=MO,WE", now, 5, arr);

    if (VERBOSE) {
        printf("Start %s", icalctime(&now));
    }

    for (i = 0; i < 5; i++) {
        numfound++;
        if (VERBOSE) {
            printf("i=%d  %s\n", i, icalctime(&arr[i]));
        }
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
    BY_SET_POS = 8,
};

void icalrecurrencetype_test(void)
{
    icalvalue *v =
        icalvalue_new_from_string(
            ICAL_RECUR_VALUE,
            "FREQ=YEARLY;UNTIL=20060101T000000;INTERVAL=2;BYDAY=SU,WE;BYSECOND=15,30; BYMONTH=1,6,11");

    struct icalrecurrencetype *r = icalvalue_get_recur(v);
    struct icaltimetype t = icaltime_from_timet_with_zone(time(0), 0, NULL);
    struct icaltimetype next;
    icaltime_t tt;

    struct icalrecur_iterator_impl *itr =
        (struct icalrecur_iterator_impl *)icalrecur_iterator_new(r, t);

    do {
        next = icalrecur_iterator_next(itr);
        tt = icaltime_as_timet(next);

        printf("%s", icalctime(&tt));

    } while (!icaltime_is_null_time(next));

    icalvalue_free(v);

    icalrecur_iterator_free(itr);
}

/* From Federico Mena Quintero <federico@ximian.com>    */
void test_recur_parameter_bug(void)
{
    static const char test_icalcomp_str[] =
        "BEGIN:VEVENT\r\n"
        "RRULE;X-EVOLUTION-ENDDATE=20030209T081500:FREQ=DAILY;INTERVAL=6;COUNT=10\r\n"
        "END:VEVENT\r\n";

    icalcomponent *icalcomp;
    icalproperty *prop;
    struct icalrecurrencetype *recur;
    int n_errors;
    const char *str;

    icalcomp = icalparser_parse_string((char *)test_icalcomp_str);
    ok("icalparser_parse_string()", (icalcomp != NULL));
    assert(icalcomp != NULL);

    str = icalcomponent_as_ical_string(icalcomp);
    str_is("parsed matches original", str, (char *)test_icalcomp_str);
    if (VERBOSE) {
        printf("%s\n\n", str);
    }

    n_errors = icalcomponent_count_errors(icalcomp);
    int_is("no parse errors", n_errors, 0);

    if (n_errors) {
        icalproperty *p;
        for (p = icalcomponent_get_first_property(icalcomp, ICAL_XLICERROR_PROPERTY);
             p;
             p = icalcomponent_get_next_property(icalcomp, ICAL_XLICERROR_PROPERTY)) {
            fprintf(stderr, "error: %s\n", icalproperty_as_ical_string(p));
        }
    }

    prop = icalcomponent_get_first_property(icalcomp, ICAL_RRULE_PROPERTY);
    ok("get RRULE property", (prop != NULL));
    assert(prop != NULL);

    recur = icalproperty_get_rrule(prop);

    if (VERBOSE) {
        printf("%s\n", icalrecurrencetype_as_string(recur));
    }

    icalcomponent_free(icalcomp);
}

void test_duration(void)
{
    struct icaldurationtype d;

    d = icaldurationtype_from_string("PT8H30M");
    if (VERBOSE) {
        printf("%s\n", icaldurationtype_as_ical_string(d));
    }
    int_is("PT8H30M", icaldurationtype_as_seconds(d), 30600);
    int_is("PT8H30M", icaldurationtype_as_utc_seconds(d), 30600);

    d = icaldurationtype_from_string("-PT8H30M");
    if (VERBOSE) {
        printf("%s\n", icaldurationtype_as_ical_string(d));
    }
    int_is("-PT8H30M", icaldurationtype_as_seconds(d), -30600);
    int_is("-PT8H30M", icaldurationtype_as_utc_seconds(d), -30600);

    d = icaldurationtype_from_string("PT10H10M10S");
    if (VERBOSE) {
        printf("%s\n", icaldurationtype_as_ical_string(d));
    }
    int_is("PT10H10M10S", icaldurationtype_as_seconds(d), 36610);
    int_is("PT10H10M10S", icaldurationtype_as_utc_seconds(d), 36610);

    icalerror_set_errors_are_fatal(false);

    /* Test conversion of bad input */

    d = icaldurationtype_from_string("P7W");
    if (VERBOSE) {
        printf("%s\n", icaldurationtype_as_ical_string(d));
    }
    int_is("P7W", icaldurationtype_as_seconds(d), 0);
    int_is("P7W", icaldurationtype_as_utc_seconds(d), 4233600);

    d = icaldurationtype_from_string("P2DT8H30M");
    if (VERBOSE) {
        printf("%s\n", icaldurationtype_as_ical_string(d));
    }
    int_is("P2DT8H30M", icaldurationtype_as_seconds(d), 0);
    int_is("P2DT8H30M", icaldurationtype_as_utc_seconds(d), 203400);

    d = icaldurationtype_from_string("P2W1DT5H");
    if (VERBOSE) {
        printf("%s %d\n", icaldurationtype_as_ical_string(d), icaldurationtype_as_seconds(d));
    }
    int_is("P2W1DT5H", icaldurationtype_as_seconds(d), 0);
    int_is("P2W1DT5H", icaldurationtype_as_utc_seconds(d), 1314000);

    d = icaldurationtype_from_string("-P2DT8H30M");
    if (VERBOSE) {
        printf("%s\n", icaldurationtype_as_ical_string(d));
    }
    int_is("-P2DT8H30M", icaldurationtype_as_seconds(d), 0);
    int_is("-P2DT8H30M", icaldurationtype_as_utc_seconds(d), -203400);

    d = icaldurationtype_from_string("P7WT8H");
    if (VERBOSE) {
        printf("%s\n", icaldurationtype_as_ical_string(d));
    }
    int_is("P7WT8H", icaldurationtype_as_seconds(d), 0);
    int_is("P7WT8H", icaldurationtype_as_utc_seconds(d), 4262400);

    d = icaldurationtype_from_string("T10H");
    if (VERBOSE) {
        printf("%s\n", icaldurationtype_as_ical_string(d));
    }
    int_is("T10H", icaldurationtype_as_seconds(d), 0);
    int_is("T10H", icaldurationtype_as_utc_seconds(d), 0);

    icalerror_set_errors_are_fatal(true);

    d = icaldurationtype_from_seconds(4233600);
    if (VERBOSE) {
        printf("%s\n", icaldurationtype_as_ical_string(d));
    }
    str_is("PT4233600S", icaldurationtype_as_ical_string(d), "PT4233600S");

    d = icaldurationtype_from_seconds(4424400);
    if (VERBOSE) {
        printf("%s\n", icaldurationtype_as_ical_string(d));
    }
    str_is("PT4424400S", icaldurationtype_as_ical_string(d), "PT4424400S");
}

void test_period(void)
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

void test_strings(void)
{
    icalvalue *v;

    v = icalvalue_new_text("foo;bar;bats");
    if (VERBOSE) {
        printf("%s\n", icalvalue_as_ical_string(v));
    }

    str_is("test encoding of 'foo;bar;bats'", "foo\\;bar\\;bats", icalvalue_as_ical_string(v));

    icalvalue_free(v);

    v = icalvalue_new_text("foo\\;b\nar\\;ba\tts");
    if (VERBOSE) {
        printf("%s\n", icalvalue_as_ical_string(v));
    }

    str_is("test encoding of 'foo\\\\;b\\nar\\\\;ba\\tts'",
           "foo\\\\\\;b\\nar\\\\\\;ba	ts", icalvalue_as_ical_string(v));

    icalvalue_free(v);
}

/* Check RFC6868 encoding of "unsafe" chars in parameter values, such as '\n' */
void test_tzid_escape(void)
{
    icalparameter *tzid;
    icalproperty *prop;

    tzid = icalparameter_new_tzid("Timezone\nwith a newline and\"mo^re");
    prop = icalproperty_new_dtstart(icaltime_from_day_of_year(26, 2009));
    icalproperty_add_parameter(prop, tzid);

    if (VERBOSE) {
        printf("%s\n", icalproperty_as_ical_string(prop));
    }

    str_is("test encoding of 'Timezone\\nwith a newline'",
           icalproperty_as_ical_string(prop),
           "DTSTART;VALUE=DATE;TZID=Timezone^nwith a newline and^'mo^^re:20090126\r\n");

    icalproperty_free(prop);
}

void test_requeststat(void)
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

    str_is("icalproperty_new_from_string()",
           icalproperty_as_ical_string(p),
           "REQUEST-STATUS:2.1;Success but fallback taken  on one or more property  \r\n values.;booga\r\n");
    icalerror_set_error_state(ICAL_MALFORMEDDATA_ERROR, ICAL_ERROR_NONFATAL);
    st2 = icalreqstattype_from_string("16.4");

    ok("test unknown code", (st2.code == ICAL_UNKNOWN_STATUS));

    st2 = icalreqstattype_from_string("1.");

    ok("test malformed code", (st2.code == ICAL_UNKNOWN_STATUS));
    icalerror_set_error_state(ICAL_MALFORMEDDATA_ERROR, ICAL_ERROR_DEFAULT);

    icalproperty_free(p);
    icalcomponent_free(c);
}

void test_dtstart(void)
{
    struct icaltimetype tt, tt2;

    icalproperty *p;

    tt = icaltime_from_string("19970101");

    int_is("19970101 is a date", tt.is_date, 1);

    p = icalproperty_new_dtstart(tt);

    if (VERBOSE) {
        printf("%s\n", icalvalue_kind_to_string(icalvalue_isa(icalproperty_get_value(p))));
    }

    ok("ICAL_DATE_VALUE", (icalvalue_isa(icalproperty_get_value(p)) == ICAL_DATE_VALUE));

    tt2 = icalproperty_get_dtstart(p);
    int_is("converted date is date", tt2.is_date, 1);

    if (VERBOSE) {
        printf("%s\n", icalproperty_as_ical_string(p));
    }

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

    if (VERBOSE) {
        printf("%s\n", icalproperty_as_ical_string(p));
    }

    icalproperty_free(p);
}

void do_test_time(const char *zone)
{
    struct icaltimetype ictt, icttutc, icttzone, icttdayl,
        icttla, icttny, icttphoenix, icttlocal, icttnorm;
    icaltime_t tt, tt2, tt_p200;
    icaltime_t offset_tz;
    icalvalue *v;
    short day_of_week, start_day_of_week, day_of_year;
    icaltimezone *azone, *utczone;
    char msg[256];

    icalerror_set_errors_are_fatal(false);

    azone = icaltimezone_get_builtin_timezone(zone);
    utczone = icaltimezone_get_utc_timezone();

    /* Test new API */
    if (VERBOSE) {
        printf("\n---> From icaltime_t \n");
    }

    tt = 1025127869; /* stick with a constant... Wed, 26 Jun 2002 21:44:29 GMT */

    if (VERBOSE) {
        printf("Orig        : %s\n", ical_timet_string(tt));
    }

    ictt = icaltime_from_timet_with_zone(tt, 0, NULL);
    str_is("Floating time from icaltime_t", ictt_as_string(ictt), "2002-06-26 21:44:29 (floating)");

    ictt = icaltime_from_timet_with_zone(tt, 0, utczone);
    str_is("icaltime_from_timet_with_zone(tt,0,utc)", ictt_as_string(ictt),
           "2002-06-26 21:44:29 Z UTC");

    if (VERBOSE) {
        printf("\n---> Convert from floating \n");
    }

    ictt = icaltime_from_timet_with_zone(tt, 0, NULL);
    icttutc = icaltime_convert_to_zone(ictt, utczone);

    str_is("Convert from floating to UTC", ictt_as_string(icttutc), "2002-06-26 21:44:29 Z UTC");

    icttzone = icaltime_convert_to_zone(ictt, azone);
    ok("Convert from floating to zone",
       (strncmp(ictt_as_string(icttzone), "2002-06-26 21:44:29", 19) == 0));

    if (VERBOSE) {
        printf("\n---> Convert from UTC \n");
    }

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

    ok("test icaltime -> icaltime_t for 20001103T183030Z", (tt == 973276230));
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
        printf("\n As icaltime_t \n");
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

    offset_tz = (icaltime_t)(-icaltimezone_get_utc_offset_of_utc_time(azone, &ictt, 0));
    if (VERBOSE) {
        printf("offset_tz               : %ld\n", (long)offset_tz);
    }

    ok("test utc offset", (tt - tt2 == offset_tz));

    /* FIXME with the new API, it's not very useful */
    icttlocal = ictt;
    icaltimezone_convert_time(&icttlocal,
                              icaltimezone_get_utc_timezone(),
                              icaltimezone_get_builtin_timezone(zone));

    if (VERBOSE) {
        printf("As local    : %s\n", ictt_as_string(icttlocal));
    }

    if (VERBOSE) {
        printf("\n Convert to and from lib c \n");
    }

    if (VERBOSE) {
        printf("System time is: %s\n", ical_timet_string(tt));
    }

    v = icalvalue_new_datetime(ictt);

    if (VERBOSE) {
        printf("System time from libical: %s\n", icalvalue_as_ical_string(v));
    }

    icalvalue_free(v);

    tt2 = icaltime_as_timet(ictt);

    if (VERBOSE) {
        printf("Converted back to libc: %s\n", ical_timet_string(tt2));
    }

    if (VERBOSE) {
        printf("\n Incrementing time  \n");
    }

    icttnorm = ictt;

    icttnorm.year++;
    tt2 = icaltime_as_timet(icttnorm);
    if (VERBOSE) {
        printf("Add a year: %s\n", ical_timet_string(tt2));
    }

    icttnorm.month += 13;
    tt2 = icaltime_as_timet(icttnorm);
    if (VERBOSE) {
        printf("Add 13 months: %s\n", ical_timet_string(tt2));
    }

    icttnorm.second += 90;
    tt2 = icaltime_as_timet(icttnorm);
    if (VERBOSE) {
        printf("Add 90 seconds: %s\n", ical_timet_string(tt2));
    }

    if (VERBOSE) {
        printf("\n Day Of week \n");
    }

    day_of_week = icaltime_day_of_week(ictt);
    start_day_of_week = icaltime_start_doy_week(ictt, 1);
    day_of_year = icaltime_day_of_year(ictt);

    snprintf(msg, sizeof(msg), "Testing day of week %d", day_of_week);
    int_is(msg, day_of_week, 6);

    snprintf(msg, sizeof(msg), "Testing day of year %d", day_of_year);
    int_is(msg, day_of_year, 308);

    snprintf(msg, sizeof(msg), "Week started on doy of %d", start_day_of_week);
    int_is(msg, start_day_of_week, 303);

    if (VERBOSE) {
        printf("\n TimeZone Conversions \n");
    }

    icttla = icaltime_convert_to_zone(ictt,
                                      icaltimezone_get_builtin_timezone("America/Los_Angeles"));

#if !defined(USE_BUILTIN_TZDATA)
    int_is("Converted hour in America/Los_Angeles is 10", icttla.hour, 10);
#endif
    icttutc = icaltime_convert_to_zone(icttla, icaltimezone_get_utc_timezone());

#if !defined(USE_BUILTIN_TZDATA)
    ok("America/Los_Angeles local time is 2000-11-03 10:30:30",
       (strncmp(ictt_as_string(icttla), "2000-11-03 10:30:30", 19) == 0));
#endif
    ok("Test conversion back to UTC", (icaltime_compare(icttutc, ictt) == 0));

    icttny = icaltime_convert_to_zone(ictt, icaltimezone_get_builtin_timezone("America/New_York"));

    icttphoenix = icaltime_convert_to_zone(ictt,
                                           icaltimezone_get_builtin_timezone("America/Phoenix"));

    if (VERBOSE) {
        printf("Orig (icalctime): %s\n", ical_timet_string(tt));
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

        printf("Orig (icalctime): %s\n", ical_timet_string(tt));
        printf("Orig (ical) : %s\n", ictt_as_string(ictt));
        printf("NY          : %s\n", ictt_as_string(icttny));
    }
#if !defined(USE_BUILTIN_TZDATA)
    ok("Converted time in zone America/New_York is 2000-11-03 13:30:30",
       (strncmp(ictt_as_string(icttny), "2000-11-03 13:30:30", 19) == 0));
#endif
    tt_p200 = tt + 200 * 24 * 60 * 60; /* Add 200 days */

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
        printf("\nOrig (icalctime): %s\n", ical_timet_string(tt));
        printf("Orig (ical) : %s\n", ictt_as_string(ictt));
        printf("LA          : %s\n", ictt_as_string(icttla));
    }
#if !defined(USE_BUILTIN_TZDATA)
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

    icalerror_set_errors_are_fatal(true);

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

    /* Conversion to time_t around the epoch */
    ictt = icaltime_from_string("19691231T235958Z");
    tt = icaltime_as_timet_with_zone(ictt, utczone);
    int_is("convert to time_t EPOCH-2", (int)tt, -2);

    ictt = icaltime_from_string("19691231T235959Z");
    tt = icaltime_as_timet_with_zone(ictt, utczone);
    int_is("convert to time_t EPOCH-1", (int)tt, -1);

    ictt = icaltime_from_string("19700101T000000Z");
    tt = icaltime_as_timet_with_zone(ictt, utczone);
    int_is("convert to time_t EPOCH", (int)tt, 0);

    ictt = icaltime_from_string("19700101T000001Z");
    tt = icaltime_as_timet_with_zone(ictt, utczone);
    int_is("convert to time_t EPOCH+1", (int)tt, 1);

    ictt = icaltime_from_string("19700101T000002Z");
    tt = icaltime_as_timet_with_zone(ictt, utczone);
    int_is("convert to time_t EPOCH+2", (int)tt, 2);
}

void test_iterators(void)
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
    /* at this point the icalcompiter should be invalid */
    assert(icalcompiter_is_valid(&i));

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

        if (s) {
            nomore = 0;
        }
    }

    ok("test if any components remain after deleting the rest", nomore == 1);

    icalcomponent_free(c);
}

void test_time(void)
{
    const char *zones[6] =
        {
            "America/Los_Angeles", "America/New_York", "Europe/London", "Asia/Shanghai", NULL};

    int i;

    do_test_time(0);

    for (i = 0; zones[i] != NULL; i++) {
        if (VERBOSE) {
            printf(" ######### Timezone: %s ############\n", zones[i]);
        }

        do_test_time(zones[i]);
    }
}

void test_icalset(void)
{
    icalcomponent *c;

    icalset *f = icalset_new_file("2446.ics");
    icalset *d = icalset_new_dir("outdir");

    assert(f != 0);
    assert(d != 0);

    for (c = icalset_get_first_component(f); c != 0; c = icalset_get_next_component(f)) {
        icalcomponent *clone;

        clone = icalcomponent_clone(c);

        (void)icalset_add_component(d, clone);

        printf(" class %d\n", icalclassify(c, 0, "user"));
    }

    icalset_free(f);
    icalset_free(d);
}

void test_overlaps(void)
{
    icalcomponent *cset, *c;
    icalset *set;
    icaltime_t tm1 = 973353600; /*Sat Nov  4 16:00:00 UTC 2000,
                                   Sat Nov  4 08:00:00 PST 2000 */
    icaltime_t tm2 = 973382400; /*Sat Nov  5 00:00:00 UTC 2000
                                   Sat Nov  4 16:00:00 PST 2000 */

    icaltime_t hh = 1800; /* one half hour */

    icalfileset_options options = {O_RDONLY, 0644, 0, NULL};
    set = icalset_new(ICAL_FILE_SET, TEST_DATADIR "/overlaps.ics", &options);

    c = icalcomponent_vanew(ICAL_VEVENT_COMPONENT,
                            icalproperty_vanew_dtstart(icaltime_from_timet_with_zone(tm1 - hh, 0, NULL), (void *)0),
                            icalproperty_vanew_dtend(icaltime_from_timet_with_zone(tm2 - hh, 0, NULL), (void *)0),
                            (void *)0);

    cset = icalclassify_find_overlaps(set, c);
    ok("find overlaps 1", (cset != NULL));

    if (VERBOSE && cset) {
        printf("%s\n", icalcomponent_as_ical_string(cset));
    }

    if (cset) {
        icalcomponent_free(cset);
    }

    if (c) {
        icalcomponent_free(c);
    }

    c = icalcomponent_vanew(
        ICAL_VEVENT_COMPONENT,
        icalproperty_vanew_dtstart(icaltime_from_timet_with_zone(tm1 - hh, 0, NULL), (void *)0),
        icalproperty_vanew_dtend(icaltime_from_timet_with_zone(tm2, 0, NULL), (void *)0),
        (void *)0);

    cset = icalclassify_find_overlaps(set, c);

    ok("find overlaps 2", cset != NULL);
    if (VERBOSE && cset) {
        printf("%s\n", icalcomponent_as_ical_string(cset));
    }

    if (cset) {
        icalcomponent_free(cset);
    }

    if (c) {
        icalcomponent_free(c);
    }

    c = icalcomponent_vanew(
        ICAL_VEVENT_COMPONENT,
        icalproperty_vanew_dtstart(icaltime_from_timet_with_zone(tm1 + 3 * hh, 0, NULL), (void *)0),
        icalproperty_vanew_dtend(icaltime_from_timet_with_zone(tm2 + 3 * hh, 0, NULL), (void *)0),
        (void *)0);

    cset = icalclassify_find_overlaps(set, c);
    ok("find overlaps 3", cset != NULL);
    if (VERBOSE && cset) {
        printf("%s\n", icalcomponent_as_ical_string(cset));
    }

    if (cset) {
        icalcomponent_free(cset);
    }

    if (c) {
        icalcomponent_free(c);
    }

    c = icalcomponent_vanew(
        ICAL_VEVENT_COMPONENT,
        icalproperty_vanew_dtstart(icaltime_from_timet_with_zone(tm1 + 5 * hh, 0, NULL), (void *)0),
        icalproperty_vanew_dtend(icaltime_from_timet_with_zone(tm2 + 5 * hh, 0, NULL), (void *)0),
        (void *)0);

    cset = icalclassify_find_overlaps(set, c);
    ok("find overlaps 4", cset == NULL); // there are no overlaps in this range
    if (VERBOSE && cset) {
        printf("%s\n", icalcomponent_as_ical_string(cset));
    }

    if (cset) {
        icalcomponent_free(cset);
    }

    if (c) {
        icalcomponent_free(c);
    }

    if (set) {
        icalset_free(set);
    }
}

void test_fblist(void)
{
    icalspanlist *sl, *new_sl;
    icalfileset_options options = {O_RDONLY, 0644, 0, NULL};
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
        icalcomponent *fbcomp = icalspanlist_as_vfreebusy(sl, "a@foo.com", "b@foo.com");
        printf("%s\n", icalcomponent_as_ical_string(fbcomp));
        icalcomponent_free(fbcomp);
    }

    foo = icalspanlist_as_freebusy_matrix(sl, 3600);

    for (i = 0; foo[i] != -1; i++) {
        ; /* find number entries */
    }

    int_is("Calculating freebusy hourly matrix", i, (7 * 24));

    if (VERBOSE) {
        for (i = 0; foo[i] != -1; i++) {
            printf("%d", foo[i]);
            if ((i % 24) == 23) {
                printf("\n");
            }
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
            snprintf(strp, (size_t)(79 - i), "%1d", foo[i]);
            strp++;
        }
        str_is("Checking freebusy validity", out_str, "1121110");

        if (VERBOSE) {
            for (i = 0; foo[i] != -1; i++) {
                printf("%d", foo[i]);
                if ((i % 7) == 6) {
                    printf("\n");
                }
            }
            printf("\n\n");
        }
        free(foo);
    }

    icalspanlist_free(sl);

    if (VERBOSE) {
        printf("Unrestricted spanlist\n");
    }

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

    icalspanlist_free(sl);

    icalspanlist_free(new_sl);

    icalcomponent_free(comp);

    icalset_free(set);
}

void test_convenience(void)
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

    if (VERBOSE) {
        printf("\n%s\n", icalcomponent_as_ical_string(c));
    }

    duration = icaldurationtype_as_seconds(icalcomponent_get_duration(c)) / 60;

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

    if (VERBOSE) {
        printf("\n%s\n", icalcomponent_as_ical_string(c));
    }

    duration = icaldurationtype_as_seconds(icalcomponent_get_duration(c)) / 60;

    str_is("Start is 1997-08-01 12:00:00 Z UTC",
           ictt_as_string(icalcomponent_get_dtstart(c)), "1997-08-01 12:00:00 Z UTC");
    str_is("End is 1997-08-01 13:30:00 Z UTC",
           ictt_as_string(icalcomponent_get_dtend(c)), "1997-08-01 13:30:00 Z UTC");
    ok("Duration is 90 m", (duration == 90));

    icalcomponent_free(c);

    icalerror_set_errors_are_fatal(false);

    c = icalcomponent_vanew(
        ICAL_VCALENDAR_COMPONENT,
        icalcomponent_vanew(
            ICAL_VEVENT_COMPONENT,
            icalproperty_new_dtstart(icaltime_from_string("19970801T120000")),
            icalproperty_new_dtend(icaltime_from_string("19970801T130000")),
            (void *)0),
        (void *)0);

    icalcomponent_set_duration(c, icaldurationtype_from_string("PT1H30M"));

    if (VERBOSE) {
        printf("\n%s\n", icalcomponent_as_ical_string(c));
    }

    duration = icaldurationtype_as_seconds(icalcomponent_get_duration(c)) / 60;

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

    if (VERBOSE) {
        printf("\n%s\n", icalcomponent_as_ical_string(c));
    }

    duration = icaldurationtype_as_seconds(icalcomponent_get_duration(c)) / 60;

    ok("Start is 1997-08-01 12:00:00 Z UTC",
       (0 == strcmp("1997-08-01 12:00:00 Z UTC", ictt_as_string(icalcomponent_get_dtstart(c)))));
    ok("End is 1997-08-01 13:30:00 Z UTC",
       (0 == strcmp("1997-08-01 13:30:00 Z UTC", ictt_as_string(icalcomponent_get_dtend(c)))));
    ok("Duration is 90 m", (duration == 90));

    icalerror_set_errors_are_fatal(true);

    icalcomponent_free(c);

    c = icalcomponent_vanew(ICAL_VCALENDAR_COMPONENT,
                            icalcomponent_vanew(ICAL_VEVENT_COMPONENT, (void *)0),
                            (void *)0);

    icalcomponent_set_dtstart(c, icaltime_from_string("19970801T120000Z"));
    icalcomponent_set_dtend(c, icaltime_from_string("19970801T133000Z"));

    if (VERBOSE) {
        printf("\n%s\n", icalcomponent_as_ical_string(c));
    }

    duration = icaldurationtype_as_seconds(icalcomponent_get_duration(c)) / 60;

    ok("Start is 1997-08-01 12:00:00 Z UTC",
       (0 == strcmp("1997-08-01 12:00:00 Z UTC", ictt_as_string(icalcomponent_get_dtstart(c)))));
    ok("End is 1997-08-01 13:30:00 Z UTC",
       (0 == strcmp("1997-08-01 13:30:00 Z UTC", ictt_as_string(icalcomponent_get_dtend(c)))));
    ok("Duration is 90 m", (duration == 90));

    icalcomponent_free(c);

    c = icalcomponent_vanew(ICAL_VCALENDAR_COMPONENT,
                            icalcomponent_vanew(ICAL_VEVENT_COMPONENT, (void *)0),
                            (void *)0);

    icalcomponent_set_dtstart(c, icaltime_from_string("19970801T120000Z"));
    icalcomponent_set_duration(c, icaldurationtype_from_string("PT1H30M"));

    if (VERBOSE) {
        printf("\n%s\n", icalcomponent_as_ical_string(c));
    }

    duration = icaldurationtype_as_seconds(icalcomponent_get_duration(c)) / 60;

    ok("Start is 1997-08-01 12:00:00 Z UTC",
       (0 == strcmp("1997-08-01 12:00:00 Z UTC", ictt_as_string(icalcomponent_get_dtstart(c)))));
    ok("End is 1997-08-01 13:30:00 Z UTC",
       (0 == strcmp("1997-08-01 13:30:00 Z UTC", ictt_as_string(icalcomponent_get_dtend(c)))));
    ok("Duration is 90 m", (duration == 90));

    icalcomponent_free(c);

    c = icalcomponent_vanew(ICAL_VCALENDAR_COMPONENT,
                            icalcomponent_vanew(ICAL_VEVENT_COMPONENT, (void *)0),
                            (void *)0);

    tt = icaltime_from_string("19970801T120000");
    (void)icaltime_set_timezone(&tt, icaltimezone_get_builtin_timezone("Europe/Rome"));
    icalcomponent_set_dtstart(c, tt);

    if (VERBOSE) {
        printf("\n%s\n", icalcomponent_as_ical_string(c));
    }

    icalcomponent_set_duration(c, icaldurationtype_from_string("PT1H30M"));
    duration = icaldurationtype_as_seconds(icalcomponent_get_duration(c)) / 60;

    ok("Start is 1997-08-01 12:00:00 Europe/Rome",
       (0 == strcmp("1997-08-01 12:00:00 " TESTS_TZID_PREFIX "Europe/Rome",
                    ictt_as_string(icalcomponent_get_dtstart(c)))));
    ok("End is 1997-08-01 13:30:00 Europe/Rome",
       (0 == strcmp("1997-08-01 13:30:00 " TESTS_TZID_PREFIX "Europe/Rome",
                    ictt_as_string(icalcomponent_get_dtend(c)))));
    ok("Duration is 90 m", (duration == 90));

    icalcomponent_free(c);

    c = icalcomponent_vanew(ICAL_VCALENDAR_COMPONENT,
                            icalcomponent_vanew(ICAL_VEVENT_COMPONENT, (void *)0),
                            (void *)0);

    tt = icaltime_from_string("20250127T130000");
    (void)icaltime_set_timezone(&tt, icaltimezone_get_builtin_timezone("Europe/Berlin"));
    icalcomponent_set_dtstart(c, tt);

    tt = icaltime_from_string("20250127T140000");
    (void)icaltime_set_timezone(&tt, icaltimezone_get_builtin_timezone("America/New_York"));
    icalcomponent_set_dtend(c, tt);

    if (VERBOSE) {
        printf("\n%s\n", icalcomponent_as_ical_string(c));
    }

    duration = icaldurationtype_as_seconds(icalcomponent_get_duration(c)) / 3600;

    ok("Duration is 7 h", (duration == 7));

    icalcomponent_free(c);

    c = icalcomponent_vanew(ICAL_VCALENDAR_COMPONENT,
                            icalcomponent_vanew(ICAL_VTODO_COMPONENT, (void *)0),
                            (void *)0);

    tt = icaltime_from_string("20250127T130000");
    (void)icaltime_set_timezone(&tt, icaltimezone_get_builtin_timezone("Europe/Berlin"));
    icalcomponent_set_dtstart(c, tt);

    tt = icaltime_from_string("20250127T140000");
    (void)icaltime_set_timezone(&tt, icaltimezone_get_builtin_timezone("America/New_York"));
    icalcomponent_set_due(c, tt);

    if (VERBOSE) {
        printf("\n%s\n", icalcomponent_as_ical_string(c));
    }

    duration = icaldurationtype_as_seconds(icalcomponent_get_duration(c)) / 3600;

    ok("Duration is 7 h", (duration == 7));

    icalcomponent_free(c);
}

void test_time_parser(void)
{
    struct icaltimetype tt;

    icalerror_set_errors_are_fatal(false);

    tt = icaltime_from_string("19970101T1000");
    ok("19970101T1000 is null time", icaltime_is_null_time(tt));

    tt = icaltime_from_string("19970101X100000");
    ok("19970101X100000 is null time", icaltime_is_null_time(tt));

    tt = icaltime_from_string("19970101T100000");
    ok("19970101T100000 is valid", !icaltime_is_null_time(tt));

    if (VERBOSE) {
        printf("%s\n", icaltime_as_ctime(tt));
    }

    tt = icaltime_from_string("19970101T100000Z");

    ok("19970101T100000Z is valid", !icaltime_is_null_time(tt));
    if (VERBOSE) {
        printf("%s\n", icaltime_as_ctime(tt));
    }

    tt = icaltime_from_string("19970101");
    ok("19970101 is valid", (!icaltime_is_null_time(tt)));

    if (VERBOSE) {
        printf("%s\n", icaltime_as_ctime(tt));
    }

    icalerror_set_errors_are_fatal(true);
}

void test_recur_parser(void)
{
    struct icalrecurrencetype *rt;
    icalvalue *v = NULL;
    icalerrorstate es;
    const char *str;

    str =
        "FREQ=YEARLY;BYMONTH=1,2,3,4,8;BYYEARDAY=34,65,76,78;BYDAY=-1TU,3WE,-4FR,SA,SU;UNTIL=20000131T090000Z";
    rt = icalrecurrencetype_new_from_string(str);
    str_is(str, icalrecurrencetype_as_string(rt), str);

    /* Add COUNT and make sure its ignored in lieu of UNTIL */
    rt->count = 3;
    str_is(str, icalrecurrencetype_as_string(rt), str);

    /* Try to create a new RRULE value with UNTIL + COUNT */
    es = icalerror_supress("BADARG");
    v = icalvalue_new_recur(rt);
    icalerror_restore("BADARG", es);
    ok("COUNT + UNTIL not allowed", icalvalue_get_recur(v) == NULL);
    icalrecurrencetype_unref(rt);

    /* Try to parse a RRULE with illegal BY* part combination */
    str = "FREQ=DAILY;COUNT=3;BYDAY=-1TU,3WE,-4FR,SA,SU;BYYEARDAY=34,65,76,78;BYMONTH=1,2,3,4,8";

    es = icalerror_supress("MALFORMEDDATA");
    rt = icalrecurrencetype_new_from_string(str);
    icalerror_restore("MALFORMEDDATA", es);
    ok("DAILY + BYYEARDAY not allowed", rt == NULL);

    /* Parse the same RRULE but ignore invalid BY* parts */
    ical_set_invalid_rrule_handling_setting(ICAL_RRULE_IGNORE_INVALID);
    rt = icalrecurrencetype_new_from_string(str);
    str_is(str, icalrecurrencetype_as_string(rt),
           "FREQ=DAILY;BYMONTH=1,2,3,4,8;BYDAY=-1TU,3WE,-4FR,SA,SU;COUNT=3");
    icalrecurrencetype_unref(rt);

    /* Try to parse an RRULE value with UNTIL + COUNT */
    str = "FREQ=YEARLY;UNTIL=20000131T090000Z;COUNT=3";

    es = icalerror_supress("MALFORMEDDATA");
    rt = icalrecurrencetype_new_from_string(str);
    icalerror_restore("MALFORMEDDATA", es);
    ok(str, rt == NULL);
    icalmemory_free_buffer(v);
}

static int test_juldat_caldat_instance(long year, int month, int day)
{
    struct icaltimetype t;
    struct ut_instant_int originalInstant;

    memset(&t, 0, sizeof(t));
    t.year = year;
    t.month = month;
    t.day = day;

    memset(&originalInstant, 0, sizeof(originalInstant));
    originalInstant.year = year;
    originalInstant.month = month;
    originalInstant.day = day;

    ical_juldat(&originalInstant);
    ical_caldat(&originalInstant);

    if (icaltime_day_of_week(t) != originalInstant.weekday + 1) {
        return -1;
    }

    if (icaltime_start_doy_week(t, 1) != originalInstant.day_of_year - originalInstant.weekday) {
        return -1;
    }

    return 0;
}

/*
 * This test verifies the ical_caldat and ical_juldat functions. The functions are reworked versions
 * of the original ical_caldat and ical_juldat functions but avoid using floating point arithmetic. As the
 * new functions are not exported, the test cannot access them directly. It therefore checks the
 * output of the icaltime_day_of_week and icaltime_start_doy_week functions
 * which are based on the functions to be tested.
 */
void test_juldat_caldat(void)
{
    int i;
    int failed = 0;

    ok("ical_juldat and ical_caldat return the expected values for specified min input values", test_juldat_caldat_instance(-4713, 1, 1) == 0);
    ok("ical_juldat and ical_caldat return the expected values for specified max input values", test_juldat_caldat_instance(+32767, 12, 31) == 0);

    ok("ical_juldat and ical_caldat return the expected values before end of julian calendar", test_juldat_caldat_instance(1582, 10, 4) == 0);
    ok("ical_juldat and ical_caldat return the expected values at end of julian calendar", test_juldat_caldat_instance(1582, 10, 5) == 0);
    ok("ical_juldat and ical_caldat return the expected values before introduction of gregorian calendar", test_juldat_caldat_instance(1582, 10, 14) == 0);
    ok("ical_juldat and ical_caldat return the expected values at introduction of gregorian calendar", test_juldat_caldat_instance(1582, 10, 15) == 0);
    ok("ical_juldat and ical_caldat return the expected values after introduction of gregorian calendar", test_juldat_caldat_instance(1582, 10, 16) == 0);

    for (i = 0; i < 2582; i++) {
        long y = i;

        failed |= (test_juldat_caldat_instance(y, 1, 1) != 0);
        failed |= (test_juldat_caldat_instance(y, 2, 28) != 0);

        // Not every year has a leap day, but ical_juldat/ical_caldat should still produce
        // the same output as the original implementation.
        failed |= (test_juldat_caldat_instance(y, 2, 29) != 0);
        failed |= (test_juldat_caldat_instance(y, 3, 1) != 0);
        failed |= (test_juldat_caldat_instance(y, 12, 31) != 0);
    }
    ok("ical_juldat and ical_caldat return the expected values for random input values", failed == 0);

    failed = 0;
    for (i = 0; i < 10000; i++) {
        /* coverity[dc.weak_crypto] */
        long y = rand() % 2582;
        int m = rand() % 12 + 1;

        // Might produce some invalid dates, but ical_juldat/ical_caldat should still produce
        // the same output as the original implementation.
        int d = rand() % 31 + 1;

        failed |= (test_juldat_caldat_instance(y, m, d) != 0);
    }

    ok("ical_juldat and ical_caldat return the expected values for random input values", failed == 0);
}

char *ical_strstr(const char *haystack, const char *needle)
{
    return strstr(haystack, needle);
}

void test_start_of_week(void)
{
    struct icaltimetype tt1 = icaltime_from_string("19900110");

    do {
        tt1 = icaltime_normalize(tt1);

        int doy = icaltime_start_doy_week(tt1, 1);
        int dow = icaltime_day_of_week(tt1);

        struct icaltimetype tt2 = icaltime_from_day_of_year(doy, tt1.year);
        int start_dow = icaltime_day_of_week(tt2);

        if (doy == 1) {
            char msg[128];

            snprintf(msg, sizeof(msg), "%s", ictt_as_string(tt1));
            int_is(msg, start_dow, 1);
        }

        if (start_dow != 1) { /* Sunday is 1 */
            printf("failed: Start of week (%s) is not a Sunday \n for %s (doy=%d,dow=%d)\n",
                   ictt_as_string(tt2), ictt_as_string(tt1), dow, start_dow);
        }

        assert(start_dow == 1);

        tt1.day += 1;

    } while (tt1.year < 2010);
}

void test_doy(void)
{
    struct icaltimetype tt1, tt2;
    short doy, doy2;
    char msg[128];

    tt1 = icaltime_from_string("19900101");

    if (VERBOSE) {
        printf("Test icaltime_day_of_year() agreement with mktime\n");
    }

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

        (void)icalmktime(&stm);

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

    if (VERBOSE) {
        printf("\nTest icaltime_day_of_year() agreement with icaltime_from_day_of_year()\n");
    }

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

void test_x(void)
{
    static const char test_icalcomp_str[] =
        "BEGIN:VEVENT\r\n"
        "RRULE\r\n"
        " ;X-EVOLUTION-ENDDATE=20030209T081500\r\n"
        " :FREQ=DAILY;COUNT=10;INTERVAL=6\r\n"
        "X-COMMENT;X-FOO=BAR: Booga\r\n"
        "END:VEVENT\r\n";

    icalcomponent *icalcomp;
    icalproperty *prop;
    struct icalrecurrencetype *recur;
    int n_errors;

    icalcomp = icalparser_parse_string((char *)test_icalcomp_str);
    assert(icalcomp != NULL);

    if (VERBOSE) {
        printf("%s\n\n", icalcomponent_as_ical_string(icalcomp));
    }

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

    if (VERBOSE) {
        printf("%s\n", icalrecurrencetype_as_string(recur));
    }

    icalcomponent_free(icalcomp);
}

void test_gauge_sql(void)
{
    icalgauge *g;
    const char *str;

    str =
        "SELECT DTSTART,DTEND,COMMENT FROM VEVENT,VTODO WHERE VEVENT.SUMMARY = 'Bongoa' AND SEQUENCE < 5";

    g = icalgauge_new_from_sql(str, 0);
    ok(str, (g != NULL));
    if (VERBOSE) {
        icalgauge_dump(g);
    }

    icalgauge_free(g);

    str =
        "SELECT * FROM VEVENT,VTODO WHERE VEVENT.SUMMARY = 'Bongoa' AND SEQUENCE < 5 OR METHOD != 'CREATE'";

    g = icalgauge_new_from_sql(str, 0);
    ok(str, (g != NULL));
    if (VERBOSE) {
        icalgauge_dump(g);
    }

    icalgauge_free(g);

    str = "SELECT * FROM VEVENT WHERE SUMMARY == 'BA301'";

    g = icalgauge_new_from_sql(str, 0);
    ok(str, (g != NULL));
    if (VERBOSE) {
        icalgauge_dump(g);
    }

    icalgauge_free(g);

    str = "SELECT * FROM VEVENT WHERE SUMMARY == 'BA301'";

    g = icalgauge_new_from_sql(str, 0);
    ok(str, (g != NULL));
    if (VERBOSE) {
        icalgauge_dump(g);
    }

    icalgauge_free(g);

    str = "SELECT * FROM VEVENT WHERE LOCATION == '104 Forum'";

    g = icalgauge_new_from_sql(str, 0);
    ok(str, (g != NULL));
    if (VERBOSE) {
        icalgauge_dump(g);
    }

    icalgauge_free(g);
}

void test_gauge_compare(void)
{
    icalgauge *g;
    icalcomponent *c;
    const char *str;

    /* Equality */

    c = icalcomponent_vanew(
        ICAL_VCALENDAR_COMPONENT,
        icalcomponent_vanew(
            ICAL_VEVENT_COMPONENT,
            icalproperty_new_dtstart(icaltime_from_string("20000101T000002")), (void *)0),
        (void *)0);

    str = "SELECT * FROM VEVENT WHERE DTSTART = '20000101T000002'";
    g = icalgauge_new_from_sql(str, 0);

    ok(str, (c != 0 && g != 0));
    assert(c != 0);
    assert(g != 0);

    int_is("compare", icalgauge_compare(g, c), 1);

    if (g != NULL) {
        icalgauge_free(g);
    }
    str = "SELECT * FROM VEVENT WHERE DTSTART = '20000101T000001'";
    g = icalgauge_new_from_sql(str, 0);

    ok(str, (g != 0));

    assert(g != 0);
    int_is("compare", icalgauge_compare(g, c), 0);

    icalgauge_free(g);

    str = "SELECT * FROM VEVENT WHERE DTSTART != '20000101T000003'";
    g = icalgauge_new_from_sql(str, 0);

    ok(str, (c != 0 && g != 0));

    assert(g != 0);
    int_is("compare", icalgauge_compare(g, c), 1);

    if (g != NULL) {
        icalgauge_free(g);
    }

    /* Less than */

    str = "SELECT * FROM VEVENT WHERE DTSTART < '20000101T000003'";
    g = icalgauge_new_from_sql(str, 0);

    ok(str, (c != 0 && g != 0));

    assert(g != 0);
    int_is("compare", icalgauge_compare(g, c), 1);

    if (g != NULL) {
        icalgauge_free(g);
    }
    str = "SELECT * FROM VEVENT WHERE DTSTART < '20000101T000002'";
    g = icalgauge_new_from_sql(str, 0);

    ok(str, (g != 0));

    assert(g != 0);
    int_is("compare", icalgauge_compare(g, c), 0);

    icalgauge_free(g);

    /* Greater than */

    str = "SELECT * FROM VEVENT WHERE DTSTART > '20000101T000001'";
    g = icalgauge_new_from_sql(str, 0);

    ok(str, (g != 0));

    assert(g != 0);
    int_is("compare", icalgauge_compare(g, c), 1);

    if (g != NULL) {
        icalgauge_free(g);
    }

    str = "SELECT * FROM VEVENT WHERE DTSTART > '20000101T000002'";
    g = icalgauge_new_from_sql(str, 0);

    ok(str, (g != 0));

    assert(g != 0);
    int_is("compare", icalgauge_compare(g, c), 0);

    icalgauge_free(g);

    /* Greater than or Equal to */

    str = "SELECT * FROM VEVENT WHERE DTSTART >= '20000101T000002'";
    g = icalgauge_new_from_sql(str, 0);

    ok(str, (g != 0));

    int_is("compare", icalgauge_compare(g, c), 1);

    if (g != NULL) {
        icalgauge_free(g);
    }

    str = "SELECT * FROM VEVENT WHERE DTSTART >= '20000101T000003'";
    g = icalgauge_new_from_sql(str, 0);

    ok(str, (g != 0));

    int_is("compare", icalgauge_compare(g, c), 0);

    icalgauge_free(g);

    /* Less than or Equal to */

    str = "SELECT * FROM VEVENT WHERE DTSTART <= '20000101T000002'";
    g = icalgauge_new_from_sql(str, 0);

    ok(str, (g != 0));

    assert(g != 0);
    int_is("compare", icalgauge_compare(g, c), 1);

    if (g != NULL) {
        icalgauge_free(g);
    }

    str = "SELECT * FROM VEVENT WHERE DTSTART <= '20000101T000001'";
    g = icalgauge_new_from_sql(str, 0);

    ok(str, (g != 0));

    int_is("compare", icalgauge_compare(g, c), 0);

    icalgauge_free(g);

    icalcomponent_free(c);

    /* Combinations */

    c = icalcomponent_vanew(
        ICAL_VCALENDAR_COMPONENT,
        icalcomponent_vanew(
            ICAL_VEVENT_COMPONENT,
            icalproperty_new_dtstart(icaltime_from_string("20000102T000000")), (void *)0),
        (void *)0);

    str = "SELECT * FROM VEVENT WHERE DTSTART > '20000101T000000' and DTSTART < '20000103T000000'";

    g = icalgauge_new_from_sql(str, 0);
    ok(str, (g != 0));
    int_is("compare", icalgauge_compare(g, c), 1);

    if (g != NULL) {
        icalgauge_free(g);
    }

    str = "SELECT * FROM VEVENT WHERE DTSTART > '20000101T000000' and DTSTART < '20000102T000000'";

    g = icalgauge_new_from_sql(str, 0);
    ok(str, (g != 0));
    int_is("compare", icalgauge_compare(g, c), 0);

    icalgauge_free(g);

    str = "SELECT * FROM VEVENT WHERE DTSTART > '20000101T000000' or DTSTART < '20000102T000000'";

    g = icalgauge_new_from_sql(str, 0);
    ok(str, (g != 0));
    int_is("compare", icalgauge_compare(g, c), 1);

    if (g != NULL) {
        icalgauge_free(g);
    }

    icalcomponent_free(c);

    /* Combinations, non-cannonical component */

    c = icalcomponent_vanew(ICAL_VEVENT_COMPONENT,
                            icalproperty_new_dtstart(icaltime_from_string("20000102T000000")),
                            (void *)0);

    str = "SELECT * FROM VEVENT WHERE DTSTART > '20000101T000000' and DTSTART < '20000103T000000'";

    g = icalgauge_new_from_sql(str, 0);
    ok(str, (g != 0));
    int_is("compare", icalgauge_compare(g, c), 1);

    if (g != NULL) {
        icalgauge_free(g);
    }

    str = "SELECT * FROM VEVENT WHERE DTSTART > '20000101T000000' and DTSTART < '20000102T000000'";

    g = icalgauge_new_from_sql(str, 0);
    ok(str, (g != 0));
    int_is("compare", icalgauge_compare(g, c), 0);

    icalgauge_free(g);

    str = "SELECT * FROM VEVENT WHERE DTSTART > '20000101T000000' or DTSTART < '20000102T000000'";

    g = icalgauge_new_from_sql(str, 0);
    ok(str, (g != 0));
    int_is("compare", icalgauge_compare(g, c), 1);

    if (g != NULL) {
        icalgauge_free(g);
    }
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

    g = icalgauge_new_from_sql(str, 0);
    ok(str, (g != 0));
    int_is("compare", icalgauge_compare(g, c), 1);

    if (g != NULL) {
        icalgauge_free(g);
    }

    str = "SELECT * FROM VEVENT WHERE COMMENT = 'foo'";
    g = icalgauge_new_from_sql(str, 0);
    ok(str, (g != 0));
    int_is("compare", icalgauge_compare(g, c), 1);

    if (g != NULL) {
        icalgauge_free(g);
    }

    str = "SELECT * FROM VEVENT WHERE COMMENT = 'foo' AND  VALARM.DTSTART = '20000101T120000'";
    g = icalgauge_new_from_sql(str, 0);
    ok(str, (g != 0));
    int_is("compare", icalgauge_compare(g, c), 1);

    if (g != NULL) {
        icalgauge_free(g);
    }

    str = "SELECT * FROM VEVENT WHERE COMMENT = 'bar' AND  VALARM.DTSTART = '20000101T120000'";
    g = icalgauge_new_from_sql(str, 0);
    ok(str, (g != 0));
    int_is("compare", icalgauge_compare(g, c), 0);

    icalgauge_free(g);

    str = "SELECT * FROM VEVENT WHERE COMMENT = 'bar' or  VALARM.DTSTART = '20000101T120000'";
    g = icalgauge_new_from_sql(str, 0);
    ok(str, (g != 0));
    int_is("compare", icalgauge_compare(g, c), 1);

    if (g != NULL) {
        icalgauge_free(g);
    }

    icalcomponent_free(c);
}

icalcomponent *make_component(int i)
{
    icalcomponent *c;

    struct icaltimetype t = icaltime_from_string("20000101T120000Z");

    t.day += i;

    c = icalcomponent_vanew(ICAL_VCALENDAR_COMPONENT,
                            icalproperty_new_method(ICAL_METHOD_REQUEST),
                            icalcomponent_vanew(ICAL_VEVENT_COMPONENT,
                                                icalproperty_new_dtstart(t), (void *)0),
                            (void *)0);

    assert(c != 0);

    return c;
}

void test_fileset(void)
{
#if defined(HAVE_UNLINK)
    icalset *fs;
    icalcomponent *c;
    int i;
    int comp_count = 0;
    const char *path = "test_fileset.ics";
    icalgauge *g =
        icalgauge_new_from_sql(
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

    if (VERBOSE) {
        printf("== No Selections \n");
    }

    comp_count = 0;
    for (c = icalfileset_get_first_component(fs); c != 0; c = icalfileset_get_next_component(fs)) {
        struct icaltimetype t = icalcomponent_get_dtstart(c);

        comp_count++;
        if (VERBOSE) {
            printf("%s\n", icaltime_as_ctime(t));
        }
    }
    int_is("icalfileset get components", comp_count, 10);

    (void)icalfileset_select(fs, g);

    if (VERBOSE) {
        printf("\n== DTSTART > '20000103T120000Z' AND DTSTART <= '20000106T120000Z' \n");
    }

    comp_count = 0;
    for (c = icalfileset_get_first_component(fs); c != 0; c = icalfileset_get_next_component(fs)) {
        struct icaltimetype t = icalcomponent_get_dtstart(c);

        comp_count++;
        if (VERBOSE) {
            printf("%s\n", icaltime_as_ctime(t));
        }
    }
    int_is("icalfileset get components with gauge", comp_count, 3);

    icalset_free(fs);

    /*icalgauge_free(g); */
#endif
}

void microsleep(int us)
{ /*us is in microseconds */
#if defined(HAVE_NANOSLEEP)
    struct timespec ts;

    ts.tv_sec = 0;
    ts.tv_nsec = us * 1000; /* convert from microseconds to nanoseconds */

    nanosleep(&ts, NULL);
#elif defined(HAVE_USLEEP)
    usleep(us);
#endif /*Windows Sleep is useless for microsleeping */
}

void test_file_locks(void)
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

        d = icaldurationtype_from_seconds(1);

        icalcomponent_set_duration(c, d);

        (void)icalfileset_add_component(fs, c);

        c2 = icalcomponent_clone(c);

        (void)icalfileset_add_component(fs, c2);

        (void)icalfileset_commit(fs);
    }

    icalset_free(fs);

    assert(icalerrno == ICAL_NO_ERROR);

    pid = fork();

    assert(pid >= 0);

    if (pid == 0) {
        /*child */
        microsleep(rand() / (RAND_MAX / 100));

        for (int ii = 0; ii < 50; ii++) {
            fs = icalfileset_new(path);

            assert(fs != 0);

            c = icalfileset_get_first_component(fs);

            assert(c != 0);

            d = icalcomponent_get_duration(c);
            d = icaldurationtype_from_seconds(icaldurationtype_as_seconds(d) + 1);

            icalcomponent_set_duration(c, d);
            icalcomponent_set_summary(c, "Child");

            c2 = icalcomponent_clone(c);
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
        for (int ii = 0; ii < 50; ii++) {
            fs = icalfileset_new(path);

            assert(fs != 0);

            c = icalfileset_get_first_component(fs);

            assert(c != 0);

            d = icalcomponent_get_duration(c);
            d = icaldurationtype_from_seconds(icaldurationtype_as_seconds(d) + 1);

            icalcomponent_set_duration(c, d);
            icalcomponent_set_summary(c, "Parent");

            c2 = icalcomponent_clone(c);
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
    final = icaldurationtype_as_seconds(icalcomponent_get_duration(c));
    for (c = icalfileset_get_next_component(fs); c != 0; c = icalfileset_get_next_component(fs)) {
        struct icaldurationtype next_d = icalcomponent_get_duration(c);

        sec = icaldurationtype_as_seconds(next_d);

        /*printf("%d,%d ",i,sec); */
        assert(i == sec);
        i++;
    }

    printf("\nFinal: %d\n", final);

    assert(sec == final);
    _unused(sec);
#endif
}

void test_action(void)
{
    icalcomponent *c;
    icalproperty *p;

    static const char test_icalcomp_str[] =
        "BEGIN:VEVENT\r\n"
        "ACTION:EMAIL\r\n"
        "ACTION:PROCEDURE\r\n"
        "ACTION:AUDIO\r\n"
        "ACTION:FUBAR\r\n"
        "END:VEVENT\r\n";

    c = icalparser_parse_string((char *)test_icalcomp_str);

    ok("icalparser_parse_string(), ACTIONS", (c != NULL));
    assert(c != 0);

    const char *str = icalcomponent_as_ical_string(c);
    str_is("icalcomponent_as_ical_string()", str, ((char *)test_icalcomp_str));
    if (VERBOSE) {
        printf("%s\n\n", str);
    }

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

void test_trigger(void)
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
        "TRIGGER:-PT15M\r\n"
        "END:VEVENT\r\n";

    c = icalparser_parse_string((char *)test_icalcomp_str);
    ok("icalparser_parse_string()", (c != NULL));
    assert(c != NULL);

    str_is("parsed triggers", icalcomponent_as_ical_string(c), (char *)test_icalcomp_str);

    for (p = icalcomponent_get_first_property(c, ICAL_TRIGGER_PROPERTY);
         p != 0; p = icalcomponent_get_next_property(c, ICAL_TRIGGER_PROPERTY)) {
        tr = icalproperty_get_trigger(p);

        if (!icaltime_is_null_time(tr.time)) {
            if (VERBOSE) {
                printf("value=DATE-TIME:%s\n", icaltime_as_ical_string(tr.time));
            }
        } else {
            if (VERBOSE) {
                printf("value=DURATION:%s\n", icaldurationtype_as_ical_string(tr.duration));
            }
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
    /*TRIGGER, as a DURATION, VALUE=DURATION */
    tr.time = icaltime_null_time();
    tr.duration = icaldurationtype_from_string("P3DT3H50M45S");
    p = icalproperty_new_trigger(tr);
    icalproperty_add_parameter(p, icalparameter_new_value(ICAL_VALUE_DURATION));

    str = icalproperty_as_ical_string(p);

    str_is("TRIGGER:P3DT3H50M45S", str, "TRIGGER:P3DT3H50M45S\r\n");
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

    str_is("TRIGGER:P3DT3H50M45S", str, "TRIGGER:P3DT3H50M45S\r\n");
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

void test_rdate(void)
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
           "RDATE:19970101T120000\r\n");
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

    str_is("RDATE, as DATE-TIME, VALUE=PERIOD", str, "RDATE:19970101T120000\r\n");
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

void test_langbind(void)
{
    icalcomponent *c, *inner;
    icalproperty *p;
    const char *test_str_parsed;
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
        "FREEBUSY:19970101T120000/P3DT4H25M\n"
        "END:VEVENT\n";

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
        "FREEBUSY:19970101T120000/P3DT4H25M\r\n"
        "END:VEVENT\r\n";

    if (VERBOSE) {
        printf("%s\n", test_str);
    }

    c = icalparser_parse_string(test_str);

    ok("icalparser_parse_string()", (c != NULL));
    assert(c != NULL);

    test_str_parsed = icalcomponent_as_ical_string(c);

    str_is("parsed version with bad chars, etc", test_str_parsed, test_str_parsed_good);

    inner = icalcomponent_get_inner(c);

    for (p = icallangbind_get_first_property(inner, "ANY");
         p != 0; p = icallangbind_get_next_property(inner, "ANY")) {
        const char *str = icallangbind_property_eval_string(p, ":");

        /** TODO add tests **/
        if (VERBOSE) {
            printf("%s\n", str);
        }
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

void test_property_parse(void)
{
    icalcomponent *c;
    icalproperty *p;
    const char *str;

    static const char test_icalcomp_str[] =
        "BEGIN:VEVENT\n"
        "ATTENDEE;RSVP=TRUE;ROLE=REQ-PARTICIPANT;CUTYPE=GROUP:MAILTO:employee-A@host.com\n"
        "DTSTART:19970101T120000Z\n"
        "END:VEVENT\n";

    c = icalparser_parse_string((char *)test_icalcomp_str);
    ok("icalparser_parse_string()", (c != NULL));
    if (!c) {
        exit(EXIT_FAILURE);
    }

    p = icalcomponent_get_first_property(c, ICAL_ATTENDEE_PROPERTY);

    ok("icalproperty_from_string(), ATTENDEE", (p != 0));

    str = icalproperty_as_ical_string(p);
    if (VERBOSE) {
        printf("%s\n", str);
    }

    icalproperty_free(p);

    p = icalcomponent_get_first_property(c, ICAL_DTSTART_PROPERTY);

    ok("icalproperty_from_string(), simple DTSTART", (p != 0));

    str = icalproperty_as_ical_string(p);
    if (VERBOSE) {
        printf("%s\n", str);
    }

    icalproperty_free(p);
    icalcomponent_free(c);
}

void test_value_from_string(void)
{
    icalproperty *p = icalproperty_new_from_string("SUMMARY:foo");
    ok("value_from_string(), SUMMARY FOO", (p != 0));

    icalproperty_set_value_from_string(p, "not_a_boolean", "BOOLEAN");
    str_is("summary boolean value", icalproperty_as_ical_string(p), "SUMMARY:foo\r\n");

    icalproperty_set_value_from_string(p, "0.0;a", "GEO");
    str_is("summary geo value", icalproperty_as_ical_string(p), "SUMMARY:foo\r\n");
    icalproperty_free(p);
}

void test_value_parameter(void)
{
    icalcomponent *c;
    icalproperty *p;
    icalparameter *param;

    static const char test_icalcomp_str[] =
        "BEGIN:VEVENT\n"
        "DTSTART;VALUE=DATE-TIME:19971123T123000\n"
        "DTSTART;VALUE=DATE:19971123\n"
        "DTSTART;VALUE=FOO:19971123T123000\n"
        "END:VEVENT\n";

    c = icalparser_parse_string((char *)test_icalcomp_str);
    ok("icalparser_parse_string()", (c != NULL));
    if (!c) {
        exit(EXIT_FAILURE);
    }

    if (VERBOSE) {
        printf("%s", icalcomponent_as_ical_string(c));
    }

    p = icalcomponent_get_first_property(c, ICAL_DTSTART_PROPERTY);
    param = icalproperty_get_first_parameter(p, ICAL_VALUE_PARAMETER);

    ok("icalproperty_get_value()", (icalparameter_get_value(param) == ICAL_VALUE_DATETIME));

    p = icalcomponent_get_next_property(c, ICAL_DTSTART_PROPERTY);
    param = icalproperty_get_first_parameter(p, ICAL_VALUE_PARAMETER);
    ok("icalproperty_get_first_parameter()", (icalparameter_get_value(param) == ICAL_VALUE_DATE));

    icalcomponent_free(c);
}

void test_empty_parameter(void)
{
    icalcomponent *c;
    icalproperty *p;
    icalparameter *param;

    static const char test_icalcomp_str[] =
        "BEGIN:VEVENT\n"
        "ATTENDEE;CN=\"\";RSVP=TRUE;ROLE=REQ-PARTICIPANT;CUTYPE=GROUP:MAILTO:employee-A@host.com\n"
        "END:VEVENT\n";

    c = icalparser_parse_string((char *)test_icalcomp_str);
    ok("icalparser_parse_string()", (c != NULL));
    if (!c) {
        exit(EXIT_FAILURE);
    }

    if (VERBOSE) {
        printf("%s", icalcomponent_as_ical_string(c));
    }

    p = icalcomponent_get_first_property(c, ICAL_ATTENDEE_PROPERTY);
    param = icalproperty_get_first_parameter(p, ICAL_CN_PARAMETER);

    ok("icalparameter_get_cn()", (0 == strcmp("", icalparameter_get_cn(param))));

    icalcomponent_free(c);
}

void test_x_parameter(void)
{
    icalcomponent *c;
    icalproperty *p;

    static const char test_icalcomp_str[] =
        "BEGIN:VEVENT\n"
        "COMMENT;X-A=1;X-B=2:\\sThis is a note\n"
        "END:VEVENT\n";

    c = icalparser_parse_string((char *)test_icalcomp_str);
    ok("icalparser_parse_string()", (c != NULL));
    if (!c) {
        exit(EXIT_FAILURE);
    }

    if (VERBOSE) {
        printf("%s", icalcomponent_as_ical_string(c));
    }

    p = icalcomponent_get_first_property(c, ICAL_COMMENT_PROPERTY);
    icalproperty_set_parameter_from_string(p, "X-LIES", "no");
    icalproperty_set_parameter_from_string(p, "X-LAUGHS", "big");
    icalproperty_set_parameter_from_string(p, "X-TRUTH", "yes");
    icalproperty_set_parameter_from_string(p, "X-HUMOUR", "bad");

    if (VERBOSE) {
        printf("%s\n", icalproperty_as_ical_string(p));
    }

    str_is("COMMENT parses param", icalproperty_get_comment(p), " This is a note");

    str_is("Check X-LIES", icalproperty_get_parameter_as_string(p, "X-LIES"), "no");
    str_is("Check X-LAUGHS", icalproperty_get_parameter_as_string(p, "X-LAUGHS"), "big");
    str_is("Check X-TRUTH", icalproperty_get_parameter_as_string(p, "X-TRUTH"), "yes");
    str_is("Check X-HUMOUR", icalproperty_get_parameter_as_string(p, "X-HUMOUR"), "bad");

    icalcomponent_free(c);
}

void test_empty_property(void)
{
    icalcomponent *c;
    icalproperty *p;
    struct icaltimetype tt;

    static const char test_icalcomp_str[] =
        "BEGIN:VEVENT\n"
        "DESCRIPTION:\n"
        "DTSTART;TZID=America/New_York:\n"
        "GEO:\n" //structured
        "END:VEVENT\n";

    int estate = icalerror_get_errors_are_fatal();
    icalerror_set_errors_are_fatal(false);

    int pstate = icalproperty_get_allow_empty_properties();

    /* First test: do not allow empty properties */
    icalproperty_set_allow_empty_properties(false);

    static const char *test_icalcomp_str_out =
        "BEGIN:VEVENT\r\n"
        "X-LIC-ERROR;X-LIC-ERRORTYPE=VALUE-PARSE-ERROR:No value for DESCRIPTION \r\n"
        " property. Removing entire property:\r\n"
        "X-LIC-ERROR;X-LIC-ERRORTYPE=VALUE-PARSE-ERROR:No value for DTSTART \r\n"
        " property. Removing entire property:\r\n"
        "X-LIC-ERROR;X-LIC-ERRORTYPE=VALUE-PARSE-ERROR:No value for GEO property. \r\n"
        " Removing entire property:\r\n"
        "END:VEVENT\r\n";

    c = icalparser_parse_string((char *)test_icalcomp_str);
    ok("icalparser_parse_string()", (c != NULL));
    if (!c) {
        exit(EXIT_FAILURE);
    }

    if (VERBOSE) {
        printf("%s", icalcomponent_as_ical_string(c));
    }

    str_is("test results empty property", icalcomponent_as_ical_string(c), test_icalcomp_str_out);

    p = icalcomponent_get_first_property(c, ICAL_DESCRIPTION_PROPERTY);
    ok("description is empty", icalproperty_get_description(p) == NULL);
    p = icalcomponent_get_first_property(c, ICAL_DTSTART_PROPERTY);
    tt = icalproperty_get_dtstart(p);
    ok("dtstart is empty", icaltime_is_null_time(tt));

    icalcomponent_free(c);

    /* Second test: allow empty properties */
    icalproperty_set_allow_empty_properties(true);

    static const char *test_icalcomp_str_out_allow =
        "BEGIN:VEVENT\r\n"
        "DESCRIPTION:\r\n"
        "DTSTART;TZID=America/New_York:\r\n"
        "GEO:\r\n"
        "END:VEVENT\r\n";

    c = icalparser_parse_string((char *)test_icalcomp_str);
    ok("icalparser_parse_string()", (c != NULL));
    if (!c) {
        exit(EXIT_FAILURE);
    }

    if (VERBOSE) {
        printf("%s", icalcomponent_as_ical_string(c));
    }

    str_is("test results empty property", icalcomponent_as_ical_string(c), test_icalcomp_str_out_allow);

    p = icalcomponent_get_first_property(c, ICAL_DESCRIPTION_PROPERTY);
    ok("description is empty", icalproperty_get_description(p) == NULL);
    p = icalcomponent_get_first_property(c, ICAL_DTSTART_PROPERTY);
    tt = icalproperty_get_dtstart(p);
    ok("dtstart is empty", icaltime_is_null_time(tt));

    icalcomponent_free(c);

    icalproperty_set_allow_empty_properties(pstate);
    icalerror_set_errors_are_fatal(estate);
}

void test_x_property(void)
{
    icalcomponent *c;
    icalproperty *p;

    static const char test_icalcomp_str[] =
        "BEGIN:VEVENT\n"
        "X-LIC-PROPERTY:\\sThis is a note\n"
        "END:VEVENT\n";

    c = icalparser_parse_string((char *)test_icalcomp_str);
    ok("icalparser_parse_string()", (c != NULL));
    if (!c) {
        exit(EXIT_FAILURE);
    }

    if (VERBOSE) {
        printf("%s", icalcomponent_as_ical_string(c));
    }

    p = icalcomponent_get_first_property(c, ICAL_X_PROPERTY);
    ok("x-property is correct kind", (icalproperty_isa(p) == ICAL_X_PROPERTY));
    str_is("icalproperty_get_x_name() works", icalproperty_get_x_name(p), "X-LIC-PROPERTY");
    str_is("icalproperty_get_x() works", icalproperty_get_x(p), " This is a note");

    icalcomponent_free(c);
}

void test_utcoffset(void)
{
    icalcomponent *c;

    static const char test_icalcomp_str[] =
        "BEGIN:VTIMEZONE\n"
        "TZOFFSETFROM:-001608\n"
        "END:VTIMEZONE\n";

    c = icalparser_parse_string((char *)test_icalcomp_str);
    ok("parse TZOFFSETFROM:-001608", (c != NULL));

    if (VERBOSE && c) {
        printf("%s", icalcomponent_as_ical_string(c));
    }

    if (c) {
        icalcomponent_free(c);
    }
}

void test_attach(void)
{
    icalcomponent *c;

    static const char test_icalcomp_str[] =
        "BEGIN:VEVENT\n"
        "ATTACH:CID:jsmith.part3.960817T083000.xyzMain@host1.com\n"
        "ATTACH:FMTTYPE=application/postscript;ftp://xyzCorp.com/pub/reports/r-960812.ps\n"
        "END:VEVENT\n";

    c = icalparser_parse_string((char *)test_icalcomp_str);
    ok("parse simple attachment", (c != NULL));

    if (VERBOSE && c) {
        printf("%s", icalcomponent_as_ical_string(c));
    }

    if (c) {
        icalcomponent_free(c);
    }
}

void test_attach_caldav(void)
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

    if (VERBOSE && c) {
        printf("%s", icalcomponent_as_ical_string(c));
    }

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

    if (p) {
        icalproperty_free(p);
    }

    if (c) {
        icalcomponent_free(c);
    }
}

void test_attach_url(void)
{
    static const char test_icalcomp_str_attachwithurl[] =
        "BEGIN:VALARM\r\n"
        "ATTACH:foofile\r\n"
        "END:VALARM\r\n";

    icalattach *attach = icalattach_new_from_url("foofile");
    icalcomponent *ac = icalcomponent_new(ICAL_VALARM_COMPONENT);
    icalproperty *ap = icalproperty_new_attach(attach);

    icalcomponent_add_property(ac, ap);
    if (VERBOSE) {
        printf("%s\n", icalcomponent_as_ical_string(ac));
    }
    str_is("attach url", icalattach_get_url(attach), "foofile");
    str_is("attach with url", icalcomponent_as_ical_string(ac), test_icalcomp_str_attachwithurl);
    icalattach_unref(attach);
    icalproperty_free(ap);
    icalcomponent_free(ac);
}

static void test_free_attach_data(char *data, void *user_data)
{
    int *pbeen_called = (int *)user_data;

    free(data);

    (*pbeen_called)++;
}

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wanalyzer-malloc-leak"
#endif
void test_attach_data(void)
{
    static const char test_icalcomp_str_attachwithdata[] =
        "BEGIN:VALARM\r\n"
        "ATTACH;VALUE=BINARY:foofile\r\n"
        "END:VALARM\r\n";
    static const char test_icalcomp_str_attachwithencodingdata[] =
        "BEGIN:VALARM\r\n"
        "ATTACH;VALUE=BINARY;ENCODING=BASE64:YQECAAACAWEK\r\n"
        "END:VALARM\r\n";

    int free_been_called = 0;
    icalattach *attach = icalattach_new_from_data("foofile", NULL, 0);
    icalcomponent *ac = icalcomponent_new(ICAL_VALARM_COMPONENT);
    icalproperty *ap = icalproperty_new_attach(attach);

    icalcomponent_add_property(ac, ap);
    if (VERBOSE) {
        printf("%s\n", icalcomponent_as_ical_string(ac));
    }
    str_is("attach data", (const char *)icalattach_get_data(attach), "foofile");
    str_is("attach with data", icalcomponent_as_ical_string(ac), test_icalcomp_str_attachwithdata);

    icalattach_unref(attach);
    icalcomponent_free(ac);

    ac = icalcomponent_new_from_string(test_icalcomp_str_attachwithdata);
    ap = icalcomponent_get_first_property(ac, ICAL_ATTACH_PROPERTY);
    attach = icalproperty_get_attach(ap);
    str_is("attach data 2", (const char *)icalattach_get_data(attach), "foofile");
    str_is("attach with data 2", icalcomponent_as_ical_string(ac), test_icalcomp_str_attachwithdata);

    icalcomponent_free(ac);

    char *dupStr = strdup("foofile"); // will be freed in the unref
    if (dupStr) {
        attach = icalattach_new_from_data(dupStr, test_free_attach_data, &free_been_called);
        if (attach) {
            ac = icalcomponent_new(ICAL_VALARM_COMPONENT);
            ap = icalproperty_new_attach(attach);

            icalcomponent_add_property(ac, ap);
            if (VERBOSE) {
                printf("%s\n", icalcomponent_as_ical_string(ac));
            }
            str_is("attach data 3", (const char *)icalattach_get_data(attach), "foofile");
            str_is("attach with data 3", icalcomponent_as_ical_string(ac), test_icalcomp_str_attachwithdata);

            icalattach_unref(attach);
            ok("Free should not be called yet", (!free_been_called));
            icalcomponent_free(ac);
            ok("Free should be called now", (free_been_called == 1));
        } else {
            free(dupStr);
        }
    }

    ac = icalcomponent_new_from_string(test_icalcomp_str_attachwithencodingdata);
    ap = icalcomponent_get_first_property(ac, ICAL_ATTACH_PROPERTY);
    attach = icalproperty_get_attach(ap);
    str_is("attach data 4", (const char *)icalattach_get_data(attach), "YQECAAACAWEK");
    str_is("attach with data 4", icalcomponent_as_ical_string(ac), test_icalcomp_str_attachwithencodingdata);

    icalcomponent_free(ac);
}
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

void test_vcal(void)
{
    VObject *vcal;
    const char *file = TEST_DATADIR "/user-cal.vcf";

    vcal = Parse_MIME_FromFileName(file);

    ok("Parsing " TEST_DATADIR "/user-cal.vcf", (vcal != 0));

    if (vcal) {
        icalcomponent *comp = icalvcal_convert(vcal);

        ok("Converting to ical component", (comp != 0));

        if (VERBOSE && comp) {
            printf("%s\n", icalcomponent_as_ical_string(comp));
        }

        if (comp) {
            icalcomponent_free(comp);
        }

        cleanVObject(vcal);
    }
}

/*
 * Test to see if recurrences are excluded in certain situations
 * See r961 for more information
 */
void test_recurrenceexcluded(void)
{
    char funTime[2048] = {0};
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
    icalcomponent_free(calendar);
}

#if defined(USE_BUILTIN_TZDATA)
// this test is setup for testing against our builtin tzdata
void test_bad_dtstart_in_timezone(void)
{
    icaltimezone *myTZ = NULL;
    icalcomponent *vtimezone = NULL;
    char *str = NULL;

    myTZ = icaltimezone_get_builtin_timezone("Pacific/Guam");
    vtimezone = icaltimezone_get_component(myTZ);
    str = icalcomponent_as_ical_string(vtimezone);

    if (VERBOSE) {
        printf("%s\n", str);
    }
    ok("bad-dtstart-in-timezone.patch r960", (strstr(str, "DTSTART:19700906T020000") != NULL));
}
#endif

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
        "CLASS:PUBLIC\n"
        "TRANSP:OPAQUE\n"
        "SEQUENCE:1\n"
        "END:VEVENT\n"
        "END:VCALENDAR\n";
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
        "BEGIN:VEVENT\n"
        "X-TEST;VALUE=URI:\"geo:10.123456,-70.123456\"\n"
        "END:VEVENT\n";

    c = icalparser_parse_string((char *)test_icalcomp_str);
    ok("icalparser_parse_string()", (c != NULL));
    if (!c) {
        exit(EXIT_FAILURE);
    }

    if (VERBOSE) {
        printf("%s", icalcomponent_as_ical_string(c));
    }

    p = icalcomponent_get_first_property(c, ICAL_X_PROPERTY);
    ok("x-property is correct kind", (icalproperty_isa(p) == ICAL_X_PROPERTY));
    str_is("icalproperty_get_x_name() works", icalproperty_get_x_name(p), "X-TEST");
    str_is("icalproperty_get_value_as_string() works",
           icalproperty_get_value_as_string(p), "\"geo:10.123456,-70.123456\"");

    icalcomponent_free(c);
}

void test_geo_props(void)
{
    int estate;
    icalcomponent *c;
    icalproperty *p;

    c = icalparser_parse_string("BEGIN:VEVENT\n"
                                "GEO:49.42612;7.75473\n"
                                "END:VEVENT\n");
    ok("icalparser_parse_string()", (c != NULL));
    if (!c) {
        exit(EXIT_FAILURE);
    }
    if (VERBOSE) {
        printf("%s", icalcomponent_as_ical_string(c));
    }
    p = icalcomponent_get_first_property(c, ICAL_GEO_PROPERTY);
    str_is("icalproperty_get_value_as_string() works",
           icalproperty_get_value_as_string(p), "49.42612;7.75473");
    icalcomponent_free(c);

    c = icalparser_parse_string("BEGIN:VEVENT\n"
                                "GEO:-0;+0\n"
                                "END:VEVENT\n");
    ok("icalparser_parse_string()", (c != NULL));
    if (!c) {
        exit(EXIT_FAILURE);
    }
    if (VERBOSE) {
        printf("%s", icalcomponent_as_ical_string(c));
    }
    p = icalcomponent_get_first_property(c, ICAL_GEO_PROPERTY);
    str_is("icalproperty_get_value_as_string() works",
           icalproperty_get_value_as_string(p), "-0;+0");
    icalcomponent_free(c);

    /* failure situations */
    estate = icalerror_get_errors_are_fatal();
    icalerror_set_errors_are_fatal(false);
    c = icalparser_parse_string("BEGIN:VEVENT\n"
                                "GEO:-0a;+0\n"
                                "END:VEVENT\n");
    if (!c) {
        exit(EXIT_FAILURE);
    }
    if (VERBOSE) {
        printf("%s", icalcomponent_as_ical_string(c));
    }
    p = icalcomponent_get_first_property(c, ICAL_GEO_PROPERTY);
    ok("expected fail icalcomponent_get_first_property()", (p == NULL));
    icalcomponent_free(c);

    /* truncation situations */
    c = icalparser_parse_string("BEGIN:VEVENT\n"
                                "GEO:16.815151515151515151;+0\n"
                                "END:VEVENT\n");
    ok("icalparser_parse_string()", (c != NULL));
    if (!c) {
        exit(EXIT_FAILURE);
    }
    if (VERBOSE) {
        printf("%s", icalcomponent_as_ical_string(c));
    }
    p = icalcomponent_get_first_property(c, ICAL_GEO_PROPERTY);
    str_is("icalproperty_get_value_as_string() works",
           icalproperty_get_value_as_string(p), "16.815151515151;+0");
    icalcomponent_free(c);

    icalerror_set_errors_are_fatal(estate);
}

void test_zoneinfo_stuff(void)
{
#if defined(HAVE_SETENV)
    setenv("TZDIR", TEST_DATADIR, 1);
#else
    char tzdir[256] = {0};
    strncat(tzdir, "TZDIR=" TEST_DATADIR, 255);
    putenv(tzdir);
#endif
    icaltimezone_set_system_zone_directory(NULL); /*resets to empty */
    str_is("icaltimezone_get_system_zone_directory by TZDIR", icaltimezone_get_system_zone_directory(), TEST_DATADIR);
    icaltimezone_set_system_zone_directory("foo");
    str_is("icaltimezone_get_system_zone_directory", icaltimezone_get_system_zone_directory(), "foo");

    /* reset the environment */
#if defined(HAVE_SETENV)
    unsetenv("TZDIR");
#else
    strcpy(tzdir, "TZDIR=");
    putenv(tzdir);
#endif
    icaltimezone_set_system_zone_directory(NULL);
}

void test_tzid_with_utc_time(void)
{
    const char *strcomp =
        "BEGIN:VCALENDAR\r\n"
        "BEGIN:VTIMEZONE\r\n"
        "TZID:my_zone\r\n"
        "BEGIN:STANDARD\r\n"
        "TZNAME:my_zone\r\n"
        "DTSTART:19160429T230000\r\n"
        "TZOFFSETFROM:+0100\r\n"
        "TZOFFSETTO:+0200\r\n"
        "RRULE:FREQ=YEARLY;UNTIL=19160430T220000Z;BYDAY=-1SU;BYMONTH=4\r\n"
        "END:STANDARD\r\n"
        "END:VTIMEZONE\r\n"
        "BEGIN:VEVENT\r\n"
        "UID:0\r\n"
        "DTSTART;TZID=my_zone:20180101T010000Z\r\n"
        "DTEND;TZID=my_zone:20180101T030000\r\n"
        "DUE:20180101T030000Z\r\n"
        "RRULE:FREQ=HOURLY;UNTIL=20180505T050000Z;BYHOUR=1\r\n"
        "EXDATE:20180101T010000\r\n"
        "EXDATE:20180202T020000Z\r\n"
        "EXDATE;TZID=my_zone:20180303T030000\r\n"
        "EXDATE;TZID=my_zone:20180404T040000Z\r\n"
        "END:VEVENT\r\n"
        "END:VCALENDAR\r\n";
    const struct _exdate_expected {
        const char *tzid;
    } exdate_expected[] = {
        {""},
        {"UTC"},
        {"my_zone"},
        {"UTC"},
        {NULL}};
    icalcomponent *comp, *subcomp;
    struct icaltimetype dtstart, dtend, due;
    icalproperty *prop;
    int idx;

    comp = icalcomponent_new_from_string(strcomp);
    ok("icalcomponent_new_from_string()", (comp != NULL));
    subcomp = icalcomponent_get_first_component(comp, ICAL_VEVENT_COMPONENT);
    ok("get subcomp", (subcomp != NULL));

    dtstart = icalcomponent_get_dtstart(subcomp);
    dtend = icalcomponent_get_dtend(subcomp);
    due = icalcomponent_get_due(subcomp);

    ok("DTSTART is UTC", (icaltime_is_utc(dtstart)));
    ok("DTEND is my_zone", (strcmp(icaltimezone_get_tzid((icaltimezone *)dtend.zone), "my_zone") == 0));
    ok("DUE is UTC", (icaltime_is_utc(due)));

    for (prop = icalcomponent_get_first_property(subcomp, ICAL_EXDATE_PROPERTY), idx = 0;
         prop;
         prop = icalcomponent_get_next_property(subcomp, ICAL_EXDATE_PROPERTY), idx++) {
        struct icaltimetype exdate = icalproperty_get_exdate(prop);

        ok("EXDATE is not null-time", (!icaltime_is_null_time(exdate)));
        ok("expected EXDATE not NULL", (exdate_expected[idx].tzid != NULL));

        if (*(exdate_expected[idx].tzid)) {
            ok("EXDATE zone is not NULL", (exdate.zone != NULL));
            ok("EXDATE matches timezone", (strcmp(exdate_expected[idx].tzid, icaltimezone_get_tzid((icaltimezone *)exdate.zone)) == 0));
        } else {
            ok("EXDATE zone is NULL", (exdate.zone == NULL));
        }

        ok("EXDATE is excluded", (icalproperty_recurrence_is_excluded(subcomp, &dtstart, &exdate)));
    }

    icalcomponent_free(comp);
}

void test_recur_tzid(void)
{
    const char *calstr =
        "BEGIN:VCALENDAR\r\n"
        "BEGIN:VTIMEZONE\r\n"
        "TZID:test_tz\r\n"
        "BEGIN:STANDARD\r\n"
        "TZOFFSETFROM:-0400\r\n"
        "TZOFFSETTO:-0500\r\n"
        "TZNAME:EST\r\n"
        "DTSTART:19701101T020000\r\n"
        "RRULE:FREQ=YEARLY;BYMONTH=11;BYDAY=1SU\r\n"
        "END:STANDARD\r\n"
        "END:VTIMEZONE\r\n"
        "BEGIN:VEVENT\r\n"
        "DTSTART;TZID=test_tz:20250603T023000\r\n"
        "DTEND;TZID=test_tz:20250603T033000\r\n"
        "RRULE:FREQ=WEEKLY;BYDAY=TU\r\n"
        "UID:a\r\n"
        "END:VEVENT\r\n"
        "BEGIN:VEVENT\r\n"
        "DTSTART;TZID=test_tz:20250617T030000\r\n"
        "DTEND;TZID=test_tz:20250617T040000\r\n"
        "DTSTAMP:20250603T063931Z\r\n"
        "UID:a\r\n"
        "RECURRENCE-ID;TZID=test_tz:20250617T023000\r\n"
        "END:VEVENT\r\n"
        "END:VCALENDAR\r\n";

    icalcomponent *comp, *subcomp;
    struct icaltimetype recurid;

    comp = icalcomponent_new_from_string(calstr);
    ok("icalcomponent_new_from_string()", (comp != NULL));
    icalcomponent_get_first_component(comp, ICAL_VEVENT_COMPONENT);
    subcomp = icalcomponent_get_next_component(comp, ICAL_VEVENT_COMPONENT);
    ok("get subcomp", (subcomp != NULL));

    recurid = icalcomponent_get_recurrenceid(subcomp);

    ok("RECURRENCE-ID is test_tz", (strcmp(icaltimezone_get_tzid((icaltimezone *)recurid.zone), "test_tz") == 0));

    icalcomponent_free(comp);
}

void test_kind_to_string(void)
{
    // value testing
    ok("VALUE_KIND ICAL_ANY_VALUE is NULL",
       (icalvalue_kind_to_string(ICAL_ANY_VALUE) == NULL));
    str_is("VALUE_KIND ICAL_POLLCOMPLETION_VALUE",
           icalvalue_kind_to_string(ICAL_POLLCOMPLETION_VALUE), "POLLCOMPLETION");
    str_is("VALUE_KIND ICAL_NO_VALUE",
           icalvalue_kind_to_string(ICAL_NO_VALUE), "");

    ok("VALUE_KIND ICAL_ANY_VALUE is invalid",
       !icalvalue_kind_is_valid(ICAL_ANY_VALUE));
    ok("VALUE_KIND ICAL_POLLCOMPLETION_VALUE is valid",
       icalvalue_kind_is_valid(ICAL_POLLCOMPLETION_VALUE));
    ok("VALUE_KIND ICAL_NO_VALUE is valid",
       icalvalue_kind_is_valid(ICAL_NO_VALUE));

    // parameter testing
    ok("PARAMETER_KIND ICAL_ANY_PARAMETER is NULL",
       (icalparameter_kind_to_string(ICAL_ANY_PARAMETER) == NULL));
    str_is("PARAMETER_KIND ICAL_EMAIL_PARAMETER",
           icalparameter_kind_to_string(ICAL_EMAIL_PARAMETER), "EMAIL");
    str_is("PARAMETER_KIND ICAL_NO_PARAMETER",
           icalparameter_kind_to_string(ICAL_NO_PARAMETER), "");

    ok("PARAMETER_KIND ICAL_ANY_PARAMETER is invalid",
       !icalparameter_kind_is_valid(ICAL_ANY_PARAMETER));
    ok("PARAMETER_KIND ICAL_EMAIL_PARAMETER is valid",
       icalparameter_kind_is_valid(ICAL_EMAIL_PARAMETER));
    ok("PARAMETER_KIND ICAL_NO_PARAMETER is valid",
       icalparameter_kind_is_valid(ICAL_NO_PARAMETER));

    // property testing
    ok("PROPERTY_KIND ICAL_ANY_PROPERTY is NULL",
       (icalproperty_kind_to_string(ICAL_ANY_PROPERTY) == NULL));
    str_is("PROPERTY_KIND ICAL_VOTER_PROPERTY",
           icalproperty_kind_to_string(ICAL_VOTER_PROPERTY), "VOTER");
    str_is("PROPERTY_KIND ICAL_NO_PROPERTY",
           icalproperty_kind_to_string(ICAL_NO_PROPERTY), "");

    ok("PROPERTY_KIND ICAL_ANY_PROPERTY is invalid",
       !icalproperty_kind_is_valid(ICAL_ANY_PROPERTY));
    ok("PROPERTY_KIND ICAL_VOTER_PROPERTY is valid",
       icalproperty_kind_is_valid(ICAL_VOTER_PROPERTY));
    ok("PROPERTY_KIND ICAL_NO_PROPERTY is valid",
       icalproperty_kind_is_valid(ICAL_NO_PROPERTY));
}

void test_string_to_kind(void)
{
    ok("VALUE NULL is ICAL_NO_VALUE",
       (icalvalue_string_to_kind(NULL) == ICAL_NO_VALUE));
    int_is("ICAL_POLLCOMPLETION_VALUE is POLLCOMPLETION",
           (int)icalvalue_string_to_kind("POLLCOMPLETION"), ICAL_POLLCOMPLETION_VALUE);
    int_is("ICAL_NO_VALUE is empty string",
           (int)icalvalue_string_to_kind(""), ICAL_NO_VALUE);

    ok("PARAMETER NULL is ICAL_NO_PARAMETER",
       (icalparameter_string_to_kind(NULL) == ICAL_NO_PARAMETER));
    int_is("ICAL_EMAIL_PARAMETER is EMAIL",
           (int)icalparameter_string_to_kind("EMAIL"), ICAL_EMAIL_PARAMETER);
    int_is("ICAL_NO_PARAMETER is empty string",
           (int)icalparameter_string_to_kind(""), ICAL_NO_PARAMETER);

    ok("PROPERTY NULL is ICAL_NO_PROPERTY",
       (icalproperty_string_to_kind(NULL) == ICAL_NO_PROPERTY));
    int_is("ICAL_VOTER_PROPERTY is VOTER",
           (int)icalproperty_string_to_kind("VOTER"), ICAL_VOTER_PROPERTY);
    int_is("ICAL_NO_PROPERTY is empty string",
           (int)icalproperty_string_to_kind(""), ICAL_NO_PROPERTY);
}

void test_set_date_datetime_value(void)
{
    icalproperty *prop;
    icalparameter *param;
    struct icaltimetype itt;
    const char *propstr;

    itt = icaltime_current_time_with_zone(icaltimezone_get_utc_timezone());

    prop = icalproperty_new_dtstart(itt);
    propstr = icalproperty_as_ical_string(prop);
    icalproperty_free(prop);
    prop = icalproperty_new_from_string(propstr);
    ok("DATE-TIME on DTSTART is saved without VALUE",
       (icalproperty_get_first_parameter(prop, ICAL_VALUE_PARAMETER) == NULL));

    itt.is_date = 1;
    icalproperty_set_dtstart(prop, itt);
    propstr = icalproperty_as_ical_string(prop);
    icalproperty_free(prop);
    prop = icalproperty_new_from_string(propstr);
    param = icalproperty_get_first_parameter(prop, ICAL_VALUE_PARAMETER);
    ok("DATE on DTSTART is saved with VALUE", (param != NULL));
    ok("DATE on DTSTART is saved with DATE VALUE", (icalparameter_get_value(param) == ICAL_VALUE_DATE));

    itt.is_date = 0;
    icalproperty_set_dtstart(prop, itt);
    ok("VALUE parameter on DTSTART is removed right after set",
       (icalproperty_get_first_parameter(prop, ICAL_VALUE_PARAMETER) == NULL));
    propstr = icalproperty_as_ical_string(prop);
    icalproperty_free(prop);
    prop = icalproperty_new_from_string(propstr);
    ok("DATE-TIME on DTSTART is saved without VALUE",
       (icalproperty_get_first_parameter(prop, ICAL_VALUE_PARAMETER) == NULL));

    /* Try it twice */
    itt.is_date = 1;
    icalproperty_set_dtstart(prop, itt);
    propstr = icalproperty_as_ical_string(prop);
    icalproperty_free(prop);
    prop = icalproperty_new_from_string(propstr);
    param = icalproperty_get_first_parameter(prop, ICAL_VALUE_PARAMETER);
    ok("DATE on DTSTART is saved with VALUE", (param != NULL));
    ok("DATE on DTSTART is saved with DATE VALUE", (icalparameter_get_value(param) == ICAL_VALUE_DATE));

    icalproperty_free(prop);
}

void test_timezone_from_builtin(void)
{
    const char *strcomp_fmt =
        "BEGIN:VCALENDAR\r\n"
        "BEGIN:VTIMEZONE\r\n"
        "TZID:my_zone\r\n"
        "BEGIN:STANDARD\r\n"
        "TZNAME:my_zone\r\n"
        "DTSTART:19160429T230000\r\n"
        "TZOFFSETFROM:+0100\r\n"
        "TZOFFSETTO:+0200\r\n"
        "RRULE:FREQ=YEARLY;UNTIL=19160430T220000Z;BYDAY=-1SU;BYMONTH=4\r\n"
        "END:STANDARD\r\n"
        "END:VTIMEZONE\r\n"
        "BEGIN:VEVENT\r\n"
        "UID:0\r\n"
        "DTSTART;TZID=my_zone:20180101T010000\r\n"
        "DTEND;TZID=%s:20180101T030000\r\n"
        "DUE;TZID=Europe/Berlin:20180101T030000\r\n"
        "END:VEVENT\r\n"
        "END:VCALENDAR\r\n";
    icalcomponent *comp;
    icaltimezone *zone;
    char *strcomp, *tzidprefix, *prevslash = NULL, *prevprevslash = NULL, *p;
    size_t len;

    /* Builtins are not enabled, zone loading will fail. */
    if (!icaltimezone_get_builtin_tzdata()) {
        return;
    }

    zone = icaltimezone_get_builtin_timezone("America/New_York");
    tzidprefix = strdup(icaltimezone_get_tzid(zone));
    p = tzidprefix;
    while (p && (p = strchr(p + 1, '/'))) {
        prevprevslash = prevslash;
        prevslash = p;
    }
    if (prevprevslash) {
        prevprevslash[1] = 0;
    }

    icaltimezone_set_tzid_prefix(tzidprefix);

    len = strlen(strcomp_fmt) + strlen(icaltimezone_get_tzid(zone)) + 2;
    strcomp = (char *)malloc(len + 1);
    if (strcomp) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
        snprintf(strcomp, len, strcomp_fmt, icaltimezone_get_tzid(zone));
#pragma GCC diagnostic pop

        comp = icalcomponent_new_from_string(strcomp);
        ok("icalcomponent_new_from_string()", (comp != NULL));
        icalcomponent *subcomp = icalcomponent_get_first_component(comp, ICAL_VEVENT_COMPONENT);
        ok("get subcomp", (subcomp != NULL));

        struct icaltimetype dtstart = icalcomponent_get_dtstart(subcomp);
        struct icaltimetype dtend = icalcomponent_get_dtend(subcomp);
        struct icaltimetype due = icalcomponent_get_due(subcomp);

        ok("DTSTART is my_zone", (strcmp(icaltimezone_get_tzid((icaltimezone *)dtstart.zone), "my_zone") == 0));
        ok("DTEND is America/New_York", (strcmp(icaltimezone_get_location((icaltimezone *)dtend.zone), "America/New_York") == 0));
        ok("DUE is Europe/Berlin", (strcmp(icaltimezone_get_location((icaltimezone *)due.zone), "Europe/Berlin") == 0));

        icalcomponent_free(comp);
        free(strcomp);
    }

    free(tzidprefix);
    zone = icaltimezone_get_builtin_timezone("Pacific/Midway");
    ok("builtin location is Pacific/Midway", (strcmp(icaltimezone_get_location((icaltimezone *)zone), "Pacific/Midway") == 0));
    comp = icaltimezone_get_component(zone);
    strcomp = icalcomponent_as_ical_string_r(comp);
    comp = icalcomponent_new_from_string(strcomp);
    icalmemory_free_buffer(strcomp);

    ok("VTIMEZONE icalcomponent_new_from_string()", (comp != NULL));

    zone = icaltimezone_new();
    ok("set icaltimezone component", (icaltimezone_set_component(zone, comp)));
    ok("read from string builtin location is still Pacific/Midway", (strcmp(icaltimezone_get_location((icaltimezone *)zone), "Pacific/Midway") == 0));
    icaltimezone_free(zone, 1);

    icaltimezone_set_tzid_prefix(TESTS_TZID_PREFIX);
}

void test_icalvalue_decode_ical_string(void)
{
    char buff[12];
    const char *defvalue, *value;

    /* Without escape characters */
    defvalue = "xxxxx|VALUE";
    strcpy(buff, defvalue);
    value = buff + 6;

    ok("Fails to decode into too small buffer", (icalvalue_decode_ical_string(value, buff, 4) == 0));
    ok("Buffer not changed", (strcmp(buff, defvalue) == 0));
    ok("Fails to decode into small buffer (only without nul-terminator)", (icalvalue_decode_ical_string(value, buff, 5) == 0));
    ok("Buffer not changed", (strcmp(buff, defvalue) == 0));
    ok("Decodes into large-enough buffer", (icalvalue_decode_ical_string(value, buff, 6) != 0));
    ok("Properly decoded", (strcmp(buff, value) == 0));

    /* With escape character */
    defvalue = "xxxxx|a\\\\b!";
    strcpy(buff, defvalue);
    value = buff + 6;

    ok("Fails to decode into too small buffer", (icalvalue_decode_ical_string(value, buff, 3) == 0));
    ok("Buffer not changed", (strcmp(buff, defvalue) == 0));
    ok("Fails to decode into small buffer (only without nul-terminator)", (icalvalue_decode_ical_string(value, buff, 4) == 0));
    ok("Buffer not changed", (strcmp(buff, defvalue) == 0));
    ok("Decodes into large-enough buffer", (icalvalue_decode_ical_string(value, buff, 5) != 0));
    ok("Properly decoded", (strcmp(buff, "a\\b!") == 0));

    /* With ending escape character, which will be ignored */
    defvalue = "xxxxx|a\\\\\\";
    strcpy(buff, defvalue);
    value = buff + 6;

    ok("Fails to decode into too small buffer", (icalvalue_decode_ical_string(value, buff, 1) == 0));
    ok("Buffer not changed", (strcmp(buff, defvalue) == 0));
    ok("Fails to decode into small buffer (only without nul-terminator)", (icalvalue_decode_ical_string(value, buff, 2) == 0));
    ok("Buffer not changed", (strcmp(buff, defvalue) == 0));
    ok("Decodes into large-enough buffer", (icalvalue_decode_ical_string(value, buff, 3) != 0));
    ok("Properly decoded", (strcmp(buff, "a\\") == 0));
}

static int test_icalarray_sort_compare_char(const void *p1, const void *p2)
{
    char c1 = *((char *)p1);
    char c2 = *((char *)p2);

    return (c1 < c2) ? -1 : ((c1 > c2) ? 1 : 0);
}

void test_icalarray_sort(void)
{
    /* this test is based on the work from the PDCLib project */

    char presort[] = {"shreicnyjqpvozxmbt"};
    const char sorted1[] = {"bcehijmnopqrstvxyz"};
    unsigned int i;

    icalarray *array = icalarray_new(1, 2);

    for (i = 0; i < sizeof(presort) - 1; i++) {
        icalarray_append(array, &presort[i]);
    }

    icalarray_sort(array, test_icalarray_sort_compare_char);

    for (i = 0; i < sizeof(presort) - 1; i++) {
        void *pItem = icalarray_element_at(array, i);
        char c = *((char *)pItem);
        ok("icalarray_sort - item sorted as expected", c == sorted1[i]);
    }

    icalarray_free(array);
}

void test_icalcomponent_normalize(void)
{
    const char *calStr1 =
        "BEGIN:VCALENDAR\n"
        "PRODID:-//ACME//NONSGML ACME Calendar//EN\n"
        "VERSION:2.0\n"
        "CALSCALE:GREGORIAN\n"
        "BEGIN:VTIMEZONE\n"
        "TZID:America/New_York\n"
        "BEGIN:DAYLIGHT\n"
        "TZNAME:EDT\n"
        "TZOFFSETFROM:-0500\n"
        "TZOFFSETTO:-0400\n"
        "DTSTART:20070311T020000\n"
        "RRULE:FREQ=YEARLY;BYMONTH=3;BYDAY=2SU\n"
        "END:DAYLIGHT\n"
        "BEGIN:STANDARD\n"
        "TZNAME:EST\n"
        "TZOFFSETFROM:-0400\n"
        "TZOFFSETTO:-0500\n"
        "DTSTART:20071104T020000\n"
        "RRULE:FREQ=YEARLY;BYMONTH=11;BYDAY=1SU\n"
        "END:STANDARD\n"
        "END:VTIMEZONE\n"
        "BEGIN:VEVENT\n"
        "DTSTART;VALUE=DATE-TIME:20180220T020000Z\n"
        "DURATION:PT1H\n"
        "END:VEVENT\n"
        "END:VCALENDAR\n";
    const char *calStr2 =
        "BEGIN:VCALENDAR\n"
        "VERSION:2.0\n"
        "PRODID:-//ACME//NONSGML ACME Calendar//EN\n"
        "BEGIN:VEVENT\n"
        "TRANSP:OPAQUE\n"
        "DURATION:PT1H\n"
        "DTSTART;VALUE=DATE-TIME:20180220T020000Z\n"
        "END:VEVENT\n"
        "BEGIN:VTIMEZONE\n"
        "TZID:America/New_York\n"
        "BEGIN:STANDARD\n"
        "TZNAME:EST\n"
        "TZOFFSETFROM:-0400\n"
        "TZOFFSETTO:-0500\n"
        "DTSTART:20071104T020000\n"
        "RRULE:FREQ=YEARLY;BYMONTH=11;BYDAY=1SU\n"
        "END:STANDARD\n"
        "BEGIN:DAYLIGHT\n"
        "TZNAME:EDT\n"
        "TZOFFSETFROM:-0500\n"
        "TZOFFSETTO:-0400\n"
        "DTSTART:20070311T020000\n"
        "RRULE:FREQ=YEARLY;BYMONTH=3;BYDAY=2SU\n"
        "END:DAYLIGHT\n"
        "END:VTIMEZONE\n"
        "END:VCALENDAR\n";

    icalcomponent *ical1 = icalcomponent_new_from_string(calStr1);
    icalcomponent *ical2 = icalcomponent_new_from_string(calStr2);

    icalcomponent_normalize(ical1);
    icalcomponent_normalize(ical2);

    calStr1 = icalcomponent_as_ical_string(ical1);
    calStr2 = icalcomponent_as_ical_string(ical2);

    icalcomponent_free(ical1);
    icalcomponent_free(ical2);

    if (VERBOSE) {
        printf("iCal1:\n%s\n\n", calStr1);
        printf("iCal2:\n%s\n\n", calStr2);
    }

    str_is("Normalized components match", calStr1, calStr2);
}

void test_icalcomponent_normalize_missing_mandatory_props(void)
{
    // This test asserts that normalize does not crash when comparing
    // components which do not have mandatory properties set. If one
    // component has the property set and the other does not, then
    // the component having the property sorts first.

    const struct testcase {
        const char *calStr;
        const char *expect;
    } tests[] = {{.calStr =
                      "BEGIN:VEVENT\r\n"
                      "BEGIN:VALARM\r\n"
                      // no ACTION
                      // no TRIGGER
                      "END:VALARM\r\n"
                      "BEGIN:VALARM\r\n"
                      "ACTION:DISPLAY\r\n"
                      "TRIGGER:PT0S\r\n"
                      "END:VALARM\r\n"
                      "END:VEVENT\r\n",
                  .expect =
                      "BEGIN:VEVENT\r\n"
                      "BEGIN:VALARM\r\n"
                      "ACTION:DISPLAY\r\n"
                      "TRIGGER:PT0S\r\n"
                      "END:VALARM\r\n"
                      "BEGIN:VALARM\r\n"
                      // no ACTION
                      // no TRIGGER
                      "END:VALARM\r\n"
                      "END:VEVENT\r\n"},
                 {.calStr =
                      "BEGIN:VEVENT\r\n"
                      "BEGIN:VALARM\r\n"
                      // no ACTION
                      "TRIGGER:PT0S\r\n"
                      "END:VALARM\r\n"
                      "BEGIN:VALARM\r\n"
                      "ACTION:DISPLAY\r\n"
                      "TRIGGER:PT0S\r\n"
                      "END:VALARM\r\n"
                      "END:VEVENT\r\n",
                  .expect =
                      "BEGIN:VEVENT\r\n"
                      "BEGIN:VALARM\r\n"
                      "ACTION:DISPLAY\r\n"
                      "TRIGGER:PT0S\r\n"
                      "END:VALARM\r\n"
                      "BEGIN:VALARM\r\n"
                      // no ACTION
                      "TRIGGER:PT0S\r\n"
                      "END:VALARM\r\n"
                      "END:VEVENT\r\n"},
                 {
                     .calStr =
                         "BEGIN:VCALENDAR\r\n"
                         "BEGIN:VTIMEZONE\r\n"
                         // no TZID
                         "BEGIN:STANDARD\r\n"
                         "DTSTART:16010101T000000\r\n"
                         "TZOFFSETFROM:-0200\r\n"
                         "TZOFFSETTO:-0200\r\n"
                         "END:STANDARD\r\n"
                         "END:VTIMEZONE\r\n"
                         "BEGIN:VTIMEZONE\r\n"
                         "TZID:/test\r\n"
                         "BEGIN:STANDARD\r\n"
                         "DTSTART:16010101T000000\r\n"
                         "TZOFFSETFROM:-0200\r\n"
                         "TZOFFSETTO:-0200\r\n"
                         "END:STANDARD\r\n"
                         "END:VTIMEZONE\r\n"
                         "END:VCALENDAR\r\n",
                     .expect =
                         "BEGIN:VCALENDAR\r\n"
                         "BEGIN:VTIMEZONE\r\n"
                         "TZID:/test\r\n"
                         "BEGIN:STANDARD\r\n"
                         "DTSTART:16010101T000000\r\n"
                         "TZOFFSETFROM:-0200\r\n"
                         "TZOFFSETTO:-0200\r\n"
                         "END:STANDARD\r\n"
                         "END:VTIMEZONE\r\n"
                         "BEGIN:VTIMEZONE\r\n"
                         // no TZID
                         "BEGIN:STANDARD\r\n"
                         "DTSTART:16010101T000000\r\n"
                         "TZOFFSETFROM:-0200\r\n"
                         "TZOFFSETTO:-0200\r\n"
                         "END:STANDARD\r\n"
                         "END:VTIMEZONE\r\n"
                         "END:VCALENDAR\r\n",
                 },
                 {.calStr =
                      "BEGIN:VTIMEZONE\r\n"
                      "BEGIN:STANDARD\r\n"
                      // no DTSTART
                      "TZOFFSETFROM:-0200\r\n"
                      "TZOFFSETTO:-0200\r\n"
                      "END:STANDARD\r\n"
                      "BEGIN:STANDARD\r\n"
                      "DTSTART:16010101T000000\r\n"
                      "TZOFFSETFROM:-0200\r\n"
                      "TZOFFSETTO:-0200\r\n"
                      "END:STANDARD\r\n"
                      "END:VTIMEZONE\r\n",
                  .expect =
                      "BEGIN:VTIMEZONE\r\n"
                      "BEGIN:STANDARD\r\n"
                      "DTSTART:16010101T000000\r\n"
                      "TZOFFSETFROM:-0200\r\n"
                      "TZOFFSETTO:-0200\r\n"
                      "END:STANDARD\r\n"
                      "BEGIN:STANDARD\r\n"
                      // no DTSTART
                      "TZOFFSETFROM:-0200\r\n"
                      "TZOFFSETTO:-0200\r\n"
                      "END:STANDARD\r\n"
                      "END:VTIMEZONE\r\n"},
                 {NULL, NULL}};

    for (const struct testcase *tc = tests; tc->calStr; tc++) {
        icalcomponent *ical = icalcomponent_new_from_string(tc->calStr);
        icalcomponent_normalize(ical);
        const char *s = icalcomponent_as_ical_string(ical);
        icalcomponent_free(ical);

        if (VERBOSE) {
            printf("iCal:\n%s\n\n", s);
        }
        str_is("Normalized component matches", s, tc->expect);
    }
}

static void test_builtin_compat_tzid(void)
{
    struct _cases {
        const char *name;
        const char *tzid;
        int should_match;
    } cases[] = {
        {"Matches current TZID", "/freeassociation.sourceforge.net/Europe/London", 1},
        {"Matches Tzfile compat TZID", "/freeassociation.sourceforge.net/Tzfile/Europe/London", 1},
        {"Matches citadel.org TZID", "/citadel.org/20190914_1/Europe/London", 1},
        {"Does not match custom TZID", "/custom/test/tzid/Europe/London", 0},
        {NULL, NULL, 0}};
    int ii, jj;

    for (jj = 0; jj < 2; jj++) {
        if (jj == 1) {
            icaltimezone_set_tzid_prefix(NULL);
            icaltimezone_free_builtin_timezones();
        }

        const icaltimezone *tz = icaltimezone_get_builtin_timezone("Europe/London");
        for (ii = 0; cases[ii].tzid; ii++) {
            icaltimezone *zone;

            if (cases[ii].should_match) {
                zone = icaltimezone_get_builtin_timezone_from_tzid(cases[ii].tzid);
                ok(cases[ii].name, (zone == tz));
                if (zone != tz && VERBOSE) {
                    printf("Returned builtin zone (%s) doesn't match expected zone for TZID '%s'\n",
                           zone ? icaltimezone_get_location(zone) : "NULL",
                           cases[ii].tzid);
                }
            } else {
                zone = icaltimezone_get_builtin_timezone_from_tzid(cases[ii].tzid);
                ok(cases[ii].name, (!zone));
                if (zone != NULL && VERBOSE) {
                    printf("Returned builtin zone (%s), but it should fail for TZID '%s'\n",
                           icaltimezone_get_location(zone),
                           cases[ii].tzid);
                }
            }
        }
    }

    icaltimezone_set_tzid_prefix(TESTS_TZID_PREFIX);
    icaltimezone_free_builtin_timezones();
}

static void test_vcc_vcard_parse(void)
{
    /* Two VCARD-s, because some arches can parse the first and some the second. */
    const char *vcard1 =
        "BEGIN:VCARD\r\n"
        "VERSION:3.0\r\n"
        "NOTE:\r\n"
        "FN:Xxxx\r\n"
        "N:;Xxxx;;;\r\n"
        "END:VCARD";
    const char *vcard2 =
        "BEGIN:VCARD\r\n"
        "VERSION:3.0\r\n"
        "X-XXX-ORIGINAL-VCARD:\r\n"
        "X-XXX-KIND:XX_XXXXXXXX\r\n"
        "NOTE:\r\n"
        "X-XXXXXXXXX-FILE-AS:Xxxxxx\r\n"
        "FN:Xxxxxx\r\n"
        "N:;Xxxxxx;;;\r\n"
        "X-XXX-XXXXXXXXX:\r\n"
        "END:VCARD";
    const char *vcalendar =
        "BEGIN:VCALENDAR\r\n"
        "BEGIN:VEVENT\r\n"
        "UID:123\r\n"
        "SUMMARY:Summary\r\n"
        "DTSTAMP:20210803T063522Z\r\n"
        "DTSTART;VALUE=DATE:20210902\r\n"
        "END:VEVENT\r\n"
        "END:VCALENDAR\r\n";
    const char *vcalendar_broken =
        "BEGIN:VCALENDAR\r\n"
        "BEGIN:VTIMEZONE\r\n"
        "TZID:tz/id\r\n"
        "BEGIN:STANDARD\r\n"
        "TZNAME:PMT\r\n"
        "TZOFFSETFROM:+005744\r\n"
        "TZOFFSETTO:+005744\r\n"
        "DTSTART:18500101T000000\r\n"
        "END:STANDARD\r\n"
        "BEGIN:STANDARD\r\n"
        "TZNAME:CET\r\n"
        "TZOFFSETFROM:+005744\r\n"
        "TZOFFSETTO:+0100\r\n"
        "DTSTART:18911001T000000\r\n"
        "END:STANDARD\r\n"
        "BEGIN:DAYLIGHT\r\n"
        "TZNAME:CEST\r\n"
        "TZOFFSETFROM:+0100\r\n"
        "TZOFFSETTO:+0200\r\n"
        "DTSTART:19160430T230000\r\n"
        "END:DAYLIGHT\r\n"
        "END:VTIMEZONE\r\n"
        "BEGIN:VEVENT\r\n"
        "UID:321\r\n"
        "SUMMARY:Summary\r\n"
        "DTSTAMP:20210803T063522Z\r\n"
        "DTSTART;VALUE=DATE:20210902\r\n"
        "END:VEVENT\r\n"
        "BEGIN:VEVENT\r\n"
        "UID:123\r\n"
        "DTSTAMP:20210803T063522Z\r\n"
        "DTSTART;VALUE=DATE:20210902\r\n"
        "BEGIN:VALARM\r\n"
        "ACTION:DISPLAY\r\n"
        "TRIGGER:-PT15M\r\n"
        "END:VALARM\r\n"
        "DESCRIPTION:aaa \r\n"
        "\r\n"
        " aaa\\naaa 1\\n \r\n"
        "SUMMARY:Summary\r\n"
        "END:VEVENT\r\n"
        "END:VCALENDAR\r\n";
    VObject *vcal;

    vcal = Parse_MIME(vcard1, (unsigned long)strlen(vcard1));
    if (vcal) {
        icalcomponent *icalcomp;

        icalcomp = icalvcal_convert(vcal);
        ok("vCard1 is not iCalendar", (icalcomp == NULL));
        if (icalcomp) {
            icalcomponent_free(icalcomp);
        }

        cleanVObject(vcal);
    } else {
        ok("vCard1 cannot be parsed", (vcal == NULL));
    }

    vcal = Parse_MIME(vcard2, (unsigned long)strlen(vcard2));
    if (vcal) {
        icalcomponent *icalcomp;

        icalcomp = icalvcal_convert(vcal);
        ok("vCard2 is not iCalendar", (icalcomp == NULL));
        if (icalcomp) {
            icalcomponent_free(icalcomp);
        }

        cleanVObject(vcal);
    } else {
        ok("vCard2 cannot be parsed", (vcal == NULL));
    }

    vcal = Parse_MIME(vcalendar, (unsigned long)strlen(vcalendar));
    ok("vCalendar can be parsed", (vcal != NULL));
    if (vcal) {
        icalcomponent *icalcomp;

        icalcomp = icalvcal_convert(vcal);
        ok("vCalendar can be converted", (icalcomp != NULL));
        if (icalcomp) {
            icalcomponent *child;

            ok("vCalendar is VCALENDAR", (icalcomponent_isa(icalcomp) == ICAL_VCALENDAR_COMPONENT));
            ok("vCalendar has one child", (icalcomponent_count_components(icalcomp, ICAL_ANY_COMPONENT) == 1));
            child = icalcomponent_get_inner(icalcomp);
            ok("vCalendar has inner comp", (child != NULL && child != icalcomp));
            ok("vCalendar child is VEVENT", (icalcomponent_isa(child) == ICAL_VEVENT_COMPONENT));
            ok("vCalendar child UID matches", (strcmp(icalcomponent_get_uid(child), "123") == 0));
            ok("vCalendar child SUMMARY matches", (strcmp(icalcomponent_get_summary(child), "Summary") == 0));
            icalcomponent_free(icalcomp);
        }

        cleanVObject(vcal);
    }

    vcal = Parse_MIME(vcalendar_broken, (unsigned long)strlen(vcalendar_broken));
    ok("vCalendar-broken cannot be parsed", (vcal == NULL));
}

static void test_implicit_dtend_duration(void)
{
    const struct icaltimetype start1 = icaltime_from_string("20220108");
    icalcomponent *c = icalcomponent_vanew(
        ICAL_VCALENDAR_COMPONENT,
        icalcomponent_vanew(
            ICAL_VEVENT_COMPONENT,
            icalproperty_vanew_dtstart(start1, (void *)0),
            (void *)0),
        (void *)0);
    struct icaldurationtype d = icalcomponent_get_duration(c);
    struct icaltimetype end = icalcomponent_get_dtend(c),
                        start = icaltime_from_string("20220108T101010Z");
    if (VERBOSE) {
        printf("%s\n", icaldurationtype_as_ical_string(d));
    }
    str_is("icaldurationtype_as_ical_string(d)", "P1D", icaldurationtype_as_ical_string(d));

    if (VERBOSE) {
        printf("%s\n", icaltime_as_ical_string(end));
    }
    str_is("icaltime_as_ical_string(end)", "20220109", icaltime_as_ical_string(end));

    icalcomponent_set_dtstart(c, start);
    d = icalcomponent_get_duration(c);
    end = icalcomponent_get_dtend(c);
    if (VERBOSE) {
        printf("%s\n", icaldurationtype_as_ical_string(d));
    }
    int_is("icaldurationtype_as_seconds(d)", 0, icaldurationtype_as_seconds(d));

    if (VERBOSE) {
        printf("%s\n", icaltime_as_ical_string(end));
    }
    int_is("icaltime_compare(start, end)", 0, icaltime_compare(start, end));
    icalcomponent_free(c);

    c = icalcomponent_vanew(
        ICAL_VCALENDAR_COMPONENT,
        icalcomponent_vanew(
            ICAL_VTODO_COMPONENT,
            icalproperty_vanew_dtstart(start1, (void *)0),
            (void *)0),
        (void *)0);
    icalcomponent_set_due(c, icaltime_from_string("20220109"));
    d = icalcomponent_get_duration(c);
    end = icalcomponent_get_dtend(c);
    if (VERBOSE) {
        printf("%s\n", icaldurationtype_as_ical_string(d));
    }
    str_is("P1D", "P1D", icaldurationtype_as_ical_string(d));

    if (VERBOSE) {
        printf("%i\n", icaltime_is_null_time(end));
    }
    int_is("icaltime_is_null_time(end)", 1, icaltime_is_null_time(end));
    icalcomponent_free(c);
}

static void test_icalcomponent_get_dtend_from_duration_single(const char *dtstart, const char *duration, const char *expected_dtend)
{
    icalcomponent *comp = icalcomponent_new(ICAL_VEVENT_COMPONENT);

    icaltimetype dtstartt = icaltime_from_string(dtstart);
    dtstartt.zone = icaltimezone_get_builtin_timezone("Europe/Vienna");
    icalcomponent_set_dtstart(comp, dtstartt);

    icalcomponent_set_duration(comp, icaldurationtype_from_string(duration));

    ok("icalcomponent_get_dtend must consider DURATION's time part as accurate.", icaltime_compare(icalcomponent_get_dtend(comp), icaltime_from_string(expected_dtend)) == 0);

    icalcomponent_free(comp);
}

static void test_icalcomponent_get_dtend_from_duration(void)
{
    // The test cases refer to the 'Europe/Vienna' time zone, where DST ended at 2022-10-30 3:00.
    // Duration doesn't cross DST end.
    test_icalcomponent_get_dtend_from_duration_single("20221029T010000", "PT4H", "20221029T050000");

    // Duration crosses DST end. As it's only the time part, it must be considered as accurate duration.
    test_icalcomponent_get_dtend_from_duration_single("20221030T010000", "PT4H", "20221030T040000");

    // The date part is added first, which doesn't cross DST end.
    // Afterwards the time part is added, which crosses DST end. The time part must be considered as accurate duration.
    test_icalcomponent_get_dtend_from_duration_single("20221029T010000", "P1DT4H", "20221030T040000");

    // The date part is added first, which crosses DST end. The date part must be considered as nominal duration.
    // Afterwards the time part is added, which doesn't crross DST end.
    test_icalcomponent_get_dtend_from_duration_single("20221030T010000", "P1DT4H", "20221031T050000");

    // The duration exceeds 1D but is specified in hours, so it must be considered as accurate duration.
    test_icalcomponent_get_dtend_from_duration_single("20221030T010000", "PT28H", "20221031T040000");

    test_icalcomponent_get_dtend_from_duration_single("20221030T010000", "P2D", "20221101T010000");

    test_icalcomponent_get_dtend_from_duration_single("20220327T010000", "P2D", "20220329T010000");
}

static void
test_icalvalue_resets_timezone_on_set(void)
{
    const char *strcomp =
        "BEGIN:VCALENDAR\r\n"
        "BEGIN:VTIMEZONE\r\n"
        "TZID:my_zone\r\n"
        "BEGIN:STANDARD\r\n"
        "TZNAME:my_zone\r\n"
        "DTSTART:19160429T230000\r\n"
        "TZOFFSETFROM:+0100\r\n"
        "TZOFFSETTO:+0200\r\n"
        "RRULE:FREQ=YEARLY;UNTIL=19160430T220000Z;BYDAY=-1SU;BYMONTH=4\r\n"
        "END:STANDARD\r\n"
        "END:VTIMEZONE\r\n"
        "BEGIN:VEVENT\r\n"
        "UID:0\r\n"
        "DTSTART;TZID=my_zone:20180101T010000\r\n"
        "DTEND:20180202T020000Z\r\n"
        "DUE:20180302T030000\r\n"
        "END:VEVENT\r\n"
        "END:VCALENDAR\r\n";
    icalcomponent *comp, *clone, *inner;
    icaltimetype comp_dtstart, comp_dtend, comp_due;
    icaltimetype clone_dtstart, clone_dtend, clone_due;
    const char *orig_str, *clone_str;
    int estate;

    estate = icalerror_get_errors_are_fatal();
    icalerror_set_errors_are_fatal(false);

    /* First try without calling 'set' */
    comp = icalcomponent_new_from_string(strcomp);
    ok("1st - vCalendar can be parsed", (comp != NULL));
    inner = icalcomponent_get_inner(comp);
    ok("1st - inner exists", (inner != NULL));
    orig_str = icalcomponent_as_ical_string(inner);
    comp_dtstart = icalcomponent_get_dtstart(inner);
    comp_dtend = icalcomponent_get_dtend(inner);
    comp_due = icalcomponent_get_due(inner);
    ok("1st - comp dtstart is non-UTC zone", (comp_dtstart.zone != NULL && comp_dtstart.zone != icaltimezone_get_utc_timezone()));
    ok("1st - comp dtend is UTC zone", (comp_dtend.zone == icaltimezone_get_utc_timezone()));
    ok("1st - comp due is floating", (comp_due.zone == NULL));
    clone = icalcomponent_clone(inner);
    icalcomponent_free(comp);
    /* note the comp_dtstart.zone points to a freed memory now (it was freed with the 'comp') */
    clone_dtstart = icalcomponent_get_dtstart(clone);
    clone_dtend = icalcomponent_get_dtend(clone);
    clone_due = icalcomponent_get_due(clone);
    ok("1st - clone dtstart is null zone", (clone_dtstart.zone == NULL));
    ok("1st - clone dtend is UTC zone", (clone_dtend.zone == icaltimezone_get_utc_timezone()));
    ok("1st - clone due is floating", (clone_due.zone == NULL));
    clone_str = icalcomponent_as_ical_string(clone);
    ok("1st - clone and orig components match", (strcmp(orig_str, clone_str) == 0));
    icalcomponent_free(clone);

    /* Second try with calling 'set' */
    comp = icalcomponent_new_from_string(strcomp);
    inner = icalcomponent_get_inner(comp);
    orig_str = icalcomponent_as_ical_string(inner);
    comp_dtstart = icalcomponent_get_dtstart(inner);
    comp_dtend = icalcomponent_get_dtend(inner);
    comp_due = icalcomponent_get_due(inner);
    ok("2nd - comp dtstart is non-UTC zone", (comp_dtstart.zone != NULL && comp_dtstart.zone != icaltimezone_get_utc_timezone()));
    ok("2nd - comp dtend is UTC zone", (comp_dtend.zone == icaltimezone_get_utc_timezone()));
    ok("2nd - comp due is floating", (comp_due.zone == NULL));
    icalcomponent_set_dtstart(inner, comp_dtstart);
    icalcomponent_set_dtend(inner, comp_dtend);
    icalcomponent_set_due(inner, comp_due);
    comp_dtstart = icalcomponent_get_dtstart(inner);
    comp_dtend = icalcomponent_get_dtend(inner);
    comp_due = icalcomponent_get_due(inner);
    ok("2nd - comp dtstart is non-UTC zone", (comp_dtstart.zone != NULL && comp_dtstart.zone != icaltimezone_get_utc_timezone()));
    ok("2nd - comp dtend is UTC zone after set", (comp_dtend.zone == icaltimezone_get_utc_timezone()));
    ok("2nd - comp due is floating after set", (comp_due.zone == NULL));
    clone = icalcomponent_clone(inner);
    icalcomponent_free(comp);
    /* note the comp_dtstart.zone points to a freed memory now (it was freed with the 'comp') */
    clone_dtstart = icalcomponent_get_dtstart(clone);
    clone_dtend = icalcomponent_get_dtend(clone);
    clone_due = icalcomponent_get_due(clone);
    ok("2nd - clone dtstart is null zone", (clone_dtstart.zone == NULL));
    ok("2nd - clone dtend is UTC zone", (clone_dtend.zone == icaltimezone_get_utc_timezone()));
    ok("2nd - clone due is floating", (clone_due.zone == NULL));
    clone_str = icalcomponent_as_ical_string(clone);
    ok("2nd - clone and orig components match", (strcmp(orig_str, clone_str) == 0));
    icalcomponent_free(clone);

    icalerror_set_errors_are_fatal(estate);
    icalerror_clear_errno();
}

static void test_comma_in_xproperty(void)
{
    // X-property value without explicit value type
    icalproperty *xproperty = icalproperty_new_from_string("X-TEST-PROPERTY:test,test");
    icalcomponent *c;

    c = icalcomponent_vanew(
        ICAL_VCALENDAR_COMPONENT,
        icalcomponent_vanew(
            ICAL_VEVENT_COMPONENT,
            xproperty,
            (void *)0),
        (void *)0);

    str_is("icalproperty_as_ical_string()", "X-TEST-PROPERTY:test,test\r\n", icalproperty_as_ical_string(icalcomponent_get_first_property(icalcomponent_get_inner(c), ICAL_X_PROPERTY)));

    icalcomponent_free(c);

    // X-property value with TEXT value type
    xproperty = icalproperty_new_from_string("X-TEST-PROPERTY;VALUE=TEXT:test\\,test");

    c = icalcomponent_vanew(
        ICAL_VCALENDAR_COMPONENT,
        icalcomponent_vanew(
            ICAL_VEVENT_COMPONENT,
            xproperty,
            (void *)0),
        (void *)0);

    str_is("icalproperty_as_ical_string()", "X-TEST-PROPERTY;VALUE=TEXT:test\\,test\r\n", icalproperty_as_ical_string(icalcomponent_get_first_property(icalcomponent_get_inner(c), ICAL_X_PROPERTY)));

    icalcomponent_free(c);
}

static void test_remove_tzid_from_due(void)
{
    icalproperty *due = icalproperty_vanew_due(icaltime_from_string("20220120T120000"), (void *)0);
    icalcomponent *c;

    icalproperty_add_parameter(due, icalparameter_new_tzid("America/New_York"));

    c = icalcomponent_vanew(
        ICAL_VCALENDAR_COMPONENT,
        icalcomponent_vanew(
            ICAL_VTODO_COMPONENT,
            due,
            (void *)0),
        (void *)0);

    icalcomponent_set_due(c, icaltime_from_string("20220120"));
    str_is("icalproperty_as_ical_string()", "DUE;VALUE=DATE:20220120\r\n", icalproperty_as_ical_string(icalcomponent_get_first_property(icalcomponent_get_inner(c), ICAL_DUE_PROPERTY)));

    icalcomponent_free(c);
}

void test_icaltime_as_timet(void)
{
    icaltimetype tt;
    time_t expectedTimeT;

    ok("icaltime_from_string translates 19011231T235959Z to -1", icaltime_as_timet(icaltime_from_string("19011231T235959Z")) == -1);

    ok("icaltime_from_string translates 19020101T000000Z to -2145916800", icaltime_as_timet(icaltime_from_string("19020101T000000Z")) == -2145916800);
    ok("icaltime_from_string translates 19290519T000000Z to -1281916800", icaltime_as_timet(icaltime_from_string("19290519T000000Z")) == -1281916800);
    ok("icaltime_from_string translates 19561004T000000Z to -417916800", icaltime_as_timet(icaltime_from_string("19561004T000000Z")) == -417916800);
    ok("icaltime_from_string translates 19700101T000000Z to 0", icaltime_as_timet(icaltime_from_string("19700101T000000Z")) == 0);
    ok("icaltime_from_string translates 19700301T235959Z to 5183999", icaltime_as_timet(icaltime_from_string("19700301T235959Z")) == 5183999);
    ok("icaltime_from_string translates 19970717T235959Z to 869183999", icaltime_as_timet(icaltime_from_string("19970717T235959Z")) == 869183999);
    ok("icaltime_from_string translates 20241202T235959Z to 1733183999", icaltime_as_timet(icaltime_from_string("20241202T235959Z")) == 1733183999);
    ok("icaltime_from_string translates 20371231T235959Z to 2145916799", icaltime_as_timet(icaltime_from_string("20371231T235959Z")) == 2145916799);
#if (SIZEOF_ICALTIME_T > 4)
    ok("icaltime_from_string translates 20520419T235959Z to 2597183999", icaltime_as_timet(icaltime_from_string("20520419T235959Z")) == 2597183999);
    ok("icaltime_from_string translates 20790905T235959Z to 3461183999", icaltime_as_timet(icaltime_from_string("20790905T235959Z")) == 3461183999);
    ok("icaltime_from_string translates 21000101T235959Z to 4102531199", icaltime_as_timet(icaltime_from_string("21000101T235959Z")) == 4102531199);
    ok("icaltime_from_string translates 21000301T235959Z to 4107628799", icaltime_as_timet(icaltime_from_string("21000301T235959Z")) == 4107628799);
    ok("icaltime_from_string translates 22370122T235959Z to 8427628799", icaltime_as_timet(icaltime_from_string("22370122T235959Z")) == 8427628799);
    ok("icaltime_from_string translates 23731215T235959Z to 12747628799", icaltime_as_timet(icaltime_from_string("23731215T235959Z")) == 12747628799);
    ok("icaltime_from_string translates 25101107T235959Z to 17067628799", icaltime_as_timet(icaltime_from_string("25101107T235959Z")) == 17067628799);
    ok("icaltime_from_string translates 25821231T235959Z to 19344441599", icaltime_as_timet(icaltime_from_string("25821231T235959Z")) == 19344441599);
    ok("icaltime_from_string translates 99991231T235959Z to 253402300799", icaltime_as_timet(icaltime_from_string("99991231T235959Z")) == 253402300799);

    tt = icaltime_from_string("99991231T235959Z");
    icaltime_adjust(&tt, 0, 0, 0, 1);
    ok("icaltime_from_string translates 100000101T000000Z to -1", icaltime_as_timet(tt) == -1);
#else
    ok("icaltime_from_string translates 20380118T000000Z to -1", icaltime_as_timet(icaltime_from_string("20380118T000000Z")) == -1);
#endif

    tt = icaltime_from_string("19020101T000000Z");
    expectedTimeT = -2145916800;

#if (SIZEOF_TIME_T > 4)
    // Going through each day until 10000 takes ~250ms on a reasonably powered year 2020 business laptop.
    while (tt.year < 10000)
#else
    while ((tt.year < 2038) || ((tt.year == 2038) && (tt.month == 1) && (tt.day <= 17)))
#endif
    {
        time_t actualTimeT = icaltime_as_timet(tt);
        if (actualTimeT != expectedTimeT) {
            ok("icaltime_as_timet translates correctly", actualTimeT == expectedTimeT);
        }

        icaltime_adjust(&tt, 1, 0, 0, 0);
        expectedTimeT += (24 * 60 * 60);
    }

    ok("icaltime_as_timet translates out of bounds correctly", icaltime_as_timet(tt) == -1);
}

void test_icalcomponent_with_lastmodified(void)
{
    /* for https://github.com/libical/libical/issues/585 */

    icalcomponent *comp;
    struct icaltimetype lm = icaltime_from_timet_with_zone(1661280150, 0, NULL);
    comp = icalcomponent_vanew(ICAL_VCALENDAR_COMPONENT,
                               icalproperty_new_version("2.0"),
                               icalproperty_new_prodid("PROD-ABC"),
                               icalproperty_new_uid("1234abcd"),
                               icalproperty_new_lastmodified(lm),
                               icalproperty_new_name("name1"),
                               (void *)0);
    icalcomponent_free(comp);
}

void test_tzid_setter(void)
{
    char *saveTzid, *builtinTzid;

    /* save current tzid for restoring later */
    saveTzid = icalmemory_strdup(icaltimezone_tzid_prefix());

    /* reset to library builtin tzid */
    icaltimezone_set_tzid_prefix(NULL);
    builtinTzid = icalmemory_strdup(icaltimezone_tzid_prefix());

    /* setting/unsetting the testing tzid */
    icaltimezone_set_tzid_prefix(TESTS_TZID_PREFIX);
    str_is("new tzid is set correctly", icaltimezone_tzid_prefix(), TESTS_TZID_PREFIX);
    icaltimezone_set_tzid_prefix(NULL);
    str_is("reset tzid to default value", icaltimezone_tzid_prefix(), builtinTzid);

    /* Allow erasing the current tzid */
    icaltimezone_set_tzid_prefix("");
    str_is("reset tzid to default", icaltimezone_tzid_prefix(), "");

    /* restore to original tzid */
    icaltimezone_set_tzid_prefix(saveTzid);
    str_is("reset tzid to initial value", icaltimezone_tzid_prefix(), saveTzid);

    icalmemory_free_buffer(saveTzid);
    icalmemory_free_buffer(builtinTzid);
}

static void verify_comp_attendee(icalcomponent *comp)
{
    icalproperty *prop;

#define get_param(_kind, _suffix) icalparameter_get_##_suffix(icalproperty_get_first_parameter(prop, _kind))

    prop = icalcomponent_get_first_property(comp, ICAL_ATTENDEE_PROPERTY);
    str_is("value", icalproperty_get_attendee(prop), "mailto:att1");
    str_is("member", icalparameter_get_member_nth(icalproperty_get_first_parameter(prop, ICAL_MEMBER_PARAMETER), 0), "member");
    ok("cutype", get_param(ICAL_CUTYPE_PARAMETER, cutype) == ICAL_CUTYPE_INDIVIDUAL);
    ok("role", get_param(ICAL_ROLE_PARAMETER, role) == ICAL_ROLE_CHAIR);
    ok("partstat", get_param(ICAL_PARTSTAT_PARAMETER, partstat) == ICAL_PARTSTAT_NEEDSACTION);
    ok("rsvp", (get_param(ICAL_RSVP_PARAMETER, rsvp) == ICAL_RSVP_FALSE));
    str_is("delegatedfrom", icalparameter_get_delegatedfrom_nth(icalproperty_get_first_parameter(prop, ICAL_DELEGATEDFROM_PARAMETER), 0), "mailto:delgfrom");
    str_is("delegatedto", icalparameter_get_delegatedto_nth(icalproperty_get_first_parameter(prop, ICAL_DELEGATEDTO_PARAMETER), 0), "mailto:delgto");
    str_is("sentby", get_param(ICAL_SENTBY_PARAMETER, sentby), "mailto:sentby");
    str_is("cn", get_param(ICAL_CN_PARAMETER, cn), "First attendee");
    str_is("language", get_param(ICAL_LANGUAGE_PARAMETER, language), "en_US");

#undef get_param
}

void test_attendees(void)
{
    icalcomponent *comp, *clone;
    icalproperty *prop;
    const char *str;

    comp = icalcomponent_new_vevent();
    prop = icalproperty_new(ICAL_ATTENDEE_PROPERTY);
    icalproperty_set_attendee(prop, "mailto:att1");
#define set_param(_suffix, _value)   \
    icalproperty_add_parameter(prop, \
                               icalparameter_new_##_suffix(_value))
    set_param(member, "member");
    set_param(cutype, ICAL_CUTYPE_INDIVIDUAL);
    set_param(role, ICAL_ROLE_CHAIR);
    set_param(partstat, ICAL_PARTSTAT_NEEDSACTION);
    set_param(rsvp, ICAL_RSVP_FALSE);
    set_param(delegatedfrom, "mailto:delgfrom");
    set_param(delegatedto, "mailto:delgto");
    set_param(sentby, "mailto:sentby");
    set_param(cn, "First attendee");
    set_param(language, "en_US");
#undef set_param

    icalcomponent_add_property(comp, prop);
    verify_comp_attendee(comp);

    str = icalcomponent_as_ical_string(comp);
    icalcomponent_free(comp);
    comp = icalcomponent_new_from_string(str);
    verify_comp_attendee(comp);

    clone = icalcomponent_clone(comp);
    verify_comp_attendee(clone);
    icalcomponent_free(comp);

    str = icalcomponent_as_ical_string(clone);
    comp = icalcomponent_new_from_string(str);
    verify_comp_attendee(comp);
    icalcomponent_free(clone);
    icalcomponent_free(comp);
}

static void verify_comp_relations(icalcomponent *comp)
{
    icalproperty *prop;

#define get_param(_kind, _suffix) icalparameter_get_##_suffix(icalproperty_get_first_parameter(prop, _kind))

    prop = icalcomponent_get_first_property(comp, ICAL_CONCEPT_PROPERTY);
    // serialized property SHOULD NOT have VALUE=URI (the default)
    str_is("concept", icalproperty_as_ical_string(prop),
           "CONCEPT:https://example.com/event-types/arts/music\r\n");

    prop = icalcomponent_get_first_property(comp, ICAL_LINK_PROPERTY);
    str_is("link-value", icalproperty_get_link(prop),
           "https://example.com/xmlDocs/bidFramework.xml#xpointer(descendant::CostStruc/range-to(following::CostStrucEND[1]))");
    str_is("link-linkrel", get_param(ICAL_LINKREL_PARAMETER, linkrel),
           "https://example.com/linkrel/costStructure");
    str_is("link-label", get_param(ICAL_LABEL_PARAMETER, label), "Bid");
    ok("link-value-type", get_param(ICAL_VALUE_PARAMETER, value) == ICAL_VALUE_XMLREFERENCE);

    prop = icalcomponent_get_first_property(comp, ICAL_RELATEDTO_PROPERTY);
    str_is("relto-1-value", icalproperty_get_relatedto(prop),
           "https://example.com/caldav/user/jb/cal/19960401-080045-4000F192713.ics");
    str_is("relto-1-gap",
           icaldurationtype_as_ical_string(get_param(ICAL_GAP_PARAMETER, gap)),
           "PT5M");
    ok("relto-1-value-type", get_param(ICAL_VALUE_PARAMETER, value) == ICAL_VALUE_URI);
    ok("relto-1-reltype", get_param(ICAL_RELTYPE_PARAMETER, reltype) == ICAL_RELTYPE_STARTTOFINISH);

    prop = icalcomponent_get_next_property(comp, ICAL_RELATEDTO_PROPERTY);
    str_is("relto-2-value", icalproperty_get_relatedto(prop), "foo;bar");
    ok("relto-2-value-type", get_param(ICAL_VALUE_PARAMETER, value) == ICAL_VALUE_UID);
    // serialized property value should be escaped
    str_is("relto-2", icalproperty_as_ical_string(prop),
           "RELATED-TO;VALUE=UID:foo\\;bar\r\n");

    prop = icalcomponent_get_next_property(comp, ICAL_RELATEDTO_PROPERTY);
    // serialized property SHOULD NOT have VALUE=TEXT (the default)
    str_is("relto-3", icalproperty_as_ical_string(prop),
           "RELATED-TO:19960401-080045-4000F192713-0052@example.com\r\n");

    prop = icalcomponent_get_first_property(comp, ICAL_REFID_PROPERTY);
    str_is("refid-value", icalproperty_get_refid(prop), "1,2");
    // serialized property SHOULD NOT have VALUE=TEXT (the default)
    str_is("refid", icalproperty_as_ical_string(prop), "REFID:1\\,2\r\n");

#undef get_param
}

void test_ical_relationships(void)
{
    icalcomponent *comp, *clone;
    const char *str;

    comp = icalcomponent_vanew(
        ICAL_VEVENT_COMPONENT,
        icalproperty_vanew_concept("https://example.com/event-types/arts/music",
                                   icalparameter_new_value(ICAL_VALUE_URI),
                                   (void *)0),
        icalproperty_vanew_link("https://example.com/xmlDocs/bidFramework.xml#xpointer(descendant::CostStruc/range-to(following::CostStrucEND[1]))",
                                icalparameter_new_value(ICAL_VALUE_XMLREFERENCE),
                                icalparameter_new_label("Bid"),
                                icalparameter_new_linkrel("https://example.com/linkrel/costStructure"),
                                (void *)0),
        icalproperty_vanew_relatedto("https://example.com/caldav/user/jb/cal/19960401-080045-4000F192713.ics",
                                     icalparameter_new_value(ICAL_VALUE_URI),
                                     icalparameter_new_reltype(ICAL_RELTYPE_STARTTOFINISH),
                                     icalparameter_new_gap(
                                         icaldurationtype_from_string("+PT05M")),
                                     (void *)0),
        icalproperty_vanew_relatedto("foo;bar",
                                     icalparameter_new_value(ICAL_VALUE_UID),
                                     (void *)0),
        icalproperty_vanew_relatedto("19960401-080045-4000F192713-0052@example.com",
                                     icalparameter_new_value(ICAL_VALUE_TEXT),
                                     (void *)0),
        icalproperty_vanew_refid("1,2",
                                 icalparameter_new_value(ICAL_VALUE_TEXT),
                                 (void *)0),
        (void *)0);

    verify_comp_relations(comp);

    str = icalcomponent_as_ical_string(comp);
    icalcomponent_free(comp);
    comp = icalcomponent_new_from_string(str);
    verify_comp_relations(comp);

    clone = icalcomponent_clone(comp);
    verify_comp_relations(clone);
    icalcomponent_free(comp);

    str = icalcomponent_as_ical_string(clone);
    comp = icalcomponent_new_from_string(str);
    verify_comp_relations(comp);
    icalcomponent_free(clone);
    icalcomponent_free(comp);
}

static void test_vtodo_partstat_inprocess(void)
{
    const char *str =
        "BEGIN:VCALENDAR\r\n"
        "PRODID:-//FOO//bar//EN\r\n"
        "VERSION:2.0\r\n"
        "BEGIN:VTODO\r\n"
        "ATTENDEE;PARTSTAT=IN-PROCESS:mailto:foo@example.com\r\n"
        "DTSTAMP:20060102T030405Z\r\n"
        "UID:4dba9882-e4a2-43e6-9944-b93e726fa6d3\r\n"
        "ORGANIZER:mailto:bar@example.com\r\n"
        "END:VTODO\r\n"
        "END:VCALENDAR\r\n";

    icalcomponent *ical = icalcomponent_new_from_string(str);
    ok("Parsed iCalendar object", (ical != NULL));

    icalcomponent *comp = icalcomponent_get_first_real_component(ical);
    ok("Parsed VTODO component", (icalcomponent_isa(comp) == ICAL_VTODO_COMPONENT));

    icalproperty *prop = icalcomponent_get_first_property(comp, ICAL_ATTENDEE_PROPERTY);
    ok("Parsed ATTENDEE property", (prop != NULL));

    icalparameter *param = icalproperty_get_first_parameter(prop, ICAL_PARTSTAT_PARAMETER);
    ok("Parsed PARTSTAT parameter", (param != NULL));

    icalparameter_partstat partstat = icalparameter_get_partstat(param);
    ok("Parsed IN-PROCESS PARTSTAT parameter value", (partstat == ICAL_PARTSTAT_INPROCESS));

    icalcomponent_free(ical);
}

static void test_icalpropiter(void)
{
    const char *str =
        "BEGIN:VEVENT\r\n"
        "DTSTAMP:20060102T030405Z\r\n"
        "COMMENT:comment1\r\n"
        "UID:4dba9882-e4a2-43e6-9944-b93e726fa6d3\r\n"
        "DTSTART;VALUE=DATE:20250120\r\n"
        "COMMENT:comment2\r\n"
        "END:VEVENT\r\n";

    icalcomponent *comp = icalcomponent_new_from_string(str);
    ok("Parsed VEVENT component", (comp != NULL));

    const icalproperty *dtstamp =
        icalcomponent_get_first_property(comp, ICAL_DTSTAMP_PROPERTY);
    assert(dtstamp != NULL);
    const icalproperty *dtstart =
        icalcomponent_get_first_property(comp, ICAL_DTSTART_PROPERTY);
    assert(dtstart != NULL);
    const icalproperty *comment1 =
        icalcomponent_get_first_property(comp, ICAL_COMMENT_PROPERTY);
    assert(comment1 != NULL);
    const icalproperty *comment2 =
        icalcomponent_get_next_property(comp, ICAL_COMMENT_PROPERTY);
    assert(comment2 != NULL);
    const icalproperty *uid =
        icalcomponent_get_first_property(comp, ICAL_UID_PROPERTY);
    assert(uid != NULL);

    // Iterate all properties.
    icalpropiter iter = icalcomponent_begin_property(comp, ICAL_ANY_PROPERTY);
    assert(icalpropiter_is_valid(&iter));
    ok("iter at DTSTAMP",
       dtstamp == icalpropiter_deref(&iter));
    ok("iter at COMMENT (comment1)",
       comment1 == icalpropiter_next(&iter) &&
           comment1 == icalpropiter_deref(&iter));
    ok("iter at UID",
       uid == icalpropiter_next(&iter) &&
           uid == icalpropiter_deref(&iter));
    ok("iter at DTSTART",
       dtstart == icalpropiter_next(&iter) &&
           dtstart == icalpropiter_deref(&iter));
    ok("iter at COMMENT (comment2)",
       comment2 == icalpropiter_next(&iter) &&
           comment2 == icalpropiter_deref(&iter));
    ok("iter at end",
       NULL == icalpropiter_next(&iter) &&
           NULL == icalpropiter_deref(&iter));

    // Iterate COMMENT property.
    iter = icalcomponent_begin_property(comp, ICAL_COMMENT_PROPERTY);
    assert(icalpropiter_is_valid(&iter));
    ok("iter at COMMENT (comment1)",
       comment1 == icalpropiter_deref(&iter));
    ok("iter at COMMENT (comment2)",
       comment2 == icalpropiter_next(&iter) &&
           comment2 == icalpropiter_deref(&iter));
    ok("iter at end",
       NULL == icalpropiter_next(&iter) &&
           NULL == icalpropiter_deref(&iter));

    // Iterate non-existent property.
    iter = icalcomponent_begin_property(comp, ICAL_DESCRIPTION_PROPERTY);
    assert(!icalpropiter_is_valid(&iter)); //will be invalid
    ok("iter at end", NULL == icalpropiter_deref(&iter));

    icalcomponent_free(comp);
}

static void test_icalparamiter(void)
{
    const char *str =
        "BEGIN:VEVENT\r\n"
        "DTSTAMP:20060102T030405Z\r\n"
        "UID:4dba9882-e4a2-43e6-9944-b93e726fa6d3\r\n"
        "DTSTART;VALUE=DATE:20250120\r\n"
        "ATTENDEE\r\n"
        " ;RSVP=TRUE\r\n"
        " ;PARTSTAT=TENTATIVE\r\n"
        " :mailto:foo@example.com\r\n"
        "END:VEVENT\r\n";

    icalcomponent *comp = icalcomponent_new_from_string(str);
    ok("Parsed VEVENT component", (comp != NULL));

    icalproperty *prop =
        icalcomponent_get_first_property(comp, ICAL_ATTENDEE_PROPERTY);
    ok("Parsed ATTENDEE property", (prop != NULL));

    const icalparameter *rsvp =
        icalproperty_get_first_parameter(prop, ICAL_RSVP_PARAMETER);
    assert(rsvp != NULL);
    const icalparameter *partstat =
        icalproperty_get_first_parameter(prop, ICAL_PARTSTAT_PARAMETER);
    assert(partstat != NULL);

    // Iterate all parameters.
    icalparamiter iter = icalproperty_begin_parameter(prop, ICAL_ANY_PARAMETER);
    ok("iter at RSVP",
       rsvp == icalparamiter_deref(&iter));
    ok("iter at PARTSTAT",
       partstat == icalparamiter_next(&iter) &&
           partstat == icalparamiter_deref(&iter));
    ok("iter at end",
       NULL == icalparamiter_next(&iter) &&
           NULL == icalparamiter_deref(&iter));

    // Iterate PARTSTAT parameter.
    iter = icalproperty_begin_parameter(prop, ICAL_PARTSTAT_PARAMETER);
    ok("iter at PARTSTAT",
       partstat == icalparamiter_deref(&iter));
    ok("iter at end",
       NULL == icalparamiter_next(&iter) &&
           NULL == icalparamiter_deref(&iter));

    // Iterate in-existent parameter.
    iter = icalproperty_begin_parameter(prop, ICAL_CN_PARAMETER);
    ok("iter at end", NULL == icalparamiter_deref(&iter));

    icalcomponent_free(comp);
}

static void test_icalproperty_enum_convert_string(void)
{
    icalproperty_action action = icalproperty_string_to_action("DISPLAY");
    ok("action", action == ICAL_ACTION_DISPLAY);
    str_is("action", icalproperty_action_to_string(action), "DISPLAY");

    icalproperty_transp transp = icalproperty_string_to_transp("OPAQUE");
    ok("transp", transp == ICAL_TRANSP_OPAQUE);
    str_is("transp", icalproperty_transp_to_string(transp), "OPAQUE");

    icalproperty_class class = icalproperty_string_to_class("PRIVATE");
    ok("class", class == ICAL_CLASS_PRIVATE);
    str_is("class", icalproperty_class_to_string(class), "PRIVATE");

    icalproperty_participanttype ptype =
        icalproperty_string_to_participanttype("CONTACT");
    ok("participanttype", ptype == ICAL_PARTICIPANTTYPE_CONTACT);
    str_is("participanttype",
           icalproperty_participanttype_to_string(ptype), "CONTACT");

    icalproperty_resourcetype rtype =
        icalproperty_string_to_resourcetype("PROJECTOR");
    ok("resourcetype", rtype == ICAL_RESOURCETYPE_PROJECTOR);
    str_is("resourcetype",
           icalproperty_resourcetype_to_string(rtype), "PROJECTOR");
}

static void test_icalparameter_parse_multivalued(void)
{
    char buffer[4096] = {0};
    strcat(buffer, "BEGIN:VEVENT\r\n");

    const char *display_param_str =
        "IMAGE;VALUE=URI;DISPLAY=X-FOO,BADGE:https://local/img2.png\r\n";
    strcat(buffer, display_param_str);

    const char *delegatedfrom_param_str =
        "ATTENDEE;DELEGATED-FROM=\"mailto:d1\",\"mailto:d2\":mailto:delgfrom\r\n";
    strcat(buffer, delegatedfrom_param_str);

    const char *delegatedto_param_str =
        "ATTENDEE;DELEGATED-TO=\"mailto:d1\",\"mailto:d2\":mailto:delgto\r\n";
    strcat(buffer, delegatedto_param_str);

    const char *member_param_str =
        "ATTENDEE;MEMBER=\"mailto:d1\",\"mailto:d2\":mailto:member\r\n";
    strcat(buffer, member_param_str);

    strcat(buffer, "END:VEVENT\r\n");

    icalcomponent *comp = icalcomponent_new_from_string(buffer);
    icalproperty *prop;

    prop = icalcomponent_get_first_property(comp, ICAL_ANY_PROPERTY);
    str_is("DISPLAY", icalproperty_as_ical_string(prop),
           display_param_str);

    prop = icalcomponent_get_next_property(comp, ICAL_ANY_PROPERTY);
    str_is("DELEGATED-FROM", icalproperty_as_ical_string(prop),
           delegatedfrom_param_str);

    prop = icalcomponent_get_next_property(comp, ICAL_ANY_PROPERTY);
    str_is("DELEGATED-TO", icalproperty_as_ical_string(prop),
           delegatedto_param_str);

    prop = icalcomponent_get_next_property(comp, ICAL_ANY_PROPERTY);
    str_is("MEMBER", icalproperty_as_ical_string(prop),
           member_param_str);

    icalcomponent_free(comp);
}

static void test_icalparameter_create_multivalued(void)
{
    icalparameter *param;

    // Test icalenumarray-valued parameter.

    // Generic constructor:
    param = icalparameter_new(ICAL_DISPLAY_PARAMETER);

    icalenumarray *display = icalenumarray_new(5);
    icalenumarray_element elem = {0};
    elem.val = ICAL_DISPLAY_BADGE;
    icalenumarray_append(display, &elem);
    elem.val = ICAL_DISPLAY_X;
    elem.xvalue = "X-FOO";
    icalenumarray_append(display, &elem);
    icalparameter_set_display(param, display);
    str_is("DISPLAY", icalparameter_as_ical_string(param),
           "DISPLAY=BADGE,X-FOO");
    icalparameter_free(param);

    // Generated "new_list" constructor:
    display = icalenumarray_new(5);
    elem.val = ICAL_DISPLAY_BADGE;
    elem.xvalue = NULL;
    icalenumarray_append(display, &elem);
    param = icalparameter_new_display_list(display);
    str_is("DISPLAY", icalparameter_as_ical_string(param), "DISPLAY=BADGE");
    icalparameter_free(param);

    // Generated "new" constructor:
    param = icalparameter_new_display(ICAL_DISPLAY_BADGE);
    str_is("DISPLAY", icalparameter_as_ical_string(param), "DISPLAY=BADGE");
    icalparameter_free(param);

    // Test icalstrarray-valued parameter.
    param = icalparameter_new(ICAL_MEMBER_PARAMETER);

    icalstrarray *member = icalstrarray_new(5);
    icalstrarray_append(member, "mailto:member1");
    icalstrarray_append(member, "mailto:member2");
    icalparameter_set_member(param, member);

    str_is("MEMBER", icalparameter_as_ical_string(param),
           "MEMBER=\"mailto:member1\",\"mailto:member2\"");

    icalparameter_free(param);

    // Generated "new_list" constructor:
    member = icalstrarray_new(5);
    icalstrarray_append(member, "mailto:member1");
    param = icalparameter_new_member_list(member);
    str_is("MEMBER", icalparameter_as_ical_string(param),
           "MEMBER=\"mailto:member1\"");
    icalparameter_free(param);

    // Generated "new" constructor:
    param = icalparameter_new_member("mailto:member1");
    str_is("MEMBER", icalparameter_as_ical_string(param),
           "MEMBER=\"mailto:member1\"");
    icalparameter_free(param);
}

static void test_icalstrarray(void)
{
    icalstrarray *array = icalstrarray_new(0);
    icalstrarray *clone;
    const char *val;
    size_t pos;

    ok("array: empty", 0 == icalstrarray_size(array));

    icalstrarray_append(array, "foo");
    ok("array: append foo", 1 == icalstrarray_size(array));

    val = icalstrarray_element_at(array, 0);
    ok("array[0] == foo", val && !strcmp(val, "foo"));

    icalstrarray_append(array, "bar");
    ok("array: append bar", 2 == icalstrarray_size(array));

    val = icalstrarray_element_at(array, 1);
    ok("array[1] == bar", val && !strcmp(val, "bar"));

    pos = icalstrarray_find(array, "bar");
    ok("array: find(bar) == 1", 1 == pos);

    icalstrarray_add(array, "bar");
    ok("array: add bar (omit duplicate)", 2 == icalstrarray_size(array));

    icalstrarray_append(array, "bar");
    ok("array: append bar (duplicate)", 3 == icalstrarray_size(array));

    pos = icalstrarray_find(array, "bar");
    ok("array: find(bar) == 1", 1 == pos);

    clone = icalstrarray_clone(array);
    ok("array: clone", 3 == icalstrarray_size(array));
    val = icalstrarray_element_at(clone, 0);
    ok("clone[0] == foo", val && !strcmp(val, "foo"));
    val = icalstrarray_element_at(clone, 1);
    ok("clone[1] == bar", val && !strcmp(val, "bar"));
    val = icalstrarray_element_at(clone, 2);
    ok("clone[2] == bar", val && !strcmp(val, "bar"));

    icalstrarray_remove_element_at(clone, 0);
    ok("clone: remove clone[0]", 2 == icalstrarray_size(clone));

    val = icalstrarray_element_at(clone, 0);
    ok("clone[0] == bar", val && !strcmp(val, "bar"));
    val = icalstrarray_element_at(clone, 1);
    ok("clone[1] == bar", val && !strcmp(val, "bar"));

    icalstrarray_remove(clone, "bar");
    ok("clone: remove bar", 0 == icalstrarray_size(clone));

    icalstrarray_sort(array);
    ok("array: sort", 3 == icalstrarray_size(array));
    val = icalstrarray_element_at(array, 0);
    ok("array[0] == bar", val && !strcmp(val, "bar"));
    val = icalstrarray_element_at(array, 1);
    ok("array[1] == bar", val && !strcmp(val, "bar"));
    val = icalstrarray_element_at(array, 2);
    ok("array[2] == foo", val && !strcmp(val, "foo"));

    icalstrarray_remove(array, "bar");
    ok("array: remove bar", 1 == icalstrarray_size(array));
    val = icalstrarray_element_at(array, 0);
    ok("array[0] == foo", val && !strcmp(val, "foo"));

    icalstrarray_free(array);
    icalstrarray_free(clone);

    // NULL array pointer safety
    ok("NULL: size == 0", 0 == icalstrarray_size(NULL));

    icalstrarray_append(NULL, "foo");
    icalstrarray_add(NULL, "foo");
    icalstrarray_remove_element_at(NULL, 0);
    icalstrarray_remove(NULL, 0);
    icalstrarray_sort(NULL);
    icalstrarray_free(NULL);

    val = icalstrarray_element_at(NULL, 0);
    ok("NULL: element_at(0) == NULL", val == NULL);

    pos = icalstrarray_find(NULL, "foo");
    ok("NULL: find foo == 0", pos == 0);

    clone = icalstrarray_clone(NULL);
    ok("NULL: clone == NULL", clone == NULL);

    // NULL or invalid argument safety
    array = icalstrarray_new(0);

    icalstrarray_append(array, NULL);
    ok("array: append NULL", 0 == icalstrarray_size(array));

    icalstrarray_add(array, NULL);
    ok("array: add NULL", 0 == icalstrarray_size(array));

    pos = icalstrarray_find(array, NULL);
    ok("array: find NULL == 0", pos == 0);

    icalstrarray_remove_element_at(array, 0);
    icalstrarray_remove(array, 0);

    icalstrarray_free(array);
}

static void test_icalenumarray(void)
{
    icalenumarray *array = icalenumarray_new(0);
    icalenumarray *clone;
    const icalenumarray_element *elem;
    size_t pos;

    icalenumarray_element foo = {42, "foo"};
    icalenumarray_element bar = {11, NULL};

    ok("array: empty", 0 == icalenumarray_size(array));

    icalenumarray_append(array, &foo);
    ok("array: append foo", 1 == icalenumarray_size(array));

    elem = icalenumarray_element_at(array, 0);
    ok("array[0] == foo", elem && elem->val == 42 && !strcmp(elem->xvalue, "foo"));

    icalenumarray_append(array, &bar);
    ok("array: append bar", 2 == icalenumarray_size(array));

    elem = icalenumarray_element_at(array, 1);
    ok("array[1] == bar", elem && elem->val == 11 && elem->xvalue == NULL);

    pos = icalenumarray_find(array, &bar);
    ok("array: find(bar) == 1", 1 == pos);

    icalenumarray_add(array, &bar);
    ok("array: add bar (omit duplicate)", 2 == icalenumarray_size(array));

    icalenumarray_append(array, &bar);
    ok("array: append bar (duplicate)", 3 == icalenumarray_size(array));

    pos = icalenumarray_find(array, &bar);
    ok("array: find(bar) == 1", 1 == pos);

    clone = icalenumarray_clone(array);
    ok("array: clone", 3 == icalenumarray_size(array));
    elem = icalenumarray_element_at(clone, 0);
    ok("clone[0] == foo", elem && elem->val == 42 && !strcmp(elem->xvalue, "foo"));
    elem = icalenumarray_element_at(clone, 1);
    ok("clone[1] == bar", elem && elem->val == 11 && elem->xvalue == NULL);
    elem = icalenumarray_element_at(clone, 2);
    ok("clone[2] == bar", elem && elem->val == 11 && elem->xvalue == NULL);

    icalenumarray_remove_element_at(clone, 0);
    ok("clone: remove clone[0]", 2 == icalenumarray_size(clone));

    elem = icalenumarray_element_at(clone, 0);
    ok("clone[0] == bar", elem && elem->val == 11 && elem->xvalue == NULL);
    elem = icalenumarray_element_at(clone, 1);
    ok("clone[1] == bar", elem && elem->val == 11 && elem->xvalue == NULL);

    icalenumarray_remove(clone, &bar);
    ok("clone: remove bar", 0 == icalenumarray_size(clone));

    icalenumarray_free(clone);

    icalenumarray_sort(array);
    ok("array: sort", 3 == icalenumarray_size(array));
    elem = icalenumarray_element_at(array, 0);
    ok("array[0] == bar", elem && elem->val == 11 && elem->xvalue == NULL);
    elem = icalenumarray_element_at(array, 1);
    ok("array[1] == bar", elem && elem->val == 11 && elem->xvalue == NULL);
    elem = icalenumarray_element_at(array, 2);
    ok("array[2] == foo", elem && elem->val == 42 && !strcmp(elem->xvalue, "foo"));

    icalenumarray_remove(array, &bar);
    ok("array: remove bar", 1 == icalenumarray_size(array));
    elem = icalenumarray_element_at(array, 0);
    ok("array[0] == foo", elem && elem->val == 42 && !strcmp(elem->xvalue, "foo"));

    icalenumarray_free(array);

    // NULL array pointer safety
    ok("NULL: size == 0", 0 == icalenumarray_size(NULL));

    icalenumarray_append(NULL, &foo);
    icalenumarray_add(NULL, &foo);
    icalenumarray_remove_element_at(NULL, 0);
    icalenumarray_remove(NULL, 0);
    icalenumarray_sort(NULL);
    icalenumarray_free(NULL);

    elem = icalenumarray_element_at(NULL, 0);
    ok("NULL: element_at(0) == NULL", elem == NULL);

    pos = icalenumarray_find(NULL, &foo);
    ok("NULL: find foo == 0", pos == 0);

    clone = icalenumarray_clone(NULL);
    ok("NULL: clone == NULL", clone == NULL);

    // NULL or invalid argument safety
    array = icalenumarray_new(0);

    icalenumarray_append(array, NULL);
    ok("array: append NULL", 0 == icalenumarray_size(array));

    icalenumarray_add(array, NULL);
    ok("array: add NULL", 0 == icalenumarray_size(array));

    pos = icalenumarray_find(array, NULL);
    ok("array: find NULL == 0", pos == 0);

    icalenumarray_remove_element_at(array, 0);
    icalenumarray_remove(array, 0);

    icalenumarray_free(array);
}

static void test_xcomponent_as_string(void)
{
    const char *str =
        "BEGIN:VCALENDAR\r\n"
        "BEGIN:X-FOO\r\n"
        "UID:5D0D3350\r\n"
        "END:X-FOO\r\n"
        "END:VCALENDAR\r\n";

    icalcomponent *ical = icalcomponent_new_from_string(str);

    ok("Parsed VCALENDAR",
       (ical != NULL) && icalcomponent_isa(ical) == ICAL_VCALENDAR_COMPONENT);

    str_is("Assert iCalendar", icalcomponent_as_ical_string(ical), str);

    icalcomponent_free(ical);
}

static void test_clone_xcomponent(void)
{
    const char *str =
        "BEGIN:VCALENDAR\r\n"
        "BEGIN:X-FOO\r\n"
        "UID:5D0D3350\r\n"
        "END:X-FOO\r\n"
        "END:VCALENDAR\r\n";

    icalcomponent *ical = icalcomponent_new_from_string(str);

    ok("Parsed VCALENDAR",
       (ical != NULL) && icalcomponent_isa(ical) == ICAL_VCALENDAR_COMPONENT);

    icalcomponent *clone = icalcomponent_clone(ical);

    ok("Cloned VCALENDAR",
       (clone != NULL) && icalcomponent_isa(clone) == ICAL_VCALENDAR_COMPONENT);

    str_is("Assert iCalendar", icalcomponent_as_ical_string(clone), str);

    icalcomponent_free(clone);
    icalcomponent_free(ical);
}

static void test_icaltime_proper_zone(void)
{
    icaltimetype first, second;
    icaltimezone *utc = icaltimezone_get_utc_timezone();
    icaltimezone *first_zone = icaltimezone_get_builtin_timezone("Europe/Brussels");
    icaltimezone *second_zone = icaltimezone_get_builtin_timezone("America/New_York");

    first = icaltime_current_time_with_zone(first_zone);
    ok("first::zone is not NULL", (icaltime_get_timezone(first) != NULL));
    ok("first::zone is not UTC", (icaltime_get_timezone(first) != utc));
    ok("first::zone preserves zone", (icaltime_get_timezone(first) == first_zone));

    second = icaltime_current_time_with_zone(second_zone);
    ok("second::zone is not NULL", (icaltime_get_timezone(second) != NULL));
    ok("second::zone is not UTC", (icaltime_get_timezone(second) != utc));
    ok("second::zone preserves zone", (icaltime_get_timezone(second) == second_zone));

    ok("first is before or same with the second", (icaltime_compare(first, second) <= 0));

    second = first;
    ok("first is the same as the second after assignment", (icaltime_compare(first, second) == 0));
    ok("second::zone is first zone", (icaltime_get_timezone(second) == first_zone));

    icaltimezone_convert_time(&first, first_zone, second_zone);
    ok("converted first::zone is second zone", (icaltime_get_timezone(first) == second_zone));
    ok("first is the same as the second after first's convert", (icaltime_compare(first, second) == 0));

    second = icaltime_convert_to_zone(second, utc);
    ok("converted second::zone is UTC", (icaltime_get_timezone(second) == utc));
    ok("first is the same as the second after second's convert", (icaltime_compare(first, second) == 0));
}

static void test_internal_limits(void)
{
    /* follow the _MAX* defaults at the top of icalllimits.c */
    int_is("max parse failures default", (int)icallimit_get(ICAL_LIMIT_PARSE_FAILURES), 1000);
    int_is("parse search default", (int)icallimit_get(ICAL_LIMIT_PARSE_SEARCH), 100000);
    int_is("max parse failure messages", (int)icallimit_get(ICAL_LIMIT_PARSE_FAILURE_ERROR_MESSAGES), 100);
    int_is("max props default", (int)icallimit_get(ICAL_LIMIT_PROPERTIES), 10000);
    int_is("max params default", (int)icallimit_get(ICAL_LIMIT_PARAMETERS), 100);
    int_is("max value chars", (int)icallimit_get(ICAL_LIMIT_VALUE_CHARS), 10485760);
    int_is("max prop values", (int)icallimit_get(ICAL_LIMIT_PROPERTY_VALUES), 500);
    int_is("max recurrence search", (int)icallimit_get(ICAL_LIMIT_RECURRENCE_SEARCH), 100000);
    int_is("time standing still", (int)icallimit_get(ICAL_LIMIT_RECURRENCE_TIME_STANDING_STILL), 50);
    int_is("max rrule search", (int)icallimit_get(ICAL_LIMIT_RRULE_SEARCH), 100);

    icallimit_set(ICAL_LIMIT_PROPERTIES, 10);
    int_is("properties limit", (int)icallimit_get(ICAL_LIMIT_PROPERTIES), 10);

    icallimit_set(ICAL_LIMIT_VALUE_CHARS, SIZE_MAX);
    ok("value chars is size_max", icallimit_get(ICAL_LIMIT_VALUE_CHARS) == SIZE_MAX);
}

void test_icaldurationtype_normalize(void)
{
#define assert_normalized_duration(input, want) \
    str_is("normalize(" input ")=" want, want,  \
           icaldurationtype_as_ical_string(     \
               icaldurationtype_normalize(      \
                   icaldurationtype_from_string(input))))

    assert_normalized_duration("PT0S", "PT0S");
    assert_normalized_duration("PT59S", "PT59S");
    assert_normalized_duration("PT60S", "PT1M");
    assert_normalized_duration("PT0M", "PT0S");
    assert_normalized_duration("PT59M", "PT59M");
    assert_normalized_duration("PT60M", "PT1H");
    assert_normalized_duration("PT86400S", "PT24H");
    assert_normalized_duration("PT86460S", "PT24H1M");
    assert_normalized_duration("PT86461S", "PT24H1M1S");
    assert_normalized_duration("P2W14D", "P4W");
    assert_normalized_duration("P21D", "P3W");
    assert_normalized_duration("P2W3DT86400S", "P17DT24H");
    assert_normalized_duration("P1WT1S", "P7DT1S");

#undef assert_normalized_duration
}

static void test_icalcomponent_remove_property_by_kind(void)

{
    const char *str =
        "BEGIN:VEVENT\r\n"
        "COMMENT:comment1\r\n"
        "DTSTAMP:20060102T030405Z\r\n"
        "COMMENT:comment2\r\n"
        "UID:4dba9882-e4a2-43e6-9944-b93e726fa6d3\r\n"
        "DTSTART;VALUE=DATE:20250120\r\n"
        "COMMENT:comment3\r\n"
        "END:VEVENT\r\n";

    icalcomponent *test_comp = icalcomponent_new_from_string(str);
    ok("Parsed VEVENT component", (test_comp != NULL));

    icalcomponent *comp;
    icalproperty *prop;

    // Assert removing existing property kind.
    comp = icalcomponent_clone(test_comp);
    icalcomponent_remove_property_by_kind(comp, ICAL_COMMENT_PROPERTY);
    prop = icalcomponent_get_first_property(comp, ICAL_ANY_PROPERTY);
    ok("DTSTAMP", icalproperty_isa(prop) == ICAL_DTSTAMP_PROPERTY);
    prop = icalcomponent_get_next_property(comp, ICAL_ANY_PROPERTY);
    ok("UID", icalproperty_isa(prop) == ICAL_UID_PROPERTY);
    prop = icalcomponent_get_next_property(comp, ICAL_ANY_PROPERTY);
    ok("DTSTART", icalproperty_isa(prop) == ICAL_DTSTART_PROPERTY);
    prop = icalcomponent_get_next_property(comp, ICAL_ANY_PROPERTY);
    ok("<end>", prop == NULL);
    icalcomponent_free(comp);

    // Assert removing non-existing property kind.
    comp = icalcomponent_clone(test_comp);
    icalcomponent_remove_property_by_kind(comp, ICAL_CLASS_PROPERTY);
    prop = icalcomponent_get_first_property(comp, ICAL_ANY_PROPERTY);
    ok("COMMENT", icalproperty_isa(prop) == ICAL_COMMENT_PROPERTY);
    prop = icalcomponent_get_next_property(comp, ICAL_ANY_PROPERTY);
    ok("DTSTAMP", icalproperty_isa(prop) == ICAL_DTSTAMP_PROPERTY);
    prop = icalcomponent_get_next_property(comp, ICAL_ANY_PROPERTY);
    ok("COMMENT", icalproperty_isa(prop) == ICAL_COMMENT_PROPERTY);
    prop = icalcomponent_get_next_property(comp, ICAL_ANY_PROPERTY);
    ok("UID", icalproperty_isa(prop) == ICAL_UID_PROPERTY);
    prop = icalcomponent_get_next_property(comp, ICAL_ANY_PROPERTY);
    ok("DTSTART", icalproperty_isa(prop) == ICAL_DTSTART_PROPERTY);
    prop = icalcomponent_get_next_property(comp, ICAL_ANY_PROPERTY);
    ok("COMMENT", icalproperty_isa(prop) == ICAL_COMMENT_PROPERTY);
    prop = icalcomponent_get_next_property(comp, ICAL_ANY_PROPERTY);
    ok("<end>", prop == NULL);
    icalcomponent_free(comp);

    // Assert removing any property kind.
    comp = icalcomponent_clone(test_comp);
    icalcomponent_remove_property_by_kind(comp, ICAL_ANY_PROPERTY);
    prop = icalcomponent_get_first_property(comp, ICAL_ANY_PROPERTY);
    ok("<end>", prop == NULL);
    icalcomponent_free(comp);

    icalcomponent_free(test_comp);
}

static void test_icalcomponent_get_duration(void)
{
#define assert_icalcomponent_get_duration(desc, want, ctlines)                           \
    {                                                                                    \
        const char *str =                                                                \
            "BEGIN:VCALENDAR\r\n"                                                        \
            "VERSION:2.0\r\n"                                                            \
            "PRODID:-//foo/bar//v1.0//EN\r\n"                                            \
            "BEGIN:VEVENT\r\n"                                                           \
            "UID:4dba9882-e4a2-43e6-9944-b93e726fa6d3\r\n"                               \
            "DTSTAMP:20060102T030405Z\r\n" ctlines                                       \
            "END:VEVENT\r\n"                                                             \
            "END:VCALENDAR\r\n";                                                         \
        icalcomponent *ical = icalcomponent_new_from_string(str);                        \
        ok("Parsed iCalendar object", (ical != NULL));                                   \
        icalcomponent *comp = icalcomponent_get_first_real_component(ical);              \
        ok("Parsed VEVENT component", icalcomponent_isa(comp) == ICAL_VEVENT_COMPONENT); \
        str_is(desc,                                                                     \
               icaldurationtype_as_ical_string(                                          \
                   icalcomponent_get_duration(comp)),                                    \
               want);                                                                    \
        icalcomponent_free(ical);                                                        \
    }

    assert_icalcomponent_get_duration(
        "nominal duration (days)", "P2D",
        "DTSTART;TZID=Europe/Vienna:20250101T010000\r\n"
        "DURATION:P2D\r\n");

    assert_icalcomponent_get_duration(
        "nominal duration (weeks)", "P2W",
        "DTSTART;TZID=Europe/Vienna:20250101T010000\r\n"
        "DURATION:P2W\r\n");

    assert_icalcomponent_get_duration(
        "nominal duration (weeks & days - non-standard)", "P1W2D",
        "DTSTART;TZID=Europe/Vienna:20250101T010000\r\n"
        "DURATION:P1W2D\r\n");

    assert_icalcomponent_get_duration(
        "mixed nominal and accurate", "P2DT3H2M",
        "DTSTART;TZID=Europe/Vienna:20250101T010000\r\n"
        "DURATION:P2DT3H2M\r\n");

    assert_icalcomponent_get_duration(
        "accurate duration - not normalized", "PT26H120S",
        "DTSTART;TZID=Europe/Vienna:20250101T010000\r\n"
        "DURATION:PT26H120S\r\n");

    assert_icalcomponent_get_duration(
        "DTEND - same timezones", "PT7200S",
        "DTSTART;TZID=Europe/Vienna:20250101T010000\r\n"
        "DTEND;TZID=Europe/Vienna:20250101T030000\r\n");

    assert_icalcomponent_get_duration(
        "DTEND - different timezones", "PT7200S",
        "DTSTART;TZID=Europe/Vienna:20250101T010000\r\n"
        "DTEND;TZID=Asia/Bangkok:20250101T090000\r\n");

    assert_icalcomponent_get_duration(
        "DTEND - more than one day", "PT93600S",
        "DTSTART;TZID=Europe/Vienna:20250101T010000\r\n"
        "DTEND;TZID=Europe/Vienna:20250102T030000\r\n");

    assert_icalcomponent_get_duration(
        "DTEND - across STD-to-DST gap", "PT82800S",
        "DTSTART;TZID=Europe/Vienna:20250329T230000\r\n"
        "DTEND;TZID=Europe/Vienna:20250330T230000\r\n");

    assert_icalcomponent_get_duration(
        "DTEND - across DST-to-STD", "PT90000S",
        "DTSTART;TZID=Europe/Vienna:20251025T230000\r\n"
        "DTEND;TZID=Europe/Vienna:20251026T230000\r\n");

    // XXX - this incorrectly uses the standard time occurrence of
    // 2025-10-26T02:00:00, it should use the daylight offset.
    // (see RFC 5545, Section 3.3.5).
    // This requires the is_daylight field be set in icaltimetype.
    assert_icalcomponent_get_duration(
        "DTEND - same timezones, DST-to-STD shift (should be PT3H)", "PT14400S",
        "DTSTART;TZID=Europe/Vienna:20251025T230000\r\n"
        "DTEND;TZID=Europe/Vienna:20251026T020000\r\n");

#undef assert_icalcomponent_get_duration
}

int main(int argc, const char *argv[])
{
#if !defined(HAVE_UNISTD_H)
    extern char *optarg;
    extern int optopt;
#endif
#if defined(HAVE_GETOPT)
    int c;
#endif
    int do_test = 0;
    int do_header = 0;
    int failed_count = 0;

#if !defined(MEMORY_CONSISTENCY)
    // With MEMORY_CONSISTENCY we are building the entire library using the
    // test_* functions; therefore, no need to set them here again.

    // We specify special versions of malloc et al. that perform some extra verifications.
    // Most notably they ensure, that memory allocated with icalmemory_new_buffer() is freed
    // using icalmemory_free() rather than using free() directly and vice versa. Failing to
    // do so would cause the test to fail with assertions or access violations.
    icalmemory_set_mem_alloc_funcs(&test_malloc, &test_realloc, &test_free);
#endif

    icaltimezone_set_zone_directory(TEST_ZONEDIR);
    icaltimezone_set_tzid_prefix(TESTS_TZID_PREFIX);

    test_start(0);

#if defined(HAVE_GETOPT)
    while ((c = getopt(argc, (char **)argv, "lvq")) != -1) {
        switch (c) {
        case 'v': {
            VERBOSE = 1;
            break;
        }
        case 'q': {
            QUIET = 1;
            break;
        }
        case 'l': {
            do_header = 1;
            break;
        }
        default: { /* '?' */
            fprintf(stderr, "Usage: %s [-v|-q|-l]\n", strrchr(argv[0], '/'));
            exit(1);
        }
        }
    }

    if (optind < argc) {
        do_test = atoi(argv[argc - 1]);
    }
#else
    if (argc > 1) {
        do_test = atoi(argv[1]);
    }

#endif

    test_run("Test time parser functions", test_time_parser, do_test, do_header);
    test_run("Test icaltime_as_timet", test_icaltime_as_timet, do_test, do_header);
    test_run("Test time", test_time, do_test, do_header);
    test_run("Test calculation of DOY and WD", test_juldat_caldat, do_test, do_header);
    test_run("Test day of Year", test_doy, do_test, do_header);
    test_run("Test duration", test_duration, do_test, do_header);
    test_run("Test period", test_period, do_test, do_header);
    test_run("Test DTSTART", test_dtstart, do_test, do_header);
    test_run("Test day of year of week start", test_start_of_week, do_test, do_header);
    test_run("Test recur parser", test_recur_parser, do_test, do_header);
    test_run("Test recur", test_recur, do_test, do_header);
    test_run("Test recur encode by[ICAL_BY_DAY]", test_recur_encode_by_day, do_test, do_header);
    test_run("Test recur encode by[ICAL_BY_MONTH]", test_recur_encode_by_month, do_test, do_header);
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
    test_run("Test Empty Parameter", test_empty_parameter, do_test, do_header);
    test_run("Test Empty property", test_empty_property, do_test, do_header);
    test_run("Test X property", test_x_property, do_test, do_header);
    test_run("Test X parameter", test_x_parameter, do_test, do_header);
    test_run("Test request status", test_requeststat, do_test, do_header);
    test_run("Test UTC-OFFSET", test_utcoffset, do_test, do_header);
    test_run("Test Values", test_values, do_test, do_header);
    test_run("Test Parameters", test_parameters, do_test, do_header);
    test_run("Test Properties", test_properties, do_test, do_header);
    test_run("Test Components", test_components, do_test, do_header);
    test_run("Test icalcomponent_foreach_recurrence", test_component_foreach, do_test, do_header);
    test_run("Test icalcomponent_foreach_recurrence with start as date", test_component_foreach_start_as_date, do_test, do_header);
    test_run("Test icalcomponent_foreach_recurrence with nominal duration", test_component_foreach_dtend_nominal, do_test, do_header);
    test_run("Test icalcomponent_foreach_recurrence with exact duration", test_component_foreach_dtend_exact, do_test, do_header);
    test_run("Test icalrecur_iterator_set_start with date", test_recur_iterator_set_start, do_test, do_header);
    test_run("Test weekly icalrecur_iterator on January 1", test_recur_iterator_on_jan_1, do_test, do_header);
    test_run("Test Convenience", test_convenience, do_test, do_header);
    test_run("Test classify ", test_classify, do_test, do_header);
    test_run("Test Iterators", test_iterators, do_test, do_header);
    test_run("Test strings", test_strings, do_test, do_header);
    test_run("Test TZID escaping", test_tzid_escape, do_test, do_header);
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
    test_run("Test icaltime_compare UTC and zone handling", test_icaltime_compare_utc_zone, do_test, do_header);
    test_run("Test exclusion of recurrences as per r961", test_recurrenceexcluded, do_test,
             do_header);
#if defined(USE_BUILTIN_TZDATA)
    // this test is setup for testing against our builtin tzdata
    test_run("Test bad dtstart in timezone as per r960", test_bad_dtstart_in_timezone, do_test,
             do_header);
#endif
    test_run("Test icalcomponent_new_from_string()", test_icalcomponent_new_from_string, do_test,
             do_header);
    test_run("Test comma in quoted value of x property", test_comma_in_quoted_value, do_test,
             do_header);
    test_run("Test setting/unsetting zoneinfo dir", test_zoneinfo_stuff, do_test, do_header);
    test_run("Test TZID with UTC time", test_tzid_with_utc_time, do_test, do_header);
    test_run("Test RECURRENCE-ID TZID", test_recur_tzid, do_test, do_header);
    test_run("Test kind_to_string", test_kind_to_string, do_test, do_header);
    test_run("Test string_to_kind", test_string_to_kind, do_test, do_header);
    test_run("Test set DATE/DATE-TIME VALUE", test_set_date_datetime_value, do_test, do_header);
    test_run("Test timezone from builtin", test_timezone_from_builtin, do_test, do_header);
    test_run("Test icalvalue_decode_ical_string", test_icalvalue_decode_ical_string, do_test, do_header);

    test_run("Test icalarray_sort", test_icalarray_sort, do_test, do_header);

    test_run("Test icalcomponent_normalize", test_icalcomponent_normalize, do_test, do_header);
    test_run("Test icalcomponent_normalize_missing_mandatory_props", test_icalcomponent_normalize_missing_mandatory_props, do_test, do_header);
    test_run("Test builtin compat TZID", test_builtin_compat_tzid, do_test, do_header);
    test_run("Test VCC vCard parse", test_vcc_vcard_parse, do_test, do_header);
    test_run("Test implicit DTEND and DURATION for VEVENT and VTODO", test_implicit_dtend_duration, do_test, do_header);
    test_run("Test DTEND with exact and nominal DURATION", test_icalcomponent_get_dtend_from_duration, do_test, do_header);
    test_run("Test icalvalue resets timezone on set", test_icalvalue_resets_timezone_on_set, do_test, do_header);
    test_run("Test removing TZID from DUE with icalcomponent_set_due", test_remove_tzid_from_due, do_test, do_header);
    test_run("Test geo precision", test_geo_props, do_test, do_header);
    test_run("Test commas in x-property", test_comma_in_xproperty, do_test, do_header);
    test_run("Test icalcomponent_vanew with lastmodified property", test_icalcomponent_with_lastmodified, do_test, do_header);
    test_run("Test attendees", test_attendees, do_test, do_header);
    test_run("Test iCalendar Relationships", test_ical_relationships, do_test, do_header);
    test_run("Test IN-PROCESS PARTSTAT parameter value", test_vtodo_partstat_inprocess, do_test, do_header);
    test_run("Test external property iterator", test_icalpropiter, do_test, do_header);
    test_run("Test external parameter iterator", test_icalparamiter, do_test, do_header);
    test_run("Test property enum value string conversion", test_icalproperty_enum_convert_string, do_test, do_header);
    test_run("Test parsing multi-valued parameters", test_icalparameter_parse_multivalued, do_test, do_header);
    test_run("Test creating multi-valued parameters", test_icalparameter_create_multivalued, do_test, do_header);
    test_run("Test string arrays", test_icalstrarray, do_test, do_header);
    test_run("Test enum arrays", test_icalenumarray, do_test, do_header);
    test_run("Test serializing x-component", test_xcomponent_as_string, do_test, do_header);
    test_run("Test cloning x-component", test_clone_xcomponent, do_test, do_header);
    test_run("Test manipulating tzid", test_tzid_setter, do_test, do_header);
    test_run("Test icaltime proper zone set", test_icaltime_proper_zone, do_test, do_header);
    test_run("Test property values from string", test_value_from_string, do_test, do_header);
    test_run("Test normalizing time", test_icaltime_normalize, do_test, do_header);
    test_run("Test setting/getting internal limits", test_internal_limits, do_test, do_header);
    test_run("Test normalizing duration", test_icaldurationtype_normalize, do_test, do_header);
    test_run("Test removing component properties by kind", test_icalcomponent_remove_property_by_kind, do_test, do_header);
    test_run("Test icalcomponent_get_duration", test_icalcomponent_get_duration, do_test, do_header);
    /** OPTIONAL TESTS go here... **/

#if defined(LIBICAL_CXX_BINDINGS)
    test_run("Test C++ API", test_cxx, do_test, do_header);
#endif

#if defined(HAVE_BDB)
    test_run("Test BDB Set", test_bdbset, do_test, do_header);
#endif

    icaltimezone_free_builtin_timezones();
    icaltimezone_free_zone_directory();
    icalmemory_free_ring();

    failed_count = test_end();

    /* return a non-zero exit code if a test failed */
    return failed_count > 0;
}

//NOLINTEND(clang-analyzer-security.insecureAPI.strcpy)

/* make sure to keep tabs in our test strings */
/* Local Variables:    */
/* mode: c             */
/* indent-tabs-mode: t */
/* End:                */
