#!/usr/bin/env python 

# $Id: Libical.py,v 1.7 2001-02-26 15:11:44 plewis Exp $

from LibicalWrap import *
from types import *
from string import upper, join
import base64

class Property:
    """ Represent any iCalendar Property.

    Usage:
    Property(dict)

    Where:
    dict is a dictionary with keys of 'name', 'value_type', and 'value'.
    In addition, parameter:parameter value entries may be included.
    """



    def __init__(self, dict):
        # TEMP_NOTE: I have split the singular dict into two separate ones.
        
        # Object descriptive & value data stored in self.desc
        # Properties are stored in self.parameters
        self.desc = {}
        self.parameters = {}
  
        desc_keys = ('name', 'value', 'value_type', 'pid')

        # Initialize all of the required keys
        for key in desc_keys:
            self.desc[key] = None

        for key in dict.keys():
            if key in desc_keys:
                self.desc[key]=dict[key]
            else:
                self[key]=dict[key]

    def name(self,v=None):
        """ Return the name of the property """
        if(v != None):
            self.desc['name'] = v

        return self.desc['name']

    def value_type(self,v=None):
        """ Return the RFC2445 name of the value """
        if(v != None):
            self.desc['value_type'] = v

        return self.desc['value_type']

    def value(self,v=None):
        """ Return the RFC2445 representation of the value """
        if(v != None):
            self.desc['value'] = v

        return self.desc['value']

    def getAllParameters(self):
        """Returns a list containing all the set parameters (not values).

        Only paramters whose values are not None are returned (i.e. if
        a value is None, the parameter value is assumed to be unset).
        """
        return filter(lambda p, s=self: s[p] != None, self.parameters.keys())

    def _update_value():
        """
        A virtual method that creates the dict entry value from another
        representation of the property
        """
        pass

    def asIcalString(self):
        " "
        return str(self)

    def __getitem__(self,key):
        """ Return property values by name """
        key = upper(key)
        if self.parameters.has_key(key):
            return self.parameters[key]
        else:
            return None

    def __setitem__(self,key,value):
        """ Set Property Values by Name """
        key = upper(key)
        self.parameters[key] = value

        return self.parameters[key]

    def __str__(self):
        outS = "%s" % self.name()              # Add the name
        for param in self.getAllParameters():  # Add parameters (if any)
            outS = "%s;%s=" % (outS, param )
            
            paramValues = self[param]
            # Check for mutiple parameter values
            if isinstance(paramValues, ListType) or \
               isinstance(paramValues, TupleType):
                outS="%s%s" % (outS, str(paramValues[0]))
                for val in paramValues[1:]:       # add comma separated values
                    outS="%s,%s" % (outS, str(val))
            # Handle single parameter values
            else:        
                outS = "%s%s" % (outS, str(paramValues))
                
        outS = "%s:%s" % (outS, str(self.value())) # Add Property value

        # "Fold" output (Shouldn't happen at Property level, but at top
        # component level).
##         if len(outS) > 75:
##             outL = [outS[0:75]]             # Chop outS into 75 char chunks
##             for chop in range(75, len(outS), 74): #74 chars because of space
##                 outL.append(outS[chop:chop+74])
##             outS = join(outL, "\r\n ")         
        # return "%s%s" % (outS, "\r\n")            # insert final CRLF

        return outS

