/*======================================================================
 FILE: vcardparameter.h
 CREATOR: Ken Murchison 24 Aug 2022 <murch@fastmailteam.com>

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 ======================================================================*/

#ifndef VCARDPARAMETER_H
#define VCARDPARAMETER_H

#include "libical_vcard_export.h"
#include "vcardderivedparameter.h"
#include "vcardderivedvalue.h"

/* Declared in vcardderivedparameter.h */
/*typedef struct vcardparameter_impl vcardparameter;*/

/**
 * @brief Creates new vcardparameter object.
 * @param kind The kind of vcardparameter to create.
 * @return An vcardparameter with the given kind.
 *
 * @par Error handling
 * If there was an internal error regarding
 * memory allocation, it returns `NULL` and sets
 * ::icalerrno to ::ICAL_NEWFAILED_ERROR.
 *
 * @par Ownership
 * Objects created by this method are owned by the caller and
 * must be released with the vcardparameter_free() method.
 *
 * @par Usage
 * ```c
 * // create new parameter
 * vcardparameter *parameter = vcardparameter_new();
 *
 * if(parameter) {
 *     // use parameter ...
 * }
 *
 * // release parameter
 * vcardparameter_free(parameter);
 * ```
 */
LIBICAL_VCARD_EXPORT vcardparameter *vcardparameter_new(vcardparameter_kind kind);

/**
 * @brief Creates new vcardparameter as a clone of the given one.
 * @param old The existing, non-`NULL` parameter to clone.
 * @return An vcardparameter that is a clone of the given one.
 *
 * @par Error handling
 * If @a old is `NULL`, it returns `NULL` and sets ::icalerrno to ::ICAL_BADARG_ERROR.
 * If there was an internal error cloning the data, it returns `NULL`
 * without reporting any error in ::icalerrno.
 *
 * @par Ownership
 * Objects created by this method are owned by the caller and
 * must be released with the vcardparameter_free() method.
 *
 * @par Usage
 * ```x
 * // create an vcardparameter
 * vcardparameter *param = vcardparameter_new_from_string("ROLE=CHAIR");
 *
 * // clone the parameter
 * vcardparameter *clone = vcardparameter_clone(param);
 *
 * if(clone) {
 *     // use clone ...
 * }
 *
 * // release parameters
 * vcardparameter_free(param);
 * vcardparameter_free(clone);
 * ```
 * @since 4.0
 */
LIBICAL_VCARD_EXPORT vcardparameter *vcardparameter_clone(const vcardparameter *old);

/**
 * @brief Creates new vcardparameter object from string
 * @param str The string from which to create the vcardparameter, in the form `"PARAMNAME=VALUE"`
 * @return An vcardparameter that corresponds to the given string.
 *
 * @par Error handling
 * If there was an internal error copying data, it returns `NULL` and sets
 * ::icalerrno to ::ICAL_NEWFAILED_ERROR. If @a str was `NULL`, it returns
 * `NULL` and sets ::icalerrno to ::ICAL_BADARG_ERROR. If @a str was malformed,
 * it returns `NULL` and sets ::icalerrno to ::ICAL_MALFORMEDDATA_ERROR.
 *
 * @par Ownership
 * Objects created by this method are owned by the caller and
 * must be released with the vcardparameter_free() method.
 *
 * @par Usage
 * ```c
 * vcardparameter *param = vcardparameter_new_from_string("ROLE=CHAIR");
 *
 * if(param) {
 *     // use param ...
 * }
 *
 * vcardparameter_free(param);
 * ```
 */
LIBICAL_VCARD_EXPORT vcardparameter *vcardparameter_new_from_string(const char *str);

/**
 * @brief Creates new vcardparameter of a given @a kind with a given @a value
 * @param kind The kind of vcardparameter to create
 * @param value The value of the parameter
 * @return An vcardparameter with the given kind and value.
 *
 * @par Error handling
 * If value is `NULL`, it returns `NULL` and sets ::icalerrno to ::ICAL_BADARG_ERROR.
 *
 * @par Ownership
 * Objects created by this method are owned by the caller and
 * must be released with the vcardparameter_free() method.
 *
 * @par Example
 * ```c
 * // create new parameter
 * vcardparameter *param;
 * param = vcardparameter_new_from_value_string(ICAL_ROLE_PARAMETER, "CHAIR");
 *
 * // check parameter
 * assert(0 == strcmp(vcardparameter_get_iana_name(param), "ROLE"));
 * assert(0 == strcmp(vcardparameter_get_iana_value(param), "CHAIR"));
 *
 * // release memory
 * vcardparameter_free(param);
 * ```
 */
