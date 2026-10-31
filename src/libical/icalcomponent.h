/*======================================================================
 FILE: icalcomponent.h
 CREATOR: eric 20 March 1999

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
======================================================================*/

/**
 * @file icalcomponent.h
 * @brief Defines the data structure for iCalendar components
 */

#ifndef ICALCOMPONENT_H
#define ICALCOMPONENT_H

#include "libical_sentinel.h"
#include "libical_ical_export.h"
#include "icalenums.h" /* Defines icalcomponent_kind */
#include "icalproperty.h"

typedef struct icalcomponent_impl icalcomponent;

/* These are exposed so that callers will not have to allocate and
   deallocate iterators. Pretend that you can't see them. */
/// @cond PRIVATE
struct icalpvl_elem_t;
typedef struct icalcompiter {
    icalcomponent_kind kind;
    struct icalpvl_elem_t *iter;
} icalcompiter;

typedef struct icalpropiter {
    icalproperty_kind kind;
    struct icalpvl_elem_t *iter;
} icalpropiter;
/// @endcond

/**
 * Constructs a new icalcomponent of a specified kind.
 *
 * The data structure contains all null values and is essentially invalid.
 * Free the allocated memory with icalcomponent_free.
 *
 * @param kind is the icalcompent_kind to construct
 *
 * @return a pointer to a valid icalcomponent.
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_new(icalcomponent_kind kind);

/**
 * Deeply clones an icalcomponent.
 *
 * Free the allocated memory with icalcomponent_free.
 *
 * @param old a pointer to a valid icalcomponent
 *
 * @return a pointer to the memory for the newly cloned icalcomponent.
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_clone(const icalcomponent *old);

/**
 * Construct a new icalcomponent from a character string.
 *
 * Free the allocated memory with icalcomponent_free.
 *
 * @param str a char string containing a properly formatted ICS calendar.
 *
 * @return a pointer to an icalcomponent or NULL if an anomaly was encountered.
 * To handle parse errors, set the ::icalparser_state to ::ICALPARSER_ERROR
 * and/or return components of the type ::ICAL_XLICINVALID_COMPONENT,
 * or components with properties of the type ICAL_XLICERROR_PROPERTY.
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_new_from_string(const char *str);

/**
 * Construct a new icalcomponent from a list of icalproperties of icalcomponents.
 *
 * Free the allocated memory with icalcomponent_free.
 * Make sure to pass NULL (not 0) as the final argument!
 *
 * @param kind the icalcomponent_kind of icalcomponent to use
 *
 * @return a pointer to an icalcomponent. NULL .
 */
LIBICAL_ICAL_EXPORT LIBICAL_SENTINEL icalcomponent *icalcomponent_vanew(icalcomponent_kind kind, ...);

/**
 * Construct a new X-NAME icalcomponent.
 *
 * Free the allocated memory with icalcomponent_free.
 *
 * @param x_name a pointer to a char string containing the value for the "X-NAME"
 *
 * @return a pointer to a valid icalcomponent.
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_new_x(const char *x_name);

/**
 * Construct a new icalcomponent with an IANA component name.
 *
 * @param iana_name a pointer to a char string containing the IANA name.
 *
 * @return A pointer to a new icalcomponent; or NULL on memory allocation failure.
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_new_iana(const char *iana_name);

/**
 * Destruct the specified icalcomponent.
 *
 * Frees the memory alloced for the specified icalcomponent.
 *
 * @param component a pointer to a valid icalcompent
 */
LIBICAL_ICAL_EXPORT void icalcomponent_free(icalcomponent *component);

/**
 * Creates a character string representation of an icalcomponent.
 *
 * @param component a pointer to a icalcomponent
 *
 * @return a pointer to a char string containing the @p component string
 * representation; NULL if a memory allocation failure is encountered.
 *
 * @see icalcomponent_as_ical_string_r
 */
LIBICAL_ICAL_EXPORT char *icalcomponent_as_ical_string(const icalcomponent *component);

/**
 * Creates a character string representation of an icalcomponent.
 *
 * @param component a pointer to a icalcomponent
 *
 * @return a pointer to a char string containing the @p component string
 * representation; NULL if a memory allocation failure is encountered.
 *
 * @see icalcomponent_as_ical_string
 */
LIBICAL_ICAL_EXPORT char *icalcomponent_as_ical_string_r(const icalcomponent *component);

/**
 * Determines if the specified icalcomponent is valid.
 *
 * @param component a pointer to possibly invalid icalcomponent
 *
 * @return true if @p component is valid; false otherwise.
 */
LIBICAL_ICAL_EXPORT bool icalcomponent_is_valid(const icalcomponent *component);

/**
 * Determines the icalcomponent_kind of the specified icalcomponent.
 *
 * @param component a pointer to valid icalcomponent.
 *
 * @return the icalcomponent_kind of @p component.
 */
LIBICAL_ICAL_EXPORT icalcomponent_kind icalcomponent_isa(const icalcomponent *component);

/**
 * Determines if the specified pointer is an icalcomponent pointer.
 *
 * Just looks if the ::id member strncmps to "comp".
 *
 * @param component a pointer, presumably to an icalcomponent.
 *
 * @return true if the pointer looks like an icalcomponent; false otherwise.
 */
LIBICAL_ICAL_EXPORT bool icalcomponent_isa_component(const void *component);

/* Deal with X components */

/**
 * Sets the X-NAME property for the specified icalcomponent.
 *
 * @param comp a pointer to a valid icalcomponent
 * @param name a non-NULL pointer to a char string containing the X-NAME to use.
 */
LIBICAL_ICAL_EXPORT void icalcomponent_set_x_name(icalcomponent *comp, const char *name);

/**
 * Gets the X-NAME property of an icalcomponent.
 *
 * @param comp a pointer to a valid icalcomponent
 *
 * @return pointer to a char string with the X-NAME property for @p comp.
 */
LIBICAL_ICAL_EXPORT const char *icalcomponent_get_x_name(const icalcomponent *comp);

/* Deal with IANA components */

