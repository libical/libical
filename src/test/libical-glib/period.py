#!/usr/bin/env python3
#GI_TYPELIB_PATH=$PREFIX/lib/girepository-1.0/ ./period-type.py

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

string = "19970101T183248Z/19970102T071625Z";

period = ICalGLib.Period.new_from_string(string);
retrieved_string = period.as_ical_string();
assert(retrieved_string == string);

null_period = ICalGLib.Period.new_null_period();
assert(null_period.is_null_period() == 1);
assert(period.is_null_period() == 0);

assert(null_period.is_valid_period() == 1);
assert(period.is_valid_period() == 1);

start = period.get_start();
assert(start.get_year() == 1997);
assert(start.get_month() == 1);
assert(start.get_day() == 1);
assert(start.get_hour() == 18);
assert(start.get_minute() == 32);
assert(start.get_second() == 48);

end = period.get_end();
assert(end.get_year() == 1997);
assert(end.get_month() == 1);
assert(end.get_day() == 2);
assert(end.get_hour() == 7);
assert(end.get_minute() == 16);
assert(end.get_second() == 25);

duration = period.get_duration();
assert(duration.as_int() == 0);

string = "19970101T182346Z/PT5H30M";
period = ICalGLib.Period.new_from_string(string);
retrieved_string = period.as_ical_string();
assert(retrieved_string == string);

start = period.get_start();
assert(start.get_year() == 1997);
assert(start.get_month() == 1);
assert(start.get_day() == 1);
assert(start.get_hour() == 18);
assert(start.get_minute() == 23);
assert(start.get_second() == 46);

end = period.get_end();
assert(end.get_year() == 0);
assert(end.get_month() == 0);
assert(end.get_day() == 0);
assert(end.get_hour() == 0);
assert(end.get_minute() == 0);
assert(end.get_second() == 0);

duration = period.get_duration();
assert(duration.as_ical_string() == "PT5H30M");
