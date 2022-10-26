/*======================================================================
 FILE: icalmemory.c
 CREATOR: eric 30 June 1999

 Copyright (C) 2000 Eric Busboom <eric@civicknowledge.com>

 This library is free software; you can redistribute it and/or modify
 it under the terms of either:

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: https://www.gnu.org/licenses/lgpl-2.1.html

 Or:

    The Mozilla Public License Version 2.0. You may obtain a copy of
    the License at https://www.mozilla.org/MPL/

 This library is free software; you can redistribute it and/or modify
 it under the terms of either:

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: https://www.gnu.org/licenses/lgpl-2.1.html

 Or:

    The Mozilla Public License Version 2.0. You may obtain a copy of
    the License at https://www.mozilla.org/MPL/
======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "icalmemory.h"
#include "icalerror.h"

#include <stdlib.h>

/**
 * @brief Determines the size of the ring buffer used for keeping track of
 * temporary buffers.
 */
#define BUFFER_RING_SIZE 2500

/**
 * @brief Determines the minimal size of buffers in the ring that are created
 * with icalmemory_tmp_buffer().
 */
#define MIN_BUFFER_SIZE 200

/* HACK. Not threadsafe */

typedef struct
{
    int pos;
    void *ring[BUFFER_RING_SIZE];
} buffer_ring;

#if !defined(HAVE_PTHREAD)
/**
 * @private
 */
static buffer_ring *global_buffer_ring = 0;
#endif

/**
 * @private
 *
 * Get rid of this buffer ring.
 */
static void icalmemory_free_ring_byval(buffer_ring * br)
{
    int i;

    for (i = 0; i < BUFFER_RING_SIZE; i++) {
        if (br->ring[i] != 0) {
            free(br->ring[i]);
        }
    }
    free(br);
}

#if defined(HAVE_PTHREAD)
#include <pthread.h>

static pthread_key_t ring_key;
static pthread_once_t ring_key_once = PTHREAD_ONCE_INIT;

static void ring_destroy(void *buf)
{
    if (buf)
        icalmemory_free_ring_byval((buffer_ring *) buf);

    pthread_setspecific(ring_key, NULL);
}

static void ring_key_alloc(void)
{
    pthread_key_create(&ring_key, ring_destroy);
}

#endif

#if 0
/*keep for historical sake*/
static void icalmemory_free_tmp_buffer(void *buf)
{
    if (buf == 0) {
        return;
    }

    free(buf);
}

#endif

/**
 * @private
 */
static buffer_ring *buffer_ring_new(void)
{
    buffer_ring *br;
    int i;

    br = (buffer_ring *) malloc(sizeof(buffer_ring));

    for (i = 0; i < BUFFER_RING_SIZE; i++) {
        br->ring[i] = 0;
    }
    br->pos = 0;
    return (br);
}

#if defined(HAVE_PTHREAD)
/**
 * @private
 */
static buffer_ring *get_buffer_ring_pthread(void)
{
    buffer_ring *br;

    pthread_once(&ring_key_once, ring_key_alloc);

    br = pthread_getspecific(ring_key);

    if (!br) {
        br = buffer_ring_new();
        pthread_setspecific(ring_key, br);
    }
    return (br);
}

#else
/**
 * @private
 *
 * Get buffer ring via a single global for a non-threaded program
 */
static buffer_ring *get_buffer_ring_global(void)
{
    if (global_buffer_ring == 0) {
        global_buffer_ring = buffer_ring_new();
    }
    return (global_buffer_ring);
}

#endif

/**
 * @private
 */
static buffer_ring *get_buffer_ring(void)
{
#if defined(HAVE_PTHREAD)
    return (get_buffer_ring_pthread());
#else
    return get_buffer_ring_global();
#endif
}

/* Add an existing buffer to the buffer ring */
void icalmemory_add_tmp_buffer(void *buf)
{
    buffer_ring *br = get_buffer_ring();

    /* Wrap around the ring */
    if (++(br->pos) == BUFFER_RING_SIZE) {
        br->pos = 0;
    }

    /* Free buffers as their slots are overwritten */
    if (br->ring[br->pos] != 0) {
        free(br->ring[br->pos]);
    }

    /* Assign the buffer to a slot */
    br->ring[br->pos] = buf;
}

