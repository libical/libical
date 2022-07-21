/*======================================================================
 FILE: vcardvalueimpl.h

======================================================================*/

#ifndef VCARDVALUEIMPL_H
#define VCARDVALUEIMPL_H

#include "vcardproperty.h"
#include "vcardderivedvalue.h"

struct vcardvalue_impl
{
    vcardvalue_kind kind;     /*this is the kind that is visible from the outside */

    char id[5];
    int size;
    vcardproperty *parent;
    char *x_value;

    union data
    {
        const char *v_string;
        /*char *v_text; */      /* use v_string */
        /*char *v_language_tag; *//* use v_string */
        /*char *v_uri; */       /* use v_string */

        float v_float;

        int v_int;
        /*int v_boolean; */  /* use v_int */
        /*int v_integer; */  /* use v_int */
        /*int v_utcoffset; *//* use v_int */

//        struct vcardtimetype v_time;
        /*struct vcardtimetype v_date; */         /* use v_time */
        /*struct vcardtimetype v_time; */         /* use v_time */
        /*struct vcardtimetype v_datetime; */     /* use v_time */
        /*struct vcardtimetype v_dateandortime; *//* use v_time */

        int v_enum;
        /* v_enum takes care of several enumerated types including:
           vcardproperty_method v_method;
           vcardproperty_status v_status;
           vcardproperty_action v_action;
           vcardproperty_class v_class;
           vcardproperty_transp v_transp;
           vcardproperty_busytype v_busytype;
           vcardproperty_taskmode v_taskmode;
           vcardproperty_pollmode v_pollmode;
           vcardproperty_pollcompletion v_pollcomplete;
         */

    } data;
};

#endif
