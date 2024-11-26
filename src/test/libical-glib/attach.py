#!/usr/bin/env python3
# GI_TYPELIB_PATH=$PREFIX/lib/girepository-1.0/ ./attach.py

###############################################################################
#
# SPDX-FileCopyrightText: 2015 William Yu <williamyu@gnome.org>
#
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
#
###############################################################################

"""Test Python bindings for libical attachments"""

import gi

gi.require_version('ICalGLib', '3.0')
from gi.repository import ICalGLib  # noqa E402

dummyUrl = 'https://people.gnome.org/~engagement/logos/GnomeLogoVertical.svg'
dummyData = "It's a super large attachment"

attachUrl = ICalGLib.Attach.new_from_url(dummyUrl)
assert attachUrl.get_is_url() == 1

retrievedUrl = attachUrl.get_url()
assert retrievedUrl == dummyUrl

attach_data = ICalGLib.Attach.new_from_data(dummyData, None, None)
assert attach_data.get_is_url() == 0
retrieved_data = attach_data.get_data()
assert retrieved_data == dummyData
