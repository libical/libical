/* -*- Mode: C -*- */
/*======================================================================
 FILE: icalgauge.c
 CREATOR: eric 23 December 1999


 $Id: icalgauge.c,v 1.1.1.1 2001-01-02 07:33:03 ebusboom Exp $
 $Locker:  $

 (C) COPYRIGHT 2000, Eric Busboom, http://www.softwarestudio.org

 This program is free software; you can redistribute it and/or modify
 it under the terms of either: 

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.fsf.org/copyleft/lesser.html

  Or:

    The Mozilla Public License Version 1.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/

 The Original Code is eric. The Initial Developer of the Original
 Code is Eric Busboom


======================================================================*/

#include "ical.h"
#include "icalgauge.h"
#include "icalgaugeimpl.h"
#include <stdlib.h>

extern char* input_buffer;
extern char* input_buffer_p;
int ssparse(void);

struct icalgauge_impl *icalss_yy_gauge;

icalgauge* icalgauge_new_from_sql(char* sql)
{
    struct icalgauge_impl *impl;

    int r;
    
    if ( ( impl = (struct icalgauge_impl*)
	   malloc(sizeof(struct icalgauge_impl))) == 0) {
	icalerror_set_errno(ICAL_NEWFAILED_ERROR);
	return 0;
    }

    impl->select = pvl_newlist();
    impl->from = pvl_newlist();
    impl->where = pvl_newlist();

    icalss_yy_gauge = impl;

    input_buffer_p = input_buffer = sql;
    r = ssparse();

    return impl;
}


void icalgauge_free(icalgauge* gauge)
{
      struct icalgauge_impl *impl =  (struct icalgauge_impl*)gauge;

      icalcomponent_free(impl->select);
      icalcomponent_free(impl->from);
      icalcomponent_free(impl->where);

}

/* Convert a VQUERY component into a gauge */
icalcomponent* icalgauge_make_gauge(icalcomponent* query);

/* icaldirset_test compares a component against a gauge, and returns
   true if the component passes the test 

   The gauge is a VCALENDAR component that specifies how to test the
   target components. The guage holds a collection of VEVENT, VTODO or
   VJOURNAL sub-components. Each of the sub-components has a
   collection of properties that are compared to corresponding
   properties in the target component, according to the
   X-LIC-COMPARETYPE parameters to the gauge's properties.

   When a gauge has several sub-components, the results of testing the
   target against each of them is ORed together - the target
   component will pass if it matches any of the sub-components in the
   gauge. However, the results of matching the properties in a
   sub-component are ANDed -- the target must match every property in
   a gauge sub-component to match the sub-component.

   Here is an example:

   BEGIN:XROOT
   DTSTART;X-LIC-COMPARETYPE=LESS:19981025T020000
   ORGANIZER;X-LIC-COMPARETYPE=EQUAL:mrbig@host.com 
   END:XROOT
   BEGIN:XROOT
   LOCATION;X-LIC-COMPARETYPE=EQUAL:McNary's Pub
   END:XROOT

   This gauge has two sub-components; one which will match a VEVENT
   based on start time, and organizer, and another that matches based
   on LOCATION. A target component will pass the test if it matched
   either of the sub-components.
   
  */


