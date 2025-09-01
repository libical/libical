/*======================================================================
 FILE: vcardparameterimpl.h
 CREATOR: Ken Murchison 24 Aug 2022 <murch@fastmailteam.com>

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 ======================================================================*/

#ifndef VCARDPARAMETERIMPL_H
#define VCARDPARAMETERIMPL_H

#include "vcardproperty.h"
#include "vcardderivedvalue.h"
#include "icalarray.h"

struct vcardparameter_impl
{
    vcardparameter_kind kind;
    char id[5];
    int size;
    const char *x_name;
    vcardproperty *parent;

    vcardvalue_kind value_kind;
    int is_multivalued;

    int data;           /* int or enum */
    const char *string; /* string */
    icalarray *values;  /* array of enums or strings */
    vcardtimetype date; /* date, time, date-time, date-and-or-time, timestamp */
    vcardstructuredtype *structured;
};

#endif /* VCARDPARAMETER_IMPL */
