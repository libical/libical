/* -*- Mode: C -*-*/
/*======================================================================
  FILE: ical.i

  (C) COPYRIGHT 1999 Eric Busboom
  http://www.softwarestudio.org

  The contents of this file are subject to the Mozilla Public License
  Version 1.0 (the "License"); you may not use this file except in
  compliance with the License. You may obtain a copy of the License at
  http://www.mozilla.org/MPL/

  Software distributed under the License is distributed on an "AS IS"
  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
  the License for the specific language governing rights and
  limitations under the License.

  The original author is Eric Busboom

  Contributions from:
  Graham Davison (g.m.davison@computer.org)

  ======================================================================*/  

%module Net__ICal__Libical

%{
#include "ical.h"

#include <sys/types.h> /* for size_t */
#include <time.h>

%}

typedef void icalcomponent;
typedef void icalproperty;

icalcomponent* icalparser_parse_string(char* str);

char* icalcomponent_as_ical_string(icalcomponent* component);
void icalcomponent_free(icalcomponent* component);
int icalcomponent_count_errors(icalcomponent* component);
void icalcomponent_strip_errors(icalcomponent* component);
void icalcomponent_convert_errors(icalcomponent* component);

int icalrestriction_check(icalcomponent* comp);


int* icallangbind_new_array(int size);
void icallangbind_free_array(int* array);
int icallangbind_access_array(int* array, int index);
int icalrecur_expand_recurrence(char* rule, int start, 
				int count, int* array);

icalproperty* icallangbind_get_property(icalcomponent *c, int n, const char* prop);
const char* icallangbind_get_property_val(icalproperty* p);
const char* icallangbind_get_parameter(icalproperty *p, const char* parameter);
icalcomponent* icallangbind_get_component(icalcomponent *c, const char* comp);


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

/* Make an individual error fatal or non-fatal. */
typedef enum icalerrorstate { 
    ICAL_ERROR_FATAL,     /* Not fata */
    ICAL_ERROR_NONFATAL,  /* Fatal */
    ICAL_ERROR_DEFAULT,   /* Use the value of icalerror_errors_are_fatal*/
    ICAL_ERROR_UNKNOWN    /* Asked state for an unknown error type */
} icalerrorstate ;

void icalerror_set_error_state( icalerrorenum error, icalerrorstate);
icalerrorstate icalerror_get_error_state( icalerrorenum error);



