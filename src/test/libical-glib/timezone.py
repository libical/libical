#!/usr/bin/env python3
#GI_TYPELIB_PATH=$PREFIX/lib/girepository-1.0/ ./timezone.py

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

la = ICalGLib.Timezone.get_builtin_timezone ("America/Los_Angeles");
chicago = ICalGLib.Timezone.get_builtin_timezone ("America/Chicago");
assert la.get_tzid().find ("Los_Angeles") != -1;
assert la.get_location() == "America/Los_Angeles";
assert la.get_tznames() == "PST/PDT";

la_copy = la.copy();
assert la_copy.get_tzid() == la.get_tzid();
assert la_copy.get_location() == la.get_location();

timezones = ICalGLib.Timezone.get_builtin_timezones();
timezone = ICalGLib.Timezone.array_element_at(timezones, 0);
assert timezone.get_display_name() == "Europe/Andorra";
assert timezones.size() > 0;


from_tzid = ICalGLib.Timezone.get_builtin_timezone_from_tzid (la.get_tzid());
assert from_tzid.get_location() == "America/Los_Angeles";

utc = ICalGLib.Timezone.get_utc_timezone();
assert utc.get_display_name() == "UTC";

time = ICalGLib.Timetype.new();
before = time.get_hour();
ICalGLib.Timezone.convert_time(time, la, chicago);
after = time.get_hour();
assert abs(after - before) == 2;

component = la.get_component();
timezone = ICalGLib.Timezone.new();
timezone.set_component (component);
assert timezone.get_location() == la.get_location();

array = ICalGLib.Timezone.array_new();
ICalGLib.Timezone.array_append_from_vtimezone (array, la.get_component());
ICalGLib.Timezone.array_append_from_vtimezone (array, chicago.get_component());
assert array.size() == 2;
timezone1 = ICalGLib.Timezone.array_element_at (array, 0);
assert timezone1.get_display_name() == la.get_display_name();
timezone2 = ICalGLib.Timezone.array_element_at (array, 1);
assert timezone2.get_display_name() == chicago.get_display_name();
