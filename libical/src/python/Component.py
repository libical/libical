#!/usr/bin/env python 
# -*- Mode: python -*-
#======================================================================
# FILE: Component.py
# CREATOR: eric 
#
# DESCRIPTION:
#   
#
#  $Id: Component.py,v 1.1 2001-03-04 18:47:30 plewis Exp $
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

from LibicalWrap import *
from types import *
from Property import *

class Component:

    def __init__(self,str=None):
        self.comp_p = None
        if str != None:
            self.comp_p = icalparser_parse_string(str)
        else:
            self.comp_p = icalparser_parse_string('')
       
        self.cached_props = {}

    def __del__(self):
        if self.comp_p != None and \
           icalcomponent_get_parent(self.comp_p) != None:

            for k in self.cached_props.keys():
                del self.cached_props[k]
            
            icalcomponent_free(self.comp_p)
            self.comp_p = None

    def _parse_component_string(self, comp_str):
        comp = icalparser_parse_string(comp_str)
        #c = icallangbind_get_first_component(comp, 'ANY')
        print icalcomponent_as_ical_string(comp)

        # Parse properties for this component
        anyProperty = icalenum_string_to_property_kind('ICAL_ANY_PROPERTY')
        #print comp
        #print icalenum_property_kind_to_string(anyKind)
        p = icalcomponent_get_first_property(comp, anyProperty)

        while p!='NULL':
            p_dict = icallangbind_property_eval_string(p,":")
            print "p_dict = ", p_dict
            if p_dict==None: p_dict="{'name':'X-DUM', 'value':'Err', 'value_type':'TEXT'}"
            p_dict = eval(p_dict)
            print p_dict
            self._addPropertyFromDict(p_dict)
            p = icalcomponent_get_next_property(comp, anyProperty)

        # Get remaining components
        c = icallangbind_get_first_component(comp, 'ANY')
        while c!='NULL':
            self.addComponent(NewComponent(c))
            c = icallangbind_get_next_component(comp, 'ANY')
            
        
        
    def properties(self,type='ANY'): 
        """  
        Return a list of Property instances, each representing a
        property of the type 'type.'
        """

        props = []

        p = icallangbind_get_first_property(self.comp_p,type)

        while p !='NULL':
            d_string = icallangbind_property_eval_string(p,":")
            d = eval(d_string)
            d['ref'] = p

            if not self.cached_props.has_key(p):

                if d['value_type'] == 'DATE-TIME' or d['value_type'] == 'DATE':
                    prop = Time(d,)
                elif d['value_type'] == 'PERIOD':
                    prop = Period(d)
                elif d['value_type'] == 'DURATION':
                    prop = Duration(d)
                elif d['name'] == 'ATTACH':
                    prop = Attach(d)
                elif d['name'] == 'ATTENDEE':
                    prop = Attendee(d)
                else:
                    print "Unknown property type",d['name']
                    
                    
                self.cached_props[p] = prop

            prop =  self.cached_props[p]

            props.append(prop)

            p = icallangbind_get_next_property(self.comp_p,type)

        return props
 
    def addPropertyFromString(self, propertyStr):
        """
        Add a Property instance to the component. 
        """

        p = icalproperty_new_from_string(propertyStr);

        if(p == None):
            # Error, failed to create property
            raise ValueError, "unable to create a new property."
        else:
            p_dict = icallangbind_property_eval_string(p, ":")
            p_dict = eval(p_dict)
            self._addPropertyFromDict(p_dict)

    def addProperty(self, prop):
        "Adds the property object to the component."
        
        if not isinstance(prop,Property):
            raise TypeError

        prop_p = prop.ref()

        if not prop_p:
            s = str(prop)
            prop_p = icalproperty_new_from_string(s)

            if prop_p == 'NULL':
                raise "Bad property string: " + s

            prop.ref(prop_p)

        if icalproperty_get_parent(prop_p)=='NULL':
            icalcomponent_add_property(self.comp_p, prop_p)
        elif  icalproperty_get_parent(prop_p) != self.comp_p:
            raise "Property is already a child of another component"


    def removeProperty(self,prop):

        if prop.ref() and self.cached_props.kas_key(prop.ref()):
            del self.cached_props[prop.ref]
        
        # More to Come...

        pass

    def removeProperties(self, type):
        """Remove all properties with the name equal to the argument name.

        Passing "ANY" as the type will remove all Properties.
        """
        to_remvoe = self.properties(type)
        for p in to_remove:
            self.removeProperty(p)

    def addComponent(self, componentObj):
        "Adds a child component."
        pass
        
    def components(self,type='ANY'):        
        props = []

        return props

    def __str__(self):

        return icalcomponent_as_ical_string(self.comp_p)

    def remove_component(self, component):
        pass

