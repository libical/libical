/*======================================================================
 FILE: icalerror.c
 CREATOR: eric 16 May 1999

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
======================================================================*/

/**
 * @file icalerror.c
 * @brief Error handling for libical
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "icalerror.h"
#include "icalerror_p.h"
#include "icalmemory.h"

#include <stdbool.h>
#include <stdlib.h>

#if defined(HAVE_BACKTRACE)
#include <execinfo.h>
#endif

static ICAL_GLOBAL_VAR bool icalerror_errors_are_fatal = false;

void icalerror_set_errors_are_fatal(bool fatal)
{
    icalerror_errors_are_fatal = fatal;
}

bool icalerror_get_errors_are_fatal(void)
{
    return icalerror_errors_are_fatal;
}

#if ICAL_SYNC_MODE == ICAL_SYNC_MODE_PTHREAD
#include <pthread.h>

static pthread_key_t icalerrno_key;
static pthread_once_t icalerrno_key_once = PTHREAD_ONCE_INIT;

static void icalerrno_destroy(void *buf)
{
    icalmemory_free_buffer(buf);
    pthread_setspecific(icalerrno_key, NULL);
}

static void icalerrno_key_alloc(void)
{
    pthread_key_create(&icalerrno_key, icalerrno_destroy);
}

icalerrorenum *icalerror_icalerrno(void)
{
    icalerrorenum *_errno;

    pthread_once(&icalerrno_key_once, icalerrno_key_alloc);

    _errno = (icalerrorenum *)pthread_getspecific(icalerrno_key);

    if (!_errno) {
        _errno = icalmemory_new_buffer(sizeof(icalerrorenum));
        *_errno = ICAL_NO_ERROR;
        pthread_setspecific(icalerrno_key, _errno);
    }
    return _errno;
}

#else

static ICAL_GLOBAL_VAR icalerrorenum icalerrno_storage = ICAL_NO_ERROR;

icalerrorenum *icalerror_icalerrno(void)
{
    return &icalerrno_storage;
}

#endif

void icalerror_clear_errno(void)
{
    icalerrno = ICAL_NO_ERROR;
}

void icalerror_set_errno(icalerrorenum x)
{
    icalerrno = x;
    if (icalerror_get_error_state(x) == ICAL_ERROR_FATAL ||
        (icalerror_get_error_state(x) == ICAL_ERROR_DEFAULT && icalerror_errors_are_fatal == 1)) {
        icalerror_warn(icalerror_strerror(x));
        icalerror_backtrace();
        icalassert(0);
    }
}

void icalerror_backtrace(void)
{
#if defined(HAVE_BACKTRACE)
    void *stack_frames[50];
    int i, num;
    size_t size;
    char **strings;

    size = sizeof(stack_frames) / sizeof(stack_frames[0]);
    num = backtrace(stack_frames, (int)size);
    strings = backtrace_symbols(stack_frames, num);
    for (i = 0; i < num; i++) {
        if (strings != NULL) {
            icalerrprintf("%s\n", strings[i]);
        } else {
            icalerrprintf("%p\n", stack_frames[i]);
        }
    }
    free((void *)strings); /* Not icalmemory_free_buffer(), allocated by backtrace_symbols() */
#endif
}
