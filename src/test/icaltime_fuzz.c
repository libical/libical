/*======================================================================
 *
 * SPDX-FileCopyrightText: 2024 Contributors to the libical project <git@github.com:libical/libical>
 * SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 *
 * Based on the original code by Gabe Sherman in
 * https://github.com/libical/libical/issues/678
 * ======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdint.h>

#include "libical/ical.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef unsigned int usize;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef int isize;
typedef float f32;
typedef double f64;

int main(void)
{
    struct _icaltimezone *v1;

    icaltimetype v0 = {
        5,
        -2147483639,
        -1,
        1,
        60,
        14,
        7,
        118,
        NULL,
    }; // dtstart

    v1 = icaltimezone_new(); // zone
    if (v1 == NULL)
        return 0;

    icaltimetype v3 = {
        3,
        -11,
        3,
        4,
        -4,
        -3,
        6,
        -1,
        v1,
    }; // dtend

    i32 v4 = 40;                         // is_busy
    (void)icaltime_span_new(v0, v3, v4); // $target

    icaltimezone_free(v1, 1);
    return 0;
}
