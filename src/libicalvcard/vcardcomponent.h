/*======================================================================
 FILE: vcardcomponent.h
 CREATOR: Ken Murchison 24 Aug 2022 <murch@fastmailteam.com>

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 ======================================================================*/

#ifndef VCARDCOMPONENT_H
#define VCARDCOMPONENT_H

#include "libical_sentinel.h"
#include "libical_vcard_export.h"
#include "vcardproperty.h"

#include <stdbool.h>

typedef enum vcardcomponent_kind
{
    VCARD_NO_COMPONENT,
    VCARD_ANY_COMPONENT,   /* Used to select all components */
    VCARD_XROOT_COMPONENT, /* Used as container for multiple vCards */
    VCARD_VCARD_COMPONENT,
    VCARD_X_COMPONENT,
    VCARD_NUM_COMPONENT_TYPES /* MUST be last (unless we can put NO_COMP last) */
} vcardcomponent_kind;

typedef struct vcardcomponent_impl vcardcomponent;

/** @brief Constructor
 */
LIBICAL_VCARD_EXPORT vcardcomponent *vcardcomponent_new(vcardcomponent_kind kind);

/**
 * @brief Deeply clones an vcard.
 * Returns a pointer to the memory for the newly cloned vcard.
 * @since 4.0
 */
LIBICAL_VCARD_EXPORT vcardcomponent *vcardcomponent_clone(const vcardcomponent *old);

/** @brief Constructor
 */
LIBICAL_VCARD_EXPORT vcardcomponent *vcardcomponent_new_from_string(const char *str);

/** @brief Constructor
 */
LIBICAL_VCARD_EXPORT LIBICAL_SENTINEL vcardcomponent *vcardcomponent_vanew(vcardcomponent_kind kind, ...);

/*** @brief Destructor
 */
LIBICAL_VCARD_EXPORT void vcardcomponent_free(vcardcomponent *comp);

LIBICAL_VCARD_EXPORT char *vcardcomponent_as_vcard_string(vcardcomponent *comp);

LIBICAL_VCARD_EXPORT char *vcardcomponent_as_vcard_string_r(vcardcomponent *comp);

LIBICAL_VCARD_EXPORT bool vcardcomponent_is_valid(const vcardcomponent *comp);

LIBICAL_VCARD_EXPORT vcardcomponent_kind vcardcomponent_isa(const vcardcomponent *comp);

LIBICAL_VCARD_EXPORT bool vcardcomponent_isa_component(void *comp);

/***** Working with Properties *****/

LIBICAL_VCARD_EXPORT void vcardcomponent_add_property(vcardcomponent *comp,
                                                      vcardproperty *property);

LIBICAL_VCARD_EXPORT void vcardcomponent_remove_property(vcardcomponent *comp,
                                                         vcardproperty *property);

LIBICAL_VCARD_EXPORT int vcardcomponent_count_properties(vcardcomponent *comp,
                                                         vcardproperty_kind kind,
                                                         int ignore_alts);

/***** Working with Components *****/

LIBICAL_VCARD_EXPORT void vcardcomponent_add_component(vcardcomponent *parent, vcardcomponent *child);

LIBICAL_VCARD_EXPORT void vcardcomponent_remove_component(vcardcomponent *parent,
                                                          vcardcomponent *child);

LIBICAL_VCARD_EXPORT int vcardcomponent_count_components(vcardcomponent *comp,
                                                         vcardcomponent_kind kind);

/* Kind conversion routines */

LIBICAL_VCARD_EXPORT bool vcardcomponent_kind_is_valid(const vcardcomponent_kind kind);

LIBICAL_VCARD_EXPORT vcardcomponent_kind vcardcomponent_string_to_kind(const char *string);

LIBICAL_VCARD_EXPORT const char *vcardcomponent_kind_to_string(vcardcomponent_kind kind);

/* Iteration Routines. There are two forms of iterators, internal and
external. The internal ones came first, and are almost completely
sufficient, but they fail badly when you want to construct a loop that
removes components from the container.*/

/* Iterate through components */
LIBICAL_VCARD_EXPORT vcardcomponent *vcardcomponent_get_current_component(vcardcomponent *comp);

LIBICAL_VCARD_EXPORT vcardcomponent *vcardcomponent_get_first_component(vcardcomponent *comp,
                                                                        vcardcomponent_kind kind);
LIBICAL_VCARD_EXPORT vcardcomponent *vcardcomponent_get_next_component(vcardcomponent *comp,
                                                                       vcardcomponent_kind kind);

/**
 * @brief Sets the parent vcard for the specified vcardproperty @p property.
 * @since 3.0
 */
LIBICAL_VCARD_EXPORT void vcardproperty_set_parent(vcardproperty *property,
                                                   vcardcomponent *comp);

/**
 * @brief Returns the parent vcard for the specified @p property.
 */
LIBICAL_VCARD_EXPORT vcardcomponent *vcardproperty_get_parent(const vcardproperty *property);

/* Iterate through the properties */
LIBICAL_VCARD_EXPORT vcardproperty *vcardcomponent_get_current_property(vcardcomponent *comp);

LIBICAL_VCARD_EXPORT vcardproperty *vcardcomponent_get_first_property(vcardcomponent *comp,
                                                                      vcardproperty_kind kind);
LIBICAL_VCARD_EXPORT vcardproperty *vcardcomponent_get_next_property(vcardcomponent *comp,
                                                                     vcardproperty_kind kind);

/***** Working with embedded error properties *****/

/* Check the component against itip rules and insert error properties*/
/* Working with embedded error properties */
LIBICAL_VCARD_EXPORT int vcardcomponent_check_restrictions(vcardcomponent *comp);

/** @brief Returns the number of errors encountered parsing the data.
 *
 * This function counts the number times the X-LIC-ERROR occurs
 * in the data structure.
 */
LIBICAL_VCARD_EXPORT int vcardcomponent_count_errors(vcardcomponent *comp);

/** @brief Removes all X-LIC-ERROR properties*/
LIBICAL_VCARD_EXPORT void vcardcomponent_strip_errors(vcardcomponent *comp);

/**
 * @brief Normalizes (reorders and sorts the properties) the specified vcard @p comp.
 * @since 3.0
 */
LIBICAL_VCARD_EXPORT void vcardcomponent_normalize(vcardcomponent *comp);

LIBICAL_VCARD_EXPORT void vcardcomponent_transform(vcardcomponent *impl,
                                                   vcardproperty_version version);

/******************** Convenience routines **********************/

LIBICAL_VCARD_EXPORT enum vcardproperty_version vcardcomponent_get_version(vcardcomponent *comp);
LIBICAL_VCARD_EXPORT const char *vcardcomponent_get_uid(vcardcomponent *comp);
LIBICAL_VCARD_EXPORT const char *vcardcomponent_get_fn(vcardcomponent *comp);

#endif /* !VCARDCOMPONENT_H */
