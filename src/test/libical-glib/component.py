#!/usr/bin/env python3
#GI_TYPELIB_PATH=$PREFIX/lib/girepository-1.0/ ./component.py

###############################################################################
#
# Copyright (C) 2015 William Yu <williamyu@gnome.org>
#
# This library is free software: you can redistribute it and/or modify it
# under the terms of version 2.1. of the GNU Lesser General Public License
# as published by the Free Software Foundation.
#
# This library is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
# for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library. If not, see <http://www.gnu.org/licenses/>.
#
###############################################################################

from gi.repository import ICalGLib

event_str1 = \
    "BEGIN:VEVENT\n"                                \
    "UID:event-uid-123\n"                           \
    "SUMMARY;LANGUAGE=en-US:test1\n"                \
    "DTSTART;TZID=Europe/Prague:20140306T090000\n"  \
    "DTEND;TZID=Europe/Prague:20140306T093000\n"    \
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
    "SUMMARY;LANGUAGE=en-US:test2\n"                \
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
    "SUMMARY;LANGUAGE=en-US:test3\n"                \
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
    "SUMMARY;LANGUAGE=en-US:test4\n"                \
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
    "SUMMARY;LANGUAGE=en-US:test5\n"                \
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

#############################################################
#The implementation of i_cal_component_foreach_tzid in Python
def comp_foreach_tzid(comp, callback, data):
    prop = ICalGLib.Component.get_first_property(comp, ICalGLib.PropertyKind.ANY_PROPERTY);
    while prop != None:
        kind = prop.isa();
        if (kind == ICalGLib.PropertyKind.DTSTART_PROPERTY
        or kind == ICalGLib.PropertyKind.DTEND_PROPERTY
        or kind == ICalGLib.PropertyKind.DUE_PROPERTY
        or kind == ICalGLib.PropertyKind.EXDATE_PROPERTY
        or kind == ICalGLib.PropertyKind.RDATE_PROPERTY):
            param = prop.get_first_parameter(ICalGLib.ParameterKind.TZID_PARAMETER);
            if param != None:
                callback(param, data);
        prop = comp.get_next_property(ICalGLib.PropertyKind.ANY_PROPERTY);

    subcomp = comp.get_first_component(ICalGLib.ComponentKind.ANY_COMPONENT);
    while (subcomp != None):
        comp_foreach_tzid(subcomp, callback, data);
        subcomp = comp.get_next_component();

def setParam(param, data):
    ICalGLib.Parameter.set_tzid(param, data);

#############################################################

