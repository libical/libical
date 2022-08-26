/*======================================================================
 FILE: vcardparameterimpl.h

======================================================================*/

#ifndef VCARDPARAMETERIMPL_H
#define VCARDPARAMETERIMPL_H

#include "icalarray.h"
#include "vcardproperty.h"
#include "vcardderivedvalue.h"

struct vcardparameter_impl
{
    vcardparameter_kind kind;
    char id[5];
    int size;
    const char *x_name;
    vcardproperty *parent;

    vcardvalue_kind value_kind;
    int is_multivalued;

    int data;
    const char *string;
    icalarray *values;
};

#endif /* VCARDPARAMETER_IMPL */
