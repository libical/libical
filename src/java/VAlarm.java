/*======================================================================
 FILE: VAlarm.java
 CREATOR: fnguyen 01/11/02

 SPDX-FileCopyrightText: 2002, Critical Path
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
======================================================================*/

package net.cp.jlibical;

public class VAlarm extends VComponent {
	public VAlarm()
	{
		super(ICalComponentKind.ICAL_VALARM_COMPONENT);
	}

	public VAlarm(long obj)
	{
		super(obj);
	}

	public VAlarm(String str)
	{
		super(str);
	}
}
