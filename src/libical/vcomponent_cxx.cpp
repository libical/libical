/**
 * @file    vcomponent_cxx.cpp
 * @author  fnguyen (12/10/01)
 * @brief   Implementation of C++ Wrapper for icalcomponent.c
 *
 * (C) COPYRIGHT 2001, Critical Path

 This library is free software; you can redistribute it and/or modify
 it under the terms of either:

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.gnu.org/licenses/lgpl-2.1.html

 Or:

    The Mozilla Public License Version 2.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "vcomponent_cxx.h"
#include "icalparameter_cxx.h"
#include "icalproperty_cxx.h"
#include "icalvalue_cxx.h"
using namespace LibICal;

extern "C" {
#include "icalmemory.h"
}

#include <cstdlib>

VComponent::VComponent() throw(icalerrorenum) : imp(icalcomponent_new(ICAL_ANY_COMPONENT))
{
}

VComponent::VComponent(const VComponent &v) throw(icalerrorenum)
    : imp(icalcomponent_new_clone(v.imp))
{
    if (imp == NULL) {
        throw icalerrno;
    }
}

VComponent &VComponent::operator=(const VComponent &v) throw(icalerrorenum)
{
    if (this == &v) {
        return *this;
    }

    if (imp != NULL) {
        icalcomponent_free(imp);
        imp = icalcomponent_new_clone(v.imp);
        if (imp == NULL) {
            throw icalerrno;
        }
    }

    return *this;
}

void VComponent::detach()
{
    imp = NULL;
}

VComponent::~VComponent()
{
    if (imp != NULL) {
        icalcomponent_free(imp);
    }
}

VComponent::VComponent(icalcomponent *v) throw (icalerrorenum) : imp(v)
{
}

/* char* returned is in the ring buffer. caller doesn't have to free it */
char *VComponent::quote_ical_string(char *str)
{
    const char *p;
    size_t  buf_sz;
    buf_sz = strlen(str) * 2; // assume worse case scenarios.
    // otherwise, we have to parse the string and count \ */
    char *out = static_cast<char *>(icalmemory_new_buffer(buf_sz)); /* memory from the ring buf */
    char *pout;

    if (out == 0) {
        return 0;
    }

    pout = out;

    for (p = str; *p != 0; p++) {

        if (*p == '\\') {
            *pout++ = '\\';
        }
        *pout++ = *p;
    }
    *pout++ = '\0';

    return out;
}

/**
 * @brief Constructor
 *
 * Create a new VComponent from a string.
 *
 * @exception ICAL_MALFORMEDDATA_ERROR
 *            Catch this error if you
 *
 */
VComponent::VComponent(const std::string &str) throw (icalerrorenum)
    : imp(icalcomponent_new_from_string(str.c_str()))
{
    if (imp == NULL) {
        if (! icalerrno) {
            icalerrno = ICAL_BADARG_ERROR;
        }
        throw icalerrno;
    }
}

VComponent::VComponent(const icalcomponent_kind &kind) throw(icalerrorenum)
    : imp(icalcomponent_new(kind))
{
    if (imp == NULL) {
        throw icalerrno;
    }
}

std::string VComponent::as_ical_string() throw(icalerrorenum)
{
    char *str = icalcomponent_as_ical_string(imp);

    if (str == NULL) {
        throw icalerrno;
    }

    return (str);
}

bool VComponent::is_valid()
{
    if (imp == NULL) {
        return false;
    }
    return (icalcomponent_is_valid(imp) != 0);
}

icalcomponent_kind VComponent::isa()
{
    return icalcomponent_isa(imp);
}

int VComponent::isa_component(void *component)
{
    return icalcomponent_isa_component(component);
}

void VComponent::new_from_string(const std::string &str)
{
    if (imp != NULL) {
        icalcomponent_free(imp);
    }
    imp = icalcomponent_new_from_string(str.c_str());
}

/* Working with properties */
void VComponent::add_property(ICalProperty *property)
{
    icalcomponent_add_property(imp, *property);
}

