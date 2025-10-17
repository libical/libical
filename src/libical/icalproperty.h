/*======================================================================
 FILE: icalproperty.h
 CREATOR: eric 20 March 1999

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
======================================================================*/

#ifndef ICALPROPERTY_H
#define ICALPROPERTY_H

#include "libical_ical_export.h"
#include "icalderivedproperty.h" /* To get icalproperty_kind enumerations */
#include "icalpvl.h"

#include <stdarg.h> /* for va_... */

/** @file icalproperty.h */

LIBICAL_ICAL_EXPORT icalproperty *icalproperty_new(icalproperty_kind kind);

LIBICAL_ICAL_EXPORT icalproperty *icalproperty_new_impl(icalproperty_kind kind);

/** @brief Deeply clones an icalproperty.
 *
 * Returns a pointer to the memory for the newly cloned icalproperty.
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT icalproperty *icalproperty_clone(const icalproperty *old);

LIBICAL_ICAL_EXPORT icalproperty *icalproperty_new_from_string(const char *str);

LIBICAL_ICAL_EXPORT const char *icalproperty_as_ical_string(icalproperty *prop);

LIBICAL_ICAL_EXPORT const char *icalproperty_as_ical_string_r(icalproperty *prop);

LIBICAL_ICAL_EXPORT void icalproperty_free(icalproperty *prop);

LIBICAL_ICAL_EXPORT icalproperty_kind icalproperty_isa(icalproperty *property);

LIBICAL_ICAL_EXPORT bool icalproperty_isa_property(void *property);

LIBICAL_ICAL_EXPORT void icalproperty_add_parameters(struct icalproperty_impl *prop, va_list args);

LIBICAL_ICAL_EXPORT void icalproperty_add_parameter(icalproperty *prop, icalparameter *parameter);

LIBICAL_ICAL_EXPORT void icalproperty_set_parameter(icalproperty *prop, icalparameter *parameter);

LIBICAL_ICAL_EXPORT void icalproperty_set_parameter_from_string(icalproperty *prop,
                                                                const char *name,
                                                                const char *value);
LIBICAL_ICAL_EXPORT const char *icalproperty_get_parameter_as_string(icalproperty *prop,
                                                                     const char *name);

LIBICAL_ICAL_EXPORT const char *icalproperty_get_parameter_as_string_r(icalproperty *prop,
                                                                       const char *name);

/** @brief Removes all parameters with the specified kind.
 *
 *  @param prop   A valid icalproperty.
 *  @param kind   The kind to remove (ex. ICAL_TZID_PARAMETER)
 *
 *  See icalproperty_remove_parameter_by_name() and
 *  icalproperty_remove_parameter_by_ref() for alternate ways of
 *  removing parameters
 */
LIBICAL_ICAL_EXPORT void icalproperty_remove_parameter_by_kind(icalproperty *prop,
                                                               icalparameter_kind kind);

/** @brief Removes all parameters with the specified name.
 *
 *  @param prop   A valid icalproperty.
 *  @param name   The name of the parameter to remove
 *
 *  This function removes parameters with the given name.  The name
 *  corresponds to either a built-in name (TZID, etc.) or the name of
 *  an extended parameter (X-FOO)
 *
 *  See icalproperty_remove_parameter_by_kind() and
 *  icalproperty_remove_parameter_by_ref() for alternate ways of removing
 *  parameters
 */
LIBICAL_ICAL_EXPORT void icalproperty_remove_parameter_by_name(icalproperty *prop,
                                                               const char *name);

/** @brief Removes the specified parameter reference from the property.
 *
 *  @param prop   A valid icalproperty.
 *  @param param  A reference to a specific icalparameter.
 *
 *  This function removes the specified parameter reference from the
 *  property.
 */
LIBICAL_ICAL_EXPORT void icalproperty_remove_parameter_by_ref(icalproperty *prop,
                                                              icalparameter *param);

LIBICAL_ICAL_EXPORT int icalproperty_count_parameters(const icalproperty *prop);

/* Iterate through the parameters */
LIBICAL_ICAL_EXPORT icalparameter *icalproperty_get_first_parameter(icalproperty *prop,
                                                                    icalparameter_kind kind);
LIBICAL_ICAL_EXPORT icalparameter *icalproperty_get_next_parameter(icalproperty *prop,
                                                                   icalparameter_kind kind);
/* Access the value of the property */
LIBICAL_ICAL_EXPORT void icalproperty_set_value(icalproperty *prop, icalvalue *value);
LIBICAL_ICAL_EXPORT void icalproperty_set_value_from_string(icalproperty *prop, const char *str,
                                                            const char *type);

LIBICAL_ICAL_EXPORT icalvalue *icalproperty_get_value(const icalproperty *prop);
LIBICAL_ICAL_EXPORT const char *icalproperty_get_value_as_string(const icalproperty *prop);
LIBICAL_ICAL_EXPORT char *icalproperty_get_value_as_string_r(const icalproperty *prop);

/**
 * Sets the parent @p icalproperty for the specified @p icalvalue.
 */
LIBICAL_ICAL_EXPORT void icalvalue_set_parent(icalvalue *value, icalproperty *property);

/**
 * Returns the parent @p icalproperty for the specified @p icalvalue.
 * @since 3.0
 */
LIBICAL_ICAL_EXPORT icalproperty *icalvalue_get_parent(const icalvalue *value);

/* Deal with X properties */

