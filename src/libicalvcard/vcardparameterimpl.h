/*======================================================================
 FILE: vcardparameterimpl.h

======================================================================*/

#ifndef VCARDPARAMETERIMPL_H
#define VCARDPARAMETERIMPL_H

//#include "vcardproperty.h"

struct vcardparameter_impl
{
    vcardparameter_kind kind;
    char id[5];
    int size;
    const char *string;
    const char *x_name;
//    vcardproperty *parent;
void *parent;

    int data;
};

#endif /*VCARDPARAMETER_IMPL */
