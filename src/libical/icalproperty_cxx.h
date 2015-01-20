/**
 * @file    icalproperty_cxx.h
 * @author  fnguyen (12/10/01)
 * @brief   Definition of C++ Wrapper for icalproperty.c
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

#ifndef ICALPROPERTY_CXX_H
#define ICALPROPERTY_CXX_H

extern "C" {
#include <libical/ical.h>
};
#include "icptrholder.h"

typedef char *string; // Will use the string library from STL

class ICalParameter;
class ICalValue;

class ICalProperty
{
public:
    ICalProperty();
    ICalProperty(const ICalProperty &) throw(icalerrorenum);
    ICalProperty &operator=(const ICalProperty &) throw(icalerrorenum);
    ~ICalProperty();

    explicit ICalProperty(icalproperty *v);
    explicit ICalProperty(string str);
    explicit ICalProperty(icalproperty_kind kind);
    ICalProperty(icalproperty_kind kind, string str);

    operator icalproperty *()
    {
        return imp;
    }
    int operator==(ICalProperty &rhs);

    void detach();

public:
    string as_ical_string();
    icalproperty_kind isa();
    int isa_property(void *property);

    void add_parameter(ICalParameter &parameter);
    void set_parameter(ICalParameter &parameter);
    void set_parameter_from_string(const string name, const string val);
    string get_parameter_as_string(const string name);
    void remove_parameter(icalparameter_kind kind);
    int count_parameters();

    /** Iterate through the parameters */
    ICalParameter *get_first_parameter(icalparameter_kind kind);
    ICalParameter *get_next_parameter(icalparameter_kind kind);

    /** Access the value of the property */
    void set_value(const ICalValue &val);
    void set_value_from_string(const string val, const string kind);

    ICalValue *get_value();
    string get_value_as_string();

    /** Return the name of the property -- the type name converted
     *  to a string, or the value of get_x_name if the type is X
     *  property
    */
    string get_name() const;

public:
    /* Deal with X properties */
    static void set_x_name(ICalProperty &prop, const string name);
    static string get_x_name(ICalProperty &prop);

    static icalvalue_kind icalparameter_value_to_value_kind(icalparameter_value val);

    /* Convert kinds to string and get default value type */
    static icalvalue_kind kind_to_value_kind(icalproperty_kind kind);
    static icalproperty_kind value_kind_to_kind(icalvalue_kind kind);
    static string kind_to_string(icalproperty_kind kind);
    static icalproperty_kind string_to_kind(const string str);

    static icalproperty_method string_to_method(const string str);
    static string method_to_string(icalproperty_method method);

    static string enum_to_string(int e);
    static int string_to_enum(const string str);

    static string status_to_string(icalproperty_status);
    static icalproperty_status string_to_status(const string str);

    static int enum_belongs_to_property(icalproperty_kind kind, int e);

