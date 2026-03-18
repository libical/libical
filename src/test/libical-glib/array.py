#!/usr/bin/env python3
# GI_TYPELIB_PATH=$PREFIX/lib/girepository-1.0/ ./array.py

###############################################################################
# SPDX-FileCopyrightText: 2015 William Yu <williamyu@gnome.org>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
###############################################################################

"""Test Python bindings for libical arrays"""

import os
import sys

import gi

gi.require_version('ICalGLib', '4.0')
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
