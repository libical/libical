// -*- Mode: C++ -*-
/*======================================================================
  FILE: LCAIncident.h
  CREATOR: eric 22 September 2000

  DESCRIPTION:

  LCAIncident is the base calss for Meetings, Todos and Journal entries. 

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


  $Id: lcaincident.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $
  $Locker:  $
 
  ======================================================================*/

#ifndef LCAINCIDENT_HH
#define LCAINCIDENT_HH 

#ifdef LCA_NO_LIC
typedef void icalcomponent;
#else
extern "C" {
#include "ical.h"
}
#endif

#include <string>
#include <list>
#include "lcaattendee.h"
#include "lcaatrwrap.h"

class LCAIncident
{
    public: // Types

    public: // Methods

	LCAIncident(const LCAIncident &lcaincident);
	LCAIncident();
	~LCAIncident();
   
	int  operator==(const LCAIncident &lcaincident) const;  
	int operator!=(const LCAIncident &lcaincident) const;

    public: // Attributes

	LCAOccurrenceSet& OccurrenceSet();

	LCAAttendee& Organizer();
	LCAAttendeeSet& AttendeeSet();

	list<string>& Categories();
	string& Class();
	string& Description();
	string& Comment();
	string& Summary();
	string& Location();

	LCATime& Stamp();
	LCATime& LastModified();
	LCATime& Created();
	const string& UID(); // read-only
	int& Sequence();
	LCATime& RecurrenceId(); 
 
    protected: // Methods

    protected: // Data

	icalcomponent* comp;
};


/*============================================SCDLCAIncident

  CLASS:  LCAIncident 
  
  DESCRIPTION:
  
  HOW TO USE:

  PUBLIC MEMBER FUNCTION DOCUMENTATION:   

==============================================ECDLCAIncident*/

#ifdef LCAINCIDENT_CC
static const char vcid_LCAINCIDENT_HH[] = "$Id: lcaincident.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $";
#endif

#endif /* !LCAINCIDENT_HH */



