// -*- Mode: C++ -*-
/*======================================================================
 FILE: LCAJournalEntry.h
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


 $Id: lcajournalentry.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $
 $Locker:  $
 
======================================================================*/

#ifndef LCAJOURNALENTRY_HH
#define LCAJOURNALENTRY_HH 


class LCAJournalEntry
{
   
public: // Methods

   LCAJournalEntry(const LCAJournalEntry &lcajournalentry);
   LCAJournalEntry();
   ~LCAJournalEntry();
   
   int operator==(const LCAJournalEntry &lcajournalentry) const;  
   int operator!=(const LCAJournalEntry &lcajournalentry) const;
 
protected: // Methods

protected: // Data

private: // Methods

private: // Data

};


/*============================================SCDLCAJournalEntry

  CLASS:  LCAJournalEntry 
  
  DESCRIPTION:
  
  HOW TO USE:

  PUBLIC MEMBER FUNCTION DOCUMENTATION:   

==============================================ECDLCAJournalEntry*/

#ifdef LCAJOURNALENTRY_CC
static const char vcid_LCAJOURNALENTRY_HH[] = "$Id: lcajournalentry.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $";
#endif

#endif /* !LCAJOURNALENTRY_HH */



