/* -*- Mode: C -*-
  ======================================================================
  FILE: icalderivedparameters.{c,h}
  CREATOR: eric 09 May 1999
  
  $Id: icalparameter.c,v 1.1 2001-03-17 16:47:02 ebusboom Exp $
  $Locker:  $
    

 (C) COPYRIGHT 2000, Eric Busboom, http://www.softwarestudio.org

 This program is free software; you can redistribute it and/or modify
 it under the terms of either: 

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.fsf.org/copyleft/lesser.html

  Or:

    The Mozilla Public License Version 1.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/

  The original code is icalderivedparameters.{c,h}

  Contributions from:
     Graham Davison (g.m.davison@computer.org)

 ======================================================================*/
/*#line 29 "icalparameter.c.in"*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "icalparameter.h"
#include "icalproperty.h"
#include "icalerror.h"
#include "icalmemory.h"
#include "icalparameterimpl.h"

#include <stdlib.h> /* for malloc() */
#include <errno.h>
#include <string.h> /* for memset() */


struct icalparameter_impl* icalparameter_new_impl(icalparameter_kind kind)
{
    struct icalparameter_impl* v;

    if ( ( v = (struct icalparameter_impl*)
	   malloc(sizeof(struct icalparameter_impl))) == 0) {
	icalerror_set_errno(ICAL_NEWFAILED_ERROR);
	return 0;
    }
    
    strcpy(v->id,"para");

    v->kind = kind;
    v->size = 0;
    v->string = 0;
    v->x_name = 0;
    v->parent = 0;
    memset(&(v->data),0,sizeof(v->data));

    return v;
}

icalparameter*
icalparameter_new (icalparameter_kind kind)
{
    struct icalparameter_impl* v = icalparameter_new_impl(kind);

    return (icalparameter*) v;

}

void
icalparameter_free (icalparameter* parameter)
{
    struct icalparameter_impl * impl;

    impl = (struct icalparameter_impl*)parameter;

/*  HACK. This always triggers, even when parameter is non-zero
    icalerror_check_arg_rv((parameter==0),"parameter");*/


#ifdef ICAL_FREE_ON_LIST_IS_ERROR
    icalerror_assert( (impl->parent ==0),"Tried to free a parameter that is still attached to a component. ");
    
#else
    if(impl->parent !=0){
	return;
    }
#endif

    
    if (impl->string != 0){
	free ((void*)impl->string);
    }
    
    if (impl->x_name != 0){
	free ((void*)impl->x_name);
    }
    
    memset(impl,0,sizeof(impl));

    impl->parent = 0;
    impl->id[0] = 'X';
    free(impl);
}



icalparameter* 
icalparameter_new_clone(icalparameter* param)
{
    struct icalparameter_impl *old;
    struct icalparameter_impl *new;

    old = (struct icalparameter_impl *)param;
    new = icalparameter_new_impl(old->kind);

    icalerror_check_arg_rz((param!=0),"param");

    if (new == 0){
	return 0;
    }

    memcpy(new,old,sizeof(struct icalparameter_impl));

    if (old->string != 0){
	new->string = icalmemory_strdup(old->string);
	if (new->string == 0){
	    icalparameter_free(new);
	    return 0;
	}
    }

    if (old->x_name != 0){
	new->x_name = icalmemory_strdup(old->x_name);
	if (new->x_name == 0){
	    icalparameter_free(new);
	    return 0;
	}
    }

    return new;
}

icalparameter* icalparameter_new_from_string(const char *str)
{
    char* eq;
    char* cpy;
    icalparameter_kind kind;
    icalparameter *param;

    icalerror_check_arg_rz(str != 0,"str");

    cpy = strdup(str);

    if (cpy == 0){
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return 0;
    }

    eq = strchr(cpy,'=');

    if(eq == 0){
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        return 0;
    }

    *eq = '\0';

    eq++;

    kind = icalparameter_string_to_kind(cpy);

    if(kind == ICAL_NO_PARAMETER){
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        return 0;
    }

    param = icalparameter_new_from_value_string(kind,eq);

    if(kind == ICAL_X_PARAMETER){
        icalparameter_set_xname(param,cpy);
    }

    free(cpy);

    return param;
    
}

