/**
 * @file     icalspanlist_cxx.cpp
 * @author   Critical Path
 * @brief    C++ class wrapping the icalspanlist data structure
 *
 * SPDX-FileCopyrightText: 2001, Critical Path
 * SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
*/

#include "icalspanlist_cxx.hpp"
#include "vcomponent_cxx.hpp"

#include <cstdlib> // for free()

using namespace LibICal;

ICalSpanList::ICalSpanList()
    : data(0)
{
    throw icalerrno;
}

ICalSpanList::ICalSpanList(const ICalSpanList &v)
    : data(v.data)
{
    if (data == NULL) {
        throw icalerrno;
    }
}

/** Construct an ICalSpanList from an icalset
    @param set     The icalset containing the VEVENTS
    @param start   Designated start of the spanlist
    @param end     Designated end of the spanlist
*/
ICalSpanList::ICalSpanList(icalset *set, icaltimetype start, icaltimetype end)
    : data(icalspanlist_new(set, start, end))
{
    if (data == NULL) {
        throw icalerrno;
    }
}

/** @brief Constructor
    @param comp  A valid icalcomponent with a VFREEBUSY section
*/

ICalSpanList::ICalSpanList(icalcomponent *comp)
    : data(icalspanlist_from_vfreebusy(comp))
{
    if (data == NULL) {
        throw icalerrno;
    }
}

/** @brief Constructor
    @param comp  A valid VComponent with a VFREEBUSY section
*/
ICalSpanList::ICalSpanList(VComponent &comp)
    : data(icalspanlist_from_vfreebusy(static_cast<icalcomponent *>(comp)))
{
    if (data == NULL) {
        throw icalerrno;
    }
}

void ICalSpanList::dump()
{
    icalspanlist_dump(data);
}

/** Destructor */
ICalSpanList::~ICalSpanList()
{
    if (data == NULL) {
        icalspanlist_free(data);
    }
}

/**
 * @brief Returns a VFREEBUSY component for the object.
 *
 * @see icalspanlist_as_vfreebusy()
 */

VComponent *ICalSpanList::get_vfreebusy(const char *organizer, const char *attendee)
{
    icalcomponent *comp;
    VComponent *vcomp;

    comp = icalspanlist_as_vfreebusy(data, organizer, attendee);
    if (comp == NULL) {
        throw icalerrno;
    }

    vcomp = new VComponent(comp);
    if (vcomp == NULL) {
        throw icalerrno;
    }

    return vcomp;
}

/**
 * @brief Returns a summary of events over delta_t
 *
 * @param delta_t    Number of seconds to divide the spanlist time period
 *                   into.
 *
 * This method calculates the total number of events in each time slot
 * of delta_t seconds.
 *
 * @see icalspanlist_as_freebusy_matrix()
 */

std::vector<int> ICalSpanList::as_vector(int delta_t)
{
    int *matrix;
    const int i = 0;
    std::vector<int> event_vec;

    matrix = icalspanlist_as_freebusy_matrix(data, delta_t);

    if (matrix == NULL) {
        throw icalerrno;
    }

    while (matrix[i] != -1) {
        event_vec.push_back(matrix[i]); // Add item at end of vector
    }

    free(matrix);
    return event_vec;
}
