#!/usr/bin/env python3
#GI_TYPELIB_PATH=$PREFIX/lib/girepository-1.0/ ./comprehensive.py

###############################################################################
#
# Copyright (C) 2015 William Yu <williamyu@gnome.org>
#
# This library is free software; you can redistribute it and/or modify
# it under the terms of either:
#
#   The LGPL as published by the Free Software Foundation, version
#   2.1, available at: https://www.gnu.org/licenses/lgpl-2.1.txt
#
# Or:
#
#   The Mozilla Public License Version 2.0. You may obtain a copy of
#   the License at https://www.mozilla.org/MPL/
#
###############################################################################

import gi

gi.require_version('ICalGLib', '3.0')

from gi.repository import ICalGLib

event_str1 = \
    "BEGIN:VEVENT\n"                                \
    "UID:event-uid-123\n"                           \
    "SUMMARY;LANGUAGE=en-US:parent\n"               \
    "DTSTART;TZID=Europe/Prague:20140306T090000\n"  \
    "DTEND;TZID=Europe/Berlin:20140306T093000\n"    \
    "CLASS:PUBLIC\n"                                \
    "PRIORITY:5\n"                                  \
    "DTSTAMP:20140207T020756Z\n"                    \
    "TRANSP:OPAQUE\n"                               \
    "STATUS:CONFIRMED\n"                            \
    "SEQUENCE:0\n"                                  \
    "LOCATION;LANGUAGE=en-US:Location\n"            \
    "END:VEVENT\n"

event_str2 = \
    "BEGIN:VEVENT\n"                                \
    "UID:event-uid-123\n"                           \
    "SUMMARY;LANGUAGE=en-US:childEvent1\n"          \
    "DTSTART;TZID=Europe/Prague:20140306T090000\n"  \
    "DTEND;TZID=Europe/Berlin:20140306T093000\n"    \
    "CLASS:PUBLIC\n"                                \
    "PRIORITY:5\n"                                  \
    "DTSTAMP:20140207T020756Z\n"                    \
    "TRANSP:OPAQUE\n"                               \
    "STATUS:CONFIRMED\n"                            \
    "SEQUENCE:0\n"                                  \
    "LOCATION;LANGUAGE=en-US:Location\n"            \
    "END:VEVENT\n"

event_str3 = \
    "BEGIN:VEVENT\n"                                \
    "UID:event-uid-123\n"                           \
    "SUMMARY;LANGUAGE=en-US:childEvent2\n"          \
    "DTSTART;TZID=Europe/Prague:20140306T090000\n"  \
    "DTEND;TZID=Europe/Berlin:20140306T093000\n"    \
    "CLASS:PUBLIC\n"                                \
    "PRIORITY:5\n"                                  \
    "DTSTAMP:20140207T020756Z\n"                    \
    "TRANSP:OPAQUE\n"                               \
    "STATUS:CONFIRMED\n"                            \
    "SEQUENCE:0\n"                                  \
    "LOCATION;LANGUAGE=en-US:Location\n"            \
    "END:VEVENT\n"

event_str4 = \
    "BEGIN:VEVENT\n"                                \
    "UID:event-uid-123\n"                           \
    "SUMMARY;LANGUAGE=en-US:childEvent3\n"          \
    "DTSTART;TZID=Europe/Prague:20140306T090000\n"  \
    "DTEND;TZID=Europe/Berlin:20140306T093000\n"    \
    "CLASS:PUBLIC\n"                                \
    "PRIORITY:5\n"                                  \
    "DTSTAMP:20140207T020756Z\n"                    \
    "TRANSP:OPAQUE\n"                               \
    "STATUS:CONFIRMED\n"                            \
    "SEQUENCE:0\n"                                  \
    "LOCATION;LANGUAGE=en-US:Location\n"            \
    "END:VCALENDAR\n"

