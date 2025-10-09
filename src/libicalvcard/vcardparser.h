/*======================================================================
 FILE: vcardparser.h
 CREATOR: Ken Murchison 24 Aug 2022
 CONTRIBUTOR: Bron Gondwana

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 ======================================================================*/

#ifndef VCARDPARSER_H
#define VCARDPARSER_H

#include "libical_vcard_export.h"
#include "vcardcomponent.h"

LIBICAL_VCARD_EXPORT vcardcomponent *vcardparser_parse_string(const char *str);
LIBICAL_VCARD_EXPORT const char *vcardparser_errstr(int err);

#endif /* VCARDPARSER_H */
