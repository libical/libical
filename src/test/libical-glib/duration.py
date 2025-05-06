#!/usr/bin/env python3
# GI_TYPELIB_PATH=$PREFIX/lib/girepository-1.0/ ./duration-type.py

###############################################################################
#
# SPDX-FileCopyrightText: 2015 William Yu <williamyu@gnome.org>
#
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
#
###############################################################################

"""Test Python bindings for libical duration"""

import gi

gi.require_version('ICalGLib', '4.0')
from gi.repository import ICalGLib  # noqa E402

length = 1000000000
badString = 'This is a bad string'

duration = ICalGLib.Duration.new_from_int(length)
assert duration.as_int() == length
length_in_string = duration.as_ical_string()
duration1 = ICalGLib.Duration.new_from_string(length_in_string)
assert duration1.as_ical_string() == length_in_string
assert length == duration1.as_int()

duration = ICalGLib.Duration.new_from_string(badString)
durationBad = ICalGLib.Duration.new_bad_duration()
assert duration.as_ical_string() == durationBad.as_ical_string()
assert duration.is_bad_duration() == 1
duration_null = ICalGLib.Duration.new_null_duration()
assert duration_null.is_null_duration() == 1