LIBICAL_VCARD_EXPORT vcardparameter *vcardparameter_new_from_value_string(vcardparameter_kind kind,
                                                                          const char *value);

LIBICAL_VCARD_EXPORT void vcardparameter_set_value_from_string(vcardparameter *param,
                                                               const char *val);
LIBICAL_VCARD_EXPORT void vcardparameter_add_value_from_string(vcardparameter *param,
                                                               const char *val);

/**
 * @brief Frees an vcardparameter object.
 * @param parameter The vcardparameter to free
 *
 * This method needs to be used on all parameter objects returned
 * from any of the `_new()` methods including vcardparameter_new(),
 * vcardparameter_new_from_string() and vcardparameter_new_from_value_string()
 * and on cloned parameter objects returned by vcardparameter_clone()
 * when these object are not needed anymore and to be released.
 *
 * @par Usage
 * ```c
 * vcardparameter *param = vcardparameter_new();
 *
 * if(param) {
 *     // use param...
 * }
 *
 * // after use, release it
 * vcardparameter_free(param);
 * ```
 */
LIBICAL_VCARD_EXPORT void vcardparameter_free(vcardparameter *parameter);

/**
 * @brief Converts vcardparameter into a string representation
 * @param parameter The vcardparameter to convert
 * @return A string representing the parameter according to RFC5445/RFC6868.
 * @sa vcardparameter_as_ical_string_r()
 *
 * @par Error handling
 * If there is any error, the method returns `NULL`. Furthermore,
 * if @a parameter is `NULL`, it also sets ::icalerrno to ::ICAL_BADARG_ERROR.
 * If it doesn't recognize the kind of the parameter, it sets ::icalerrno
 * it ::ICAL_BADARG_ERROR. If the parameter is otherwise malformed, it
 * sets ::icalerrno to ::ICAL_MALFORMEDDATA_ERROR.
 *
 * @par Ownership
 * Strings returned by this method are owned by libical, they must
 * not be freed and they may be reclaimed with the next call into
 * the library. A version of this function, which returns strings
 * that are not owned by libical, is vcardparameter_as_ical_string_r().
 *
 * @par Usage
 * ```c
 * vcardparameter *param = vcardparameter_new_from_string("ROLE=CHAIR");
 *
 * if(param) {
 *     printf("%s\n", vcardparameter_as_ical_string(param));
 * }
 *
 * vcardparameter_free(param);
 * ```
 */
LIBICAL_VCARD_EXPORT char *vcardparameter_as_vcard_string(vcardparameter *parameter);

/**
 * @brief Converts vcardparameter into an string representation according to RFC5445/RFC6868.
 * @param parameter The vcardparameter to convert
 * @return A string representing the parameter
 * @sa vcardparameter_as_ical_string()
 *
 * @par Error handling
 * If there is any error, the method returns `NULL`. Furthermore,
 * if parameter is `NULL`, it also sets ::icalerrno to ::ICAL_BADARG_ERROR.
 * If it doesn't recognize the kind of the parameter, it sets ::icalerrno
 * to ::ICAL_BADARG_ERROR. If the parameter is otherwise malformed, it
 * sets ::icalerrno to ::ICAL_MALFORMEDDATA_ERROR.
 *
 * @par Ownership
 * Strings returned by this method are owned by the caller, thus they need
 * to be manually `icalmemory_free_buffer()`d after use.
 * A version of this function which returns strings that do not
 * need to be freed manually is vcardparameter_as_ical_string().
 *
 * @par Usage
 * ```c
 * vcardparameter *param = vcardparameter_new_from_string("ROLE=CHAIR");
 *
 * if(param) {
 *     char *str = vcardparameter_as_ical_string(param);
 *     printf("%s\n", str);
 *     icalmemory_free_buffer(str);
 * }
 *
 * vcardparameter_free(param);
 * ```
 */
LIBICAL_VCARD_EXPORT char *vcardparameter_as_vcard_string_r(vcardparameter *parameter);

/**
 * Returns the vcardparameter_kind of parameter.
 *
 * @param parameter The vcardparameter whose kind to determine
 * @return The vcardparameter_kind of the parameter
 *
 * @b Error handling
 * Returns ICAL_NO_PARAMETER when passed NULL.
 *
 * @b Usage
 * @code
 * // create parameter
 * vcardparameter *param = vcardparameter_new_from_string("ROLE=CHAIR");
 *
 * // check what type of parameter this is
 * assert(vcardparameter_isa(param) == ICAL_ROLE_PARAMETER);
 *
 * // release memory
 * vcardparameter_free(param);
 * @endcode
 */
