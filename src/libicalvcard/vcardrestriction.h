/*======================================================================
 FILE: vcardrestriction.h
 CREATOR: Ken Murchison 24 Aug 2022

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 ======================================================================*/

#ifndef VCARDRESTRICTION_H
#define VCARDRESTRICTION_H

#include "libical_vcard_export.h"
#include "vcardcomponent.h"
#include "vcardproperty.h"

/**
 * @brief The kinds of vcardrestrictions there are
 *
 * These must stay in this order for vcardrestriction_compare to work
 */
typedef enum vcardrestriction_kind
{
    /** No restriction. */
    VCARD_RESTRICTION_NONE = 0, /* 0 */

    /** Zero. */
    VCARD_RESTRICTION_ZERO, /* 1 */

    /** One. */
    VCARD_RESTRICTION_ONE, /* 2 */

    /** Zero or more. */
    VCARD_RESTRICTION_ZEROPLUS, /* 3 */

    /** One or more. */
    VCARD_RESTRICTION_ONEPLUS, /* 4 */

    /** Zero or one. */
    VCARD_RESTRICTION_ZEROORONE, /* 5 */

    /** Zero or one, exclusive with another property. */
    VCARD_RESTRICTION_ONEEXCLUSIVE, /* 6 */

    /** Zero or one, mutual with another property. */
    VCARD_RESTRICTION_ONEMUTUAL, /* 7 */

    /** Unknown. */
    VCARD_RESTRICTION_UNKNOWN /* 8 */
} vcardrestriction_kind;

/**
 * @brief Checks if the given @a count is in accordance with the given
 *  restriction, @a restr.
 * @param restr The restriction to apply to the @a count
 * @param count The amount present that is to be checked against the restriction
 * @return 1 if the restriction is met, 0 if not
 *
 * @par Example
 * ```c
 * assert(vcardrestriction_compare(VCARDRESTRICTION_ONEPLUS, 5) == true);
 * assert(vcardrestriction_compare(VCARDRESTRICTION_NONE,    3) == false);
 * ```
 */
LIBICAL_VCARD_EXPORT int vcardrestriction_compare(vcardrestriction_kind restr, int count);

/**
 * @brief Checks if a given `VCARD` meets all the restrictions imposed by
 *  the standard.
 * @param comp The `VCARD/XROOT` component to check
 * @return 1 if the restrictions are met, 0 if not
 *
 * @par Error handling
 * Returns 0 and sets ::vcarderrno if `NULL` is passed as @a comp, or if the
 * component is not a `VCARD`.
 *
 * @par Example
 * ```c
 * vcardcomponent *component = // ...
 *
 * // check component
 * assert(vcardrestriction_check(component) == true);
 * ```
 */
LIBICAL_VCARD_EXPORT int vcardrestriction_check(vcardcomponent *comp);

#endif /* VCARDRESTRICTION_H */
