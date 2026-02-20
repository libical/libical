#!/usr/bin/env python3
# GI_TYPELIB_PATH=$PREFIX/lib/girepository-1.0/ ./component.py

###############################################################################
# SPDX-FileCopyrightText: 2015 William Yu <williamyu@gnome.org>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
###############################################################################

"""Test Python bindings for libical components"""

import os
import sys

import gi

gi.require_version('ICalGLib', '4.0')
from gi.repository import ICalGLib  # noqa E402

try:
    zoneinfodir = os.environ['ZONEINFO_DIRECTORY']
except KeyError:
    print("Error: The ZONEINFO_DIRECTORY environment variable isn't set")
    sys.exit(1)
if not os.path.isdir(zoneinfodir):
    print("Error: The ZONEINFO_DIRECTORY environment variable isn't properly set")
    sys.exit(1)
ICalGLib.Timezone.set_zone_directory(zoneinfodir)
ICalGLib.Timezone.set_tzid_prefix('/citadel.org/')

eventStr1 = (
    'BEGIN:VEVENT\n'
    'UID:event-uid-123\n'
    'SUMMARY;LANGUAGE=en-US:test1\n'
    'DTSTART;TZID=Europe/Prague:20140306T090000\n'
    'DTEND;TZID=Europe/Prague:20140306T093000\n'
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
    'SUMMARY;LANGUAGE=en-US:test2\n'
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
    'SUMMARY;LANGUAGE=en-US:test3\n'
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
    'SUMMARY;LANGUAGE=en-US:test4\n'
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
    'SUMMARY;LANGUAGE=en-US:test5\n'
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

recurringStr = (
    'BEGIN:VEVENT\r\n'
    'UID:recurring\r\n'
    'DTSTAMP:20180403T101443Z\r\n'
    'DTSTART:20180320T150000Z\r\n'
    'DTEND:20180320T153000Z\r\n'
    'SUMMARY:Recurring event\r\n'
    'CREATED:20180403T113809Z\r\n'
    'LAST-MODIFIED:20180403T113905Z\r\n'
    'RRULE:FREQ=DAILY;COUNT=10;INTERVAL=1\r\n'
    'END:VEVENT\r\n'
)

#############################################################


class TestCounter:
    counter = 0

    def inc(self):
        self.counter = self.counter + 1


def foreachTZIDCb(param, user_data):
    del param
    user_data.inc()


def foreachRecurrenceCb(comp, span, user_data):
    del comp
    del span
    user_data.inc()


#############################################################


def main():
    # Test as_ical_string
    comp = ICalGLib.Component.new_from_string(eventStr1)
    string = comp.as_ical_string()

    # Test new_clone
    clone = comp.clone()
    string1 = clone.as_ical_string()
    assert string == string1
    assert comp.is_valid() == 1
    assert comp.isa_component() == 1
    assert comp.isa() == ICalGLib.ComponentKind.VEVENT_COMPONENT

    # Test check_restrictions
    assert comp.check_restrictions() == 0

    # Test count_errors
    assert comp.count_errors() == 0

    # Test kind_is_valid
    assert ICalGLib.Component.kind_is_valid(ICalGLib.ComponentKind.VEVENT_COMPONENT) is True

    # Test kind_to_string
    kindString = ICalGLib.Component.kind_to_string(ICalGLib.ComponentKind.VEVENT_COMPONENT)
    assert ICalGLib.Component.kind_from_string(kindString) == ICalGLib.ComponentKind.VEVENT_COMPONENT

    # Test child component manipulation
    parent = ICalGLib.Component.new_from_string(eventStr1)
    comp1 = ICalGLib.Component.new_from_string(eventStr2)
    comp2 = ICalGLib.Component.new_from_string(eventStr3)
    comp3 = ICalGLib.Component.new_from_string(eventStr4)
    comp4 = ICalGLib.Component.new_from_string(eventStr5)

    parent.add_component(comp1)
    parent.add_component(comp2)
    parent.add_component(comp3)
    parent.add_component(comp4)

    assert parent.count_components(ICalGLib.ComponentKind.VEVENT_COMPONENT) == 3
    assert parent.count_components(ICalGLib.ComponentKind.VCALENDAR_COMPONENT) == 1

    # Traverse with internal API.
    count = parent.count_components(ICalGLib.ComponentKind.VEVENT_COMPONENT)
    childComponent = parent.get_first_component(ICalGLib.ComponentKind.VEVENT_COMPONENT)
    for i in range(0, count):
        prefix = 'test'
        index = i + 2
        assert childComponent.get_summary() == prefix + str(index)
        if i != count - 1:
            childComponent = parent.get_next_component(ICalGLib.ComponentKind.VEVENT_COMPONENT)

    # Traverse with external API.
    iterator = parent.begin_component(ICalGLib.ComponentKind.VEVENT_COMPONENT)
    assert iterator.is_valid()
    childComponent = iterator.deref()
    for i in range(0, count):
        prefix = 'test'
        index = i + 2
        assert childComponent.get_summary() == prefix + str(index)
        if i != count - 1:
            childComponent = iterator.next()

    iterator = parent.end_component(ICalGLib.ComponentKind.VEVENT_COMPONENT)
    childComponent = iterator.prior()
    for i in range(0, count):
        prefix = 'test'
        index = count + 1 - i
        assert childComponent.get_summary() == prefix + str(index)
        if i != count - 1:
            childComponent = iterator.prior()

    # Traverse and remove with external API.
    iterator = parent.begin_component(ICalGLib.ComponentKind.VEVENT_COMPONENT)
    childComponent = iterator.deref()
    for i in range(0, count):
        if i != count - 1:
            iterator.next()
        parent.remove_component(childComponent)
        if i != count - 1:
            childComponent = iterator.deref()
    assert parent.count_components(ICalGLib.ComponentKind.VEVENT_COMPONENT) == 0

    # Test property mainpulation
    propertyString = 'SUMMARY:Bastille Day Party'
    stringProperty = ICalGLib.Property.new_from_string(propertyString)
    component = ICalGLib.Component.new(ICalGLib.ComponentKind.VEVENT_COMPONENT)
    component.add_property(stringProperty)
    assert component.count_properties(ICalGLib.PropertyKind.SUMMARY_PROPERTY) == 1
    component.remove_property(stringProperty)
    assert component.count_properties(ICalGLib.PropertyKind.SUMMARY_PROPERTY) == 0

    component.add_property(stringProperty)
    propertyString2 = 'SUMMARY:event-uid-123'
    stringProperty2 = ICalGLib.Property.new_from_string(propertyString2)
    component.add_property(stringProperty2)
    component.add_property(ICalGLib.Property.new_from_string('SUMMARY:20140306T090000'))
    assert component.count_properties(ICalGLib.PropertyKind.SUMMARY_PROPERTY) == 3
    property1 = component.get_first_property(ICalGLib.PropertyKind.SUMMARY_PROPERTY)
    assert property1.as_ical_string().split('\n', 1)[0] == 'SUMMARY:Bastille Day Party\r'
    property2 = component.get_next_property(ICalGLib.PropertyKind.SUMMARY_PROPERTY)
    assert property2.as_ical_string().split('\n', 1)[0] == 'SUMMARY:event-uid-123\r'
    property3 = component.get_next_property(ICalGLib.PropertyKind.SUMMARY_PROPERTY)
    assert property3.as_ical_string().split('\n', 1)[0] == 'SUMMARY:20140306T090000\r'

    # Test getters and setters
    # Test get_dtstart and get_dtend
    comp = ICalGLib.Component.new_from_string(eventStr1)
    dtstart = comp.get_dtstart()
    startString = ICalGLib.Time.as_ical_string(dtstart)
    assert startString == '20140306T090000'
    dtend = comp.get_dtend()
    endString = dtend.as_ical_string()
    assert endString == '20140306T093000'

    # Test span
    span = comp.get_span()
    assert span.get_start() == 1394096400
    assert span.get_end() == 1394098200
    assert span.get_is_busy() == 1
    utc = ICalGLib.Timezone.get_utc_timezone()
    comp.set_dtstart(ICalGLib.Time.new_from_timet_with_zone(1494096400, 0, utc))
    comp.set_dtend(ICalGLib.Time.new_from_timet_with_zone(1494098200, 0, utc))
    span = comp.get_span()
    assert span.get_start() == 1494096400
    assert span.get_end() == 1494098200
    assert span.get_is_busy() == 1

    # Test set_summary/get_summary
    assert comp.get_summary() == 'test1'
    comp.set_summary('newSummary')
    assert comp.get_summary() == 'newSummary'

    # Test set_comment/get_comment
    assert comp.get_comment() is None
    comp.set_comment('newcomment')
    assert comp.get_comment() == 'newcomment'

    # Test set_uid/get_uid
    assert comp.get_uid() == 'event-uid-123'
    comp.set_uid('newuid')
    assert comp.get_uid() == 'newuid'

    # Test set_relcalid/get_relcalid
    assert comp.get_relcalid() is None
    comp.set_relcalid('newrelcalid')
    assert comp.get_relcalid() == 'newrelcalid'

    # Test set_description/get_description
    assert comp.get_description() is None
    comp.set_description('newdescription')
    assert comp.get_description() == 'newdescription'

    # Test set_location/get_location
    assert comp.get_location() == 'Location'
    comp.set_location('newlocation')
    assert comp.get_location() == 'newlocation'

    # Test set_sequence/get_sequence
    assert comp.get_sequence() == 0
    comp.set_sequence(5)
    assert comp.get_sequence() == 5

    # Call comp_foreach_tzid
    comp = ICalGLib.Component.new_from_string(eventStr1)
    counter = TestCounter()
    comp.foreach_tzid(foreachTZIDCb, counter)
    assert counter.counter == 2

    # Test propiter and paramiter
    comp = ICalGLib.Component.new_from_string(eventStr1)
    iter = comp.begin_property(ICalGLib.PropertyKind.ANY_PROPERTY)
    assert iter.is_valid()
    assert iter.deref().as_ical_string().split('\n', 1)[0] == 'UID:event-uid-123\r'
    iter.next()
    prop = iter.deref()
    assert prop.as_ical_string().split('\n', 1)[0] == 'SUMMARY;LANGUAGE=en-US:test1\r'
    paramiter = prop.begin_parameter(ICalGLib.ParameterKind.ANY_PARAMETER)
    assert paramiter.deref().as_ical_string().split('\n', 1)[0] == 'LANGUAGE=en-US'
    paramiter.next()
    assert paramiter.deref() is None

    counter = TestCounter()
    comp = ICalGLib.Component.new_from_string(recurringStr)
    comp.foreach_recurrence(
        ICalGLib.Time.new_from_string('20180321T000000Z'),
        ICalGLib.Time.new_from_string('20180323T235959Z'),
        foreachRecurrenceCb,
        counter,
    )
    assert counter.counter == 3

    comp = ICalGLib.Component.new_from_string(eventStr1)
    prop = comp.get_first_property(ICalGLib.PropertyKind.DTSTART_PROPERTY)
    prop.remove_parameter_by_kind(ICalGLib.ParameterKind.TZID_PARAMETER)
    zone = ICalGLib.Timezone.get_builtin_timezone('Europe/Prague')

    ICalGLib.Timezone.set_tzid_prefix(zone.get_tzid().replace('Europe/Prague', ''))

    prop.set_parameter(ICalGLib.Parameter.new_tzid(zone.get_tzid()))

    itt = prop.get_datetime_with_component(comp)
    assert itt.get_timezone() is not None
    assert itt.get_timezone().get_location() == 'Europe/Prague'

    itt = prop.get_datetime_with_component(None)
    assert itt.get_timezone() is not None
    assert itt.get_timezone().get_location() == 'Europe/Prague'

    itt = comp.get_dtstart()
    assert itt.get_timezone() is not None
    assert itt.get_timezone().get_location() == 'Europe/Prague'


if __name__ == '__main__':
    main()
