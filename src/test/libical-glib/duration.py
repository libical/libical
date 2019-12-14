#!/usr/bin/env python3
#GI_TYPELIB_PATH=$PREFIX/lib/girepository-1.0/ ./duration-type.py

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

length = 1000000000;
bad_string = "This is a bad string";

duration = ICalGLib.Duration.new_from_int(length);
assert(duration.as_int() == length);
length_in_string = duration.as_ical_string();
duration1 = ICalGLib.Duration.new_from_string(length_in_string);
assert(duration1.as_ical_string() == length_in_string);
assert(length == duration1.as_int());

duration = ICalGLib.Duration.new_from_string(bad_string);
duration_bad = ICalGLib.Duration.new_bad_duration();
assert(duration.as_ical_string() == duration_bad.as_ical_string());
assert(duration.is_bad_duration() == 1);
duration_null = ICalGLib.Duration.new_null_duration();
assert(duration_null.is_null_duration() == 1);
