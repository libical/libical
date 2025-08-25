/*======================================================================
 FILE: VAgenda.java
 CREATOR: fnguyen 01/11/02

 SPDX-FileCopyrightText: 2002, Critical Path
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
======================================================================*/

package net.cp.jlibical;

public class VAgenda extends VComponent {
	public VAgenda()
	{
		super(ICalComponentKind.ICAL_VAGENDA_COMPONENT);
	}

	public VAgenda(long obj)
	{
		super(obj);
	}

	public VAgenda(String str)
	{
		super(str);
	}
}
