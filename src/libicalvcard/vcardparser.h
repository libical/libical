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
#include "vcardderivedvalue.h"

LIBICAL_VCARD_EXPORT vcardcomponent *vcardparser_parse_string(const char *str);
LIBICAL_VCARD_EXPORT const char *vcardparser_errstr(int err);

/**
 * @brief Callback function pointer to define x-property default value types.
 *
 * This typedef defines a pointer to a function that the vCard parser calls
 * to determine the default value type of an x-property.
 *
 * @param name The name of the x-property currently parsed. This is the name as
 * it appears in the vCard data except that it already is unfolded, e.g. it is
 * not normalized to upper case.
 * @param data A pointer to some user-defined callback data. Can be NULL.
 * @return The default value type. Return VCARD_X_VALUE if unknown.
 *
 * @see vcardparser_set_xprop_value_kind() how to set the callback.
 */
typedef vcardvalue_kind (*vcard_xprop_value_kind_func)(const char *name, void *data);

/**
 * @brief Registers a parser callback to override the default value type of an
 * x-property.
 *
 * Extended properties ("x-properties") in vCard do not have a default value
 * type. Instead, they are parsed as unknown values and preserved verbatim.
 * This function allows to register a callback to define the default value
 * type for some x-property. Any VALUE parameter set on the property overrides
 * the default value.
 *
 * @param func The function pointer to the callback. Use NULL to disable a
 * previously registered callback.
 * @param data Some callback-specific data. Can be NULL.
 *
 * For example, the callback might return VCARD_TEXT_VALUE when parsing an
 * x-property named "X-ABLabel".
 *
 * This function is not reentrant. Depending on libical is built, the callback
 * either is registered as a process-global or thread-local variable.
 */
LIBICAL_VCARD_EXPORT void vcardparser_set_xprop_value_kind(vcard_xprop_value_kind_func func, void *data);

#endif /* VCARDPARSER_H */
