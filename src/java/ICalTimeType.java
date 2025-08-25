/*======================================================================
 FILE: ICalTimeType.java
 CREATOR: structConverter 01/11/02

 SPDX-FileCopyrightText: 2002, Critical Path
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
======================================================================*/

package net.cp.jlibical;

/** struct icaltimetype */
public class ICalTimeType
{
	/**
	 * Constructor for pre-existing native icaltimetype
	 * @param obj c++ pointer
	 */
	ICalTimeType(long obj)
	{
		init(obj);
	}

	/**
	 * Constructor for default ICalTimeType
	 */
	public ICalTimeType()
	{
	}

    public void setYear(int lcl_arg0)
    {
	year = lcl_arg0;
    }
    public int getYear()
    {
	return year;
    }

    public void setMonth(int lcl_arg0)
    {
	month = lcl_arg0;
    }
    public int getMonth()
    {
	return month;
    }

    public void setDay(int lcl_arg0)
    {
	day = lcl_arg0;
    }
    public int getDay()
    {
	return day;
    }

    public void setHour(int lcl_arg0)
    {
	hour = lcl_arg0;
    }
    public int getHour()
    {
	return hour;
    }

    public void setMinute(int lcl_arg0)
    {
	minute = lcl_arg0;
    }
    public int getMinute()
    {
	return minute;
    }

    public void setSecond(int lcl_arg0)
    {
	second = lcl_arg0;
    }
    public int getSecond()
    {
	return second;
    }

    public void setIs_date(boolean lcl_arg0)
    {
	is_date = lcl_arg0 ? 1 : 0;
    }
    public boolean getIs_date()
    {
	return is_date == 0 ? false : true;
    }

    public void setZone(String lcl_arg0)
    {
	zone = lcl_arg0;
    }
    public String getZone()
    {
	return zone;
    }

	// --------------------------------------------------------
	// Initialization
	// --------------------------------------------------------

	/**
	 * copy data from an existing struct.
	 */
	private native void init(long obj);

	/**
	 * optimization: init field id cache,
	 */
    private native static void initFIDs();

	/**
	 * load the jni library for this class
	 */
	static {
		System.loadLibrary("ical_jni");
		initFIDs();
	}

	// --------------------------------------------------------
	// Fields
	// --------------------------------------------------------

    private int	month;
    private int	day;
    private int	year;
    private int	hour;
    private int	minute;
    private int	second;
    private int	is_date;
    private String	zone = new String();	//   Converted from char*
}
