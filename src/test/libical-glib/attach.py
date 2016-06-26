#!/usr/bin/env python3
#GI_TYPELIB_PATH=$PREFIX/lib/girepository-1.0/ ./attach.py

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

dummy_url = "https://people.gnome.org/~engagement/logos/GnomeLogoVertical.svg";
dummy_data = "It's a super large attachment";

attach_url = ICalGLib.Attach.new_from_url(dummy_url);
assert(attach_url.get_is_url() == 1);

retrieved_url = attach_url.get_url();
assert(retrieved_url == dummy_url);

attach_data = ICalGLib.Attach.new_from_data(dummy_data, ICalGLib.memory_free_buffer, None);
assert(attach_data.get_is_url() == 0);
retrieved_data = attach_data.get_data();
assert(retrieved_data == dummy_data);
