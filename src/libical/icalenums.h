/*======================================================================
 FILE: icalenums.h

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 Contributions from:
    Graham Davison <g.m.davison@computer.org>
======================================================================*/

#ifndef ICALENUMS_H
#define ICALENUMS_H

#include "libical_ical_export.h"

/** @file icalenums.h */

/***********************************************************************
 * Component enumerations
**********************************************************************/

typedef enum icalcomponent_kind
{
    ICAL_NO_COMPONENT,
    ICAL_ANY_COMPONENT, /* Used to select all components */
    ICAL_XROOT_COMPONENT,
    ICAL_XATTACH_COMPONENT, /* MIME attached data, returned by parser. */
    ICAL_VEVENT_COMPONENT,
    ICAL_VTODO_COMPONENT,
    ICAL_VJOURNAL_COMPONENT,
    ICAL_VCALENDAR_COMPONENT,
    ICAL_VAGENDA_COMPONENT,
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
    ICAL_VREPLY_COMPONENT,
    ICAL_VCAR_COMPONENT,
    ICAL_VCOMMAND_COMPONENT,
    ICAL_XLICINVALID_COMPONENT,
    ICAL_XLICMIMEPART_COMPONENT, /* A non-stardard component that mirrors
                                           structure of MIME data */
    ICAL_VAVAILABILITY_COMPONENT,
    ICAL_XAVAILABLE_COMPONENT,
    ICAL_VPOLL_COMPONENT,
    ICAL_VVOTER_COMPONENT,
    ICAL_XVOTE_COMPONENT,
    ICAL_VPATCH_COMPONENT,
    ICAL_XPATCH_COMPONENT,
    ICAL_PARTICIPANT_COMPONENT,
    ICAL_VLOCATION_COMPONENT,
    ICAL_VRESOURCE_COMPONENT,
    ICAL_NUM_COMPONENT_TYPES /* MUST be last (unless we can put NO_COMP last) */
} icalcomponent_kind;

/***********************************************************************
 * Request Status codes
 **********************************************************************/

typedef enum icalrequeststatus
{
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
    ICAL_3_15_INVCOMMAND,
    ICAL_4_0_BUSY_STATUS,
    ICAL_4_1_STORE_ACCESS_DENIED,
    ICAL_4_2_STORE_FAILED,
    ICAL_4_3_STORE_NOT_FOUND,
    ICAL_5_0_MAYBE_STATUS,
    ICAL_5_1_UNAVAIL_STATUS,
    ICAL_5_2_NOSERVICE_STATUS,
    ICAL_5_3_NOSCHED_STATUS,
    ICAL_6_1_CONTAINER_NOT_FOUND,
    ICAL_9_0_UNRECOGNIZED_COMMAND
} icalrequeststatus;

/** @brief Returns the descriptive text for a request status.
 */
LIBICAL_ICAL_EXPORT const char *icalenum_reqstat_desc(icalrequeststatus stat);

/** @brief Returns the major number for a request status
 */
LIBICAL_ICAL_EXPORT short icalenum_reqstat_major(icalrequeststatus stat);

/** @brief Returns the minor number for a request status
 */
LIBICAL_ICAL_EXPORT short icalenum_reqstat_minor(icalrequeststatus stat);

/** @brief Returns a request status for major/minor status numbers
 */
LIBICAL_ICAL_EXPORT icalrequeststatus icalenum_num_to_reqstat(short major, short minor);

LIBICAL_ICAL_EXPORT char *icalenum_reqstat_code(icalrequeststatus stat);

/** @brief Returns the code for a request status.
 */
LIBICAL_ICAL_EXPORT char *icalenum_reqstat_code_r(icalrequeststatus stat);

/***********************************************************************
 * Conversion functions
**********************************************************************/

/* These routines used to be in icalenums.c, but were moved into the
   icalproperty, icalparameter, icalvalue, or icalcomponent modules. */

/*const char* icalproperty_kind_to_string(icalproperty_kind kind);*/
#define icalenum_property_kind_to_string(x) icalproperty_kind_to_string(x)

/*icalproperty_kind icalproperty_string_to_kind(const char* string)*/
#define icalenum_string_to_property_kind(x) icalproperty_string_to_kind(x)

/*icalvalue_kind icalproperty_kind_to_value_kind(icalproperty_kind kind);*/
#define icalenum_property_kind_to_value_kind(x) icalproperty_kind_to_value_kind(x)

/*const char* icalenum_method_to_string(icalproperty_method);*/
#define icalenum_method_to_string(x) icalproperty_method_to_string(x)

/*icalproperty_method icalenum_string_to_method(const char* string);*/
#define icalenum_string_to_method(x) icalproperty_string_to_method(x)

/*const char* icalenum_status_to_string(icalproperty_status);*/
#define icalenum_status_to_string(x) icalproperty_status_to_string(x)

/*icalproperty_status icalenum_string_to_status(const char* string);*/
#define icalenum_string_to_status(x) icalproperty_string_to_status(x)

/*icalvalue_kind icalenum_string_to_value_kind(const char* str);*/
#define icalenum_string_to_value_kind(x) icalvalue_string_to_kind(x)

/*const char* icalenum_value_kind_to_string(icalvalue_kind kind);*/
#define icalenum_value_kind_to_string(x) icalvalue_kind_to_string(x)

/*const char* icalenum_component_kind_to_string(icalcomponent_kind kind);*/
#define icalenum_component_kind_to_string(x) icalcomponent_kind_to_string(x)

/*icalcomponent_kind icalenum_string_to_component_kind(const char* string);*/
#define icalenum_string_to_component_kind(x) icalcomponent_string_to_kind(x)

/*const char* icalenum_action_to_string(icalproperty_action);*/
#define icalenum_action_to_string(x) icalproperty_action_to_string(x)

/*icalproperty_action icalenum_string_to_action(const char* string);*/
#define icalenum_string_to_action(x) icalproperty_string_to_action(x)

/*const char* icalenum_transp_to_string(icalproperty_transp);*/
#define icalenum_transp_to_string(x) icalproperty_transp_to_string(x)

/*icalproperty_transp icalenum_string_to_transp(const char* string);*/
#define icalenum_string_to_transp(x) icalproperty_string_to_transp(x)

/*const char* icalenum_class_to_string(icalproperty_class);*/
#define icalenum_class_to_string(x) icalproperty_class_to_string(x)

/*icalproperty_class icalenum_string_to_class(const char* string);*/
#define icalenum_string_to_class(x) icalproperty_string_to_class(x)

/*const char* icalenum_participanttype_to_string(icalproperty_participanttype);*/
#define icalenum_participanttype_to_string(x) icalproperty_participanttype_to_string(x)

/*icalproperty_participanttype icalenum_string_to_participanttype(const char* string);*/
#define icalenum_string_to_participanttype(x) icalproperty_string_to_participanttype(x)

/*const char* icalenum_resourcetype_to_string(icalproperty_resourcetype);*/
#define icalenum_resourcetype_to_string(x) icalproperty_resourcetype_to_string(x)

/*icalproperty_resourcetype icalenum_string_to_resourcetype(const char* string);*/
#define icalenum_string_to_resourcetype(x) icalproperty_string_to_resourcetype(x)

#endif /* !ICALENUMS_H */
