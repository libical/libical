/* -*- Mode: C -*- */
/*======================================================================
 FILE: icalgaugeimpl.h
 CREATOR: eric 09 Aug 2000


 $Id: icalgaugeimpl.h,v 1.1.1.1 2001-01-02 07:33:04 ebusboom Exp $
 $Locker:  $

 (C) COPYRIGHT 2000, Eric Busboom, http://www.softwarestudio.org

 This program is free software; you can redistribute it and/or modify
 it under the terms of either: 

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.fsf.org/copyleft/lesser.html

  Or:

    The Mozilla Public License Version 1.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/

======================================================================*/

#include "ical.h"

#include "pvl.h" 

typedef enum icalgaugecompare {
    ICALGAUGECOMPARE_NONE=0,
    ICALGAUGECOMPARE_EQUAL=1,
    ICALGAUGECOMPARE_LESS=2,
    ICALGAUGECOMPARE_LESSEQUAL=3,
    ICALGAUGECOMPARE_GREATER=4,
    ICALGAUGECOMPARE_GREATEREQUAL=5,
    ICALGAUGECOMPARE_NOTEQUAL=6,
    ICALGAUGECOMPARE_REGEX=7
} icalgaugecompare;


struct icalgauge_where {
	icalcomponent_kind comp;
	icalproperty_kind prop;
	icalgaugecompare compare;
	char* value;
};

struct icalgauge_impl
{

	pvl_list select; /*Of icalgaugecompare, using only prop and comp fields*/
	pvl_list from; /* List of component_kinds, as integers */
	pvl_list where; /* List of icalgaugecompare */
};


