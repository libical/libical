// -*- Mode: C++ -*-
/*======================================================================
  FILE: LCATimeZone.h
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


  $Id: lcatimezone.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $
  $Locker:  $
 
  ======================================================================*/

#ifndef LCATIMEZONE_HH
#define LCATIMEZONE_HH 


class LCATimeZone
{
   
    public: // Methods

	LCATimeZone(const LCATimeZone &lcatimezone);
	LCATimeZone();
	~LCATimeZone();
   
	int operator==(const LCATimeZone &lcatimezone) const;  
	int operator!=(const LCATimeZone &lcatimezone) const;
 
    protected: // Methods

    protected: // Data

    private: // Methods

    private: // Data

};


/*============================================SCDLCATimeZone

  CLASS:  LCATimeZone 
  
  DESCRIPTION:
  
  HOW TO USE:

  PUBLIC MEMBER FUNCTION DOCUMENTATION:   

==============================================ECDLCATimeZone*/

#ifdef LCATIMEZONE_CC
static const char vcid_LCATIMEZONE_HH[] = "$Id: lcatimezone.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $";
#endif

#endif /* !LCATIMEZONE_HH */



