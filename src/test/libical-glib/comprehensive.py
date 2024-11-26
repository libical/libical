#!/usr/bin/env python3
# GI_TYPELIB_PATH=$PREFIX/lib/girepository-1.0/ ./comprehensive.py

###############################################################################
#
# SPDX-FileCopyrightText: 2015 William Yu <williamyu@gnome.org>
#
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
#
###############################################################################

"""Comprehensive testing for Python libical bindings"""

import gi

gi.require_version('ICalGLib', '3.0')
from gi.repository import ICalGLib  # noqa E402

eventStr1 = (
    'BEGIN:VEVENT\n'
    'UID:event-uid-123\n'
    'SUMMARY;LANGUAGE=en-US:parent\n'
    'DTSTART;TZID=Europe/Prague:20140306T090000\n'
    'DTEND;TZID=Europe/Berlin:20140306T093000\n'
    'CLASS:PUBLIC\n'
    'PRIORITY:5\n'
    'DTSTAMP:20140207T020756Z\n'
    'TRANSP:OPAQUE\n'
    'STATUS:CONFIRMED\n'
    'SEQUENCE:0\n'
    'LOCATION;LANGUAGE=en-US:Location\n'
    'END:VEVENT\n'
)

eventStr2 = (
    'BEGIN:VEVENT\n'
    'UID:event-uid-123\n'
    'SUMMARY;LANGUAGE=en-US:childEvent1\n'
    'DTSTART;TZID=Europe/Prague:20140306T090000\n'
    'DTEND;TZID=Europe/Berlin:20140306T093000\n'
    'CLASS:PUBLIC\n'
    'PRIORITY:5\n'
    'DTSTAMP:20140207T020756Z\n'
    'TRANSP:OPAQUE\n'
    'STATUS:CONFIRMED\n'
    'SEQUENCE:0\n'
    'LOCATION;LANGUAGE=en-US:Location\n'
    'END:VEVENT\n'
)

eventStr3 = (
    'BEGIN:VEVENT\n'
    'UID:event-uid-123\n'
    'SUMMARY;LANGUAGE=en-US:childEvent2\n'
    'DTSTART;TZID=Europe/Prague:20140306T090000\n'
    'DTEND;TZID=Europe/Berlin:20140306T093000\n'
    'CLASS:PUBLIC\n'
    'PRIORITY:5\n'
    'DTSTAMP:20140207T020756Z\n'
    'TRANSP:OPAQUE\n'
    'STATUS:CONFIRMED\n'
    'SEQUENCE:0\n'
    'LOCATION;LANGUAGE=en-US:Location\n'
    'END:VEVENT\n'
)

eventStr4 = (
    'BEGIN:VEVENT\n'
    'UID:event-uid-123\n'
    'SUMMARY;LANGUAGE=en-US:childEvent3\n'
    'DTSTART;TZID=Europe/Prague:20140306T090000\n'
    'DTEND;TZID=Europe/Berlin:20140306T093000\n'
    'CLASS:PUBLIC\n'
    'PRIORITY:5\n'
    'DTSTAMP:20140207T020756Z\n'
    'TRANSP:OPAQUE\n'
    'STATUS:CONFIRMED\n'
    'SEQUENCE:0\n'
    'LOCATION;LANGUAGE=en-US:Location\n'
    'END:VCALENDAR\n'
)

eventStr5 = (
    'BEGIN:VCALENDAR\n'
    'UID:event-uid-123\n'
    'SUMMARY;LANGUAGE=en-US:childCalendar1\n'
    'DTSTART;TZID=Europe/Prague:20140306T090000\n'
    'DTEND;TZID=Europe/Berlin:20140306T093000\n'
    'CLASS:PUBLIC\n'
    'PRIORITY:5\n'
    'DTSTAMP:20140207T020756Z\n'
    'TRANSP:OPAQUE\n'
    'STATUS:CONFIRMED\n'
    'SEQUENCE:0\n'
    'LOCATION;LANGUAGE=en-US:Location\n'
    'END:VCALENDAR\n'
)

