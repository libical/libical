// -*- Mode: C++ -*-
/*======================================================================
  FILE: LCAMessage.h
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


  $Id: lcamessage.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $
  $Locker:  $
 
  ======================================================================*/

#ifndef LCAMESSAGE_HH
#define LCAMESSAGE_HH 

#include "lcaincident.h" 


class LCAMessage
{
    public: // Types

	enum Type {
	    NONE,PUBLISH,NEW_EVENT,UPDATE,RESCHEDULE,
	    DELEGATE,NEW_ORGANIZER,FORWARD,
	    REQUEST_STATUS,ACCEPT,DECLINE,CRASHER_ACCEPT,
	    CRASHER_DECLINE,ADD_INSTANCE,CANCEL_EVENT,CANCEL_INSTANCE,
	    CANCEL_ALL,REFRESH,COUNTER,DECLINECOUNTER,MALFORMED, 
	    OBSOLETE,MISSEQUENCED
	};
   
    public: // Methods

	LCAMessage(const LCAMessage &lcamessage);
	LCAMessage();
	~LCAMessage();
   
	int operator==(const LCAMessage &lcamessage) const;  
	int operator!=(const LCAMessage &lcamessage) const;
 
	int FindMatch() const;

    public: // Attributes

	LCAIncident& Event() const;
	LCAIncident& Match() const;
	const string& Sender() const; // Read Only
	const string& Recipient() const; // Read Only
	const enum Type Type() const; // Read Only

    protected: // Methods

    protected: // Data

};


/*============================================SCDLCAMessage

  CLASS:  LCAMessage 
  
  DESCRIPTION:
  
  HOW TO USE:

  PUBLIC MEMBER FUNCTION DOCUMENTATION:   

==============================================ECDLCAMessage*/

#ifdef LCAMESSAGE_CC
static const char vcid_LCAMESSAGE_HH[] = "$Id: lcamessage.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $";
#endif

#endif /* !LCAMESSAGE_HH */



