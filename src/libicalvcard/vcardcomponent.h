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
#include "pvl.h"

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
 * @since 3.1.0
 */
LIBICAL_VCARD_EXPORT vcardcomponent *vcardcomponent_clone(const vcardcomponent *card);

/** @brief Constructor
 */
LIBICAL_VCARD_EXPORT vcardcomponent *vcardcomponent_new_from_string(const char *str);

/** @brief Constructor
 */
LIBICAL_VCARD_EXPORT LIBICAL_SENTINEL vcardcomponent *vcardcomponent_vanew(vcardcomponent_kind kind, ...);

/*** @brief Destructor
 */
LIBICAL_VCARD_EXPORT void vcardcomponent_free(vcardcomponent *card);

LIBICAL_VCARD_EXPORT char *vcardcomponent_as_vcard_string(vcardcomponent *card);

LIBICAL_VCARD_EXPORT char *vcardcomponent_as_vcard_string_r(vcardcomponent *card);

LIBICAL_VCARD_EXPORT int vcardcomponent_is_valid(vcardcomponent *card);

LIBICAL_VCARD_EXPORT vcardcomponent_kind vcardcomponent_isa(const vcardcomponent *component);

LIBICAL_VCARD_EXPORT int vcardcomponent_isa_component(void *component);

/***** Working with Properties *****/

LIBICAL_VCARD_EXPORT void vcardcomponent_add_property(vcardcomponent *card,
                                                      vcardproperty *property);

LIBICAL_VCARD_EXPORT void vcardcomponent_remove_property(vcardcomponent *card,
                                                         vcardproperty *property);

LIBICAL_VCARD_EXPORT int vcardcomponent_count_properties(vcardcomponent *card,
                                                         vcardproperty_kind kind,
                                                         int ignore_alts);

/***** Working with Components *****/

LIBICAL_VCARD_EXPORT void vcardcomponent_add_component(vcardcomponent *parent, vcardcomponent *child);

LIBICAL_VCARD_EXPORT void vcardcomponent_remove_component(vcardcomponent *parent,
                                                          vcardcomponent *child);

LIBICAL_VCARD_EXPORT int vcardcomponent_count_components(vcardcomponent *component,
                                                         vcardcomponent_kind kind);

/* Kind conversion routines */

LIBICAL_VCARD_EXPORT int vcardcomponent_kind_is_valid(const vcardcomponent_kind kind);

LIBICAL_VCARD_EXPORT vcardcomponent_kind vcardcomponent_string_to_kind(const char *string);

LIBICAL_VCARD_EXPORT const char *vcardcomponent_kind_to_string(vcardcomponent_kind kind);

/**
 *  This takes 2 VCARD components and merges the second one into the first.
 *  comp_to_merge will no longer exist after calling this function.
 */
LIBICAL_VCARD_EXPORT void vcardcomponent_merge_component(vcardcomponent *comp,
                                                         vcardcomponent *comp_to_merge);

/* Iteration Routines. There are two forms of iterators, internal and
external. The internal ones came first, and are almost completely
sufficient, but they fail badly when you want to construct a loop that
removes components from the container.*/

/* Iterate through components */
LIBICAL_VCARD_EXPORT vcardcomponent *vcardcomponent_get_current_component(vcardcomponent *component);

LIBICAL_VCARD_EXPORT vcardcomponent *vcardcomponent_get_first_component(vcardcomponent *component,
                                                                        vcardcomponent_kind kind);
LIBICAL_VCARD_EXPORT vcardcomponent *vcardcomponent_get_next_component(vcardcomponent *component,
                                                                       vcardcomponent_kind kind);

/**
 * @brief Sets the parent vcard for the specified vcardproperty @p property.
 * @since 3.0
 */
LIBICAL_VCARD_EXPORT void vcardproperty_set_parent(vcardproperty *property,
                                                   vcardcomponent *card);

/**
 * @brief Returns the parent vcard for the specified @p property.
 */
LIBICAL_VCARD_EXPORT vcardcomponent *vcardproperty_get_parent(const vcardproperty *property);

/* Iterate through the properties */
LIBICAL_VCARD_EXPORT vcardproperty *vcardcomponent_get_current_property(vcardcomponent *card);

LIBICAL_VCARD_EXPORT vcardproperty *vcardcomponent_get_first_property(vcardcomponent *card,
                                                                      vcardproperty_kind kind);
LIBICAL_VCARD_EXPORT vcardproperty *vcardcomponent_get_next_property(vcardcomponent *component,
                                                                     vcardproperty_kind kind);

/**
 *  This takes 2 VCARD components and merges the second one into the first.
 *  comp_to_merge will no longer exist after calling this function.
 */
LIBICAL_VCARD_EXPORT void vcardcomponent_merge_card(vcardcomponent *card,
                                                    vcardcomponent *card_to_merge);

/***** Working with embedded error properties *****/

/* Check the component against itip rules and insert error properties*/
/* Working with embedded error properties */
LIBICAL_VCARD_EXPORT int vcardcomponent_check_restrictions(vcardcomponent *card);

/** @brief Returns the number of errors encountered parsing the data.
 *
 * This function counts the number times the X-LIC-ERROR occurs
 * in the data structure.
 */
LIBICAL_VCARD_EXPORT int vcardcomponent_count_errors(vcardcomponent *card);

/** @brief Removes all X-LIC-ERROR properties*/
LIBICAL_VCARD_EXPORT void vcardcomponent_strip_errors(vcardcomponent *card);

/** @brief Converts some X-LIC-ERROR properties into RETURN-STATUS properties*/
LIBICAL_VCARD_EXPORT void vcardcomponent_convert_errors(vcardcomponent *card);

/**
 * @brief Normalizes (reorders and sorts the properties) the specified vcard @p comp.
 * @since 3.0
 */
LIBICAL_VCARD_EXPORT void vcardcomponent_normalize(vcardcomponent *card);

LIBICAL_VCARD_EXPORT void vcardcomponent_transform(vcardcomponent *impl,
                                                   vcardproperty_version version);

/******************** Convenience routines **********************/

LIBICAL_VCARD_EXPORT enum vcardproperty_version vcardcomponent_get_version(vcardcomponent *card);
LIBICAL_VCARD_EXPORT const char *vcardcomponent_get_uid(vcardcomponent *card);
LIBICAL_VCARD_EXPORT const char *vcardcomponent_get_fn(vcardcomponent *card);

#endif /* !VCARDCOMPONENT_H */