icalparameter* icalparameter_new_from_value_string(icalparameter_kind kind,const  char* val)
{

    icalparameter* param=0;

    icalerror_check_arg_rz((val!=0),"val");

    switch (kind) {
	case ICAL_ALTREP_PARAMETER:
	{
	    param = icalparameter_new_altrep(val);

	    break;
	}
	case ICAL_CN_PARAMETER:
	{
	    param = icalparameter_new_cn(val);

	    break;
	}
	case ICAL_CUTYPE_PARAMETER:
	{
	    if(strcmp(val,"INDIVIDUAL") == 0){ 
		param = icalparameter_new_cutype(ICAL_CUTYPE_INDIVIDUAL);
	    }
	    else if(strcmp(val,"GROUP") == 0){ 
		param = icalparameter_new_cutype(ICAL_CUTYPE_GROUP);
	    }
	    else if(strcmp(val,"RESOURCE") == 0){ 
		param = icalparameter_new_cutype(ICAL_CUTYPE_RESOURCE);
	    }
	    else if(strcmp(val,"ROOM") == 0){ 
		param = icalparameter_new_cutype(ICAL_CUTYPE_ROOM);
	    }
	    else if(strcmp(val,"UNKNOWN") == 0){ 
		param = icalparameter_new_cutype(ICAL_CUTYPE_UNKNOWN);
	    }
	    else {
		param = icalparameter_new_cutype(ICAL_CUTYPE_X);
		icalparameter_set_xvalue(param,val);
	    } 
	    break;
	}

	case ICAL_DELEGATEDFROM_PARAMETER:
	{
	    param = icalparameter_new_delegatedfrom(val);

	    break;
	}
	case ICAL_DELEGATEDTO_PARAMETER:
	{
	    param = icalparameter_new_delegatedto(val);

	    break;
	}
	case ICAL_DIR_PARAMETER:
	{
	    param = icalparameter_new_dir(val);

	    break;
	}
	case ICAL_ENCODING_PARAMETER:
	{
	    if(strcmp(val,"BIT8") == 0){ 
		param = icalparameter_new_encoding(ICAL_ENCODING_8BIT);
	    }
	    else if(strcmp(val,"BASE64") == 0){ 
		param = icalparameter_new_encoding(ICAL_ENCODING_BASE64);
	    }
	    else {
		param = icalparameter_new_encoding(ICAL_ENCODING_X);
		icalparameter_set_xvalue(param,val);
	    } 
	    break;
	}
	case ICAL_FBTYPE_PARAMETER:
	{
	    if(strcmp(val,"FREE") == 0){ 
		param = icalparameter_new_fbtype(ICAL_FBTYPE_FREE);
	    }
	    else if(strcmp(val,"BUSY") == 0){ 
		param = icalparameter_new_fbtype(ICAL_FBTYPE_BUSY);
	    }
	    else if(strcmp(val,"BUSYUNAVAILABLE") == 0){ 
		param = icalparameter_new_fbtype(ICAL_FBTYPE_BUSYUNAVAILABLE);
	    }
	    else if(strcmp(val,"BUSYTENTATIVE") == 0){ 
		param = icalparameter_new_fbtype(ICAL_FBTYPE_BUSYTENTATIVE);
	    }
	    else {
		param = icalparameter_new_fbtype(ICAL_FBTYPE_X);
		icalparameter_set_xvalue(param,val);
	    } 
	    break;
	}
	case ICAL_FMTTYPE_PARAMETER:
	{
	    param = icalparameter_new_fmttype(val);
	    break;
	}
	case ICAL_LANGUAGE_PARAMETER:
	{
	    param = icalparameter_new_language(val);

	    break;
	}
	case ICAL_MEMBER_PARAMETER:
	{
	    param = icalparameter_new_member(val);

	    break;
	}
	case ICAL_PARTSTAT_PARAMETER:
	{
	    if(strcmp(val,"NEEDS-ACTION") == 0){ 
		param = icalparameter_new_partstat(ICAL_PARTSTAT_NEEDSACTION);
	    }
	    else if(strcmp(val,"ACCEPTED") == 0){ 
		param = icalparameter_new_partstat(ICAL_PARTSTAT_ACCEPTED);
	    }
	    else if(strcmp(val,"DECLINED") == 0){ 
		param = icalparameter_new_partstat(ICAL_PARTSTAT_DECLINED);
	    }
	    else if(strcmp(val,"TENTATIVE") == 0){ 
		param = icalparameter_new_partstat(ICAL_PARTSTAT_TENTATIVE);
	    }
	    else if(strcmp(val,"DELEGATED") == 0){ 
		param = icalparameter_new_partstat(ICAL_PARTSTAT_DELEGATED);
	    }
	    else if(strcmp(val,"COMPLETED") == 0){ 
		param = icalparameter_new_partstat(ICAL_PARTSTAT_COMPLETED);
	    }
	    else if(strcmp(val,"INPROCESS") == 0){ 
		param = icalparameter_new_partstat(ICAL_PARTSTAT_INPROCESS);
	    }
	    else {
		param = icalparameter_new_partstat(ICAL_PARTSTAT_X);
		icalparameter_set_xvalue(param,val);
	    } 
	    break;
	}
	case ICAL_RANGE_PARAMETER:
	{
	    if(strcmp(val,"THISANDFUTURE") == 0){ 
		param = icalparameter_new_range(ICAL_RANGE_THISANDFUTURE);
	    }
	    else if(strcmp(val,"THISANDPRIOR") == 0){ 
		param = icalparameter_new_range(ICAL_RANGE_THISANDPRIOR);
	    }

	    break;
	}
	case ICAL_RELATED_PARAMETER:
	{
	    if(strcmp(val,"START") == 0){ 
		param = icalparameter_new_related(ICAL_RELATED_START);
	    }
	    else if(strcmp(val,"END") == 0){ 
		param = icalparameter_new_related(ICAL_RELATED_END);
	    }

	    break;
	}
	case ICAL_RELTYPE_PARAMETER:
	{
	    if(strcmp(val,"PARENT") == 0){ 
		param = icalparameter_new_reltype(ICAL_RELTYPE_PARENT);
	    }
	    else if(strcmp(val,"CHILD") == 0){ 
		param = icalparameter_new_reltype(ICAL_RELTYPE_CHILD);
	    }
	    else if(strcmp(val,"SIBLING") == 0){ 
		param = icalparameter_new_reltype(ICAL_RELTYPE_SIBLING);
	    }
	    else {
		param = icalparameter_new_reltype(ICAL_RELTYPE_X);
		icalparameter_set_xvalue(param,val);
	    } 
	    break;
	}
	case ICAL_ROLE_PARAMETER:
	{
	    if(strcmp(val,"CHAIR") == 0){ 
		param = icalparameter_new_role(ICAL_ROLE_CHAIR);
	    }
	    else if(strcmp(val,"REQ-PARTICIPANT") == 0){ 
		param = icalparameter_new_role(ICAL_ROLE_REQPARTICIPANT);
	    }
	    else if(strcmp(val,"OPT-PARTICIPANT") == 0){ 
		param = icalparameter_new_role(ICAL_ROLE_OPTPARTICIPANT);
	    }
	    else if(strcmp(val,"NON-PARTICIPANT") == 0){ 
		param = icalparameter_new_role(ICAL_ROLE_NONPARTICIPANT);
	    }
	    else {
		param = icalparameter_new_role(ICAL_ROLE_X);
		icalparameter_set_xvalue(param,val);
	    } 
	    break;
	}
	case ICAL_RSVP_PARAMETER:
	{
	    if(strcmp(val,"TRUE") == 0){ 
		param = icalparameter_new_rsvp(1);
	    }
	    else if(strcmp(val,"FALSE") == 0){ 
		param = icalparameter_new_rsvp(0);
	    }

	    break;
	}
	case ICAL_SENTBY_PARAMETER:
	{
	    param = icalparameter_new_sentby(val);

	    break;
	}
	case ICAL_TZID_PARAMETER:
	{
	    param = icalparameter_new_tzid(val);

	    break;
	}
	case ICAL_VALUE_PARAMETER:
	{
	    if(strcmp(val,"BINARY") == 0){ 
		param = icalparameter_new_value(ICAL_VALUE_BINARY);
	    }
	    else if(strcmp(val,"BOOLEAN") == 0){ 
		param = icalparameter_new_value(ICAL_VALUE_BOOLEAN);
	    }
	    else if(strcmp(val,"CAL-ADDRESS") == 0){ 
		param = icalparameter_new_value(ICAL_VALUE_CALADDRESS);
	    }
	    else if(strcmp(val,"DATE") == 0){ 
		param = icalparameter_new_value(ICAL_VALUE_DATE);
	    }
	    else if(strcmp(val,"DATE-TIME") == 0){ 
		param = icalparameter_new_value(ICAL_VALUE_DATETIME);
	    }
	    else if(strcmp(val,"DURATION") == 0){ 
		param = icalparameter_new_value(ICAL_VALUE_DURATION);
	    }
	    else if(strcmp(val,"FLOAT") == 0){ 
		param = icalparameter_new_value(ICAL_VALUE_FLOAT);
	    }
	    else if(strcmp(val,"INTEGER") == 0){ 
		param = icalparameter_new_value(ICAL_VALUE_INTEGER);
	    }
	    else if(strcmp(val,"PERIOD") == 0){ 
		param = icalparameter_new_value(ICAL_VALUE_PERIOD);
	    }
	    else if(strcmp(val,"RECUR") == 0){ 
		param = icalparameter_new_value(ICAL_VALUE_RECUR);
	    }
	    else if(strcmp(val,"TEXT") == 0){ 
		param = icalparameter_new_value(ICAL_VALUE_TEXT);
	    }
	    else if(strcmp(val,"TIME") == 0){ 
		param = icalparameter_new_value(ICAL_VALUE_TIME);
	    }
	    else if(strcmp(val,"URI") == 0){ 
		param = icalparameter_new_value(ICAL_VALUE_URI);
	    }
	    else if(strcmp(val,"UTC-OFFSET") == 0){ 
		param = icalparameter_new_value(ICAL_VALUE_UTCOFFSET);
	    }
	    else {
		param = 0;
	    } 
	    break;
	}
	case ICAL_XLICERRORTYPE_PARAMETER:
	{

	    if(strcmp(val,"COMPONENT_PARSE_ERROR") == 0){ 
		param = icalparameter_new_xlicerrortype(ICAL_XLICERRORTYPE_COMPONENTPARSEERROR);
	    }
	    else if(strcmp(val,"PROPERTY_PARSE_ERROR") == 0){ 
		param = icalparameter_new_xlicerrortype(ICAL_XLICERRORTYPE_PROPERTYPARSEERROR);
	    }
	    else if(strcmp(val,"PARAMETER_NAME_PARSE_ERROR") == 0){ 
		param = icalparameter_new_xlicerrortype(ICAL_XLICERRORTYPE_PARAMETERNAMEPARSEERROR);
	    }
	    else if(strcmp(val,"PARAMETER_VALUE_PARSE_ERROR") == 0){ 
		param = icalparameter_new_xlicerrortype(ICAL_XLICERRORTYPE_PARAMETERVALUEPARSEERROR);
	    }
	    else if(strcmp(val,"VALUE_PARSE_ERROR") == 0){ 
		param = icalparameter_new_xlicerrortype(ICAL_XLICERRORTYPE_VALUEPARSEERROR);
	    }
	    else if(strcmp(val,"INVALID_ITIP") == 0){ 
		param = icalparameter_new_xlicerrortype(ICAL_XLICERRORTYPE_INVALIDITIP);
	    }
	    else if(strcmp(val,"MIME_PARSE_ERROR") == 0){ 
		param = icalparameter_new_xlicerrortype(ICAL_XLICERRORTYPE_MIMEPARSEERROR);
	    }
	    else if(strcmp(val,"UNKNOWN_VCAL_PROP_ERROR") == 0){ 
		param = icalparameter_new_xlicerrortype(ICAL_XLICERRORTYPE_UNKNOWNVCALPROPERROR);
	    }
	    break;
	}

	case ICAL_XLICCOMPARETYPE_PARAMETER:
	{

	    if(strcmp(val,"EQUAL") == 0){ 
		param = icalparameter_new_xliccomparetype(ICAL_XLICCOMPARETYPE_EQUAL);
	    }
	    else if(strcmp(val,"NOTEQUAL") == 0){ 
		param = icalparameter_new_xliccomparetype(ICAL_XLICCOMPARETYPE_NOTEQUAL);
	    }
	    else if(strcmp(val,"LESS") == 0){ 
		param = icalparameter_new_xliccomparetype(ICAL_XLICCOMPARETYPE_LESS);
	    }
	    else if(strcmp(val,"GREATER") == 0){ 
		param = icalparameter_new_xliccomparetype(ICAL_XLICCOMPARETYPE_GREATER);
	    }
	    else if(strcmp(val,"LESSEQUAL") == 0){ 
		param = icalparameter_new_xliccomparetype(ICAL_XLICCOMPARETYPE_LESSEQUAL);
	    }
	    else if(strcmp(val,"GREATEREQUAL") == 0){ 
		param = icalparameter_new_xliccomparetype(ICAL_XLICCOMPARETYPE_GREATEREQUAL);
	    }
	    else if(strcmp(val,"REGEX") == 0){ 
		param = icalparameter_new_xliccomparetype(ICAL_XLICCOMPARETYPE_REGEX);
	    } else {
		param = 0;
	    }
	    break;
	}
	

	case ICAL_X_PARAMETER:
	{
            param = icalparameter_new_x(val);
	    break;
	}

	case ICAL_NO_PARAMETER: 
	default:
	{
	    return 0;
	}
	

    }
    
    return param;
}