/**
 * Sets the IANA name for an icalcomponent.
 *
 * @param comp a pointer to a valid icalcomponent
 * @param name a pointer to char string containing the IANA name.
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT void icalcomponent_set_iana_name(icalcomponent *comp, const char *name);

/**
 * Gets the IANA name of the specified icalcomponent.
 *
 * @param comp a pointer to a valid icalcomponent
 *
 * @return a pointer to a char string containing the @p comp IANA name; will be NULL if the
 * name has yet to be specified.
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT const char *icalcomponent_get_iana_name(const icalcomponent *comp);

/**
 * Gets the name of the specified icalcomponent.
 *
 * The icalcomponent's name is its type name converted to a string,
 * or the value of _get_x_name if is an X- component type.
 *
 * @param comp a pointer to a valid icalcomponent
 *
 * @return a pointer to a char string with the name of the specified icalcomponent.
 * @see icalcomponent_get_component_name_r
 */
LIBICAL_ICAL_EXPORT const char *icalcomponent_get_component_name(const icalcomponent *comp);

/**
 * Gets the name of the specified icalcomponent.
 *
 * The icalcomponent's name is its type name converted to a string,
 * or the value of _get_x_name if is an X- component type.
 *
 * @param comp a pointer to a valid icalcomponent
 *
 * @return a pointer to a char string with the name of the specified icalcomponent.
 * @see icalcomponent_get_component_name
 */
LIBICAL_ICAL_EXPORT char *icalcomponent_get_component_name_r(const icalcomponent *comp);

/***** Working with Properties *****/

/**
 * Adds an icalproperty to the specified icalcomponent.
 *
 * No error checking is performed by this function, although it will
 * assert if the specified icalproperty has already been added.
 *
 * @param component a pointer to a valid icalcomponent
 * @param property a pointer to a presumably valid icalproperty
 */
LIBICAL_ICAL_EXPORT void icalcomponent_add_property(icalcomponent *component,
                                                    icalproperty *property);

/**
 * Removes an icalproperty from the specified icalcomponent.
 *
 * @param component a pointer to a valid icalcomponent
 * @param property a pointer to the icalproperty to remove
 */
LIBICAL_ICAL_EXPORT void icalcomponent_remove_property(icalcomponent *component,
                                                       icalproperty *property);

/**
 * Removes all icalproperty's of an icalproperty_kind from the specified icalcomponent.
 *
 * @param component a pointer to a valid icalcomponent
 * @param kind is the icalproperty_kind to use
 */
LIBICAL_ICAL_EXPORT void icalcomponent_remove_property_by_kind(icalcomponent *component,
                                                               icalproperty_kind kind);

/**
 * Gets the number of properties of an icalproperty_kind in an icalcomponent.
 *
 * @param component a pointer to a valid icalcomponent
 * @param kind the ical_property_kind to use for the search
 *
 * @return the number of @p kind property types attached to @p component.
 */
LIBICAL_ICAL_EXPORT int icalcomponent_count_properties(icalcomponent *component,
                                                       icalproperty_kind kind);

/**
 * Sets the parent icalcomponent for the specified icalproperty.
 *
 * @param property a pointer to a valid icalproperty
 * @param component a pointer to a valid icalcomponent to use as the parent
 *
 * @since 3.0
 */
LIBICAL_ICAL_EXPORT void icalproperty_set_parent(icalproperty *property,
                                                 icalcomponent *component);

/**
 * Gets the parent icalcomponent for the specified icalproperty.
 *
 * @param property a pointer to a valid icalproperty
 *
 * @return a pointer to the parent icalcomponent of the specified icalproperty.
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalproperty_get_parent(const icalproperty *property);

/* Iterate through the properties */

/**
 * Gets the icalproperty associated to the current icalpropiter of the
 * specified icalcomponent.
 *
 * @param component a pointer to a valid icalcomponent
 *
 * @return a pointer to the current icalproperty or NULL if the current
 * icalpropiter is invalid.
 */
LIBICAL_ICAL_EXPORT icalproperty *icalcomponent_get_current_property(icalcomponent *component);

/**
 * From the head position, iterates the icalproperty list attached to the
 * specified icalcomponent until the first icalproperty_kind is found.
 *
 * @param component a pointer to a valid icalcomponent
 * @param kind an icalproperty_kind to use for the search
 *
 * @return a pointer to the first icalproperty found of icalproperty_kind @p kind;
 * NULL if no matches are found.
 */
LIBICAL_ICAL_EXPORT icalproperty *icalcomponent_get_first_property(icalcomponent *component,
                                                                   icalproperty_kind kind);

/**
 * From the current position, iterates the icalproperty list attached to the
 * specified icalcomponent until the next icalproperty_kind is found.
 *
 * @param component a pointer to a valid icalcomponent
 * @param kind an icalproperty_kind to use for the search
 *
 * @return a pointer to the next icalproperty found of icalproperty_kind @p kind;
 * NULL if no matches are found.
 */
LIBICAL_ICAL_EXPORT icalproperty *icalcomponent_get_next_property(icalcomponent *component,
                                                                  icalproperty_kind kind);

/***** Working with Components *****/

/**
 * Gets a pointer to the first VEVENT, VTODO or VJOURNAL sub-component
 * located in the specified icalcomponent.
 *
 * @param comp a pointer to a valid icalcomponent
 *
 * @return a pointer to the first VEVENT, VTODO or VJOURNAL located in
 * the specified icalcomponent. @p comp itself returned if it is found to
 * be a VEVENT, VTODO or VJOURNAL. NULL is returned if the search failed.
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_get_inner(icalcomponent *comp);

/**
 * Attaches a child icalcomponent to a parent icalcomponent
 *
 * @param parent a pointer to a valid parent icalcomponent
 * @param child a pointer to a valid child icalcomponent
 */
LIBICAL_ICAL_EXPORT void icalcomponent_add_component(icalcomponent *parent, icalcomponent *child);

/**
 * Detaches a child icalcomponent from a parent icalcomponent
 *
 * @param parent a pointer to a valid parent icalcomponent
 * @param child a pointer to a valid child icalcomponent
 */
LIBICAL_ICAL_EXPORT void icalcomponent_remove_component(icalcomponent *parent,
                                                        icalcomponent *child);

/**
 * Gets the number of components of an icalcomponent_kind in an icalcomponent.
 *
 * @param component a pointer to a valid icalcomponent
 * @param kind the icalcomponent_kind to use for the search
 *
 * @return the number of @p kind component types attached to @p component.
 */
