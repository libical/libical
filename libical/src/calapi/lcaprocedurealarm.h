// -*- Mode: C++ -*-
/*======================================================================
 FILE: LCAProcedureAlarm.h
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


 $Id: lcaprocedurealarm.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $
 $Locker:  $
 
======================================================================*/

#ifndef LCAPROCEDUREALARM_HH
#define LCAPROCEDUREALARM_HH 


class LCAProcedureAlarm
{
   
public: // Methods

   LCAProcedureAlarm(const LCAProcedureAlarm &lcaprocedurealarm);
   LCAProcedureAlarm();
   ~LCAProcedureAlarm();
   
   int operator==(const LCAProcedureAlarm &lcaprocedurealarm) const;  
   int operator!=(const LCAProcedureAlarm &lcaprocedurealarm) const;
 
protected: // Methods

protected: // Data

private: // Methods

private: // Data

};


/*============================================SCDLCAProcedureAlarm

  CLASS:  LCAProcedureAlarm 
  
  DESCRIPTION:
  
  HOW TO USE:

  PUBLIC MEMBER FUNCTION DOCUMENTATION:   

==============================================ECDLCAProcedureAlarm*/

#ifdef LCAPROCEDUREALARM_CC
static const char vcid_LCAPROCEDUREALARM_HH[] = "$Id: lcaprocedurealarm.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $";
#endif

#endif /* !LCAPROCEDUREALARM_HH */



