// -*- Mode: C++ -*-
/*======================================================================
 FILE: LCAFBList.h
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


 $Id: lcafblist.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $
 $Locker:  $
 
======================================================================*/

#ifndef LCAFBLIST_HH
#define LCAFBLIST_HH 


class LCAFBList
{
   
public: // Methods

   LCAFBList(const LCAFBList &lcafblist);
   LCAFBList();
   ~LCAFBList();
   
   int operator==(const LCAFBList &lcafblist) const;  
   int operator!=(const LCAFBList &lcafblist) const;
 
protected: // Methods

protected: // Data

private: // Methods

private: // Data

};


/*============================================SCDLCAFBList

  CLASS:  LCAFBList 
  
  DESCRIPTION:
  
  HOW TO USE:

  PUBLIC MEMBER FUNCTION DOCUMENTATION:   

==============================================ECDLCAFBList*/

#ifdef LCAFBLIST_CC
static const char vcid_LCAFBLIST_HH[] = "$Id: lcafblist.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $";
#endif

#endif /* !LCAFBLIST_HH */



