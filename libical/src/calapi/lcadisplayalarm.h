// -*- Mode: C++ -*-
/*======================================================================
 FILE: LCADisplayAlarm.h
 CREATOR: eric 28 September 2000

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


 $Id: lcadisplayalarm.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $
 $Locker:  $
 
======================================================================*/

#ifndef LCADISPLAYALARM_HH
#define LCADISPLAYALARM_HH 


class LCADisplayAlarm
{
   
public: // Methods

   LCADisplayAlarm(const LCADisplayAlarm &lcadisplayalarm);
   LCADisplayAlarm();
   ~LCADisplayAlarm();
   
   int operator==(const LCADisplayAlarm &lcadisplayalarm) const;  
   int operator!=(const LCADisplayAlarm &lcadisplayalarm) const;
 
protected: // Methods

protected: // Data

private: // Methods

private: // Data

};


/*============================================SCDLCADisplayAlarm

  CLASS:  LCADisplayAlarm 
  
  DESCRIPTION:
  
  HOW TO USE:

  PUBLIC MEMBER FUNCTION DOCUMENTATION:   

==============================================ECDLCADisplayAlarm*/

#ifdef LCADISPLAYALARM_CC
static const char vcid_LCADISPLAYALARM_HH[] = "$Id: lcadisplayalarm.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $";
#endif

#endif /* !LCADISPLAYALARM_HH */