def NewComponent(comp):
    "Converts a string or C icalcomponent into the right component object."

    wasStr=0 # Were we passed a string or an icalcomponent?

    if isinstance (comp, StringType):
        compStr = comp
        comp = icalparser_parse_string(comp)
        wasStr=1
    else:
        compStr = icalcomponent_as_ical_string(comp)

    kind = icalcomponent_isa(comp)
    kindStr = icalenum_component_kind_to_string(kind)
    # Do I need to free kind? (I think not).

    if kindStr == 'VEVENT':
        newComp = Event(compStr)
    elif kindStr == 'VTODO':
        newComp = Todo(compStr)
    elif kindStr == 'VJOURNAL':
        newComp = Journal(compstr)
    else:
        newComp = Component(compStr)

    # I don't think I need to free the component created when passed a string,
    # as it wasn't created with a _new function.

    return newComp


class GenericComponent(Component):

    def __init__(self, str=None):
                
        Component.__init__(self, str)
        self._recurrence_set=None

    def _singular_property(self, name, value_type, value=None):
        """Sets or gets the value of a method which exists once per Component.

        This is a constructor method for properties without a strictly defined
        object."""
        
        curr_properties = self.properties(propType)
        if value==None:
            if len(curr_properties) == 0:
                return None
            elif len(curr_properties) == 1:
                return curr_properties[0].value()
            else:
                raise ValueError, "too many properties of type %s" % propType
        else:
            if len(curr_properties) == 0:
                p = Property(name)
                p.value_type(value_type)
                p.value(value)
                self.addProperty(Property(dict))
            elif len(curr_properties) == 1:
                curr_properties[0].value(value)
            else:
                raise ValueError, "too many properties of type %s" % propType
            
    def method(self, v=None):
        "Sets or returns the value of the METHOD property."
        self._singular_property("METHOD", "TEXT", v)

    def prodid(self, v=None):
        "Sets or returns the value of the PRODID property."
        self._singular_property("PRODID", "TEXT", v)

    def calscale(self, v=None):
        "Sets or returns the value of the CALSCALE property."
        self._singular_property("CALSCALE", "TEXT", v)

    def class_prop(self, v=None):  # Class is a reserved word
        "Sets or returns the value of the CLASS property."
        if v!=None:
            v = upper(v)
        self._singular_property('CLASS', 'TEXT', v)

    def created(self, v=None):
        """Sets or returns the value of the CREATED property.

        Usage:
        created(time_obj)           # Set the value using a Time object
        created('19970101T123000Z') # Set using an iCalendar string
        created(982362522)          # Set using seconds 
        created()                   # Return an iCalendar string
        """
        if isinstance(v, StringType) or  isinstance(value, IntType) or     \
           isinstance(value, FloatType):
            v = Time(v)
        if isinstance(value, Time):
            self.removeProperties('CREATED')
            self.addProperty(v)
        elif v==None:
            self._singlularProperty("CREATED", "DATE-TIME", v)
        else:
            raise TypeError, "%s is an invalid type for CREATED property"\
                  % str(type(v))
        
    def description(self, v=None):
        "Sets or returns the value of the DESCRIPTION property."
        self._singular_property("DESCRIPTION", "TEXT", v)

    def dtstamp(self, v=None):
        """Sets or returns the value of the DTSTAMP property.

        Usage:
        dtstamp(time_obj)          # Set the value using a Time object
        dtstamp('19970101T123000Z')# Set using an iCalendar string
        dtstamp(982362522)         # Set using seconds 
        dtstamp()                  # Return an iCalendar string
        """
        if isinstance(v, StringType) or  isinstance(value, IntType) or     \
           isinstance(value, FloatType):
            v = Time(v)
        if isinstance(value, Time):
            self.removeProperties('DTSTAMP')
            self.addProperty(v)
        elif v==None:
            self._singular_property("DTSTAMP", "DATE-TIME", v)
        else:
            raise TypeError, "%s is an invalid type for LAST-MODIFIED property"\
                  % str(type(v))

    def dtstart(self, v=None):
        """Sets or returns the value of the DTSTART property.

        Usage:
        dtstart(time_obj)           # Set the value using a Time object
        dtstart('19970101T123000Z') # Set the value as an iCalendar string
        dtstart(982362522)          # Set the value using seconds (time_t)
        dtstart()                   # Return the time as an iCalendar string
        """
        if isinstance(v, StringType) or isinstance(v, IntType) or \
           isinstance(v, FloatType):
            v = Time(v)
        if isinstance(v, Time):
            self.removeProperties("DTSTART")
            self.addProperty(v)
        elif v==None:
            self._singular_property("DTSTART", "DATE-TIME", v)
        else:
            raise TypeError, "%s is an invalid type for DTSTART property" % \
                  str(type(v))

    def last_modified(self, v=None):
        """Sets or returns the value of the LAST-MODIFIED property.

        Usage:
        lastmodified(time_obj)          # Set the value using a Time object
        lastmodified('19970101T123000Z')# Set using an iCalendar string
        lastmodified(982362522)         # Set using seconds 
        lastmodified()                  # Return an iCalendar string
        """
        if isinstance(v, StringType) or  isinstance(value, IntType) or     \
           isinstance(value, FloatType):
            v = Time(v)
        if isinstance(value, Time):
            self.removeProperties('LAST-MODIFIED')
            self.addProperty(v)
        elif v==None:
            self._singular_property("LAST-MODIFIED", "DATE-TIME", v)
        else:
            raise TypeError, "%s is an invalid type for LAST-MODIFIED property"\
                  % str(type(v))

    def organizer(self, v=None):
        """Sets or gets the value of the ORGANIZER property.

        Usage:
        organizer(orgObj)              # Set value using an organizer object
        organizer('MAILTO:jd@not.com') # Set value using a CAL-ADDRESS string
        organizer()                    # Return a CAL-ADDRESS string
        """
        if isinstance(v, StringType):
            v = Organizer({'value':v})
        if isinstance(v, Organizer):
            self.removeProperties('ORGANIZER')
            self.addProperty(v)
        elif v==None:
            self._singular_property('ORGANIZER', 'CAL-ADDRESS', v)
        else:
            raise TypeError, "%s is not a string or Organizer object." % str(v)

    def recurrence_id(self, v=None):
        """Sets or gets the value for the RECURRENCE-ID property.

        Usage:
        recurrence_id(recIdObj)             # Set using a Recurrence_Id object
        recurrence_id("19700801T133000")    # Set using an iCalendar string
        recurrence_id(8349873494)           # Set using seconds from epoch
        recurrence_id()                     # Return an iCalendar string
        """
        if isinstance(v, StringType) or isinstance(v, IntType) or \
                                        isinstance(v, FloatType):
            v = Recurrence_Id(v)
        if isinstance(v, Recurrence_Id):
            self.removeProperties('RECURRENCE-ID')
            self.addProperty(v)
        elif v==None:
            self._singular_property('RECURRENCE-ID', 'DATE-TIME', v)
        else:
            raise TypeError, "%s is not a valid type for recurrence_id" % \
                  str(v)

    def sequence(self, v=None):
        """Sets or gets the SEQUENCE value of the Event.

        Usage:
        sequence(1)     # Set the value using an integer
        sequence('2')   # Set the value using a string containing an integer
        sequence()      # Return an integer
        """
        if isinstance(v, StringType):
            v = int(str)
        return self._singular_property('SEQUENCE', 'INTEGER', v)

    def summary(self, v=None):
        "Sets or gets the SUMMARY value of the Event."
        self._singular_property('SUMMARY', 'TEXT', v)

    def uid(self, v=None):
        "Sets or gets the UID of the Event."
        self._singular_property('UID', 'TEXT', v)

    def url(self, v=None):
        """Sets or returns the URL property."""
        self._singular_property('URL', 'URI', v)

    ####
    # Not quite sure if this is how we want to handle recurrence rules, but
    # this is a start.
    
    def recurrence_set(self):
        "Returns the Events RecurrenceSet object."
        if self._recurrence_set == None:  # i.e haven't initialized one
            self._recurrence_set = RecurrenceSet()
        return self._recurrence_set

    ###
    # Alarm interface.  Returns an ComponentCollection.

    def alarms(self, values=None):
        """Sets or returns ALARM components.

        Examples:
        alarms((alarm1,))   # Set using Alarm component
        alarms()            # Returns an ComponentCollection of all Alarms
        """
        if values!=None:
            for alarm in values:
                self.addComponent(alarm)
        else:
            return ComponentCollection(self, self.components('VALARM'))

    ####
    # Methods that deal with Properties that can occur multiple times are below.
    # They use the Collection class to return their Properties

    def _multiple_properties(self, name, value_type, values):
        "Handles generic cases for Properties that can have multiple instances."
        if values!=None:
            if not isinstance(value, TupleType) \
               and not isinstance(value, ListType):
                raise TypeError, "%s is not a tuple or list."
            self.removeProperties(name)
            for v in values:
                new_prop= Property(name)
                new_prop.value_type(value_type)
                new_prop.value(v)
                self.addProperty(new_prop)
        else:
            return Collection(self, self.properties(name))

    def attachments(self, values=None):
        """Sets or returns a Collection of Attach properties.

        'values' can be a sequence containing URLs (strings) and/or file-ish
        objects.
        """
        if values:
            if not isinstance(values, TupleType) \
               and not isinstance(values, ListType):
                raise TypeError, "%s is not a tuple or list."
            self.removeProperties('ATTACH')
            for att in value:
                if isinstance(att, StringType) or hasattr(att, 'read'):
                    att = Attach(att)
                if isinstance(att, Attach):
                    self.addProperty(att)
                else:
                    raise TypeError, \
                          "%s is not a string or Attach object." % str(att)
        else:
            return Collection(self, self.properties('ATTACH'))

    def attendees(self, value=None):
        """Sets attendees or returns a Collection of Attendee objects.

        If setting the attendees, pass a sequence as the argument.
        Examples:
        # Set using Attendee objects
        attendees((attObj1, attObj2))
        # Set using a CAL-ADDRESS string
        attendees(['MAILTO:jdoe@somewhere.com'])
        # Set using a combination of Attendee objects and strings
        attendees(['MAILTO:jdoe@somewhere.com', attObj1])
        # Returns a list of Attendee objects
        attendees()

        When setting the attendees, any previous Attendee objects in the Event
        are overwritten.  If you want to add to the Attendees, one way to do it
        is:

        attendees().append(Attendee('MAILTO:jdoe@nothere.com'))
        """
        if value:
            if not isinstance(value, TupleType) \
               and not isinstance(value, ListType):
                raise TypeError, "%s is not a tuple or list."
            self.removeProperties('ATTENDEE')
            for att in value:
                if isinstance(att, StringType):
                    att = Attendee(att)
                if isinstance(att, Attendee):
                    self.addProperty(att)
                else:
                    raise TypeError, \
                          "%s is not a string or Attendee object." % str(att)
        else:
            return Collection(self, self.properties('ATTENDEE'))

    def categories(self, value=None):
        """Sets categories or returns a Collection of CATEGORIES properties.

        If setting the categories, pass a sequence as the argument.
        Examples:
        # Set using string[s]
        categories(('APPOINTMENT', 'EDUCATION'))
        # Returns a list of Category properites
        categories()

        When setting the attendees, any previous category Properties in the
        Event are overwritten.  If you want to add to the categories, one way to
        do it is:

        new_cat=Property("CATEGORIES")
        new_cat.value_type('TEXT')
        new_cat.value('PERSONAL')
        categories().append(new_cat)
        """
        self._multiple_properties("CATEGORIES", "TEXT", value)

    def comments(self, value=None):
        "Sets or returns a Collection of COMMENT properties."
        self._multiple_properties('COMMENT', 'TEXT', value)

    def contacts(self, value=None):
        "Sets or returns a Collection of CONTACT properties."
        self._multiple_properties('CONTACT', 'TEXT', value)

    def related_tos(self, value=None):
        "Sets or returns a Collectoin of RELATED-TO properties."
        self._multiple_properties('RELATED-TO', 'TEXT', value)