LIBICAL_ICAL_EXPORT int icalcomponent_count_components(icalcomponent *component,
                                                       icalcomponent_kind kind);

/**
 * Merges two VCALENDAR components.
 *
 * This takes 2 VCALENDAR components and merges the second one into the first,
 * resolving any problems with conflicting TZIDs. comp_to_merge will no
 * longer exist after calling this function.
 *
 * @param comp a pointer to a valid icalcomponent
 * @param comp_to_merge a pointer to a valid icalcompent to merge into @p comp
 */
LIBICAL_ICAL_EXPORT void icalcomponent_merge_component(icalcomponent *comp,
                                                       icalcomponent *comp_to_merge);

/* Iteration Routines. There are two forms of iterators, internal and
external. The internal ones came first, and are almost completely
sufficient, but they fail badly when you want to construct a loop that
removes components from the container.*/

/* Iterate through components */

/**
 * Gets the icalcomponent associated to the current icalcompiter of the
 * specified icalcomponent.
 *
 * @param component a pointer to a valid icalcomponent
 *
 * @return a pointer to the current icalcomponent
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_get_current_component(icalcomponent *component);

/**
 * From the head position, iterates the icalcomponent list attached to the
 * specified icalcomponent until the first icalcomponent_kind is found.
 *
 * @param component a pointer to a valid icalcomponent
 * @param kind an icalcomponent_kind to use for the search
 *
 * @return a pointer to the first icalcomponent found of icalcomponent_kind @p kind;
 * NULL if no matches are found.
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_get_first_component(icalcomponent *component,
                                                                     icalcomponent_kind kind);

/**
 * From the current position, iterates the icalcomponent list attached to the
 * specified icalcomponent until the next icalcomponent_kind is found.
 *
 * @param component a pointer to a valid icalcomponent
 * @param kind an icalcomponent_kind to use for the search
 *
 * @return a pointer to the next icalcomponent found of icalcomponent_kind @p kind;
 * NULL if no matches are found.
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_get_next_component(icalcomponent *component,
                                                                    icalcomponent_kind kind);

/* Using external iterators */

/**
 * Gets an icalcompiter associated to the first icalcomponent with icalcomponent_kind
 * attached to the specified icalcomponent.
 *
 * @param component a pointer to a valid icalcomponent
 * @param kind the icalcomponent_kind to use for the search
 *
 * @return an icalcompiter for the first component found with icalcomponent_kind @p kind;
 * Use icalcompiter_is_valid() to determine if the return value is valid.
 */
LIBICAL_ICAL_EXPORT icalcompiter icalcomponent_begin_component(icalcomponent *component,
                                                               icalcomponent_kind kind);

/**
 * Gets an icalcompiter associated to the last icalcomponent with icalcomponent_kind
 * attached to the specified icalcomponent.
 *
 * @param component a pointer to a valid icalcomponent
 * @param kind the icalcomponent_kind to use for the search
 *
 * @return an icalcompiter for the last component found with icalcomponent_kind
 * @p kind; if no matches are found ::icalcompiter_null is returned.
 */
LIBICAL_ICAL_EXPORT icalcompiter icalcomponent_end_component(icalcomponent *component,
                                                             icalcomponent_kind kind);

/**
 * Gets the next icalcomponent referenced by the specified icalcompiter with
 * a matching icalcomponent_kind.
 *
 * @param i a pointer to a valid icalcomponent
 *
 * @return a pointer to the next icalcomponent with a matching icalcomponent_kind
 * after the one referenced by @p i. NULL is returned if @p is invalid or if
 * there are no matches found.
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcompiter_next(icalcompiter *i);

/**
 * Gets the previous icalcomponent referenced by the specified icalcompiter with
 * a matching icalcomponent_kind.
 *
 * @param i a pointer to a valid icalcomponent
 *
 * @return a pointer to the previous icalcomponent with a matching icalcomponent_kind
 * before the one referenced by @p i. NULL is returned if @p is invalid or if
 * there are no matches found.
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcompiter_prior(icalcompiter *i);

/**
 * Gets the current icalcomponent referenced by the specified icalcompiter.
 *
 * @param i a pointer to a valid icalcompiter
 *
 * @return a pointer to the icalcomponent associated to @p i or NULL
 * if @p i is invalid.
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcompiter_deref(icalcompiter *i);

/**
 * Checks if the specified icalcompiter is valid.
 *
 * @param i a pointer to a possibly invalid icalcompiter
 *
 * @return true if @p i points to a valid icalcompiter; false otherwise.
 *
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT bool icalcompiter_is_valid(const icalcompiter *i);

/**
 * Gets an icalpropiter associated to the first icalproperty with icalproperty_kind
 * attached to the specified icalcomponent.
 *
 * @param component a pointer to a valid icalcomponent
 * @param kind the ical_property_kind to use for the search
 *
 * @return an icalpropiter for the first property found with icalproperty_kind @p kind;
 * Use icalpropiter_is_valid() to determine if the return value is valid.
 */
LIBICAL_ICAL_EXPORT icalpropiter icalcomponent_begin_property(icalcomponent *component,
                                                              icalproperty_kind kind);

/**
 * Gets the next icalproperty referenced by the specified icalpropiter with
 * a matching icalproperty_kind.
 *
 * @param i a pointer to a valid icalpropiter
 *
 * @return a pointer to the next icalproperty with a matching icalproperty_kind
 * after the one referenced by @p i. NULL is returned if @p is invalid or if
 * there are no matches found.
 */
LIBICAL_ICAL_EXPORT icalproperty *icalpropiter_next(icalpropiter *i);

/**
 * Gets the current icalproperty referenced by the specified icalpropiter.
 *
 * @param i a pointer to a valid icalpropiter
 *
 * @return a pointer to the icalproperty associated to @p i or NULL
 * if @p i is invalid.
 */
LIBICAL_ICAL_EXPORT icalproperty *icalpropiter_deref(icalpropiter *i);

/**
 * Checks if the specified icalpropiter is valid.
 *
 * @param i a pointer to a possibly invalid icalpropiter
 *
 * @return true if @p i points to a valid icalpropiter; false otherwise.
 *
 * @since 4.0
 */
