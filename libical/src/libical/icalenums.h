
/* -*- Mode: C -*-*/
/*======================================================================
 FILE: icalenums.h

 

 (C) COPYRIGHT 2000, Eric Busboom, http://www.softwarestudio.org

 This program is free software; you can redistribute it and/or modify
 it under the terms of either: 

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.fsf.org/copyleft/lesser.html

  Or:

    The Mozilla Public License Version 1.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/

  The original code is icalenums.h

  Contributions from:
     Graham Davison (g.m.davison@computer.org)

======================================================================*/

#ifndef ICALENUMS_H
#define ICALENUMS_H



/***********************************************************************
 * Component enumerations
**********************************************************************/

typedef enum icalcomponent_kind {
    ICAL_NO_COMPONENT,
    ICAL_ANY_COMPONENT,	/* Used to select all components*/
    ICAL_XROOT_COMPONENT,
    ICAL_XATTACH_COMPONENT, /* MIME attached data, returned by parser. */
    ICAL_VEVENT_COMPONENT,
    ICAL_VTODO_COMPONENT,
    ICAL_VJOURNAL_COMPONENT,
    ICAL_VCALENDAR_COMPONENT,
    ICAL_VFREEBUSY_COMPONENT,
    ICAL_VALARM_COMPONENT,
    ICAL_XAUDIOALARM_COMPONENT,  
    ICAL_XDISPLAYALARM_COMPONENT,
    ICAL_XEMAILALARM_COMPONENT,
    ICAL_XPROCEDUREALARM_COMPONENT,
    ICAL_VTIMEZONE_COMPONENT,
    ICAL_XSTANDARD_COMPONENT,
    ICAL_XDAYLIGHT_COMPONENT,
    ICAL_X_COMPONENT,
    ICAL_VSCHEDULE_COMPONENT,
    ICAL_VQUERY_COMPONENT,
    ICAL_VCAR_COMPONENT,
    ICAL_VCOMMAND_COMPONENT,
    ICAL_XLICINVALID_COMPONENT,
    ICAL_XLICMIMEPART_COMPONENT /* a non-stardard component that mirrors
				structure of MIME data */

} icalcomponent_kind;

/***********************************************************************
 * Property Enumerations
**********************************************************************/

typedef enum icalproperty_kind {
    ICAL_ANY_PROPERTY = 0, /* This must be the first enum, for iteration */
    ICAL_CALSCALE_PROPERTY,
    ICAL_METHOD_PROPERTY,
    ICAL_PRODID_PROPERTY,
    ICAL_VERSION_PROPERTY,
    ICAL_ATTACH_PROPERTY,
    ICAL_CATEGORIES_PROPERTY,
    ICAL_CLASS_PROPERTY,
    ICAL_COMMENT_PROPERTY,
    ICAL_DESCRIPTION_PROPERTY,
    ICAL_GEO_PROPERTY,
    ICAL_LOCATION_PROPERTY,
    ICAL_PERCENTCOMPLETE_PROPERTY,
    ICAL_PRIORITY_PROPERTY,
    ICAL_RESOURCES_PROPERTY,
    ICAL_STATUS_PROPERTY,
    ICAL_SUMMARY_PROPERTY,
    ICAL_COMPLETED_PROPERTY,
    ICAL_DTEND_PROPERTY,
    ICAL_DUE_PROPERTY,
    ICAL_DTSTART_PROPERTY,
    ICAL_DURATION_PROPERTY,
    ICAL_FREEBUSY_PROPERTY,
    ICAL_TRANSP_PROPERTY,
    ICAL_TZID_PROPERTY,
    ICAL_TZNAME_PROPERTY,
    ICAL_TZOFFSETFROM_PROPERTY,
    ICAL_TZOFFSETTO_PROPERTY,
    ICAL_TZURL_PROPERTY,
    ICAL_ATTENDEE_PROPERTY,
    ICAL_CONTACT_PROPERTY,
    ICAL_ORGANIZER_PROPERTY,
    ICAL_RECURRENCEID_PROPERTY,
    ICAL_RELATEDTO_PROPERTY,
    ICAL_URL_PROPERTY,
    ICAL_UID_PROPERTY,
    ICAL_EXDATE_PROPERTY,
    ICAL_EXRULE_PROPERTY,
    ICAL_RDATE_PROPERTY,
    ICAL_RRULE_PROPERTY,
    ICAL_ACTION_PROPERTY,
    ICAL_REPEAT_PROPERTY,
    ICAL_TRIGGER_PROPERTY,
    ICAL_CREATED_PROPERTY,
    ICAL_DTSTAMP_PROPERTY,
    ICAL_LASTMODIFIED_PROPERTY,
    ICAL_SEQUENCE_PROPERTY,
    ICAL_REQUESTSTATUS_PROPERTY,
    ICAL_X_PROPERTY,

    /* CAP Properties */
    ICAL_SCOPE_PROPERTY,
    ICAL_MAXRESULTS_PROPERTY,
    ICAL_MAXRESULTSSIZE_PROPERTY,
    ICAL_QUERY_PROPERTY,
    ICAL_QUERYNAME_PROPERTY, 
    ICAL_TARGET_PROPERTY,

    /* libical private properties */
    ICAL_XLICERROR_PROPERTY,
    ICAL_XLICCLUSTERCOUNT_PROPERTY,
    ICAL_XLICMIMECONTENTTYPE_PROPERTY,
    ICAL_XLICMIMEENCODING_PROPERTY,
    ICAL_XLICMIMECID_PROPERTY,
    ICAL_XLICMIMEFILENAME_PROPERTY,
    ICAL_XLICMIMECHARSET_PROPERTY,
    ICAL_XLICMIMEOPTINFO_PROPERTY,

    ICAL_NO_PROPERTY /* This must be the last enum, for iteration */

} icalproperty_kind;

