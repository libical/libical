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

/**
 * @brief Represents a decoded, structured text value.
 *
 * vCard defines structured values for properties such as N
 * and ADR. These values consist of "components", where each
 * component may contain multiple values.
 *
 * To avoid confusion with existing libical terminology, the
 * vcardstructuredtype uses the term "field" to refer to a
 * "component". A vcardstructuredtype consists of zero or more
 * fields, each of which consists of zero or more values.
 * A field is represented by a vcardstrarray. A field of value
 * NULL is treated equally to an empty vcardstrarray.
 *
 * @par Ownership
 * A vcardstructuredtype is reference counted, but its fields
 * are not. Callers may read and alter the contents of fields,
 * but they must not free a field other than by calling
 * vcardstructured_set_field_at() with a NULL field value.
 */
typedef struct vcardstructuredtype_impl vcardstructuredtype;

/**
 * @brief Creates a new instance of vcardstructuredtype.
 * @param num_fields The initial number of fields.
 * @sa vcardstructuredtype_unref()
 *
 * @par Ownership
 * The returned value has refcount 1. It is the caller's responsibility
 * to call vcardstructuredtype_unref() after it's no longer used.
 */
LIBICAL_VCARD_EXPORT vcardstructuredtype *vcardstructured_new(size_t num_fields);

/**
 * @brief Creates a new instance of vcardstructuredtype from a string.
 * @param s The string containing a vCard representation of the structured value.
 * @sa vcardstructuredtype_unref()
 *
 * @par Ownership
 * The returned value has refcount 1. It is the caller's responsibility
 * to call vcardstructuredtype_unref() after it's no longer used.
 */
LIBICAL_VCARD_EXPORT vcardstructuredtype *vcardstructured_new_from_string(const char *s);

/**
 * @brief Clones a vcardstructuredtype.
 * @param st The object to clone.
 * @sa vcardstructuredtype_unref()
 *
 * @par Error handling
 * If @a st is `NULL` it sets ::icalerrno to ::ICAL_BADARG_ERROR.
 *
 * @par Ownership
 * The clone is a deep copy of @a st.
 * The returned value has refcount 1. It is the caller's responsibility
 * to call vcardstructuredtype_unref() after it's no longer used.
 */
LIBICAL_VCARD_EXPORT vcardstructuredtype *vcardstructured_clone(const vcardstructuredtype *st);

/**
 * @brief Returns the number of fields in a vcardstructuredtype.
 * @param st The structured type object.
 * @return The number of fields
 *
 * @par Error handling
 * If @a st is `NULL`, it sets ::icalerrno to ::ICAL_BADARG_ERROR and returns 0.
 */
LIBICAL_VCARD_EXPORT size_t vcardstructured_num_fields(const vcardstructuredtype *st);

/**
 * @brief Sets the number of fields in a vcardstructuredtype.
 * @param st The structured type object.
 * @param num_fields The new number of fields.
 *
 * If @a num_fields is smaller than the current number of fields,
 * the excess fields are freed.
 *
 * @par Error handling
 * If @a st is `NULL`, it sets ::icalerrno to ::ICAL_BADARG_ERROR.
 */
LIBICAL_VCARD_EXPORT void vcardstructured_set_num_fields(vcardstructuredtype *st, size_t num_fields);

/**
 * @brief Returns the field at the given position in a vcardstructuredtype.
 * @param st The structured type object.
 * @param position The zero-based index of the field to retrieve.
 * @return The vcardstrarray at @a position, or `NULL` on error.
 *
 * @par Error handling
 * If @a st is `NULL` or @a position is out of bounds,
 * it sets ::icalerrno to ::ICAL_BADARG_ERROR and returns `NULL`.
 */
LIBICAL_VCARD_EXPORT vcardstrarray *vcardstructured_field_at(const vcardstructuredtype *st, size_t position);

/**
 * @brief Sets the field at the given position in a vcardstructuredtype.
 * @param st The structured type object.
 * @param position The zero-based index of the field to set.
 * @param field The vcardstrarray to store at @a position.
 *
 * If @a position is beyond the current number of fields,
 * the number of fields is extended to accommodate it.
 * Any existing field at @a position is freed before being replaced.
 *
 * @par Error handling
 * If @a st is `NULL`, it sets ::icalerrno to ::ICAL_BADARG_ERROR.
 */
LIBICAL_VCARD_EXPORT void vcardstructured_set_field_at(vcardstructuredtype *st, size_t position, vcardstrarray *field);

/**
 * @brief Formats a vcardstructuredtype as a vCard property or parameter value.
 * @param st The structured type object to format.
 * @param is_param Whether the value is being formatted as a parameter value.
 * @return A newly allocated string representation, or `NULL` on error.
 *
 * @par Ownership
 * The caller is responsible for freeing the returned string.
 */
LIBICAL_VCARD_EXPORT char *vcardstructured_as_vcard_string_r(const vcardstructuredtype *st, bool is_param);

/**
 * @brief Increments the reference count of the vcardstructuredtype.
 * @param st The object to increase the reference count of
 * @sa vcardstructuredtype_unref()
 *
 * @par Error handling
 * If @a st is `NULL`, or the reference count is smaller than 0,
 * it sets ::icalerrno to ::ICAL_BADARG_ERROR.
 *
 * @par Ownership
 * By increasing the refcount of @a st, you are signaling that
 * you are using it, and it is the owner's responsibility to call
 * vcardstructuredtype_unref() after it's no longer used.
 */
LIBICAL_VCARD_EXPORT void vcardstructured_ref(vcardstructuredtype *st);

/**
 * @brief Decrements the reference count of the vcardstructuredtype.
 * @param st The object to decrease the reference count of
 * @sa vcardstructuredtype_ref()
 *
 * Decreases the reference count of @a st. If this was the
 * last user of the object, it is freed.
 *
 * @par Error handling
 * If @a s is `NULL`, or the reference count is smaller than 0,
 * it sets ::icalerrno to ::ICAL_BADARG_ERROR.
 *
 * @par Ownership
 * Calling this function releases the vcardstructuredtype back to the library,
 * and it must not be used afterwards.
 */
LIBICAL_VCARD_EXPORT void vcardstructured_unref(vcardstructuredtype *st);

#endif /* VCARDSTRUCTURED_H */
