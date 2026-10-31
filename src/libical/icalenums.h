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

/**
 * @file icalenums.h
 * @brief Defines enums not belonging to other data structures
 */

/***********************************************************************
 * Component enumerations
**********************************************************************/

/**
 * Kinds of components.
 */
typedef enum icalcomponent_kind
{
    ICAL_NO_COMPONENT,              /**< invalid component */
    ICAL_ANY_COMPONENT,             /**< Used to select all components */
    ICAL_XROOT_COMPONENT,           /**< The root component */
    ICAL_XATTACH_COMPONENT,         /**< MIME attached data, returned by parser */
    ICAL_VEVENT_COMPONENT,          /**< VEVENT */
    ICAL_VTODO_COMPONENT,           /**< VTODO */
    ICAL_VJOURNAL_COMPONENT,        /**< VJOURNAL */
    ICAL_VCALENDAR_COMPONENT,       /**< VCALENDAR */
    ICAL_VAGENDA_COMPONENT,         /**< VAGENDA */
    ICAL_VFREEBUSY_COMPONENT,       /**< VFREEBUSY */
    ICAL_VALARM_COMPONENT,          /**< VALARM */
    ICAL_XAUDIOALARM_COMPONENT,     /**< XAUDIOALARM */
    ICAL_XDISPLAYALARM_COMPONENT,   /**< XDISPLAYALARM */
    ICAL_XEMAILALARM_COMPONENT,     /**< XEMAILALARM */
    ICAL_XPROCEDUREALARM_COMPONENT, /**< XPROCEDUREALARM */
    ICAL_VTIMEZONE_COMPONENT,       /**< VTIMEZONE */
    ICAL_XSTANDARD_COMPONENT,       /**< XSTANDARD */
    ICAL_XDAYLIGHT_COMPONENT,       /**< XDAYLIGHT */
    ICAL_X_COMPONENT,               /**< X-NAME component */
    ICAL_VSCHEDULE_COMPONENT,       /**< VSCHEDULE */
    ICAL_VQUERY_COMPONENT,          /**< VQUERY */
    ICAL_VREPLY_COMPONENT,          /**< VREPLY */
    ICAL_VCAR_COMPONENT,            /**< VCAR */
    ICAL_VCOMMAND_COMPONENT,        /**< VCOMMAND */
    ICAL_XLICINVALID_COMPONENT,     /**< XLICINVALID */
    ICAL_XLICMIMEPART_COMPONENT,    /**< A non-stardard component that mirrors
                                           structure of MIME data */
    ICAL_VAVAILABILITY_COMPONENT,   /**< VAVAILABILITY */
    ICAL_XAVAILABLE_COMPONENT,      /**< XAVAILABLE */
    ICAL_VPOLL_COMPONENT,           /**< VPOLL */
    ICAL_VVOTER_COMPONENT,          /**< VVOTER */
    ICAL_XVOTE_COMPONENT,           /**< XVOTE */
    ICAL_VPATCH_COMPONENT,          /**< VPATCH */
    ICAL_XPATCH_COMPONENT,          /**< XPATCH */
    ICAL_PARTICIPANT_COMPONENT,     /**< PARTICIPANT */
    ICAL_VLOCATION_COMPONENT,       /**< VLOCATION */
    ICAL_VRESOURCE_COMPONENT,       /**< VRESOURCE */
    ICAL_IANA_COMPONENT,            /**< IANA */
    ICAL_NUM_COMPONENT_TYPES        /**< MUST be last (unless we can put NO_COMP last) */
} icalcomponent_kind;

/***********************************************************************
 * Request Status codes
 **********************************************************************/

/**
 * Request Status codes.
 */
