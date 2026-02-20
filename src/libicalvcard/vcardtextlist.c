/*======================================================================
 FILE: vcardtextlist.c
 CREATOR: Robert Stepanek 1 Feb 2026

 SPDX-FileCopyrightText: 2026, Fastmail Pty. Ltd. (https://fastmail.com)
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 ======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "vcardtextlist.h"
#include "vcardvalue.h"
#include "icalerror.h"
#include "icalmemory.h"

vcardstrarray *vcardtextlist_new_from_string(const char *str, char sep)
{
    icalerror_check_arg_rz(str != 0, "str");
    icalerror_check_arg_rz(sep != 0, "sep");

    vcardstrarray *array = vcardstrarray_new(2);
    char sep_str[2] = {sep, 0};

    do {
        char *dequoted_str = vcardvalue_strdup_and_dequote_text(&str, sep_str);
        vcardstrarray_append(array, dequoted_str);
        icalmemory_free_buffer(dequoted_str);
    } while (*str++ != '\0');

    return array;
}
