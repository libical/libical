/*======================================================================
 FILE: vcardvalueimpl.h
 CREATOR: Ken Murchison 24 Aug 2022

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 ======================================================================*/

#ifndef VCARDVALUEIMPL_H
#define VCARDVALUEIMPL_H

#include "vcardderivedvalue.h"
#include "vcardproperty.h"
#include "vcardtime.h"

#define LIBICAL_VCARDVALUE_ID_LENGTH 5
struct vcardvalue_impl {
    vcardvalue_kind kind; /* the kind that is visible from the outside */

    char id[LIBICAL_VCARDVALUE_ID_LENGTH];
    int size;
    vcardproperty *parent;
    char *x_value;

    union data {
        vcardstructuredtype *v_structured;
        /*char *v_name; */ /* use v_structured field[0-4] only */
        /*char *v_adr; */  /* use v_structured field[0-6] */

        /*char *v_nickname; */   /* use v_textlist */
        /*char *v_gender; */     /* use v_textlist */
        /*char *v_org; */        /* use v_textlist */
        /*char *v_categories; */ /* use v_textlist */
        vcardstrarray *v_textlist;

        vcardgeotype v_geo;

        const char *v_string;
        /*char *v_text; */         /* use v_string */
        /*char *v_language_tag; */ /* use v_string */
        /*char *v_uri; */          /* use v_string */

        float v_float;

        int v_int;
        /*int v_boolean; */   /* use v_int */
        /*int v_integer; */   /* use v_int */
        /*int v_utcoffset; */ /* use v_int */

        vcardtimetype v_time;
        /*struct vcardtimetype v_date; */          /* use v_time */
        /*struct vcardtimetype v_time; */          /* use v_time */
        /*struct vcardtimetype v_datetime; */      /* use v_time */
        /*struct vcardtimetype v_dateandortime; */ /* use v_time */

        int v_enum;
        /* v_enum takes care of several enumerated types including:
           vcardproperty_kind    v_kind;
           vcardproperty_version v_version;
         */

    } data;
};

#endif /* VCARDVALUEIMPL_H */
