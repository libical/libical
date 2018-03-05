/*======================================================================
FILE: regression-malloc.c

(C) COPYRIGHT 2018, Markus Minichmayr

This library is free software; you can redistribute it and/or modify
it under the terms of either:

The LGPL as published by the Free Software Foundation, version
2.1, available at: http://www.gnu.org/licenses/lgpl-2.1.html

Or:

The Mozilla Public License Version 2.0. You may obtain a copy of
the License at http://www.mozilla.org/MPL/
======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libical/ical.h"

#include "regression.h"
#include "regression-malloc.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>



struct testmalloc_statistics global_testmalloc_statistics;
static int global_testmalloc_remainint_attempts = -1;

#define TESTMALLOC_MAGIC_NO 0x1234abcd
struct testmalloc_hdr {
	uint32_t magic_no;
	size_t size;
};

void *test_malloc(size_t size) {

	void* block;
	struct testmalloc_hdr* hdr;

	global_testmalloc_statistics.malloc_cnt++;
	if (global_testmalloc_remainint_attempts == 0) {
		global_testmalloc_statistics.malloc_failed_cnt++;
		return NULL;
	}

	block = malloc(size + sizeof(struct testmalloc_hdr));
	if (block == NULL) {
		global_testmalloc_statistics.malloc_failed_cnt++;
		return NULL;
	}

	hdr = (struct testmalloc_hdr*) block;
	hdr->magic_no = TESTMALLOC_MAGIC_NO;
	hdr->size = size;

	global_testmalloc_statistics.mem_allocated_current += size;
	if (global_testmalloc_statistics.mem_allocated_current > global_testmalloc_statistics.mem_allocated_max)
		global_testmalloc_statistics.mem_allocated_max = global_testmalloc_statistics.mem_allocated_current;

	global_testmalloc_statistics.blocks_allocated++;

	if (global_testmalloc_remainint_attempts > 0)
		global_testmalloc_remainint_attempts--;

	return (void*) (hdr + 1);
}

void *test_realloc(void* p, size_t size) {

	struct testmalloc_hdr* hdr;
	size_t old_size;

	global_testmalloc_statistics.realloc_cnt++;
	if (global_testmalloc_remainint_attempts == 0) {
		global_testmalloc_statistics.realloc_failed_cnt++;
		return NULL;
	}

	if (p == NULL) {
		global_testmalloc_statistics.realloc_failed_cnt++;
		return NULL;
	}

	hdr = ((struct testmalloc_hdr*) p) - 1;
	if (hdr->magic_no != TESTMALLOC_MAGIC_NO) {
		global_testmalloc_statistics.realloc_failed_cnt++;
		return NULL;
	}

	old_size = hdr->size;
	hdr->magic_no = 0;
	hdr = (struct testmalloc_hdr*) realloc(hdr, size + sizeof(struct testmalloc_hdr));
	if (hdr == NULL) {
		global_testmalloc_statistics.realloc_failed_cnt++;
		return NULL;
	}

	hdr->magic_no = TESTMALLOC_MAGIC_NO;
	hdr->size = size;

	global_testmalloc_statistics.mem_allocated_current += size - old_size;
	if (global_testmalloc_statistics.mem_allocated_current > global_testmalloc_statistics.mem_allocated_max)
		global_testmalloc_statistics.mem_allocated_max = global_testmalloc_statistics.mem_allocated_current;

	if (global_testmalloc_remainint_attempts > 0)
		global_testmalloc_remainint_attempts--;

	return (void*) (hdr + 1);
}

void test_free(void* p) {

	struct testmalloc_hdr* hdr;
	size_t old_size;

	if (p == NULL)
		return;

	global_testmalloc_statistics.free_cnt++;

	hdr = ((struct testmalloc_hdr*) p) - 1;
	if (hdr->magic_no != TESTMALLOC_MAGIC_NO) {
        // assert may not have any effect in release mode
        assert(hdr->magic_no == TESTMALLOC_MAGIC_NO);
        ok("free consistent", hdr->magic_no == TESTMALLOC_MAGIC_NO);
		global_testmalloc_statistics.free_failed_cnt++;
		return;
	}

	old_size = hdr->size;
	hdr->magic_no = 0;

	free(hdr);

	global_testmalloc_statistics.mem_allocated_current -= old_size;
	global_testmalloc_statistics.blocks_allocated--;
}



void testmalloc_reset() {
	memset(&global_testmalloc_statistics, 0, sizeof(global_testmalloc_statistics));
	global_testmalloc_remainint_attempts = -1;
}

/** Sets the maximum number of malloc or realloc attemts that will succeed. If
* the number is negative, no limit will be applied. */
void testmalloc_set_max_successful_allocs(int n) {
	global_testmalloc_remainint_attempts = n;
}
