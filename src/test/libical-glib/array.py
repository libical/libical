#!/usr/bin/env python3
#GI_TYPELIB_PATH=$PREFIX/lib/girepository-1.0/ ./array.py

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

array = ICalGLib.Timezone.array_new();

#TEST APPEND
zone0 = ICalGLib.Timezone.get_builtin_timezone("Pacific/Midway");
zone1 = ICalGLib.Timezone.get_builtin_timezone("America/Vancouver");
zone2 = ICalGLib.Timezone.get_builtin_timezone("Atlantic/Bermuda");
zone3 = ICalGLib.Timezone.get_builtin_timezone("Africa/Casablanca");
zone4 = ICalGLib.Timezone.get_builtin_timezone("Asia/Irkutsk");

ICalGLib.Timezone.array_append_from_vtimezone(array, zone0.get_component());
ICalGLib.Timezone.array_append_from_vtimezone(array, zone1.get_component());
ICalGLib.Timezone.array_append_from_vtimezone(array, zone2.get_component());
ICalGLib.Timezone.array_append_from_vtimezone(array, zone3.get_component());
ICalGLib.Timezone.array_append_from_vtimezone(array, zone4.get_component());

assert array.size() == 5

z0 = ICalGLib.Timezone.array_element_at(array, 0);
assert(z0.get_location() == zone0.get_location());
z1 = ICalGLib.Timezone.array_element_at(array, 1);
assert(z1.get_location() == zone1.get_location());
z2 = ICalGLib.Timezone.array_element_at(array, 2);
assert(z2.get_location() == zone2.get_location());
z3 = ICalGLib.Timezone.array_element_at(array, 3);
assert(z3.get_location() == zone3.get_location());
z4 = ICalGLib.Timezone.array_element_at(array, 4);
assert(z4.get_location() == zone4.get_location());

array2 = array.copy();

assert array2.size() == 5

z0 = ICalGLib.Timezone.array_element_at(array2, 0);
assert(z0.get_location() == zone0.get_location());
z1 = ICalGLib.Timezone.array_element_at(array2, 1);
assert(z1.get_location() == zone1.get_location());
z2 = ICalGLib.Timezone.array_element_at(array2, 2);
assert(z2.get_location() == zone2.get_location());
z3 = ICalGLib.Timezone.array_element_at(array2, 3);
assert(z3.get_location() == zone3.get_location());
z4 = ICalGLib.Timezone.array_element_at(array2, 4);
assert(z4.get_location() == zone4.get_location());

array.remove_element_at(2);
assert array.size() == 4

z0 = ICalGLib.Timezone.array_element_at(array, 0);
assert(z0.get_location() == zone0.get_location());
z1 = ICalGLib.Timezone.array_element_at(array, 1);
assert(z1.get_location() == zone1.get_location());
z3 = ICalGLib.Timezone.array_element_at(array, 2);
assert(z3.get_location() == zone3.get_location());
z4 = ICalGLib.Timezone.array_element_at(array, 3);
assert(z4.get_location() == zone4.get_location());
