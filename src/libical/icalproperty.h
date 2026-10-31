/*======================================================================
 FILE: icalproperty.h
 CREATOR: eric 20 March 1999

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
======================================================================*/

/**
 * @file icalproperty.h
 * @brief Defines the data structure representing iCalendar properties.
 */

#ifndef ICALPROPERTY_H
#define ICALPROPERTY_H

#include "libical_ical_export.h"
#include "icalderivedproperty.h" /* To get icalproperty_kind enumerations */

#include <stdarg.h> /* for va_... */

/// @cond PRIVATE
/* This is exposed so that callers will not have to allocate and
 *   deallocate iterators. Pretend that you can't see it. */
struct icalpvl_elem_t;
typedef struct icalparamiter {
    icalparameter_kind kind;
    struct icalpvl_elem_t *iter;
} icalparamiter;
/// @endcond

/**
 * Construct a new icalproperty of the specified icalproperty_kind.
 *
 * @param kind is the icalproperty_kind to use
 *
 * @return a pointer to the newly allocated icalproperty. The data structure
 * contains all null values and is essentially invalid.
 * Free the resulting memory with icalproperty_free.
 */
LIBICAL_ICAL_EXPORT icalproperty *icalproperty_new(icalproperty_kind kind);

/**
 * Deeply clones an icalproperty.
 *
 * @param old a pointer to the icalproperty to clone
 * @return a pointer to the memory for the newly cloned icalproperty.
 *
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT icalproperty *icalproperty_clone(const icalproperty *old);

/**
 * Construct and populate a new icalproperty from a char string.
 *
 * @param str a non-NULL pointer to the data used to populate the new icalproperty.
 *
 * @return a pointer to the newly allocated icalproperty. Free the resulting
 * memory with icalproperty_free.
 */
LIBICAL_ICAL_EXPORT icalproperty *icalproperty_new_from_string(const char *str);

/**
 * Stringify an icalproperty.
 *
 * @param prop a pointer to an icalproperty
 *
 * @return a pointer to a char string containing the string representation of @p prop.
 * NULL is returned is @p prop is invalid.
 *
 * @see icalproperty_as_ical_string_r
 */
LIBICAL_ICAL_EXPORT const char *icalproperty_as_ical_string(icalproperty *prop);

/**
 * Stringify an icalproperty.
 *
 * @param prop a pointer to an icalproperty
 *
 * @return a pointer to a char string containing the string representation of @p prop.
 * NULL is returned is @p prop is invalid.
 *
 * @see icalproperty_as_ical_string
 */
LIBICAL_ICAL_EXPORT char *icalproperty_as_ical_string_r(icalproperty *prop);

/**
 * Free the memory for an icalproperty.
 *
 * @param prop a pointer to a valid icalproperty
 */
LIBICAL_ICAL_EXPORT void icalproperty_free(icalproperty *prop);

/**
 * Determines the icalproperty_kind of the specified icalproperty.
 *
 * @param property a pointer to valid icalproperty.
 *
 * @return the icalproperty_kind of @p property; ICAL_NO_PROPERTY is
 * returned if @p property is NULL.
 */
LIBICAL_ICAL_EXPORT icalproperty_kind icalproperty_isa(const icalproperty *property);

/**
 * Determines if the specified pointer is an icalproperty pointer.
 *
 * Just looks if the ::id member strncmps to "prop".
 *
 * @param property a pointer, presumably to an icalproperty.
 *
 * @return true if the pointer looks like an icalproperty; false otherwise.
 */
LIBICAL_ICAL_EXPORT bool icalproperty_isa_property(void *property);

/**
 * Adds a list of parameter to the specified icalproperty.
 *
 * @param prop a pointer to a valid icalproperty.
 * @param args a va_list of pointers that presumably point to list of
 * valid icalparameters.
 *
 * If any item in @p va_list points to an illegal icalparameter the internal
 * library ::icalerrno is set to ::ICAL_BADARG_ERROR.
 *
 * Any item in @p va_list points to an icalvalue is ignored.
 */
