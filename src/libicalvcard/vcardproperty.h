/*======================================================================
 FILE: vcardproperty.h
 CREATOR: Ken Murchison 24 Aug 2022

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 ======================================================================*/

#ifndef VCARDPROPERTY_H
#define VCARDPROPERTY_H

#include "libical_vcard_export.h"
#include "vcardderivedproperty.h" /* To get vcardproperty_kind enumerations */

#include <stdarg.h> /* for va_... */

/** @file vcardproperty.h */

LIBICAL_VCARD_EXPORT vcardproperty *vcardproperty_new(vcardproperty_kind kind);

LIBICAL_VCARD_EXPORT vcardproperty *vcardproperty_new_impl(vcardproperty_kind kind);

/** @brief Deeply clones an vcardproperty.
 *
 * Returns a pointer to the memory for the newly cloned vcardproperty.
 * @since 4.0
 */
LIBICAL_VCARD_EXPORT vcardproperty *vcardproperty_clone(const vcardproperty *old);

LIBICAL_VCARD_EXPORT vcardproperty *vcardproperty_new_from_string(const char *str);

LIBICAL_VCARD_EXPORT const char *vcardproperty_as_vcard_string(vcardproperty *prop);

LIBICAL_VCARD_EXPORT char *vcardproperty_as_vcard_string_r(vcardproperty *prop);

LIBICAL_VCARD_EXPORT void vcardproperty_free(vcardproperty *prop);

LIBICAL_VCARD_EXPORT vcardproperty_kind vcardproperty_isa(vcardproperty *property);

LIBICAL_VCARD_EXPORT bool vcardproperty_isa_property(void *property);

LIBICAL_VCARD_EXPORT void vcardproperty_add_parameters(struct vcardproperty_impl *prop, va_list args);

LIBICAL_VCARD_EXPORT void vcardproperty_add_parameter(vcardproperty *prop, vcardparameter *parameter);

LIBICAL_VCARD_EXPORT void vcardproperty_set_parameter(vcardproperty *prop, vcardparameter *parameter);

LIBICAL_VCARD_EXPORT void vcardproperty_set_parameter_from_string(vcardproperty *prop,
                                                                  const char *name,
                                                                  const char *value);
LIBICAL_VCARD_EXPORT const char *vcardproperty_get_parameter_as_string(vcardproperty *prop,
                                                                       const char *name);

LIBICAL_VCARD_EXPORT char *vcardproperty_get_parameter_as_string_r(vcardproperty *prop,
                                                                   const char *name);

/** @brief Removes all parameters with the specified kind.
 *
 *  @param prop   A valid vcardproperty.
 *  @param kind   The kind to remove (ex. VCARD_TZ_PARAMETER)
 *
 *  See vcardproperty_remove_parameter_by_name() and
 *  vcardproperty_remove_parameter_by_ref() for alternate ways of
 *  removing parameters
 */
LIBICAL_VCARD_EXPORT void vcardproperty_remove_parameter_by_kind(vcardproperty *prop,
                                                                 vcardparameter_kind kind);

/** @brief Removes all parameters with the specified name.
 *
 *  @param prop   A valid vcardproperty.
 *  @param name   The name of the parameter to remove
 *
 *  This function removes parameters with the given name.  The name
 *  corresponds to either a built-in name (TZID, etc.) or the name of
 *  an extended parameter (X-FOO)
 *
 *  See vcardproperty_remove_parameter_by_kind() and
 *  vcardproperty_remove_parameter_by_ref() for alternate ways of removing
 *  parameters
 */
LIBICAL_VCARD_EXPORT void vcardproperty_remove_parameter_by_name(vcardproperty *prop,
                                                                 const char *name);

/** @brief Removes the specified parameter reference from the property.
 *
 *  @param prop   A valid vcardproperty.
 *  @param param  A reference to a specific vcardparameter.
 *
 *  This function removes the specified parameter reference from the
 *  property.
 */
LIBICAL_VCARD_EXPORT void vcardproperty_remove_parameter_by_ref(vcardproperty *prop,
                                                                vcardparameter *param);

LIBICAL_VCARD_EXPORT int vcardproperty_count_parameters(const vcardproperty *prop);

/* Iterate through the parameters */
LIBICAL_VCARD_EXPORT vcardparameter *vcardproperty_get_first_parameter(vcardproperty *prop,
                                                                       vcardparameter_kind kind);
LIBICAL_VCARD_EXPORT vcardparameter *vcardproperty_get_next_parameter(vcardproperty *prop,
                                                                      vcardparameter_kind kind);
