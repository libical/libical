/*======================================================================
  FILE: icaldirsetimpl.h
  CREATOR: eric 21 Aug 2000

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 The Original Code is eric. The Initial Developer of the Original
 Code is Eric Busboom
======================================================================*/

#ifndef ICALDIRSETIMPL_H
#define ICALDIRSETIMPL_H

#include "icalcluster.h"
#include "icaldirset.h"
#include "icalset.h"

/* This definition is in its own file so it can be kept out of the
   main header file, but used by "friend classes" like icalset*/

struct icaldirset_impl
{
    icalset super;               /**< parent class */
    char *dir;                   /**< directory containing ics files  */
    icaldirset_options options;  /**< copy of options passed to icalset_new() */
    icalcluster *cluster;        /**< cluster containing data */
    icalgauge *gauge;            /**< gauge for filtering out data  */
    int first_component;         /**< ??? */
    pvl_list directory;          /**< ??? */
    pvl_elem directory_iterator; /**< ??? */
};

#endif
