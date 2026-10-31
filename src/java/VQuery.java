/*======================================================================
 FILE: VQuery.java
 CREATOR: fnguyen 01/11/02

 SPDX-FileCopyrightText: 2002, Critical Path
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
======================================================================*/

package net.cp.jlibical;

public class VQuery extends VComponent {
    public VQuery() {
        super(ICalComponentKind.ICAL_VQUERY_COMPONENT);
    }

    public VQuery(long obj) {
        super(obj);
    }

    public VQuery(String str) {
        super(str);
    }
}