LIBICAL_VCARD_EXPORT vcardparameter_kind vcardparameter_isa(const vcardparameter *parameter);

/**
 * Determines if the given param is an vcardparameter
 * @param param The libical-originated object to check
 * @return true if the object is an vcardparameter, false otherwise.
 * @note This function expects to be given an object originating from
 *  libical - if this function is passed anything that is not from
 *  libical, its behavior is undefined.
 *
 * @b Error handling
 * When given a `NULL` object, it returns 0.
 *
 * @b Usage
 * ```c
 * // create parameter
 * vcardparameter *param = vcardparameter_new_from_string("ROLE=CHAIR");
 *
 * // check if it's a parameter
 * assert(vcardparameter_isa_parameter(param));
 *
 * // release memory
 * vcardparameter_free(param);
 * ```
 */
LIBICAL_VCARD_EXPORT bool vcardparameter_isa_parameter(void *param);

/* Access the name of an X parameter */

/**
 * @brief Sets the X-name of @a param to @a v
 * @param param The vcardparameter to change
 * @param v The X-name to set @a param to
 * @sa vcardparameter_get_xname()
 *
 * @par Error handling
 * If either @a param or @a v are `NULL`, it sets ::icalerrno to ICAL_BARARG_ERROR.
 * If there is an error acquiring memory, it sets `errno` to `ENOMEM`.
 *
 * @par Ownership
 * The passed string @a v stays in the ownership of the caller - libical
 * creates a copy of it.
 *
 * @par Usage
 * ```c
 * // creates new parameter
 * vcardparameter *param = vcardparameter_new();
 *
 * // sets xname
 * vcardparameter_set_xname(param, "X-TEST");
 *
 * // compare xname
 * assert(0 == strcmp(vcardparameter_get_xname(param), "X-TEST"));
 *
 * vcardparameter_free(param);
 * ```
 */
LIBICAL_VCARD_EXPORT void vcardparameter_set_xname(vcardparameter *param, const char *v);

/**
 * @brief Returns the X-name of @a param
 * @param param The vcardparameter whose X-name is to be returned
 * @return A string representing the X-name of @a param
 * @sa vcardparameter_set_xname()
 *
 * @par Error handling
 * Returns `NULL` and sets ::icalerrno to ::ICAL_BADARG_ERROR when a `NULL`
 * is passed instead of an vcardparameter.
 *
 * @par Ownership
 * The string that is returned stays owned by libical and must not
 * be freed by the caller.
 *
 * @par Usage
 * ```c
 * // creates new parameter
 * vcardparameter *param = vcardparameter_new();
 *
 * // sets xname
 * vcardparameter_set_xname(param, "X-TEST");
 *
 * // compare xname
 * assert(0 == strcmp(vcardparameter_get_xname(param), "X-TEST"));
 *
 * vcardparameter_free(param);
 * ```
 */
LIBICAL_VCARD_EXPORT const char *vcardparameter_get_xname(const vcardparameter *param);

/**
 * @brief Sets the X-value of @a param to @a v
 * @param param The vcardparameter to change
 * @param v The X-value to set @a param to
 * @sa vcardparameter_get_xvalue()
 *
 * @par Error handling
 * If either @a param or @a v are `NULL`, it sets ::icalerrno to ICAL_BARARG_ERROR.
 * If there is an error acquiring memory, it sets `errno` to `ENOMEM`.
 *
 * @par Ownership
 * The passed string @a v stays in the ownership of the caller - libical
 * creates a copy of it.
 *
 * @par Usage
 * ```c
 * // create new parameter
 * vcardparameter *param = vcardparameter_new_from_string("X-TEST=FAIL");
 *
 * // set test to success
 * vcardparameter_set_xvalue(param, "SUCCESS");
 *
 * // check that it worked
 * assert(0 == strcmp(vcardparameter_get_xvalue(param), "SUCCESS"));
 *
 * // release memory
 * vcardparameter_free(param);
 * ```
 */
LIBICAL_VCARD_EXPORT void vcardparameter_set_xvalue(vcardparameter *param, const char *v);