void VComponent::remove_property(ICalProperty *property)
{
    icalcomponent_remove_property(imp, *property);
    icalproperty_free(*property);
    property->detach();     // set imp to null, it's free already.
}

int VComponent::count_properties(const icalproperty_kind &kind)
{
    return icalcomponent_count_properties(imp, kind);
}

/* Iterate through the properties */
ICalProperty *VComponent::get_current_property()
{
    icalproperty *t = icalcomponent_get_current_property(imp);
    return ((t != NULL) ? new ICalProperty(t) : NULL);
}

ICalProperty *VComponent::get_first_property(const icalproperty_kind &kind)
{
    icalproperty *t = icalcomponent_get_first_property(imp, kind);
    return ((t != NULL) ? new ICalProperty(t) : NULL);
}

ICalProperty *VComponent::get_next_property(const icalproperty_kind &kind)
{
    icalproperty *t = icalcomponent_get_next_property(imp, kind);
    return ((t != NULL) ? new ICalProperty(t) : NULL);
}

/* Working with components */
/* Return the first VEVENT, VTODO or VJOURNAL sub-component if it is one of those types */
VComponent *VComponent::get_inner()
{
    return new VComponent(icalcomponent_get_inner(imp));
}

void VComponent::add_component(VComponent *child)
{
    icalcomponent_add_component(imp, *child);
}

void VComponent::remove_component(VComponent *child)
{
    icalcomponent_remove_component(imp, *child);
}

int VComponent::count_components(const icalcomponent_kind &kind)
{
    return  icalcomponent_count_components(imp, kind);
}

/* Iteration Routines. There are two forms of iterators, internal and
   external. The internal ones came first, and are almost completely
   sufficient, but they fail badly when you want to construct a loop that
   removes components from the container.
*/

/* Iterate through components */
VComponent *VComponent::get_current_component()
{
    icalcomponent *t = icalcomponent_get_current_component(imp);
    return ((t != NULL) ? new VComponent(t) : NULL);
}

VComponent *VComponent::get_first_component(const icalcomponent_kind &kind)
{
    VComponent *result = NULL;
    icalcomponent *t = icalcomponent_get_first_component(imp, kind);
    if (t != NULL) {
        switch (kind) {
        case ICAL_VALARM_COMPONENT:
            result = new VAlarm(t);
            break;
        case ICAL_VCALENDAR_COMPONENT:
            result = new VCalendar(t);
            break;
        case ICAL_VEVENT_COMPONENT:
            result = new VEvent(t);
            break;
        case ICAL_VQUERY_COMPONENT:
            result = new VQuery(t);
            break;
        case ICAL_VTODO_COMPONENT:
            result = new VToDo(t);
            break;
        case ICAL_VAGENDA_COMPONENT:
            result = new VAgenda(t);
            break;
        default:
            result = new VComponent(t);
        }
    }

    return (result);
}

VComponent *VComponent::get_next_component(const icalcomponent_kind &kind)
{
    VComponent *result = NULL;
    icalcomponent *t = icalcomponent_get_next_component(imp, kind);
    if (t != NULL) {
        switch (kind) {
        case ICAL_VALARM_COMPONENT:
            result = new VAlarm(t);
            break;
        case ICAL_VCALENDAR_COMPONENT:
            result = new VCalendar(t);
            break;
        case ICAL_VEVENT_COMPONENT:
            result = new VEvent(t);
            break;
        case ICAL_VQUERY_COMPONENT:
            result = new VQuery(t);
            break;
        case ICAL_VTODO_COMPONENT:
            result = new VToDo(t);
            break;
        case ICAL_VAGENDA_COMPONENT:
            result = new VAgenda(t);
            break;
        default:
            result = new VComponent(t);
        }
    }

    return (result);
}

/* Using external iterators */
icalcompiter VComponent::begin_component(const icalcomponent_kind &kind)
{
    return icalcomponent_begin_component(imp, kind);
}

icalcompiter VComponent::end_component(const icalcomponent_kind &kind)
{
    return icalcomponent_end_component(imp, kind);
}

VComponent *VComponent::next(icalcompiter *i)
{
    return reinterpret_cast<VComponent *>(icalcompiter_next(i));
}

