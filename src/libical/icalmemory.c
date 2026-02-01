/*======================================================================
 FILE: icalmemory.c
 CREATOR: eric 30 June 1999

 SPDX-FileCopyrightText: 2000 Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "icalmemory.h"
#include "icalerror.h"
#if defined(MEMORY_CONSISTENCY)
#include "test-malloc.h"
#endif

#include <stdlib.h>
#include <stdbool.h>

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

#if ICAL_SYNC_MODE != ICAL_SYNC_MODE_PTHREAD
/**
 * @private
 */
static ICAL_GLOBAL_VAR buffer_ring *global_buffer_ring = 0;
#endif

/**
 * @private
 *
 * Get rid of this buffer ring.
 */
static void icalmemory_free_ring_byval(buffer_ring *br)
{
    int i;

    for (i = 0; i < BUFFER_RING_SIZE; i++) {
        icalmemory_free_buffer(br->ring[i]);
    }
    icalmemory_free_buffer(br);
}

#if ICAL_SYNC_MODE == ICAL_SYNC_MODE_PTHREAD
#include <pthread.h>

static pthread_key_t ring_key;
static pthread_once_t ring_key_once = PTHREAD_ONCE_INIT;

static void ring_destroy(void *buf)
{
    icalmemory_free_ring_byval((buffer_ring *)buf);
    pthread_setspecific(ring_key, NULL);
}

static void ring_key_alloc(void)
{
    pthread_key_create(&ring_key, ring_destroy);
}

#endif

/**
 * @private
 */
static buffer_ring *buffer_ring_new(void)
{
    buffer_ring *br;
    int i;

    br = (buffer_ring *)icalmemory_new_buffer(sizeof(buffer_ring));
    if (!br) {
        return NULL;
    }

    for (i = 0; i < BUFFER_RING_SIZE; i++) {
        br->ring[i] = 0;
    }
    br->pos = 0;
    return (br);
}

#if ICAL_SYNC_MODE == ICAL_SYNC_MODE_PTHREAD
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
#if ICAL_SYNC_MODE == ICAL_SYNC_MODE_PTHREAD
    return (get_buffer_ring_pthread());
#else
    return get_buffer_ring_global();
#endif
}

/* Add an existing buffer to the buffer ring */
void icalmemory_add_tmp_buffer(void *buf)
{
    buffer_ring *br = get_buffer_ring();
    if (!br) {
        return;
    }

    /* Wrap around the ring */
    if (++(br->pos) == BUFFER_RING_SIZE) {
        br->pos = 0;
    }

    /* Free buffers as their slots are overwritten */
    icalmemory_free_buffer(br->ring[br->pos]);

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

    buf = (void *)icalmemory_new_buffer(size);

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
    if (!br) {
        return;
    }

    icalmemory_free_ring_byval(br);
#if ICAL_SYNC_MODE == ICAL_SYNC_MODE_PTHREAD
    pthread_setspecific(ring_key, 0);
#else
    global_buffer_ring = 0;
#endif
}

/* Like strdup, but the buffer is on the ring. */
char *icalmemory_tmp_copy(const char *str)
{
    char *b;

    if (!str || str[0] == '\0') {
        return NULL;
    }

    const size_t len_b = strlen(str) + 1;
    b = icalmemory_tmp_buffer(len_b);

    if (!b) {
        return NULL;
    }

    strncpy(b, str, len_b);

    return b;
}

char *icalmemory_strdup(const char *s)
{
    size_t l;
    char *res;

    if (!s) {
        return NULL;
    }

    l = (strlen(s) + 1) * sizeof(char);
    res = (char *)icalmemory_new_buffer(l);
    if (res == NULL) {
        return NULL;
    }

    memcpy(res, s, l);

    return res;
}

