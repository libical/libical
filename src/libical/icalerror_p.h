/*======================================================================
  FILE: icalerror_p.h
  CREATOR: eric 09 May 1999

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
======================================================================*/

/*************************************************************************
 * WARNING: USE AT YOUR OWN RISK                                         *
 * These are library internal-only functions.                            *
 * Be warned that these functions can change at any time without notice. *
 *************************************************************************/

#ifndef ICALERROR_P_H
#define ICALERROR_P_H

#include "libical_ical_export.h"

#include "icalerror.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

/**
 * @brief Triggered before any error is called
 *
 * This routine is called before any error is triggered.
 * It is called by icalerror_set_errno(), so it does not
 * appear in all of the macros below.
 *
 * This routine can be used while debugging by setting
 * a breakpoint here.
 */
LIBICAL_ICAL_EXPORT void icalerror_stop_here(void);

/**
 * @brief Triggered to abort the process
 *
 * This routine is called to abort the process in the
 * case of an error.
 */
LIBICAL_ICAL_EXPORT void icalerror_crash_here(void);

/* Warning messages */

/**
 * @def icalerror_warn(message)
 * @brief Prints a formatted warning message to stderr
 * @param message Warning message to print
 *
 * @par Usage
 * ```c
 * icalerror_warn("Non-standard tag encountered");
 * ```
 */
#define icalerrprintf(...) fprintf(stderr, __VA_ARGS__)
#ifdef __GNUC__
#define icalerror_warn(message)                                                        \
    {                                                                                  \
        icalerrprintf("%s(), %s:%d: %s\n", __FUNCTION__, __FILE__, __LINE__, message); \
    }
#else /* __GNU_C__ */
#define icalerror_warn(message)                                    \
    {                                                              \
        icalerrprintf("%s:%d: %s\n", __FILE__, __LINE__, message); \
    }
#endif /* __GNU_C__ */

#define icalerror_check_value_type(value, type) ;
#define icalerror_check_property_type(value, type) ;
#define icalerror_check_parameter_type(value, type) ;
#define icalerror_check_component_type(value, type) ;

/* Assert with a message */
/**
 * @def icalerror_assert(test, message)
 * @brief Assert with a message
 * @param test The assertion to test
 * @param message The message to print on failure of assertion
 *
 * Tests the given assertion @a test, and if it fails, prints the
 * @a message given on @a stderr as a warning and aborts the process.
 * This only works if icalerror_get_errors_are_fatal() is true, otherwise
 * does nothing.
 */

#ifdef __GNUC__
#define icalerror_assert(test, message)                                                \
    if (icalerror_get_errors_are_fatal() && !(test)) {                                 \
        icalerrprintf("%s(), %s:%d: %s\n", __FUNCTION__, __FILE__, __LINE__, message); \
        icalerror_stop_here();                                                         \
        abort();                                                                       \
    }
#else /*__GNUC__*/
#define icalerror_assert(test, message)                            \
    if (icalerror_get_errors_are_fatal() && !(test)) {             \
        icalerrprintf("%s:%d: %s\n", __FILE__, __LINE__, message); \
        icalerror_stop_here();                                     \
        abort();                                                   \
    }
#endif /*__GNUC__*/

/**
 * @brief Checks the assertion @a test and raises error on failure
 * @param test The assertion to check
 * @param arg  The argument involved (as a string)
 *
 * This function checks the assertion @a test, which is used to
 * test if the parameter @a arg is correct. If the assertion fails,
 * it sets ::icalerrno to ::ICAL_BADARG_ERROR.
 *
 * @par Example
 * ```c
 * void test_function(icalcomponent *component) {
 *    icalerror_check_arg(component != 0, "component");
 *
 *    // use component
 * }
 * ```
 */
#define icalerror_check_arg(test, arg)          \
    if (!(test)) {                              \
        icalerror_set_errno(ICAL_BADARG_ERROR); \
    }

/**
 * @brief Checks the assertion @a test and raises error on failure, returns void
 * @param test The assertion to check
 * @param arg  The argument involved (as a string)
 *
 * This function checks the assertion @a test, which is used to
 * test if the parameter @a arg is correct. If the assertion fails,
 * it sets ::icalerrno to ::ICAL_BADARG_ERROR and causes the enclosing
 * function to return `void`.
 *
 * @par Example
 * ```c
 * void test_function(icalcomponent *component) {
 *    icalerror_check_arg_rv(component != 0, "component");
 *
 *    // use component
 * }
 * ```
 */
#define icalerror_check_arg_rv(test, arg)       \
    if (!(test)) {                              \
        icalerror_set_errno(ICAL_BADARG_ERROR); \
        return;                                 \
    }

/**
 * @brief Checks the assertion @a test and raises error on failure, returns 0
 * @param test The assertion to check
 * @param arg  The argument involved (as a string)
 *
 * This function checks the assertion @a test, which is used to
 * test if the parameter @a arg is correct. If the assertion fails,
 * it sets ::icalerrno to ::ICAL_BADARG_ERROR and causes the enclosing
 * function to return `0`.
 *
 * @par Example
 * ```c
 * int test_function(icalcomponent *component) {
 *    icalerror_check_arg_rz(component != 0, "component");
 *
 *    // use component
 *    return icalcomponent_count_kinds(component, ICAL_ANY_COMPONENT);
 * }
 * ```
 */
#define icalerror_check_arg_rz(test, arg)       \
    if (!(test)) {                              \
        icalerror_set_errno(ICAL_BADARG_ERROR); \
        return 0;                               \
    }

/**
 * @brief Checks the assertion @a test and raises error on failure, returns @a error
 * @param test The assertion to check
 * @param arg  The argument involved (as a string)
 * @param error What to return on error
 *
 * This function checks the assertion @a test, which is used to
 * test if the parameter @a arg is correct. If the assertion fails,
 * it aborts the process with `assert(0)` and causes the enclosing
 * function to return @a error.
 *
 * @par Example
 * ```c
 * icalcomponent *test_function(icalcomponent *component) {
 *    icalerror_check_arg_re(component != 0, "component", NULL);
 *
 *    // use component
 *    return icalcomponent_get_first_real_component(component);
 * }
 * ```
 */
#define icalerror_check_arg_re(test, arg, error) \
    if (!(test)) {                               \
        icalerror_stop_here();                   \
        icalassert(0);                           \
        return error;                            \
    }

/**
 * @brief Checks the assertion @a test and raises error on failure, returns @a x
 * @param test The assertion to check
 * @param arg  The argument involved (as a string)
 * @param x    What to return on error
 *
 * This function checks the assertion @a test, which is used to
 * test if the parameter @a arg is correct. If the assertion fails,
 * it sets ::icalerrno to ::ICAL_BADARG_ERROR and causes the enclosing
 * function to return @a x.
 *
 * @par Example
 * ```c
 * icalcomponent *test_function(icalcomponent *component) {
 *    icalerror_check_arg_rx(component != 0, "component", NULL);
 *
 *    // use component
 *    return icalcomponent_get_first_real_component(component);
 * }
 * ```
 */
#define icalerror_check_arg_rx(test, arg, x)    \
    if (!(test)) {                              \
        icalerror_set_errno(ICAL_BADARG_ERROR); \
        return x;                               \
    }

#endif /* !ICALERROR_H */
