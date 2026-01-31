/*
 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 The Original Code is eric. The Initial Developer of the Original
 Code is Eric Busboom
*/

/**
 @file icalset.h
 @author eric 28 November 1999

 @brief Icalset is the "base class" for representations of a collection of iCal components.

 Derived classes (actually delegatees) include:

    icalfileset   Store components in a single file
    icaldirset    Store components in multiple files in a directory
    icalbdbset    Store components in a Berkeley DB File
    icalheapset   Store components on the heap
    icalmysqlset  Store components in a mysql database.
*/

#ifndef ICALSET_H
#define ICALSET_H

#include "libical_icalss_export.h"
#include "icalgauge.h"
#include "icalcomponent.h"
#include "icalerror.h"

typedef struct icalset_impl icalset;

/**
 * The kinds of supported sets.
 */
typedef enum icalset_kind
{
    ICAL_FILE_SET, /**< file based */
    ICAL_DIR_SET,  /**< directory based */
    ICAL_BDB_SET   /**< Berkely database based */
} icalset_kind;

struct icalcompiter;
typedef struct icalsetiter {
    icalcompiter iter; /* icalcomponent_kind, icalpvl_elem iter */
    icalgauge *gauge;
    icalrecur_iterator *ritr;      /*the last iterator */
    icalcomponent *last_component; /*the pending recurring component to be processed  */
    const char *tzid;              /* the calendar's timezone id */
} icalsetiter;

struct icalset_impl {
    icalset_kind kind;
    size_t size;
    char *dsn;
    icalset *(*init)(icalset *set, const char *dsn, void *options);
    void (*free)(icalset *set);
    const char *(*path)(icalset *set);
    void (*mark)(icalset *set);
    icalerrorenum (*commit)(icalset *set);
    icalerrorenum (*add_component)(icalset *set, icalcomponent *comp);
    icalerrorenum (*remove_component)(icalset *set, icalcomponent *comp);
    int (*count_components)(icalset *set, icalcomponent_kind kind);
    icalerrorenum (*select)(icalset *set, icalgauge *gauge);
    void (*clear)(icalset *set);
    icalcomponent *(*fetch)(icalset *set, icalcomponent_kind kind, const char *uid);
    icalcomponent *(*fetch_match)(icalset *set, const icalcomponent *comp);
    int (*has_uid)(icalset *set, const char *uid);
    icalerrorenum (*modify)(icalset *set, icalcomponent *old, icalcomponent *newc);
    icalcomponent *(*get_current_component)(icalset *set);
    icalcomponent *(*get_first_component)(icalset *set);
    icalcomponent *(*get_next_component)(icalset *set);
    icalsetiter (*icalset_begin_component)(icalset *set,
                                           icalcomponent_kind kind, icalgauge *gauge,
                                           const char *tzid);
    icalcomponent *(*icalsetiter_to_next)(icalset *set, icalsetiter *i);
    icalcomponent *(*icalsetiter_to_prior)(icalset *set, icalsetiter *i);
};

/**
 * Registers a new derived class.
 *
 * @param set a pointer to a valid icalset to register.
 *
 * @return true if the registration succeeded; false otherwise.
 */
LIBICAL_ICALSS_EXPORT bool icalset_register_class(icalset *set);

/**
 * Generic icalset constructor.
 *
 * @param kind the type of icalset to create
 * @param dsn the data Source Name - usually a pathname or DB handle
 * @param options any implementation specific options
 *
 * @return a valid icalset reference or NULL if error.
 *
 * This creates any of the icalset types available.
 */

LIBICAL_ICALSS_EXPORT icalset *icalset_new(icalset_kind kind, const char *dsn, void *options);

LIBICAL_ICALSS_EXPORT icalset *icalset_new_file(const char *path);

LIBICAL_ICALSS_EXPORT icalset *icalset_new_file_reader(const char *path);

LIBICAL_ICALSS_EXPORT icalset *icalset_new_file_writer(const char *path);

LIBICAL_ICALSS_EXPORT icalset *icalset_new_dir(const char *path);

