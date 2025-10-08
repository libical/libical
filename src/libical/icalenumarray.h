/*======================================================================
 FILE: icalenumarray.h
 CREATOR: Ken Murchison 24 Aug 2022 <murch@fastmailteam.com>

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 ======================================================================*/

#ifndef ICALENUMARRAY_H
#define ICALENUMARRAY_H

#include "libical_ical_export.h"
#include "icalarray.h"

#include <stdlib.h>

typedef icalarray icalenumarray;

typedef struct {
    int val;
    const char *xvalue;
} icalenumarray_element;

/**
 * @brief Creates a new icalenumarray object.
 * @param increment_size How many slots to allocate on array expansion
 * @return The new icalenumarray object
 * @sa icalenumarray_free()
 *
 * Creates a new icalenumarray object. The parameter @a increment_size determines
 * how many slots to allocate when expanding the array. If zero, then the
 * default increment size specified when libical was built is chosen.
 *
 * @par Error handling
 * If there is an error while creating the object, it returns `NULL` and sets
 * ::icalerrno to ::ICAL_NEWFAILED_ERROR.
 *
 * @par Ownership
 * The returned icalenumarray object is owned by the caller of the function,
 * and needs to be released properly after it's no longer needed with
 * icalenumarray_free().
 *
 */
#define icalenumarray_new(increment_size) \
    icalarray_new(sizeof(icalenumarray_element), increment_size)

/**
 * @brief Accesses an element stored in the array.
 * @param array The array object in which the element is stored
 * @param position The slot position of the element in the array
 * @return A pointer to the element
 *
 * Accesses an element by returning a pointer to it, given an @a array and a
 * valid slot @a position.
 *
 * @par Error handling
 * If @a array is `NULL` or @a position is not set in the array, then the
 * return value is `NULL`.
 *
 * @par Ownership
 * The element is owned by the icalenumarray, it must not be freed by
 * the user.
 *
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT const icalenumarray_element *icalenumarray_element_at(icalenumarray *array, size_t position);

/**
 * @brief Indicates the count of elements stored in the array.
 * @param array The array object in which the element is stored
 * @return the count of elements
 *
 * Returns the count of elements stored in the @a array.
 *
 * @par Error handling
 * If @a array is `NULL` then the return value is zero.
 *
 * @since 4.0
 *
 */
LIBICAL_ICAL_EXPORT size_t icalenumarray_size(const icalenumarray *array);

/**
 * @brief Finds an element in the array.
 * @param array The array object in which to search for the element
 * @param needle The element to search for
 * @return the slot position of the element in the array, or icalenumarray_size()
 *
 * Looks for element @a needle in the @a array, comparing the numeric value for
 * equality and the optional xvalue for byte-equality. The position of the first
 * occurrence is returned if the element is found. If the element is not found,
 * then the count of elements in the array is returned.
 *
 * @par Error handling
 * If @a array or @a needle is `NULL`, then the return value is the same
 * as if the value is not found.
 *
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT size_t icalenumarray_find(icalenumarray *array,
                                              const icalenumarray_element *needle);

/**
 * @brief Appends an element to the array.
 * @param array The array object to which the element append to
 * @param elem The element to append
 *
 * Appends element @p elem to @a array, not checking for duplicates.
 * Use icalenumarray_add() to only add elements that are not present
 * already in the array.
 *
 * @par Error handling
 * If @a array or @a elem is `NULL` then this function is a noop.
 *
 * @since 4.0
 *
 */
LIBICAL_ICAL_EXPORT void icalenumarray_append(icalenumarray *array,
                                              const icalenumarray_element *elem);

/**
 * @brief Appends an element to the array, omitting duplicates.
 * @param array The array object to which the element append to
 * @param elem The element to append
 *
 * Appends element @p elem to @a array unless a byte-equal string
 * already is present in the array.
 *
 * @par Error handling
 * If @a array or @a elem is `NULL` then this function is a noop.
 *
 * @since 4.0
 *
 */
LIBICAL_ICAL_EXPORT void icalenumarray_add(icalenumarray *array,
                                           const icalenumarray_element *elem);

/**
 * @brief Removes the element at an array position.
 * @param array The array object from which to remove the element
 * @param position The array position of the element to remove
 *
 * Removes the element at @a position in the @a array.
 *
 * @par Error handling
 * If @a array is `NULL` or @a position is higher or equal to the
 * count of elements, this function is a noop.
 *
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT void icalenumarray_remove_element_at(icalenumarray *array,
                                                         size_t position);

/**
 * @brief Removes all occurrences of an element.
 * @param array The array object from which to remove the element
 * @param del The element to remove from the array
 *
 * Removes all occurrences of the element @a del from @a array.
 *
 * @par Error handling
 * If @a array or @a del is `NULL`, then this function is a noop.
 *
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT void icalenumarray_remove(icalenumarray *array,
                                              const icalenumarray_element *del);

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
LIBICAL_ICAL_EXPORT void icalenumarray_free(icalenumarray *array);

/**
 * @brief Sorts the elements in the array in ascending order.
 * @param array The array object to sort.
 *
 * Sorts the array such that all element elements occur in
 * ascending order, comparing the elements as bytes.
 *
 * @par Error handling
 * If @a array is `NULL`, this is a noop.
 *
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT void icalenumarray_sort(icalenumarray *array);

/**
 * @brief Clones the array and all its elements.
 * @param array The array object to clone.
 * @return the cloned array
 *
 * Creates an independent copy of the @a array and all its elements.
 *
 * @par Error handling
 * If @a array is `NULL` then the return value is `NULL`.
 *
 * @since 4.0
 *
 */
LIBICAL_ICAL_EXPORT icalenumarray *icalenumarray_clone(icalenumarray *array);

#endif /* ICALENUMARRAY_H */