LIBICAL_ICAL_EXPORT bool icalpropiter_is_valid(const icalpropiter *i);

/***** Working with embedded error properties *****/

/* Check the component against itip rules and insert error properties */
/* Working with embedded error properties */

/**
 * Checks restrictions on the specified icalcomponent
 *
 * @param comp a pointer to a valid icalcomponent
 *
 * @return true if @p comp is a VCALENDAR component and it has restrictions;
 * other false is returned. If a non-VCALENDER component is encountered
 * the internal library ::icalerrno is set to ::ICAL_BADARG_ERROR.
 */
LIBICAL_ICAL_EXPORT bool icalcomponent_check_restrictions(icalcomponent *comp);

/**
 * Gets the number of errors encountered parsing the data.
 *
 * @param component a pointer to a valid icalcomponent
 *
 * @return the number of X-LIC-ERROR instances in the specified icalcomponent.
 */
LIBICAL_ICAL_EXPORT int icalcomponent_count_errors(icalcomponent *component);

/**
 * Removes all X-LIC-ERROR properties in an icalcomponent.
 *
 * @param component a pointer to a valid icalcomponent
 */
LIBICAL_ICAL_EXPORT void icalcomponent_strip_errors(icalcomponent *component);

/**
 * Converts X-LIC-ERROR into RETURN-STATUS properties in an icalcomponent.
 *
 * @param component a pointer to a valid icalcomponent
 */
LIBICAL_ICAL_EXPORT void icalcomponent_convert_errors(icalcomponent *component);

/// @cond PRIVATE
/* Internal operations. They are private, and you should not be using them. */

/**
 * Gets the parent icalcomponent for the specified icalcomponent.
 *
 * @param component a pointer to a valid icalcomponent
 *
 * @return a pointer to the parent icalcomponent of @p component.
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_get_parent(const icalcomponent *component);

/**
 * Sets the parent icalcomponent for another icalcomponent.
 *
 * @param component a pointer to a valid icalcomponent to use as the child
 * @param parent a pointer to a valid icalcomponent to use as the parent
 */
LIBICAL_ICAL_EXPORT void icalcomponent_set_parent(icalcomponent *component,
                                                  icalcomponent *parent);

/// @endcond

/* Kind conversion routines */

/**
 * Check the validity of the specified icalcomponent_kind.
 *
 * @param kind is the icalcomponent_kind to use
 *
 * @return true if the icalcomponent_kind is valid; false otherwise.
 */
LIBICAL_ICAL_EXPORT bool icalcomponent_kind_is_valid(const icalcomponent_kind kind);

/**
 * Converts a char string into an icalcomponent_kind.
 *
 * @param string a pointer to a char string containing the character representation
 * of an icalcomponent_kind
 *
 * @return the icalcomponent_kind associated with @p string. ::ICAL_NO_COMPONENT
 * is returned if @p string is invalid.
 */
LIBICAL_ICAL_EXPORT icalcomponent_kind icalcomponent_string_to_kind(const char *string);

/**
 * Converts an icalcomponent_kind to its string representation.
 *
 * @param kind is the icalcomponent_kind to use
 *
 * @return a pointer to a char string containing the string representation
 * of the specified icalcomponent_kind.
 */
LIBICAL_ICAL_EXPORT const char *icalcomponent_kind_to_string(icalcomponent_kind kind);

/************* Derived class methods.  ****************************

If the code was in an OO language, the remaining routines would be
members of classes derived from icalcomponent. Don't call them on the
wrong component subtypes. */

/**
 * Get a reference to the first VEVENT, VTODO or VJOURNAL in an icalcomponent.
 *
 * @param c a pointer to a valid icalcomponent
 *
 * @return a pointer to the first VEVENT, VTODO or VJOURNAL found
 * in the specified icalcomponent, or NULL if none are found.
 *
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_get_first_real_component(const icalcomponent *c);

/**
 * Gets the timespan covered by the specified icalcomponent, in UTC.
 *
 * See icalcomponent_foreach_recurrence() for a better way to
 * extract spans from a component.
 *
 * This method can be called on either a VCALENDAR or any real
 * component. If the VCALENDAR contains no real component, but
 * contains a VTIMEZONE, we return that span instead.
 * This might not be a desirable behavior; we keep it for now
 * for backward compatibility, but it might be deprecated at a
 * future time.
 *
 * @param comp a pointer to a valid icalcomponent
 *
 * FIXME this API needs to be clarified. DTEND is defined as the
 * first available time after the end of this event, so the span
 * should actually end 1 second before DTEND.
 *
 * @return the timespan in icaltime_span covered by the specified icalcomponent.
 *
 * @see icalcomponent_foreach_recurrence
 *
 */
LIBICAL_ICAL_EXPORT struct icaltime_span icalcomponent_get_span(icalcomponent *comp);

/******************** Convenience routines **********************/

/**
 * Sets the DTSTART property for the specified icalcomponent.
 *
 * This method respects the icaltime type (DATE vs DATE-TIME) and
 * timezone (or lack thereof).
 *
 * @param comp a pointer to a valid icalcomponent
 * @param v is the icaltimetype to use as the DSTART property
 *
 * If @p comp is invalid then the internal library ::icalerrno is
 * set to ::ICAL_MALFORMEDDATA_ERROR.
 */
LIBICAL_ICAL_EXPORT void icalcomponent_set_dtstart(icalcomponent *comp, struct icaltimetype v);

/**
 * Gets the DTSTART property of an icalcomponent.
 *
 * If DTSTART is a DATE-TIME with a timezone parameter and a
 * corresponding VTIMEZONE is present in the component, the
 * returned component will already be in the correct timezone;
 * otherwise the caller is responsible for converting it.
 *
 * @param comp a pointer to a valid icalcomponent
 *
 * FIXME this is useless until we can flag the failure
 *
 * @return the DTSTART as an icaltimetype of the specified icalcomponent.
 */
LIBICAL_ICAL_EXPORT struct icaltimetype icalcomponent_get_dtstart(icalcomponent *comp);