LIBICAL_ICAL_EXPORT void icalproperty_add_parameters(icalproperty *prop, va_list args);

/**
 * Adds a parameter to the specified icalproperty.
 *
 * No error checking is performed by this function.
 *
 * @param prop a pointer to a valid icalproperty
 * @param parameter a pointer to a presumably valid icalparameter
 */
LIBICAL_ICAL_EXPORT void icalproperty_add_parameter(icalproperty *prop, icalparameter *parameter);

/**
 * Adds an icalparameter to an icalproperty.
 *
 * No error checking is performed by this function.
 *
 * @param prop a pointer to a valid icalproperty
 * @param parameter a pointer to a presumably valid icalparameter
 *
 * If @p parameter kind is already attached to @p prop then the old
 * parameter is removed in favor of @p parameter.
 */
LIBICAL_ICAL_EXPORT void icalproperty_set_parameter(icalproperty *prop, icalparameter *parameter);

/**
 * Sets a icalparameter from a string for the specified icalproperty.
 *
 * @param prop a pointer to a valid icalproperty
 * @param name a pointer to a char string containing the parameter name
 * @param value a pointer to a char string containing the parameter value
 *
 * If @p name refers to an invalid icalparameter_kind or @p value is illegal,
 * then the internal library ::icalerrno is set to ::ICAL_BADARG_ERROR.
 *
 * @see icalproperty_get_parameter_as_string
 */
LIBICAL_ICAL_EXPORT void icalproperty_set_parameter_from_string(icalproperty *prop,
                                                                const char *name,
                                                                const char *value);

/**
 * Gets the parameter value for a specified icalproperty parameter.
 *
 * @param prop a pointer to a valid icalproperty
 * @param name a a pointer to a char string containing the name of icalparameter to use
 *
 * @return a pointer to a char string containing the value for the specified
 * icalparameter name; NULL is returned is @p name refers to an invalid
 * icalproperty_kind.
 *
 * @see icalproperty_get_parameter_as_string_r
 */
LIBICAL_ICAL_EXPORT const char *icalproperty_get_parameter_as_string(icalproperty *prop,
                                                                     const char *name);

/**
 * Gets the parameter value for a specified icalproperty parameter.
 *
 * @param prop a pointer to a valid icalproperty
 * @param name a pointer to a char string containing the name of icalparameter to use
 *
 * @return a pointer to a char string containing the value for the specified
 * icalparameter name; NULL is returned is @p name refers to an invalid
 * icalproperty_kind.
 *
 * @see icalproperty_get_parameter_as_string
 */