char no_parameter[]="Error: No Parameter";
char*
icalparameter_as_ical_string (icalparameter* parameter)
{
    struct icalparameter_impl* impl;
    size_t buf_size = 1024;
    char* buf; 
    char* buf_ptr;
    char *out_buf;
    const char *kind_string;

    char tend[1024]; /* HACK . Should be using memory buffer ring */

    icalerror_check_arg_rz( (parameter!=0), "parameter");

    /* Create new buffer that we can append names, parameters and a
       value to, and reallocate as needed. Later, this buffer will be
       copied to a icalmemory_tmp_buffer, which is managed internally
       by libical, so it can be given to the caller without fear of
       the caller forgetting to free it */

    buf = icalmemory_new_buffer(buf_size);
    buf_ptr = buf;
    impl = (struct icalparameter_impl*)parameter;

    if(impl->kind == ICAL_X_PARAMETER) {

	icalmemory_append_string(&buf, &buf_ptr, &buf_size, 
				 icalparameter_get_xname(impl));

    } else {

	kind_string = icalparameter_kind_to_string(impl->kind);
	
	if (impl->kind == ICAL_NO_PARAMETER || 
	    impl->kind == ICAL_ANY_PARAMETER || 
	    kind_string == 0)
	{
	    icalerror_set_errno(ICAL_BADARG_ERROR);
	    return 0;
	}
	
	
	/* Put the parameter name into the string */
	icalmemory_append_string(&buf, &buf_ptr, &buf_size, kind_string);

    }

	icalmemory_append_string(&buf, &buf_ptr, &buf_size, "=");
    
    switch (impl->kind) {
	case ICAL_CUTYPE_PARAMETER:
	{
	    switch (impl->data.v_cutype) {
		case ICAL_CUTYPE_INDIVIDUAL: {
		    strcpy(tend,"INDIVIDUAL");break;
		}
		case ICAL_CUTYPE_GROUP:{
		    strcpy(tend,"GROUP");break;
		}
		case ICAL_CUTYPE_RESOURCE: {
		    strcpy(tend,"RESOURCE");break;
		}
		case ICAL_CUTYPE_ROOM:{
		    strcpy(tend,"ROOM");break;
		}
		case ICAL_CUTYPE_UNKNOWN:{
		    strcpy(tend,"UNKNOWN");break;
		}
		case ICAL_CUTYPE_X:{
		    if (impl->string == 0){ return no_parameter;}
		    strcpy(tend,impl->string);break;
		}		
		default:{
		    icalerror_set_errno(ICAL_BADARG_ERROR);break;
		}
	    }
	    break;

	}
	case ICAL_ENCODING_PARAMETER:
	{
	    switch (impl->data.v_encoding) {
		case ICAL_ENCODING_8BIT: {
		    strcpy(tend,"8BIT");break;
		}
		case ICAL_ENCODING_BASE64:{
		    strcpy(tend,"BASE64");break;
		}
		default:{
		    icalerror_set_errno(ICAL_BADARG_ERROR);break;
		}
	    }
	    break;
	}

	case ICAL_FBTYPE_PARAMETER:
	{
	    switch (impl->data.v_fbtype) {
		case ICAL_FBTYPE_FREE:{
		    strcpy(tend,"FREE");break;
		}
		case ICAL_FBTYPE_BUSY: {
		    strcpy(tend,"BUSY");break;
		}
		case ICAL_FBTYPE_BUSYUNAVAILABLE:{
		    strcpy(tend,"BUSYUNAVAILABLE");break;
		}
		case ICAL_FBTYPE_BUSYTENTATIVE:{
		    strcpy(tend,"BUSYTENTATIVE");break;
		}
		case ICAL_FBTYPE_X:{
		    if (impl->string == 0){ return no_parameter;}
		    strcpy(tend,impl->string);break;
		}
		default:{
		    icalerror_set_errno(ICAL_BADARG_ERROR);break;
		}
	    }
	    break;

	}
	case ICAL_PARTSTAT_PARAMETER:
	{
	    switch (impl->data.v_partstat) {
		case ICAL_PARTSTAT_NEEDSACTION: {
		    strcpy(tend,"NEEDS-ACTION");break;
		}
		case ICAL_PARTSTAT_ACCEPTED: {
		    strcpy(tend,"ACCEPTED");break;
		}
		case ICAL_PARTSTAT_DECLINED:{
		    strcpy(tend,"DECLINED");break;
		}
		case ICAL_PARTSTAT_TENTATIVE:{
		    strcpy(tend,"TENTATIVE");break;
		}
		case ICAL_PARTSTAT_DELEGATED:{
		    strcpy(tend,"DELEGATED");break;
		}
		case ICAL_PARTSTAT_COMPLETED:{
		    strcpy(tend,"COMPLETED");break;
		}
		case ICAL_PARTSTAT_INPROCESS:{
		    strcpy(tend,"INPROCESS");break;
		}
		case ICAL_PARTSTAT_X:{
		    if (impl->string == 0){ return no_parameter;}
		    strcpy(tend,impl->string);break;
		}
		default:{
		    icalerror_set_errno(ICAL_BADARG_ERROR);break;
		}
	    }
	    break;

	}
	case ICAL_RANGE_PARAMETER:
	{
	    switch (impl->data.v_range) {
		case ICAL_RANGE_THISANDPRIOR: {
		    strcpy(tend,"THISANDPRIOR");break;
		}
		case ICAL_RANGE_THISANDFUTURE: {
		    strcpy(tend,"THISANDFUTURE");break;
		}
		default:{
		    icalerror_set_errno(ICAL_BADARG_ERROR);break;
		}
	    }
	    break;
	}
	case ICAL_RELATED_PARAMETER:
	{
	    switch (impl->data.v_related) {
		case ICAL_RELATED_START: {
		    strcpy(tend,"START");break;
		}
		case ICAL_RELATED_END: {
		    strcpy(tend,"END");break;
		}
		default:{
		    icalerror_set_errno(ICAL_BADARG_ERROR);break;
		}
	    }
	    break;
	}
	case ICAL_RELTYPE_PARAMETER:
	{
	    switch (impl->data.v_reltype) {
		case ICAL_RELTYPE_PARENT: {
		    strcpy(tend,"PARENT");break;
		}
		case ICAL_RELTYPE_CHILD:{
		    strcpy(tend,"CHILD");break;
		}
		case ICAL_RELTYPE_SIBLING:{
		    strcpy(tend,"SIBLING");break;
		}
		case ICAL_RELTYPE_X:{
		    if (impl->string == 0){ return no_parameter;}
		    strcpy(tend,impl->string);break;
		}
		default:{
		    icalerror_set_errno(ICAL_BADARG_ERROR);break;
		}
	    }
	    break;
	}
	case ICAL_ROLE_PARAMETER:
	{
	    switch (impl->data.v_role) {
		case ICAL_ROLE_CHAIR: {
		    strcpy(tend,"CHAIR");break;
		}
		case ICAL_ROLE_REQPARTICIPANT: {
		    strcpy(tend,"REQ-PARTICIPANT");break;
		}
		case ICAL_ROLE_OPTPARTICIPANT:  {
		    strcpy(tend,"OPT-PARTICIPANT");break;
		}
		case ICAL_ROLE_NONPARTICIPANT: {
		    strcpy(tend,"NON-PARTICIPANT");break;
		}
		case ICAL_ROLE_X:{
		    if (impl->string == 0){ return no_parameter;}
		    strcpy(tend,impl->string);break;
		}
		default:{
		    icalerror_set_errno(ICAL_BADARG_ERROR);break;
		}
	    }
	    break;
	}
	case ICAL_RSVP_PARAMETER:
	{
	    switch (impl->data.v_rsvp) {
		case 1: {
		    strcpy(tend,"TRUE");break;
		}
		case 0: {
		    strcpy(tend,"FALSE");break;
		}
		default:{
		    icalerror_set_errno(ICAL_BADARG_ERROR);break;
		}
	    }
	    break;
	}
	case ICAL_VALUE_PARAMETER:
	{
	    switch (impl->data.v_value) {
		case ICAL_VALUE_BINARY:  {
		    strcpy(tend,"BINARY");break;
		}
		case ICAL_VALUE_BOOLEAN:  {
		    strcpy(tend,"BOOLEAN");break;
		}
		case ICAL_VALUE_CALADDRESS:  {
		    strcpy(tend,"CAL-ADDRESS");break;
		}
		case ICAL_VALUE_DATE:  {
		    strcpy(tend,"DATE");break;
		}
		case ICAL_VALUE_DATETIME:  {
		    strcpy(tend,"DATE-TIME");break;
		}
		case ICAL_VALUE_DURATION:  {
		    strcpy(tend,"DURATION");break;
		}
		case ICAL_VALUE_FLOAT:  {
		    strcpy(tend,"FLOAT");break;
		}
		case ICAL_VALUE_INTEGER:  {
		    strcpy(tend,"INTEGER");break;
		}
		case ICAL_VALUE_PERIOD:  {
		    strcpy(tend,"PERIOD");break;
		}
		case ICAL_VALUE_RECUR:  {
		    strcpy(tend,"RECUR");break;
		}
		case ICAL_VALUE_TEXT:  {
		    strcpy(tend,"TEXT");break;
		}
		case ICAL_VALUE_TIME:  {
		    strcpy(tend,"TIME");break;
		}
		case ICAL_VALUE_URI:  {
		    strcpy(tend,"URI");break;
		}
		case ICAL_VALUE_UTCOFFSET: {
		    strcpy(tend,"UTC-OFFSET");break;
		}
		case ICAL_VALUE_X: {
		    if (impl->string == 0){ return no_parameter;}
		    strcpy(tend,impl->string);break;
		}
		default:{
		    strcpy(tend,"ERROR");
		    icalerror_set_errno(ICAL_BADARG_ERROR);break;
		}
	    }
	    break;
	}


	case ICAL_XLICERRORTYPE_PARAMETER:
	{
	    switch (impl->data.v_xlicerrortype) {
		case ICAL_XLICERRORTYPE_COMPONENTPARSEERROR:
		{
		    strcpy(tend,"COMPONENT_PARSE_ERROR");break;
		}
		case ICAL_XLICERRORTYPE_PROPERTYPARSEERROR:
		{
		    strcpy(tend,"PROPERTY_PARSE_ERROR");break;
		}
		case ICAL_XLICERRORTYPE_PARAMETERNAMEPARSEERROR:
		{
		    strcpy(tend,"PARAMETER_NAME_PARSE_ERROR");break;
		}
		case ICAL_XLICERRORTYPE_PARAMETERVALUEPARSEERROR:
		{
		    strcpy(tend,"PARAMETER_VALUE_PARSE_ERROR");break;
		}
		case ICAL_XLICERRORTYPE_VALUEPARSEERROR:
		{
		    strcpy(tend,"VALUE_PARSE_ERROR");break;
		}
		case ICAL_XLICERRORTYPE_INVALIDITIP:
		{
		    strcpy(tend,"INVALID_ITIP");break;
		}
		case ICAL_XLICERRORTYPE_UNKNOWNVCALPROPERROR:
		{
		    strcpy(tend,"UNKNOWN_VCAL_PROP_ERROR");break;
		}
		case ICAL_XLICERRORTYPE_MIMEPARSEERROR:
		{
		    strcpy(tend,"MIME_PARSE_ERROR");break;
		}
	    }
	    break;
	}
	
	case ICAL_XLICCOMPARETYPE_PARAMETER:
	{
	    switch (impl->data.v_xliccomparetype) {
		case ICAL_XLICCOMPARETYPE_EQUAL:
		{
		    strcpy(tend,"EQUAL");break;
		}
		case ICAL_XLICCOMPARETYPE_NOTEQUAL:
		{
		    strcpy(tend,"NOTEQUAL");break;
		}
		case ICAL_XLICCOMPARETYPE_LESS:
		{
		    strcpy(tend,"LESS");break;
		}
		case ICAL_XLICCOMPARETYPE_GREATER:
		{
		    strcpy(tend,"GREATER");break;
		}
		case ICAL_XLICCOMPARETYPE_LESSEQUAL:
		{
		    strcpy(tend,"LESSEQUAL");break;
		}
		case ICAL_XLICCOMPARETYPE_GREATEREQUAL:
		{
		    strcpy(tend,"GREATEREQUAL");break;
		}
		case ICAL_XLICCOMPARETYPE_REGEX:
		{
		    strcpy(tend,"REGEX");break;
		}
		default:{
		    icalerror_set_errno(ICAL_BADARG_ERROR);break;
		}
		break;
	    }

	    break;
	}


	case ICAL_SENTBY_PARAMETER:
	case ICAL_TZID_PARAMETER:
	case ICAL_X_PARAMETER:
	case ICAL_FMTTYPE_PARAMETER:
	case ICAL_LANGUAGE_PARAMETER:
	case ICAL_MEMBER_PARAMETER:
	case ICAL_DELEGATEDFROM_PARAMETER:
	case ICAL_DELEGATEDTO_PARAMETER:
	case ICAL_DIR_PARAMETER:
	case ICAL_ALTREP_PARAMETER:
	case ICAL_CN_PARAMETER:
	{
	    if (impl->string == 0){ return no_parameter;}
	    strcpy(tend,impl->string);break;
	    break;
	}

	case ICAL_NO_PARAMETER:
	case ICAL_ANY_PARAMETER:
	{
	    /* These are actually handled before the case/switch
               clause */
	}

    }

    icalmemory_append_string(&buf, &buf_ptr, &buf_size, tend); 

    /* Now, copy the buffer to a tmp_buffer, which is safe to give to
       the caller without worring about de-allocating it. */

    
    out_buf = icalmemory_tmp_buffer(strlen(buf));
    strcpy(out_buf, buf);

    icalmemory_free_buffer(buf);

    return out_buf;

}