class Time(Property):
    """ Represent iCalendar DATE, TIME and DATE-TIME """
    def __init__(self, value):
        """ 
        Create a new Time from a string or number of seconds past the 
        POSIX epoch

        Time("19970325T123000Z")  Construct from an iCalendar string
        Time(8349873494)          Construct from seconds past POSIX epoch
        
        """
        Property.__init__(self, {})

        if isinstance(value, DictType):
            Property.__init__(self, value)
            self.tt = icaltime_from_string(self.desc['value'])
        elif isinstance(value, StringType):
            self.tt = icaltime_from_string(value)
        elif isinstance(value, IntType) or   \
             isinstance(value, FloatType): 
            self.tt = icaltime_from_timet(int(value))
        else:
            self.tt = icaltime_null_time()

        self._update_value()

    def _update_value(self):
        """Updates value and value_type based on the (internal) self.tt."""

        if(self.is_date()):
            self.value_type('DATE')
        else:
            self.value_type('DATE-TIME')

        self.tt = icaltime_normalize(self.tt)
        self.desc['value'] = icaltime_as_ical_string(self.tt)


    def utc_seconds(self,v=None):
        """ Return or set time in seconds past POSIX epoch"""
        if (v!=None):
            self.tt = icaltime_from_timet(v,0)
            self._update_value()
        return icaltime_as_timet(self.tt)

    def is_utc(self,v=None):
        """ Return or set boolean indicating if time is in UTC """
        if(v != None):
            icaltimetype_is_utc_set(self.tt,v)
            self._update_value()
        return icaltimetype_is_utc_get(self.tt)

    def is_date(self,v=None):
        """ Return or set boolean indicating if time is actually a date """
        if(v != None):
            icaltimetype_is_date_set(self.tt,v)
            self._update_value()
        return icaltimetype_is_date_get(self.tt)

    def timezone(self,v=None):
        """ Return or set the timezone string for this time """
        if (v != None):
            self['TZID'] = v
        return  self['TZID']

    def second(self,v=None):
        """ Get or set the seconds component of this time """
        if(v != None):
            icaltimetype_second_set(self.tt,v)
            self._update_value()
        return icaltimetype_second_get(self.tt)

    def minute(self,v=None):
        """ Get or set the minute component of this time """
        if(v != None):
            icaltimetype_minute_set(self.tt,v)
            self._update_value()
        return icaltimetype_minute_get(self.tt)

    def hour(self,v=None):
        """ Get or set the hour component of this time """
        if(v != None):
            icaltimetype_hour_set(self.tt,v)
            self._update_value()
        return icaltimetype_hour_get(self.tt)

    def day(self,v=None):
        """ Get or set the month day component of this time """
        if(v != None):
            icaltimetype_day_set(self.tt,v)
            self._update_value()
        return icaltimetype_day_get(self.tt)

    def month(self,v=None):
        """ Get or set the month component of this time. January is month 1 """
        if(v != None):
            icaltimetype_month_set(self.tt,v)
            self._update_value()
        return icaltimetype_month_get(self.tt)

    def year(self,v=None):
        """ Get or set the year component of this time """
        if(v != None):
            icaltimetype_year_set(self.tt,v)
            self._update_value()

        return icaltimetype_year_get(self.tt)

    def value(self, v=None):
        """Get or set the time using a RFC2445 time string."""
        # We need to ensure self.tt always matches value, because many
        # methods use self.tt
        if v!= None:
            self.tt = icaltime_from_string(v)
            self._update_value()

        return self.desc['value']


def test_time():
    "Test routine"
    t = Time("19970325T123010Z")
    
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
    assert(str(t)=='None;TZID=America/Los_Angeles:19970325T123010Z')
    t.name("DTSTART")

    t.second(t.second()+80)

    print t

    assert(t.minute() == 31)
    assert(t.second() == 30)


class Duration(Property):
    """ 
    Represent a length of time, like 3 minutes, or 6 days, 20 seconds.
    

    """

    def __init__(self, value):
        """
        Create a new duration from an RFC2445 string or number of seconds.
        Construct the duration from an iCalendar string or a number of seconds.

        Duration("P3DT2H34M45S")   Construct from an iCalendar string
        Duration(3660)             Construct from seconds 
        """ 

        Property.__init__(self, {})
        
        if isinstance(value, DictType):
            Property.__init__(self, value)
            self.dur = icaldurationtype_from_string(self.desc['value'])
        elif isinstance(value, StringType):
            self.dur = icaldurationtype_from_string(value)
        elif isinstance(value, IntType): 
            self.dur = icaldurationtype_from_int(value)
        else:
            self.dur = icaldurationtype_null_duration()

        Property.name(self, 'DURATION')
        Property.value_type(self, 'DURATION')
        self._update_value()

    def _update_value(self):
        self.desc['value'] = icaldurationtype_as_ical_string(self.dur)

    def seconds(self,v=None):
        """Return or set duration in seconds"""
        if(v != None):
            self.dur = icaldurationtype_from_int(v);
            self.dict['value'] = icaltimedurationtype_as_ical_string(self.dur)
        return icaldurationtype_as_int(self.dur)

    def value(self, v=None):
        "Return or set duration using iCalendar duration string."
        if v!=None:
            self.dur = icaldurationtype_from_string(v)
            self._update_value()
            
        return self.desc['value']

    def name(self):
        "Return the name of the property."
        return Property.name(self)

    def value_type(self):
        "Return the value type of the property."
        return Property.value_type(self)