LIBICAL_ICAL_EXPORT char *icalproperty_get_parameter_as_string_r(icalproperty *prop,
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

/**
 * Gets the number of parameter for a specified icalproperty.
 *
 * @param prop a pointer to a valid icalproperty
 *
 * @return the number of parameters attached to @p prop.
 * Returns -1 if @p prop is NULL.
 */
LIBICAL_ICAL_EXPORT int icalproperty_count_parameters(const icalproperty *prop);

/* Iterate through the parameters */

/**
 * From the head position, iterates the icalparameter list attached to the
 * specified icalproperty until the first icalparameter_kind is found.
 *
 * @param prop a pointer to a valid icalproperty
 * @param kind an icalparameter_kind to use for the search
 *
 * @return a pointer to the first icalparameter found of icalparameter_kind @p kind
;
 * NULL if no matches are found.
 */
LIBICAL_ICAL_EXPORT icalparameter *icalproperty_get_first_parameter(icalproperty *prop,
                                                                    icalparameter_kind kind);

/**
 * From the current position, iterates the icalparameter list attached to the
 * specified icalproperty until the next icalparameter_kind is found.
 *
 * @param prop a pointer to a valid icalproperty
 * @param kind an icalparameter_kind to use for the search
 *
 * @return a pointer to the next icalparameter found of icalparamter_kind @p kind;
 * NULL if no matches are found.
 */
LIBICAL_ICAL_EXPORT icalparameter *icalproperty_get_next_parameter(icalproperty *prop,
                                                                   icalparameter_kind kind);

/* Access the value of the property */

/**
 * Sets an icalvalue for the specified icalproperty.
 *
 * @param prop a pointer to a valid icalproperty
 * @param value a pointer to a valid icalvalue to use
 *
 * If @p prop already has an icalvalue it will be replaced.
 */
LIBICAL_ICAL_EXPORT void icalproperty_set_value(icalproperty *prop, icalvalue *value);

/**
 * Sets an icalvalue for the specified icalproperty from a char string..
 *
 * @param prop a pointer to a valid icalproperty
 * @param str a non-NULL pointer to char string to be converted to the icalvalue
 * @param type a non-NULL pointer to a char string to be converted to an icalvalue_kind
 *
 * If @p type string matches "NO" then the icalvalue_kind will be taken
 * from an existing icalvalue (if one exists) or will use the default
 * icalvalue_kind for the icalproperty.
 *
 * If @p prop already has an icalvalue it will be replaced.
 */
LIBICAL_ICAL_EXPORT void icalproperty_set_value_from_string(icalproperty *prop, const char *str,
                                                            const char *type);

/**
 * Gets the icalvalue for the specified icalproperty.
 *
 * @param prop a pointer to a valid icalproperty
 *
 * @return a pointer the @p prop icalvalue; or NULL if @p prop does not
 * have an attached icalvalue yet.
 */
LIBICAL_ICAL_EXPORT icalvalue *icalproperty_get_value(const icalproperty *prop);

/**
 * Gets the icalvalue for the specified icalproperty as a char string.
 *
 * @param prop a pointer to a valid icalproperty
 *
 * @return a pointer the string-converted @p prop icalvalue; or NULL
 * if @p prop does not have an attached icalvalue yet.
 *
 * @see icalproperty_get_value_as_string_r
 */
LIBICAL_ICAL_EXPORT const char *icalproperty_get_value_as_string(const icalproperty *prop);

/**
 * Gets the icalvalue for the specified icalproperty as a char string.
 *
 * @param prop a pointer to a valid icalproperty
 *
 * @return a pointer the string-converted @p prop icalvalue; or NULL
 * if @p prop does not have an attached icalvalue yet.
 *
 * @see icalproperty_get_value_as_string
 */
LIBICAL_ICAL_EXPORT char *icalproperty_get_value_as_string_r(const icalproperty *prop);

/**
 * Sets the parent @p icalproperty for the specified @p icalvalue.
 *
 * @param value a pointer to a valid icalvalue
 * @param property a pointer to a presumably valid icalproperty to use
 */
LIBICAL_ICAL_EXPORT void icalvalue_set_parent(icalvalue *value, icalproperty *property);

/**
 * Gets the parent icalproperty of the specified icalvalue.
 *
 * @param value a pointer to the icalvalue
 *
 * @return the parent @p icalproperty for the specified @p icalvalue.
 * @since 3.0
 */
LIBICAL_ICAL_EXPORT icalproperty *icalvalue_get_parent(const icalvalue *value);

/* Deal with X properties */

/**
 * Sets the X-NAME property of the specified icalproperty.
 *
 * @param prop a pointer to a valid icalproperty
 * @param name a pointer to a char string to use as the X-NAME
 */
LIBICAL_ICAL_EXPORT void icalproperty_set_x_name(icalproperty *prop, const char *name);

/**
 * Gets the X-NAME icalproperty of the specified icalproperty.
 *
 * @param prop a pointer to a valid icalproperty
 *
 * @return a pointer to a char string containing the X-NAME property.
 * Will be NULL if an X-NAME property has not been set yet.
 *
 * @since 3.0
 */
LIBICAL_ICAL_EXPORT const char *icalproperty_get_x_name(const icalproperty *prop);

/* Deal with IANA properties */

/**
 * Sets the IANA name for an icalproperty.
 *
 * @param prop a pointer to a valid icalproperty
 * @param name a pointer to char string containing the IANA name.
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT void icalproperty_set_iana_name(icalproperty *prop, const char *name);

/**
 * Gets the IANA name of the specified icalproperty.
 *
 * @param prop a pointer to a valid icalproperty
 *
 * @return a pointer to a char string containing the @p comp IANA name; will be NULL if the
 * name has yet to be specified.
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT const char *icalproperty_get_iana_name(const icalproperty *prop);

/**
 * Gets the name of the specified icalproperty.
 *
 * Returns the name of the property -- the type name converted to a
 * string, or the value of _get_x_name if the type is and X property.
 *
 * @param prop a pointer to the icalproperty
 *
 * @return a pointer to a string containing the icalproperty's name.
 *
 * @see icalproperty_get_property_name_r
 */
LIBICAL_ICAL_EXPORT const char *icalproperty_get_property_name(const icalproperty *prop);

/**
 * Gets the name of the specified icalproperty.
 *
 * Returns the name of the property -- the type name converted to a
 * string, or the value of _get_x_name if the type is and X property.
 *
 * @param prop a pointer to the icalproperty
 *
 * @return a pointer to a string containing the icalproperty's name.
 *
 * @see icalproperty_get_property_name
 */
LIBICAL_ICAL_EXPORT char *icalproperty_get_property_name_r(const icalproperty *prop);

/**
 * Converts an icalparameter_value to an icalvalue_kind.
 *
 * @param value the icalparameter_value to use
 *
 * @return the icalvalue_kind conversion from @p value; returns ICAL_NO_VALUE
 * if @p value is invalid.
 */
LIBICAL_ICAL_EXPORT icalvalue_kind icalparameter_value_to_value_kind(icalparameter_value value);

/**
 * Sets the parent icalproperty for the specified icalparameter.
 *
 * @param param a pointer to a valid icalparameter
 * @param property a pointer to a presumably valid icalproperty to use
 *
 * @since 3.0
 */
LIBICAL_ICAL_EXPORT void icalparameter_set_parent(icalparameter *param, icalproperty *property);

/**
 * Gets the parent icalproperty of an icalparameter.
 *
 * @param param a pointer to the icalparameter
 *
 * @return the parent @p icalproperty for the specified @p icalparameter.
 * @since 3.0
 */
LIBICAL_ICAL_EXPORT icalproperty *icalparameter_get_parent(const icalparameter *param);

/* Convert kinds to string and get default value type */

/**
 * Converts an icalproperty_kind to an icalvalue_kind.
 *
 * @param kind is the icalproperty_kind to use
 *
 * @return the icalvalue_kind from @p kind; returns ICAL_NO_VALUE if
 * @p kind is invalid.
 */
LIBICAL_ICAL_EXPORT icalvalue_kind icalproperty_kind_to_value_kind(icalproperty_kind kind);

/**
 * Converts an icalvalue_kind to an icalproperty_kind.
 *
 * @param kind is the icalvalue_kind to use
 *
 * @return the icalproperty_kind from @p kind; returns ICAL_NO_PROPERTY if
 * @p kind is invalid.
 */
LIBICAL_ICAL_EXPORT icalproperty_kind icalproperty_value_kind_to_kind(icalvalue_kind kind);

/**
 * Converts an icalproperty_kind to its string representation.
 *
 * @param kind is the icalproperty_kind to use
 *
 * @return a pointer to a char string containing the string representation
 * of the specified icalproperty_kind.
 */
LIBICAL_ICAL_EXPORT const char *icalproperty_kind_to_string(icalproperty_kind kind);

/**
 * Converts a char string into an icalproperty_kind.
 *
 * @param string a pointer to a char string containing the character representati
on
 * of an icalproperty_kind
 *
 * @return the icalproperty_kind associated with @p string.
 * ICAL_NO_PROPERTY is returned if @p string is invalid.
 */
LIBICAL_ICAL_EXPORT icalproperty_kind icalproperty_string_to_kind(const char *string);

/**
 * Check validity of a specified icalproperty_kind.
 *
 * @param kind is the icalproperty_kind
 *
 * @return true if the specified icalproperty_kind is valid; false otherwise.
 */
LIBICAL_ICAL_EXPORT bool icalproperty_kind_is_valid(const icalproperty_kind kind);

/**
 * Converts a char string into an icalproperty_method.
 *
 * @param str a pointer to a char string containing the character representati
on
 * of an icalproperty_method
 *
 * @return the icalproperty_method associated with @p string.
 * ICAL_METHOD_NONE is returned if @p str is invalid.
 */
LIBICAL_ICAL_EXPORT icalproperty_method icalproperty_string_to_method(const char *str);

/**
 * Converts an icalproperty_method to its string representation.
 *
 * @param method is the icalproperty_method to use
 *
 * @return a pointer to a char string containing the string representation
 * of the specified icalproperty_method.
 */
LIBICAL_ICAL_EXPORT const char *icalproperty_method_to_string(icalproperty_method method);

/**
 * Converts an integer representing some icalproperty type to its
 * string representation.
 *
 * @param e is an integer between ICALPROPERTY_FIRST_ENUM and ICALPROPERTY_LAST_ENUM, inclusive.
 *
 * @return a pointer to a char string containing the string representation
 * of the specified icalproperty type.
 *
 * @see icalproperty_enum_to_string_r
 */
LIBICAL_ICAL_EXPORT const char *icalproperty_enum_to_string(int e);

/**
 * Converts an integer representing some icalproperty type to its
 * string representation.
 *
 * @param e is an integer between ICALPROPERTY_FIRST_ENUM and ICALPROPERTY_LAST_ENUM, inclusive.
 *
 * @return a pointer to a char string containing the string representation
 * of the specified icalproperty type.
 *
 * @see icalproperty_enum_to_string
 */
LIBICAL_ICAL_EXPORT char *icalproperty_enum_to_string_r(int e);

/**
 * Converts an integer representation of an icalvalue_kind enum along
 * with a string representation of an icalproperty_kind to an integer
 * representation of an icalproperty enum.
 *
 * @param kind is a integer representation of an icalvalue_kind
 * @param str is a non-NULL pointer to a char string containing
 * the string representation of the icalproperty enum.
 *
 * @return an integer representation of an icalproperty_kind enum; returns
 * zero if @p kind is invalid.
 */
LIBICAL_ICAL_EXPORT int icalproperty_kind_and_string_to_enum(const int kind, const char *str);

/**
 * Converts an icalproperty_status to its string representation.
 *
 * @param status is the icalproperty_status to use
 *
 * @return a pointer to a char string containing the string representation
 * of the specified icalproperty_status.
 */
LIBICAL_ICAL_EXPORT const char *icalproperty_status_to_string(icalproperty_status status);

/**
 * Converts a char string into an icalproperty_status.
 *
 * @param string a pointer to a char string containing the character representation
 * of an icalproperty_status
 *
 * @return the icalproperty_status associated with @p string.
 * ICAL_STATUS_NONE is returned if @p string is invalid.
 */
LIBICAL_ICAL_EXPORT icalproperty_status icalproperty_string_to_status(const char *string);

/**
 * Converts an icalproperty_action to its string representation.
 *
 * @param action is the icalproperty_action to use
 *
 * @return a pointer to a char string containing the string representation
 * of the specified icalproperty_action.
 */
LIBICAL_ICAL_EXPORT const char *icalproperty_action_to_string(icalproperty_action action);

/**
 * Converts a char string into an icalproperty_action.
 *
 * @param string a pointer to a char string containing the character representation
 * of an icalproperty_action
 *
 * @return the icalproperty_action associated with @p string.
 * ICAL_ACTION_NONE is returned if @p string is invalid.
 */
LIBICAL_ICAL_EXPORT icalproperty_action icalproperty_string_to_action(const char *string);

/**
 * Converts an icalproperty_transp to its string representation.
 *
 * @param transp is the icalproperty_transp to use
 *
 * @return a pointer to a char string containing the string representation
 * of the specified icalproperty_transp.
 */
LIBICAL_ICAL_EXPORT const char *icalproperty_transp_to_string(icalproperty_transp transp);

/**
 * Converts a char string into an icalproperty_transp.
 *
 * @param string a pointer to a char string containing the character representation
 * of an icalproperty_transp
 *
 * @return the icalproperty_transp associated with @p string.
 * ICAL_TRANSP_NONE is returned if @p string is invalid.
 */
LIBICAL_ICAL_EXPORT icalproperty_transp icalproperty_string_to_transp(const char *string);

/**
 * Converts an icalproperty_class to its string representation.
 *
 * @param propclass is the icalproperty_class to use
 *
 * @return a pointer to a char string containing the string representation
 * of the specified icalproperty_class.
 */
LIBICAL_ICAL_EXPORT const char *icalproperty_class_to_string(icalproperty_class propclass);

/**
 * Converts a char string into an icalproperty_class.
 *
 * @param string a pointer to a char string containing the character representati
on
 * of an icalproperty_class_
 *
 * @return the icalproperty_class associated with @p string.
 * ICAL_CLASS_NONE is returned if @p string is invalid.
 */
LIBICAL_ICAL_EXPORT icalproperty_class icalproperty_string_to_class(const char *string);

/**
 * Converts an icalproperty_participanttype to its string representation.
 *
 * @param participanttype is the icalproperty_participanttype to use
 *
 * @return a pointer to a char string containing the string representation
 * of the specified icalproperty_participanttype.
 */
LIBICAL_ICAL_EXPORT const char *icalproperty_participanttype_to_string(icalproperty_participanttype participanttype);

/**
 * Converts a char string into an icalproperty_participanttype.
 *
 * @param string a pointer to a char string containing the character representation
 * of an icalproperty_participanttype
 *
 * @return the icalproperty_participanttype associated with @p string.
 * ICAL_PARTICIPANTTYPE_NONE is returned if @p string is invalid.
 */
LIBICAL_ICAL_EXPORT icalproperty_participanttype icalproperty_string_to_participanttype(const char *string);

/**
 * Converts an icalproperty_resourcetype to its string representation.
 *
 * @param resourcetype is the icalproperty_resourcetype to use
 *
 * @return a pointer to a char string containing the string representation
 * of the specified icalproperty_resourcetype.
 */
LIBICAL_ICAL_EXPORT const char *icalproperty_resourcetype_to_string(icalproperty_resourcetype resourcetype);

/**
 * Converts a char string into an icalproperty_resourcetype.
 *
 * @param string a pointer to a char string containing the character representation
 * of an icalproperty_resourcetype
 *
 * @return the icalproperty_resourcetype associated with @p string.
 * ICAL_RESOURCETYPE_NONE is returned if @p string is invalid.
 */
LIBICAL_ICAL_EXPORT icalproperty_resourcetype icalproperty_string_to_resourcetype(const char *string);

/**
 * Determines if a integer ia a valid enum for a specified icalproperty_kind.
 *
 * @param kind is the icalproperty_kind
 * @param e is a an integer to validate as an enum of type icalproperty_kind
 *
 * @return true if the specified integer is a valid enum for icalproperty_kind;
 * false otherwise.
 */
LIBICAL_ICAL_EXPORT bool icalproperty_enum_belongs_to_property(icalproperty_kind kind, int e);

/**
 * Normalizes (reorders and sorts the parameters) the specified icalproperty.
 *
 * @param prop a pointer a valid icalproperty
 *
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

/// @cond PRIVATE
LIBICAL_ICAL_EXPORT icalparamiter icalproperty_begin_parameter(icalproperty *property, icalparameter_kind kind);

LIBICAL_ICAL_EXPORT icalparameter *icalparamiter_next(icalparamiter *i);

LIBICAL_ICAL_EXPORT icalparameter *icalparamiter_deref(icalparamiter *i);
/// @endcond

#endif /*ICALPROPERTY_H */
