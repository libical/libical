/*======================================================================
FILE: test-malloc.h

SPDX-FileCopyrightText: 2018-2022, Markus Minichmayr <markus@tapkey.com>
SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
======================================================================*/

#ifndef TESTMALLOC_H
#define TESTMALLOC_H

#include <stddef.h>
#include <stdint.h>

struct testmalloc_statistics {
    int malloc_cnt;
    int realloc_cnt;
    int free_cnt;

    int malloc_failed_cnt;
    int realloc_failed_cnt;
    int free_failed_cnt;

    size_t mem_allocated_max;
    size_t mem_allocated_current;
    int blocks_allocated;
};

/** Allocates the specified amount of memory and returns a pointer to the allocated memory.
  * Memory allocated using this function must be freed using test_free().
  * The number of allocations that can be made using this function can be limited via
  * testmalloc_set_max_successful_allocs().
  */
void *test_malloc(size_t size);

/** Resizes the specified buffer.
 * Can only be used with memory that has previously been allocated using test_malloc().
 */
void *test_realloc(void *p, size_t size);

/** Frees a block of memory that has previously been allocated via the test_malloc() function. Specifying memory that
 * has not been allocated via test_malloc() causes an assertion.
 */
void test_free(void *p);

/** Resets the memory management statistics and sets the number of successful
  * allocations limit to infinite.
  */
void testmalloc_reset(void);

/** Sets the maximum number of malloc or realloc attempts that will succeed. If
  * the number is negative, no limit will be applied.
  */
void testmalloc_set_max_successful_allocs(int n);

/** Gets current memory allocation statistics. */
void testmalloc_get_statistics(struct testmalloc_statistics *statistics);

#endif /* !TESTMALLOC_H */
