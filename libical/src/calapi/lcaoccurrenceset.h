// -*- Mode: C++ -*-
/*======================================================================
  FILE: LCAOccurrenceSet.h
  CREATOR: eric 22 September 2000

  DESCRIPTION:

  LCAOccurrrenceSet represents a set of occurences. For most events,
  the set has only one element, but the LCAOccurrenceSet can also
  represent many occurrences, as given by the iCAL properties RRULE,
  EXRULE, RDATE and EXDATE

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


  $Id: lcaoccurrenceset.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $
  $Locker:  $
 
  ======================================================================*/

#ifndef LCAOCCURRENCESET_HH
#define LCAOCCURRENCESET_HH 

#include <iterator>
#include <lcaoccurrence.h>

template<class T, class Ref, class Ptr>
class LCAOccurrenceIterator : public <forward_iterator_tag, 
			      T, ptrdiff_t, Ptr, Ref>
			      
{
	typedef  LCAOccurrenceIterator<T,Ref,Ptr> self;

	void incr() { }
	
	bool operator==(const self & x) const {
	}

	bool operator!=(const self& x) const {
	}                          
	
	LCAOccurrenceIterator(T* t);
	LCAOccurrenceIterator();
	LCAOccurrenceIterator(const iterator& t);

	reference operator*() const {  }
	pointer operator->() const { }
	
	self& operator++()
	{
	    incr();
	    return *this;
	}
	self operator++(int)
	{
	    self tmp = *this;
	    incr();
	    return tmp;
	}                                

};

class LCAOccurrenceSet
{
    public: // Types

	// Container types
	typedef LCAOccurrence value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	
	typedef LCAOccurrenceIterator<LCAOccurrence, LCAOccurrence&,
	    LCAOccurrence*> iterator;

	typedef LCAOccurrenceIterator<LCAOccurrence, const LCAOccurrence&,
	    const LCAOccurrence*> const_iterator;


    public: // Methods

	LCAOccurrenceSet(const LCAOccurrenceSet &lcaoccurrenceset);
	LCAOccurrenceSet();
	~LCAOccurrenceSet();
   
	 LCAOccurrenceSet &lcaoccurrenceset
	 operator=(const LCAOccurrenceSet &lcaoccurrenceset);  

	int operator==(const LCAOccurrenceSet &lcaoccurrenceset) const;  
	int operator!=(const LCAOccurrenceSet &lcaoccurrenceset) const;

	// Working with RRULES and EXRULES

	AddRule(LCAOccurrenceSet &rule);
	ExcludeRule(LCARecurrenceRule &rule)

	// RDATES and EXDATES. AddDate will add a new RDATE property,
	// or it will delete an EXDATE property if one exists for that
	// date.

	void AddDate();
	void ExcludeDate();

	// Container routines

	iterator begin() {  }
	const_iterator begin() const {}

	iterator end() { }
	const_iterator end() const {  }

	size_type size() const { }

	size_type max_size() const { }

	bool empty() const { }

	void swap(LCAOccurrenceSet& s){}
                                             

    public: // Attributes

	LCATimeZone& Standard();
	LCATimeZone& Daylight();

	// This is the general alarm to be applied to occurrences that
	// don't have their own alarms.
	LCAAlarm& Alarm();

 
    protected: // Methods

    protected: // Data


};


/*============================================SCDLCAOccurrenceSet

  CLASS:  LCAOccurrenceSet 
  
  DESCRIPTION:
  
  HOW TO USE:

  PUBLIC MEMBER FUNCTION DOCUMENTATION:   

==============================================ECDLCAOccurrenceSet*/

#ifdef LCAOCCURRENCESET_CC
static const char vcid_LCAOCCURRENCESET_HH[] = "$Id: lcaoccurrenceset.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $";
#endif

#endif /* !LCAOCCURRENCESET_HH */