combinedString = (
    'BEGIN:VEVENT\r\n'
    'UID:event-uid-123\r\n'
    'SUMMARY;LANGUAGE=en-US:parent\r\n'
    'DTSTART;TZID=Europe/Prague:20140306T090000\r\n'
    'DTEND;TZID=Europe/Berlin:20140306T093000\r\n'
    'CLASS:PUBLIC\r\n'
    'PRIORITY:5\r\n'
    'DTSTAMP:20140207T020756Z\r\n'
    'TRANSP:OPAQUE\r\n'
    'STATUS:CONFIRMED\r\n'
    'SEQUENCE:0\r\n'
    'LOCATION;LANGUAGE=en-US:Location\r\n'
    'BEGIN:VEVENT\r\n'
    'UID:event-uid-123\r\n'
    'SUMMARY;LANGUAGE=en-US:childEvent1\r\n'
    'DTSTART;TZID=Europe/Prague:20140306T090000\r\n'
    'DTEND;TZID=Europe/Berlin:20140306T093000\r\n'
    'CLASS:PUBLIC\r\n'
    'PRIORITY:5\r\n'
    'DTSTAMP:20140207T020756Z\r\n'
    'TRANSP:OPAQUE\r\n'
    'STATUS:CONFIRMED\r\n'
    'SEQUENCE:0\r\n'
    'LOCATION;LANGUAGE=en-US:Location\r\n'
    'END:VEVENT\r\n'
    'BEGIN:VEVENT\r\n'
    'UID:event-uid-123\r\n'
    'SUMMARY;LANGUAGE=en-US:childEvent2\r\n'
    'DTSTART;TZID=Europe/Prague:20140306T090000\r\n'
    'DTEND;TZID=Europe/Berlin:20140306T093000\r\n'
    'CLASS:PUBLIC\r\n'
    'PRIORITY:5\r\n'
    'DTSTAMP:20140207T020756Z\r\n'
    'TRANSP:OPAQUE\r\n'
    'STATUS:CONFIRMED\r\n'
    'SEQUENCE:0\r\n'
    'LOCATION;LANGUAGE=en-US:Location\r\n'
    'END:VEVENT\r\n'
    'BEGIN:VEVENT\r\n'
    'UID:event-uid-123\r\n'
    'SUMMARY;LANGUAGE=en-US:childEvent3\r\n'
    'DTSTART;TZID=Europe/Prague:20140306T090000\r\n'
    'DTEND;TZID=Europe/Berlin:20140306T093000\r\n'
    'CLASS:PUBLIC\r\n'
    'PRIORITY:5\r\n'
    'DTSTAMP:20140207T020756Z\r\n'
    'TRANSP:OPAQUE\r\n'
    'STATUS:CONFIRMED\r\n'
    'SEQUENCE:0\r\n'
    'LOCATION;LANGUAGE=en-US:Location\r\n'
    'END:VEVENT\r\n'
    'BEGIN:VCALENDAR\r\n'
    'UID:event-uid-123\r\n'
    'SUMMARY;LANGUAGE=en-US:childCalendar1\r\n'
    'DTSTART;TZID=Europe/Prague:20140306T090000\r\n'
    'DTEND;TZID=Europe/Berlin:20140306T093000\r\n'
    'CLASS:PUBLIC\r\n'
    'PRIORITY:5\r\n'
    'DTSTAMP:20140207T020756Z\r\n'
    'TRANSP:OPAQUE\r\n'
    'STATUS:CONFIRMED\r\n'
    'SEQUENCE:0\r\n'
    'LOCATION;LANGUAGE=en-US:Location\r\n'
    'END:VCALENDAR\r\n'
    'END:VEVENT\r\n'
)


def main():
    """Test child component manipulation"""
    parent = ICalGLib.Component.new_from_string(eventStr1)
    comp1 = ICalGLib.Component.new_from_string(eventStr2)
    comp2 = ICalGLib.Component.new_from_string(eventStr3)
    comp3 = ICalGLib.Component.new_from_string(eventStr4)
    comp4 = ICalGLib.Component.new_from_string(eventStr5)

    parent.add_component(comp1)
    parent.add_component(comp2)
    parent.add_component(comp3)
    parent.add_component(comp4)

    assert parent.as_ical_string() == combinedString

    count = parent.count_components(ICalGLib.ComponentKind.VEVENT_COMPONENT)
    childComponent = parent.get_first_component(ICalGLib.ComponentKind.VEVENT_COMPONENT)
    for i in range(0, count):
        if childComponent.get_summary() == 'childEvent2':
            childComponent.set_summary('childEventTwo')

            start = ICalGLib.Time.new_from_string('20141115T211923')
            end = ICalGLib.Time.new_from_string('20141115T221923')
            childComponent.set_dtstart(start)
            childComponent.set_dtend(end)

            childComponent.set_dtstamp(start)
            childComponent.set_location('East Lansing, MI, US')

            childComponent.set_relcalid('relcalid for childEventTwo')
            recurString = 'RRULE:FREQ=DAILY;INTERVAL=10;COUNT=5'
            recurProperty = ICalGLib.Property.new_from_string(recurString)
            childComponent.add_property(recurProperty)
        if i != count - 1:
            childComponent = parent.get_next_component(ICalGLib.ComponentKind.VEVENT_COMPONENT)

    modifiedCombinedString = parent.as_ical_string()
    ICalGLib.Component.new_from_string(modifiedCombinedString)

    count = parent.count_components(ICalGLib.ComponentKind.VEVENT_COMPONENT)
    childComponent = parent.get_first_component(ICalGLib.ComponentKind.VEVENT_COMPONENT)
    for i in range(0, count):
        if childComponent.get_summary() == 'childEventTwo':
            childComponent.set_summary('childEventTwo')

            dtstart = childComponent.get_dtstart()
            startString = ICalGLib.Time.as_ical_string(dtstart)
            assert startString == '20141115T211923'
            dtend = childComponent.get_dtend()
            endString = ICalGLib.Time.as_ical_string(dtend)
            assert endString == '20141115T221923'

            timestamp = childComponent.get_dtstamp()
            assert ICalGLib.Time.as_ical_string(timestamp) == '20141115T211923'
            assert childComponent.get_location() == 'East Lansing, MI, US'
            assert childComponent.get_relcalid() == 'relcalid for childEventTwo'

            recurProperty = childComponent.get_first_property(ICalGLib.PropertyKind.RRULE_PROPERTY)
            assert recurProperty.as_ical_string() == 'RRULE:FREQ=DAILY;INTERVAL=10;COUNT=5\r\n'
        if i != count - 1:
            childComponent = parent.get_next_component(ICalGLib.ComponentKind.VEVENT_COMPONENT)


if __name__ == '__main__':
    main()
