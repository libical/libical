/*======================================================================
 FILE: icalparser_ctrl_test.c
 CREATOR: Robert Stepanek, 10 October 2023

 SPDX-FileCopyrightText: 2000 Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
======================================================================*/

/*
 * Program to test handling of CONTROL characters in iCalendar parsing.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(NDEBUG)
#undef NDEBUG
#endif

#include "libical/ical.h"

static void assert_ctrl(enum icalparser_ctrl ctrl,
                        const char *data,
                        const char *want_desc,
                        int want_xlicerror)
{
    icalparser_set_ctrl(ctrl);

    icalcomponent *ical = icalparser_parse_string(data);
    assert(ical);

    icalcomponent *comp = icalcomponent_get_first_real_component(ical);
    assert(comp);

    if (want_desc) {
        assert(!strcmp(want_desc, icalcomponent_get_description(comp)));
    } else {
        assert(!icalcomponent_get_first_property(comp, ICAL_DESCRIPTION_PROPERTY));
    }

    if (want_xlicerror) {
        assert(icalcomponent_get_first_property(comp, ICAL_XLICERROR_PROPERTY));
    } else {
        assert(!icalcomponent_get_first_property(comp, ICAL_XLICERROR_PROPERTY));
    }

    icalcomponent_free(ical);
}

/* cppcheck-suppress constParameter */
int main(int argc, char *argv[])
{
    if (argc != 1) {
        fprintf(stderr, "Usage: %s\n", argv[0]);
        return 0;
    }

    // Assert default value
    assert(icalparser_get_ctrl() == ICALPARSER_CTRL_KEEP);

    // Assert icalparser_ctrl settings
    static const char *data = ""
                              "BEGIN:VCALENDAR\r\n"
                              "VERSION:2.0\r\n"
                              "PRODID:-//ACME/DesktopCalendar//E\r\n"
                              "CALSCALE:GREGORIAN\r\n"
                              "BEGIN:VEVENT\r\n"
                              "DTSTART:20160929T010000Z\r\n"
                              "DURATION:PT1H\r\n"
                              "UID:40d6fe3c-6a51-489e-823e-3ea22f427a3e\r\n"
                              "CREATED:20150928T125212Z\r\n"
                              "LAST-MODIFIED:20150928T132434Z\r\n"
                              "SUMMARY:test\r\n"
                              "DESCRIPTION:ct\x15rl\r\n" // this contains a CTRL char
                              "END:VEVENT\r\n"
                              "END:VCALENDAR\r\n";

    assert_ctrl(ICALPARSER_CTRL_KEEP, data, "ct\x15rl", 0);
    assert_ctrl(ICALPARSER_CTRL_OMIT, data, "ctrl", 0);
    assert_ctrl(ICALPARSER_CTRL_ERROR, data, NULL, 1);

    return 0;
}
