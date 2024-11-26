#!/usr/bin/env python3
# GI_TYPELIB_PATH=$PREFIX/lib/girepository-1.0/ ./value.py

###############################################################################
#
# SPDX-FileCopyrightText: 2015 William Yu <williamyu@gnome.org>
#
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
#
###############################################################################

"""Test Python bindings for libical value"""

import gi

gi.require_version('ICalGLib', '3.0')
from gi.repository import ICalGLib  # noqa E402

kind = ICalGLib.ValueKind.ATTACH_VALUE
value = ICalGLib.Value.new(kind)
assert value.is_valid() == 1
assert value.isa() == kind
assert value.isa_value() == 1

string = 'This is a link'
value_from_string = ICalGLib.Value.new_from_string(kind, string)
assert value_from_string.as_ical_string() == string
assert value_from_string.is_valid() == 1
assert value_from_string.isa() == kind
assert value_from_string.isa_value() == 1

value_from_string_clone = value_from_string.clone()
assert value_from_string_clone.as_ical_string() == string
assert value_from_string_clone.is_valid() == 1
assert value_from_string_clone.isa() == kind
assert value_from_string_clone.isa_value() == 1

value_a = ICalGLib.Value.new(ICalGLib.ValueKind.RECUR_VALUE)
value_b = ICalGLib.Value.new(ICalGLib.ValueKind.ATTACH_VALUE)
compare_result = ICalGLib.Value.compare(value_a, value_b)
assert compare_result == ICalGLib.ParameterXliccomparetype.NOTEQUAL

stringA = 'a string'
stringB = 'a string'
value_a = ICalGLib.Value.new_from_string(kind, stringA)
value_b = ICalGLib.Value.new_from_string(kind, stringB)
compare_result = ICalGLib.Value.compare(value_a, value_b)
assert compare_result == ICalGLib.ParameterXliccomparetype.EQUAL

stringA = 'a string'
stringB = 'b string'
value_a = ICalGLib.Value.new_from_string(kind, stringA)
value_b = ICalGLib.Value.new_from_string(kind, stringB)
compare_result = ICalGLib.Value.compare(value_a, value_b)
assert compare_result == ICalGLib.ParameterXliccomparetype.NOTEQUAL

kind_string = ICalGLib.Value.kind_to_string(kind)
assert kind_string == 'ATTACH'
converted_kind = ICalGLib.Value.kind_from_string(kind_string)
assert converted_kind == kind

szText = 'This is a text\nand this is a new line'
afterEncodedSZText = 'This is a text\\nand this is a new line'
beforeDecodedSZText = 'This is a text\\\nand this is a new line'
result = ICalGLib.Value.encode_ical_string(szText)
assert result == afterEncodedSZText

result = ICalGLib.Value.decode_ical_string(beforeDecodedSZText)
assert result == szText

szText = 'Simple text'
result = ICalGLib.Value.encode_ical_string(szText)
assert result == szText
result = ICalGLib.Value.decode_ical_string(result)
assert result == szText
