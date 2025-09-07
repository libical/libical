/*======================================================================
 * FILE: icalduration_p.h
 * SPDX-FileCopyrightText: 2025 Contributors to the libical project <git@github.com:libical/libical>
 * SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 *
 * The original code is icalduration.h by Eric Busboom
 *======================================================================*/

/*************************************************************************
 * WARNING: USE AT YOUR OWN RISK                                         *
 * These are library internal-only functions.                            *
 * Be warned that these functions can change at any time without notice. *
 *************************************************************************/

#ifndef ICAL_DURATION_P_H
#define ICAL_DURATION_P_H

#include "libical_ical_export.h"

#include "ical.h"

/**
 * @brief Creates a new ::icaldurationtype from a duration in seconds.
 * @param t The duration in seconds
 * @return An ::icaldurationtype representing the duration @a t in seconds
 *
 * @par Example
 * ```c
 * // create a new icaldurationtype with a duration of 60 seconds
 * struct icaldurationtype duration;
 * duration = icaldurationtype_from_int(60);
 *
 * // verify that the duration is one minute
 * assert(duration.minutes == 1);
 * ```
 */
LIBICAL_ICAL_EXPORT struct icaldurationtype icaldurationtype_from_int(int t);

/**
 * @brief Converts an ::icaldurationtype into the duration in seconds as `int`.
 * @param duration The duration to convert to seconds
 * @return An `int` representing the duration in seconds
 *
 * @par Usage
 * ```c
 * // create icaldurationtype with given duration
 * struct icaldurationtype duration;
 * duration = icaldurationtype_from_int(3532342);
 *
 * // get the duration in seconds and verify it
 * assert(icaldurationtype_as_int(duration) == 3532342);
 * ```
 */
LIBICAL_ICAL_EXPORT int icaldurationtype_as_int(struct icaldurationtype duration);

#endif