class Period(Property):
    """Represent a span of time"""
    def __init__(self,dict):
        """ """
        Property.__init__(self,dict)


    def start(self,v=None):
        """
        Return or set start time of the period. The start time may be
        expressed as an RFC2445 format string or an instance of Time.
        The return value is an instance of Time
        """

        if(v != None):
            pass
        return 

    def end(self,v=None):
        """
        Return or set end time of the period. The end time may be
        expressed as an RFC2445 format string or an instance of Time.
        The return value is an instance of Time
        """        
        if(v != None):
            pass
        return 

    def duration(self,v=None):
        """
        Return or set the duration of the period. The duration may be
        expressed as an RFC2445 format string or an instance of Duration.
        The return value is an instance of Duration
        """        

        if(v != None):
            pass
        return 


class Attendee(Property):
    """Class for Attendee properties.

    Usage:
    Attendee([dict])
    Attendee([address])

    Where:
    dict is an optional dictionary with keys of
     'value': CAL-ADDRESS string and any parameter: parameter_value entries.
     'name' and 'value_type' entries in dict are ignored and automatically set
     with the appropriate values.
    address is the CAL-ADDRESS (string) of the Attendee 
    """

    def __init__(self, arg=None):
        if isinstance(arg, DictType):
            Property.__init__(self, dict)
        else:
            Property.__init__(self, {})
            self.value(arg)
        param_t = ('CUTYPE', 'MEMBER', 'ROLE', 'PARTSTAT', 'RSVP',
                   'DELEGATED-TO', 'DELEGATED-FROM', 'SENT-BY', 'CN', 'DIR',
                   'LANGUAGE' )
        for param in param_t:
            self[param] = None
        Property.name(self, 'ATTENDEE')
        Property.value_type(self, 'CAL-ADDRESS')
        
    def _doParam(self, parameter, v):
        if v!=None:
            self[parameter]=v
        return self[parameter]

    def name(self):
        "Return the name of the property."
        return self.desc['name']

    def value_type(self):
        "Return the value type of the property."
        return self.desc['value_type']

    # Methods for accessing enumerated parameters
    def cn(self, v=None): self._doParam('CN', v)
    def cutype(self, v=None): self._doParam('CUTYPE', v)
    def dir(self, v=None): self._doParam('DIR', v)
    def delegated_from(self, v=None): self._doParam('DELEGATED-FROM', v)
    def delegated_to(self, v=None): self._doParam('DELEGATED-TO', v)
    def language(self, v=None): self._doParam('LANGUAGE', v)
    def member(self, v=None): self._doParam('MEMBER', v)
    def partstat(self, v=None): self._doParam('PARTSTAT', v)
    def role(self, v=None): self._doParam('ROLE', v)
    def rsvp(self, v=None): self._doParam('RSVP', v)
    def sent_by(self, v=None): self._doParam('SENT-BY', v)


