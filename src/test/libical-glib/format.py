#!/usr/bin/env python3
#GI_TYPELIB_PATH=$PREFIX/lib/girepository-1.0/ ./format.py

###############################################################################
#
# Copyright 2021 Corentin Noël <corentin.noel@collabora.com>
# Copyright 2021 Collabora, Ltd. (https://collabora.com)
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
from gi.repository import GLib

icaltime = ICalGLib.Time.new_from_string("20181224T000000Z")
glibdatetime = GLib.DateTime.new_from_iso8601("20181224T000000Z")

assert(glibdatetime.to_unix() == icaltime.as_timet());

test_format = "a%a A%A b%b B%B c%c C%C d%d e%e F%F g%g G%G h%h H%H I%I j%j m%m M%M n%n p%p r%r R%R S%S t%t T%T u%u V%V w%w x%x X%X y%y Y%Y z%z Z%Z %%"
# 127997 is prime, 1315005118 is now
for t in range(0, 1315005118, 127997):
    glibdatetime = GLib.DateTime.new_from_unix_utc(t)
    icaltime = ICalGLib.Time.new_from_timet_with_zone(t, 0, None)
    assert(glibdatetime.format(test_format) == icaltime.format(test_format))