#if defined(MEMORY_CONSISTENCY)
static ICAL_GLOBAL_VAR icalmemory_malloc_f global_icalmem_malloc = &test_malloc;
#elif defined(ICALMEMORY_DEFAULT_MALLOC) && !defined(S_SPLINT_S)
static ICAL_GLOBAL_VAR icalmemory_malloc_f global_icalmem_malloc = &ICALMEMORY_DEFAULT_MALLOC;
#else
static ICAL_GLOBAL_VAR icalmemory_malloc_f global_icalmem_malloc = NULL;
#endif

#if defined(MEMORY_CONSISTENCY)
static ICAL_GLOBAL_VAR icalmemory_realloc_f global_icalmem_realloc = &test_realloc;
#elif defined(ICALMEMORY_DEFAULT_REALLOC) && !defined(S_SPLINT_S)
static ICAL_GLOBAL_VAR icalmemory_realloc_f global_icalmem_realloc = &ICALMEMORY_DEFAULT_REALLOC;
#else
static ICAL_GLOBAL_VAR icalmemory_realloc_f global_icalmem_realloc = NULL;
#endif

#if defined(MEMORY_CONSISTENCY)
static ICAL_GLOBAL_VAR icalmemory_free_f global_icalmem_free = &test_free;
#elif defined(ICALMEMORY_DEFAULT_FREE) && !defined(S_SPLINT_S)
static ICAL_GLOBAL_VAR icalmemory_free_f global_icalmem_free = &ICALMEMORY_DEFAULT_FREE;
#else
static ICAL_GLOBAL_VAR icalmemory_free_f global_icalmem_free = NULL;
#endif

void icalmemory_set_mem_alloc_funcs(icalmemory_malloc_f f_malloc,
                                    icalmemory_realloc_f f_realloc,
                                    icalmemory_free_f f_free)
{
    global_icalmem_malloc = f_malloc;
    global_icalmem_realloc = f_realloc;
    global_icalmem_free = f_free;
}

void icalmemory_get_mem_alloc_funcs(icalmemory_malloc_f *f_malloc,
                                    icalmemory_realloc_f *f_realloc,
                                    icalmemory_free_f *f_free)
{
    if (f_malloc) {
        *f_malloc = global_icalmem_malloc;
    }
    if (f_realloc) {
        *f_realloc = global_icalmem_realloc;
    }
    if (f_free) {
        *f_free = global_icalmem_free;
    }
}

/*
 * These buffer routines create memory the old fashioned way -- so the
 * caller will have to deallocate the new memory
 */

void *icalmemory_new_buffer(size_t size)
{
    void *b;

    if (global_icalmem_malloc == NULL) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return 0;
    }

    b = global_icalmem_malloc(size);

    if (b == 0) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return 0;
    }

    memset(b, 0, size);

    return b;
}

void *icalmemory_resize_buffer(void *buf, size_t size)
{
    void *b;

    if (global_icalmem_realloc == NULL) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return 0;
    }

    b = global_icalmem_realloc(buf, size);

    if (b == 0) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return 0;
    }

    return b;
}

void icalmemory_free_buffer(void *buf)
{
    if (global_icalmem_free == NULL) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return;
    }

    global_icalmem_free(buf);
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
    data_length = (size_t)*pos - (size_t)*buf;
    final_length = data_length + string_length;

    if (final_length >= (size_t)*buf_size) {
        *buf_size = (*buf_size) * 2 + final_length;

        new_buf = icalmemory_resize_buffer(*buf, *buf_size);
        if (!new_buf) {
            // an error was set in the resize function, so we just return here.
            return;
        }

        new_pos = (void *)((size_t)new_buf + data_length);

        *pos = new_pos;
        *buf = new_buf;
    }

    strcpy(*pos, string); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)

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

    data_length = (size_t)*pos - (size_t)*buf;

    final_length = data_length + 2;

    if (final_length > (size_t)*buf_size) {
        *buf_size = (*buf_size) * 2 + final_length + 1;

        new_buf = icalmemory_resize_buffer(*buf, *buf_size);
        if (!new_buf) {
            // an error was set in the resize function, so we just return here.
            return;
        }

        new_pos = (void *)((size_t)new_buf + data_length);

        *pos = new_pos;
        *buf = new_buf;
    }

    **pos = ch;
    *pos += 1;
    **pos = 0;
}

