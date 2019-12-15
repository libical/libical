/*======================================================================
 FILE: qsort_gen.c

 This library is free software; you can redistribute it and/or modify
 it under the terms of either:

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: https://www.gnu.org/licenses/lgpl-2.1.html

 Or:

    The Mozilla Public License Version 2.0. You may obtain a copy of
    the License at https://www.mozilla.org/MPL/

 The code in this file was initially authored as part of the PDCLib project
 and placed in the public domain. The initial copyright notice was as follows:

    "This file is part of the Public Domain C Library (PDCLib).
    Permission is granted to use, modify, and / or redistribute at will.

    This implementation is taken from Paul Edward's PDPCLIB.

    Original code is credited to Raymond Gardner, Englewood CO.
    Minor mods are credited to Paul Edwards.
    Some reformatting and simplification done by Martin Baute."

 Modifications have been made by Markus Minichmayr to allow more generic use.
======================================================================*/

#include <stdlib.h>

/* Wrapper for _PDCLIB_memswp protects against multiple argument evaluation. */
void qsort_gen_memswap(void *m1, void *m2, size_t size)
{
    char *p1 = (char *)m1;
    char *p2 = (char *)m2;
    char tmp;

    while (size) {
        tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        size--;
        p1++;
        p2++;
    }
}

/* For small sets, insertion sort is faster than quicksort.
   T is the threshold below which insertion sort will be used.
   Must be 3 or larger.
*/
#define T 7

/* Macros for handling the QSort stack */
#define PREPARE_STACK size_t stack[STACKSIZE]; size_t* stackptr = stack
#define PUSH(base, limit) stackptr[0] = base; stackptr[1] = limit; stackptr += 2
#define POP(base, limit) stackptr -= 2; base = stackptr[0]; limit = stackptr[1]
/* TODO: Stack usage is log2(nmemb) (minus what T shaves off the worst case).
         Worst-case nmemb is platform dependent and should probably be
         configured.
*/
#define STACKSIZE 64

void qsort_gen(void *list, size_t nitems,
               int(*compar)(const void *, size_t, size_t),
               void(*swapr)(void *, size_t, size_t))
{
    size_t i;
    size_t j;
    size_t base_          = 0;
    size_t limit          = nitems;
    PREPARE_STACK;

    for (;;) {
        if ((limit - base_) > T) { /* QSort for more than T elements. */
            /* We work from second to last - first will be pivot element. */
            i = base_ + 1;
            j = limit - 1;
            /* We swap first with middle element, then sort that with second
               and last element so that eventually first element is the median
               of the three - avoiding pathological pivots.
               TODO: Instead of middle element, chose one randomly.
            */
            swapr(list, (limit - base_) / 2 + base_, base_);
            if (compar(list, i, j) > 0) {
                swapr(list, i, j);
            }
            if (compar(list, base_, j) > 0) {
                swapr(list, base_, j);
            }
            if (compar(list, i, base_) > 0) {
                swapr(list, i, base_);
            }

            /* Now we have the median for pivot element, entering main Quicksort. */
            for (;;) {
                do {
                    /* move i right until *i >= pivot */
                    i ++;
                } while (compar(list, i, base_) < 0);
                do {
                    /* move j left until *j <= pivot */
                    j --;
                } while (compar(list, j, base_) > 0);
                if (i > j) {
                    /* break loop if pointers crossed */
                    break;
                }
                /* else swap elements, keep scanning */
                swapr(list, i, j);
            }
            /* move pivot into correct place */
            swapr(list, base_, j);
            /* larger subfile base / limit to stack, sort smaller */
            if (j - base_ > limit - i) {
                /* left is larger */
                PUSH(base_, j);
                base_ = i;
            } else {
                /* right is larger */
                PUSH(i, limit);
                limit = j;
            }
        } else {
            /* insertion sort for less than T elements */
            for (j = base_, i = j + 1; i < limit; j = i, i++) {
                for (; compar(list, j, j + 1) > 0; j --) {
                    swapr(list, j, j + 1);
                    if (j == base_) {
                        break;
                    }
                }
            }
            if (stackptr != stack) {
                /* if any entries on stack  */
                POP(base_, limit);
            } else {
                /* else stack empty, done   */
                break;
            }
        }
    }
}