class Organizer(Property):
    """Class for Organizer property.

    Usage:
    Organizer([dict])

    Where dict is an optional dictionary with keys of 'value': CAL-ADDRESS
    string and any parameter: parameter_value entries.  'name' and 'value_type'
    entries in dict are ignored and automatically set with the appropriate
    values.
    """

    def __init__(self, dict={}):
        param_t = ( 'CN', 'DIR', 'SENT-BY', 'LANGUAGE' )
        for param in param_t:
            self[param] = None
        Property.__init__(self, dict)
        Property.name(self, 'ORGANIZER')
        Property.value_type(self, 'CAL-ADDRESS')

    def _doParam(self, parameter, v):
        if v!=None:
            self[parameter]=v
        return self[parameter]

    def name(self):
        "Return the name of the property."
        return Property.name(self)

    def value_type(self):
        "Return the value type of the property."
        return self.desc['value_type']

    # Methods for accessing enumerated parameters
    def cn(self, v=None): self._doParam('CN', v)
    def dir(self, v=None): self._doParam('DIR', v)
    def language(self, v=None): self._doParam('LANGUAGE', v)
    def sent_by(self, v=None): self._doParam('SENT-BY', v)

class Recurrence_Id(Time):
    """Class for RECURRENCE-ID property.

    Usage:
    Reccurence_Id(dict)         # A normal property dictionary
    Reccurence_Id("19960401")   # An iCalendar string
    Reccurence_Id(8349873494)   # Seconds from epoch

    If the 'dict' constructor is used, 'name' and 'value_type'
    entries in dict are ignored and automatically set with the appropriate
    values.
    """

    def __init__(self, dict={}):
        Time.__init__(self, dict)
        Property.name(self, 'RECURRENCE-ID')

    def name(self):
        return Property.name(self)

    def _doParam(self, parameter, v):
        if v!=None:
            self[parameter]=v
        return self[parameter]

    # Enumerated parameters
    def value_parameter(self, v=None):
        """Sets or gets the VALUE parameter value.

        The value passed should be either "DATE-TIME" or "DATE".  Setting this
        parameter has no impact on the property's value_type.  Doing something
        like:

        rid=Recurrence_Id("19960401")    # Sets value & makes value_type="DATE"
        rid.value_parameter("DATE-TIME") # Sets the parameter VALUE=DATE-TIME

        Would be allowed (even though it is wrong), so pay attention.
        Verifying the component will reveal the error.
        """
        if v!=None and v!="DATE" and v!="DATE-TIME":
            raise ValueError, "%s is an invalid VALUE parameter value" % str(v)
        self._doParam("VALUE", v)

    def tzid(self, v=None):
        "Sets or gets the TZID parameter value."
        self._doParam("TZID", v)

    def range_parameter(self, v=None): # 'range' is a builtin function
        "Sets or gets the RANGE parameter value."
        if v!=None and v!="THISANDPRIOR" and v!= "THISANDFUTURE":
            raise ValueError, "%s is an invalid RANGE parameter value" % str(v)
        self._doParam("RANGE", v)

class Attach(Property):
    """A class representing an ATTACH property.

    Usage:
    Attach(uriString [, parameter_dict])
    Attach(fileObj [, parameter_dict])
    """

    def __init__(self, value=None, parameter_dict={}):
        Property.__init__(self, parameter_dict)
        Property.name(self, 'ATTACH')
        self.value(value)

    def value(self, v=None):
        "Returns or sets the value of the property."
        if v != None:
            if isinstance(v, StringType):  # Is a URI
                self.desc['value']=v
                Property.value_type(self, 'URI')
            else:
                try:
                    tempStr = v.read()
                except:
                    raise TypeError,"%s must be a URL string or file-ish type"\
                          % str(v)
                self.desc['value'] = base64.encodestring(tempStr)
                Property.value_type(self, 'BINARY')
        else:
            return self.desc['value']

    def name(self):
        "Returns the name of the property."
        return Property.name(self)

    def value_type(self):
        return Property.value_type(self)

    def fmttype(self, v=None):
        "Gets or sets the FMTYPE parameter."
        if v!= None:
            self['FMTTYPE']=v
        else:
            return self['FMTTYPE']

