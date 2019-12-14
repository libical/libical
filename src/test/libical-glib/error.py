#!/usr/bin/env python3
#GI_TYPELIB_PATH=$PREFIX/lib/girepository-1.0/ ./error.py

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

message = "This is a stinky error!";

string_rep = ICalGLib.error_strerror(ICalGLib.ErrorEnum.NEWFAILED_ERROR);
assert(string_rep == "NEWFAILED: Failed to create a new object via a *_new() routine")
string_perror = ICalGLib.error_perror();
assert(string_perror == "NO: No error");

ICalGLib.error_set_error_state(ICalGLib.ErrorEnum.NEWFAILED_ERROR, ICalGLib.ErrorState.FATAL);
state = ICalGLib.error_get_error_state(ICalGLib.ErrorEnum.NEWFAILED_ERROR);
assert(state == ICalGLib.ErrorState.FATAL);

ICalGLib.error_stop_here();
enumeration = ICalGLib.errno_return();
string_rep = ICalGLib.error_strerror(enumeration);
assert(string_rep == "NO: No error");

state = ICalGLib.error_supress(message);
ICalGLib.error_restore(message, state);