/* Access the value of the property */
LIBICAL_VCARD_EXPORT void vcardproperty_set_value(vcardproperty *prop, vcardvalue *value);
LIBICAL_VCARD_EXPORT void vcardproperty_set_value_from_string(vcardproperty *prop, const char *str,
                                                              const char *type);

LIBICAL_VCARD_EXPORT vcardvalue *vcardproperty_get_value(const vcardproperty *prop);
LIBICAL_VCARD_EXPORT const char *vcardproperty_get_value_as_string(const vcardproperty *prop);
LIBICAL_VCARD_EXPORT char *vcardproperty_get_value_as_string_r(const vcardproperty *prop);

/**
 * Sets the parent @p vcardproperty for the specified @p vcardvalue.
 */
LIBICAL_VCARD_EXPORT void vcardvalue_set_parent(vcardvalue *value, vcardproperty *property);

/**
 * Returns the parent @p vcardproperty for the specified @p vcardvalue.
 * @since 3.0
 */
LIBICAL_VCARD_EXPORT vcardproperty *vcardvalue_get_parent(const vcardvalue *value);

/* Deal with X properties */

LIBICAL_VCARD_EXPORT void vcardproperty_set_x_name(vcardproperty *prop, const char *name);
LIBICAL_VCARD_EXPORT const char *vcardproperty_get_x_name(const vcardproperty *prop);

/* Deal with grouped properties */

LIBICAL_VCARD_EXPORT void vcardproperty_set_group(vcardproperty *prop, const char *group);
LIBICAL_VCARD_EXPORT const char *vcardproperty_get_group(const vcardproperty *prop);

/** Returns the name of the property -- the type name converted to a
 *  string, or the value of _get_x_name if the type is and X
 *  property
 */
LIBICAL_VCARD_EXPORT const char *vcardproperty_get_property_name(const vcardproperty *prop);
LIBICAL_VCARD_EXPORT char *vcardproperty_get_property_name_r(const vcardproperty *prop);

LIBICAL_VCARD_EXPORT vcardvalue_kind vcardparameter_value_to_value_kind(vcardparameter_value value);

/**
 * Sets the parent @p vcardproperty for the specified @p vcardparameter.
 * @since 3.0
 */
LIBICAL_VCARD_EXPORT void vcardparameter_set_parent(vcardparameter *param, vcardproperty *property);

/**
 * Returns the parent @p vcardproperty for the specified @p vcardparameter.
 * @since 3.0
 */
LIBICAL_VCARD_EXPORT vcardproperty *vcardparameter_get_parent(const vcardparameter *param);

/* Convert kinds to string and get default value type */
LIBICAL_VCARD_EXPORT vcardvalue_kind vcardproperty_kind_to_value_kind(vcardproperty_kind kind);
LIBICAL_VCARD_EXPORT vcardproperty_kind vcardproperty_value_kind_to_kind(vcardvalue_kind kind);
LIBICAL_VCARD_EXPORT const char *vcardproperty_kind_to_string(vcardproperty_kind kind);
LIBICAL_VCARD_EXPORT vcardproperty_kind vcardproperty_string_to_kind(const char *string);

/** Check validity of a specific vcardproperty_kind **/
LIBICAL_VCARD_EXPORT bool vcardproperty_kind_is_valid(const vcardproperty_kind kind);

LIBICAL_VCARD_EXPORT const char *vcardproperty_enum_to_string(int e);
LIBICAL_VCARD_EXPORT char *vcardproperty_enum_to_string_r(int e);
LIBICAL_VCARD_EXPORT int vcardproperty_kind_and_string_to_enum(const int kind, const char *str);

LIBICAL_VCARD_EXPORT bool vcardproperty_enum_belongs_to_property(vcardproperty_kind kind, int e);

/**
 * Normalizes (reorders and sorts the parameters) the specified @p vcardproperty.
 * @since 3.0
 */
LIBICAL_VCARD_EXPORT void vcardproperty_normalize(vcardproperty *prop);

LIBICAL_VCARD_EXPORT bool vcardproperty_is_structured(vcardproperty_kind pkind);
LIBICAL_VCARD_EXPORT bool vcardproperty_is_multivalued(vcardproperty_kind pkind);

LIBICAL_VCARD_EXPORT void vcardproperty_add_type_parameter(vcardproperty *prop,
                                                           vcardenumarray_element *type);

#endif /*VCARDPROPERTY_H */
