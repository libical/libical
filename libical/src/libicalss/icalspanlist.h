/* -*- Mode: C -*- */
/*======================================================================
 FILE: icalspanlist.h
 CREATOR: eric 21 Aug 2000


 $Id: icalspanlist.h,v 1.3 2002-06-11 18:53:00 acampi Exp $
 $Locker:  $

 (C) COPYRIGHT 2000, Eric Busboom, http://www.softwarestudio.org

 This program is free software; you can redistribute it and/or modify
 it under the terms of either: 

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.fsf.org/copyleft/lesser.html

  Or:

    The Mozilla Public License Version 1.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/


 =========================================================================*/
#ifndef ICALSPANLIST_H
#define ICALSPANLIST_H

#include "ical.h"
#include "icalset.h"

/** @file icalspanlist.h
 *  @brief Code that supports collections of free/busy spans of time
 */

typedef struct icalspanlist_impl icalspanlist;


/** @brief Constructor
 * Make a free list from a set of component. Start and end should be in UTC 
 */

icalspanlist* icalspanlist_new(icalset *set, 
				struct icaltimetype start,
				struct icaltimetype end);

/** @brief Destructor
 */
void icalspanlist_free(icalspanlist* spl);

icalcomponent* icalspanlist_make_free_list(icalspanlist* sl);
icalcomponent* icalspanlist_make_busy_list(icalspanlist* sl);

/**
 * Get first free or busy time after time t. all times are in UTC 
 */
struct icalperiodtype icalspanlist_next_free_time(icalspanlist* sl,
						struct icaltimetype t);
struct icalperiodtype icalspanlist_next_busy_time(icalspanlist* sl,
						struct icaltimetype t);

void icalspanlist_dump(icalspanlist* s);

/** @brief Return a valid VFREEBUSY component for this span
 */
icalcomponent *icalspanlist_as_vfreebusy(icalspanlist* s_in,
					 const char* organizer,
					 const char* attendee);

#endif
				    