/*
 * Create a new temporary buffer on the ring. Libical owns these and
 * will deallocate them.
 */

void *icalmemory_tmp_buffer(size_t size)
{
    char *buf;

    if (size < MIN_BUFFER_SIZE) {
        size = MIN_BUFFER_SIZE;
    }

    buf = (void *)malloc(size);

    if (buf == 0) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return 0;
    }

    memset(buf, 0, size);

    icalmemory_add_tmp_buffer(buf);

    return buf;
}

void icalmemory_free_ring(void)
{
    buffer_ring *br;

    br = get_buffer_ring();

    icalmemory_free_ring_byval(br);
#if defined(HAVE_PTHREAD)
    pthread_setspecific(ring_key, 0);
#else
    global_buffer_ring = 0;
#endif
}

/* Like strdup, but the buffer is on the ring. */
char *icalmemory_tmp_copy(const char *str)
{
    char *b = icalmemory_tmp_buffer(strlen(str) + 1);

    strcpy(b, str);

    return b;
}

char *icalmemory_strdup(const char *s)
{
    return strdup(s);
}

/*
 * These buffer routines create memory the old fashioned way -- so the
 * caller will have to deallocate the new memory
 */

void *icalmemory_new_buffer(size_t size)
{
    void *b = malloc(size);

    if (b == 0) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return 0;
    }

    memset(b, 0, size);

    return b;
}

void *icalmemory_resize_buffer(void *buf, size_t size)
{
    void *b = realloc(buf, size);

    if (b == 0) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return 0;
    }

    return b;
}

void icalmemory_free_buffer(void *buf)
{
    free(buf);
}

void icalmemory_append_string(char **buf, char **pos, size_t *buf_size, const char *string)
{
    char *new_buf;
    char *new_pos;

    size_t data_length, final_length, string_length;

#if !defined(ICAL_NO_INTERNAL_DEBUG)
    icalerror_check_arg_rv((buf != 0), "buf");
    icalerror_check_arg_rv((*buf != 0), "*buf");
    icalerror_check_arg_rv((pos != 0), "pos");
    icalerror_check_arg_rv((*pos != 0), "*pos");
    icalerror_check_arg_rv((buf_size != 0), "buf_size");
    icalerror_check_arg_rv((*buf_size != 0), "*buf_size");
    icalerror_check_arg_rv((string != 0), "string");
#endif

    string_length = strlen(string);
    data_length = (size_t) * pos - (size_t) * buf;
    final_length = data_length + string_length;

    if (final_length >= (size_t) * buf_size) {

        *buf_size = (*buf_size) * 2 + final_length;

        new_buf = realloc(*buf, *buf_size);

        new_pos = (void *)((size_t) new_buf + data_length);

        *pos = new_pos;
        *buf = new_buf;
    }

    strcpy(*pos, string);

    *pos += string_length;
}

void icalmemory_append_char(char **buf, char **pos, size_t *buf_size, char ch)
{
    char *new_buf;
    char *new_pos;

    size_t data_length, final_length;

#if !defined(ICAL_NO_INTERNAL_DEBUG)
    icalerror_check_arg_rv((buf != 0), "buf");
    icalerror_check_arg_rv((*buf != 0), "*buf");
    icalerror_check_arg_rv((pos != 0), "pos");
    icalerror_check_arg_rv((*pos != 0), "*pos");
    icalerror_check_arg_rv((buf_size != 0), "buf_size");
    icalerror_check_arg_rv((*buf_size != 0), "*buf_size");
#endif

    data_length = (size_t) * pos - (size_t) * buf;

    final_length = data_length + 2;

    if (final_length > (size_t) * buf_size) {

        *buf_size = (*buf_size) * 2 + final_length + 1;

        new_buf = realloc(*buf, *buf_size);

        new_pos = (void *)((size_t) new_buf + data_length);

        *pos = new_pos;
        *buf = new_buf;
    }

    **pos = ch;
    *pos += 1;
    **pos = 0;
}