/***********************************************************************
 * Enumerations for the values of properties
 ***********************************************************************/

typedef enum icalproperty_method {
    ICAL_METHOD_PUBLISH,
    ICAL_METHOD_REQUEST,
    ICAL_METHOD_REPLY,
    ICAL_METHOD_ADD,
    ICAL_METHOD_CANCEL,
    ICAL_METHOD_REFRESH,
    ICAL_METHOD_COUNTER,
    ICAL_METHOD_DECLINECOUNTER,
    /* CAP Methods */
    ICAL_METHOD_CREATE,
    ICAL_METHOD_READ,
    ICAL_METHOD_RESPONSE,
    ICAL_METHOD_MOVE,
    ICAL_METHOD_MODIFY,
    ICAL_METHOD_GENERATEUID,
    ICAL_METHOD_DELETE,
    ICAL_METHOD_NONE
} icalproperty_method ;

typedef enum icalproperty_transp {
    ICAL_TRANSP_OPAQUE,
    ICAL_TRANS_TRANSPARENT
}  icalproperty_trans;

typedef enum icalproperty_calscale {
    ICAL_CALSCALE_GREGORIAN
} icalproperty_calscale ;


typedef enum icalproperty_class {
    ICAL_CLASS_PUBLIC,
    ICAL_CLASS_PRIVATE,
    ICAL_CLASS_CONFIDENTIAL,
    ICAL_CLASS_XNAME
} icalproperty_class;


typedef enum icalproperty_status {
    ICAL_STATUS_NONE,
    ICAL_STATUS_TENTATIVE,
    ICAL_STATUS_CONFIRMED,
    ICAL_STATUS_CANCELLED, /* CANCELED? SIC from RFC*/
    ICAL_STATUS_NEEDSACTION,
    ICAL_STATUS_COMPLETED,
    ICAL_STATUS_INPROCESS,
    ICAL_STATUS_DRAFT,
    ICAL_STATUS_FINAL
}  icalproperty_status;

typedef enum icalproperty_action {
    ICAL_ACTION_AUDIO,
    ICAL_ACTION_DISPLAY,
    ICAL_ACTION_EMAIL,
    ICAL_ACTION_PROCEDURE,
    ICAL_ACTION_XNAME
} icalproperty_action;

/***********************************************************************
 * Value enumerations
**********************************************************************/

