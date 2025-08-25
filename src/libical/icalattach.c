/*======================================================================
 FILE: icalattach.c
 CREATOR: acampi 28 May 02

 SPDX-FileCopyrightText: 2002, Andrea Campi <a.campi@inet.it>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 ======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "icalattachimpl.h"
#include "icalerror.h"
#include "icalmemory.h"

#include <errno.h>
#include <stdlib.h>

icalattach *icalattach_new_from_url(const char *url)
{
    icalattach *attach;
    char *url_copy;

    icalerror_check_arg_rz((url != NULL), "url");

    if ((attach = icalmemory_new_buffer(sizeof(icalattach))) == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    if ((url_copy = icalmemory_strdup(url)) == NULL) {
        icalmemory_free_buffer(attach);
        errno = ENOMEM;
        return NULL;
    }

    attach->refcount = 1;
    attach->is_url = 1;
    attach->u.url.url = url_copy;

    return attach;
}

static void attach_data_free(char *data, void *free_fn_data)
{
    _unused(free_fn_data);
    icalmemory_free_buffer(data);
}

icalattach *icalattach_new_from_data(const char *data, icalattach_free_fn_t free_fn,
                                     void *free_fn_data)
{
    icalattach *attach;

    icalerror_check_arg_rz((data != NULL), "data");

    if ((attach = icalmemory_new_buffer(sizeof(icalattach))) == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    if (!free_fn) {
        data = icalmemory_strdup(data);
        if (!data) {
            icalmemory_free_buffer(attach);
            errno = ENOMEM;
            return NULL;
        }
        free_fn = attach_data_free;
    }

    attach->refcount = 1;
    attach->is_url = 0;
    attach->u.data.data = (char *)data;
    attach->u.data.free_fn = free_fn;
    attach->u.data.free_fn_data = free_fn_data;

    return attach;
}

void icalattach_ref(icalattach *attach)
{
    icalerror_check_arg_rv((attach != NULL), "attach");
    icalerror_check_arg_rv((attach->refcount > 0), "attach->refcount > 0");

    attach->refcount++;
}

void icalattach_unref(icalattach *attach)
{
    icalerror_check_arg_rv((attach != NULL), "attach");
    icalerror_check_arg_rv((attach->refcount > 0), "attach->refcount > 0");

    attach->refcount--;

    if (attach->refcount != 0)
        return;

    if (attach->is_url) {
        icalmemory_free_buffer(attach->u.url.url);
    } else if (attach->u.data.free_fn) {
        (*attach->u.data.free_fn)(attach->u.data.data, attach->u.data.free_fn_data);
    }

    icalmemory_free_buffer(attach);
}

int icalattach_get_is_url(icalattach *attach)
{
    icalerror_check_arg_rz((attach != NULL), "attach");

    return attach->is_url ? 1 : 0;
}

const char *icalattach_get_url(icalattach *attach)
{
    icalerror_check_arg_rz((attach != NULL), "attach");
    icalerror_check_arg_rz((attach->is_url), "attach->is_url");

    return attach->u.url.url;
}

unsigned char *icalattach_get_data(icalattach *attach)
{
    icalerror_check_arg_rz((attach != NULL), "attach");
    icalerror_check_arg_rz((!attach->is_url), "!attach->is_url");

    return (unsigned char *)attach->u.data.data;
}
