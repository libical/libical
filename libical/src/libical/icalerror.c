/* -*- Mode: C -*-
  ======================================================================
  FILE: icalerror.c
  CREATOR: eric 16 May 1999
  
  $Id: icalerror.c,v 1.2 2001-01-12 21:22:20 ebusboom Exp $
  $Locker:  $
    

 (C) COPYRIGHT 2000, Eric Busboom, http://www.softwarestudio.org

 This program is free software; you can redistribute it and/or modify
 it under the terms of either: 

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.fsf.org/copyleft/lesser.html

  Or:

    The Mozilla Public License Version 1.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/

  The original code is icalerror.c

 ======================================================================*/


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "icalerror.h"

icalerrorenum icalerrno;

int foo;
void icalerror_stop_here(void)
{
    foo++; /* Keep optimizers from removing routine */
}

void icalerror_crash_here(void)
{
    int *p=0;
    *p = 1;

    assert( *p);
}


void icalerror_clear_errno() {
    
    icalerrno = ICAL_NO_ERROR;
}

#ifdef ICAL_ERRORS_ARE_FATAL
int icalerror_errors_are_fatal = 1;
#else
int icalerror_errors_are_fatal = 0;
#endif

void icalerror_set_errno(icalerrorenum e) {

   
    icalerrno = e;
    icalerror_stop_here();

    if(icalerror_errors_are_fatal == 1){

	fprintf(stderr,"libical: icalerrno_set_error: %s\n",icalerror_strerror(e));
#ifdef NDEBUG
	icalerror_crash_here();
#else
	assert(0);
#endif 
    }


}


struct icalerror_string_map {
	icalerrorenum error;
	char name[160];
};

static struct icalerror_string_map string_map[] = 
{
    {ICAL_BADARG_ERROR,"Bad argument to function"},
    {ICAL_NEWFAILED_ERROR,"Failed to create a new object via a *_new() routine"},
    {ICAL_MALFORMEDDATA_ERROR,"An input string was not correctly formed or a component has missing or extra properties"},
    {ICAL_PARSE_ERROR,"Failed to parse a part of an iCal component"},
    {ICAL_INTERNAL_ERROR,"Random internal error. This indicates an error in the library code, not an error in use"}, 
    {ICAL_FILE_ERROR,"An operation on a file failed. Check errno for more detail."},
    {ICAL_ALLOCATION_ERROR,"Failed to allocate memory"},
    {ICAL_USAGE_ERROR,"The caller failed to properly sequence called to an object's interface"},
    {ICAL_NO_ERROR,"No error"},
    {ICAL_UNKNOWN_ERROR,"Unknown error type -- icalerror_strerror() was probably given bad input"}
};


char* icalerror_strerror(icalerrorenum e) {

    int i;

    for (i=0; string_map[i].error != ICAL_UNKNOWN_ERROR; i++) {
	if (string_map[i].error == e) {
	    return string_map[i].name;
	}
    }

    return string_map[i].name; /* Return string for ICAL_UNKNOWN_ERROR*/
    
}