typedef enum icalvalue_kind {
    ICAL_NO_VALUE,
    ICAL_ATTACH_VALUE, /* Non-Standard*/
    ICAL_BINARY_VALUE,
    ICAL_BOOLEAN_VALUE,
    ICAL_CALADDRESS_VALUE,
    ICAL_DATE_VALUE,
    ICAL_DATETIME_VALUE,
    ICAL_DATETIMEDATE_VALUE, /* Non-Standard */
    ICAL_DATETIMEPERIOD_VALUE, /* Non-Standard */
    ICAL_DURATION_VALUE,
    ICAL_FLOAT_VALUE,
    ICAL_GEO_VALUE, /* Non-Standard */
    ICAL_INTEGER_VALUE,
    ICAL_METHOD_VALUE, /* Non-Standard */
    ICAL_STATUS_VALUE, /* Non-Standard */
    ICAL_PERIOD_VALUE,
    ICAL_RECUR_VALUE,
    ICAL_STRING_VALUE, /* Non-Standard */
    ICAL_TEXT_VALUE,
    ICAL_TIME_VALUE,
    ICAL_TRIGGER_VALUE, /* Non-Standard */
    ICAL_URI_VALUE,
    ICAL_UTCOFFSET_VALUE,
    ICAL_QUERY_VALUE,
    ICAL_XNAME_VALUE
} icalvalue_kind;


/***********************************************************************
 * Parameter Enumerations
 **********************************************************************/


typedef enum icalparameter_kind {
    ICAL_NO_PARAMETER,
    ICAL_ANY_PARAMETER,
    ICAL_ALTREP_PARAMETER, /* DQUOTE uri DQUOTE */
    ICAL_CN_PARAMETER, /* text */
    ICAL_CUTYPE_PARAMETER, /*INDIVIDUAL, GROUP, RESOURCE,ROOM,UNKNOWN, x-name*/
    ICAL_DELEGATEDFROM_PARAMETER, /* *("," DQUOTE cal-address DQUOTE) */
    ICAL_DELEGATEDTO_PARAMETER, /*  *("," DQUOTE cal-address DQUOTE) */
    ICAL_DIR_PARAMETER, /*  DQUOTE uri DQUOTE */
    ICAL_ENCODING_PARAMETER, /* *BIT, BASE64, x-name */
    ICAL_FMTTYPE_PARAMETER, /* registered MINE content type */
    ICAL_FBTYPE_PARAMETER, /* FREE, BUSY, BUSY-UNAVAILABLE, BUSY-TENTATIVE,x-name */
    ICAL_LANGUAGE_PARAMETER, /* text from RFC 1766 */
    ICAL_MEMBER_PARAMETER, /*  DQUOTE cal-address DQUOTE */
    ICAL_PARTSTAT_PARAMETER, /* NEEDS-ACTION, ACCEPTED, DECLINED, TENTATIVE, DELEGATED, x-name */
    ICAL_RANGE_PARAMETER, /* THISANDPRIOR, THISANDFUTURE */
    ICAL_RELATED_PARAMETER, /* START, END */
    ICAL_RELTYPE_PARAMETER, /* PARENT, CHILD, SIBLING,x-name */
    ICAL_ROLE_PARAMETER, /* CHAIR, REQ_PARTICIPANT, OPT_PARTICIPANT, NON_PARTICIPANT, x-name */
    ICAL_RSVP_PARAMETER, /* TRUE. FALSE */
    ICAL_SENTBY_PARAMETER, /*  DQUOTE uri DQUOTE */
    ICAL_TZID_PARAMETER, /*  [tzidprefix] paramtext CRLF */
    ICAL_VALUE_PARAMETER, /* BINARY, BOOLEAN, CAL_ADDRESS, DATE, DATE-TIME, DURATION, FLOAT, INTEGER, PERIOD, RECUR, TEXT, TIME, UTC_OFFSET, x-name */
    ICAL_XLICERRORTYPE_PARAMETER, /*ICAL_XLICERROR_PARSE_ERROR,ICAL_XLICERROR_INVALID_ITIP*/
    ICAL_XLICCOMPARETYPE_PARAMETER, /**/
    ICAL_X_PARAMETER /* text */ 
} icalparameter_kind;

