#!/usr/bin/env python3
#GI_TYPELIB_PATH=$PREFIX/lib/girepository-1.0/ ./property.py

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

action_property = ICalGLib.Property.new(ICalGLib.PropertyKind.ACKNOWLEDGED_PROPERTY);
action_property_clone = action_property.clone();
assert(action_property_clone.as_ical_string() == action_property.as_ical_string());

property_string = "SUMMARY:Bastille Day Party";
string_property = ICalGLib.Property.new_from_string(property_string);
string_property_clone = string_property.clone();
assert(string_property.as_ical_string() == string_property_clone.as_ical_string());
assert(string_property.isa() == ICalGLib.PropertyKind.SUMMARY_PROPERTY);
assert(string_property.isa_property() == 1);
assert(string_property.get_property_name() == "SUMMARY");

altrep1 = "This is an altrep 1";
parameter1 = ICalGLib.Parameter.new_altrep(altrep1);
altrep2 = "This is an altrep 2";
parameter2 = ICalGLib.Parameter.new_altrep(altrep2);
string_property.add_parameter(parameter1);
string_property.add_parameter(parameter2);
string_property.set_parameter_from_string("ACTIONPARAM", "This is a action param");

assert(string_property.count_parameters() == 3);
retrieved_parameter1 = string_property.get_first_parameter(ICalGLib.ParameterKind.ALTREP_PARAMETER);
assert(retrieved_parameter1.as_ical_string() == parameter1.as_ical_string());
retrieved_parameter2 = string_property.get_next_parameter(ICalGLib.ParameterKind.ALTREP_PARAMETER);
assert(retrieved_parameter2.as_ical_string() == parameter2.as_ical_string());
retrieved_parameter3 = string_property.get_first_parameter(ICalGLib.ParameterKind.ACTIONPARAM_PARAMETER);
assert(retrieved_parameter3.as_ical_string() == "ACTIONPARAM=This is a action param");

string_property.remove_parameter_by_kind(ICalGLib.ParameterKind.CHARSET_PARAMETER);
assert(string_property.count_parameters() == 3);
string_property.remove_parameter_by_kind(ICalGLib.ParameterKind.ALTREP_PARAMETER);
assert(string_property.count_parameters() == 2);
string_property.remove_parameter_by_name("ACTIONPARAM");
assert(string_property.count_parameters() == 1);

kind = ICalGLib.ValueKind.ATTACH_VALUE;
string = "This is a link";
value_from_string = ICalGLib.Value.new_from_string(kind, string);
string_property.set_value(value_from_string);
string_from_property_api = string_property.get_value_as_string();
assert(string_from_property_api == string);
value_got_from_property = string_property.get_value();
assert(value_got_from_property.as_ical_string() == string);
string_property.set_value_from_string(string, ICalGLib.Value.kind_to_string(kind));
string_from_property_api = string_property.get_value_as_string();
assert(string_from_property_api == string);
value_got_from_property = string_property.get_value();
assert(value_got_from_property.as_ical_string() == string);

string_property.set_x_name("This is an x name!");
assert(string_property.get_x_name() == "This is an x name!");

valuekind_from_parametervalue = ICalGLib.Parameter.value_to_value_kind(ICalGLib.ParameterValue.BINARY);
assert(valuekind_from_parametervalue == ICalGLib.ValueKind.BINARY_VALUE);

valuekind_from_property_kind = ICalGLib.Property.kind_to_value_kind(ICalGLib.PropertyKind.ACKNOWLEDGED_PROPERTY);
assert(valuekind_from_property_kind == ICalGLib.ValueKind.DATETIME_VALUE);
propertyKind = ICalGLib.Value.kind_to_property_kind(valuekind_from_property_kind);
assert(propertyKind == ICalGLib.PropertyKind.ACKNOWLEDGED_PROPERTY);
string = ICalGLib.Property.kind_to_string(ICalGLib.PropertyKind.ACKNOWLEDGED_PROPERTY);
assert(string == "ACKNOWLEDGED");
kind = ICalGLib.Property.kind_from_string(string);
assert(kind == ICalGLib.PropertyKind.ACKNOWLEDGED_PROPERTY);

string = ICalGLib.Property.method_to_string(ICalGLib.PropertyMethod.PUBLISH);
assert(ICalGLib.Property.method_from_string(string) == ICalGLib.PropertyMethod.PUBLISH);

string = ICalGLib.Property.enum_to_string(ICalGLib.PropertyMethod.PUBLISH);
assert(string == "PUBLISH");

#Test i_cal_value_set_parent. No error will be thrown.
property_string = "SUMMARY:Bastille Day Party";
string_property = ICalGLib.Property.new_from_string(property_string);
kind = ICalGLib.ValueKind.ATTACH_VALUE
string = "This is a link"
value_from_string = ICalGLib.Value.new_from_string(kind, string)
value_from_string.set_parent(string_property)
value_from_string.set_parent(None)
