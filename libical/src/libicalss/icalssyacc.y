%{
/* -*- Mode: C -*-
  ======================================================================
  FILE: icalssyacc.y
  CREATOR: eric 08 Aug 2000
  
  DESCRIPTION:
  
  $Id: icalssyacc.y,v 1.1.1.1 2001-01-02 07:33:04 ebusboom Exp $
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

#include <stdlib.h>
#include <string.h> /* for strdup() */
#include <limits.h> /* for SHRT_MAX*/
#include "ical.h"
#include "pvl.h"
#include "icalgaugeimpl.h"


extern struct icalgauge_impl *icalss_yy_gauge;

void ssyacc_add_where(struct icalgauge_impl* impl, char* prop, 
	icalgaugecompare compare , char* value);
void ssyacc_add_select(struct icalgauge_impl* impl, char* str1);
void ssyacc_add_from(struct icalgauge_impl* impl, char* str1);
void move_where(int w);
void sserror(char *s); /* Don't know why I need this.... */



%}

%union {
	char* v_string;
}


%token <v_string> STRING
%token SELECT FROM WHERE COMMA EQUALS NOTEQUALS  LESS GREATER LESSEQUALS
%token GREATEREQUALS AND OR EOL END

%%

query_min: SELECT select_list FROM from_list WHERE where_list
	   | error { 
		 icalparser_clear_flex_input();
                 yyclearin;
           }	
	   ;	

select_list:
	STRING {ssyacc_add_select(icalss_yy_gauge,$1);}
	| select_list COMMA STRING {ssyacc_add_select(icalss_yy_gauge,$3);}
	;


from_list:
	STRING {ssyacc_add_from(icalss_yy_gauge,$1);}
	| from_list COMMA STRING {ssyacc_add_from(icalss_yy_gauge,$3);}
	;

where_clause:
	/* Empty */
	| STRING EQUALS STRING {ssyacc_add_where(icalss_yy_gauge,$1,ICALGAUGECOMPARE_EQUAL,$3); }
	
	| STRING NOTEQUALS STRING {ssyacc_add_where(icalss_yy_gauge,$1,ICALGAUGECOMPARE_NOTEQUAL,$3); }
	| STRING LESS STRING {ssyacc_add_where(icalss_yy_gauge,$1,ICALGAUGECOMPARE_LESS,$3); }
	| STRING GREATER STRING {ssyacc_add_where(icalss_yy_gauge,$1,ICALGAUGECOMPARE_GREATER,$3); }
	| STRING LESSEQUALS STRING {ssyacc_add_where(icalss_yy_gauge,$1,ICALGAUGECOMPARE_LESSEQUAL,$3); }
	| STRING GREATEREQUALS STRING {ssyacc_add_where(icalss_yy_gauge,$1,ICALGAUGECOMPARE_GREATEREQUAL,$3); }
	;

where_list:
	where_clause {move_where(1);}
	| where_list AND where_clause {move_where(2);} 
	| where_list OR where_clause {move_where(3);}
	;


%%

void ssyacc_add_where(struct icalgauge_impl* impl, char* prop, 
	icalgaugecompare compare , char* value)
{

    struct icalgauge_where *where;
    
    if ( (where = malloc(sizeof(struct icalgauge_where))) ==0){
	icalerror_set_errno(ICAL_NEWFAILED_ERROR);
	return;
    }
    
    where->prop = icalenum_string_to_property_kind(prop);

    /* HACK This does not handle the case where 'prop' has the form
       COMPONENT.PROPERTY*/
    if(where->prop == ICAL_NO_PROPERTY){
	icalgauge_free(where);
	icalerror_set_errno(ICAL_BADARG_ERROR);
	return;
    }

    where->compare = compare;

    where->value = strdup(value);

    if(where->value == 0){
	icalerror_set_errno(ICAL_NEWFAILED_ERROR);
	return;
    }

    pvl_push(impl->where,where);

}


void ssyacc_add_select(struct icalgauge_impl* impl, char* str1)
{
    icalproperty *p;
    icalproperty_kind pkind;
    icalcomponent_kind ckind = ICAL_NO_COMPONENT;
    char *c, *compstr, *propstr;
    struct icalgauge_where *where;
    
    /* Uses only the prop and comp fields of the where structure */
    if ( (where = malloc(sizeof(struct icalgauge_where))) ==0){
	icalerror_set_errno(ICAL_NEWFAILED_ERROR);
	return;
    }

    memset(where,0,sizeof(struct icalgauge_where));

    /* Is there a period in str1 ? If so, the string specified both a
       component and a property*/
    if( (c = strrchr(str1,'.')) != 0){
	compstr = str1;
	propstr = c+1;
	*c = '\0';
    } else {
	compstr = 0;
	propstr = str1;
    }


    /* Handle the case where a component was specified */
    if(compstr != 0){
	where->comp = icalenum_string_to_component_kind(compstr);
    } else {
	where->comp = ICAL_NO_COMPONENT;
    }


    /* If the property was '*', then accept all properties */
    if(strcmp("*",propstr) == 0) {
	where->prop = ICAL_ANY_PROPERTY; 	    
    } else {
	where->prop = icalenum_string_to_property_kind(str1);    
    }
    

    if(where->prop == ICAL_NO_PROPERTY){
	icalgauge_free(where);
	icalerror_set_errno(ICAL_BADARG_ERROR);
	return;
    }

    pvl_push(impl->select,where);
}

void ssyacc_add_from(struct icalgauge_impl* impl, char* str1)
{
    icalcomponent *c;
    icalcomponent_kind ckind;

    ckind = icalenum_string_to_component_kind(str1);

    if(ckind == ICAL_NO_COMPONENT){
	assert(0);
    }

    pvl_push(impl->from,(void*)ckind);

}

void move_where(int w)
{
}

void sserror(char *s){
    fprintf(stderr,"Parse error \'%s\'\n", s);
}
