#!/usr/bin/env python3
# GI_TYPELIB_PATH=$PREFIX/lib/girepository-1.0/ ./misc.py

###############################################################################
# SPDX-FileCopyrightText: 2019 Red Hat Inc. <www.redhat.com>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
###############################################################################

"""Test Python bindings for libical miscellaneous types"""

import gi

gi.require_version('ICalGLib', '4.0')
from gi.repository import ICalGLib  # noqa E402

geo = ICalGLib.Geo.new(10.0, 20.0)
assert geo.get_lat() == 10.0
assert geo.get_lon() == 20.0
geo_clone = geo.clone()
assert geo.get_lat() == geo_clone.get_lat()
assert geo.get_lon() == geo_clone.get_lon()
geo_clone.set_lat(30.0)
geo_clone.set_lon(40.0)
assert geo_clone.get_lat() == 30.0
assert geo_clone.get_lon() == 40.0
assert geo.get_lat() != geo_clone.get_lat()
assert geo.get_lon() != geo_clone.get_lon()

start = ICalGLib.Time.new_from_string('20190130T111213Z')
end = ICalGLib.Time.new_from_string('20190203T100908Z')
span = ICalGLib.TimeSpan.new(start, end, 0)
assert span.get_start() == start.as_timet()
assert span.get_end() == end.as_timet()
assert span.get_is_busy() == 0
start = ICalGLib.Time.new_from_string('20190330T131415Z')
end = ICalGLib.Time.new_from_string('20190403T070605Z')
span = ICalGLib.TimeSpan.new(start, end, 1)
assert span.get_start() == start.as_timet()
assert span.get_end() == end.as_timet()
assert span.get_is_busy() == 1

span = ICalGLib.TimeSpan.new_timet(11, 22, 1)
assert span.get_start() == 11
assert span.get_end() == 22
assert span.get_is_busy() == 1
span_clone = span.clone()
assert span.get_start() == span_clone.get_start()
assert span.get_end() == span_clone.get_end()
assert span.get_is_busy() == span_clone.get_is_busy()
span_clone.set_start(33)
span_clone.set_end(44)
span_clone.set_is_busy(0)
assert span_clone.get_start() == 33
assert span_clone.get_end() == 44
assert span_clone.get_is_busy() == 0
assert span.get_start() != span_clone.get_start()
assert span.get_end() != span_clone.get_end()
assert span.get_is_busy() != span_clone.get_is_busy()
span = ICalGLib.TimeSpan.new_timet(55, 66, 0)
assert span.get_start() == 55
assert span.get_end() == 66
assert span.get_is_busy() == 0

limits_kinds = [
    ICalGLib.LimitsKind.PARSE_FAILURES,
    ICalGLib.LimitsKind.PARSE_SEARCH,
    ICalGLib.LimitsKind.PARSE_FAILURE_ERROR_MESSAGES,
    ICalGLib.LimitsKind.PROPERTIES,
    ICalGLib.LimitsKind.PARAMETERS,
    ICalGLib.LimitsKind.VALUE_CHARS,
    ICalGLib.LimitsKind.PROPERTY_VALUES,
    ICalGLib.LimitsKind.RECURRENCE_SEARCH,
    ICalGLib.LimitsKind.RECURRENCE_TIME_STANDING_STILL,
    ICalGLib.LimitsKind.RRULE_SEARCH,
]
old_limits = []

for limit_kind in limits_kinds:
    old_limit = ICalGLib.limit_get(limit_kind)
    old_limits.append(old_limit)
    assert old_limit != 0

# limit_diff is used to recognize changes
limit_diff = 3

# verify it does not clash with other default limits
for i in range(len(old_limits)):
    for j in range(len(old_limits)):
        if i == j:
            continue
        assert old_limits[i] + limit_diff != old_limits[j]

# verify each can be changed and it does not influence the others
for i in range(len(old_limits)):
    ICalGLib.limit_set(limits_kinds[i], old_limits[i] + limit_diff)
    assert old_limits[i] + limit_diff == ICalGLib.limit_get(limits_kinds[i])

    for j in range(len(old_limits)):
        if i == j:
            continue
        assert old_limits[j] == ICalGLib.limit_get(limits_kinds[j])

    ICalGLib.limit_set(limits_kinds[i], old_limits[i])
    assert old_limits[i] == ICalGLib.limit_get(limits_kinds[i])
