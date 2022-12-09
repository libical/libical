/*======================================================================
 FILE: icalmemory.h
 CREATOR: eric 30 June 1999

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>

 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 The Initial Developer of the Original Code is Eric Busboom
======================================================================*/

#ifndef ICALMEMORY_H
#define ICALMEMORY_H

/**
 * @file icalmemory.h
 * @brief Common memory management routines.
 *
 * libical often passes strings back to the caller. To make these
 * interfaces simple, I did not want the caller to have to pass in a
 * memory buffer, but having libical pass out newly allocated memory
 * makes it difficult to de-allocate the memory.
 *
 * The ring buffer in this scheme makes it possible for libical to pass out
 * references to memory which the caller does not own, and be able to
 * de-allocate the memory later. The ring allows libical to have several buffers
 * active simultaneously, which is handy when creating string representations of
 * components. Methods for working with these temporary buffers are marked with
 * `icalmemory_tmp_*()`.
 *
 * Other memory management routines include wrappers around the system
 * management routines like icalmemory_new_buffer() and icalmemory_free_buffer()
 * as well as routines to work with strings, like icalmemory_append_string().
 */

#include "libical_ical_export.h"

/**
 * @brief Creates a new temporary buffer on the ring and returns it.
 * @param size How big (in bytes) the buffer should be
 * @return A pointer to the newly created buffer on the ring
 *
 * Creates a temporary buffer on the ring. Regardless of what @a size you
 * specify, the buffer will always be at least MIN_BUFFER_SIZE big, and it
 * will be zeroed out.
 *
 * @par Error handling
 * If there is a problem allocating memory for the buffer, it sets ::icalerrno
 * to ::ICAL_NEWFAILED_ERROR and returns `NULL`.
 *
 * @par Ownership
 * The returned buffer is owned by icalmemory. It must not be freed by the
 * caller and the returned memory will be automatically reclaimed as more items
 * are added to the ring buffer.
 *
 * @par Usage
 * ```c
 * char *str = icalmemory_tmp_buffer(256);
 * strcpy(str, "some data");
 *
 * // use str
 * ```
 */
LIBICAL_ICAL_EXPORT void *icalmemory_tmp_buffer(size_t size);

/**
 * @brief Creates a copy of the given string, stored on the ring buffer, and
 *  returns it.
 * @param str The string to copy
 * @return A copy of @a str, which has been placed on the ring buffer for
 *  automatic reclamation.
 *
 * @par Error handling
 * The passed string @a str must not be `NULL`, otherwise a segfault might
 * ensue, since the routine calls `strlen()` on it.
 *
 * @par Ownership
 * The returned string is owned by icalmemory. It must not be freed by the
 * caller, and it will be automatically reclaimed as more items are added to the
 * buffer.
 *
 * @par Usage
 * ```c
 * const char *str = "Example string";
 * char *tmp_copy = icalmemory_tmp_copy(str);
 * ```
 */
LIBICAL_ICAL_EXPORT char *icalmemory_tmp_copy(const char *str);

/**
 * @brief Adds an externally allocated buffer to the ring.
 * @param buf The externally allocated buffer to add to the ring
 *
 * Adds an externally allocated buffer to the ring. This ensures that libical
 * will `free()` the buffer automatically, either after BUFFER_RING_SIZE other
 * buffers have been created or added, or after ::icalmemory_free_ring() has
 * been called. Note that freeing the buffers is done using the
 * icalmemory_free_buffer() function, which by default is a wrapper around stdlib's
 * free() function. However, if the memory management functions are
 * customized by the user, the user must make sure to only pass in buffers
 * that have been allocated in a compatible manner.
 *
 * @par Error handling
 * No error is raised if @a buf is `NULL`.
 *
 * @par Ownership
 * After adding @a buf to the ring, it becomes owned by icalmemory and must not
 * be `free()`d manually anymore, it leads to a double-`free()` when icalmemory
 * reclaims the memory.
 *
 * @par Usage
 * ```c
 * char *buf = calloc(256, sizeof(char));
 *
 * icalmemory_add_tmp_buffer(buf);
 * ```
 */
LIBICAL_ICAL_EXPORT void icalmemory_add_tmp_buffer(void *buf);

/**
 * @brief Frees all memory used in the ring
 *
 * Frees all memory used in the ring. If PTHREAD is used or thread-local mode is configured,
 * the ring buffer is allocated on a per-thread basis, meaning that if all
 * rings are to be released, it must be called once in every thread.
 *
 * @par Usage
 * ``` c
 * void *buf = icalmemory_tmp_buffer(256);
 *
 * // use buf
 *
 * // release buf and all other memory in the ring buffer
 * icalmemory_free_ring();
 * ```
 */
