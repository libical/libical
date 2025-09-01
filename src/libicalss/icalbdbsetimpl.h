/*======================================================================
 FILE: icalbdbsetimpl.h

 SPDX-FileCopyrightText: 2001, Critical Path
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
======================================================================*/

#ifndef ICALBDBSETIMPL_H
#define ICALBDBSETIMPL_H

#include "icalset.h"
#include <db.h>

/* This definition is in its own file so it can be kept out of the
   main header file, but used by "friend classes" like icaldirset*/

struct icalbdbset_impl
{
    icalset super; /**< parent class */
    const char *path;
    const char *subdb;
    const char *sindex;
    const char *key;
    void *data;
    int datasize;
    int changed;
    icalcomponent *cluster;
    icalgauge *gauge;
    DB_ENV *dbenv;
    DB *dbp;
    DB *sdbp;
    DBC *dbcp;
};

#endif
