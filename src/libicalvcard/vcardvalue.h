/*======================================================================
 FILE: vcardvalue.h

 ======================================================================*/

#ifndef VCARDVALUE_H
#define VCARDVALUE_H

#include "libical_vcard_export.h"
#include "vcardvalueimpl.h"

/** @file vcardvalue.h */

#define VCARD_BOOLEAN_TRUE  1
#define VCARD_BOOLEAN_FALSE 0

LIBICAL_VCARD_EXPORT vcardvalue *vcardvalue_new(vcardvalue_kind kind);

/** @brief Deeply clones an vcardvalue.
 *
 * Returns a pointer to the memory for the newly cloned vcardvalue.
 * @since 3.1.0
 */
LIBICAL_VCARD_EXPORT vcardvalue *vcardvalue_clone(const vcardvalue *value);

LIBICAL_VCARD_EXPORT vcardvalue *vcardvalue_new_from_string(vcardvalue_kind kind, const char *str);

LIBICAL_VCARD_EXPORT void vcardvalue_free(vcardvalue *value);

LIBICAL_VCARD_EXPORT int vcardvalue_is_valid(const vcardvalue *value);

LIBICAL_VCARD_EXPORT const char *vcardvalue_as_vcard_string(const vcardvalue *value);

LIBICAL_VCARD_EXPORT char *vcardvalue_as_vcard_string_r(const vcardvalue *value);

LIBICAL_VCARD_EXPORT vcardvalue_kind vcardvalue_isa(const vcardvalue *value);

LIBICAL_VCARD_EXPORT int vcardvalue_isa_value(void *);

//LIBICAL_VCARD_EXPORT icalparameter_xliccomparetype vcardvalue_compare(const vcardvalue *a,
//                                                                    const vcardvalue *b);

/* Convert enumerations */

LIBICAL_VCARD_EXPORT vcardvalue_kind vcardvalue_string_to_kind(const char *str);

LIBICAL_VCARD_EXPORT const char *vcardvalue_kind_to_string(const vcardvalue_kind kind);

/** Check validity of a specific vcardvalue_kind **/
LIBICAL_VCARD_EXPORT int vcardvalue_kind_is_valid(const vcardvalue_kind kind);

/** Encode a character string in ical format, escape certain characters, etc. */
LIBICAL_VCARD_EXPORT int vcardvalue_encode_ical_string(const char *szText,
                                                       char *szEncText, int MaxBufferLen);

/** Extract the original character string encoded by the above function **/
LIBICAL_VCARD_EXPORT int vcardvalue_decode_ical_string(const char *szText,
                                                       char *szDecText, int nMaxBufferLen);

/* For the library only -- do not make visible */
/// @cond
//extern void print_date_to_string(char *str, const struct icaltimetype *data);
//extern void print_datetime_to_string(char *str, const struct icaltimetype *data);
/// @endcond

#endif /*VCARDVALUE_H */
