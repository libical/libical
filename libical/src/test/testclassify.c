/* -*- Mode: C -*-
  ======================================================================
  FILE: testclassify.c
  CREATOR: eric 11 February 2000
  
  $Id: testclassify.c,v 1.4 2001-04-12 18:33:15 ebusboom Exp $
  $Locker:  $
    
 (C) COPYRIGHT 2000 Eric Busboom
 http://www.softwarestudio.org

 The contents of this file are subject to the Mozilla Public License
 Version 1.0 (the "License"); you may not use this file except in
 compliance with the License. You may obtain a copy of the License at
 http://www.mozilla.org/MPL/
 
 Software distributed under the License is distributed on an "AS IS"
 basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 the License for the specific language governing rights and
 limitations under the License.
 
 The Original Code is eric. The Initial Developer of the Original
 Code is Eric Busboom


 ======================================================================*/

#include <stdio.h> /* for printf */
#include "ical.h"
#include <errno.h>
#include <string.h> /* For strerror */
#include "icalset.h"
#include "icalclassify.h"


int main(int argc, char* argv[])
{
    icalcomponent *c;
    int i=0;

    icalset* f = icalset_new_file("../../test-data/incoming.ics");
    icalset* cal = icalset_new_file("../../test-data/calendar.ics");

    assert(f!= 0);
    assert(cal!=0);
	

    /* Foreach incoming message */
    for(c=icalset_get_first_component(f);c!=0;
	c=icalset_get_next_component(f)){
	
	icalproperty_xlicclass class;
	icalcomponent *match;
	icalcomponent *inner = icalcomponent_get_first_real_component(c);
	icalcomponent *p;
	const char *this_uid;
	const char *i_x_note=0;
	const char *c_x_note=0;

	i++;

        printf("%s\n",icalcomponent_as_ical_string(c));

	if(inner == 0){
            continue;
	}

	p = icalcomponent_get_first_property(inner,ICAL_UID_PROPERTY);

        if(p != 0){
            this_uid = icalproperty_get_uid(p);
            /* Find a booked component that is matched to the incoming
	   message, based on the incoming component's UID, SEQUENCE
	   and RECURRENCE-ID*/
            
            match = icalset_fetch(cal,this_uid);
        } else {
            this_uid = 0;
            match = 0;
        }


	class = icalclassify(c,match,"A@example.com");

	for(p = icalcomponent_get_first_property(c,ICAL_X_PROPERTY);
	    p!= 0;
	    p = icalcomponent_get_next_property(c,ICAL_X_PROPERTY)){
	    if(strcmp(icalproperty_get_x_name(p),"X-LIC-NOTE")==0){
		i_x_note = icalproperty_get_x(p);
	    }
	}


	if(i_x_note == 0){
	    i_x_note = "None";
	}

        if (match != 0){
            for(p = icalcomponent_get_first_property(match,ICAL_X_PROPERTY);
                p!= 0;
                p = icalcomponent_get_next_property(match,ICAL_X_PROPERTY)){
                
                if(strcmp(icalproperty_get_x_name(p),"X-LIC-NOTE")==0){
                    c_x_note = icalproperty_get_x(p);
                }
                
            }
        }

	if(c_x_note == 0){
	    c_x_note = "None";
	}


	printf("Test %d\nIncoming:      %s\nMatched:       %s\nClassification: %s\n\n",i,i_x_note,c_x_note,icalproperty_enum_to_string(class));	
    }

    return 0;
}


