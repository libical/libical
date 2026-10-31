/*======================================================================
 FILE: VFreeBusy.java
 CREATOR:

 SPDX-FileCopyrightText: 2002, Critical Path
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
======================================================================*/

package net.cp.jlibical;

public class VFreeBusy extends VComponent {
    public VFreeBusy() {
        super(ICalComponentKind.ICAL_VFREEBUSY_COMPONENT);
    }

    public VFreeBusy(long obj) {
        super(obj);
    }

    public VFreeBusy(String str) {
        super(str);
    }
}
