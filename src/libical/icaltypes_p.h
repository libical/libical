/*======================================================================
 FILE: icaltypes_p.h

 SPDX-FileCopyrightText: Allen Winter <winter@kde.org>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
======================================================================*/

/**
 * @file icaltypes_p.h
 * @brief Define private internal types.
 */

#ifndef ICALTYPES_P_H
#define ICALTYPES_P_H

/**
 * Identifiers for structure types.
 */
typedef enum
{
    ICAL_STRUCTURE_TYPE_UNKNOWN,         /** Unknown structure type */
    ICAL_STRUCTURE_TYPE_VALUE,           /** Populated value structure */
    ICAL_STRUCTURE_TYPE_VALUE_EMPTY,     /** Empty value structure */
    ICAL_STRUCTURE_TYPE_PARAMETER,       /** Populated parameter structure */
    ICAL_STRUCTURE_TYPE_PARAMETER_EMPTY, /** Empty parameter structure */
    ICAL_STRUCTURE_TYPE_PROPERTY,        /** Populated property structure */
    ICAL_STRUCTURE_TYPE_PROPERTY_EMPTY,  /** Empty property structure */
    ICAL_STRUCTURE_TYPE_COMPONENT,       /** Populated component structure */
    ICAL_STRUCTURE_TYPE_COMPONENT_EMPTY, /** Empty component structure */
} icalstructuretype;

#endif /* !ICALTYPES_P_H */
