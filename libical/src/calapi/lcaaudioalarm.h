// -*- Mode: C++ -*-
/*======================================================================
 FILE: LCAAudioAlarm.h
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


 $Id: lcaaudioalarm.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $
 $Locker:  $
 
======================================================================*/

#ifndef LCAAUDIOALARM_HH
#define LCAAUDIOALARM_HH 


class LCAAudioAlarm
{
   
public: // Methods

   LCAAudioAlarm(const LCAAudioAlarm &lcaaudioalarm);
   LCAAudioAlarm();
   ~LCAAudioAlarm();
   
   int operator==(const LCAAudioAlarm &lcaaudioalarm) const;  
   int operator!=(const LCAAudioAlarm &lcaaudioalarm) const;
 
protected: // Methods

protected: // Data

private: // Methods

private: // Data

};


/*============================================SCDLCAAudioAlarm

  CLASS:  LCAAudioAlarm 
  
  DESCRIPTION:
  
  HOW TO USE:

  PUBLIC MEMBER FUNCTION DOCUMENTATION:   

==============================================ECDLCAAudioAlarm*/

#ifdef LCAAUDIOALARM_CC
static const char vcid_LCAAUDIOALARM_HH[] = "$Id: lcaaudioalarm.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $";
#endif

#endif /* !LCAAUDIOALARM_HH */



