/*======================================================================
 FILE: vcardtextlist.h
 CREATOR: Robert Stepanek 1 Feb 2026

 SPDX-FileCopyrightText: 2026, Fastmail Pty. Ltd. (https://fastmail.com)
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 ======================================================================*/

#ifndef VCARDTEXTLIST_H
#define VCARDTEXTLIST_H

#include "libical_vcard_export.h"
#include "vcardstrarray.h"

LIBICAL_VCARD_EXPORT vcardstrarray *vcardtextlist_new_from_string(const char *s, char sep);

#endif /* VCARDTEXTLIST_H */