VComponent *VComponent::prev(icalcompiter *i)
{
    return reinterpret_cast<VComponent *>(icalcompiter_prior(i));
}

VComponent *VComponent::current(icalcompiter *i)
{
    return reinterpret_cast<VComponent *>(icalcompiter_deref(i));
}

/* Working with embedded error properties */
int VComponent::count_errors()
{
    return icalcomponent_count_errors(imp);
}

/* Remove all X-LIC-ERROR properties*/
void VComponent::strip_errors()
{
    icalcomponent_strip_errors(imp);
}

/* Convert some X-LIC-ERROR properties into RETURN-STATUS properties*/
void VComponent::convert_errors()
{
    icalcomponent_convert_errors(imp);
}

/* Kind conversion routines */
icalcomponent_kind VComponent::string_to_kind(const std::string &str)
{
    return icalcomponent_string_to_kind(str.c_str());
}

std::string VComponent::kind_to_string(const icalcomponent_kind &kind)
{
    return static_cast<std::string>(icalcomponent_kind_to_string(kind));
}

struct icaltimetype VComponent::get_dtstart() const {
    return icalcomponent_get_dtstart(imp);
}

void VComponent::set_dtstart(const struct icaltimetype &v)
{
    icalcomponent_set_dtstart(imp, v);
}

/* For the icalcomponent routines only, dtend and duration are tied
   together. If you call the set routine for one and the other exists,
   the routine will calculate the change to the other. That is, if
   there is a DTEND and you call set_duration, the routine will modify
   DTEND to be the sum of DTSTART and the duration. If you call a get
   routine for one and the other exists, the routine will calculate
   the return value. If you call a set routine and neither exists, the
   routine will create the apcompriate comperty.
*/

struct icaltimetype VComponent::get_dtend() const {
    return icalcomponent_get_dtend(imp);
}

void VComponent::set_dtend(const struct icaltimetype &v)
{
    icalcomponent_set_dtend(imp, v);
}

struct icaltimetype VComponent::get_due() const {
    return icalcomponent_get_due(imp);
}

void VComponent::set_due(const struct icaltimetype &v)
{
    icalcomponent_set_due(imp, v);
}

struct icaldurationtype VComponent::get_duration() const {
    return icalcomponent_get_duration(imp);
}

void VComponent::set_duration(const struct icaldurationtype &v)
{
    icalcomponent_set_duration(imp, v);
}

icalproperty_method VComponent::get_method() const
{
    return icalcomponent_get_method(imp);
}

void VComponent::set_method(const icalproperty_method &method)
{
    icalcomponent_set_method(imp, method);
}

struct icaltimetype VComponent::get_dtstamp() const {
    return icalcomponent_get_dtstamp(imp);
}

void VComponent::set_dtstamp(const struct icaltimetype &v)
{
    icalcomponent_set_dtstamp(imp, v);
}

std::string VComponent::get_summary() const
{
    return static_cast<std::string>(icalcomponent_get_summary(imp));
}

void VComponent::set_summary(const std::string &v)
{
    icalcomponent_set_summary(imp, v.c_str());
}

std::string VComponent::get_location() const
{
    return static_cast<std::string>(icalcomponent_get_location(imp));
}

void VComponent::set_location(const std::string &v)
{
    icalcomponent_set_location(imp, v.c_str());
}

std::string VComponent::get_description() const
{
    return static_cast<std::string>(icalcomponent_get_description(imp));
}

void VComponent::set_description(const std::string &v)
{
    icalcomponent_set_description(imp, v.c_str());
}

std::string VComponent::get_comment() const
{
    return static_cast<std::string>(icalcomponent_get_comment(imp));
}

void VComponent::set_comment(const std::string &v)
{
    icalcomponent_set_comment(imp, v.c_str());
}

std::string VComponent::get_uid() const
{
    return static_cast<std::string>(icalcomponent_get_uid(imp));
}

void VComponent::set_uid(const std::string &v)
{
    icalcomponent_set_uid(imp, v.c_str());
}

