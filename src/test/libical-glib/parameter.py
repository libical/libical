#!/usr/bin/env python3
# GI_TYPELIB_PATH=$PREFIX/lib/girepository-1.0/ ./parameter.py

###############################################################################
# SPDX-FileCopyrightText: 2015 William Yu <williamyu@gnome.org>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
###############################################################################

"""Test Python bindings for Libical parameter"""

import gi

gi.require_version('ICalGLib', '4.0')
from gi.repository import ICalGLib  # noqa E402

# Test interaction with gchar
altrep = 'This is an altrep'
parameter = ICalGLib.Parameter.new_altrep(altrep)
retrievedAltrep = parameter.get_altrep()
assert retrievedAltrep == altrep
anotherAltrep = 'This is an another altrep'
parameter.set_altrep(anotherAltrep)
retrievedAltrep = parameter.get_altrep()
assert retrievedAltrep == anotherAltrep

# Test interaction with enum
action = ICalGLib.ParameterAction.X
parameter = ICalGLib.Parameter.new_actionparam(action)
retrievedAction = parameter.get_actionparam()
assert retrievedAction == action
action = ICalGLib.ParameterAction.ASK
parameter.set_actionparam(action)
retrievedAction = parameter.get_actionparam()
assert retrievedAction == action

# Test general utilities
kind = ICalGLib.ParameterKind.ACTIONPARAM_PARAMETER
parameter = ICalGLib.Parameter.new(kind)
assert parameter.isa() == kind
assert parameter.isa_parameter() == 1
string = parameter.as_ical_string()
assert string is None
clone = parameter.clone()
assert clone.isa() == kind
assert clone.isa_parameter() == 1
string = clone.as_ical_string()
assert string is None

string = ICalGLib.Parameter.kind_to_string(kind)
assert string == 'ACTIONPARAM'
assert ICalGLib.Parameter.kind_from_string(string) == kind

value = 'This is a value'
typevalue = string + '=' + value
parameter = ICalGLib.Parameter.new_from_string(typevalue)
assert parameter.as_ical_string() == typevalue
assert parameter.isa() == kind
assert parameter.isa_parameter() == 1

anotherParameter = ICalGLib.Parameter.new_from_value_string(kind, value)
assert anotherParameter.as_ical_string() == typevalue