/* For the icalcomponent routines only, dtend and duration are tied
   together. If you call the get routine for one and the other exists,
   the routine will calculate the return value. That is, if there is a
   DTEND and you call get_duration, the routine will return the difference
   between DTEND and DTSTART. However, if you call a set routine for
   one and the other exists, no action will be taken and icalerrno will
   be set to ICAL_MALFORMEDDATA_ERROR. If you call a set routine and
   neither exists, the routine will create the appropriate property. */

/**
 * Gets the DTEND property of an icalcomponent.
 *
 * If a DTEND property is not present but a DURATION is, we use
 * that to determine the proper end.
 *
 * If DTSTART is a DATE-TIME with a timezone parameter and a
 * corresponding VTIMEZONE is present in the component, the
 * returned component will already be in the correct timezone;
 * otherwise the caller is responsible for converting it.
 *
 * For the icalcomponent routines only, dtend and duration are tied
 * together. If you call the get routine for one and the other
 * exists, the routine will calculate the return value. That is, if
 * there is a DTEND and you call get_duration, the routine will
 * return the difference between DTEND and DTSTART.
 *
 * When DURATION and DTEND are both missing, for VEVENT an implicit
 * DTEND is calculated based of DTSTART; for AVAILABLE, VAVAILABILITY,
 * and VFREEBUSY null-time is returned.
 *
 * @param comp a pointer to a valid icalcomponent
 *
 * @return null-time, unless called on AVAILABLE, VEVENT,
 * VAVAILABILITY, or VFREEBUSY components.
 *
 * FIXME this is useless until we can flag the failure
 */
LIBICAL_ICAL_EXPORT struct icaltimetype icalcomponent_get_dtend(icalcomponent *comp);

/**
 * Sets the DTEND property for the specified icalcomponent.
 *
 * This method respects the icaltime type (DATE vs DATE-TIME) and
 * timezone (or lack thereof).
 *
 * This also checks that a DURATION property isn't already there,
 * and returns an error if it is. It's the caller's responsibility
 * to remove it.
 *
 * For the icalcomponent routines only, DTEND and DURATION are tied
 * together. If you call this routine and DURATION exists, no action
 * will be taken and icalerrno will be set to ICAL_MALFORMEDDATA_ERROR.
 * If neither exists, the routine will create the appropriate
 * property.
 *
 * @param comp a pointer to a valid icalcomponent
 * @param v is the icaltimetype to use
 *
 * If @p comp is invalid then the internal library ::icalerrno is
 * set to ::ICAL_MALFORMEDDATA_ERROR.
 */
LIBICAL_ICAL_EXPORT void icalcomponent_set_dtend(icalcomponent *comp, struct icaltimetype v);

/**
 * Gets the DUE property of a VTODO icalcomponent.
 *
 * @param comp a pointer to a valid VTODO icalcomponent
 *
 * Uses the DUE property if it exists, otherwise we calculate the DUE
 * value by adding the task's duration to the DTSTART time.
 *
 * @return the DURATION as an icaltimetype of the specified icalcomponent
 */
LIBICAL_ICAL_EXPORT struct icaltimetype icalcomponent_get_due(icalcomponent *comp);

/**
 * Sets the due date of a VTODO task.
 *
 * @param comp a pointer to a valid VTODO icalcomponent
 * @param v    a valid due date time.
 *
 * The DUE and DURATION properties are tied together:
 * - If no duration or due properties then sets the DUE property.
 * - If a DUE property is already set, then resets it to the value v.
 * - If a DURATION property is already set, then calculates the new
 *   duration based on the supplied value of @p v.
 *
 * If @p comp is invalid then the internal library ::icalerrno is
 * set to ::ICAL_MALFORMEDDATA_ERROR.
 */
LIBICAL_ICAL_EXPORT void icalcomponent_set_due(icalcomponent *comp, struct icaltimetype v);

/**
 * Sets the DURATION property for the specified icalcomponent.
 *
 * This method respects the icaltime type (DATE vs DATE-TIME) and
 * timezone (or lack thereof).
 *
 * This also checks that a DTEND property isn't already there,
 * and returns an error if it is. It's the caller's responsibility
 * to remove it.
 *
 * For the icalcomponent routines only, DTEND and DURATION are tied
 * together. If you call this routine and DTEND exists, no action
 * will be taken and icalerrno will be set to ICAL_MALFORMEDDATA_ERROR.
 * If neither exists, the routine will create the appropriate  property.
 *
 * @param comp a pointer to a valid icalcomponent
 * @param v is the icalduration type DURATION to use
 *
 * If @p comp is invalid then the internal library ::icalerrno is
 * set to ::ICAL_MALFORMEDDATA_ERROR.
 */
LIBICAL_ICAL_EXPORT void icalcomponent_set_duration(icalcomponent *comp,
                                                    struct icaldurationtype v);

/**
 * Gets the DURATION property of an icalcomponent.
 *
 * For the icalcomponent routines only, DTEND and DURATION are tied
 * together. If you call the get routine for one and the other
 * exists, the routine will calculate the return value. That is, if
 * there is a DTEND and you call get_duration, the routine will return
 * the difference between DTEND and DTSTART in AVAILABLE, VEVENT, or
 * VAVAILABILITY; and the difference between DUE and DTSTART in VTODO.
 * When both DURATION and DTEND are missing from VEVENT an implicit
 * duration is returned, based on the value-type of DTSTART. Otherwise
 * null-duration is returned.
 *
 * @param comp a pointer to a valid icalcomponent
 *
 * @return the DURATION as an icalduration of the specified icalcomponent.
 */
LIBICAL_ICAL_EXPORT struct icaldurationtype icalcomponent_get_duration(icalcomponent *comp);

/**
 * Sets the METHOD property of the specified icalcomponent.
 *
 * @param comp a pointer to a valid icalcomponent
 * @param method is the icalproperty_method METHOD property to use
 *
 * If @p comp is invalid then the internal library ::icalerrno is
 * set to ::ICAL_MALFORMEDDATA_ERROR.
 */
LIBICAL_ICAL_EXPORT void icalcomponent_set_method(icalcomponent *comp, icalproperty_method method);

/**
 * Gets the METHOD property of an icalcomponent.
 *
 * @param comp a pointer to a valid icalcomponent
 *
 * @return an icalproperty_method with the METHOD property for @p comp.
 * If there is no such property then a value of ICAL_METHOD_NONE is returned.
 * If @p comp is invalid then a value of ICAL_METHOD_NONE is returned
 * and the internal library ::icalerrno is set to ::ICAL_MALFORMEDDATA_ERROR.
 */
