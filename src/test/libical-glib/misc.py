#!/usr/bin/env python3
#GI_TYPELIB_PATH=$PREFIX/lib/girepository-1.0/ ./misc.py

###############################################################################
#
# Copyright (C) 2019 Red Hat Inc. <www.redhat.com>
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

geo = ICalGLib.Geo.new(10.0, 20.0)
assert geo.get_lat() == 10.0
assert geo.get_lon() == 20.0
geo_clone = geo.clone()
assert geo.get_lat() == geo_clone.get_lat()
assert geo.get_lon() == geo_clone.get_lon()
geo_clone.set_lat(30.0)
geo_clone.set_lon(40.0);
assert geo_clone.get_lat() == 30.0
assert geo_clone.get_lon() == 40.0
assert geo.get_lat() != geo_clone.get_lat()
assert geo.get_lon() != geo_clone.get_lon()

start = ICalGLib.Time.new_from_string("20190130T111213Z");
end = ICalGLib.Time.new_from_string("20190203T100908Z")
span = ICalGLib.TimeSpan.new(start, end, 0);
assert span.get_start() == start.as_timet()
assert span.get_end() == end.as_timet()
assert span.get_is_busy() == 0
start = ICalGLib.Time.new_from_string("20190330T131415Z");
end = ICalGLib.Time.new_from_string("20190403T070605Z")
span = ICalGLib.TimeSpan.new(start, end, 1);
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
