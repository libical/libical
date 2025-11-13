/**
 * @file    vcomponent_cxx.hpp
 * @author  fnguyen (12/10/01)
 * @brief   C++ classes for the icalcomponent wrapper (VToDo VEvent, etc..).
 *
 * SPDX-FileCopyrightText: 2001, Critical Path
 * SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 */

#ifndef ICAL_VCOMPONENT_CXX_H
#define ICAL_VCOMPONENT_CXX_H

#include "libical_ical_export.h"
#include "icptrholder_cxx.hpp"

extern "C" {
#include "icalerror.h"
#include "icalcomponent.h"
}

#include <string>

namespace LibICal
{

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
    VComponent();
    VComponent(const VComponent &);
    VComponent &operator=(const VComponent &);
    virtual ~VComponent();

    explicit VComponent(icalcomponent *v);
    explicit VComponent(const std::string &str);
    explicit VComponent(const icalcomponent_kind &kind);

    operator icalcomponent *()
    {
        return imp;
    }

    void new_from_string(const std::string &str);

    // detach imp to this object. use with caution. it would cause
    // memory leak if you are not careful.
    void detach();

    std::string as_ical_string();
    bool is_valid();
    icalcomponent_kind isa();
    bool isa_component(void *component);

    /// Working with properties
    void add_property(ICalProperty *property);
    void remove_property(ICalProperty *property);
    int count_properties(const icalproperty_kind &kind);

    // Iterate through the properties
    ICalProperty *get_current_property();
    ICalProperty *get_first_property(const icalproperty_kind &kind);
    ICalProperty *get_next_property(const icalproperty_kind &kind);

    // Working with components

    /**
     *  Return the first VEVENT, VTODO or VJOURNAL sub-component if
     *  it is one of those types
     */

    VComponent *get_inner();

    void add_component(VComponent *child);
    void remove_component(VComponent *child);
    int count_components(const icalcomponent_kind &kind);

    /**
     * Iteration Routines. There are two forms of iterators,
     * internal and external. The internal ones came first, and
     * are almost completely sufficient, but they fail badly when
     * you want to construct a loop that removes components from
     * the container.
     */

    /// Iterate through components
    VComponent *get_current_component();
    VComponent *get_first_component(const icalcomponent_kind &kind);
    VComponent *get_next_component(const icalcomponent_kind &kind);

    /// Using external iterators
    icalcompiter begin_component(const icalcomponent_kind &kind);
    icalcompiter end_component(const icalcomponent_kind &kind);
    static VComponent *next(icalcompiter *i);
    static VComponent *prev(icalcompiter *i);
    static VComponent *current(icalcompiter *i);

    /// Working with embedded error properties
    int count_errors();

    /// Remove all X-LIC-ERROR properties
    void strip_errors();

    /// Convert some X-LIC-ERROR properties into RETURN-STATUS properties
    void convert_errors();

    /// Kind conversion routines
    static icalcomponent_kind string_to_kind(const std::string &str);
    static std::string kind_to_string(const icalcomponent_kind &kind);

    struct icaltimetype get_dtstart() const;
    void set_dtstart(const struct icaltimetype &v);

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
    void set_dtend(const struct icaltimetype &v);

    struct icaltimetype get_due() const;
    void set_due(const struct icaltimetype &v);

    struct icaldurationtype get_duration() const;
    void set_duration(const struct icaldurationtype &v);

    icalproperty_method get_method() const;
    void set_method(const icalproperty_method &method);

    struct icaltimetype get_dtstamp() const;
    void set_dtstamp(const struct icaltimetype &v);

    std::string get_summary() const;
    void set_summary(const std::string &v);

    std::string get_location() const;
    void set_location(const std::string &v);

    std::string get_description() const;
    void set_description(const std::string &v);

    std::string get_comment() const;
    void set_comment(const std::string &v);

    std::string get_uid() const;
    void set_uid(const std::string &v);

    std::string get_relcalid() const;
    void set_relcalid(const std::string &v);

    struct icaltimetype get_recurrenceid() const;
    void set_recurrenceid(const struct icaltimetype &v);

    int get_sequence() const;
    void set_sequence(const int &v);