LIBICAL_ICAL_EXPORT icalproperty_method icalcomponent_get_method(icalcomponent *comp);

/**
 * Gets the DTSTAMP property for the specified icalcomponent.
 *
 * @param comp a pointer to a valid icalcomponent
 *
 * @return an icaltimetype with the DTSTAMP property for @p comp.
 * If there is no such property then a value of `icaltime_null_time()` is
 * returned.
 * If @p comp is invalid then a value of `icaltime_null_time()` is returned
 * and the internal library ::icalerrno is set to ::ICAL_MALFORMEDDATA_ERROR.
 */
LIBICAL_ICAL_EXPORT struct icaltimetype icalcomponent_get_dtstamp(icalcomponent *comp);

/**
 * Sets the DTSTAMP property of an icalcomponent.
 *
 * @param comp a pointer to a valid icalcomponent
 * @param v is the icaltimetype to use as the DTSTAMP property
 *
 * If @p comp is invalid then the internal library ::icalerrno is
 * set to ::ICAL_MALFORMEDDATA_ERROR.
 */
LIBICAL_ICAL_EXPORT void icalcomponent_set_dtstamp(icalcomponent *comp, struct icaltimetype v);

/**
 * Sets the SUMMARY property for the specified icalcomponent.
 *
 * @param comp a pointer to a valid icalcomponent
 * @param v a pointer to a char string with the SUMMARY to use
 *
 * If @p comp is invalid then the internal library ::icalerrno is
 * set to ::ICAL_MALFORMEDDATA_ERROR.
 */
LIBICAL_ICAL_EXPORT void icalcomponent_set_summary(icalcomponent *comp, const char *v);

/**
 * Gets the SUMMARY property for the specified icalcomponent.
 *
 * @param comp a pointer to a valid icalcomponent
 *
 * @return pointer to a char string with the SUMMARY property for @p comp.
 * If there is no such property then a value of NULL is returned.
 * If @p comp is invalid then a value of NULL is returned and the
 * internal library ::icalerrno is set to ::ICAL_MALFORMEDDATA_ERROR.
 */
LIBICAL_ICAL_EXPORT const char *icalcomponent_get_summary(icalcomponent *comp);

/**
 * Sets the COMMENT property for the specified icalcomponent.
 *
 * @param comp a pointer to a valid icalcomponent
 * @param v a pointer to a char string with the COMMENT to use
 *
 * If @p comp is invalid then the internal library ::icalerrno is
 * set to ::ICAL_MALFORMEDDATA_ERROR.
 */
LIBICAL_ICAL_EXPORT void icalcomponent_set_comment(icalcomponent *comp, const char *v);

/**
 * Gets the COMMENT property for the specified icalcomponent.
 *
 * @param comp a pointer to a valid icalcomponent
 *
 * @return pointer to a char string with the COMMENT property for @p comp.
 * If there is no such property then a value of NULL is returned.
 * If @p comp is invalid then a value of NULL is returned and the
 * internal library ::icalerrno is set to ::ICAL_MALFORMEDDATA_ERROR.
 */
LIBICAL_ICAL_EXPORT const char *icalcomponent_get_comment(icalcomponent *comp);

/**
 * Sets the UID property for the specified icalcomponent.
 *
 * @param comp a pointer to a valid icalcomponent
 * @param v a pointer to a char string with the UID to use
 *
 * If @p comp is invalid then the internal library ::icalerrno is
 * set to ::ICAL_MALFORMEDDATA_ERROR.
 */
LIBICAL_ICAL_EXPORT void icalcomponent_set_uid(icalcomponent *comp, const char *v);

/**
 * Gets the UID property for the specified icalcomponent.
 *
 * @param comp a pointer to a valid icalcomponent
 *
 * @return pointer to a char string with the UID property for @p comp.
 * If there is no such property then a value of NULL is returned.
 * If @p comp is invalid then a value of NULL is returned and the
 * internal library ::icalerrno is set to ::ICAL_MALFORMEDDATA_ERROR.
 */
LIBICAL_ICAL_EXPORT const char *icalcomponent_get_uid(icalcomponent *comp);

/**
 * Sets the RELCALID property for the specified icalcomponent.
 *
 * @param comp a pointer to a valid icalcomponent
 * @param v a pointer to a char string with the RELCALID to use
 *
 * If @p comp is invalid then the internal library ::icalerrno is
 * set to ::ICAL_MALFORMEDDATA_ERROR.
 */
LIBICAL_ICAL_EXPORT void icalcomponent_set_relcalid(icalcomponent *comp, const char *v);

/**
 * Gets the RELCALID property of an icalcomponent.
 *
 * @param comp a pointer to a valid icalcomponent
 *
 * @return the relcalid of the specified icalcomponent.
 * If there is no such property then a value of NULL is returned.
 * If @p comp is invalid then a value of NULL is returned and the
 * internal library ::icalerrno is set to ::ICAL_MALFORMEDDATA_ERROR.
 */
LIBICAL_ICAL_EXPORT const char *icalcomponent_get_relcalid(icalcomponent *comp);

/**
 * Sets the RECURRENCEID property for the specified icalcomponent.
 *
 * @param comp a pointer to a valid icalcomponent
 * @param v is the icaltimetype RECURRENCEID to use
 *
 * If @p comp is invalid then the internal library ::icalerrno is
 * set to ::ICAL_MALFORMEDDATA_ERROR.
 */
LIBICAL_ICAL_EXPORT void icalcomponent_set_recurrenceid(icalcomponent *comp,
                                                        struct icaltimetype v);

/**
 * Gets the RECURRENCEID property of the specified icalcomponent.
 *
 * @param comp a pointer to a valid icalcomponent
 *
 * @return an icaltimetype with the RECURRENCEID property for @p comp.
 * If there is no such property then a value of `icaltime_null_time()`
 * is returned.
 * If @p comp is invalid then a value of `icaltime_null_time()` is returned
 * and the internal library ::icalerrno is set to ::ICAL_MALFORMEDDATA_ERROR.
 */