LIBICAL_ICAL_EXPORT void icalproperty_set_x_name(icalproperty *prop, const char *name);
LIBICAL_ICAL_EXPORT const char *icalproperty_get_x_name(const icalproperty *prop);

/** Returns the name of the property -- the type name converted to a
 *  string, or the value of _get_x_name if the type is and X
 *  property
 */
LIBICAL_ICAL_EXPORT const char *icalproperty_get_property_name(const icalproperty *prop);
LIBICAL_ICAL_EXPORT char *icalproperty_get_property_name_r(const icalproperty *prop);

LIBICAL_ICAL_EXPORT icalvalue_kind icalparameter_value_to_value_kind(icalparameter_value value);

/**
 * Sets the parent @p icalproperty for the specified @p icalparameter.
 * @since 3.0
 */
LIBICAL_ICAL_EXPORT void icalparameter_set_parent(icalparameter *param, icalproperty *property);

/**
 * Returns the parent @p icalproperty for the specified @p icalparameter.
 * @since 3.0
 */
LIBICAL_ICAL_EXPORT icalproperty *icalparameter_get_parent(const icalparameter *param);

/* Convert kinds to string and get default value type */
LIBICAL_ICAL_EXPORT icalvalue_kind icalproperty_kind_to_value_kind(icalproperty_kind kind);
LIBICAL_ICAL_EXPORT icalproperty_kind icalproperty_value_kind_to_kind(icalvalue_kind kind);
LIBICAL_ICAL_EXPORT const char *icalproperty_kind_to_string(icalproperty_kind kind);
LIBICAL_ICAL_EXPORT icalproperty_kind icalproperty_string_to_kind(const char *string);

/** Check validity of a specific icalproperty_kind **/
LIBICAL_ICAL_EXPORT bool icalproperty_kind_is_valid(const icalproperty_kind kind);

LIBICAL_ICAL_EXPORT icalproperty_method icalproperty_string_to_method(const char *str);
LIBICAL_ICAL_EXPORT const char *icalproperty_method_to_string(icalproperty_method method);

LIBICAL_ICAL_EXPORT const char *icalproperty_enum_to_string(int e);
LIBICAL_ICAL_EXPORT char *icalproperty_enum_to_string_r(int e);
LIBICAL_ICAL_EXPORT int icalproperty_kind_and_string_to_enum(const int kind, const char *str);

LIBICAL_ICAL_EXPORT const char *icalproperty_status_to_string(icalproperty_status);
LIBICAL_ICAL_EXPORT icalproperty_status icalproperty_string_to_status(const char *string);

LIBICAL_ICAL_EXPORT const char *icalproperty_action_to_string(icalproperty_action);
LIBICAL_ICAL_EXPORT icalproperty_action icalproperty_string_to_action(const char *string);

LIBICAL_ICAL_EXPORT const char *icalproperty_transp_to_string(icalproperty_transp);
LIBICAL_ICAL_EXPORT icalproperty_transp icalproperty_string_to_transp(const char *string);

LIBICAL_ICAL_EXPORT const char *icalproperty_class_to_string(icalproperty_class);
LIBICAL_ICAL_EXPORT icalproperty_class icalproperty_string_to_class(const char *string);

LIBICAL_ICAL_EXPORT const char *icalproperty_participanttype_to_string(icalproperty_participanttype);
LIBICAL_ICAL_EXPORT icalproperty_participanttype icalproperty_string_to_participanttype(const char *string);

LIBICAL_ICAL_EXPORT const char *icalproperty_resourcetype_to_string(icalproperty_resourcetype);
LIBICAL_ICAL_EXPORT icalproperty_resourcetype icalproperty_string_to_resourcetype(const char *string);

LIBICAL_ICAL_EXPORT bool icalproperty_enum_belongs_to_property(icalproperty_kind kind, int e);

/**
 * Normalizes (reorders and sorts the parameters) the specified @p icalproperty.
 * @since 3.0
 */
LIBICAL_ICAL_EXPORT void icalproperty_normalize(icalproperty *prop);

/**
 * Sets if empty properties are permitted.
 *
 * Determines the library behavior whenever an empty property is encountered.
 * When not set (the default) empty properties are replaced with X-LIC-ERROR properties.
 * Otherwise, processing proceeds normally and the property value will be empty.
 *
 * @param enable If true, libical allows empty properties; otherwise empty properties
 *               are replaced by X-LIC-ERROR properties.
 *
 * Note that if icalerror_get_errors_are_fatal is also true a SIGABRT will be raised
 * whenever an empty property is encountered.
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT void icalproperty_set_allow_empty_properties(bool enable);

/**
 * Returns if empty properties are allowed; else are replaced with X-LIC-ERROR properties.
 *
 * @return true if empty properties are allowed; else returns false
 * @since 4.0
 *
 */
LIBICAL_ICAL_EXPORT bool icalproperty_get_allow_empty_properties(void);

/* This is exposed so that callers will not have to allocate and
   deallocate iterators. Pretend that you can't see it. */
typedef struct icalparamiter {
    icalparameter_kind kind;
    icalpvl_elem iter;
} icalparamiter;

LIBICAL_ICAL_EXPORT icalparamiter icalproperty_begin_parameter(icalproperty *property, icalparameter_kind kind);

LIBICAL_ICAL_EXPORT icalparameter *icalparamiter_next(icalparamiter *i);

LIBICAL_ICAL_EXPORT icalparameter *icalparamiter_deref(icalparamiter *i);

#endif /*ICALPROPERTY_H */
