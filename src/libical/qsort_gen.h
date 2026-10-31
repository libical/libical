/*======================================================================
 FILE: qsort_gen.h

 SPDX-FileCopyrightText: 2018, Markus Minichmayr <markus@tapkey.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 The Initial Developer of the Original Code is Markus Minichmayr.
======================================================================*/

#ifndef QSORT_GEN_H
#define QSORT_GEN_H

#include <stddef.h>

/**
 * @file qsort_gen.h
 * @brief An implementation of qsort that is more flexible than the version
 * provided with stdlib.
 *
 * In contrast to the qsort provided with stdlib, this version doesn't assume
 * that the data to be sorted is stored in a contiguous block of memory.
 */

/**
 * @brief Sort an arbitrary list of items using the qsort algorithm.
 * interpreted by this function but passed to the compar and swapr functions.
 * @param nitems The number of items in the list.
 * @param compar The comparator function. The function receives the pointer
 * to the list to be sorted and the indices of the elements to be compared.
 * @param swapr The function used to swap two elements within the list. The
 * function receives the pointer to the list to be sorted and the indices of
 * the elements to be compared.
 */
void qsort_gen(void *list, size_t nitems,
               int (*compar)(const void *, size_t, size_t),
               void (*swapr)(void *, size_t, size_t));

/**
 * @brief Swaps two arbitrary blocks of memory.
 * @param m1 Pointer to the first block of memory.
 * @param m2 Pointer to the second block of memory.
 * @param size Size of the memory blocks to be swapped.
 */
void qsort_gen_memswap(void *m1, void *m2, size_t size);

#endif /* QSORT_GEN_H */