/*
 * Checks whether this character is allowed in a (Q)SAFE-CHAR
 *
 * QSAFE-CHAR   = WSP / %x21 / %x23-7E / NON-US-ASCII
 * ; any character except CTLs and DQUOTE
 * SAFE-CHAR    = WSP / %x21 / %x23-2B / %x2D-39 / %x3C-7E / NON-US-ASCII
 * ; any character except CTLs, DQUOTE. ";", ":", and ","
 * WSP      = SPACE / HTAB
 * NON-US-ASCII       = %x80-F8
 * ; Use restricted by charset parameter
 * ; on outer MIME object (UTF-8 preferred)
 *
 * Note that comma IS actually safe in vCard but we will quote it anyway
 */
#define UNSAFE_CHARS ";:,"

static bool icalmemory_is_safe_char(unsigned char character, bool quoted)
{
    if (character == ' ' || character == '\t' || character == '!') {
        return true;
    }

    if (character < 0x23 || character == 0x7f || character > 0xf8 ||
        (!quoted && strchr(UNSAFE_CHARS, character))) {
        return false;
    }

    return true;
}

/**
 * Appends the string to the buffer, encoding per RFC 6868
 * and filtering out those characters not permitted by the specifications
 *
 * paramtext    = *SAFE-CHAR
 * quoted-string= DQUOTE *QSAFE-CHAR DQUOTE
 */
void icalmemory_append_encoded_string(char **buf, char **pos,
                                      size_t *buf_size, const char *string)
{
    bool quoted = false;
    const char *p;

    /* Encapsulate the string in quotes if necessary */
    if (!*string || strpbrk(string, UNSAFE_CHARS) != 0) {
        icalmemory_append_char(buf, pos, buf_size, '"');
        quoted = true;
    }

    /* Copy the string */
    for (p = string; *p; p++) {
        /* Encode unsafe characters per RFC6868, otherwise replace with SP */
        switch (*p) {
        case '\n':
            icalmemory_append_string(buf, pos, buf_size, "^n");
            break;

        case '^':
            icalmemory_append_string(buf, pos, buf_size, "^^");
            break;

        case '"':
            icalmemory_append_string(buf, pos, buf_size, "^'");
            break;

        default:
            if (icalmemory_is_safe_char((unsigned char)*p, quoted)) {
                icalmemory_append_char(buf, pos, buf_size, *p);
            } else {
                icalmemory_append_char(buf, pos, buf_size, ' ');
            }
            break;
        }
    }

    if (quoted == true) {
        icalmemory_append_char(buf, pos, buf_size, '"');
    }
}

void icalmemory_append_decoded_string(char **buf, char **pos,
                                      size_t *buf_size, const char *string)
{
    const char *p;

    /* Copy the string */
    for (p = string; *p; p++) {
        switch (*p) {
        case '"':
            /* Remove encapsulating quotes if necessary */
            break;

        case '^':
            /* Decode unsafe characters per RFC6868 */
            if (p[1] == 'n') {
                icalmemory_append_char(buf, pos, buf_size, '\n');
                p++;
            } else if (p[1] == '^') {
                icalmemory_append_char(buf, pos, buf_size, '^');
                p++;
            } else if (p[1] == '\'') {
                icalmemory_append_char(buf, pos, buf_size, '"');
                p++;
            } else {
                // Unknown escape sequence, copy verbatim.
                icalmemory_append_char(buf, pos, buf_size, *p);
            }
            break;

        default:
            icalmemory_append_char(buf, pos, buf_size, *p);
        }
    }
}
