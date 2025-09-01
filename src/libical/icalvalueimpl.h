/*======================================================================
 FILE: icalvalueimpl.h
 CREATOR: eric 02 May 1999

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

  Contributions from:
     Graham Davison (g.m.davison@computer.org)
======================================================================*/

#ifndef ICALVALUEIMPL_H
#define ICALVALUEIMPL_H

#include "icalproperty.h"

struct icalvalue_impl
{
    icalvalue_kind kind; /*this is the kind that is visible from the outside */

    char id[5];
    int size;
    icalproperty *parent;
    char *x_value;

    union data
    {
        icalattach *v_attach;
        /* void *v_binary; */ /* use v_attach */

        const char *v_string;
        /*char *v_text; */       /* use v_string */
        /*char *v_caladdress; */ /* use v_string */
        /*char *v_query; */      /* use v_string */
        /*char *v_uri; */        /* use v_string */

        float v_float;

        int v_int;
        /*int v_boolean; */   /* use v_int */
        /*int v_integer; */   /* use v_int */
        /*int v_utcoffset; */ /* use v_int */

        struct icaldurationtype v_duration;

        struct icalperiodtype v_period;
        /*struct icalperiodtype v_datetimeperiod; */ /* use v_time/v_period */

        struct icalgeotype v_geo;

        struct icaltimetype v_time;
        /*struct icaltimetype v_date; */         /* use v_time */
        /*struct icaltimetype v_datetime; */     /* use v_time */
        /*struct icaltimetype v_datetimedate; */ /* use v_time */

        struct icalreqstattype v_requeststatus;

        struct icalrecurrencetype *v_recur;

        /*struct icaltriggertype v_trigger; */ /* use v_time/v_duration */

        int v_enum;
        /* v_enum takes care of several enumerated types including:
           icalproperty_method v_method;
           icalproperty_status v_status;
           icalproperty_action v_action;
           icalproperty_class v_class;
           icalproperty_transp v_transp;
           icalproperty_busytype v_busytype;
           icalproperty_taskmode v_taskmode;
           icalproperty_pollmode v_pollmode;
           icalproperty_pollcompletion v_pollcomplete;
         */

    } data;
};

#endif
