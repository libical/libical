#!/usr/bin/env python
#======================================================================
# FILE: Period.py
# CREATOR: eric
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
#===========================================================

from LibicalWrap import *
from Property import Property
from types import DictType, StringType, IntType
from Time import Time
from Duration import Duration

class Period(Property):
    """Represent a span of time"""
    def __init__(self,arg,name='FREEBUSY'):
        """ """

        Property.__init__(self, type = name)

        self.pt=None

        #icalerror_clear_errno()
        e1=icalerror_supress("MALFORMEDDATA")
        e2=icalerror_supress("BADARG")

        if isinstance(arg, DictType):


            es=icalerror_supress("MALFORMEDDATA")
            self.pt = icalperiodtype_from_string(arg['value'])
            icalerror_restore("MALFORMEDDATA",es)

            Property.__init__(self, ref=arg['ref'])
        else:
            if isinstance(arg, StringType):

                self.pt = icalperiodtype_from_string(arg)

            else:
                self.pt = icalperiodtype_null_period()

            Property.__init__(self,type=name)

        icalerror_restore("MALFORMEDDATA",e1)
        icalerror_restore("BADARG",e2)


        if self.pt == None or icalperiodtype_is_null_period(self.pt):
            raise Property.ConstructorFailedError("Failed to construct Period")


        try:
            self._update_value()
        except Property.UpdateFailedError:
            raise Property.ConstructorFailedError("Failed to construct Period")

    def _end_is_duration(self):
        dur = self.pt.duration
        return 0 if icaldurationtype_is_null_duration(dur) else 1

    def _end_is_time(self):
        end = self.pt.end
        return 0 if icaltime_is_null_time(end) else 1

    def _update_value(self):

        self.value(icalperiodtype_as_ical_string(self.pt),"PERIOD")


    def valid(self):
        "Returns true if this is a valid period"

        return not icalperiodtype_is_null_period(self.dur)

    def start(self,v=None):
        """
        Return or set start time of the period. The start time may be
        expressed as an RFC2445 format string or an instance of Time.
        The return value is an instance of Time
        """

        if(v != None):
            if isinstance(t,Time):
                t = v
            elif isinstance(t,StringType) or isinstance(t,IntType):
                t = Time(v,"DTSTART")
            else:
                raise TypeError

            self.pt.start = t.tt

            self._update_value()


        return Time(self.pt.start.as_timet(),
                    "DTSTART")

    def end(self,v=None):
        """
        Return or set end time of the period. The end time may be
        expressed as an RFC2445 format string or an instance of Time.
        The return value is an instance of Time.

        If the Period has a duration set, but not an end time, this
        method will calculate the end time from the duration.  """

        if v != None:

            if isinstance(t,Time):
                t = v
            elif isinstance(t,StringType) or isinstance(t,IntType):
                t = Time(v)
            else:
                raise TypeError

            if(self._end_is_duration()):
                start = self.pt.start.as_timet()
                dur = t.utc_seconds()-start;
                self.pt.duration = icaldurationtype_from_int(dur)
            else:
                self.pt.end = t.tt

            self._update_value()

        if(self._end_is_time()):
            rt = Time(self.pt.end.as_timet(),
                      'DTEND')
            rt.timezone(self.timezone())
            return rt
        elif(self._end_is_duration()):
            start = self.pt.start.as_timet()
            dur = icaldurationtype_as_int(self.pt.duration)
            rt = Time(start+dur,'DTEND')
            rt.timezone(self.timezone())
            return rt
        else:
            return Time({},'DTEND')



    def duration(self,v=None):
        """
        Return or set the duration of the period. The duration may be
        expressed as an RFC2445 format string or an instance of Duration.
        The return value is an instance of Duration.

        If the period has an end time set, but not a duration, this
        method will calculate the duration from the end time.  """

        if(v != None):

            if isinstance(t,Duration):
                d = v
            elif isinstance(t,StringType) or isinstance(t,IntType):
                d = Duration(v)
            else:
                raise TypeError

            if self._end_is_time():
                start = self.pt.start.as_timet()
                end = start + d.seconds()

                self.pt.end = icaltimetype.from_timet(end)
            else:
                self.pt.duration = d.dur

        if self._end_is_time():
            start = self.pt.start.as_timet()
            end = self.pt.end.as_timet()

            print "End is time " + str(end-start)

            return Duration(end-start,"DURATION")

        if self._end_is_duration():
            dur = icaldurationtype_as_int(self.pt.duration)

            return Duration(dur,"DURATION")

        return Duration(0,"DURATION")


    def timezone(self,v=None):
        """ Return or set the timezone string for this time """
        if (v != None):
            self['TZID'] = v
        return  self['TZID']
