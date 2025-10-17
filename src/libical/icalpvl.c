/*======================================================================
 FILE: icalpvl.c
 CREATOR: eric November, 1995

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 The original code is pvl.c
======================================================================*/

/*************************************************************************
 * WARNING: USE AT YOUR OWN RISK                                         *
 * These are library internal-only functions.                            *
 * Be warned that these functions can change at any time without notice. *
 *************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "icalpvl.h"

#include "icalmemory.h"

#include <assert.h>
#include <errno.h>

/* To mute a ThreadSanitizer claim */
#if (ICAL_SYNC_MODE == ICAL_SYNC_MODE_PTHREAD) && defined(THREAD_SANITIZER)
#include <pthread.h>
static pthread_mutex_t icalpvl_mutex = PTHREAD_MUTEX_INITIALIZER;

static void icalpvl_global_lock(void)
{
    pthread_mutex_lock(&icalpvl_mutex);
}

static void icalpvl_global_unlock(void)
{
    pthread_mutex_unlock(&icalpvl_mutex);
}
#else
#define icalpvl_global_lock()
#define icalpvl_global_unlock()
#endif

/**
 * Globals incremented for each call to icalpvl_new_element(); each list gets a unique id.
 */

static ICAL_GLOBAL_VAR int icalpvl_elem_count = 0;
static ICAL_GLOBAL_VAR int icalpvl_list_count = 0;

/**
 *  struct icalpvl_list_t
 *
 * The list structure. This is the handle for the entire list
 *
 * This type is private. Always use icalpvl_list instead.
 *
 */
typedef struct icalpvl_list_t {
    int MAGIC;                   /**< Magic Identifier */
    struct icalpvl_elem_t *head; /**< Head of list */
    struct icalpvl_elem_t *tail; /**< Tail of list */
    int count;                   /**< Number of items in the list */
    struct icalpvl_elem_t *p;    /**< Pointer used for iterators */
} icalpvl_list_t;

/**
 * struct icalpvl_elem_t
 *
 * The element structure.
 *
 * This type is private. Always use icalpvl_elem instead.
 */
typedef struct icalpvl_elem_t {
    int MAGIC;                    /**< Magic Identifier */
    void *d;                      /**< Pointer to data user is storing */
    struct icalpvl_elem_t *next;  /**< Next element */
    struct icalpvl_elem_t *prior; /**< Prior element */
} icalpvl_elem_t;

/**
 * @brief Creates a new list, clears the pointers and assigns a magic number
 *
 * @return  Pointer to the new list, 0 if there is no available memory.
 */

icalpvl_list icalpvl_newlist(void)
{
    struct icalpvl_list_t *L;

    if ((L = (struct icalpvl_list_t *)icalmemory_new_buffer(sizeof(struct icalpvl_list_t))) == 0) {
        errno = ENOMEM;
        return 0;
    }

    icalpvl_global_lock();
    L->MAGIC = icalpvl_list_count++;
    icalpvl_global_unlock();
    L->head = 0;
    L->tail = 0;
    L->count = 0;
    L->p = 0;

    return L;
}

void icalpvl_free(icalpvl_list l)
{
    struct icalpvl_list_t *L = (struct icalpvl_list_t *)l;

    icalpvl_clear(l);

    icalmemory_free_buffer(L);
}

/**
 * @brief Creates a new list element, assigns a magic number, and assigns
 * the next and previous pointers.
 *
 * Passing in the next and previous points may seem odd, but it allows the user
 * to set them while keeping the internal data hidden. In nearly all cases,
 * the user is the icalpvl library itself.
 *
 * @param d     The data item to be stored in the list
 * @param next  Pointer value to assign to the member "next"
 * @param prior Pointer value to assign to the member "prior"
 *
 * @return A pointer to the new element, 0 if there is no memory available.
 */

icalpvl_elem icalpvl_new_element(void *d, icalpvl_elem next, icalpvl_elem prior)
{
    struct icalpvl_elem_t *E;

    if ((E = (struct icalpvl_elem_t *)icalmemory_new_buffer(sizeof(struct icalpvl_elem_t))) == 0) {
        errno = ENOMEM;
        return 0;
    }

    icalpvl_global_lock();
    E->MAGIC = icalpvl_elem_count++;
    icalpvl_global_unlock();
    E->d = d;
    E->next = next;
    E->prior = prior;

    return (icalpvl_elem)E;
}

