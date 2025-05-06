#!/usr/bin/env python3
# GI_TYPELIB_PATH=$PREFIX/lib/girepository-1.0/ ./property.py

###############################################################################
#
# SPDX-FileCopyrightText: 2015 William Yu <williamyu@gnome.org>
#
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
#
###############################################################################

"""Test Python bindings for libical properties"""

import gi

gi.require_version('ICalGLib', '4.0')
from gi.repository import ICalGLib  # noqa E402

actionProperty = ICalGLib.Property.new(ICalGLib.PropertyKind.ACKNOWLEDGED_PROPERTY)
actionPropertyClone = actionProperty.clone()
assert actionPropertyClone.as_ical_string() == actionProperty.as_ical_string()

propertyString = 'SUMMARY:Bastille Day Party'
stringProperty = ICalGLib.Property.new_from_string(propertyString)
stringPropertyClone = stringProperty.clone()
assert stringProperty.as_ical_string() == stringPropertyClone.as_ical_string()
assert stringProperty.isa() == ICalGLib.PropertyKind.SUMMARY_PROPERTY
assert stringProperty.isa_property() == 1
assert stringProperty.get_property_name() == 'SUMMARY'

altrep1 = 'This is an altrep 1'
parameter1 = ICalGLib.Parameter.new_altrep(altrep1)
altrep2 = 'This is an altrep 2'
parameter2 = ICalGLib.Parameter.new_altrep(altrep2)
stringProperty.add_parameter(parameter1)
stringProperty.add_parameter(parameter2)
stringProperty.set_parameter_from_string('ACTIONPARAM', 'This is an action param')

assert stringProperty.count_parameters() == 3
retrieved_parameter1 = stringProperty.get_first_parameter(ICalGLib.ParameterKind.ALTREP_PARAMETER)
assert retrieved_parameter1.as_ical_string() == parameter1.as_ical_string()
retrieved_parameter2 = stringProperty.get_next_parameter(ICalGLib.ParameterKind.ALTREP_PARAMETER)
assert retrieved_parameter2.as_ical_string() == parameter2.as_ical_string()
retrieved_parameter3 = stringProperty.get_first_parameter(ICalGLib.ParameterKind.ACTIONPARAM_PARAMETER)
assert retrieved_parameter3.as_ical_string() == 'ACTIONPARAM=This is an action param'

stringProperty.remove_parameter_by_kind(ICalGLib.ParameterKind.CHARSET_PARAMETER)
assert stringProperty.count_parameters() == 3
stringProperty.remove_parameter_by_kind(ICalGLib.ParameterKind.ALTREP_PARAMETER)
assert stringProperty.count_parameters() == 2
stringProperty.remove_parameter_by_name('ACTIONPARAM')
assert stringProperty.count_parameters() == 1

kind = ICalGLib.ValueKind.ATTACH_VALUE
string = 'This is a link'
value_from_string = ICalGLib.Value.new_from_string(kind, string)
stringProperty.set_value(value_from_string)
string_from_property_api = stringProperty.get_value_as_string()
assert string_from_property_api == string
value_got_from_property = stringProperty.get_value()
assert value_got_from_property.as_ical_string() == string
stringProperty.set_value_from_string(string, ICalGLib.Value.kind_to_string(kind))
string_from_property_api = stringProperty.get_value_as_string()
assert string_from_property_api == string
value_got_from_property = stringProperty.get_value()
assert value_got_from_property.as_ical_string() == string

stringProperty.set_x_name('This is an x name!')
assert stringProperty.get_x_name() == 'This is an x name!'

valuekind_from_parametervalue = ICalGLib.Parameter.value_to_value_kind(ICalGLib.ParameterValue.BINARY)
assert valuekind_from_parametervalue == ICalGLib.ValueKind.BINARY_VALUE

valuekind_from_property_kind = ICalGLib.Property.kind_to_value_kind(ICalGLib.PropertyKind.ACKNOWLEDGED_PROPERTY)
assert valuekind_from_property_kind == ICalGLib.ValueKind.DATETIME_VALUE
propertyKind = ICalGLib.Value.kind_to_property_kind(valuekind_from_property_kind)
assert propertyKind == ICalGLib.PropertyKind.ACKNOWLEDGED_PROPERTY
string = ICalGLib.Property.kind_to_string(ICalGLib.PropertyKind.ACKNOWLEDGED_PROPERTY)
assert string == 'ACKNOWLEDGED'
kind = ICalGLib.Property.kind_from_string(string)
assert kind == ICalGLib.PropertyKind.ACKNOWLEDGED_PROPERTY

string = ICalGLib.Property.method_to_string(ICalGLib.PropertyMethod.PUBLISH)
assert ICalGLib.Property.method_from_string(string) == ICalGLib.PropertyMethod.PUBLISH

string = ICalGLib.Property.enum_to_string(ICalGLib.PropertyMethod.PUBLISH)
assert string == 'PUBLISH'

# Test i_cal_value_set_parent. No error will be thrown.
propertyString = 'SUMMARY:Bastille Day Party'
stringProperty = ICalGLib.Property.new_from_string(propertyString)
kind = ICalGLib.ValueKind.ATTACH_VALUE
string = 'This is a link'
value_from_string = ICalGLib.Value.new_from_string(kind, string)
value_from_string.set_parent(stringProperty)
value_from_string.set_parent(None)
