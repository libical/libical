/*======================================================================
 FILE: icalpvl.h
 CREATOR: eric November, 1995

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 The original code is pvl.h
======================================================================*/

/*************************************************************************
 * WARNING: USE AT YOUR OWN RISK                                         *
 * These are library internal-only functions.                            *
 * Be warned that these functions can change at any time without notice. *
 *************************************************************************/

#ifndef ICAL_PVL_P_H
#define ICAL_PVL_P_H

#include "libical_ical_export.h"

typedef struct icalpvl_list_t *icalpvl_list;
typedef struct icalpvl_elem_t *icalpvl_elem;

/* Create new lists or elements */
LIBICAL_ICAL_EXPORT icalpvl_elem icalpvl_new_element(void *d, icalpvl_elem next, icalpvl_elem prior);

LIBICAL_ICAL_EXPORT icalpvl_list icalpvl_newlist(void);

LIBICAL_ICAL_EXPORT void icalpvl_free(icalpvl_list);

/* Add, remove, or get the head of the list */
LIBICAL_ICAL_EXPORT void icalpvl_unshift(icalpvl_list l, void *d);

LIBICAL_ICAL_EXPORT void *icalpvl_shift(icalpvl_list l);

LIBICAL_ICAL_EXPORT icalpvl_elem icalpvl_head(icalpvl_list);

/* Add, remove or get the tail of the list */
LIBICAL_ICAL_EXPORT void icalpvl_push(icalpvl_list l, void *d);

LIBICAL_ICAL_EXPORT void *icalpvl_pop(icalpvl_list l);

LIBICAL_ICAL_EXPORT icalpvl_elem icalpvl_tail(icalpvl_list);

/* Insert elements in random places */
typedef int (*icalpvl_comparef)(void *a, void *b); /* a, b are of the data type */

LIBICAL_ICAL_EXPORT void icalpvl_insert_ordered(icalpvl_list l, icalpvl_comparef f, void *d);

LIBICAL_ICAL_EXPORT void icalpvl_insert_after(icalpvl_list l, icalpvl_elem p, void *d);

LIBICAL_ICAL_EXPORT void icalpvl_insert_before(icalpvl_list l, icalpvl_elem p, void *d);

/* Remove an element, or clear the entire list */
LIBICAL_ICAL_EXPORT void *icalpvl_remove(icalpvl_list, icalpvl_elem); /* Remove element, return data */

LIBICAL_ICAL_EXPORT void icalpvl_clear(icalpvl_list); /* Remove all elements, de-allocate all data */

LIBICAL_ICAL_EXPORT int icalpvl_count(icalpvl_list);

/* Navigate the list */
LIBICAL_ICAL_EXPORT icalpvl_elem icalpvl_next(icalpvl_elem e);

LIBICAL_ICAL_EXPORT icalpvl_elem icalpvl_prior(icalpvl_elem e);

/* get the data in the list */
LIBICAL_ICAL_EXPORT void *icalpvl_data(icalpvl_elem);

/* Find an element for which a function returns true */
typedef int (*icalpvl_findf)(void *a, void *b); /*a is list elem, b is other data */

LIBICAL_ICAL_EXPORT icalpvl_elem icalpvl_find(icalpvl_list l, icalpvl_findf f, void *v);

LIBICAL_ICAL_EXPORT icalpvl_elem icalpvl_find_next(icalpvl_list l, icalpvl_findf f, void *v);

/**
 * Pass each element in the list to a function
 * a is list elem, b is other data
 */
typedef void (*icalpvl_applyf)(void *a, void *b);

LIBICAL_ICAL_EXPORT void icalpvl_apply(icalpvl_list l, icalpvl_applyf f, void *v);

#endif /* ICAL_PVL_P_H */
