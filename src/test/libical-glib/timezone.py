#!/usr/bin/env python3
#GI_TYPELIB_PATH=$PREFIX/lib/girepository-1.0/ ./timezone.py

###############################################################################
#
# Copyright (C) 2015 William Yu <williamyu@gnome.org>
#
# This library is free software; you can redistribute it and/or modify
# it under the terms of either:
#
#   The LGPL as published by the Free Software Foundation, version
#   2.1, available at: http://www.gnu.org/licenses/lgpl-2.1.txt
#
# Or:
#
#   The Mozilla Public License Version 2.0. You may obtain a copy of
#   the License at http://www.mozilla.org/MPL/
#
###############################################################################

import gi

gi.require_version('ICalGLib', '3.0')

from gi.repository import ICalGLib

import os
import sys

try:
    zoneinfodir = os.environ['ZONEINFO_DIRECTORY']
except KeyError:
    print("Error: The ZONEINFO_DIRECTORY environment variable isn't set")
    sys.exit(1)
if not os.path.isdir(zoneinfodir):
    print("Error: The ZONEINFO_DIRECTORY environment variable isn't properly set")
    sys.exit(1)
ICalGLib.Timezone.set_zone_directory(zoneinfodir);
ICalGLib.Timezone.set_tzid_prefix("/citadel.org/");

la = ICalGLib.Timezone.get_builtin_timezone("America/Los_Angeles");
chicago = ICalGLib.Timezone.get_builtin_timezone("America/Chicago");
assert la.get_tzid().find("Los_Angeles") != -1;
assert la.get_location() == "America/Los_Angeles";
assert la.get_tznames() == "PST/PDT";

la_copy = la.copy();
assert la_copy.get_tzid() == la.get_tzid();
assert la_copy.get_location() == la.get_location();

timezones = ICalGLib.Timezone.get_builtin_timezones();
timezone = ICalGLib.Timezone.array_element_at(timezones, 0);
if not ICalGLib.Timezone.get_builtin_tzdata():
    assert timezone.get_display_name() == "Europe/Andorra";
assert timezones.size() > 0;

from_tzid = ICalGLib.Timezone.get_builtin_timezone_from_tzid(la.get_tzid());
assert from_tzid.get_location() == "America/Los_Angeles";

utc = ICalGLib.Timezone.get_utc_timezone();
assert utc.get_display_name() == "UTC";
utc2 = ICalGLib.Timezone.get_utc_timezone();
assert utc == utc2

time = ICalGLib.Timetype.new();
before = time.get_hour();
ICalGLib.Timezone.convert_time(time, la, chicago);
after = time.get_hour();
assert abs(after - before) == 2;
ICalGLib.Timetype.set_zone(time, utc);
assert ICalGLib.Timetype.get_zone(time) == utc;
ICalGLib.Timetype.set_zone(time, la);
assert ICalGLib.Timetype.get_zone(time) == la;

component = la.get_component();
timezone = ICalGLib.Timezone.new();
timezone.set_component(ICalGLib.Component.new_clone(component));
assert timezone.get_location() == la.get_location();

array = ICalGLib.Timezone.array_new();
ICalGLib.Timezone.array_append_from_vtimezone(array, ICalGLib.Component.new_clone(la.get_component()));
ICalGLib.Timezone.array_append_from_vtimezone(array, ICalGLib.Component.new_clone(chicago.get_component()));
assert array.size() == 2;
timezone1 = ICalGLib.Timezone.array_element_at(array, 0);
assert timezone1.get_display_name() == la.get_display_name();
timezone2 = ICalGLib.Timezone.array_element_at(array, 1);
assert timezone2.get_display_name() == chicago.get_display_name();

ICalGLib.Timezone.free_builtin_timezones()
ICalGLib.Object.free_global_objects()
