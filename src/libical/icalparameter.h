/* -*- Mode: C -*- */
/*======================================================================
  FILE: icalparam.h
  CREATOR: eric 20 March 1999


  $Id: icalparameter.h,v 1.5 2008-01-15 23:17:40 dothebart Exp $
  $Locker:  $

  

 (C) COPYRIGHT 2000, Eric Busboom <eric@softwarestudio.org>
     http://www.softwarestudio.org

 This program is free software; you can redistribute it and/or modify
 it under the terms of either: 

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.fsf.org/copyleft/lesser.html

  Or:

    The Mozilla Public License Version 1.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/

  The original code is icalparam.h

  ======================================================================*/

#ifndef ICALPARAM_H
#define ICALPARAM_H

#include "icalderivedparameter.h"

/* Declared in icalderivedparameter.h */
/*typedef struct icalparameter_impl icalparameter;*/

icalparameter* icalparameter_new(icalparameter_kind kind);
icalparameter* icalparameter_new_clone(icalparameter* p);

/* Create from string of form "PARAMNAME=VALUE" */
icalparameter* icalparameter_new_from_string(const char* value);

/* Create from just the value, the part after the "=" */
icalparameter* icalparameter_new_from_value_string(icalparameter_kind kind, const char* value);

void icalparameter_free(icalparameter* parameter);

char* icalparameter_as_ical_string(icalparameter* parameter);
char* icalparameter_as_ical_string_r(icalparameter* parameter);

int icalparameter_is_valid(icalparameter* parameter);

icalparameter_kind icalparameter_isa(icalparameter* parameter);

int icalparameter_isa_parameter(void* param);

/* Access the name of an X parameter */
void icalparameter_set_xname (icalparameter* param, const char* v);
const char* icalparameter_get_xname(icalparameter* param);
void icalparameter_set_xvalue (icalparameter* param, const char* v);
const char* icalparameter_get_xvalue(icalparameter* param);

/* Access the name of an IANA parameter */
void icalparameter_set_iana_name (icalparameter* param, const char* v);
const char* icalparameter_get_iana_name(icalparameter* param);
void icalparameter_set_iana_value (icalparameter* param, const char* v);
const char* icalparameter_get_iana_value(icalparameter* param);

/* returns 1 if parameters have same name in ICAL, otherwise 0 */
int icalparameter_has_same_name(icalparameter* param1, icalparameter* param2);

/* Convert enumerations */

const char* icalparameter_kind_to_string(icalparameter_kind kind);
icalparameter_kind icalparameter_string_to_kind(const char* string);



#endif 
