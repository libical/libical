// -*- Mode: C++ -*-
/*======================================================================
  FILE: LCAAttendee.h
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


  $Id: lcaattendee.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $
  $Locker:  $
 
  ======================================================================*/

#ifndef LCAATTENDEE_HH
#define LCAATTENDEE_HH 

#include <set>


class LCAAttendee
{
    public: // Types

	// Int assignments are used for indexing. do not change */
	enum Status {NONE=0,NEEDSACTION=1,ACCEPTED=2,DECLINED=3,TENTATIVE=4,
		     DELEGATED=5,COMPLETED=6,INPROCESS=7,XNAME=8};

	enum CuType {NONE=0,INDIVIDUAL=1,GROUP=2,RESOURCE=3,ROOM=4,XNAME=5};

	enum Role {NONE=0,CHAIR=1,REQPARTICIPANT=2,OPTPARTICIPANT=3,
		   NONPARTICIPANT=4,XNAME=5}

    public: // Methods

	LCAAttendee(const LCAAttendee &lcaattendee);
	LCAAttendee();
	~LCAAttendee();
   
	int operator==(const LCAAttendee &lcaattendee) const;  
	int operator!=(const LCAAttendee &lcaattendee) const;

    public: // Attributes

	string& CommonName();
	string& CSID();
	enum Status& Status();
	bool& RSVP();
	enum CuType CuType();
	enum Role Role();
	string& SendBy();
	string& DelFrom();
	string& DelTo();
	string& Language();

    protected: // Methods

    protected: // Data

};

typedef set<LCAAttendee> LCAAttendeeSet;


/*============================================SCDLCAAttendee

  CLASS:  LCAAttendee 
  
  DESCRIPTION:
  
  HOW TO USE:

  PUBLIC MEMBER FUNCTION DOCUMENTATION:   

==============================================ECDLCAAttendee*/

#ifdef LCAATTENDEE_CC
static const char vcid_LCAATTENDEE_HH[] = "$Id: lcaattendee.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $";
#endif

#endif /* !LCAATTENDEE_HH */