    /**
     * Returns the status VComponent status.
     *
     * @returns the status property or ICAL_STATUS_NONE if a problem parsing the status was detected.
     */
    int get_status() const;
    void set_status(const enum icalproperty_status &v);

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

    bool recurrence_is_excluded(struct icaltimetype *dtstart, struct icaltimetype *recurtime);

    /* helper functions for adding/removing/updating property and sub-components */

    /// Note: the VComponent kind have to be the same

    bool remove(VComponent &, bool ignoreValue);
    bool update(VComponent &, bool removeMissing);
    bool add(VComponent &);

private:
    icalcomponent *imp;
};

class LIBICAL_ICAL_EXPORT VCalendar : public VComponent
{
public:
    VCalendar();
    VCalendar(const VCalendar &);
    VCalendar &operator=(const VCalendar &);
    ~VCalendar() override;

    explicit VCalendar(icalcomponent *v);
    explicit VCalendar(const std::string &str);
};

class LIBICAL_ICAL_EXPORT VEvent : public VComponent
{
public:
    VEvent();
    VEvent(const VEvent &);
    VEvent &operator=(const VEvent &);
    ~VEvent() override;

    explicit VEvent(icalcomponent *v);
    explicit VEvent(const std::string &str);
};

class LIBICAL_ICAL_EXPORT VToDo : public VComponent
{
public:
    VToDo();
    VToDo(const VToDo &);
    VToDo &operator=(const VToDo &);
    ~VToDo() override;

    explicit VToDo(icalcomponent *v);
    explicit VToDo(const std::string &str);
};

class LIBICAL_ICAL_EXPORT VAgenda : public VComponent
{
public:
    VAgenda();
    VAgenda(const VAgenda &);
    VAgenda &operator=(const VAgenda &);
    ~VAgenda() override;

    explicit VAgenda(icalcomponent *v);
    explicit VAgenda(const std::string &str);
};

class LIBICAL_ICAL_EXPORT VQuery : public VComponent
{
public:
    VQuery();
    VQuery(const VQuery &);
    VQuery &operator=(const VQuery &);
    ~VQuery() override;

    explicit VQuery(icalcomponent *v);
    explicit VQuery(const std::string &str);
};

class LIBICAL_ICAL_EXPORT VJournal : public VComponent
{
public:
    VJournal();
    VJournal(const VJournal &);
    VJournal &operator=(const VJournal &);
    ~VJournal() override;

    explicit VJournal(icalcomponent *v);
    explicit VJournal(const std::string &str);
};

class LIBICAL_ICAL_EXPORT VAlarm : public VComponent
{
public:
    VAlarm();
    VAlarm(const VAlarm &);
    VAlarm &operator=(const VAlarm &);
    ~VAlarm() override;

    explicit VAlarm(icalcomponent *v);
    explicit VAlarm(const std::string &str);

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
    ~VFreeBusy() override;

    explicit VFreeBusy(icalcomponent *v);
    explicit VFreeBusy(const std::string &str);
};

class LIBICAL_ICAL_EXPORT VTimezone : public VComponent
{
public:
    VTimezone();
    VTimezone(const VTimezone &);
    VTimezone &operator=(const VTimezone &);
    ~VTimezone() override;

    explicit VTimezone(icalcomponent *v);
    explicit VTimezone(const std::string &str);
};

class LIBICAL_ICAL_EXPORT XStandard : public VComponent
{
public:
    XStandard();
    XStandard(const XStandard &);
    XStandard &operator=(const XStandard &);
    ~XStandard() override;

    explicit XStandard(icalcomponent *v);
    explicit XStandard(const std::string &str);
};

class LIBICAL_ICAL_EXPORT XDaylight : public VComponent
{
public:
    XDaylight();
    XDaylight(const XDaylight &);
    XDaylight &operator=(const XDaylight &);
    ~XDaylight() override;

    explicit XDaylight(icalcomponent *v);
    explicit XDaylight(const std::string &str);
};

} // namespace LibICal;

typedef ICPointerHolder<LibICal::VComponent> VComponentTmpPtr;

#endif /* ICAL_VCOMPONENT_H */