/**
 * @brief Add a new element to the from of the list
 *
 * @param L     The list to add the item to
 * @param d     Pointer to the item to add
 */

void icalpvl_unshift(icalpvl_list L, void *d)
{
    struct icalpvl_elem_t *E = icalpvl_new_element(d, L->head, 0);

    if (E->next != 0) {
        /* Link the head node to it */
        E->next->prior = E;
    }

    /* move the head */
    L->head = E;

    /* maybe move the tail */

    if (L->tail == 0) {
        L->tail = E;
    }

    L->count++;
}

/**
 * @brief Removes an element from the front of the list
 *
 * @param L     The list to operate on
 *
 * @return the entry on the front of the list
 */

void *icalpvl_shift(icalpvl_list L)
{
    if (L->head == 0) {
        return 0;
    }

    return icalpvl_remove(L, (void *)L->head);
}

/**
 * @brief Add a new item to the tail of the list
 *
 * @param L     The list to operate on
 * @param d     Pointer to the item to add
 *
 */

void icalpvl_push(icalpvl_list L, void *d)
{
    struct icalpvl_elem_t *E = icalpvl_new_element(d, 0, L->tail);

    /* These are done in icalpvl_new_element
       E->next = 0;
       E->prior = L->tail;
     */

    if (L->tail != 0) {
        L->tail->next = E;
    }

    if (L->head == 0) {
        L->head = E;
    }

    L->tail = E;

    L->count++;
}

/**
 * @brief Removes an element from the tail of the list
 *
 * @param L     The list to operate on
 */

void *icalpvl_pop(icalpvl_list L)
{
    if (L->tail == 0) {
        return 0;
    }

    return icalpvl_remove(L, (void *)L->tail);
}

/**
 * Add a new item to a list that is ordered by a comparison function.
 * This routine assumes that the list is properly ordered.
 *
 * @param L     The list to operate on
 * @param f     Pointer to a comparison function
 * @param d     Pointer to data to pass to the comparison function
 */

void icalpvl_insert_ordered(icalpvl_list L, icalpvl_comparef f, void *d)
{
    struct icalpvl_elem_t *P;

    L->count++;

    /* Empty list, add to head */

    if (L->head == 0) {
        icalpvl_unshift(L, d);
        return;
    }

    /* smaller than head, add to head */

    if (((*f)(d, L->head->d)) <= 0) {
        icalpvl_unshift(L, d);
        return;
    }

    /* larger than tail, add to tail */
    if ((*f)(d, L->tail->d) >= 0) {
        icalpvl_push(L, d);
        return;
    }

    /* Search for the first element that is smaller, and add before it */

    for (P = L->head; P != 0; P = P->next) {
        if ((*f)(P->d, d) >= 0) {
            icalpvl_insert_before(L, P, d);
            return;
        }
    }

    /* badness, just return */
}

/**
 * @brief Add a new item after the referenced element.
 * @param L     The list to operate on
 * @param P     The list element to add the item after
 * @param d     Pointer to the item to add.
 */

void icalpvl_insert_after(icalpvl_list L, icalpvl_elem P, void *d)
{
    struct icalpvl_elem_t *E = 0;

    L->count++;

    if (P == 0) {
        icalpvl_unshift(L, d);
        return;
    }

    if (P == L->tail) {
        E = icalpvl_new_element(d, 0, P);
        L->tail = E;
        E->prior->next = E;
    } else {
        E = icalpvl_new_element(d, P->next, P);
        E->next->prior = E;
        E->prior->next = E;
    }
}

/**
 * @brief Add an item after a referenced item
 *
 * @param L     The list to operate on
 * @param P     The list element to add the item before
 * @param d     Pointer to the data to be added.
 */

void icalpvl_insert_before(icalpvl_list L, icalpvl_elem P, void *d)
{
    struct icalpvl_elem_t *E = 0;

    L->count++;

    if (P == 0) {
        icalpvl_unshift(L, d);
        return;
    }

    if (P == L->head) {
        E = icalpvl_new_element(d, P, 0);
        E->next->prior = E;
        L->head = E;
    } else {
        E = icalpvl_new_element(d, P, P->prior);
        E->prior->next = E;
        E->next->prior = E;
    }
}

/**
 * @brief Removes the referenced item from the list.
 *
 * This routine will free the element, but not the data item that the
 * element contains.
 *
 * @param L     The list to operate on
 * @param E     The element to remove.
 */

