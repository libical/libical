/* -*- Mode: C -*-
  ======================================================================
  FILE: icalparameterimpl.h
  CREATOR: eric 09 May 1999
  
  $Id: icalparameterimpl.h,v 1.1 2001-03-17 16:47:02 ebusboom Exp $
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef ICALPARAMETER_IMPL
#define ICALPARAMETER_IMPL

#include "icalparameter.h"
#include "icalproperty.h"

union icalparameter_impl_data {
	int v_int;
	int v_rsvp;
	icalparameter_cutype v_cutype;
	icalparameter_encoding v_encoding;
	icalparameter_fbtype v_fbtype;
	icalparameter_partstat v_partstat;
	icalparameter_range v_range;
	icalparameter_related v_related;
	icalparameter_reltype v_reltype;
	icalparameter_role v_role;
	icalparameter_value v_value;
	icalparameter_xlicerrortype v_xlicerrortype;
	icalparameter_xliccomparetype v_xliccomparetype;
} data;

struct icalparameter_impl
{
	icalparameter_kind kind;
	char id[5];
	int size;
	const char* string;
	const char* x_name;
	icalproperty* parent;

	union icalparameter_impl_data data;
};


#endif /*ICALPARAMETER_IMPL*/