LIBICAL_ICAL_EXPORT void icalmemory_free_ring(void);

typedef void *(*icalmemory_malloc_f)(size_t);
typedef void *(*icalmemory_realloc_f)(void *, size_t);
typedef void (*icalmemory_free_f)(void *);

/**
 * @brief Configures the functions to use for memory management.
 *
 * @param f_malloc The function to use for memory allocation.
 * @param f_realloc The function to use for memory reallocation.
 * @param f_free The function to use for memory deallocation.
 *
 * This function configures the library to use the specified functions for
 * memory management. By default the standard system memory management
 * functions malloc(), realloc() and free() are used.
 *
 * Note: The memory management functions configured via this
 * functions are used throughout the core libical component but not within
 * other components like libicalvcal.
 * @since 3.1.0
 */
LIBICAL_ICAL_EXPORT void icalmemory_set_mem_alloc_funcs(icalmemory_malloc_f f_malloc,
                                                        icalmemory_realloc_f f_realloc,
                                                        icalmemory_free_f f_free);

/**
 * @brief Returns the functions used for memory management.
 *
 * @param f_malloc A pointer to the function to use for memory allocation.
 * @param f_realloc A pointer to the function to use for memory reallocation.
 * @param f_free A pointer to the function to use for memory deallocation.
 *
 * Retrieves the functions used by the library for memory management.
 * @since 3.1.0
 */
LIBICAL_ICAL_EXPORT void icalmemory_get_mem_alloc_funcs(icalmemory_malloc_f *f_malloc,
                                                        icalmemory_realloc_f *f_realloc, icalmemory_free_f *f_free);

/**
 * @brief Creates new buffer with the specified size.
 * @param size The size of the buffer that is to be created.
 * @return A pointer to the newly-created buffer.
 * @sa icalmemory_free_buffer()
 *
 * @par Error handling
 * If there is a problem allocating memory, it sets ::icalerrno to
 * ::ICAL_NEWFAILED_ERROR and returns `NULL`.
 *
 * @par Ownership
 * Buffers created with this method are owned by the caller. They must be
 * released with the icalmemory_free_buffer() method.
 *
 * This creates a new (non-temporary) buffer of the specified @a size. All
 * buffers returned by this method are zeroed-out.
 *
 * By default this function delegates to stdlib's malloc() but
 * the used function can be changed via icalmemory_set_mem_alloc_funcs().
 *
 * @par Usage
 * ```c
 * // create buffer
 * char *buffer = icalmemory_new_buffer(50);
 *
 * // fill buffer
 * strcpy(buffer, "some data");
 *
 * // release buffer
 * icalmemory_free_buffer(buffer);
 * ```
 */
LIBICAL_ICAL_EXPORT void *icalmemory_new_buffer(size_t size);

/**
 * @brief Resizes a buffer created with icalmemory_new_buffer().
 * @param buf The buffer to be resized.
 * @param size The new size of the buffer.
 * @return The new, resized buffer.
 * @sa icalmemory_new_buffer()
 * @warning This method may not be used for temporary buffers (buffers allocated
 * with icalmemory_tmp_buffer() and related functions)!
 *
 * @par Error handling
 * If there is a problem while reallocating the buffer, the method sets
 * ::icalerrno to ::ICAL_NEWFAILED_ERROR and returns `NULL`.
 *
 * @par Ownership
 * The returned buffer is owned by the caller and needs to be released with the
 * appropriate icalmemory_free_buffer() method. The old buffer, @a buf, can not
 * be used anymore after calling this method.
 *
 * By default this function delegates to stdlib's realloc() but
 * the used function can be configured via icalmemory_set_mem_alloc_funcs().
 *
 * @par Usage
 * ```c
 * // create new buffer
 * char *buffer = icalmemory_new_buffer(10);
 *
 * // fill buffer
 * strcpy(buffer, "some data");
 *
 * // expand buffer
 * buffer = icalmemory_resize_buffer(buffer, 20);
 *
 * // fill with more data
 * strcpy(buffer, "a lot more data");
 *
 * // release
 * icalmemory_free_buffer(buffer);
 * ```
 */
LIBICAL_ICAL_EXPORT void *icalmemory_resize_buffer(void *buf, size_t size);

/**
 * @brief Releases a buffer
 * @param buf The buffer to release
 * @sa icalmemory_new_buffer()
 *
 * Releases the memory of the buffer.
 *
 * By default this function delegates to stdlib's free() but
 * the used function can be configured via icalmemory_set_mem_alloc_funcs().
 */
LIBICAL_ICAL_EXPORT void icalmemory_free_buffer(void *buf);

