#!/usr/bin/env python3
# GI_TYPELIB_PATH=$PREFIX/lib/girepository-1.0/ ./timezone.py

###############################################################################
#
# SPDX-FileCopyrightText: 2015 William Yu <williamyu@gnome.org>
#
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
#
###############################################################################

"""Test Python bindings for libical timezone"""

import sys
import os

import gi

gi.require_version('ICalGLib', '3.0')
from gi.repository import ICalGLib  # noqa E402

try:
    zoneinfodir = os.environ['ZONEINFO_DIRECTORY']
except KeyError:
    print("Error: The ZONEINFO_DIRECTORY environment variable isn't set")
    sys.exit(1)
if not os.path.isdir(zoneinfodir):
    print("Error: The ZONEINFO_DIRECTORY environment variable isn't properly set")
    sys.exit(1)
ICalGLib.Timezone.set_zone_directory(zoneinfodir)
ICalGLib.Timezone.set_tzid_prefix('/citadel.org/')

la = ICalGLib.Timezone.get_builtin_timezone('America/Los_Angeles')
chicago = ICalGLib.Timezone.get_builtin_timezone('America/Chicago')
assert la.get_tzid().find('Los_Angeles') != -1
assert la.get_location() == 'America/Los_Angeles'
assert la.get_tznames() == 'PST/PDT'

la_copy = la.copy()
assert la_copy.get_tzid() == la.get_tzid()
assert la_copy.get_location() == la.get_location()

timezones = ICalGLib.Timezone.get_builtin_timezones()
timezone = ICalGLib.Timezone.array_element_at(timezones, 0)
if not ICalGLib.Timezone.get_builtin_tzdata():
    assert timezone.get_display_name() == 'Europe/Andorra'
assert timezones.size() > 0

from_tzid = ICalGLib.Timezone.get_builtin_timezone_from_tzid(la.get_tzid())
assert from_tzid.get_location() == 'America/Los_Angeles'

utc = ICalGLib.Timezone.get_utc_timezone()
assert utc.get_display_name() == 'UTC'
utc2 = ICalGLib.Timezone.get_utc_timezone()
assert utc == utc2

time = ICalGLib.Time.new()
before = time.get_hour()
ICalGLib.Time.convert_timezone(time, la, chicago)
after = time.get_hour()
assert abs(after - before) == 2
ICalGLib.Time.set_timezone(time, utc)
assert ICalGLib.Time.get_timezone(time) == utc
ICalGLib.Time.set_timezone(time, la)
assert ICalGLib.Time.get_timezone(time) == la

timeclone = time.clone()
assert time != timeclone
time = ICalGLib.Time.convert_to_zone(time, chicago)
timeclone.convert_to_zone_inplace(chicago)
assert time.get_year() == timeclone.get_year()
assert time.get_month() == timeclone.get_month()
assert time.get_day() == timeclone.get_day()
assert time.get_hour() == timeclone.get_hour()
assert time.get_minute() == timeclone.get_minute()
assert time.get_second() == timeclone.get_second()
assert time.get_timezone() == timeclone.get_timezone()
assert time.is_date() == timeclone.is_date()
assert time.is_daylight() == timeclone.is_daylight()
assert time.is_utc() == timeclone.is_utc()

timeclone = ICalGLib.Time.clone(time)
assert time != timeclone
assert ICalGLib.Time.get_year(time) == ICalGLib.Time.get_year(timeclone)
assert ICalGLib.Time.get_month(time) == ICalGLib.Time.get_month(timeclone)
assert ICalGLib.Time.get_day(time) == ICalGLib.Time.get_day(timeclone)
assert ICalGLib.Time.get_hour(time) == ICalGLib.Time.get_hour(timeclone)
assert ICalGLib.Time.get_minute(time) == ICalGLib.Time.get_minute(timeclone)
assert ICalGLib.Time.get_second(time) == ICalGLib.Time.get_second(timeclone)
assert ICalGLib.Time.get_timezone(time) == ICalGLib.Time.get_timezone(timeclone)
assert ICalGLib.Time.is_date(time) == ICalGLib.Time.is_date(timeclone)
assert ICalGLib.Time.is_daylight(time) == ICalGLib.Time.is_daylight(timeclone)
assert ICalGLib.Time.is_utc(time) == ICalGLib.Time.is_utc(timeclone)

time.set_date(2019, 1, 24)
assert time.get_year() == 2019
assert time.get_month() == 1
assert time.get_day() == 24

array = time.get_date()
assert len(array) == 3
assert array[0] == 2019
assert array[1] == 1
assert array[2] == 24

time.set_time(12, 11, 10)
assert time.get_hour() == 12
assert time.get_minute() == 11
assert time.get_second() == 10

array = time.get_time()
assert len(array) == 3
assert array[0] == 12
assert array[1] == 11
assert array[2] == 10

component = la.get_component()
timezone = ICalGLib.Timezone.new()
timezone.set_component(ICalGLib.Component.clone(component))
assert timezone.get_location() == la.get_location()

array = ICalGLib.Timezone.array_new()
ICalGLib.Timezone.array_append_from_vtimezone(array, ICalGLib.Component.clone(la.get_component()))
ICalGLib.Timezone.array_append_from_vtimezone(array, ICalGLib.Component.clone(chicago.get_component()))
assert array.size() == 2
timezone1 = ICalGLib.Timezone.array_element_at(array, 0)
assert timezone1.get_display_name() == la.get_display_name()
timezone2 = ICalGLib.Timezone.array_element_at(array, 1)
assert timezone2.get_display_name() == chicago.get_display_name()

ICalGLib.Timezone.free_builtin_timezones()
ICalGLib.Object.free_global_objects()