public:
    /* ACTION */
    void set_action(const enum icalproperty_action v);
    enum icalproperty_action get_action();

    /* ATTACH */
    void set_attach(icalattach *v);
    icalattach *get_attach() const;

    /* ATTENDEE */
    void set_attendee(const string val);
    string get_attendee() const;

    /* CALSCALE */
    void set_calscale(const string val);
    string get_calscale() const;

    /* CATEGORIES */
    void set_categories(const string val);
    string get_categories() const;

    /* CLASS */
    void set_class(const enum icalproperty_class val);
    enum icalproperty_class get_class() const;

    /* COMMENT */
    void set_comment(const string val);
    string get_comment() const;

    /* COMPLETED */
    void set_completed(const struct icaltimetype val);
    struct icaltimetype get_completed() const;

    /* CONTACT */
    void set_contact(const string val);
    string get_contact() const;

    /* CREATED */
    void set_created(const struct icaltimetype val);
    struct icaltimetype get_created() const;

    /* DESCRIPTION */
    void set_description(const string val);
    string get_description() const;

    /* DTEND */
    void set_dtend(const struct icaltimetype val);
    struct icaltimetype get_dtend() const;

    /* DTSTAMP */
    void set_dtstamp(const struct icaltimetype val);
    struct icaltimetype get_dtstamp() const;

    /* DTSTART */
    void set_dtstart(const struct icaltimetype val);
    struct icaltimetype get_dtstart() const;

    /* DUE */
    void set_due(const struct icaltimetype val);
    struct icaltimetype get_due() const;

    /* DURATION */
    void set_duration(const struct icaldurationtype val);
    struct icaldurationtype get_duration() const;

    /* EXDATE */
    void set_exdate(const struct icaltimetype val);
    struct icaltimetype get_exdate() const;

    /* EXPAND */
    void set_expand(const int val);
    int get_expand() const;

    /* EXRULE */
    void set_exrule(const struct icalrecurrencetype val);
    struct icalrecurrencetype get_exrule() const;

    /* FREEBUSY */
    void set_freebusy(const struct icalperiodtype val);
    struct icalperiodtype get_freebusy() const;

    /* GEO */
    void set_geo(const struct icalgeotype val);
    struct icalgeotype get_geo() const;

    /* GRANT */
    void set_grant(const string val);
    string get_grant() const;

    /* LAST-MODIFIED */
    void set_lastmodified(const struct icaltimetype val);
    struct icaltimetype get_lastmodified() const;

    /* LOCATION */
    void set_location(const string val);
    string get_location() const;

    /* MAXRESULTS */
    void set_maxresults(const int val);
    int get_maxresults() const;

    /* MAXRESULTSSIZE */
    void set_maxresultsize(const int val);
    int get_maxresultsize() const;

    /* METHOD */
    void set_method(const enum icalproperty_method val);
    enum icalproperty_method get_method() const;

    /* OWNER */
    void set_owner(const string val);
    string get_owner() const;

    /* ORGANIZER */
    void set_organizer(const string val);
    string get_organizer() const;

    /* PERCENT-COMPLETE */
    void set_percentcomplete(const int val);
    int get_percentcomplete() const;

    /* PRIORITY */
    void set_priority(const int val);
    int get_priority() const;

    /* PRODID */
    void set_prodid(const string val);
    string get_prodid() const;

    /* QUERY */
    void set_query(const string val);
    string get_query() const;

    /* QUERYNAME */
    void set_queryname(const string val);
    string get_queryname() const;

    /* RDATE */
    void set_rdate(const struct icaldatetimeperiodtype val);
    struct icaldatetimeperiodtype get_rdate() const;

    /* RECURRENCE-ID */
    void set_recurrenceid(const struct icaltimetype val);
    struct icaltimetype get_recurrenceid() const;

    /* RELATED-TO */
    void set_relatedto(const string val);
    string get_relatedto() const;

    /* RELCALID */
    void set_relcalid(const string val);
    string get_relcalid() const;

    /* REPEAT */
    void set_repeat(const int val);
    int get_repeat() const;

    /* REQUEST-STATUS */
    void set_requeststatus(const string val);
    string get_requeststatus() const;

    /* RESOURCES */
    void set_resources(const string val);
    string get_resources() const;

    /* RRULE */
    void set_rrule(const struct icalrecurrencetype val);
    struct icalrecurrencetype get_rrule() const;

    /* SCOPE */
    void set_scope(const string val);
    string get_scope() const;

    /* SEQUENCE */
    void set_sequence(const int val);
    int get_sequence() const;

    /* STATUS */
    void set_status(const enum icalproperty_status val);
    enum icalproperty_status get_status() const;

    /* SUMMARY */
    void set_summary(const string val);
    string get_summary() const;

    /* TARGET */
    void set_target(const string val);
    string get_target() const;

    /* TRANSP */
    void set_transp(const enum icalproperty_transp val);
    enum icalproperty_transp get_transp() const;

    /* TRIGGER */
    void set_trigger(const struct icaltriggertype val);
    struct icaltriggertype get_trigger() const;

    /* TZID */
    void set_tzid(const string val);
    string get_tzid() const;

    /* TZNAME */
    void set_tzname(const string val);
    string get_tzname() const;

    /* TZOFFSETFROM */
    void set_tzoffsetfrom(const int val);
    int get_tzoffsetfrom() const;

    /* TZOFFSETTO */
    void set_tzoffsetto(const int val);
    int get_tzoffsetto() const;

    /* TZURL */
    void set_tzurl(const string val);
    string get_tzurl() const;

    /* UID */
    void set_uid(const string val);
    string get_uid() const;

    /* URL */
    void set_url(const string val);
    string get_url() const;

    /* VERSION */
    void set_version(const string val);
    string get_version() const;

    /* X */
    void set_x(const string val);
    string get_x() const;

    /* X-LIC-CLUSTERCOUNT */
    void set_xlicclustercount(const string val);
    string get_xlicclustercount() const;

    /* X-LIC-ERROR */
    void set_xlicerror(const string val);
    string get_xlicerror() const;

    /* X-LIC-MIMECHARSET */
    void set_xlicmimecharset(const string val);
    string get_xlicmimecharset() const;

    /* X-LIC-MIMECID */
    void set_xlicmimecid(const string val);
    string get_xlicmimecid() const;

    /* X-LIC-MIMECONTENTTYPE */
    void set_xlicmimecontenttype(const string val);
    string get_xlicmimecontenttype() const;

    /* X-LIC-MIMEENCODING */
    void set_xlicmimeencoding(const string val);
    string get_xlicmimeencoding() const;

    /* X-LIC-MIMEFILENAME */
    void set_xlicmimefilename(const string val);
    string get_xlicmimefilename() const;

    /* X-LIC-MIMEOPTINFO */
    void set_xlicmimeoptinfo(const string val);
    string get_xlicmimeoptinfo() const;

private:
    icalproperty *imp; /**< The actual C based icalproperty */
};

typedef ICPointerHolder<ICalProperty> ICalPropertyTmpPtr;   /* see icptrholder.h for comments */

#endif /* ICalProperty_H */