class Event(GenericComponent):
    "The iCalendar Event object."
        
    def component_type(self):
        "Returns the type of component for the object."
        return "VEVENT"

    def clone(self):
        "Returns a copy of the object."
        return Event(self.asIcalString())

    def dtend(self, v=None):
        """Sets or returns the value of the DTEND property.

        Usage:
        dtend(time_obj)             # Set the value using a Time object
        dtend('19970101T123000Z')   # Set the value as an iCalendar string
        dtend(982362522)            # Set the value using seconds (time_t)
        dtend()                     # Return the time as an iCalendar string

        If the dtend value is being set and duration() has a value, the
        duration property will be removed.
        """
        if isinstance(v, StringType) or isinstance(v, IntType) \
           or isinstance(v, FloatType):
            v = Time(v)
        if isinstance(v, Time):
            self.removeProperties("DTEND")
            self.removeProperites("DURATION") # Clear DURATION properties
            self.addProperty(v)
        elif v==None:
            self._singular_property("DTEND", "DATE-TIME", v)                
        else:
            raise TypeError, "%s is an invalid type for DTEND property" % \
                  str(type(v))
            
    def duration(self, v=None):
        """Sets or returns the value of the duration property.

        Usage:
        duration(dur_obj)       # Set the value using a Duration object
        duration("P3DT12H")     # Set value as an iCalendar string
        duration(3600)          # Set duration using seconds
        duration()              # Return duration as an iCalendar string

        If the duration value is being set and dtend() has a value, the dtend
        property will be removed.
        """

        if isinstance(v, StringType) or isinstance(v, IntType):
            v = Duration(v)
        if isinstance(v, Duration):
            self.removeProperties("DURATION")
            self.removeProperties("DTEND")   # Clear DTEND properties
        elif v==None:
            self._singular_property("DURATION", "DURATION", v)
        else:
            raise TypeError, "%s is an invalid type for duration property" % \
                  str(type(v))

    def status(self, v=None):
        "Sets or returns the value of the STATUS property."

        if v:
            v=upper(v)
            # These values are only good for VEVENT components (i.e. don't copy
            # & paste into VTODO or VJOURNAL
            valid_values=('TENTATIVE', 'CONFIRMED', 'CANCELLED')
            if v not in valid_values:
                raise ValueError, "%s is not one of %s" \
                      % (str(v), str(valid_values))
            
        self._singular_property("STATUS", "TEXT", v)

    def geo(self, v=None):
        """Sets or returns the value of the GEO property.

        Usage:
        geo(value) or
        geo()           # Returns the icalendar string

        'value' is either a icalendar GEO string or a sequence with two 'float'
        numbers.

        Examples:
        geo('40.232;-115.9531')     # Set value using string
        geo((40.232, -115.9531))    # Set value using a sequence
        geo()                       # Returns "40.232;-115.9531"

        To get the GEO property represented as a tuple and numbers instead of
        the iCalendar string, use geo_get_tuple().
        """
        
        if isinstance(v, ListType) or isinstance(v, TupleType):
            v = "%s;%s" % (float(v[0]), float(v[1]))
        self._singular_property("GEO", "FLOAT", v)

    def geo_get_tuple(self):
        """Returns the GEO property as a tuple."""

        geo = self.geo()
        geo = split(geo, ';')
        return float(geo[0]), float(geo[1])

    def location(self, v=None):
        """Sets or returns the LOCATION property."""
        self._singular_property("LOCATION", "TEXT", v)

    def transp(self, v=None):
        """Sets or returns the TRANSP property."""
        if v != None:
            ok_values = ('OPAQUE', 'TRANSPARENT')
            v = upper(v)
            if v not in ok_values:
                raise ValueError, "%s is not one of %s" %\
                      (str(v), str(ok_values))
        self._singular_property('TRANSP', 'TEXT', v)

    def resources(self, v=None):
        pass

class Todo(GenericComponent):
    "The iCalendar TODO component."

    def component_type(self):
        "Returns the type of component for the object."
        return "VTODO"

    def clone(self):
        "Returns a copy of the object."
        return Todo(self.asIcalString())

    def completed(self, value=None):
        pass

    def geo(self, value=None):
        pass

    def location(self, value=None):
        pass

    def percent(self, value=None):
        pass

    def status(self, value=None):
        pass

    def due(self, value=None):
        pass

    def duration(self, value=None):
        pass

    def resources():
        pass

class Journal(GenericComponent):
    "The iCalendar JOURNAL component."

    def component_type(self):
        "Returns the type of component for the object."
        return "VJOURNAL"

    def clone(self):
        "Returns a copy of the object."
        return Journal(self.asIcalString())