/**
 * @brief Returns the X-value of @a param
 * @param param The vcardparameter whose X-value is to be returned
 * @return A string representing the X-value of @a param
 * @sa vcardparameter_set_xvalue()
 *
 * @par Error handling
 * Returns `NULL` and sets ::icalerrno to ::ICAL_BADARG_ERROR when a `NULL`
 * is passed instead of an vcardparameter.
 *
 * @par Ownership
 * The string that is returned stays owned by libical and must not
 * be freed by the caller.
 *
 * @par Usage
 * ```c
 * // create new parameter
 * vcardparameter *param = vcardparameter_new_from_string("X-TEST=FAIL");
 *
 * // set test to success
 * vcardparameter_set_xvalue(param, "SUCCESS");
 *
 * // check that it worked
 * assert(0 == strcmp(vcardparameter_get_xvalue(param), "SUCCESS"));
 *
 * // release memory
 * vcardparameter_free(param);
 * ```
 */
LIBICAL_VCARD_EXPORT const char *vcardparameter_get_xvalue(const vcardparameter *param);

/* Access the name of an IANA parameter */

/**
 * @brief Sets the IANA name of @a param to @a v
 * @param param The vcardparameter to change
 * @param v The IANA name to set @a param to
 * @sa vcardparameter_get_iana_name()
 *
 * @par Error handling
 * If either @a param or @a v are `NULL`, it sets :calerrno to ICAL_BARARG_ERROR.
 * If there is an error acquiring memory, it sets `errno` to `ENOMEM`.
 *
 * @par Ownership
 * The passed string @a v stays in the ownership of the caller - libical
 * creates a copy of it.
 *
 * @par Usage
 * ```c
 * // creates new parameter
 * vcardparameter *param = vcardparameter_new();
 *
 * // sets iana name
 * vcardparameter_set_iana_name(param, "ROLE");
 *
 * // compare iana name
 * assert(0 == strcmp(vcardparameter_get_iana_name(param), "X-TEST"));
 *
 * vcardparameter_free(param);
 * ```
 */
LIBICAL_VCARD_EXPORT void vcardparameter_set_iana_name(vcardparameter *param, const char *v);

/**
 * @brief Returns the IANA name of @a param
 * @param param The vcardparameter whose IANA name is to be returned
 * @return A string representing the IANA name of @a param
 * @sa vcardparameter_set_iana_name()
 *
 * @par Error handling
 * Returns `NULL` and sets ::icalerrno to ::ICAL_BADARG_ERROR when a `NULL`
 * is passed instead of an vcardparameter.
 *
 * @par Ownership
 * The string that is returned stays owned by libical and must not
 * be freed by the caller.
 *
 * @par Usage
 * ```c
 * // creates new parameter
 * vcardparameter *param = vcardparameter_new();
 *
 * // sets iana name
 * vcardparameter_set_iana_name(param, "X-TEST");
 *
 * // compare iana name
 * assert(0 == strcmp(vcardparameter_get_iana_name(param), "X-TEST"));
 *
 * vcardparameter_free(param);
 * ```
 */
LIBICAL_VCARD_EXPORT const char *vcardparameter_get_iana_name(const vcardparameter *param);

/**
 * @brief Sets the IANA value of @a param to @a v
 * @param param The vcardparameter to change
 * @param v The IANA value to set @a param to
 * @sa vcardparameter_get_iana_value()
 *
 * @par Error handling
 * If either @a param or @a v are `NULL`, it sets ::icalerrno to ICAL_BARARG_ERROR.
 * If there is an error acquiring memory, it sets `errno` to `ENOMEM`.
 *
 * @par Ownership
 * The passed string @a v stays in the ownership of the caller - libical
 * creates a copy of it.
 *
 * @par Usage
 * ```c
 * // create new parameter
 * vcardparameter *param = vcardparameter_new_from_string("ROLE=ATTENDEE");
 *
 * // set role to chair
 * vcardparameter_set_iana_value(param, "CHAIR");
 *
 * // check that it worked
 * assert(0 == strcmp(vcardparameter_get_iana_value(param), "SUCCESS"));
 *
 * // release memory
 * vcardparameter_free(param);
 * ```
 */
LIBICAL_VCARD_EXPORT void vcardparameter_set_iana_value(vcardparameter *param, const char *v);