/* THESE ROUTINES CAN NOT BE USED ON TMP BUFFERS. Only use them on
   normally allocated memory, or on buffers created from
   icalmemory_new_buffer, never with buffers created by
   icalmemory_tmp_buffer. If icalmemory_append_string has to resize a
   buffer on the ring, the ring will loose track of it and you will
   have memory problems. */

/**
 * @brief Appends a string to a buffer.
 * @param buf The buffer to append the string to.
 * @param pos The position to append the string at.
 * @param buf_size The size of the buffer (will be changed if buffer is
 * reallocated)
 * @param string The string to append to the buffer.
 * @warning This method may not be used for temporary buffers (buffers allocated
 *  with icalmemory_tmp_buffer() and related functions)!
 *
 * @par Error handling
 * Sets ::icalerrno to ::ICAL_BADARG_ERROR if @a buf, `*buf`, @a pos, `* pos`,
 * @a buf_size or @a string are `NULL`.
 *
 * This method will copy the string @a string to the buffer @a buf starting at
 * position @a pos, reallocing @a buf if it is too small. @a buf_size is the
 * size of @a buf and will be changed if @a buf is reallocated. @a pos will
 * point to the last byte of the new string in @a buf, usually a `'\0'`
 *
 * @par Example
 * ```c
 * // creates a new buffer
 * int buffer_len = 15;
 * char *buffer = icalmemory_new_buffer(buffer_len);
 * strcpy(buffer, "My name is: ");
 *
 * // append a string to the buffer
 * int buffer_end = strlen(buffer);
 * char *buffer_end_pos = buffer[buffer_str_end];
 * icalmemory_append_string(&buffer, &buffer_end_pos, &buffer_len, "John Doe");
 *
 * // print string
 * printf("%s\n", buffer);
 *
 * // release memory
 * icalmemory_free_buffer(buffer);
 * ```
 */
LIBICAL_ICAL_EXPORT void icalmemory_append_string(char **buf, char **pos, size_t *buf_size,
                                                  const char *string);

/**
 * @brief Appends a character to a buffer.
 * @param buf The buffer to append the character to.
 * @param pos The position to append the character at.
 * @param buf_size The size of the buffer (will be changed if buffer is
 *  reallocated)
 * @param ch The character to append to the buffer.
 * @warning This method may not be used for temporary buffers (buffers allocated
 *  with icalmemory_tmp_buffer() and related functions)!
 *
 * @par Error handling
 * Sets ::icalerrno to ::ICAL_BADARG_ERROR if @a buf, `*buf`, @a pos, `* pos`,
 * or @a buf_size `NULL`.
 *
 * This method will copy the character @a ch and a `'\0'` character after it to
 * the buffer @a buf starting at position @a pos, reallocing @a buf if it is too
 * small. @a buf_size is the size of @a buf and will be changed if @a buf is
 * reallocated. @a pos will point to the new terminating `'\0'` character @a
 * buf.
 *
 * @par Example
 * ```c
 * // creates a new buffer
 * int buffer_len = 15;
 * char *buffer = icalmemory_new_buffer(buffer_len);
 * strcpy(buffer, "My number is: ");
 *
 * // append a char to the buffer
 * int buffer_end = strlen(buffer);
 * char *buffer_end_pos = buffer[buffer_str_end];
 * icalmemory_append_char(&buffer, &buffer_end_pos, &buffer_len, '7');
 *
 * // print string
 * printf("%s\n", buffer);
 *
 * // release memory
 * icalmemory_free_buffer(buffer);
 * ```
 */
LIBICAL_ICAL_EXPORT void icalmemory_append_char(char **buf, char **pos, size_t *buf_size, char ch);

/**
 * @brief Creates a duplicate of a string.
 * @param s The string to duplicate.
 * @return A pointer to a string containing the same data as @a s
 *
 * @par Error handling
 * The string @a s must not be `NULL`, otherwise depending on the `libc` used,
 * it might lead to undefined behaviour (read: segfaults).
 *
 * @par Ownership
 * The returned string is owned by the caller and needs to be released with the
 * `icalmemory_free_buffer()` method.
 *
 * Replaces `strdup()`. The function uses icalmemory_new_buffer() for memory
 * allocation. It also helps trapping calls to `strdup()` and solves the
 * problem that in `-ansi`, `gcc` on Red Hat claims that `strdup()` is
 * undeclared.
 *
 * @par Usage
 * ```c
 * const char *my_str = "LibIcal";
 * char *dup = icalmemory_strdup(my_str);
 * printf("%s\n", dup);
 * icalmemory_free_buffer(dup);
 * ```
 */
LIBICAL_ICAL_EXPORT char *icalmemory_strdup(const char *s);

#endif /* !ICALMEMORY_H */
