/*======================================================================
 FILE: vcard.h

======================================================================*/

/**
 *      @file vcard.h
 */

#ifndef VCARD_H
#define VCARD_H

#include "libical_vcard_export.h"
#include "vcardproperty.h"
#include "pvl.h"

typedef struct vcard_impl vcard;

/** @brief Constructor
 */
LIBICAL_VCARD_EXPORT vcard *vcard_new(vcardproperty_version version);

/**
 * @brief Deeply clones an vcard.
 * Returns a pointer to the memory for the newly cloned vcard.
 * @since 3.1.0
 */
LIBICAL_VCARD_EXPORT vcard *vcard_clone(const vcard *card);

/** @brief Constructor
 */
LIBICAL_VCARD_EXPORT vcard *vcard_new_from_string(const char *str);

/** @brief Constructor
 */
LIBICAL_VCARD_EXPORT vcard *vcard_vanew(vcardproperty_version version, ...);

/*** @brief Destructor
 */
LIBICAL_VCARD_EXPORT void vcard_free(vcard *card);

LIBICAL_VCARD_EXPORT char *vcard_as_vcard_string(vcard *card);

LIBICAL_VCARD_EXPORT char *vcard_as_vcard_string_r(vcard *card);

LIBICAL_VCARD_EXPORT int vcard_is_valid(vcard *card);


/***** Working with Properties *****/

LIBICAL_VCARD_EXPORT void vcard_add_property(vcard *card,
                                             vcardproperty *property);

LIBICAL_VCARD_EXPORT void vcard_remove_property(vcard *card,
                                                vcardproperty *property);

LIBICAL_VCARD_EXPORT int vcard_count_properties(vcard *card,
                                                vcardproperty_kind kind);

/**
 * @brief Sets the parent vcard for the specified vcardproperty @p property.
 * @since 3.0
 */
LIBICAL_VCARD_EXPORT void vcardproperty_set_parent(vcardproperty *property,
                                                   vcard *card);

/**
 * @brief Returns the parent vcard for the specified @p property.
 */
LIBICAL_VCARD_EXPORT vcard *vcardproperty_get_parent(const vcardproperty *property);

/* Iterate through the properties */
LIBICAL_VCARD_EXPORT vcardproperty *vcard_get_current_property(vcard *card);

LIBICAL_VCARD_EXPORT vcardproperty *vcard_get_first_property(vcard *card,
                                                             vcardproperty_kind kind);
LIBICAL_VCARD_EXPORT vcardproperty *vcard_get_next_property(vcard *component,
                                                                  vcardproperty_kind kind);

/**
 *  This takes 2 VCARD components and merges the second one into the first.
 *  comp_to_merge will no longer exist after calling this function.
 */
LIBICAL_VCARD_EXPORT void vcard_merge_card(vcard *card,
                                           vcard *card_to_merge);

/***** Working with embedded error properties *****/

/* Check the component against itip rules and insert error properties*/
/* Working with embedded error properties */
LIBICAL_VCARD_EXPORT int vcard_check_restrictions(vcard *card);

/** @brief Returns the number of errors encountered parsing the data.
 *
 * This function counts the number times the X-LIC-ERROR occurs
 * in the data structure.
 */
LIBICAL_VCARD_EXPORT int vcard_count_errors(vcard *card);

/** @brief Removes all X-LIC-ERROR properties*/
LIBICAL_VCARD_EXPORT void vcard_strip_errors(vcard *card);

/** @brief Converts some X-LIC-ERROR properties into RETURN-STATUS properties*/
LIBICAL_VCARD_EXPORT void vcard_convert_errors(vcard *card);
#if 0
/******************** Convenience routines **********************/

/**     @brief Sets the DTSTART property to the given icaltime,
 *
 *      This method respects the icaltime type (DATE vs DATE-TIME) and
 *      timezone (or lack thereof).
 */
LIBICAL_VCARD_EXPORT void vcard_set_dtstart(vcard *comp, struct icaltimetype v);

/**     @brief Gets the DTSTART property as an icaltime
 *
 *      If DTSTART is a DATE-TIME with a timezone parameter and a
 *      corresponding VTIMEZONE is present in the card, the
 *      returned card will already be in the correct timezone;
 *      otherwise the caller is responsible for converting it.
 *
 *      FIXME this is useless until we can flag the failure
 */
LIBICAL_VCARD_EXPORT struct icaltimetype vcard_get_dtstart(vcard *comp);
#endif
/**
 * @brief Normalizes (reorders and sorts the properties) the specified vcard @p comp.
 * @since 3.0
 */
LIBICAL_VCARD_EXPORT void vcard_normalize(vcard *card);

#endif /* !VCARD_H */
