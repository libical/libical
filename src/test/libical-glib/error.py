#!/usr/bin/env python3
#GI_TYPELIB_PATH=$PREFIX/lib/girepository-1.0/ ./error.py

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

message = "This is a stinky error!";

string_rep = ICalGLib.error_strerror (ICalGLib.ErrorEnum.NEWFAILED_ERROR);
assert (string_rep == "NEWFAILED: Failed to create a new object via a *_new() routine")
string_perror = ICalGLib.error_perror();
assert (string_perror == "NO: No error");

ICalGLib.error_set_error_state (ICalGLib.ErrorEnum.NEWFAILED_ERROR, ICalGLib.ErrorState.FATAL);
state = ICalGLib.error_get_error_state (ICalGLib.ErrorEnum.NEWFAILED_ERROR);
assert (state == ICalGLib.ErrorState.FATAL);

ICalGLib.error_stop_here();
enumeration = ICalGLib.errno_return();
string_rep = ICalGLib.error_strerror (enumeration);
assert (string_rep == "NO: No error");

state = ICalGLib.error_supress (message);
ICalGLib.error_restore (message, state);
