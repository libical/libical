/* -*- Mode: C -*-
  ======================================================================
  FILE: icalerror.c
  CREATOR: eric 16 May 1999
  
  $Id: icalerror.c,v 1.3 2001-01-24 17:14:01 ebusboom Exp $
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

struct icalerror_state { 
    icalerrorenum error;
    icalerrorstate state; 
};

struct icalerror_state error_state_map[] = 
{ 
    { ICAL_BADARG_ERROR,ICAL_ERROR_DEFAULT},
    { ICAL_NEWFAILED_ERROR,ICAL_ERROR_DEFAULT},
    { ICAL_MALFORMEDDATA_ERROR,ICAL_ERROR_DEFAULT}, 
    { ICAL_PARSE_ERROR,ICAL_ERROR_DEFAULT},
    { ICAL_INTERNAL_ERROR,ICAL_ERROR_DEFAULT}, 
    { ICAL_FILE_ERROR,ICAL_ERROR_DEFAULT},
    { ICAL_ALLOCATION_ERROR,ICAL_ERROR_DEFAULT},
    { ICAL_USAGE_ERROR,ICAL_ERROR_DEFAULT},
    { ICAL_MULTIPLEINCLUSION_ERROR,ICAL_ERROR_DEFAULT},
    { ICAL_TIMEDOUT_ERROR,ICAL_ERROR_DEFAULT},
    { ICAL_UNKNOWN_ERROR,ICAL_ERROR_DEFAULT},
    { ICAL_NO_ERROR,ICAL_ERROR_DEFAULT}

};

void icalerror_set_error_state( icalerrorenum error, 
				icalerrorstate state)
{
    int i;

    for(i = ICAL_BADARG_ERROR; error_state_map[i].error!= ICAL_NO_ERROR;i++){
	if(error_state_map[i].error == error){
	    error_state_map[i].state = state; 	
	}
    }
}

icalerrorstate icalerror_get_error_state( icalerrorenum error)
{
    int i;

    for(i = ICAL_BADARG_ERROR; error_state_map[i].error!= ICAL_NO_ERROR;i++){
	if(error_state_map[i].error == error){
	    return error_state_map[i].state; 	
	}
    }

    return ICAL_ERROR_UNKNOWN;	
}


void icalerror_set_errno(icalerrorenum e) {

    icalerrorstate es;

    icalerrno = e;
    es =  icalerror_get_error_state(e);

    icalerror_stop_here();

    if( (es == ICAL_ERROR_FATAL) ||
	(es == ICAL_ERROR_DEFAULT && icalerror_errors_are_fatal == 1)){
	
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



