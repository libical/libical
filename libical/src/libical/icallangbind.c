/* -*- Mode: C -*-
  ======================================================================
  FILE: icallangbind.c
  CREATOR: eric 15 dec 2000
  
  DESCRIPTION:
  
  $Id: icallangbind.c,v 1.2 2001-01-28 18:00:48 ebusboom Exp $
  $Locker:  $

  (C) COPYRIGHT 1999 Eric Busboom 
  http://www.softwarestudio.org
  
  This package is free software and is provided "as is" without
  express or implied warranty.  It may be used, redistributed and/or
  modified under the same terms as perl itself. ( Either the Artistic
  License or the GPL. )

  ======================================================================*/

#include "ical.h"



int* icallangbind_new_array(int size){
    int* p = malloc(size*sizeof(int));
    return p; /* Caller handles failures */
}

void icallangbind_free_array(int* array){
    free(array);
}

int icallangbind_access_array(int* array, int index) {
    return array[index];
}                    

/* Return the nth occurrence of 'prop' in c */
icalproperty* icallangbind_get_property(icalcomponent *c, int n, const char* prop)
{
    int count; 
    icalproperty_kind kind;
    icalproperty *p;
    icalcomponent * comps[3];
    int compno = 0;
    int propno = 0;

    if(c == 0 || prop == 0 || n < 0){
	return 0;
    }

    kind = icalenum_string_to_property_kind(prop);

    if (kind == ICAL_NO_PROPERTY){
	return 0;
    }

    comps[0] = c;
    comps[1] = icalcomponent_get_first_real_component(c);
    comps[2] = 0;

    if(kind == ICAL_X_PROPERTY){

	for(compno ==0; comps[compno]!=0 ; compno++){

	    for(p = icalcomponent_get_first_property(comps[compno],kind);
		p !=0;
		p = icalcomponent_get_next_property(comps[compno],kind)
		){

		if(strcmp(icalproperty_get_x_name(p),prop) == 0){

		    if(propno == n ){
			return p;
		    }
		    
		    propno++;
		}		
	    }
	}

    } else {
	for(compno ==0; comps[compno]!=0 ; compno++){

	    for(propno=0,
		    p = icalcomponent_get_first_property(comps[compno],kind);
		propno != n && p !=0;
		propno++,
		    p = icalcomponent_get_next_property(comps[compno],kind)
		)
	    {
	    }

	    if(p != 0){
		return p;
	    }

	}
    }
	
    return 0;

}

const char* icallangbind_get_property_val(icalproperty* p)
{
    icalvalue *v;
    if (p == 0){
	return 0;
    }

    v = icalproperty_get_value(p);

    if(v == 0){
	return v;
    }

    return icalvalue_as_ical_string(v);
    
}

const char* icallangbind_get_parameter(icalproperty *p, const char* parameter)
{
    icalparameter_kind kind;
    icalparameter *param;

    if(p == 0 || parameter == 0){
	return 0;
    }
    
    kind = icalenum_string_to_parameter_kind(parameter);

    if(kind == ICAL_NO_PARAMETER){
	return 0;
    }

    if(kind == ICAL_X_PARAMETER){
	for(param = icalproperty_get_first_parameter(p,ICAL_X_PARAMETER);
	    param != 0;
	    param = icalproperty_get_next_parameter(p,ICAL_X_PARAMETER)){

	    if(strcmp(icalparameter_get_xname(param),parameter) ==0){
		return icalparameter_as_ical_string(param);
	    }
	}

    } else {

	param = icalproperty_get_first_parameter(p,kind);
	
	if (param !=0){
	    return icalparameter_as_ical_string(param);
	}
	
    }

    return 0;
}

icalcomponent* icallangbind_get_component(icalcomponent *c, const char* comp)
{
    if(c == 0 || comp == 0){
	return 0;
    }


}



