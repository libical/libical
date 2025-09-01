/*======================================================================
 FILE: vcardtypes.h
 CREATOR: Ken Murchison 24 Aug 2022

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 ======================================================================*/

#ifndef VCARDTYPES_H
#define VCARDTYPES_H

#include "libical_vcard_export.h"

typedef struct vcardgeotype
{
    const char *uri;
    struct
    {
        const char *lat;
        const char *lon;
    } coords;
} vcardgeotype;

typedef struct vcardtztype
{
    const char *tzid;
    const char *uri;
    int utcoffset;
} vcardtztype;

#endif /* VCARDTYPES_H */
