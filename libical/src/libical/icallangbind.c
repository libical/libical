/* -*- Mode: C -*-
  ======================================================================
  FILE: icallangbind.c
  CREATOR: eric 15 dec 2000
  
  DESCRIPTION:
  
  $Id: icallangbind.c,v 1.3 2001-02-06 19:43:22 ebusboom Exp $
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
    int* p = (int*)malloc(size*sizeof(int));
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

#define APPENDS(x) icalmemory_append_string(&buf, &buf_ptr, &buf_size, x);

#define APPENDC(x) icalmemory_append_char(&buf, &buf_ptr, &buf_size, x);

char* icallangbind_append_time(char * key, struct icaltimetype ictt) 
{
    size_t buf_size = 1024;
    char* buf = icalmemory_new_buffer(buf_size);
    char* buf_ptr = buf;

    time_t tt = icaltime_as_timet(ictt);
    char tmp[25];

    APPENDS(", ");
    APPENDS(key);
    APPENDS(" => ");
    
    snprintf(tmp,25,"%d",tt);
    
    APPENDS(tmp);
    
    APPENDS(", is_utc => ");
    
    if(ictt.is_utc == 1){
	APPENDC('1');
    } else {
	APPENDC('0');
    }
    
    APPENDS(", is_date =>");
    
    if(ictt.is_date == 1){
	APPENDC('1');
    } else {
	APPENDC('0');
    }

    return buf;
}

const char* icallangbind_perl_eval_string(icalproperty* prop)
{
    size_t buf_size = 1024;
    char* buf = icalmemory_new_buffer(buf_size);
    char* buf_ptr = buf;
    icalparameter *param;
    
    icalvalue* value;

    if( prop == 0){
	return 0;
    }

    APPENDS("{ ");

    value = icalproperty_get_value(prop);


    APPENDS(" name => '");
    APPENDS(icalenum_property_kind_to_string(icalproperty_isa(prop)));
    APPENDC('\'');

    switch (icalvalue_isa(value)){
	
    case ICAL_ATTACH_VALUE:
    case ICAL_BINARY_VALUE: {
	/* Not implemented */
	icalerror_set_errno(ICAL_INTERNAL_ERROR);
	break;
    }

	
    case ICAL_FLOAT_VALUE:
    case ICAL_UTCOFFSET_VALUE:
    case ICAL_BOOLEAN_VALUE:
    case ICAL_INTEGER_VALUE:
    case ICAL_TEXT_VALUE:
    case ICAL_STATUS_VALUE:
    case ICAL_METHOD_VALUE:
    case ICAL_RECUR_VALUE:
    case ICAL_GEO_VALUE:
    case ICAL_STRING_VALUE:
    case ICAL_URI_VALUE:
    case ICAL_CALADDRESS_VALUE: {
	APPENDS(", data => ");
	APPENDS(icalvalue_as_ical_string(value));
	break;

    }
	
    case ICAL_TIME_VALUE:
    case ICAL_DATE_VALUE:
    case ICAL_DATETIME_VALUE:
    case ICAL_DATETIMEDATE_VALUE: {
	struct icaltimetype ictt = icalvalue_get_datetime(value);
	APPENDS(icallangbind_append_time("start_seconds:", ictt))

	break;

    }
    
    case ICAL_DURATION_VALUE: {
	struct icaldurationtype dur = icalvalue_get_duration(value);
	int s = icaldurationtype_as_int(dur);
	char tmp[25];

	APPENDS(", duration =>");
	
	snprintf(tmp,25,"%d",s);

	APPENDS(tmp);

	break;

    }
    
    case ICAL_PERIOD_VALUE: {
	struct icalperiodtype p;

	p = icalvalue_get_period(value);

	APPENDS(icallangbind_append_time("start_seconds", p.start));
				
	if(!icaltime_is_null_time(p.end)){
	    
	    APPENDS(icallangbind_append_time("end_seconds", p.start));

	} else {
	    char tmp[25];
	    struct icaldurationtype dur = icalvalue_get_duration(value);
	    int s = icaldurationtype_as_int(dur);

	    APPENDS(", duration =>");
	    
	    snprintf(tmp,25,"%d",s);
	    
	    APPENDS(tmp);
	}

	break;
    }
    
    case ICAL_NO_VALUE:
    default:
	{
	    icalerror_set_errno(ICAL_INTERNAL_ERROR);
	    
	}
    }


    /* Add Parameters */

    for(param = icalproperty_get_first_parameter(prop,ICAL_ANY_PARAMETER);
        param != 0;
        param = icalproperty_get_next_parameter(prop,ICAL_ANY_PARAMETER)){
        
        const char* str = icalparameter_as_ical_string(param);
        char *copy = icalmemory_tmp_copy(str);
        char *v;

        if(copy == 0){
            icalerror_set_errno(ICAL_NEWFAILED_ERROR);
            continue;
        }

        v = strchr(copy,'=');


        if(v == 0){
            continue;
        }

        *v = 0;

        v++;

        APPENDS(", ");
        APPENDS(copy);
        APPENDS(" => ");
        APPENDS(v);        
        
    }


    APPENDC('}');

    return buf;

}