event_str5 = \
    "BEGIN:VCALENDAR\n"                             \
    "UID:event-uid-123\n"                           \
    "SUMMARY;LANGUAGE=en-US:childCalendar1\n"       \
    "DTSTART;TZID=Europe/Prague:20140306T090000\n"  \
    "DTEND;TZID=Europe/Berlin:20140306T093000\n"    \
    "CLASS:PUBLIC\n"                                \
    "PRIORITY:5\n"                                  \
    "DTSTAMP:20140207T020756Z\n"                    \
    "TRANSP:OPAQUE\n"                               \
    "STATUS:CONFIRMED\n"                            \
    "SEQUENCE:0\n"                                  \
    "LOCATION;LANGUAGE=en-US:Location\n"            \
    "END:VCALENDAR\n"

combined_string = \
    "BEGIN:VEVENT\r\n"                              \
    "UID:event-uid-123\r\n"                         \
    "SUMMARY;LANGUAGE=en-US:parent\r\n"             \
    "DTSTART;TZID=Europe/Prague:20140306T090000\r\n"\
    "DTEND;TZID=Europe/Berlin:20140306T093000\r\n"  \
    "CLASS:PUBLIC\r\n"                              \
    "PRIORITY:5\r\n"                                \
    "DTSTAMP:20140207T020756Z\r\n"                  \
    "TRANSP:OPAQUE\r\n"                             \
    "STATUS:CONFIRMED\r\n"                          \
    "SEQUENCE:0\r\n"                                \
    "LOCATION;LANGUAGE=en-US:Location\r\n"          \
    "BEGIN:VEVENT\r\n"                              \
    "UID:event-uid-123\r\n"                         \
    "SUMMARY;LANGUAGE=en-US:childEvent1\r\n"        \
    "DTSTART;TZID=Europe/Prague:20140306T090000\r\n" \
    "DTEND;TZID=Europe/Berlin:20140306T093000\r\n"  \
    "CLASS:PUBLIC\r\n"                              \
    "PRIORITY:5\r\n"                                \
    "DTSTAMP:20140207T020756Z\r\n"                  \
    "TRANSP:OPAQUE\r\n"                             \
    "STATUS:CONFIRMED\r\n"                          \
    "SEQUENCE:0\r\n"                                \
    "LOCATION;LANGUAGE=en-US:Location\r\n"          \
    "END:VEVENT\r\n"                                \
    "BEGIN:VEVENT\r\n"                              \
    "UID:event-uid-123\r\n"                         \
    "SUMMARY;LANGUAGE=en-US:childEvent2\r\n"        \
    "DTSTART;TZID=Europe/Prague:20140306T090000\r\n" \
    "DTEND;TZID=Europe/Berlin:20140306T093000\r\n"  \
    "CLASS:PUBLIC\r\n"                              \
    "PRIORITY:5\r\n"                                \
    "DTSTAMP:20140207T020756Z\r\n"                  \
    "TRANSP:OPAQUE\r\n"                             \
    "STATUS:CONFIRMED\r\n"                          \
    "SEQUENCE:0\r\n"                                \
    "LOCATION;LANGUAGE=en-US:Location\r\n"          \
    "END:VEVENT\r\n"                                \
    "BEGIN:VEVENT\r\n"                              \
    "UID:event-uid-123\r\n"                         \
    "SUMMARY;LANGUAGE=en-US:childEvent3\r\n"        \
    "DTSTART;TZID=Europe/Prague:20140306T090000\r\n" \
    "DTEND;TZID=Europe/Berlin:20140306T093000\r\n"  \
    "CLASS:PUBLIC\r\n"                              \
    "PRIORITY:5\r\n"                                \
    "DTSTAMP:20140207T020756Z\r\n"                  \
    "TRANSP:OPAQUE\r\n"                             \
    "STATUS:CONFIRMED\r\n"                          \
    "SEQUENCE:0\r\n"                                \
    "LOCATION;LANGUAGE=en-US:Location\r\n"          \
    "END:VEVENT\r\n"                                \
    "BEGIN:VCALENDAR\r\n"                           \
    "UID:event-uid-123\r\n"                         \
    "SUMMARY;LANGUAGE=en-US:childCalendar1\r\n"     \
    "DTSTART;TZID=Europe/Prague:20140306T090000\r\n" \
    "DTEND;TZID=Europe/Berlin:20140306T093000\r\n"  \
    "CLASS:PUBLIC\r\n"                              \
    "PRIORITY:5\r\n"                                \
    "DTSTAMP:20140207T020756Z\r\n"                  \
    "TRANSP:OPAQUE\r\n"                             \
    "STATUS:CONFIRMED\r\n"                          \
    "SEQUENCE:0\r\n"                                \
    "LOCATION;LANGUAGE=en-US:Location\r\n"          \
    "END:VCALENDAR\r\n"                             \
    "END:VEVENT\r\n"

