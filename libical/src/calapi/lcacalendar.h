// -*- Mode: C++ -*-
/*======================================================================
  FILE: LCACalendar.h
  CREATOR: eric 22 September 2000

  DESCRIPTION:

  PUBLIC CLASSES AND FUNCTIONS:

  (C) COPYRIGHT 2000, Eric Busboom, eric@softwarestudio.org
  http://www.softwarestudio.org

  This program is free software; you can redistribute it and/or modify
  it under the terms of either: 

  The LGPL as published by the Free Software Foundation, version
  2.1, available at: http://www.fsf.org/copyleft/lesser.html

  Or:

  The Mozilla Public License Version 1.0. You may obtain a copy of
  the License at http://www.mozilla.org/MPL/


  $Id: lcacalendar.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $
  $Locker:  $
 
  ======================================================================*/

#ifndef LCACALENDAR_HH
#define LCACALENDAR_HH 

#include <multiset>
#include "lcaincident.h"
#include "lcamessage.h"

#include "lcastore.h"

class LCACalendar
{
    public: // Types
	
	typedef multiset<LCAIncident&> event_set;
	typedef multiset<LCAMessate&> message_set;
	
    public: // Methods
	
	LCACalendar(const LCACalendar &lcacalendar);
	LCACalendar(LCAStore &store);
	~LCACalendar();
	
	int operator==(const LCACalendar &lcacalendar) const;  
	int operator!=(const LCACalendar &lcacalendar) const;
	
	event_set& EventSet(); // Events on calendar
	message_set& MessageSet(); // Events waint to be processed

	int BookEvent(LCAMessage()); // Move a message into event set
	

	// Calendar properties (Attributes)

	LCAAttendee& Attendee();
	const LCATime& StartTime(): // Read Only
	const LCATime& EndTime(): // Read Only

	// Forming composite calendars

	//void AddSubCalendar(LCACalendar& cal);
	//void RemoveSubCalendar(LCACalendar& cal);

    protected: // Methods
	
    protected: // Data

	

};


/*============================================SCDLCACalendar

  CLASS:  LCACalendar 
  
  DESCRIPTION:
  
  HOW TO USE:

  PUBLIC MEMBER FUNCTION DOCUMENTATION:   

==============================================ECDLCACalendar*/

#ifdef LCACALENDAR_CC
static const char vcid_LCACALENDAR_HH[] = "$Id: lcacalendar.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $";
#endif

#endif /* !LCACALENDAR_HH */