int icalgauge_compare_recurse(icalcomponent* comp, icalcomponent* gauge)
{
    int pass = 1,localpass = 0;
    icalproperty *p;
    icalcomponent *child,*subgauge; 
    icalcomponent_kind gaugekind, compkind;

    icalerror_check_arg_rz( (comp!=0), "comp");
    icalerror_check_arg_rz( (gauge!=0), "gauge");

    gaugekind = icalcomponent_isa(gauge);
    compkind = icalcomponent_isa(comp);

    if( ! (gaugekind == compkind || gaugekind == ICAL_ANY_COMPONENT) ){
	return 0;
    }   

    /* Test properties. For each property in the gauge, search through
       the component for a similar property. If one is found, compare
       the two properties value with the comparison specified in the
       gauge with the X-LIC-COMPARETYPE parameter */
    
    for(p = icalcomponent_get_first_property(gauge,ICAL_ANY_PROPERTY);
	p != 0;
	p = icalcomponent_get_next_property(gauge,ICAL_ANY_PROPERTY)){
	
	icalproperty* targetprop; 
	icalparameter* compareparam;
	icalparameter_xliccomparetype compare;
	int rel; /* The relationship between the gauge and target values.*/
	
	/* Extract the comparison type from the gauge. If there is no
	   comparison type, assume that it is "EQUAL" */
	
	compareparam = icalproperty_get_first_parameter(
	    p,
	    ICAL_XLICCOMPARETYPE_PARAMETER);
	
	if (compareparam!=0){
	    compare = icalparameter_get_xliccomparetype(compareparam);
	} else {
	    compare = ICAL_XLICCOMPARETYPE_EQUAL;
	}
	
	/* Find a property in the component that has the same type
	   as the gauge property. HACK -- multiples of a single
	   property type in the gauge will match only the first
	   instance in the component */
	
	targetprop = icalcomponent_get_first_property(comp,
						      icalproperty_isa(p));
	
	if(targetprop != 0){
	
	    /* Compare the values of the gauge property and the target
	       property */
	    
	    rel = icalvalue_compare(icalproperty_get_value(p),
				    icalproperty_get_value(targetprop));
	    
	    /* Now see if the comparison is equavalent to the comparison
	       specified in the gauge */
	    
	    if (rel == compare){ 
		localpass++; 
	    } else if (compare == ICAL_XLICCOMPARETYPE_LESSEQUAL && 
		       ( rel == ICAL_XLICCOMPARETYPE_LESS ||
			 rel == ICAL_XLICCOMPARETYPE_EQUAL)) {
		localpass++;
	    } else if (compare == ICAL_XLICCOMPARETYPE_GREATEREQUAL && 
		       ( rel == ICAL_XLICCOMPARETYPE_GREATER ||
			 rel == ICAL_XLICCOMPARETYPE_EQUAL)) {
		localpass++;
	    } else if (compare == ICAL_XLICCOMPARETYPE_NOTEQUAL && 
		       ( rel == ICAL_XLICCOMPARETYPE_GREATER ||
			 rel == ICAL_XLICCOMPARETYPE_LESS)) {
		localpass++;
	    } else {
		localpass = 0;
	    }
	    
	    pass = pass && (localpass>0);
	}
    }
    
    /* Test subcomponents. Look for a child component that has a
       counterpart in the gauge. If one is found, recursively call
       icaldirset_test */
    
    for(subgauge = icalcomponent_get_first_component(gauge,ICAL_ANY_COMPONENT);
	subgauge != 0;
	subgauge = icalcomponent_get_next_component(gauge,ICAL_ANY_COMPONENT)){
	
	gaugekind = icalcomponent_isa(subgauge);

	if (gaugekind == ICAL_ANY_COMPONENT){
	    child = icalcomponent_get_first_component(comp,ICAL_ANY_COMPONENT);
	} else {
	    child = icalcomponent_get_first_component(comp,gaugekind);
	}
	
	if(child !=0){
	    localpass = icalgauge_compare_recurse(child,subgauge);
	    pass = pass && localpass;
	} else {
	    pass = 0;
	}
    }
    
    return pass;   
}

/* guagecontainer is an XROOT component that holds several gauges. The
   results of comparing against these gauges are ORed together in this
   routine */
int icalgauge_compare(icalcomponent* comp, 
		   icalcomponent* gaugecontainer)
{
    int pass = 0;
    icalcomponent *gauge; 

    icalerror_check_arg_rz( (comp!=0), "comp");
    icalerror_check_arg_rz( (gauge!=0), "gauge");
    
    for(gauge = icalcomponent_get_first_component(gaugecontainer,
						  ICAL_ANY_COMPONENT);
	gauge != 0;
	gauge = icalcomponent_get_next_component(gaugecontainer,
						 ICAL_ANY_COMPONENT)){

	pass += icalgauge_compare_recurse(comp, gauge);
    }
    return pass>0;
}
    

void icalguage_dump(icalcomponent* gauge)
{

    pvl_elem *p;
    struct icalgauge_impl *impl = (struct icalgauge_impl*)gauge;
  
  
    printf("--- Select ---\n");
    for(p = pvl_head(impl->select);p!=0;p=pvl_next(p)){
	struct icalgauge_where *w = pvl_data(p);

	if(w->comp != 0){
	    printf("%s ",icalenum_component_kind_to_string(w->comp));
	}

	if(w->prop != 0){
	    printf("%s ",icalenum_property_kind_to_string(w->prop));
	}
	
	if (w->compare != ICALGAUGECOMPARE_NONE){
	    printf("%d ",w->compare);
	}


	if (w->value!=0){
	    printf("%s",w->value);
	}


	printf("\n");
    }
    
}