typedef enum icalparameter_cutype {
    ICAL_CUTYPE_INDIVIDUAL, 
    ICAL_CUTYPE_GROUP, 
    ICAL_CUTYPE_RESOURCE, 
    ICAL_CUTYPE_ROOM,
    ICAL_CUTYPE_UNKNOWN,
    ICAL_CUTYPE_XNAME
} icalparameter_cutype;


typedef enum icalparameter_encoding {
    ICAL_ENCODING_8BIT, 
    ICAL_ENCODING_BASE64,
    ICAL_ENCODING_XNAME
} icalparameter_encoding;

typedef enum icalparameter_fbtype {
    ICAL_FBTYPE_FREE, 
    ICAL_FBTYPE_BUSY, 
    ICAL_FBTYPE_BUSYUNAVAILABLE, 
    ICAL_FBTYPE_BUSYTENTATIVE,
    ICAL_FBTYPE_XNAME
} icalparameter_fbtype;

typedef enum icalparameter_partstat {
    ICAL_PARTSTAT_NEEDSACTION, 
    ICAL_PARTSTAT_ACCEPTED, 
    ICAL_PARTSTAT_DECLINED, 
    ICAL_PARTSTAT_TENTATIVE, 
    ICAL_PARTSTAT_DELEGATED,
    ICAL_PARTSTAT_COMPLETED,
    ICAL_PARTSTAT_INPROCESS,
    ICAL_PARTSTAT_XNAME,
    ICAL_PARTSTAT_NONE
} icalparameter_partstat;

typedef enum icalparameter_range {
    ICAL_RANGE_THISANDPRIOR, 
    ICAL_RANGE_THISANDFUTURE
} icalparameter_range;

typedef enum icalparameter_related {
    ICAL_RELATED_START, 
    ICAL_RELATED_END
} icalparameter_related;

typedef enum icalparameter_reltype {
    ICAL_RELTYPE_PARENT, 
    ICAL_RELTYPE_CHILD,
    ICAL_RELTYPE_SIBLING,
    ICAL_RELTYPE_XNAME
} icalparameter_reltype;

typedef enum icalparameter_role {
    ICAL_ROLE_CHAIR, 
    ICAL_ROLE_REQPARTICIPANT, 
    ICAL_ROLE_OPTPARTICIPANT, 
    ICAL_ROLE_NONPARTICIPANT,
    ICAL_ROLE_XNAME
} icalparameter_role;

typedef enum icalparameter_xlicerrortype {
    ICAL_XLICERRORTYPE_COMPONENTPARSEERROR,
    ICAL_XLICERRORTYPE_PARAMETERVALUEPARSEERROR,
    ICAL_XLICERRORTYPE_PARAMETERNAMEPARSEERROR,
    ICAL_XLICERRORTYPE_PROPERTYPARSEERROR,
    ICAL_XLICERRORTYPE_VALUEPARSEERROR,
    ICAL_XLICERRORTYPE_UNKVCALPROP,
    ICAL_XLICERRORTYPE_INVALIDITIP,
    ICAL_XLICERRORTYPE_MIMEPARSEERROR
} icalparameter_xlicerrortype;

typedef enum icalparameter_xliccomparetype {
    ICAL_XLICCOMPARETYPE_EQUAL=0,
    ICAL_XLICCOMPARETYPE_LESS=-1,
    ICAL_XLICCOMPARETYPE_LESSEQUAL=2,
    ICAL_XLICCOMPARETYPE_GREATER=1,
    ICAL_XLICCOMPARETYPE_GREATEREQUAL=3,
    ICAL_XLICCOMPARETYPE_NOTEQUAL=4,
    ICAL_XLICCOMPARETYPE_REGEX=5
} icalparameter_xliccomparetype;

