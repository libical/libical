// -*- Mode: C++ -*-
/*======================================================================
 FILE: LCAAtrWrap.h
 CREATOR: eric 30 September 2000

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


 $Id: lcaatrwrap.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $
 $Locker:  $
 
======================================================================*/

#ifndef LCAATRWRAP_HH
#define LCAATRWRAP_HH 

/* Wrap an attribute. This class is constructed with
     v, an attribute value
     t, The container for of the attribute
     rfv, ppointer to member function of t
     lvf, pointer to member function of t

   Using the wrapper as an L-value will return an reference to v, and call 
   to lvf. 

   Using the wrapper as an R-value will return a reference to v and call 
   to rvf. 

   Thus, you can read and assign to the attribute v as a regular
   reference, and intercept and changes to the attribute

*/


template <class V,class T> class LCAAtrWrap {
    public:

	typedef void (T::*MemFunc)();     

	Wrap(V& v, T& t, MemFunc rvf, MemFunc lvf ): rvalf(rvf),
	    lvalf(lvf), value(v), target(t) {}

	V& operator=(const V& v){ 
	    value = v;
	    if ( rvalf != 0) {target.rval();}
	    return value;
	}

	operator V&() const { 
	    if (lvalf != 0) {target.lval();}
	    return value;
	}
	
	MemFunc rvalf;
	MemFunc lvalf;
	V &value;
	T &target;
};
            


/*============================================SCDLCAAtrWrap

  CLASS:  LCAAtrWrap 
  
  DESCRIPTION:
  
  HOW TO USE:

  PUBLIC MEMBER FUNCTION DOCUMENTATION:   

==============================================ECDLCAAtrWrap*/

#ifdef LCAATRWRAP_CC
static const char vcid_LCAATRWRAP_HH[] = "$Id: lcaatrwrap.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $";
#endif

#endif /* !LCAATRWRAP_HH */



