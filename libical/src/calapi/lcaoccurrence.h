// -*- Mode: C++ -*-
/*======================================================================
  FILE: LCAOccurrence.h
  CREATOR: eric 22 September 2000

  DESCRIPTION:

  An LCAOccurrence descibes a single occurrence of an event, including
  the start time, end time, etc. 

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


  $Id: lcaoccurrence.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $
  $Locker:  $
 
  ======================================================================*/

#ifndef LCAOCCURRENCE_HH
#define LCAOCCURRENCE_HH 


class LCAOccurrence
{
   
    public: // Methods

	LCAOccurrence(const LCAOccurrence &lcaoccurrence);
	LCAOccurrence();
	~LCAOccurrence();
   
	int operator==(const LCAOccurrence &lcaoccurrence) const;  
	int operator!=(const LCAOccurrence &lcaoccurrence) const;

	// Start time
	// End Time
	// Duration
	// Sequence Number, if one of a series

    protected: // Methods

    protected: // Data


};


/*============================================SCDLCAOccurrence

  CLASS:  LCAOccurrence 
  
  DESCRIPTION:
  
  HOW TO USE:

  PUBLIC MEMBER FUNCTION DOCUMENTATION:   

==============================================ECDLCAOccurrence*/

#ifdef LCAOCCURRENCE_CC
static const char vcid_LCAOCCURRENCE_HH[] = "$Id: lcaoccurrence.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $";
#endif

#endif /* !LCAOCCURRENCE_HH */