void *icalpvl_remove(icalpvl_list L, icalpvl_elem E)
{
    void *data;

    if (E == L->head) {
        if (E->next != 0) {
            E->next->prior = 0;
            L->head = E->next;
        } else {
            /* E Also points to tail -> only one element in list */
            L->tail = 0;
            L->head = 0;
        }
    } else if (E == L->tail) {
        if (E->prior != 0) {
            E->prior->next = 0;
            L->tail = E->prior;
        } else {
            /* E points to the head, so it was the last element */
            /* This case should be taken care of in the previous clause */
            L->head = 0;
            L->tail = 0;
        }
    } else {
        E->prior->next = E->next;
        E->next->prior = E->prior;
    }

    L->count--;

    data = E->d;

    E->prior = 0;
    E->next = 0;
    E->d = 0;

    icalmemory_free_buffer(E);

    return data;
}

/**
 * @brief Return a pointer to data that satisfies a function.
 *
 * This routine will iterate through the entire list and call the
 * find function for each item. It will break and return a pointer to the
 * data that causes the find function to return 1.
 *
 * @param l     The list to operate on
 * @param f     Pointer to the find function
 * @param v     Pointer to constant data to pass into the function
 *
 * @return Pointer to the element that the find function found.
 */

icalpvl_elem icalpvl_find(icalpvl_list l, icalpvl_findf f, void *v)
{
    icalpvl_elem e;

    for (e = icalpvl_head(l); e != 0; e = icalpvl_next(e)) {
        if ((*f)(((struct icalpvl_elem_t *)e)->d, v) == 1) {
            /* Save this elem for a call to find_next */
            ((struct icalpvl_list_t *)l)->p = e;
            return e;
        }
    }

    return 0;
}

/**
 * @brief Like icalpvl_find(), but continues the search where the last find() or
 * find_next() left off.
 *
 * @param l     The list to operate on
 * @param f     Pointer to the find function
 * @param v     Pointer to constant data to pass into the function
 *
 * @return Pointer to the element that the find function found.
 */

icalpvl_elem pvl_find_next(icalpvl_list l, icalpvl_findf f, void *v)
{
    icalpvl_elem e;

    for (e = icalpvl_head(l); e != 0; e = icalpvl_next(e)) {
        if ((*f)(((struct icalpvl_elem_t *)e)->d, v) == 1) {
            /* Save this elem for a call to find_next */
            ((struct icalpvl_list_t *)l)->p = e;
            return e;
        }
    }

    return 0;
}

/**
 * @brief Removes the all the elements in the list. The does not free
 * the data items the elements hold.
 */

void icalpvl_clear(icalpvl_list l)
{
    icalpvl_elem e = icalpvl_head(l);
    icalpvl_elem next;

    if (e == 0) {
        return;
    }

    while (e != 0) {
        next = icalpvl_next(e);
        (void)icalpvl_remove(l, e);
        e = next;
    }
}

/**
 * @brief Returns the number of items in the list.
 */

int icalpvl_count(icalpvl_list L)
{
    return L->count;
}

/**
 * @brief Returns a pointer to the given element
 */

icalpvl_elem icalpvl_next(icalpvl_elem E)
{
    if (E == 0) {
        return 0;
    }

    return (icalpvl_elem)E->next;
}

/**
 * @brief Returns a pointer to the element previous to the element given.
 */

icalpvl_elem icalpvl_prior(icalpvl_elem E)
{
    return (icalpvl_elem)E->prior;
}

/**
 * @brief Returns a pointer to the first item in the list.
 */

icalpvl_elem icalpvl_head(icalpvl_list L)
{
    return (icalpvl_elem)L->head;
}

/**
 * @brief Returns a pointer to the last item in the list.
 */
icalpvl_elem icalpvl_tail(icalpvl_list L)
{
    return (icalpvl_elem)L->tail;
}

void *icalpvl_data(icalpvl_elem E)
{
    if (E == 0) {
        return 0;
    }

    return E->d;
}

/**
 * @brief Call a function for every item in the list.
 *
 * @param l     The list to operate on
 * @param f     Pointer to the function to call
 * @param v     Data to pass to the function on every iteration
 */

void icalpvl_apply(icalpvl_list l, icalpvl_applyf f, void *v)
{
    icalpvl_elem e;

    for (e = icalpvl_head(l); e != 0; e = icalpvl_next(e)) {
        (*f)(((struct icalpvl_elem_t *)e)->d, v);
    }
}