std::string VComponent::get_relcalid() const
{
    return static_cast<std::string>(icalcomponent_get_relcalid(imp));
}

void VComponent::set_relcalid(const std::string &v)
{
    icalcomponent_set_relcalid(imp, v.c_str());
}

struct icaltimetype VComponent::get_recurrenceid() const {
    return icalcomponent_get_recurrenceid(imp);
}

void VComponent::set_recurrenceid(const struct icaltimetype &v)
{
    icalcomponent_set_recurrenceid(imp, v);
}

int VComponent::get_sequence() const
{
    return icalcomponent_get_sequence(imp);
}

void VComponent::set_sequence(const int &v)
{
    icalcomponent_set_sequence(imp, v);
}

int VComponent::get_status() const
{
    return icalcomponent_get_status(imp);
}

void VComponent::set_status(const enum icalproperty_status &v)
{
    icalcomponent_set_status(imp, v);
}

/* For VCOMPONENT: Return a reference to the first VEVENT, VTODO, or VJOURNAL */
VComponent *VComponent::get_first_real_component()
{
    return reinterpret_cast<VComponent *>(icalcomponent_get_first_real_component(imp));
}

/* For VEVENT, VTODO, VJOURNAL and VTIMEZONE: report the start and end
   times of an event in UTC */
struct icaltime_span VComponent::get_span()
{
    return icalcomponent_get_span(imp);
}

int VComponent::recurrence_is_excluded(struct icaltimetype *dtstart,
                                       struct icaltimetype *recurtime)
{
    return  icalproperty_recurrence_is_excluded(imp, dtstart, recurtime);
}

/* Internal operations. They are private, and you should not be using them. */
VComponent *VComponent::get_parent()
{
    return new VComponent(icalcomponent_get_parent(imp));
}

void VComponent::set_parent(VComponent *parent)
{
    icalcomponent_set_parent(imp, *parent);
}

/* ignoreValue means remove properties even if the data doesn't match */
bool VComponent::remove(VComponent &fromVC, bool ignoreValue)
{
    /* the two components must be the same kind */
    if (this->isa() != fromVC.isa()) {
        return false;
    }

    /* properties first */
    ICalPropertyTmpPtr propToBeRemoved;
    for (propToBeRemoved = fromVC.get_first_property(ICAL_ANY_PROPERTY);
         propToBeRemoved != NULL;
         propToBeRemoved = fromVC.get_next_property(ICAL_ANY_PROPERTY)) {

        /* loop through properties from this component */
        ICalPropertyTmpPtr next;
        ICalPropertyTmpPtr p;
        for (p = this->get_first_property(propToBeRemoved->isa()); p != NULL; p = next) {
            next = this->get_next_property(propToBeRemoved->isa());
            if (ignoreValue) {
                this->remove_property(p);
            } else {
                if (p == propToBeRemoved) {
                    this->remove_property(p);
                    break;
                }
            }
        }
    }

    /* components next - should remove by UID */
    VComponentTmpPtr comp;
    for (comp = fromVC.get_first_component(ICAL_ANY_COMPONENT); comp != NULL;
         comp = fromVC.get_next_component(ICAL_ANY_COMPONENT)) {
        const std::string fromCompUid = comp->get_uid();
        VComponentTmpPtr c;
        for (c = this->get_first_component(comp->isa()); c != NULL;
             c = this->get_next_component(comp->isa())) {
            if (strcmp(fromCompUid.c_str(), c->get_uid().c_str()) == 0) {
                // recursively go down the components
                c->remove(*comp, ignoreValue);
                // if all properties are removed and there is no sub-components, then
                // remove this compoent
                if ((c->count_properties(ICAL_ANY_PROPERTY) == 0) &&
                    (c->count_components(ICAL_ANY_COMPONENT) == 0)) {
                    this->remove_component(c);
                }
                break;
            }
        }
    }

    return true;
}