/**
 * Frees the memory associated with this icalset
 * automatically calls the implementation specific free routine
 */
LIBICAL_ICALSS_EXPORT void icalset_free(icalset *set);

LIBICAL_ICALSS_EXPORT const char *icalset_path(icalset *set);

/**
 * Marks the cluster as changed, so it will be written to disk when it is freed.
 **/
LIBICAL_ICALSS_EXPORT void icalset_mark(icalset *set);

/**
 * Writes changes to disk immediately.
 */
LIBICAL_ICALSS_EXPORT icalerrorenum icalset_commit(icalset *set);

LIBICAL_ICALSS_EXPORT icalerrorenum icalset_add_component(icalset *set, icalcomponent *comp);

LIBICAL_ICALSS_EXPORT icalerrorenum icalset_remove_component(icalset *set, icalcomponent *comp);

LIBICAL_ICALSS_EXPORT int icalset_count_components(icalset *set, icalcomponent_kind kind);

/**
 * Restricts the component returned by icalset_first, _next to those that pass the gauge.
 */
LIBICAL_ICALSS_EXPORT icalerrorenum icalset_select(icalset *set, icalgauge *gauge);

/**
 * Gets a component by uid.
 */
LIBICAL_ICALSS_EXPORT icalcomponent *icalset_fetch(icalset *set, const char *uid);

LIBICAL_ICALSS_EXPORT int icalset_has_uid(icalset *set, const char *uid);

LIBICAL_ICALSS_EXPORT icalcomponent *icalset_fetch_match(icalset *set, const icalcomponent *c);

/**
 * Modifies components according to the MODIFY method of CAP. Works on the
 * currently selected components.
 */
LIBICAL_ICALSS_EXPORT icalerrorenum icalset_modify(icalset *set,
                                                   icalcomponent *oldc, icalcomponent *newc);

/**
 * Iterates through the components. If a gauge has been defined, these will skip
 * over components that do not pass the gauge.
 */

/**
 * Gets the current component from the specified icalset.
 *
 * @param set is a pointer to a valid icalset
 *
 * @return a pointer to the current icalcomponent for the @p set; or NULL if there is none.
 */
LIBICAL_ICALSS_EXPORT icalcomponent *icalset_get_current_component(icalset *set);

/**
 * Gets the first component from the specified icalset.
 *
 * @param set is a pointer to a valid icalset
 *
 * @return a pointer to the first icalcomponent of the @p set; or NULL if there is none.
 */
LIBICAL_ICALSS_EXPORT icalcomponent *icalset_get_first_component(icalset *set);

/**
 * Gets the next component of the specified icalset.
 *
 * @param set is a pointer to a valid icalset
 *
 * @return a pointer to the next icalcomponent of the @p set; or NULL if there is none.
 */
LIBICAL_ICALSS_EXPORT icalcomponent *icalset_get_next_component(icalset *set);

/** External Iterator with gauge - for thread safety */
LIBICAL_ICALSS_EXPORT extern icalsetiter icalsetiter_null;

LIBICAL_ICALSS_EXPORT icalsetiter icalset_begin_component(icalset *set,
                                                          icalcomponent_kind kind,
                                                          icalgauge *gauge, const char *tzid);

/** Default _next, _prior, _deref for subclasses that use single cluster */
LIBICAL_ICALSS_EXPORT icalcomponent *icalsetiter_next(icalsetiter *i);

LIBICAL_ICALSS_EXPORT icalcomponent *icalsetiter_prior(icalsetiter *i);

LIBICAL_ICALSS_EXPORT icalcomponent *icalsetiter_deref(icalsetiter *i);

/** for subclasses that use multiple clusters that require specialized cluster traversal */

LIBICAL_ICALSS_EXPORT icalcomponent *icalsetiter_to_next(icalset *set, icalsetiter *i);

LIBICAL_ICALSS_EXPORT icalcomponent *icalsetiter_to_prior(icalset *set, icalsetiter *i);

#endif /* !ICALSET_H */
