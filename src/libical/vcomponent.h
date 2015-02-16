/**
 * @file    vcomponent.h
 * @author  fnguyen (12/10/01)
 * @brief   C++ classes for the icalcomponent wrapper (VToDo VEvent, etc..).
 *
 * (C) COPYRIGHT 2001, Critical Path

 This program is free software; you can redistribute it and/or modify
 it under the terms of either:

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.gnu.org/licenses/lgpl-2.1.html

  Or:

    The Mozilla Public License Version 1.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/
 */

#ifndef VCOMPONENT_H
#define VCOMPONENT_H

#include "libical_ical_export.h"

extern "C" {
#include <libical/ical.h>
};
#include "icptrholder.h"

typedef char *string; // Will use the string library from STL

class ICalProperty;

/**
 * @class VComponent
 * @brief A class wrapping the libical icalcomponent functions
 *
 * @exception icalerrorenum   Any errors generated in libical are
 *                            propagated via this exception type.
 */

class LIBICAL_ICAL_EXPORT VComponent
{
public:
    VComponent()                                 throw (icalerrorenum);
    VComponent(const VComponent &)               throw (icalerrorenum);
    VComponent &operator=(const VComponent &)    throw (icalerrorenum);
    virtual ~VComponent();

    explicit VComponent(icalcomponent *v)        throw (icalerrorenum);
    explicit VComponent(string str)              throw (icalerrorenum);
    explicit VComponent(icalcomponent_kind kind) throw (icalerrorenum);

    operator icalcomponent *()
    {
        return imp;
    }

    void new_from_string(string str);

    // detach imp to this object. use with caution. it would cause
    // memory leak if you are not careful.
    void detach();

public:
    string as_ical_string() throw (icalerrorenum);
    bool is_valid();
    icalcomponent_kind isa();
    int isa_component(void *component);

    /// Working with properties
    void add_property(ICalProperty *property);
    void remove_property(ICalProperty *property);
    int count_properties(icalproperty_kind kind);

    // Iterate through the properties
    ICalProperty *get_current_property();
    ICalProperty *get_first_property(icalproperty_kind kind);
    ICalProperty *get_next_property(icalproperty_kind kind);

    // Working with components

    /**
     *  Return the first VEVENT, VTODO or VJOURNAL sub-component if
     *   it is one of those types
     */

    VComponent *get_inner();

    void add_component(VComponent *child);
    void remove_component(VComponent *child);
    int count_components(icalcomponent_kind kind);

    /**
     * Iteration Routines. There are two forms of iterators,
     * internal and external. The internal ones came first, and
     * are almost completely sufficient, but they fail badly when
     * you want to construct a loop that removes components from
     * the container.
    */

    /// Iterate through components
    VComponent *get_current_component();
    VComponent *get_first_component(icalcomponent_kind kind);
    VComponent *get_next_component(icalcomponent_kind kind);

    /// Using external iterators
    icalcompiter begin_component(icalcomponent_kind kind);
    icalcompiter end_component(icalcomponent_kind kind);
    VComponent *next(icalcompiter *i);
    VComponent *prev(icalcompiter *i);
    VComponent *current(icalcompiter *i);

    /// Working with embedded error properties
    int count_errors();

    /// Remove all X-LIC-ERROR properties
    void strip_errors();

    /// Convert some X-LIC-ERROR properties into RETURN-STATUS properties
    void convert_errors();

    /// Kind conversion routines
    static icalcomponent_kind string_to_kind(string str);
    static string kind_to_string(icalcomponent_kind kind);

public:
    struct icaltimetype get_dtstart() const;
    void set_dtstart(const struct icaltimetype v);

    /** For the icalcomponent routines only, dtend and duration
     *  are tied together. If you call the set routine for one and
     *  the other exists, the routine will calculate the change to
     *  the other. That is, if there is a DTEND and you call
     *  set_duration, the routine will modify DTEND to be the sum
     *  of DTSTART and the duration. If you call a get routine for
     *  one and the other exists, the routine will calculate the
     *  return value. If you call a set routine and neither
     *  exists, the routine will create the appropriate property
    */

    struct icaltimetype get_dtend() const;
    void set_dtend(const struct icaltimetype v);

    struct icaltimetype get_due() const;
    void set_due(const struct icaltimetype v);

    struct icaldurationtype get_duration() const;
    void set_duration(const struct icaldurationtype v);

    icalproperty_method get_method() const;
    void set_method(const icalproperty_method method);

    struct icaltimetype get_dtstamp() const;
    void set_dtstamp(const struct icaltimetype v);

    string get_summary() const;
    void set_summary(const string v);

    string get_location() const;
    void set_location(const string v);

    string get_description() const;
    void set_description(const string v);

    string get_comment() const;
    void set_comment(const string v);

    string get_uid() const;
    void set_uid(const string v);

    string get_relcalid() const;
    void set_relcalid(const string v);