LIBICAL_ICAL_EXPORT struct icaltimetype icalcomponent_get_recurrenceid(icalcomponent *comp);

/**
 * Sets the DESCRIPTION property for the specified icalcomponent.
 *
 * @param comp a pointer to a valid icalcomponent
 * @param v a pointer to a char string with the DESCRIPTION to use
 *
 * If @p comp is invalid then the internal library ::icalerrno is
 * set to ::ICAL_MALFORMEDDATA_ERROR.
 */
LIBICAL_ICAL_EXPORT void icalcomponent_set_description(icalcomponent *comp, const char *v);

/**
 * Gets the DESCRIPTION property of the specified icalcomponent.
 *
 * @param comp a pointer to a valid icalcomponent
 *
 * @return pointer to a char string with the DESCRIPTION property for @p comp.
 * If there is no such property then a value of NULL is returned.
 * If @p comp is invalid then a value of NULL is returned and the
 * internal library ::icalerrno is set to ::ICAL_MALFORMEDDATA_ERROR.
 */
LIBICAL_ICAL_EXPORT const char *icalcomponent_get_description(icalcomponent *comp);

/**
 * Sets the LOCATION property for the specified icalcomponent.
 *
 * @param comp a pointer to a valid icalcomponent
 * @param v a pointer to char string with the LOCATION to use
 *
 * If @p comp is invalid then the internal library ::icalerrno is
 * set to ::ICAL_MALFORMEDDATA_ERROR.
 */
LIBICAL_ICAL_EXPORT void icalcomponent_set_location(icalcomponent *comp, const char *v);

/**
 * Gets the LOCATION property of the specified icalcomponent.
 *
 * @param comp a pointer to a valid icalcomponent
 *
 * @return a pointer to a char string with the LOCATION property for @p comp.
 * If there is no such property then a value of NULL is returned.
 * If @p comp is invalid then a value of NULL is returned and the
 * internal library ::icalerrno is set to ::ICAL_MALFORMEDDATA_ERROR.
 */
LIBICAL_ICAL_EXPORT const char *icalcomponent_get_location(icalcomponent *comp);

/**
 * Sets the SEQUENCE property for the specified icalcomponent.
 *
 * @param comp a pointer to a valid icalcomponent
 * @param v is the integer SEQUENCE to use
 *
 * If @p comp is invalid then the internal library ::icalerrno is
 * set to ::ICAL_MALFORMEDDATA_ERROR.
 */
LIBICAL_ICAL_EXPORT void icalcomponent_set_sequence(icalcomponent *comp, int v);

/**
 * Gets the SEQUENCE property of the specified icalcomponent.
 *
 * @param comp a pointer to a valid icalcomponent
 *
 * @return a integer SEQUENCE property for @p comp.
 * If there is no such property then a value of zero is returned.
 * If @p comp is invalid then a value of zero is returned and the
 * internal library ::icalerrno is set to ::ICAL_MALFORMEDDATA_ERROR.
 */
LIBICAL_ICAL_EXPORT int icalcomponent_get_sequence(icalcomponent *comp);

/**
 * Sets the STATUS for the specified icalcomponent.
 *
 * @param comp a pointer to a valid icalcomponent
 * @param v is the icalproperty_status to use as the STATUS property
 *
 * If @p comp is invalid then the internal library ::icalerrno is
 * set to ::ICAL_MALFORMEDDATA_ERROR.
 */
LIBICAL_ICAL_EXPORT void icalcomponent_set_status(icalcomponent *comp, enum icalproperty_status v);

/**
 * Gets the status property of the icalcomponent.
 *
 * @param comp a pointer to a valid icalcomponent
 *
 * @returns in normal conditions, the status property as an icalproperty_status
 * of the specified icalcomponent; else ICAL_STATUS_NONE if a problem parsing the
 * status was detected.
 */
LIBICAL_ICAL_EXPORT enum icalproperty_status icalcomponent_get_status(icalcomponent *comp);

/**
 * Calls the given function for each TZID parameter found in the
 * component, as well as any subcomponents.
 *
 * @param comp a pointer to a valid icalcomponent
 * @param callback a pointer to a function to call for each TZID parameter
 * @param callback_data a pointer to data associated with the callback function
 */
LIBICAL_ICAL_EXPORT void icalcomponent_foreach_tzid(icalcomponent *comp,
                                                    void (*callback)(icalparameter *param,
                                                                     void *data),
                                                    void *callback_data);

/**
 * Return the icaltimezone in the component corresponding to the
 * specified TZID, or NULL if a one cannot be found.
 *
 * @param comp a pointer to a valid icalcomponent
 * @param tzid a pointer to a char string containing the TZID
 *
 * @return the icaltimezone in the specified icalcomponent corresponding
 * to the TZID, or NULL if it can't be found.
 */
LIBICAL_ICAL_EXPORT icaltimezone *icalcomponent_get_timezone(icalcomponent *comp,
                                                             const char *tzid);

/**
 * @brief Decides if a recurrence is acceptable.
 *
 * @param comp a pointer to a valid icalcomponent
 * @param dtstart the base dtstart value for this component.
 * @param recurtime the time to test against.
 *
 * @return true if the recurrence value is excluded, false otherwise.
 *
 * This function decides if a specific recurrence value is
 * excluded by EXRULE or EXDATE properties.
 *
 * It's not the most efficient code.  You might get better performance
 * if you assume that recurtime is always increasing for each
 * call. Then you could:
 *
 *   - sort the EXDATE values
 *   - save the state of each EXRULE iterator for the next call.
 *
 * In this case though you don't need to worry how you call this
 * function.  It will always return the correct result.
 */
LIBICAL_ICAL_EXPORT bool icalproperty_recurrence_is_excluded(icalcomponent *comp,
                                                             struct icaltimetype *dtstart,
                                                             struct icaltimetype *recurtime);

/**
 * Iterates through all recurrences of an event.
 *
 * @param comp           a pointer to a VEVENT icalcomponent
 * @param start          Ignore timespans before this
 * @param end            Ignore timespans after this
 * @param callback       Function called for each timespan within the range
 * @param callback_data  Pointer passed back to the callback function
 *
 * This function will call the specified callback function for once
 * for the base value of DTSTART, and foreach recurring date/time
 * value.
 *
 * It will filter out events that are specified as an EXDATE or an EXRULE.
 */