int
icalparameter_is_valid (icalparameter* parameter);


icalparameter_kind
icalparameter_isa (icalparameter* parameter)
{
    if(parameter == 0){
	return ICAL_NO_PARAMETER;
    }

    return ((struct icalparameter_impl *)parameter)->kind;
}


int
icalparameter_isa_parameter (void* parameter)
{
    struct icalparameter_impl *impl = (struct icalparameter_impl *)parameter;

    if (parameter == 0){
	return 0;
    }

    if (strcmp(impl->id,"para") == 0) {
	return 1;
    } else {
	return 0;
    }
}


void
icalparameter_set_xname (icalparameter* param, const char* v)
{
    struct icalparameter_impl *impl = (struct icalparameter_impl*)param;
    icalerror_check_arg_rv( (param!=0),"param");
    icalerror_check_arg_rv( (v!=0),"v");

    if (impl->x_name != 0){
	free((void*)impl->x_name);
    }

    impl->x_name = icalmemory_strdup(v);

    if (impl->x_name == 0){
	errno = ENOMEM;
    }

}

const char*
icalparameter_get_xname (icalparameter* param)
{
    struct icalparameter_impl *impl = (struct icalparameter_impl*)param;
    icalerror_check_arg_rz( (param!=0),"param");

    return impl->x_name;
}

void
icalparameter_set_xvalue (icalparameter* param, const char* v)
{
    struct icalparameter_impl *impl = (struct icalparameter_impl*)param;

    icalerror_check_arg_rv( (param!=0),"param");
    icalerror_check_arg_rv( (v!=0),"v");

    if (impl->string != 0){
	free((void*)impl->string);
    }

    impl->string = icalmemory_strdup(v);

    if (impl->string == 0){
	errno = ENOMEM;
    }

}

const char*
icalparameter_get_xvalue (icalparameter* param)
{
    struct icalparameter_impl *impl = (struct icalparameter_impl*)param;

    icalerror_check_arg_rz( (param!=0),"param");

    return impl->string;

}

void icalparameter_set_parent(icalparameter* param,
			     icalproperty* property)
{
    struct icalparameter_impl *impl = (struct icalparameter_impl*)param;

    icalerror_check_arg_rv( (param!=0),"param");

    impl->parent = property;
}

icalproperty* icalparameter_get_parent(icalparameter* param)
{
    struct icalparameter_impl *impl = (struct icalparameter_impl*)param;

    icalerror_check_arg_rz( (param!=0),"param");

    return impl->parent;
}


/* Everything below this line is machine generated. Do not edit. */
/* ALTREP */