typedef enum icalrequeststatus
{
    ICAL_UNKNOWN_STATUS,          /**< Error: Unknown request status */
    ICAL_2_0_SUCCESS_STATUS,      /**< Success */
    ICAL_2_1_FALLBACK_STATUS,     /**< Success but fallback taken on one or more property values */
    ICAL_2_2_IGPROP_STATUS,       /**< Success, invalid property ignored */
    ICAL_2_3_IGPARAM_STATUS,      /**< Success, invalid property parameter ignored */
    ICAL_2_4_IGXPROP_STATUS,      /**< Success, unknown non-standard property ignored */
    ICAL_2_5_IGXPARAM_STATUS,     /**< Success, unknown non standard property value ignored */
    ICAL_2_6_IGCOMP_STATUS,       /**< Success, invalid calendar component ignored */
    ICAL_2_7_FORWARD_STATUS,      /**< Success, request forwarded to Calendar User */
    ICAL_2_8_ONEEVENT_STATUS,     /**< Success, repeating event ignored. Scheduled as a single component */
    ICAL_2_9_TRUNC_STATUS,        /**< Success, truncated end date time to date boundary */
    ICAL_2_10_ONETODO_STATUS,     /**< Success, repeating VTODO ignored. Scheduled as a single VTODO */
    ICAL_2_11_TRUNCRRULE_STATUS,  /**< Success, unbounded RRULE clipped at some finite number of instances */
    ICAL_3_0_INVPROPNAME_STATUS,  /**< Invalid property name */
    ICAL_3_1_INVPROPVAL_STATUS,   /**< Invalid property value */
    ICAL_3_2_INVPARAM_STATUS,     /**< Invalid property parameter */
    ICAL_3_3_INVPARAMVAL_STATUS,  /**< Invalid property parameter value */
    ICAL_3_4_INVCOMP_STATUS,      /**< Invalid calendar component */
    ICAL_3_5_INVTIME_STATUS,      /**< Invalid date or time */
    ICAL_3_6_INVRULE_STATUS,      /**< Invalid rule */
    ICAL_3_7_INVCU_STATUS,        /**< Invalid Calendar User */
    ICAL_3_8_NOAUTH_STATUS,       /**< No authority */
    ICAL_3_9_BADVERSION_STATUS,   /**< Unsupported version */
    ICAL_3_10_TOOBIG_STATUS,      /**< Request entity too large */
    ICAL_3_11_MISSREQCOMP_STATUS, /**< Required component or property missing */
    ICAL_3_12_UNKCOMP_STATUS,     /**< Unknown component or property found */
    ICAL_3_13_BADCOMP_STATUS,     /**< Unsupported component or property found */
    ICAL_3_14_NOCAP_STATUS,       /**< Unsupported capability */
    ICAL_3_15_INVCOMMAND,         /**< Invalid command */
    ICAL_4_0_BUSY_STATUS,         /**< Event conflict. Date/time is busy */
    ICAL_4_1_STORE_ACCESS_DENIED, /**< Store Access Denied */
    ICAL_4_2_STORE_FAILED,        /**< Store Failed */
    ICAL_4_3_STORE_NOT_FOUND,     /**< Store not found */
    ICAL_5_0_MAYBE_STATUS,        /**< Request MAY supported */
    ICAL_5_1_UNAVAIL_STATUS,      /**< Service unavailable */
    ICAL_5_2_NOSERVICE_STATUS,    /**< Invalid calendar service */
    ICAL_5_3_NOSCHED_STATUS,      /**< No scheduling support for user */
    ICAL_6_1_CONTAINER_NOT_FOUND, /**< Container not found */
    ICAL_9_0_UNRECOGNIZED_COMMAND /**< An unrecognized command was received */
} icalrequeststatus;

/**
 * Gets the descriptive text for an icalrequeststatus.
 *
 * @param stat is the icalrequeststatus to use
 *
 * @return a pointer to a char string containing the icalrequeststatus description.
 * NULL is returned if @p stat is invalid.
 */
LIBICAL_ICAL_EXPORT const char *icalenum_reqstat_desc(icalrequeststatus stat);

/**
 * Gets the major number for an icalrequeststatus.
 *
 * @param stat is the icalrequeststatus to use
 *
 * @return the major number of the specified icalrequeststatus.
 */
LIBICAL_ICAL_EXPORT short icalenum_reqstat_major(icalrequeststatus stat);

/**
 * Gets the minor number for an icalrequeststatus.
 *
 * @param stat is the icalrequeststatus to use
 *
 * @return the minor number of the specified icalrequeststatus.
 */
LIBICAL_ICAL_EXPORT short icalenum_reqstat_minor(icalrequeststatus stat);

/**
 * Get an icalrequeststatus for major.minor status numbers.
 *
 * @param major is the major request number
 * @param minor is the minor request number
 *
 * @return an icalrequeststatus representation of the major.minor request number.
 */
LIBICAL_ICAL_EXPORT icalrequeststatus icalenum_num_to_reqstat(short major, short minor);

/**
 * Gets a string representation of an icalrequeststatus.
 *
 * @param stat the icalrequeststatus
 *
 * @return a pointer to a char string representation of the specified icalrequest
status.
 * see icalenum_reqstat_code_r
 */
LIBICAL_ICAL_EXPORT char *icalenum_reqstat_code(icalrequeststatus stat);

/**
 * Gets a string representation of an icalrequeststatus.
 *
 * @param stat the icalrequeststatus
 *
 * @return a pointer to a char string representation of the specified icalrequeststatus.
 * see icalenum_reqstat_code
 */
LIBICAL_ICAL_EXPORT char *icalenum_reqstat_code_r(icalrequeststatus stat);

#endif /* !ICALENUMS_H */
