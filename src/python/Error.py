#======================================================================
# FILE: Error.py
# CREATOR: eric
#
# SPDX-FileCopyrightText: 2001, Eric Busboom <eric@civicknowledge.com>
# SPDX-FileCopyrightText: 2001, Patrick Lewis <plewis@inetarena.com>
#
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
#
#
#


# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
#
#
#
#===========================================================

class LibicalError(Exception):
    "Libical Error"

    def __init__(self,str):
        Exception.__init__(self,str)

    def __str__(self):
        return Exception.__str__(self)+"\nLibical errno: "+icalerror_perror()
