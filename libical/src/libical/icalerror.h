/* -*- Mode: C -*- */
/*======================================================================
  FILE: icalerror.h
  CREATOR: eric 09 May 1999
  
  $Id: icalerror.h,v 1.2 2001-01-12 21:22:20 ebusboom Exp $


 (C) COPYRIGHT 2000, Eric Busboom, http://www.softwarestudio.org

 This program is free software; you can redistribute it and/or modify
 it under the terms of either: 

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.fsf.org/copyleft/lesser.html

  Or:

    The Mozilla Public License Version 1.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/

  The original code is icalerror.h

======================================================================*/


#ifndef ICALERROR_H
#define ICALERROR_H

#include <assert.h>
#include <stdio.h> /* For icalerror_warn() */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


/* This routine is called before any error is triggered. It is called
   by icalerror_set_errno, so it does not appear in all of the macros
   below */
void icalerror_stop_here(void);

void icalerror_crash_here(void);

#ifdef ICAL_ERRORS_ARE_FATAL
#undef NDEBUG
#endif

#define icalerror_check_value_type(value,type);
#define icalerror_check_property_type(value,type);
#define icalerror_check_parameter_type(value,type);
#define icalerror_check_component_type(value,type);

/* Assert with a message */
#ifdef ICAL_ERRORS_ARE_FATAL

#ifdef __GNUC__
#define icalerror_assert(test,message) if(!(test)){fprintf(stderr,"%s(), %s:%d: %s\n",__FUNCTION__,__FILE__,__LINE__,message);icalerror_stop_here(); abort();}
#else /*__GNUC__*/
#define icalerror_assert(test,message) if(!(test)){fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,message);icalerror_stop_here(); abort();}
#endif /*__GNUC__*/
#else
#define icalerror_assert(test,message) 
#endif 

/* Check & abort if check fails */
#ifdef ICAL_ERRORS_ARE_FATAL
#define icalerror_check_arg(test,arg) icalerror_stop_here();assert(test) 
#else
#define icalerror_check_arg(test,arg)
#endif
/* Check & return void if check fails*/

#ifdef ICAL_ERRORS_ARE_FATAL
#define icalerror_check_arg_rv(test,arg) icalerror_stop_here();assert(test);
#else 
#define icalerror_check_arg_rv(test,arg) if(!(test)) { icalerror_set_errno(ICAL_BADARG_ERROR); return; }
#endif

/* Check & return 0 if check fails*/
#ifdef ICAL_ERRORS_ARE_FATAL
#define icalerror_check_arg_rz(test,arg) icalerror_stop_here();assert(test); 
#else
#define icalerror_check_arg_rz(test,arg) if(!(test)) {icalerror_set_errno(ICAL_BADARG_ERROR); return 0;}
#endif


/* Check & return an error if check fails*/
#ifdef ICAL_ERRORS_ARE_FATAL
#define icalerror_check_arg_re(test,arg,error) icalerror_stop_here();assert(test); 
#else
#define icalerror_check_arg_re(test,arg,error) if(!(test)) {icalerror_stop_here(); return error;}
#endif


/* Warning messages */

#ifdef ICAL_ERRORS_ARE_FATAL 

#ifdef __GNUC__
#define icalerror_warn(message) {fprintf(stderr,"%s(), %s:%d: %s\n",__FUNCTION__,__FILE__,__LINE__,message); abort();}
#else /* __GNU_C__ */
#define icalerror_warn(message) {fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,message); abort();}
#endif /* __GNU_C__ */

#else /*ICAL_ERRORS_ARE_FATAL */

#ifdef __GNUC__
#define icalerror_warn(message) {fprintf(stderr,"%s(), %s:%d: %s\n",__FUNCTION__,__FILE__,__LINE__,message);}
#else /* __GNU_C__ */
#define icalerror_warn(message) {fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,message);}
#endif /* __GNU_C__ */

#endif /*ICAL_ERRORS_ARE_FATAL*/

typedef enum icalerrorenum {
    
    ICAL_BADARG_ERROR,
    ICAL_NEWFAILED_ERROR,
    ICAL_MALFORMEDDATA_ERROR, 
    ICAL_PARSE_ERROR,
    ICAL_INTERNAL_ERROR, /* Like assert --internal consist. prob */
    ICAL_FILE_ERROR,
    ICAL_ALLOCATION_ERROR,
    ICAL_USAGE_ERROR,
    ICAL_NO_ERROR,
    ICAL_MULTIPLEINCLUSION_ERROR,
    ICAL_TIMEDOUT_ERROR,
    ICAL_UNKNOWN_ERROR /* Used for problems in input to icalerror_strerror()*/

} icalerrorenum;

extern icalerrorenum icalerrno;
extern int icalerror_errors_are_fatal;

void icalerror_clear_errno(void);
void icalerror_set_errno(icalerrorenum);

char* icalerror_strerror(icalerrorenum e);


#endif /* !ICALERROR_H */