def main():
    #Test as_ical_string_r
    comp = ICalGLib.Component.new_from_string(event_str1);
    string = comp.as_ical_string_r();

    #Test new_clone
    clone = comp.new_clone();
    string1 = clone.as_ical_string_r();
    assert(string == string1);
    assert(comp.is_valid() == 1);
    assert(comp.isa_component() == 1);
    assert(comp.isa() == ICalGLib.ComponentKind.VEVENT_COMPONENT);

    #Test check_restrictions
    assert(comp.check_restrictions() == 0);

    #Test count_errors
    assert(comp.count_errors() == 0);

    #Test kind_is_valid
    assert(ICalGLib.Component.kind_is_valid(ICalGLib.ComponentKind.VEVENT_COMPONENT) == True);

    #Test kind_to_string
    kind_string = ICalGLib.Component.kind_to_string(ICalGLib.ComponentKind.VEVENT_COMPONENT);
    assert(ICalGLib.Component.string_to_kind(kind_string) == ICalGLib.ComponentKind.VEVENT_COMPONENT);

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

    assert parent.count_components(ICalGLib.ComponentKind.VEVENT_COMPONENT) == 3;
    assert parent.count_components(ICalGLib.ComponentKind.VCALENDAR_COMPONENT) == 1;

    #Traverse with internal API.
    count = parent.count_components(ICalGLib.ComponentKind.VEVENT_COMPONENT);
    child_component = parent.get_first_component(ICalGLib.ComponentKind.VEVENT_COMPONENT);
    for i in range(0, count):
        prefix = "test"
        index = i+2;
        assert(child_component.get_summary() == prefix + str(index));
        if (i != count-1):
            child_component = parent.get_next_component(ICalGLib.ComponentKind.VEVENT_COMPONENT);

    #Traverse with external API.
    iter = parent.begin_component(ICalGLib.ComponentKind.VEVENT_COMPONENT);
    child_component = iter.deref();
    child_component.set_owner(parent);
    for i in range(0, count):
        prefix = "test"
        index = i+2;
        assert(child_component.get_summary() == prefix + str(index));
        if (i != count-1):
            child_component = iter.next();
            child_component.set_owner(parent);

    iter = parent.end_component(ICalGLib.ComponentKind.VEVENT_COMPONENT);
    child_component = iter.prior();
    child_component.set_owner(parent);
    for i in range(0, count):
        prefix = "test"
        index = count + 1 - i;
        assert(child_component.get_summary() == prefix + str(index));
        if (i != count - 1):
            child_component = iter.prior();
            child_component.set_owner(parent);

    #Traverse and remove with external API.
    iter = parent.begin_component(ICalGLib.ComponentKind.VEVENT_COMPONENT);
    child_component = iter.deref();
    for i in range(0, count):
        if (i != count - 1):
            iter.next();
        parent.remove_component(child_component);
        if (i != count - 1):
            child_component = iter.deref();
    assert parent.count_components(ICalGLib.ComponentKind.VEVENT_COMPONENT) == 0;

    #Test property mainpulation
    property_string = "SUMMARY:Bastille Day Party";
    string_property = ICalGLib.Property.new_from_string(property_string);
    component = ICalGLib.Component.new(ICalGLib.ComponentKind.VEVENT_COMPONENT);
    component.add_property(string_property);
    assert(component.count_properties(ICalGLib.PropertyKind.SUMMARY_PROPERTY) == 1);
    component.remove_property(string_property);
    assert(component.count_properties(ICalGLib.PropertyKind.SUMMARY_PROPERTY) == 0);

    component.add_property(string_property);
    property_string2 = "SUMMARY:event-uid-123";
    string_property2 = ICalGLib.Property.new_from_string(property_string2);
    component.add_property(string_property2);
    component.add_property(ICalGLib.Property.new_from_string("SUMMARY:20140306T090000"));
    assert(component.count_properties(ICalGLib.PropertyKind.SUMMARY_PROPERTY) == 3);
    property1 = component.get_first_property(ICalGLib.PropertyKind.SUMMARY_PROPERTY);
    assert(property1.as_ical_string_r().split('\n', 1)[0] == "SUMMARY:Bastille Day Party\r");
    property2 = component.get_next_property(ICalGLib.PropertyKind.SUMMARY_PROPERTY);
    assert(property2.as_ical_string_r().split('\n', 1)[0] == "SUMMARY:event-uid-123\r");
    property3 = component.get_next_property(ICalGLib.PropertyKind.SUMMARY_PROPERTY);
    assert(property3.as_ical_string_r().split('\n', 1)[0] == "SUMMARY:20140306T090000\r");

    #Test getters and setters
    #Test get_dtstart and get_dtend
    comp = ICalGLib.Component.new_from_string(event_str1);
    dtstart = comp.get_dtstart();
    start_string = ICalGLib.time_as_ical_string_r(dtstart);
    assert(start_string == "20140306T090000");
    dtend = comp.get_dtend();
    end_string = ICalGLib.time_as_ical_string_r(dtend);
    assert(end_string == "20140306T093000");

    #Test span
    span = comp.get_span();
    assert(span.get_start() == 1394096400);
    assert(span.get_end() == 1394098200);
    assert(span.is_busy() == 1);
    comp.set_dtstart(ICalGLib.time_from_timet(1494096400, 0));
    comp.set_dtend(ICalGLib.time_from_timet(1494098200, 0));
    span = comp.get_span();
    assert(span.get_start() == 1494096400);
    assert(span.get_end() == 1494098200);
    assert(span.is_busy() == 1);

    #Test set_summary/get_summary
    assert(comp.get_summary() == "test1");
    comp.set_summary("newSummary");
    assert(comp.get_summary() == "newSummary");

    #Test set_comment/get_comment
    assert(comp.get_comment() == None);
    comp.set_comment("newcomment");
    assert(comp.get_comment() == "newcomment");

    #Test set_uid/get_uid
    assert(comp.get_uid() == "event-uid-123");
    comp.set_uid("newuid");
    assert(comp.get_uid() == "newuid");

    #Test set_relcalid/get_relcalid
    assert(comp.get_relcalid() == None);
    comp.set_relcalid("newrelcalid");
    assert(comp.get_relcalid() == "newrelcalid");

    #Test set_description/get_description
    assert(comp.get_description() == None);
    comp.set_description("newdescription");
    assert(comp.get_description() == "newdescription");

    #Test set_location/get_location
    assert(comp.get_location() == "Location");
    comp.set_location("newlocation");
    assert(comp.get_location() == "newlocation");

    #Test set_sequence/get_sequence
    assert(comp.get_sequence() == 0);
    comp.set_sequence(5);
    assert(comp.get_sequence() == 5);

    #Call comp_foreach_tzid
    comp_foreach_tzid(comp, setParam, "America/Chicago");

if __name__ == "__main__":
    main()