/**
 * @brief Returns the IANA value of @a param
 * @param param The vcardparameter whose value is to be returned
 * @return A string representing the value of @a param
 * @sa vcardparameter_set_iana_value()
 *
 * @par Error handling
 * Returns `NULL` and sets ::icalerrno to ::ICAL_BADARG_ERROR when a `NULL`
 * is passed instead of an vcardparameter.
 *
 * @par Ownership
 * The string that is returned stays owned by libical and must not
 * be freed by the caller.
 *
 * @par Usage
 * ```c
 * // create new parameter
 * vcardparameter *param = vcardparameter_new_from_string("ROLE=ATTENDEE");
 *
 * // set role to chair
 * vcardparameter_set_iana_value(param, "CHAIR");
 *
 * // check that it worked
 * assert(0 == strcmp(vcardparameter_get_iana_value(param), "SUCCESS"));
 *
 * // release memory
 * vcardparameter_free(param);
 * ```
 */
LIBICAL_VCARD_EXPORT const char *vcardparameter_get_iana_value(const vcardparameter *param);

/**
 * @brief Determines if two parameters have the same name
 * @param param1 First parameter to compare
 * @param param2 Second parameter to compare
 * @return true if they have the same name, false otherwise.
 *
 * @par Error handling
 * If either of @a param1 or @a param2 are `NULL`, it returns 0 and sets
 * ::icalerrno to ::ICAL_BADARG_ERROR.
 *
 * @par Ownership
 * Does not take ownership of either vcardparameter.
 *
 * @par Example
 * ```c
 * // create two parameters
 * vcardparameter *param1 = vcardparameter_new_from_string("ROLE=CHAIR");
 * vcardparameter *param2 = vcardparameter_new_from_string("EMAIL=mailto@example.com");
 *
 * // compare parameter names for equality
 * assert(vcardparameter_has_same_name(param1, param2) == 0);
 *
 * // release memory
 * vcardparameter_free(param1);
 * vcardparameter_free(param2);
 * ```
 */
LIBICAL_VCARD_EXPORT bool vcardparameter_has_same_name(const vcardparameter *param1, const vcardparameter *param2);

/* Convert enumerations */

/**
 * @brief Returns a string representing the given vcardparameter_kind
 * @param kind The vcardparameter_kind
 * @return A string representing @a kind
 *
 * @par Error handling
 * When passed a non-existing vcardparameter_kind, it returns `NULL`.
 *
 * @par Ownership
 * The string that is returned by this function is owned by libical and
 * must not be freed by the caller.
 *
 * @par Usage
 * ```c
 * assert(0 == strcmp(vcardparameter_kind_to_string(ICAL_ROLE_PARAMETER), "ROLE"));
 * assert(0 == strcmp(vcardparameter_kind_to_string(ICAL_EMAIL_PARAMETER), "EMAIL));
 * assert(0 == strcmp(vcardparameter_kind_to_string(ICAL_ID_PARAMETER), "ID"));
 * ```
 */
LIBICAL_VCARD_EXPORT const char *vcardparameter_kind_to_string(vcardparameter_kind kind);

/**
 * @brief Returns the vcardparameter_kind for a given string
 * @param string A string describing an vcardparameter_kind
 * @return An vcardparameter_kind
 *
 * @par Error handling
 * Returns ICAL_NO_PARAMETER if @a string is `NULL`.
 * If it can't find the parameter, depending on
 * the ical_get_unknown_token_handling_setting(), it returns either
 * ICAL_NO_PARAMETER or ICAL_IANA_PARAMETER.
 *
 * @par Ownership
 * Does not take ownership of @a string.
 *
 * @par Usage
 * ```c
 * assert(vcardparameter_string_to_kind("ROLE")  == ICAL_ROLE_PARAMETER);
 * assert(vcardparameter_string_to_kind("EMAIL") == ICAL_EMAIL_PARAMETER);
 * assert(vcardparameter_string_to_kind("ID")    == ICAL_ID_PARAMETER);
 * ```
 */
LIBICAL_VCARD_EXPORT vcardparameter_kind vcardparameter_string_to_kind(const char *string);

/**
 * @brief Checks the validity of a vcardparameter_kind
 * @param kind The vcardparameter_kind
 * @return true if @a kind is valid, false otherwise
 *
 * @par Usage
 * ```c
 * assert(vcardparameter_kind_is_valid(ICAL_ROLE_PARAMETER));
 * ```
 * @since 3.0.4
 */
LIBICAL_VCARD_EXPORT bool vcardparameter_kind_is_valid(const vcardparameter_kind kind);

LIBICAL_VCARD_EXPORT vcardvalue_kind vcardparameter_kind_value_kind(const vcardparameter_kind kind,
                                                                    int *is_multivalued);

LIBICAL_VCARD_EXPORT bool vcardparameter_is_multivalued(const vcardparameter *param);

LIBICAL_VCARD_EXPORT bool vcardparameter_is_structured(const vcardparameter *param);

#endif
