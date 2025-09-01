/*======================================================================
 FILE: icalfilesetimpl.h
 CREATOR: eric 23 December 1999

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 The Original Code is eric. The Initial Developer of the Original
 Code is Eric Busboom
======================================================================*/

#ifndef ICALFILESETIMPL_H
#define ICALFILESETIMPL_H

#include "icalfileset.h"

struct icalfileset_impl
{
    icalset super;               /**< parent class */
    char *path;                  /**< pathname of file */
    icalfileset_options options; /**< copy of options passed to icalset_new() */

    icalcomponent *cluster; /**< cluster containing data */
    icalgauge *gauge;       /**< gauge for filtering out data */
    int changed;            /**< boolean flag, 1 if data has changed */
    int fd;                 /**< file descriptor */
};

#endif
