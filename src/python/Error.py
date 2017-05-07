#======================================================================
# FILE: Error.py
# CREATOR: eric
#
# (C) COPYRIGHT 2001, Eric Busboom <eric@softwarestudio.org>
# (C) COPYRIGHT 2001, Patrick Lewis <plewis@inetarena.com>
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


# This library is free software; you can redistribute it and/or modify
# it under the terms of either:
#
#    The LGPL as published by the Free Software Foundation, version
#    2.1, available at: http://www.gnu.org/licenses/lgpl-2.1.html
#
#  Or:
#
#    The Mozilla Public License Version 2.0. You may obtain a copy of
#    the License at http://www.mozilla.org/MPL/
#===========================================================

class LibicalError(Exception):
    "Libical Error"

    def __init__(self,str):
        Exception.__init__(self,str)

    def __str__(self):
        return Exception.__str__(self)+"\nLibical errno: "+icalerror_perror()
