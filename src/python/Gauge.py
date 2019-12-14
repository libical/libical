#!/usr/bin/env python
#======================================================================
# FILE: Gauge.py
# CREATOR: mtearle
#
# (C) COPYRIGHT 2001, Eric Busboom <eric@civicknowledge.com>
# (C) COPYRIGHT 2001, Patrick Lewis <plewis@inetarena.com>
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
#======================================================================

from LibicalWrap import *
from Error import LibicalError
from Component import Component

class Gauge:
    """
    Base class for gauge
    """

    class ConstructorFailedError(LibicalError):
        "Failed to create a Guage "

    class CloneFailedError(LibicalError):
        "Failed to clone a component given Gauge "

    class CompareFailedError(LibicalError):
        "Failed to compare a component given Gauge "

    def __init__(self,ref=None,sql=None,expand=0):
        if ref != None:
            self._ref = ref
        elif sql != None:
            s = str(sql)
            self._ref = icalgauge_new_from_sql(s,expand)
        else:
            Gauge.ConstructorFailedError("No SQL Specified")

    def __del__(self):
        if self._ref != None:
            icalgauge_free(self._ref)
            self._ref = None

    def ref(self):
        return self._ref

    def compare(self, comp):
        if not isinstance(comp,Component):
            raise Gauge.CompareFailedError("Argument is not a component")

        if comp.ref() == None:
            raise Gauge.CompareFailedError("Argument is not a component")

        return icalgauge_compare(self._ref, comp.ref())
