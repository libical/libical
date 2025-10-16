/*======================================================================
 FILE: vcardvalue.h
 CREATOR: Ken Murchison 24 Aug 2022

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 ======================================================================*/

#ifndef VCARDVALUE_H
#define VCARDVALUE_H

#include "libical_vcard_export.h"
#include "vcardvalueimpl.h"

#define VCARD_BOOLEAN_TRUE 1
#define VCARD_BOOLEAN_FALSE 0

LIBICAL_VCARD_EXPORT vcardvalue *vcardvalue_new(vcardvalue_kind kind);

LIBICAL_VCARD_EXPORT vcardvalue *vcardvalue_clone(const vcardvalue *old);

LIBICAL_VCARD_EXPORT vcardvalue *vcardvalue_new_from_string(vcardvalue_kind kind,
                                                            const char *str);

LIBICAL_VCARD_EXPORT void vcardvalue_free(vcardvalue *value);

LIBICAL_VCARD_EXPORT bool vcardvalue_is_valid(const vcardvalue *value);

LIBICAL_VCARD_EXPORT const char *vcardvalue_as_vcard_string(const vcardvalue *value);

LIBICAL_VCARD_EXPORT char *vcardvalue_as_vcard_string_r(const vcardvalue *value);

LIBICAL_VCARD_EXPORT vcardvalue_kind vcardvalue_isa(const vcardvalue *value);

LIBICAL_VCARD_EXPORT bool vcardvalue_isa_value(void *);

/* Convert enumerations */

LIBICAL_VCARD_EXPORT vcardvalue_kind vcardvalue_string_to_kind(const char *str);

LIBICAL_VCARD_EXPORT const char *vcardvalue_kind_to_string(const vcardvalue_kind kind);

/** Check validity of a specific vcardvalue_kind **/
LIBICAL_VCARD_EXPORT bool vcardvalue_kind_is_valid(const vcardvalue_kind kind);

/* Duplicate and dequote a TEXT value */
LIBICAL_VCARD_EXPORT char *vcardvalue_strdup_and_dequote_text(const char **str,
                                                              const char *sep);

#endif /* VCARDVALUE_H */
