/*======================================================================
 FILE: icalattachimpl.h
 CREATOR: acampi 28 May 02

 SPDX-FileCopyrightText: 2000, Andrea Campi <a.campi@inet.it>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
======================================================================*/

#ifndef ICALATTACHIMPL_H
#define ICALATTACHIMPL_H

#include "icalattach.h"

/* Private structure for ATTACH values */
struct icalattach_impl
{
    /* Reference count */
    int refcount;

    union
    {
        /* URL attachment data */
        struct
        {
            char *url;
        } url;

        /* Inline data */
        struct
        {
            char *data;
            icalattach_free_fn_t free_fn;
            void *free_fn_data;
        } data;
    } u;

    /* TRUE if URL, FALSE if inline data */
    unsigned int is_url : 1;
};

#endif
