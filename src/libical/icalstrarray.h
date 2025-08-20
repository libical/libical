/*======================================================================
 FILE: icalstrarray.h

 CREATOR: Ken Murchison 24 Aug 2022

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)

 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 ======================================================================*/

#ifndef ICALSTRARRAY_H
#define ICALSTRARRAY_H

#include "libical_ical_export.h"
#include "icalarray.h"

#include <stdlib.h>

typedef icalarray icalstrarray;

/**
 * @brief Creates a new icalstrarray object.
 * @param increment_size How many slots to allocate on array expansion
 * @return The new icalstrarray object
 * @sa icalstrarray_free()
 *
 * Creates a new icalstrarray object. The parameter @a increment_size determines
 * how many slots to allocate when expanding the array. If zero, then the
 * default increment size specified when libical was built is chosen.
 *
 * @par Error handling
 * If there is an error while creating the object, it returns `NULL` and sets
 * ::icalerrno to ::ICAL_NEWFAILED_ERROR.
 *
 * @par Ownership
 * The returned icalstrarray object is owned by the caller of the function,
 * and needs to be released properly after it's no longer needed with
 * icalstrarray_free().
 *
 */
#define icalstrarray_new(increment_size) \
    (icalarray_new(sizeof(char *), increment_size))

/**
 * @brief Accesses a string stored in the array.
 * @param array The array object in which the string is stored
 * @param position The slot position of the string in the array
 * @return A pointer to the string
 *
 * Accesses a string by returning a pointer to it, given an @a array and a
 * valid slot @a position.
 *
 * @par Error handling
 * If @a array is `NULL` or @a position is not set in the array, then the
 * return value is `NULL`.
 *
 * @par Ownership
 * The string is owned by the icalstrarray, it must not be freed by
 * the user.
 *
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT const char *icalstrarray_element_at(icalstrarray *array, size_t position);

/**
 * @brief Indicates the count of strings stored in the array.
 * @param array The array object in which the string is stored
 * @return the count of strings
 *
 * Returns the count of strings stored in the @a array.
 *
 * @par Error handling
 * If @a array is `NULL` then the return value is zero.
 *
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT size_t icalstrarray_size(const icalstrarray *array);

/**
 * @brief Finds a string in the array.
 * @param array The array object in which to search for the string
 * @param needle The string to search for
 * @return the slot position of the string in the array, or icalstrarray_size()
 *
 * Looks for string @a needle in the @a array, comparing strings for
 * byte-equality. The position of the first occurrence is returned if
 * the string is found. If the string is not found, then the count of
 * elements in the array is returned.
 *
 * @par Error handling
 * If @a array or @a needle is `NULL`, then the return value is the same
 * as if the value is not found.
 *
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT size_t icalstrarray_find(icalstrarray *array,
                                             const char *needle);

/**
 * @brief Appends a string to the array.
 * @param array The array object to which the string append to
 * @param elem The string to append
 *
 * Appends string @elem to @a array, not checking for duplicates.
 * Use icalstrarray_add() to only add strings that are not present
 * already in the array.
 *
 * @par Error handling
 * If @a array or @a elem is `NULL` then this function is a noop.
 *
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT void icalstrarray_append(icalstrarray *array,
                                             const char *elem);

/**
 * @brief Appends a string to the array, omitting duplicates.
 * @param array The array object to which the string append to
 * @param elem The string to append
 *
 * Appends string @elem to @a array unless a byte-equal string
 * already is present in the array.
 *
 * @par Error handling
 * If @a array or @a elem is `NULL` then this function is a noop.
 *
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT void icalstrarray_add(icalstrarray *array,
                                          const char *add);

/**
 * @brief Removes the string at an array position.
 * @param array The array object from which to remove the string
 * @param positione The array position of the string to remove
 *
 * Removes the string at @a position in the @a array.
 *
 * @par Error handling
 * If @a array is `NULL` or @a position is higher or equal to the
 * count of strings, this function is a noop.
 *
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT void icalstrarray_remove_element_at(icalstrarray *array,
                                                        size_t position);

/**
 * @brief Removes all occurrences of a string.
 * @param array The array object from which to remove the string
 * @param del The string to remove from the array
 *
 * Removes all occurrences of the string @a del from @a array.
 *
 * @par Error handling
 * If @a array or @a del is `NULL`, then this function is a noop.
 *
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT void icalstrarray_remove(icalstrarray *array,
                                             const char *del);

/**
 * @brief Frees this array's memory and all its elements.
 * @param array The array object to free
 *
 * Frees the array.
 *
 * @par Error handling
 * If @a array is `NULL`, this is a noop.
 *
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT void icalstrarray_free(icalstrarray *array);

/**
 * @brief Sorts the strings in the array in ascending order.
 * @param array The array object to sort.
 *
 * Sorts the array such that all string elements occur in
 * ascending order, comparing the strings as bytes.
 *
 * @par Error handling
 * If @a array is `NULL`, this is a noop.
 *
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT void icalstrarray_sort(icalstrarray *array);

/**
 * @brief Clones the array and all its elements.
 * @param array The array object to clone.
 * @return the cloned array
 *
 * Creates an independent copy of the @a array and all its string
 * elements.
 *
 * @par Error handling
 * If @a array is `NULL` then the return value is `NULL`.
 *
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT icalstrarray *icalstrarray_clone(icalstrarray *array);

#endif /* ICALSTRARRAY_H */
