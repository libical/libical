/*======================================================================
 FILE: VEvent.java
 CREATOR: fnguyen 01/11/02

 SPDX-FileCopyrightText: 2002, Critical Path
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
======================================================================*/

package net.cp.jlibical;

public class VEvent extends VComponent {
    public VEvent() {
        super(ICalComponentKind.ICAL_VEVENT_COMPONENT);
    }

    public VEvent(long obj) {
        super(obj);
    }

    public VEvent(String str) {
        super(str);
    }
}
