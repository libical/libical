#!/usr/bin/env python3
#GI_TYPELIB_PATH=$PREFIX/lib/girepository-1.0/ ./recurrence-type.py

###############################################################################
#
# Copyright (C) 2015 William Yu <williamyu@gnome.org>
#
# This library is free software; you can redistribute it and/or modify
# it under the terms of either:
#
#   The LGPL as published by the Free Software Foundation, version
#   2.1, available at: http://www.gnu.org/licenses/lgpl-2.1.txt
#
# Or:
#
#   The Mozilla Public License Version 2.0. You may obtain a copy of
#   the License at http://www.mozilla.org/MPL/
#
###############################################################################

from gi.repository import ICalGLib

weekday = ICalGLib.RecurrenceType.day_day_of_week(0);
assert(weekday == ICalGLib.RecurrenceTypeWeekday.NO_WEEKDAY);
weekday = ICalGLib.RecurrenceType.day_day_of_week(1);
assert(weekday == ICalGLib.RecurrenceTypeWeekday.SUNDAY_WEEKDAY);

assert(ICalGLib.RecurrenceType.day_position(15) == 1);
assert(ICalGLib.RecurrenceType.day_position(16) == 2);
assert(ICalGLib.RecurrenceType.day_position(25) == 3);

encoded = ICalGLib.RecurrenceType.encode_day(ICalGLib.RecurrenceTypeWeekday.MONDAY_WEEKDAY, 0);
assert(ICalGLib.RecurrenceType.day_day_of_week(encoded) == ICalGLib.RecurrenceTypeWeekday.MONDAY_WEEKDAY);
assert(ICalGLib.RecurrenceType.day_position(encoded) == 0);

encoded = ICalGLib.RecurrenceType.encode_day(ICalGLib.RecurrenceTypeWeekday.THURSDAY_WEEKDAY, -3);
assert(ICalGLib.RecurrenceType.day_day_of_week(encoded) == ICalGLib.RecurrenceTypeWeekday.THURSDAY_WEEKDAY);
assert(ICalGLib.RecurrenceType.day_position(encoded) == -3);

encoded = ICalGLib.RecurrenceType.encode_day(ICalGLib.RecurrenceTypeWeekday.FRIDAY_WEEKDAY, 2);
assert(ICalGLib.RecurrenceType.day_day_of_week(encoded) == ICalGLib.RecurrenceTypeWeekday.FRIDAY_WEEKDAY);
assert(ICalGLib.RecurrenceType.day_position(encoded) == 2);

encoded = ICalGLib.RecurrenceType.encode_month(3, 0);
assert(ICalGLib.RecurrenceType.month_month(encoded) == 3);
assert(not ICalGLib.RecurrenceType.month_is_leap(encoded));

encoded = ICalGLib.RecurrenceType.encode_month(12, 1);
assert(ICalGLib.RecurrenceType.month_month(encoded) == 12);
assert(ICalGLib.RecurrenceType.month_is_leap(encoded));

string = "COUNT=10;FREQ=DAILY";
recurrence = ICalGLib.RecurrenceType.from_string(string);
assert(recurrence.as_string_r() == "FREQ=DAILY;COUNT=10");

by_second = recurrence.get_by_second();
# The value is dependent on the libical version.
assert len(by_second) == 61 or len(by_second) == 62;
by_minute = recurrence.get_by_minute();
assert len(by_minute) == 61;
by_hour = recurrence.get_by_hour();
assert len(by_hour) == 25;
by_day = recurrence.get_by_day();
# The value is dependent on the libical version.
assert len(by_day) == 364 or len(by_day) == 386;
by_month_day = recurrence.get_by_month_day();
assert len(by_month_day) == 32;
by_year_day = recurrence.get_by_year_day();
# The value is dependent on the libical version.
assert len(by_year_day) == 367 or len(by_year_day) == 386;
by_week_no = recurrence.get_by_week_no();
# The value is dependent on the libical version.
assert len(by_week_no) == 54 or len(by_week_no) == 56;
by_month = recurrence.get_by_month();
# The value is dependent on the libical version.
assert len(by_month) == 13 or len(by_month) == 14;
by_set_pos = recurrence.get_by_set_pos();
# The value is dependent on the libical version.
assert len(by_set_pos) == 367 or len(by_set_pos) == 386;

recurrence.set_by_second(0, 1);
by_second = recurrence.get_by_second();
assert by_second[0] == 1;

recurrence = ICalGLib.RecurrenceType.from_string(string);

assert(ICalGLib.recur_string_to_weekday("MO") == ICalGLib.RecurrenceTypeWeekday.MONDAY_WEEKDAY);

start = 100000;
result = ICalGLib.recur_expand_recurrence(string, start, 10);
secs_per_day = 24*60*60;
for i in range(0, 9):
    assert(result[i] == start + i*secs_per_day);

string = "19970101T183248Z/19970102T071625Z";

period = ICalGLib.PeriodType.from_string(string);
start = period.get_start();

iter = ICalGLib.RecurIterator.new(recurrence, start);
timetype = iter.next();
day = timetype.get_day();
ref = 1;
while day != 0:
    assert(day == ref);
    ref += 1;
    timetype = iter.next();
    day = timetype.get_day();

recurrence = ICalGLib.RecurrenceType.from_string("FREQ=YEARLY;COUNT=10");
start = ICalGLib.time_from_string("20161224T000000Z");

iter = ICalGLib.RecurIterator.new(recurrence, start);

start = ICalGLib.time_from_string("20181224T000000Z");

assert ICalGLib.RecurIterator.set_start(iter, start) == 0;

recurrence = ICalGLib.RecurrenceType.from_string("FREQ=YEARLY");
start = ICalGLib.time_from_string("20161224T000000Z");

iter = ICalGLib.RecurIterator.new(recurrence, start);

start = ICalGLib.time_from_string("20181224T000000Z");

assert ICalGLib.RecurIterator.set_start(iter, start) == 1;
timetype = iter.next();

assert timetype.get_year() == 2018;

timetype = iter.next();
assert timetype.get_year() == 2019;