/* removeMissing == true: remove properties that are missing from fromC */
/* todo: only change the first occurrence of the property */
/* todo: removeMissing is not implemented */
bool VComponent::update(VComponent &fromC, bool removeMissing)
{
    /* make sure they are the same kind */
    if (this->isa() != fromC.isa()) {
        return false;
    }

    /* property first */
    ICalPropertyTmpPtr prop;
    for (prop = fromC.get_first_property(ICAL_ANY_PROPERTY); prop != NULL;
         prop = fromC.get_next_property(ICAL_ANY_PROPERTY)) {
        ICalPropertyTmpPtr thisProp;
        thisProp = this->get_first_property(prop->isa());
        if (thisProp == NULL) {
            thisProp = new ICalProperty(prop->isa());
            this->add_property(thisProp);
        }
        ICalValue *tempValue = prop->get_value();
        ICalValue *value = new ICalValue(*tempValue); // clone the value
        thisProp->set_value(*value);
        delete tempValue;
        delete value;
    }

    /* recursively updating sub-components */
    VComponentTmpPtr comp;
    for (comp = fromC.get_first_component(ICAL_ANY_COMPONENT); comp != NULL;
         comp = fromC.get_next_component(ICAL_ANY_COMPONENT)) {
        VComponentTmpPtr thisComp;
        thisComp = this->get_first_component(comp->isa());
        if (thisComp == NULL) {
            thisComp = new VComponent(comp->isa());
            this->add_component(thisComp);
        }
        bool err = thisComp->update(*comp, removeMissing);
        if (!err) {
            return false;
        }
    }
    return true;
}

/* add components and property. recursively goes down child components */
bool VComponent::add(VComponent &fromC)
{
    /* make sure the kind are the same */
    if (this->isa() != fromC.isa()) {
        return false;
    }

    /* properties first */
    ICalPropertyTmpPtr prop;
    for (prop = fromC.get_first_property(ICAL_ANY_PROPERTY); prop != NULL;
         prop = fromC.get_next_property(ICAL_ANY_PROPERTY)) {
        /* clone another property */
        ICalProperty *p = new ICalProperty(*prop);
        add_property(p);
        delete p;
    }

    /* sub-components next */
    bool err = false;
    VComponentTmpPtr comp;
    for (comp = fromC.get_first_component(ICAL_ANY_COMPONENT); comp != NULL;
         comp = fromC.get_next_component(ICAL_ANY_COMPONENT)) {
        VComponent *c = new VComponent(comp->isa());
        err = c->add(*comp);
        _unused(err);
        add_component(c);
        delete c;
    }

    return true;
}

VCalendar::VCalendar() : VComponent(icalcomponent_new_vcalendar())
{
}

VCalendar::VCalendar(const VCalendar &v) : VComponent(v)
{
}

VCalendar &VCalendar::operator=(const VCalendar &v)
{
    if (this == &v) {
        return *this;
    }
    VComponent::operator=(v);

    return *this;
}

VCalendar::~VCalendar()
{
}

VCalendar::VCalendar(icalcomponent *v) : VComponent(v)
{
}

VCalendar::VCalendar(const std::string &str) : VComponent(str)
{
}

/* VEvent */

VEvent::VEvent() : VComponent(icalcomponent_new_vevent())
{
}

VEvent::VEvent(const VEvent &v) : VComponent(v)
{
}

VEvent &VEvent::operator=(const VEvent &v)
{
    if (this == &v) {
        return *this;
    }
    VComponent::operator=(v);

    return *this;
}

VEvent::~VEvent()
{
}

VEvent::VEvent(icalcomponent *v) : VComponent(v)
{
}

VEvent::VEvent(const std::string &str) : VComponent(str)
{
}

/* VTodo */

VToDo::VToDo() : VComponent(icalcomponent_new_vtodo())
{
}

VToDo::VToDo(const VToDo &v) : VComponent(v)
{
}

VToDo &VToDo::operator=(const VToDo &v)
{
    if (this == &v) {
        return *this;
    }
    VComponent::operator=(v);

    return *this;
}

VToDo::~VToDo()
{
}

VToDo::VToDo(icalcomponent *v) : VComponent(v)
{
}

VToDo::VToDo(const std::string &str) : VComponent(str)
{
}

/* VAgenda */

