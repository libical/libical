/*======================================================================
 FILE: VCalendar.java
 CREATOR: echoi 01/28/02

 SPDX-FileCopyrightText: 2002, Critical Path
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
======================================================================*/

package net.cp.jlibical;

public class VCalendar extends VComponent {
    public VCalendar() {
        super(ICalComponentKind.ICAL_VCALENDAR_COMPONENT);
    }

    public VCalendar(long obj) {
        super(obj);
    }

    public VCalendar(String str) {
        super(str);
    }
}
