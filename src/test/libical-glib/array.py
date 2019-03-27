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

import sys
from gi.repository import ICalGLib
from sys import getsizeof

array = ICalGLib.Array.new(256, 100);

element1 = "hello";
element2 = "world";
element3 = "how";
element4 = "are";
element5 = "you";

#TEST APPEND
attach1 = ICalGLib.Attach.new_from_url(element1);
attach2 = ICalGLib.Attach.new_from_url(element2);
attach3 = ICalGLib.Attach.new_from_url(element3);
attach4 = ICalGLib.Attach.new_from_url(element4);
attach5 = ICalGLib.Attach.new_from_url(element5);

array.append(attach1);
array.append(attach2);
array.append(attach3);
array.append(attach4);
array.append(attach5);

a1 = array.element_at(0);
assert(a1 == attach1);
a2 = array.element_at(1);
assert(a2 == attach2);
a3 = array.element_at(2);
assert(a3 == attach3);
a4 = array.element_at(3);
assert(a4 == attach4);
a5 = array.element_at(4);
assert(a5 == attach5);

array = array.copy();
a1 = array.element_at(0);
assert(a1 == attach1);
a2 = array.element_at(1);
assert(a2 == attach2);
a3 = array.element_at(2);
assert(a3 == attach3);
a4 = array.element_at(3);
assert(a4 == attach4);
a5 = array.element_at(4);
assert(a5 == attach5);

array.remove_element_at(2);
a3 = array.element_at(2);
assert(a3 == attach4);