VAgenda::VAgenda() : VComponent(icalcomponent_new_vagenda())
{
}

VAgenda::VAgenda(const VAgenda &v) : VComponent(v)
{
}

VAgenda &VAgenda::operator=(const VAgenda &v)
{
    if (this == &v) {
        return *this;
    }
    VComponent::operator=(v);

    return *this;
}

VAgenda::~VAgenda()
{
}

VAgenda::VAgenda(icalcomponent *v) : VComponent(v)
{
}

VAgenda::VAgenda(const std::string &str) : VComponent(str)
{
}

/* VQuery */

VQuery::VQuery() : VComponent(icalcomponent_new_vquery())
{
}

VQuery::VQuery(const VQuery &v) : VComponent(v)
{
}

VQuery &VQuery::operator=(const VQuery &v)
{
    if (this == &v) {
        return *this;
    }
    VComponent::operator=(v);

    return *this;
}

VQuery::~VQuery()
{
}

VQuery::VQuery(icalcomponent *v) : VComponent(v)
{
}

VQuery::VQuery(const std::string &str) : VComponent(str)
{
}

/* VJournal */

VJournal::VJournal() : VComponent(icalcomponent_new_vjournal())
{
}

VJournal::VJournal(const VJournal &v) : VComponent(v)
{
}

VJournal &VJournal::operator=(const VJournal &v)
{
    if (this == &v) {
        return *this;
    }
    VComponent::operator=(v);

    return *this;
}

VJournal::~VJournal()
{
}

VJournal::VJournal(icalcomponent *v) : VComponent(v)
{
}

VJournal::VJournal(const std::string &str) : VComponent(str)
{
}

/* VAlarm */

VAlarm::VAlarm() : VComponent(icalcomponent_new_valarm())
{
}

VAlarm::VAlarm(const VAlarm &v) : VComponent(v)
{
}

VAlarm &VAlarm::operator=(const VAlarm &v)
{
    if (this == &v) {
        return *this;
    }
    VComponent::operator=(v);

    return *this;
}

VAlarm::~VAlarm()
{
}

VAlarm::VAlarm(icalcomponent *v) : VComponent(v)
{
}

VAlarm::VAlarm(const std::string &str) : VComponent(str)
{
}

icalrequeststatus VAlarm::getTriggerTime(VComponent &c, struct icaltriggertype *tr)
{
    ICalPropertyTmpPtr trigger_prop = this->get_first_property(ICAL_TRIGGER_PROPERTY);

    // all VALARMs must have a TRIGGER
    if (trigger_prop == NULL) {
        return ICAL_3_1_INVPROPVAL_STATUS;
    }

    *tr = trigger_prop->get_trigger();

    if (icaltime_is_null_time(tr->time) == 1) {
        struct icaltimetype tt = icaltime_null_time();

        // relative time trigger

        // TRIGGER;RELATED=END:P5M 5 minutes after END
        // TRIGGER;RELATED=START:-P15M 15 minutes before START
        // get RELATED parameter

        ICalParameter *related_param = trigger_prop->get_first_parameter(ICAL_RELATED_PARAMETER);

        if ((related_param != NULL) && related_param->is_valid()) {

            // get RELATED parameter value
            icalparameter_related related = related_param->get_related();

            if (related != 0) {
                switch (related) {
                case ICAL_RELATED_END:
                    if (c.isa() == ICAL_VEVENT_COMPONENT) {
                        tt = c.get_dtend();

                        // If a recurrenceid exists, use that to calculate the
                        // dtend from the dtstart.
                        struct icaltimetype recur_time = c.get_recurrenceid();
                        if (icaltime_is_null_time(recur_time) != 1) {
                            struct icaldurationtype dur = icaltime_subtract(c.get_dtstart(), tt);
                            tt = icaltime_add(recur_time, dur);
                        }
                    } else if (c.isa() == ICAL_VTODO_COMPONENT) {
                        tt = c.get_due();
                        struct icaltimetype recur_time = c.get_recurrenceid();
                        if (icaltime_is_null_time(recur_time) != 1) {
                            tt = recur_time;
                        }
                    }
                    // @@@ TODO: if not DTEND or DUE, then DTSTART and DURATION must be present
                    break;
                case ICAL_RELATED_START:
                case ICAL_RELATED_X:
                case ICAL_RELATED_NONE:
                default:
                    tt = c.get_dtstart();
                    struct icaltimetype recur_time = c.get_recurrenceid();
                    if (icaltime_is_null_time(recur_time) != 1) {
                        tt = recur_time;
                    }
                    break;
                }
            }
        } else { // no RELATED explicitly specified, the default is
            // relative to the start of an event or to-do, rfc2445
            // if no RELATED, we are forced to use dtstart for VEVENT,
            // due for VTODO to calculate trigger time.
            // If a recur time exists, use that. Recur time trumps dtstart or due.
            struct icaltimetype recur_time = c.get_recurrenceid();
            if (icaltime_is_null_time(recur_time) != 1) {
                tt = recur_time;
            } else if (c.isa() == ICAL_VEVENT_COMPONENT) {
                tt = c.get_dtstart();
            } else if (c.isa() == ICAL_VTODO_COMPONENT) {
                tt = c.get_due();
            }
        }

        if (related_param != NULL) {
            icalmemory_free_buffer(related_param);
        }

        // malformed? encapsulating VEVENT or VTODO MUST have DTSTART/DTEND
        if (icaltime_is_null_time(tt) == 1) {
            return ICAL_3_1_INVPROPVAL_STATUS;
        };

        // now offset using tr.duration
        tr->time = icaltime_add(tt, tr->duration);
    }
    // else absolute time trigger

    return ICAL_2_0_SUCCESS_STATUS;
}