def main():
    #Test child component manipulation
    parent = ICalGLib.Component.new_from_string(event_str1);
    comp1 = ICalGLib.Component.new_from_string(event_str2);
    comp2 = ICalGLib.Component.new_from_string(event_str3);
    comp3 = ICalGLib.Component.new_from_string(event_str4);
    comp4 = ICalGLib.Component.new_from_string(event_str5);

    parent.add_component(comp1);
    parent.add_component(comp2);
    parent.add_component(comp3);
    parent.add_component(comp4);

    assert parent.as_ical_string() == combined_string;

    count = parent.count_components(ICalGLib.ComponentKind.VEVENT_COMPONENT);
    child_component = parent.get_first_component(ICalGLib.ComponentKind.VEVENT_COMPONENT);
    for i in range(0, count):
        if (child_component.get_summary() == "childEvent2"):
            child_component.set_summary("childEventTwo");

            start = ICalGLib.Time.new_from_string("20141115T211923");
            end = ICalGLib.Time.new_from_string("20141115T221923");
            child_component.set_dtstart(start);
            child_component.set_dtend(end);

            child_component.set_dtstamp(start);
            child_component.set_location("East Lansing, MI, US");

            child_component.set_relcalid("relcalid for childEventTwo");
            recur_string = "RRULE:FREQ=DAILY;INTERVAL=10;COUNT=5";
            property = ICalGLib.Property.new_from_string(recur_string);
            child_component.add_property(property);
        if (i != count-1):
            child_component = parent.get_next_component(ICalGLib.ComponentKind.VEVENT_COMPONENT);

    modifiedCombinedString = parent.as_ical_string();
    newParent = ICalGLib.Component.new_from_string(modifiedCombinedString);

    count = parent.count_components(ICalGLib.ComponentKind.VEVENT_COMPONENT);
    child_component = parent.get_first_component(ICalGLib.ComponentKind.VEVENT_COMPONENT);
    for i in range(0, count):
        if (child_component.get_summary() == "childEventTwo"):
            child_component.set_summary("childEventTwo");

            dtstart = child_component.get_dtstart();
            start_string = ICalGLib.Time.as_ical_string(dtstart);
            assert(start_string == "20141115T211923");
            dtend = child_component.get_dtend();
            end_string = ICalGLib.Time.as_ical_string(dtend);
            assert(end_string == "20141115T221923");

            timestamp = child_component.get_dtstamp();
            assert(ICalGLib.Time.as_ical_string(timestamp) == "20141115T211923");
            assert(child_component.get_location() == "East Lansing, MI, US");
            assert(child_component.get_relcalid() == "relcalid for childEventTwo");

            recurProperty = child_component.get_first_property(ICalGLib.PropertyKind.RRULE_PROPERTY);
            assert recurProperty.as_ical_string() == "RRULE:FREQ=DAILY;COUNT=5;INTERVAL=10\r\n";
        if (i != count-1):
            child_component = parent.get_next_component(ICalGLib.ComponentKind.VEVENT_COMPONENT);

if __name__ == "__main__":
    main()
