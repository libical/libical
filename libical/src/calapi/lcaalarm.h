// -*- Mode: C++ -*-
/*======================================================================
  FILE: LCAAlarm.h
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


  $Id: lcaalarm.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $
  $Locker:  $
 
  ======================================================================*/

#ifndef LCAALARM_HH
#define LCAALARM_HH 

#include "lcaatrwrap.c"

#ifdef LCA_NO_LIC
typedef void icalcomponent;
#else
#include "icalcomponent.h"
#endif

class LCAAlarm
{
    public: // Types

	enum Relation {START, END, NONE};
	enum Type {RELATIVE, ABSOLUTE};

	// Attribute wrappers, use just like wraped types
	typedef LCAAtrWrap<enum Type, LCAAlarm> WType;	
	typedef LCAAtrWrap<enum Relation, LCAAlarm> WRelation;	

    public: // Methods

	LCAAlarm(const LCAAlarm &lcaalarm);
	LCAAlarm();
	~LCAAlarm();
   
	int operator==(const LCAAlarm &lcaalarm) const;  
	int operator!=(const LCAAlarm &lcaalarm) const;
	
    public: // Attributes

	int& Duration(); // in seconds, >=0
	int& Repeat(); // in seconds, >=0
	WType& TriggerType(); // Relative, Absolute
	WRelation& TriggerRelation(); // Start, End or None. 

    protected: // Methods

    protected: // Data

	icalcomponent* comp;

};


/*============================================SCDLCAAlarm

  CLASS:  LCAAlarm 
  
  DESCRIPTION:
  
  HOW TO USE:

  PUBLIC MEMBER FUNCTION DOCUMENTATION:   

==============================================ECDLCAAlarm*/

#ifdef LCAALARM_CC
static const char vcid_LCAALARM_HH[] = "$Id: lcaalarm.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $";
#endif

#endif /* !LCAALARM_HH */