/* VFreeBusy */

VFreeBusy::VFreeBusy() : VComponent(icalcomponent_new_vfreebusy())
{
}

VFreeBusy::VFreeBusy(const VFreeBusy &v) : VComponent(v)
{
}

VFreeBusy &VFreeBusy::operator=(const VFreeBusy &v)
{
    if (this == &v) {
        return *this;
    }
    VComponent::operator=(v);

    return *this;
}

VFreeBusy::~VFreeBusy()
{
}

VFreeBusy::VFreeBusy(icalcomponent *v) : VComponent(v)
{
}

VFreeBusy::VFreeBusy(const std::string &str) : VComponent(str)
{
}

/* VTimezone */

VTimezone::VTimezone() : VComponent(icalcomponent_new_vtimezone())
{
}

VTimezone::VTimezone(const VTimezone &v) : VComponent(v)
{
}

VTimezone &VTimezone::operator=(const VTimezone &v)
{
    if (this == &v) {
        return *this;
    }
    VComponent::operator=(v);

    return *this;
}

VTimezone::~VTimezone()
{
}

VTimezone::VTimezone(icalcomponent *v) : VComponent(v)
{
}

VTimezone::VTimezone(const std::string &str) : VComponent(str)
{
}

/* XStandard */

XStandard::XStandard() : VComponent(icalcomponent_new_xstandard())
{
}

XStandard::XStandard(const XStandard &v) : VComponent(v)
{
}

XStandard &XStandard::operator=(const XStandard &v)
{
    if (this == &v) {
        return *this;
    }
    VComponent::operator=(v);

    return *this;
}

XStandard::~XStandard()
{
}

XStandard::XStandard(icalcomponent *v) : VComponent(v)
{
}

XStandard::XStandard(const std::string &str) : VComponent(str)
{
}

/* XDaylight */

XDaylight::XDaylight() : VComponent(icalcomponent_new_xdaylight())
{
}

XDaylight::XDaylight(const XDaylight &v) : VComponent(v)
{
}

XDaylight &XDaylight::operator=(const XDaylight &v)
{
    if (this == &v) {
        return *this;
    }
    VComponent::operator=(v);

    return *this;
}

XDaylight::~XDaylight()
{
}

XDaylight::XDaylight(icalcomponent *v) : VComponent(v)
{
}

XDaylight::XDaylight(const std::string &str) : VComponent(str)
{
}