typedef enum icalparameter_value {
    ICAL_VALUE_XNAME = ICAL_XNAME_VALUE,
    ICAL_VALUE_BINARY = ICAL_BINARY_VALUE, 
    ICAL_VALUE_BOOLEAN = ICAL_BOOLEAN_VALUE, 
    ICAL_VALUE_CALADDRESS = ICAL_CALADDRESS_VALUE, 
    ICAL_VALUE_DATE = ICAL_DATE_VALUE, 
    ICAL_VALUE_DATETIME = ICAL_DATETIME_VALUE, 
    ICAL_VALUE_DURATION = ICAL_DURATION_VALUE, 
    ICAL_VALUE_FLOAT = ICAL_FLOAT_VALUE, 
    ICAL_VALUE_INTEGER = ICAL_INTEGER_VALUE, 
    ICAL_VALUE_PERIOD = ICAL_PERIOD_VALUE, 
    ICAL_VALUE_RECUR = ICAL_RECUR_VALUE, 
    ICAL_VALUE_TEXT = ICAL_TEXT_VALUE, 
    ICAL_VALUE_TIME = ICAL_TIME_VALUE, 
    ICAL_VALUE_UTCOFFSET = ICAL_UTCOFFSET_VALUE,
    ICAL_VALUE_URI = ICAL_URI_VALUE,
    ICAL_VALUE_ERROR = ICAL_NO_VALUE
} icalparameter_value;


/***********************************************************************
 * Request Status codes
 **********************************************************************/

typedef enum icalrequeststatus {
    ICAL_UNKNOWN_STATUS,
    ICAL_2_0_SUCCESS_STATUS,
    ICAL_2_1_FALLBACK_STATUS,
    ICAL_2_2_IGPROP_STATUS,
    ICAL_2_3_IGPARAM_STATUS,
    ICAL_2_4_IGXPROP_STATUS,
    ICAL_2_5_IGXPARAM_STATUS,
    ICAL_2_6_IGCOMP_STATUS,
    ICAL_2_7_FORWARD_STATUS,
    ICAL_2_8_ONEEVENT_STATUS,
    ICAL_2_9_TRUNC_STATUS,
    ICAL_2_10_ONETODO_STATUS,
    ICAL_2_11_TRUNCRRULE_STATUS,
    ICAL_3_0_INVPROPNAME_STATUS,
    ICAL_3_1_INVPROPVAL_STATUS,
    ICAL_3_2_INVPARAM_STATUS,
    ICAL_3_3_INVPARAMVAL_STATUS,
    ICAL_3_4_INVCOMP_STATUS,
    ICAL_3_5_INVTIME_STATUS,
    ICAL_3_6_INVRULE_STATUS,
    ICAL_3_7_INVCU_STATUS,
    ICAL_3_8_NOAUTH_STATUS,
    ICAL_3_9_BADVERSION_STATUS,
    ICAL_3_10_TOOBIG_STATUS,
    ICAL_3_11_MISSREQCOMP_STATUS,
    ICAL_3_12_UNKCOMP_STATUS,
    ICAL_3_13_BADCOMP_STATUS,
    ICAL_3_14_NOCAP_STATUS,
    ICAL_4_0_BUSY_STATUS,
    ICAL_5_0_MAYBE_STATUS,
    ICAL_5_1_UNAVAIL_STATUS,
    ICAL_5_2_NOSERVICE_STATUS,
    ICAL_5_3_NOSCHED_STATUS
} icalrequeststatus;


const char* icalenum_reqstat_desc(icalrequeststatus stat);
short icalenum_reqstat_major(icalrequeststatus stat);
short icalenum_reqstat_minor(icalrequeststatus stat);
icalrequeststatus icalenum_num_to_reqstat(short major, short minor);

/***********************************************************************
 * Conversion functions
**********************************************************************/

const char* icalenum_property_kind_to_string(icalproperty_kind kind);
icalproperty_kind icalenum_string_to_property_kind(const char* string);

const char* icalenum_value_kind_to_string(icalvalue_kind kind);
icalvalue_kind icalenum_value_kind_by_prop(icalproperty_kind kind);

const char* icalenum_parameter_kind_to_string(icalparameter_kind kind);
icalparameter_kind icalenum_string_to_parameter_kind(const char* string);

const char* icalenum_component_kind_to_string(icalcomponent_kind kind);
icalcomponent_kind icalenum_string_to_component_kind(const char* string);

icalvalue_kind icalenum_property_kind_to_value_kind(icalproperty_kind kind);

const char* icalenum_method_to_string(icalproperty_method);
icalproperty_method icalenum_string_to_method(const char* string);

const char* icalenum_status_to_string(icalproperty_status);
icalproperty_status icalenum_string_to_status(const char* string);

#endif /* !ICALENUMS_H */



