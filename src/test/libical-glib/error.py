#!/usr/bin/env python3
# GI_TYPELIB_PATH=$PREFIX/lib/girepository-1.0/ ./error.py

###############################################################################
#
# SPDX-FileCopyrightText: 2015 William Yu <williamyu@gnome.org>
#
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
#
###############################################################################

"""Test Python bindings for libical error handling"""

import gi

gi.require_version('ICalGLib', '4.0')
from gi.repository import ICalGLib  # noqa E402

message = 'This is a stinky error!'

string_rep = ICalGLib.error_strerror(ICalGLib.ErrorEnum.NEWFAILED_ERROR)
assert string_rep == 'NEWFAILED: Failed to create a new object via a *_new() routine'
string_perror = ICalGLib.error_perror()
assert string_perror == 'NO: No error'

ICalGLib.error_set_error_state(ICalGLib.ErrorEnum.NEWFAILED_ERROR, ICalGLib.ErrorState.FATAL)
state = ICalGLib.error_get_error_state(ICalGLib.ErrorEnum.NEWFAILED_ERROR)
assert state == ICalGLib.ErrorState.FATAL

ICalGLib.error_stop_here()
enumeration = ICalGLib.errno_return()
string_rep = ICalGLib.error_strerror(enumeration)
assert string_rep == 'NO: No error'

state = ICalGLib.error_supress(message)
ICalGLib.error_restore(message, state)
