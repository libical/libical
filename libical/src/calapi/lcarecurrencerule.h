// -*- Mode: C++ -*-
/*======================================================================
  FILE: LCARecurrenceRule.h
  CREATOR: eric 23 September 2000

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


  $Id: lcarecurrencerule.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $
  $Locker:  $
 
  ======================================================================*/

#ifndef LCARECURRENCERULE_HH
#define LCARECURRENCERULE_HH 


class LCARecurrenceRule
{
    public: // Typedefs 

	typedef enum {SECONDLY,MINUTELY,HOURLY,DAILY,
		      WEEKLY,MONTHLY,YEARLY} Frequency;

	typedef enum {SECOND,MINUTE,HOUR,DAY,YEARDAY
		      WEEKNO,MONTH,SETPOS} ByTag;

	typedef enum {} Weekday;

	// Other types defs as required...
   
    public: // Methods

	LCARecurrenceRule(const LCARecurrenceRule &lcarecurrencerule);
	LCARecurrenceRule();

	~LCARecurrenceRule();
   
	int operator==(const LCARecurrenceRule &lcarecurrencerule) const;  
	int operator!=(const LCARecurrenceRule &lcarecurrencerule) const;
 
	// ICal style access routines
	
	SetByRule(ByTag bytag, vector<int>);
	vector<int> GetByRule(ByTag bytag);

	Interval GetFrequency();
	void SetFrequency(Frequency freq);

	int GetCount();
	void SetCount(int);

	int SetUntil(const LCAOccurrence &o);
	const LCAOccurrence& GetUntil();

	Weekday GetWeekStart();
	void SetWeekStart(Weekday weekday);

	// Convienience routines

	void SetNthWeekOnDay(int interval, Weekday day);
	void SetNthOfNthMonth(int day_interval, int month_interval,int month);
	//Etc...
	
	// Iterate Through occurrences 
	// Begin, end and all that. 

    protected: // Methods

	// Output as an Ical string
	string AsString();

    protected: // Data


};


/*============================================SCDLCARecurrenceRule

  CLASS:  LCARecurrenceRule 
  
  DESCRIPTION:
  
  HOW TO USE:

  PUBLIC MEMBER FUNCTION DOCUMENTATION:   

==============================================ECDLCARecurrenceRule*/

#ifdef LCARECURRENCERULE_CC
static const char vcid_LCARECURRENCERULE_HH[] = "$Id: lcarecurrencerule.h,v 1.1.1.1 2001-01-02 07:33:09 ebusboom Exp $";
#endif

#endif /* !LCARECURRENCERULE_HH */