LIBICAL_ICAL_EXPORT void icalcomponent_foreach_recurrence(icalcomponent *comp,
                                                          struct icaltimetype start,
                                                          struct icaltimetype end,
                                                          void (*callback)(icalcomponent *comp,
                                                                           const struct icaltime_span *span,
                                                                           void *data),
                                                          void *callback_data);

/**
 * Normalizes (reorders and sorts the properties) the specified icalcomponent.
 *
 * @param comp a pointer to a valid icalcomponent
 *
 * @since 3.0
 */
LIBICAL_ICAL_EXPORT void icalcomponent_normalize(icalcomponent *comp);

/**
 * Computes the datetime corresponding to the specified @p icalproperty and @p icalcomponent.
 * If the property is a DATE-TIME with a TZID parameter and a corresponding VTIMEZONE
 * is present in the component, the returned component will already be in the correct
 * timezone; otherwise the caller is responsible for converting it.
 *
 * Call icaltime_is_null_time() on the returned value to detect failures.
 *
 * @param prop a pointer to a valid icalproperty
 * @param comp a pointer to a valid icalcomponent
 *
 * @return the DATE or DATE_TIME an icaltimetype of the specified icalcomponent.
 *
 * @since 3.0.5
 */
LIBICAL_ICAL_EXPORT struct icaltimetype icalproperty_get_datetime_with_component(
    icalproperty *prop,
    icalcomponent *comp);
/*************** Type Specific routines ***************/

/**
 * Constructs a new ::ICAL_VCALENDAR_COMPONENT icalcomponent.
 *
 * @return a pointer to the new icalcomponet. Free with icalcomponent_free
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_new_vcalendar(void);

/**
 * Constructs a new ::ICAL_VEVENT_COMPONENT icalcomponent.
 *
 * @return a pointer to the new icalcomponet. Free with icalcomponent_free
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_new_vevent(void);

/**
 * Constructs a new ::ICAL_VTODO_COMPONENT icalcomponent.
 *
 * @return a pointer to the new icalcomponet. Free with icalcomponent_free
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_new_vtodo(void);

/**
 * Constructs a new ::ICAL_VJOURNAL_COMPONENT icalcomponent.
 *
 * @return a pointer to the new icalcomponet. Free with icalcomponent_free
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_new_vjournal(void);

/**
 * Constructs a new ::ICAL_VALARM_COMPONENT icalcomponent.
 *
 * @return a pointer to the new icalcomponet. Free with icalcomponent_free
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_new_valarm(void);

/**
 * Constructs a new ::ICAL_VFREEBUSY_COMPONENT icalcomponent.
 *
 * @return a pointer to the new icalcomponet. Free with icalcomponent_free
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_new_vfreebusy(void);

/**
 * Constructs a new ::ICAL_VTIMEZONE_COMPONENT icalcomponent.
 *
 * @return a pointer to the new icalcomponet. Free with icalcomponent_free
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_new_vtimezone(void);

/**
 * Constructs a new ::ICAL_XSTANDARD_COMPONENT icalcomponent.
 *
 * @return a pointer to the new icalcomponet. Free with icalcomponent_free
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_new_xstandard(void);

/**
 * Constructs a new ::ICAL_XDAYLIGHT_COMPONENT icalcomponent.
 *
 * @return a pointer to the new icalcomponet. Free with icalcomponent_free
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_new_xdaylight(void);

/**
 * Constructs a new ::ICAL_VAGENDA_COMPONENT icalcomponent.
 *
 * @return a pointer to the new icalcomponet. Free with icalcomponent_free
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_new_vagenda(void);

/**
 * Constructs a new ::ICAL_VQUERY_COMPONENT icalcomponent.
 *
 * @return a pointer to the new icalcomponet. Free with icalcomponent_free
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_new_vquery(void);

/**
 * Constructs a new ::ICAL_VREPLY_COMPONENT icalcomponent.
 *
 * @return a pointer to the new icalcomponet. Free with icalcomponent_free
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_new_vreply(void);

/**
 * Constructs a new ::ICAL_VAVAILABILITY_COMPONENT icalcomponent.
 *
 * @return a pointer to the new icalcomponet. Free with icalcomponent_free
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_new_vavailability(void);

/**
 * Constructs a new ::ICAL_XAVAILABLE_COMPONENT icalcomponent.
 *
 * @return a pointer to the new icalcomponet. Free with icalcomponent_free
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_new_xavailable(void);

/**
 * Constructs a new ::ICAL_VPOLL_COMPONENT icalcomponent.
 *
 * @return a pointer to the new icalcomponet. Free with icalcomponent_free
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_new_vpoll(void);

/**
 * Constructs a new ::ICAL_VVOTER_COMPONENT icalcomponent.
 *
 * @return a pointer to the new icalcomponet. Free with icalcomponent_free
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_new_vvoter(void);

/**
 * Constructs a new ::ICAL_XVOTE_COMPONENT icalcomponent.
 *
 * @return a pointer to the new icalcomponet. Free with icalcomponent_free
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_new_xvote(void);

/**
 * Constructs a new ::ICAL_VPATCH_COMPONENT icalcomponent.
 *
 * @return a pointer to the new icalcomponet. Free with icalcomponent_free
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_new_vpatch(void);

/**
 * Constructs a new ::ICAL_XPATCH_COMPONENT icalcomponent.
 *
 * @return a pointer to the new icalcomponet. Free with icalcomponent_free
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_new_xpatch(void);

/**
 * Constructs a new ::ICAL_PARTICIPANT_COMPONENT icalcomponent.
 *
 * @return a pointer to the new icalcomponet. Free with icalcomponent_free
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_new_participant(void);

/**
 * Constructs a new ::ICAL_VLOCATION_COMPONENT icalcomponent.
 *
 * @return a pointer to the new icalcomponet. Free with icalcomponent_free
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_new_vlocation(void);

/**
 * Constructs a new ::ICAL_VRESOURCE_COMPONENT icalcomponent.
 *
 * @return a pointer to the new icalcomponet. Free with icalcomponent_free
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalcomponent_new_vresource(void);

#endif /* !ICALCOMPONENT_H */