class Collection:
    """A group of components that can be modified somewhat like a list.

    Usage:
        Collection(componet, propSequence)

    component is a Component object
    propSequence is a list or tuple of Property (or subclass of Property)
        of objects already in component
    """

    def __init__(self, component, propSequence):
        self._properties = list(propSequence[:])
        self._component = component

    def __getslice__(self, beg, end):
        return Collection(self._component, self._properties[beg:end])

    def __setslice__(self, beg, end, sequence):
        oldProps = self_properties[beg, end]
        for p in oldProps:
            self._component.removeProperty(p)
        self._properties.__setslice__(beg, end, sequence
        for p in sequence:
            self._component.addProperty(p)
            
    def __getitem__(self, i):
        return self._properties[i]

    def __setitem__(self, i, prop):
        self._component.removeProperty(self._properties[i])
        self._component.addProperty(prop)
        self._properties[i]=prop

    def __del__(self, i):
        self._component.removeProperty(self._properties[i])
        del self._properties[i]

    def __len__(self):
        return len(self._properties)
            
    def append(self, property):
        self._properties.append(property)
        self._component.addProperty(property)

        
    
        
class Component:

    def __init__(self,str=None, parent=None):
        self._parent = parent
        self._children = []
        self._properties=[]
        if str != None:
            self._parseComponentString(str)

#        self.comp_p = None

#        self.comp_p = icalparser_parse_string(str)

##     def __del__(self):
##         if self.comp_p != None and icalcomponent_get_parent(self.comp_p) != None:
##             icalcomponent_free(self.comp_p)

##             self.comp_p = None

    def _parseComponentString(self, comp_str):
        comp = icalparser_parse_string(comp_str)
        #c = icallangbind_get_first_component(comp, 'ANY')
        print icalcomponent_as_ical_string(comp)

        # Parse properties for this component
        #anyKind = icalenum_string_to_property_kind('ICAL_ANY_PROPERTY')
        #print comp
        #print icalenum_property_kind_to_string(anyKind)
        p = icallangbind_get_first_property(comp, 'ANY')
        print p
        while p!='NULL':
            p_dict = icallangbind_property_eval_string(p,":")
            print "p_dict = ", p_dict
            if p_dict==None: p_dict="{'name':'X-DUM', 'value':'Err', 'value_type':'TEXT'}"
            p_dict = eval(p_dict)
            print p_dict
            self._addPropertyFromDict(p_dict)
            p = icallangbind_get_next_property(comp, 'ANY')

        # Get remaining components
        c = icallangbind_get_first_component(comp, 'ANY')
        while c!='NULL':
            self.addComponent(NewComponent(c))
            c = icallangbind_get_next_component(comp, 'ANY')
            

    def _addPropertyFromDict(self, prop_dict):
        name = prop_dict['name']
        if name == 'DURATION':
            self.addProperty(Duration(prop_dict))
        elif name == 'PERIOD':
            self.addProperty(Period(prop_dict))
        elif name == 'ATTENDEE':
            self.addProperty(Attendee(prop_dict))
        elif name == 'ORGANIZER':
            self.addProperty(Organizer(prop_dict))
        elif name == 'RECURRENCE-ID':
            self.addProperty(Recuurence_Id(prop_dict))
        elif name == 'ATTACH':
            self.addProperty(Attach(prop_dict))
        elif prop_dict['value_type'] == 'DATE-TIME':
            self.addProperty(Time(prop_dict))
        else:
            self.addProperty(Property(prop_dict))
        
    def properties(self,type='ANY'): 
        """  
        Return a list of Property instances, each representing a
        property of the type 'type.'
        """

        if type=='ANY':
            return self._properties[:]
        else:
            p_list = []
            for p in self._properties:
                if p.name()==type:
                    p_list.append(p)
            return p_list

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

    def addProperty(self, property):
        "Adds the property object to the component."
        self._properties.append(property)
            
    def removeProperty(self,property):
        try:
            self._properties.remove(property)
        except ValueError:
            raise ValueError, "not a property in this component."

    def removeProperties(self, type):
        """Remove all properties with the name equal to the argument name.

        Passing "ANY" as the type will remove all Properties.
        """
        to_remvoe = self.properties(type)
        for p in to_remove:
            self.removeProperty(p)

    def addComponent(self, componentObj):
        "Adds a child component."
        self._children.append(componentObj)
        
    def components(self,type='ANY'):        
        props = []



        return props

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
    else:
        newComp = Component(compStr)

    # I don't think I need to free the component created when passed a string,
    # as it wasn't created with a _new function.

    return newComp


class Event(Component):
    "The iCalendar Event object."

##     def __init__(self, str=None, parent=None):
        
##         ## This whole block may be obsolete if the tight binding in
##         ## Component is removed
        
##         if str==None:
##             event_kind = icalenum_string_to_component_kind("VEVENT")
##             self.comp_p = icalcomponent_new(event_kind)
##             # TODO - implement generic_free() in LibicalWrap
##             # generic_free(event_kind)
##         else:
##             self.comp_p = icalparser_parse_string(str)
        
    def componentType(self):
        "Returns the type of component for the object."
        return "VEVENT"

    def clone(self):
        "Returns a copy of the object."
        return Event(self.asIcalString())

    def _singularProperty(self, name, value_type, value):
        """Sets or gets the value of a method which exists once per Component.

        This is a constructor method for properties without a strictly defined
        object."""
        
        currProperties = self.properties(propType)
        if value==None:
            if len(currProperties) == 0:
                return None
            elif len(currProperties) == 1:
                return currProperties[0].value()
            else:
                raise ValueError, "too many properties of type %s" % propType
        else:
            if len(currProperties) == 0:
                dict = {'name': propType, 'value_type': value_type,
                        'value': value}
                self.addProperty(Property(dict))
            elif len(currProperties) == 1:
                curProperties[0].value(value)
            else:
                raise ValueError, "too many properties of type %s" % propType
            
    def method(self, v=None):
        "Sets or returns the value of the method property."
        self._singularProperty("METHOD", "TEXT", v)

    def prodid(self, v=None):
        "Sets or returns the value of the prodid property."
        self._singularProperty("PRODID", "TEXT", v)

    def calscale(self, v=None):
        "Sets or returns the value of the calscale property."
        self._singularProperty("CALSCALE", "TEXT", v)

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
            self._singularProperty("DTSTART", "DATE-TIME", v)
        else:
            raise TypeError, "%s is an invalid type for DTSTART property" % \
                  str(type(v))

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
            self._singularProperty("DTEND", "DATE-TIME", v)                
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
            self._singularProperty("DURATION", "DURATION", v)
        else:
            raise TypeError, "%s is an invalid type for duration property" % \
                  str(type(v))

    def attendees(self, overwrite, *arguments):
        """Sets attendees or returns a list of Attendee objects.

        Usage:
        # Set using Attendee objects
        attendees(overwrite=1, attObj1, attObj2)
        # Set using a CAL-ADDRESS string
        attendees(1, 'MAILTO:jdoe@somewhere.com')
        # Set using a combination of Attendee objects and strings
        attendees(1, 'MAILTO:jdoe@somewhere.com', attObj1)
        # Returns a list of Attendee objects
        attendees()

        When the attendees are being set, the 'overwrite' flag determines if
        previous attendees in the Event are to be deleted.  If true, values are
        overwritten, otherwise they remain in place. For example:

        attendees(overwrite=0, attObj1)

        would add the Attendee object attObj1 to the existing attendees instead
        of overwriting.
        """
        if arguments:
            if overwrite:
                self.removeProperties('ATTENDEE')
            for att in arguments:
                if isinstance(att, StringType):
                    att = Attendee(att)
                if isinstance(att, Attendee):
                    self.addProperty(att)
                else:
                    raise TypeError, \
                          "%s is not a string or Attendee object." % str(att)
        else:
            return self.properties('ATTENDEE')

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
            self._singularProperty('ORGANIZER', 'CAL-ADDRESS', v)
        else:
            raise TypeError, "%s is not a string or Organizer object." % str(v)
    
    def summary(self, v=None):
        "Sets or gets the SUMMARY value of the Event."
        self._singularProperty('SUMMARY', 'TEXT', v)

    def uid(self, v=None):
        "Sets or gets the UID of the Event."
        self._singularProperty('UID', 'TEXT', v)

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
            self._singularProperty('RECURRENCE-ID', 'DATE-TIME', v)
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
        return self._singularProperty('SEQUENCE', 'INTEGER', v)

    def lastmodified(self, v=None):
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
            self._singularProperty("LAST-MODIFIED", "DATE-TIME", v)
        else:
            raise TypeError, "%s is an invalid type for LAST-MODIFIED property"\
                  % str(type(v))

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

    def related_to(self, v=None):
        # Should this return a list, similar to attendees?
        raise NotImplemented

    def comment(self, v=None):
        # Another multiple valued property
        raise NotImplemented

    def description(self, v=None):
        "Sets or returns the value of the DESCRIPTION property."
        self._singularProperty("DESCRIPTION", "TEXT", v)

    def categories(self, overwrite, *arguments):
        """Sets categories or returns a list of Attendee objects.

        Usage:
        # Set using string[s]
        categories(overwrite=1, 'APPOINTMENT', 'EDUCATION')
        # Returns a list of Category properites
        attendees()

        When the attendees are being set, the 'overwrite' flag determines if
        previous categories in the Event are to be deleted.  If true, values
        are overwritten, otherwise they remain in place. For example:

        categoreis(0, 'PERSONAL')

        would add the category object 'PERSONAL' to the existing attendees
        instead of overwriting.
        """
        if arguments:
            if overwrite:
                self.removeProperties('CATEGORIES')
            for c in arguments:
                self.addProperty(Property({'name': 'CATEGORIES',
                                           'value_type': 'TEXT',
                                           'value': c} ))
        else:
            return self.properties('CATEGORIES')

    def attach(self, overwrite, *arguments):
        """Sets categories or returns a list of Attach objects.
        """
        pass


class RecurrenceSet: 
    """
    Represents a set of event occurrences. This
    class controls a component's RRULE, EXRULE, RDATE and EXDATE
    properties and can produce from them a set of occurrences. 
    """

    def __init__(self):
        pass

    def include(self, **params): 
        """ 
        Include a date or rule to the set. 

        Use date= or pass in a
        Time instance to include a date. Included dates will add an
        RDATE property or will remove an EXDATE property of the same
        date.

        Use rule= or pass in a string to include a rule. Included
        rules with either add a RRULE property or remove an EXRULE
        property.

        """

    def exclude(self, **params): 
        """ 
        Exclude date or rule to the set. 

        Use date= or pass in a Time instance to exclude a
        date. Excluded dates will add an EXDATE property or will remove
        an RDATE property of the same date.

        Use rule= or pass in a string to exclude a rule. Excluded
        rules with either add an EXRULE property or remove an RRULE
        property.

        """


    def occurrences(self, count=None):
        """
        Return 'count' occurrences as a tuple of Time instances.
        """
        pass
        
        

class Store:
    """ 
    Base class for several component storage methods 
    """

    def __init__(self):

	def path(self):
            pass

	def mark(self):
            pass

        def commit(self): 
            pass

	def addComponent(self, comp):
            pass

	def removeComponent(self, comp):
            pass

	def countComponents(self, kind):
            pass

	def select(self, gauge):
            pass

	def clearSelect(self):
            pass

	def fetch(self, uid):
            pass

	def fetchMatch(self, comp):
            pass

	def modify(self, oldc, newc):
            pass

	def currentComponent(self):
            pass

	def firstComponent(self):
            pass

	def nextComponent(self):
            pass



class FileStore(Store):

    def __init__(self):
        pass

    def path(self):
        pass

    def mark(self):
        pass

    def commit(self): 
        pass

    def addComponent(self, comp):
        pass

    def removeComponent(self, comp):
        pass

    def countComponents(self, kind):
        pass

    def select(self, gauge):
        pass

    def clearSelect(self):
        pass

    def fetch(self, uid):
        pass

    def fetchMatch(self, comp):
        pass

    def modify(self, oldc, newc):
        pass

    def currentComponent(self):
        pass

    def firstComponent(self):
        pass

    def nextComponent(self):
        pass
