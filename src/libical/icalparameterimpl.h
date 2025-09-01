/*======================================================================
 FILE: icalparameterimpl.h
 CREATOR: eric 09 May 1999

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 The original code is icalderivedparameters.{c,h}

 Contributions from:
   Graham Davison (g.m.davison@computer.org)
======================================================================*/

#ifndef ICALPARAMETERIMPL_H
#define ICALPARAMETERIMPL_H

#include "icalproperty.h"

struct icalparameter_impl
{
    icalparameter_kind kind;
    char id[5];
    int size;
    const char *string;
    const char *x_name;
    icalproperty *parent;

    icalvalue_kind value_kind;
    int is_multivalued;

    int data;
    struct icaldurationtype duration;
    icalarray *values; /* array of enums or strings */
};

#endif /*ICALPARAMETER_IMPL */
