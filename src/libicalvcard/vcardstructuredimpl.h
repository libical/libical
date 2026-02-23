/*======================================================================
 FILE: vcardstructuredimpl.h
 CREATOR: Robert Stepanek 19 Feb 2026

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 ======================================================================*/

#ifndef VCARDSTRUCTUREDIMPL_H
#define VCARDSTRUCTUREDIMPL_H

#include "vcardstrarray.h"

struct vcardstructuredtype_impl {
    /* Reference count */
    unsigned refcount;

    size_t num_fields;
    size_t num_alloc;
    vcardstrarray **field;
};

#endif /* VCARDSTRUCTUREDIMPL_H */
