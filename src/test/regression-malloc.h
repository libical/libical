/*======================================================================
FILE: regression-malloc.h

(C) COPYRIGHT 2018, Markus Minichmayr

This library is free software; you can redistribute it and/or modify
it under the terms of either:

The LGPL as published by the Free Software Foundation, version
2.1, available at: http://www.gnu.org/licenses/lgpl-2.1.html

Or:

The Mozilla Public License Version 2.0. You may obtain a copy of
the License at http://www.mozilla.org/MPL/
======================================================================*/

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

extern struct testmalloc_statistics global_testmalloc_statistics;

void *test_malloc(size_t size);
void *test_realloc(void* p, size_t size);
void test_free(void* p);

/** Resets the memory management statistics and sets the number of successful
  * allocations limit to infinite.
  */
void testmalloc_reset();

/** Sets the maximum number of malloc or realloc attemts that will succeed. If 
  * the number is negative, no limit will be applied.
  */
void testmalloc_set_max_successful_allocs(int n);
