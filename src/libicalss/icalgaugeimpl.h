/*======================================================================
 FILE: icalgaugeimpl.h
 CREATOR: eric 09 Aug 2000

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
======================================================================*/

#ifndef ICALGAUGEIMPL_H
#define ICALGAUGEIMPL_H

#include "icalcomponent.h"

typedef enum icalgaugecompare
{
    ICALGAUGECOMPARE_EQUAL = ICAL_XLICCOMPARETYPE_EQUAL,
    ICALGAUGECOMPARE_LESS = ICAL_XLICCOMPARETYPE_LESS,
    ICALGAUGECOMPARE_LESSEQUAL = ICAL_XLICCOMPARETYPE_LESSEQUAL,
    ICALGAUGECOMPARE_GREATER = ICAL_XLICCOMPARETYPE_GREATER,
    ICALGAUGECOMPARE_GREATEREQUAL = ICAL_XLICCOMPARETYPE_GREATEREQUAL,
    ICALGAUGECOMPARE_NOTEQUAL = ICAL_XLICCOMPARETYPE_NOTEQUAL,
    ICALGAUGECOMPARE_REGEX = ICAL_XLICCOMPARETYPE_REGEX,
    ICALGAUGECOMPARE_ISNULL = ICAL_XLICCOMPARETYPE_ISNULL,
    ICALGAUGECOMPARE_ISNOTNULL = ICAL_XLICCOMPARETYPE_ISNOTNULL,
    ICALGAUGECOMPARE_NONE = 0
} icalgaugecompare;

typedef enum icalgaugelogic
{
    ICALGAUGELOGIC_NONE,
    ICALGAUGELOGIC_AND,
    ICALGAUGELOGIC_OR
} icalgaugelogic;

struct icalgauge_where {
    icalgaugelogic logic;
    icalcomponent_kind comp;
    icalproperty_kind prop;
    icalgaugecompare compare;
    char *value;
};

struct icalgauge_impl {
    icalpvl_list select; /**< Of icalgaugecompare, using only prop and comp fields*/
    icalpvl_list from;   /**< List of component_kinds, as integers */
    icalpvl_list where;  /**< List of icalgaugecompare */
    int expand;
};

#endif
