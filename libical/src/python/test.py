#!/usr/bin/env python 
# -*- Mode: python -*-
#======================================================================
# FILE: test.py
# CREATOR: eric 
#
# DESCRIPTION:
#   
#
#  $Id: test.py,v 1.9 2001-03-04 18:47:30 plewis Exp $
#  $Locker:  $
#
# (C) COPYRIGHT 2001, Eric Busboom <eric@softwarestudio.org>
# (C) COPYRIGHT 2001, Patrick Lewis <plewis@inetarena.com>  
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of either: 
#
#    The LGPL as published by the Free Software Foundation, version
#    2.1, available at: http://www.fsf.org/copyleft/lesser.html
#
#  Or:
#
#    The Mozilla Public License Version 1.0. You may obtain a copy of
#    the License at http://www.mozilla.org/MPL/
#======================================================================

from Libical import *


def test_property():

    print "--------------------------- Test Property ----------------------"
    
    liw = LibicalWrap
    icalprop = liw.icalproperty_new_from_string("ATTENDEE;RSVP=TRUE;ROLE=REQ-PARTICIPANT;CUTYPE=GROUP:MAILTO:employee-A@host.com")

    print liw.icalproperty_as_ical_string(icalprop)

    p = Property(ref=icalprop)

    print p.name()
    print p.parameters()
    print p['ROLE']
    
    p['ROLE'] = 'INDIVIDUAL'

    print p['ROLE']

    print p.value()
    p.value("mailto:Bob@bob.com")
    print p.value()


    print p.as_ical_string()

def test_time():
    "Test routine"

    print"-------------------Test Time  --------------------------------"

    t = Time("19970325T123010Z",'DTSTART')
    
    assert(t.year() == 1997)
    assert(t.month() == 3)
    assert(t.day() == 25)
    assert(t.hour() == 12)
    assert(t.minute() == 30)
    assert(t.second() == 10)
    assert(t.is_utc())
    assert(not t.is_date())
    
    print t

    t.timezone("America/Los_Angeles")
    print str(t)
    assert(str(t)=='DTSTART;TZID=America/Los_Angeles:19970325T123010Z')

    t.second(t.second()+80)

    print t

    assert(t.minute() == 31)
    assert(t.second() == 30)

    d = Duration(3600,"DURATION")
    t2 = t + d

    print t2
    assert(t2.hour() == 13)

    t2 = t - d

    print t2
    assert(isinstance(t2,Time))
    assert(t2.hour() == 11)


def test_period():    

    print"-------------------Test Period--------------------------------"

    p = Period("19970101T180000Z/19970101T233000Z")

    print p
    
    assert(str(p) == 'FREEBUSY:19970101T180000Z/19970101T233000Z')

    print p.start()
    assert(str(p.start()) == 'DTSTART:19970101T180000Z')

    print p.end()
    assert(str(p.end()) == 'DTEND:19970101T233000Z')

    print p.duration()
    assert(str(p.duration()) == 'DURATION:PT5H30M')

    p = Period("19970101T180000Z/PT5H30M")

    print p

    print p.start()
    assert(str(p.start()) == 'DTSTART:19970101T180000Z')

    print p.end()
    assert(str(p.end()) == 'DTEND:19970101T233000Z')

    print p.duration()
    assert(str(p.duration()) == 'DURATION:PT5H30M')


def test_attach():

    file = open('littlefile.txt')
    attachProp = Attach(file)
    file.close()
    attachProp.fmttype('text/ascii')
    print "\n" + attachProp.name()
    print attachProp.value_type()
    print attachProp.fmttype()
    attachProp['fmttype']=None
    print "Calling value()"
    print attachProp.value()
    print "Calling asIcalString()"
    print attachProp.as_ical_string()


def test_component():

    print "------------------- Test Component ----------------------"

    comp_str = """
BEGIN:VEVENT
ATTENDEE;RSVP=TRUE;ROLE=REQ-PARTICIPANT;CUTYPE=GROUP:MAILTO:employee-A@host.com
COMMENT: When in the course of writting comments and nonsense text\, it 
 becomes necessary to insert a newline
DTSTART:19972512T120000
DTSTART:19970101T120000Z
DTSTART:19970101
DURATION:P3DT4H25M
FREEBUSY:19970101T120000/19970101T120000
FREEBUSY:19970101T120000/PT3H
FREEBUSY:19970101T120000/PT3H
END:VEVENT"""


    c = Component(comp_str);
    
    props = c.properties()
    
    for p in props: 
        print p.as_ical_string()
        
    dtstart = c.properties('DTSTART')[0]
        
    print dtstart
    
    print "\n Orig hour: ", dtstart.hour()
    assert(dtstart.hour() == 12)

    dtstart.hour(dtstart.hour() + 5)

    print "\n New hour: ", dtstart.hour()
    assert(dtstart.hour() == 17)

    attendee = c.properties('ATTENDEE')[0]
    
    print attendee

    t = Time("20011111T123030")
    t.name('DTEND')

    c.addProperty(t)


    print c

    dtstart1 = c.properties('DTSTART')[0]
    dtstart2 = c.properties('DTSTART')[0]

    assert(dtstart1 is dtstart2)
    
    assert(dtstart1 == dtstart2)


if __name__ == "__main__":
    test_property()

    test_time()

    test_period()

    test_component()

    #test_attach()


