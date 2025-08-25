/*======================================================================
 FILE: test-malloc.c

 SPDX-FileCopyrightText: 2018-2022, Markus Minichmayr <markus@tapkey.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wanalyzer-malloc-leak"
#endif

#include "test-malloc.h"
#include "icalerror.h"
#if !defined(MEMORY_CONSISTENCY)
#include "regression.h"
#endif

#include <stdlib.h>
#include <string.h>

struct testmalloc_statistics global_testmalloc_statistics;
static int global_testmalloc_remaining_attempts = -1;

#define TESTMALLOC_MAGIC_NO 0x1234abcd
struct testmalloc_hdr {
    uint32_t magic_no;
    size_t size;
};

struct testmalloc_hdrlayout {
    struct testmalloc_hdr hdr;
    int data;
};

#define TESTMALLOC_HDR_SIZE ((size_t)&((struct testmalloc_hdrlayout *)0)->data)

void *test_malloc(size_t size)
{
    void *block;
    struct testmalloc_hdr *hdr;

    global_testmalloc_statistics.malloc_cnt++;
    if (global_testmalloc_remaining_attempts == 0) {
        global_testmalloc_statistics.malloc_failed_cnt++;
        return NULL;
    }

    block = malloc(size + TESTMALLOC_HDR_SIZE);
    if (block == NULL) {
        global_testmalloc_statistics.malloc_failed_cnt++;
        return NULL;
    }

    hdr = (struct testmalloc_hdr *)block;
    hdr->magic_no = TESTMALLOC_MAGIC_NO;
    hdr->size = size;

    global_testmalloc_statistics.mem_allocated_current += size;
    if (global_testmalloc_statistics.mem_allocated_current > global_testmalloc_statistics.mem_allocated_max) {
        global_testmalloc_statistics.mem_allocated_max = global_testmalloc_statistics.mem_allocated_current;
    }

    global_testmalloc_statistics.blocks_allocated++;

    if (global_testmalloc_remaining_attempts > 0) {
        global_testmalloc_remaining_attempts--;
    }

    return (void *)&((struct testmalloc_hdrlayout *)hdr)->data;
}

void *test_realloc(void *p, size_t size)
{
    struct testmalloc_hdr *hdr;
    size_t old_size;

    global_testmalloc_statistics.realloc_cnt++;
    if (global_testmalloc_remaining_attempts == 0) {
        global_testmalloc_statistics.realloc_failed_cnt++;
        return NULL;
    }

    if (p == NULL) {
        global_testmalloc_statistics.realloc_failed_cnt++;
        return NULL;
    }

    hdr = (struct testmalloc_hdr *)(((uint8_t *)p) - TESTMALLOC_HDR_SIZE);
    if (hdr->magic_no != TESTMALLOC_MAGIC_NO) {
        global_testmalloc_statistics.realloc_failed_cnt++;
        return NULL;
    }

    old_size = hdr->size;
    hdr->magic_no = 0;

    // cppcheck-suppress memleakOnRealloc; the mem block p passed to this function stays valid.
    hdr = (struct testmalloc_hdr *)realloc(hdr, size + TESTMALLOC_HDR_SIZE);
    if (hdr == NULL) {
        global_testmalloc_statistics.realloc_failed_cnt++;
        return NULL;
    }

    hdr->magic_no = TESTMALLOC_MAGIC_NO;
    hdr->size = size;

    global_testmalloc_statistics.mem_allocated_current += size - old_size;
    if (global_testmalloc_statistics.mem_allocated_current > global_testmalloc_statistics.mem_allocated_max) {
        global_testmalloc_statistics.mem_allocated_max = global_testmalloc_statistics.mem_allocated_current;
    }

    if (global_testmalloc_remaining_attempts > 0) {
        global_testmalloc_remaining_attempts--;
    }

    return (void *)&((struct testmalloc_hdrlayout *)hdr)->data;
}

void test_free(void *p)
{
    struct testmalloc_hdr *hdr;
    size_t old_size;

    if (p == NULL) {
        return;
    }

    global_testmalloc_statistics.free_cnt++;

    hdr = (struct testmalloc_hdr *)(((uint8_t *)p) - TESTMALLOC_HDR_SIZE);

    // The main objective of this check is to ensure, that only memory, that has been allocated via icalmemory is freed
    // via icalmemory_free(). A side objective is to make sure, the block of memory hasn't been corrupted.
    if (hdr->magic_no != TESTMALLOC_MAGIC_NO) {
        // If we end up here, then probably either of the following happened:
        // * The calling code tries to free a block of memory via icalmemory_free() that has been allocated outside of
        //   icalmemory, e.g. via malloc().
        // * The header in front of the memory block being freed has been corrupted.

#if !defined(MEMORY_CONSISTENCY)
        ok("freed memory was allocated via icalmemory and has not been corrupted",
           hdr->magic_no == TESTMALLOC_MAGIC_NO);
#endif
        icalerror_assert(hdr->magic_no == TESTMALLOC_MAGIC_NO,
                         "freed memory was allocated via icalmemory and has been corrupted");
        global_testmalloc_statistics.free_failed_cnt++;
        return;
    }

    old_size = hdr->size;
    hdr->magic_no = 0;

    free(hdr);

    global_testmalloc_statistics.mem_allocated_current -= old_size;
    global_testmalloc_statistics.blocks_allocated--;
}

void testmalloc_reset(void)
{
    memset(&global_testmalloc_statistics, 0, sizeof(global_testmalloc_statistics));
    global_testmalloc_remaining_attempts = -1;
}

/** Sets the maximum number of malloc or realloc attempts that will succeed. If
* the number is negative, no limit will be applied. */
void testmalloc_set_max_successful_allocs(int n)
{
    global_testmalloc_remaining_attempts = n;
}

void testmalloc_get_statistics(struct testmalloc_statistics *statistics)
{
    if (statistics) {
        *statistics = global_testmalloc_statistics;
    }
}

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
