// -*- Mode: C++ -*-
/*======================================================================
  FILE: LCAMessageFactory.h
  CREATOR: eric 06 October 2000

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


  $Id: lcamessagefactory.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $
  $Locker:  $
 
  ======================================================================*/

#ifndef LCAMESSAGEFACTORY_HH
#define LCAMESSAGEFACTORY_HH 


class LCAMessageFactory
{
   
    public: // Methods

	LCAMessageFactory(const LCAMessageFactory &lcamessagefactory);
	LCAMessageFactory();
	~LCAMessageFactory();
   
	int operator==(const LCAMessageFactory &lcamessagefactory) const;  
	int operator!=(const LCAMessageFactory &lcamessagefactory) const;

	static LCAMessage& Request();
 
	static LCAMessage& AcceptReply(LCAMessage &m);
	static LCAMessage& RejectReply(LCAMessage &m);
	static LCAMessage& Counterproposal(LCAMessage &m);
	static LCAMessage& Update(LCAMessage &m); // RESCHEDULE if caller changes time
	static LCAMessage& Cancel(LCAMessage &m);
	static LCAMessage& Delegate(LCAMessage &m);


    protected: // Methods

    protected: // Data

    private: // Methods

    private: // Data

};


/*============================================SCDLCAMessageFactory

  CLASS:  LCAMessageFactory 
  
  DESCRIPTION:
  
  HOW TO USE:

  PUBLIC MEMBER FUNCTION DOCUMENTATION:   

==============================================ECDLCAMessageFactory*/

#ifdef LCAMESSAGEFACTORY_CC
static const char vcid_LCAMESSAGEFACTORY_HH[] = "$Id: lcamessagefactory.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $";
#endif

#endif /* !LCAMESSAGEFACTORY_HH */