    struct icaltimetype get_recurrenceid() const;
    void set_recurrenceid(const struct icaltimetype v);

    int get_sequence() const;
    void set_sequence(const int v);

    int get_status() const;
    void set_status(const enum icalproperty_status v);

public:
    /**
     * For VCOMPONENT: Return a reference to the first VEVENT,
     * VTODO, or VJOURNAL
    */
    VComponent *get_first_real_component();

    /**
     * For VEVENT, VTODO, VJOURNAL and VTIMEZONE: report the
     * start and end times of an event in UTC
     */
    virtual struct icaltime_span get_span();

    int recurrence_is_excluded(struct icaltimetype *dtstart,
                               struct icaltimetype *recurtime);

public:
    /**
       helper functions for adding/removing/updating property and
       sub-components */

    /// Note: the VComponent kind have to be the same

    bool remove(VComponent &, bool ignoreValue);
    bool update(VComponent &, bool removeMissing);
    bool add(VComponent &);

private:
    /* Internal operations. They are private, and you should not be using them. */
    VComponent *get_parent();
    void set_parent(VComponent *parent);

    char *quote_ical_string(char *str);

private:
    icalcomponent *imp;
};

class LIBICAL_ICAL_EXPORT VCalendar : public VComponent
{
public:
    VCalendar();
    VCalendar(const VCalendar &);
    VCalendar &operator=(const VCalendar &);
    ~VCalendar();

    explicit VCalendar(icalcomponent *v);
    explicit VCalendar(string str);
};

class LIBICAL_ICAL_EXPORT VEvent : public VComponent
{
public:
    VEvent();
    VEvent(const VEvent &);
    VEvent &operator=(const VEvent &);
    ~VEvent();

    explicit VEvent(icalcomponent *v);
    explicit VEvent(string str);
};

class LIBICAL_ICAL_EXPORT VToDo : public VComponent
{
public:
    VToDo();
    VToDo(const VToDo &);
    VToDo &operator=(const VToDo &);
    ~VToDo();

    explicit VToDo(icalcomponent *v);
    explicit VToDo(string str);
};

class LIBICAL_ICAL_EXPORT VAgenda : public VComponent
{
public:
    VAgenda();
    VAgenda(const VAgenda &);
    VAgenda &operator=(const VAgenda &);
    ~VAgenda();

    explicit VAgenda(icalcomponent *v);
    explicit VAgenda(string str);
};

class LIBICAL_ICAL_EXPORT VQuery : public VComponent
{
public:
    VQuery();
    VQuery(const VQuery &);
    VQuery &operator=(const VQuery &);
    ~VQuery();

    explicit VQuery(icalcomponent *v);
    explicit VQuery(string str);
};

class LIBICAL_ICAL_EXPORT VJournal : public VComponent
{
public:
    VJournal();
    VJournal(const VJournal &);
    VJournal &operator=(const VJournal &);
    ~VJournal();

    explicit VJournal(icalcomponent *v);
    explicit VJournal(string str);
};

class LIBICAL_ICAL_EXPORT VAlarm : public VComponent
{
public:
    VAlarm();
    VAlarm(const VAlarm &);
    VAlarm &operator=(const VAlarm &);
    ~VAlarm();

    explicit VAlarm(icalcomponent *v);
    explicit VAlarm(string str);

    /**
     *  compute the absolute trigger time for this alarm. trigger
     *  may be related to the containing component c.  the result
     *  is populated in tr->time
     */
    icalrequeststatus getTriggerTime(VComponent &c, struct icaltriggertype *tr);
};

class LIBICAL_ICAL_EXPORT VFreeBusy : public VComponent
{
public:
    VFreeBusy();
    VFreeBusy(const VFreeBusy &);
    VFreeBusy &operator=(const VFreeBusy &);
    ~VFreeBusy();

    explicit VFreeBusy(icalcomponent *v);
    explicit VFreeBusy(string str);
};

class LIBICAL_ICAL_EXPORT VTimezone : public VComponent
{
public:
    VTimezone();
    VTimezone(const VTimezone &);
    VTimezone &operator=(const VTimezone &);
    ~VTimezone();

    explicit VTimezone(icalcomponent *v);
    explicit VTimezone(string str);
};

class LIBICAL_ICAL_EXPORT XStandard : public VComponent
{
public:
    XStandard();
    XStandard(const XStandard &);
    XStandard &operator=(const XStandard &);
    ~XStandard();

    explicit XStandard(icalcomponent *v);
    explicit XStandard(string str);
};

class LIBICAL_ICAL_EXPORT XDaylight : public VComponent
{
public:
    XDaylight();
    XDaylight(const XDaylight &);
    XDaylight &operator=(const XDaylight &);
    ~XDaylight();

    explicit XDaylight(icalcomponent *v);
    explicit XDaylight(string str);
};

typedef ICPointerHolder<VComponent> VComponentTmpPtr; /* see icptrholder.h for comments */

#endif /* VComponent_H */
