/*======================================================================
 FILE: vcardstructured.h

 CREATOR: Ken Murchison 24 Aug 2022

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)

 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 ======================================================================*/

#ifndef VCARDSTRUCTURED_H
#define VCARDSTRUCTURED_H

#include "libical_vcard_export.h"
#include "vcardstrarray.h"

#include <stdbool.h>

#define VCARD_MAX_STRUCTURED_FIELDS 20 // Extended ADR currently uses 18

typedef struct vcardstructuredtype {
    unsigned num_fields;
    vcardstrarray *field[VCARD_MAX_STRUCTURED_FIELDS];
} vcardstructuredtype;

LIBICAL_VCARD_EXPORT vcardstructuredtype *vcardstructured_new(void);

LIBICAL_VCARD_EXPORT char *vcardstructured_as_vcard_string_r(const vcardstructuredtype *s,
                                                             bool is_param);

LIBICAL_VCARD_EXPORT vcardstructuredtype *vcardstructured_from_string(const char *s);

LIBICAL_VCARD_EXPORT void vcardstructured_free(vcardstructuredtype *s);

#endif /* VCARDSTRUCTURED_H */
