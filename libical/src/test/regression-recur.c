/* -*- Mode: C -*-
  ======================================================================
  FILE: regression-recur.c
  CREATOR: ebusboom 8jun00
  
  DESCRIPTION:

  Test program for expanding recurrences. Run as:

     ./recur ../../test-data/recur.txt

  
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

  ======================================================================*/

#include "ical.h"
#include <assert.h>
#include <string.h> /* for strdup */
#include <stdlib.h> /* for malloc */
#include <stdio.h> /* for printf */
#include <time.h> /* for time() */
#include <signal.h> /* for signal */
#ifndef WIN32
#include <unistd.h> /* for alarm */
#endif

#include "icalss.h"
#include "regression.h"
extern int VERBOSE;

#ifdef WIN32
#define snprintf	_snprintf
#define strcasecmp	stricmp
#endif


static void sig_alrm(int i){
    fprintf(stderr,"Could not get lock on file\n");
    exit(1);
}

static void recur_callback(icalcomponent *comp,
			   struct icaltime_span *span,
			   void *data)
{
  if (VERBOSE) {
    printf("cb: %s", ctime(&span->start));
    printf("    %s\n", ctime(&span->end));
  }

}

void test_recur_file()
{
    icalfileset *cin = 0;
    struct icaltimetype start, next;
    icalcomponent *itr;
    icalproperty *desc, *dtstart, *rrule;
    struct icalrecurrencetype recur;
    icalrecur_iterator* ritr;
    time_t tt;
    char* file; 
	
    icalerror_set_error_state(ICAL_PARSE_ERROR, ICAL_ERROR_NONFATAL);
	
#ifndef WIN32
    signal(SIGALRM,sig_alrm);
#endif
    file = getenv("ICAL_RECUR_FILE");
    if (!file)
      file = "../../test-data/recur.txt";
	
#ifndef WIN32
    alarm(15); /* to get file lock */
#endif
    cin = icalfileset_new(file);
#ifndef WIN32
    alarm(0);
#endif
	
    ok("opening file with recurring events", (cin!=NULL));
    assert(cin!=NULL);
	
    for (itr = icalfileset_get_first_component(cin);
	itr != 0;
	itr = icalfileset_get_next_component(cin)){
      int badcomp = 0;

      struct icaltimetype start = icaltime_from_timet(1,0);
      struct icaltimetype end = icaltime_today();
      
      
      desc = icalcomponent_get_first_property(itr,ICAL_DESCRIPTION_PROPERTY);
      dtstart = icalcomponent_get_first_property(itr,ICAL_DTSTART_PROPERTY);
      rrule = icalcomponent_get_first_property(itr,ICAL_RRULE_PROPERTY);
      
      ok("check for malformed component", !(desc == 0 || dtstart == 0 || rrule == 0));

      if (desc == 0 || dtstart == 0 || rrule == 0) {
	badcomp = 1;
	if (VERBOSE) {
	  printf("\n******** Error in input component ********\n");
	  printf("The following component is malformed:\n %s\n",
		 icalcomponent_as_ical_string(itr));
	}
	continue;
      }
      
      if (VERBOSE) {
	printf("\n\n#### %s\n",icalproperty_get_description(desc));
	printf("#### %s\n",icalvalue_as_ical_string(icalproperty_get_value(rrule)));
      }
      recur = icalproperty_get_rrule(rrule);
      start = icalproperty_get_dtstart(dtstart);
      
      ritr = icalrecur_iterator_new(recur,start);
      
      tt = icaltime_as_timet(start);
      
      if (VERBOSE)
	printf("#### %s\n",ctime(&tt ));
      
      icalrecur_iterator_free(ritr);
      
      for(ritr = icalrecur_iterator_new(recur,start),
	    next = icalrecur_iterator_next(ritr); 
	  !icaltime_is_null_time(next);
	  next = icalrecur_iterator_next(ritr)){
	
	tt = icaltime_as_timet(next);
	
	if (VERBOSE)
	  printf("  %s",ctime(&tt ));		
	
      }
      icalrecur_iterator_free(ritr);
      
      icalcomponent_foreach_recurrence(itr, start, end, 
				       recur_callback, NULL);
      
      
    }
    
    icalfileset_free(cin);
    
}
