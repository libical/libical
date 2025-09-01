/**
 * @file     icalspanlist_cxx.h
 * @author   Critical Path
 * @brief    C++ class wrapping the icalspanlist data structure
 *
 * SPDX-FileCopyrightText: 2001, Critical Path
 * SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 */

#ifndef ICALSPANLIST_CXX_H
#define ICALSPANLIST_CXX_H

#include "libical_icalss_export.h"

extern "C" {
#include "icalcomponent.h"
#include "icalspanlist.h"
#include "icaltime.h"
}

#include <vector>

namespace LibICal
{

class VComponent;

/**
 * This class wraps the icalspanlist routines in libicalss
 *
 * Errors within libicalss are propagated via exceptions of type
 * icalerrorenum.  See icalerror.h for the complete list of exceptions
 * that might be thrown.
 */
class LIBICAL_ICALSS_EXPORT ICalSpanList
{
public:
    ICalSpanList();
    ICalSpanList(const ICalSpanList &v);

    /** Constructs an ICalSpanList from an icalset */
    ICalSpanList(icalset *set, icaltimetype start, icaltimetype end);

    /** Constructs an ICalSpanList from the VFREEBUSY chunk of an icalcomponent */
    explicit ICalSpanList(icalcomponent *comp);

    /** Constructs an ICalSpanList from the VFREEBUSY chunk of a vcomponent */
    explicit ICalSpanList(VComponent &comp);

    /** Destructor */
    ~ICalSpanList();

    /** Returns a VFREEBUSY icalcomponent */
    VComponent *get_vfreebusy(const char *organizer, const char *attendee);

    ICalSpanList &operator=(const ICalSpanList &);

    /** Returns the base data when casting */
    operator icalspanlist *() { return data; }

    /** Returns a vector of the number of events over delta t */
    std::vector<int> as_vector(int delta_t);

    /** Dumps the spanlist to STDOUT */
    void dump();

private:
    icalspanlist *data;
};

} // namespace LibICal;

#endif
