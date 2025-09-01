/*======================================================================
 FILE: icalclusterimpl.h
 CREATOR: acampi 13 March 2002

 SPDX-FileCopyrightText: 2002 Andrea Campi <a.campi@inet.it>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
======================================================================*/

#ifndef ICALCLUSTERIMPL_H
#define ICALCLUSTERIMPL_H

#include "icalcomponent.h"

/* This definition is in its own file so it can be kept out of the
   main header file, but used by "friend classes" like icaldirset*/

#define ICALCLUSTER_ID "clus"

struct icalcluster_impl
{
    char id[5]; /* clus */

    char *key;
    icalcomponent *data;
    int changed;
};

#endif
