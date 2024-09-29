#!/usr/bin/env python3
# GI_TYPELIB_PATH=$PREFIX/lib/girepository-1.0/ ./recurrence-type.py

###############################################################################
#
# SPDX-FileCopyrightText: 2015 William Yu <williamyu@gnome.org>
#
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
#
###############################################################################

''' Test Python bindings for libical recurrence '''

import gi
gi.require_version('ICalGLib', '3.0')
from gi.repository import ICalGLib  # nopep8 # pylint: disable=wrong-import-position

weekday = ICalGLib.Recurrence.day_day_of_week(0)
assert weekday == ICalGLib.RecurrenceWeekday.NO_WEEKDAY
weekday = ICalGLib.Recurrence.day_day_of_week(1)
assert weekday == ICalGLib.RecurrenceWeekday.SUNDAY_WEEKDAY

assert ICalGLib.Recurrence.day_position(15) == 1
assert ICalGLib.Recurrence.day_position(16) == 2
assert ICalGLib.Recurrence.day_position(25) == 3

encoded = ICalGLib.Recurrence.encode_day(ICalGLib.RecurrenceWeekday.MONDAY_WEEKDAY, 0)
assert ICalGLib.Recurrence.day_day_of_week(encoded) == ICalGLib.RecurrenceWeekday.MONDAY_WEEKDAY
assert ICalGLib.Recurrence.day_position(encoded) == 0

encoded = ICalGLib.Recurrence.encode_day(ICalGLib.RecurrenceWeekday.THURSDAY_WEEKDAY, -3)
assert ICalGLib.Recurrence.day_day_of_week(encoded) == ICalGLib.RecurrenceWeekday.THURSDAY_WEEKDAY
assert ICalGLib.Recurrence.day_position(encoded) == -3

encoded = ICalGLib.Recurrence.encode_day(ICalGLib.RecurrenceWeekday.FRIDAY_WEEKDAY, 2)
assert ICalGLib.Recurrence.day_day_of_week(encoded) == ICalGLib.RecurrenceWeekday.FRIDAY_WEEKDAY
assert ICalGLib.Recurrence.day_position(encoded) == 2

encoded = ICalGLib.Recurrence.encode_month(3, 0)
assert ICalGLib.Recurrence.month_month(encoded) == 3
assert not ICalGLib.Recurrence.month_is_leap(encoded)

encoded = ICalGLib.Recurrence.encode_month(12, 1)
assert ICalGLib.Recurrence.month_month(encoded) == 12
assert ICalGLib.Recurrence.month_is_leap(encoded)

string = "COUNT=10;FREQ=DAILY"
recurrence = ICalGLib.Recurrence.new_from_string(string)
assert recurrence.to_string() == "FREQ=DAILY;COUNT=10"

recurrence.set_by(ICalGLib.RecurrenceByRule.BY_SECOND, 0, 1)
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_SECOND, 0) == 1
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_SECOND, 1) == 0
assert recurrence.get_by_array_size(ICalGLib.RecurrenceByRule.BY_SECOND) == 1
array = recurrence.get_by_array(ICalGLib.RecurrenceByRule.BY_SECOND)
assert array[0] == 1
assert len(array) == 1
array = [100, 101]
assert len(array) == 2
recurrence.set_by_array(ICalGLib.RecurrenceByRule.BY_SECOND, array)
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_SECOND, 0) == 100
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_SECOND, 1) == 101
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_SECOND, 2) == 0
array = recurrence.get_by_array(ICalGLib.RecurrenceByRule.BY_SECOND)
assert array[0] == 100
assert array[1] == 101
assert len(array) == 2

recurrence.set_by(ICalGLib.RecurrenceByRule.BY_MINUTE, 0, 2)
recurrence.resize_by_array(ICalGLib.RecurrenceByRule.BY_MINUTE, 1)
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_MINUTE, 0) == 2
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_MINUTE, 1) == 0
array = recurrence.get_by_array(ICalGLib.RecurrenceByRule.BY_MINUTE, )
assert array[0] == 2
assert len(array) == 1
array = [200, 201]
assert len(array) == 2
recurrence.set_by_array(ICalGLib.RecurrenceByRule.BY_MINUTE, array)
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_MINUTE, 0) == 200
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_MINUTE, 1) == 201
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_MINUTE, 2) == 0
array = recurrence.get_by_array(ICalGLib.RecurrenceByRule.BY_MINUTE, )
assert array[0] == 200
assert array[1] == 201
assert len(array) == 2

recurrence.set_by(ICalGLib.RecurrenceByRule.BY_HOUR, 0, 3)
recurrence.resize_by_array(ICalGLib.RecurrenceByRule.BY_HOUR, 1)
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_HOUR, 0) == 3
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_HOUR, 1) == 0
array = recurrence.get_by_array(ICalGLib.RecurrenceByRule.BY_HOUR, )
assert array[0] == 3
assert len(array) == 1
array = [300, 301]
assert len(array) == 2
recurrence.set_by_array(ICalGLib.RecurrenceByRule.BY_HOUR, array)
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_HOUR, 0) == 300
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_HOUR, 1) == 301
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_HOUR, 2) == 0
array = recurrence.get_by_array(ICalGLib.RecurrenceByRule.BY_HOUR, )
assert array[0] == 300
assert array[1] == 301
assert len(array) == 2

recurrence.set_by(ICalGLib.RecurrenceByRule.BY_DAY, 0, 4)
recurrence.resize_by_array(ICalGLib.RecurrenceByRule.BY_DAY, 1)
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_DAY, 0) == 4
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_DAY, 1) == 0
array = recurrence.get_by_array(ICalGLib.RecurrenceByRule.BY_DAY, )
assert array[0] == 4
assert len(array) == 1
array = [400, 401]
assert len(array) == 2
recurrence.set_by_array(ICalGLib.RecurrenceByRule.BY_DAY, array)
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_DAY, 0) == 400
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_DAY, 1) == 401
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_DAY, 2) == 0
array = recurrence.get_by_array(ICalGLib.RecurrenceByRule.BY_DAY, )
assert array[0] == 400
assert array[1] == 401
assert len(array) == 2

recurrence.set_by(ICalGLib.RecurrenceByRule.BY_MONTH_DAY, 0, 5)
recurrence.resize_by_array(ICalGLib.RecurrenceByRule.BY_MONTH_DAY, 1)
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_MONTH_DAY, 0) == 5
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_MONTH_DAY, 1) == 0
array = recurrence.get_by_array(ICalGLib.RecurrenceByRule.BY_MONTH_DAY, )
assert array[0] == 5
assert len(array) == 1
array = [500, 501]
assert len(array) == 2
recurrence.set_by_array(ICalGLib.RecurrenceByRule.BY_MONTH_DAY, array)
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_MONTH_DAY, 0) == 500
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_MONTH_DAY, 1) == 501
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_MONTH_DAY, 2) == 0
array = recurrence.get_by_array(ICalGLib.RecurrenceByRule.BY_MONTH_DAY, )
assert array[0] == 500
assert array[1] == 501
assert len(array) == 2

recurrence.set_by(ICalGLib.RecurrenceByRule.BY_YEAR_DAY, 0, 6)
recurrence.resize_by_array(ICalGLib.RecurrenceByRule.BY_YEAR_DAY, 1)
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_YEAR_DAY, 0) == 6
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_YEAR_DAY, 1) == 0
array = recurrence.get_by_array(ICalGLib.RecurrenceByRule.BY_YEAR_DAY, )
assert array[0] == 6
assert len(array) == 1
array = [600, 601]
assert len(array) == 2
recurrence.set_by_array(ICalGLib.RecurrenceByRule.BY_YEAR_DAY, array)
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_YEAR_DAY, 0) == 600
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_YEAR_DAY, 1) == 601
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_YEAR_DAY, 2) == 0
array = recurrence.get_by_array(ICalGLib.RecurrenceByRule.BY_YEAR_DAY, )
assert array[0] == 600
assert array[1] == 601
assert len(array) == 2

recurrence.set_by(ICalGLib.RecurrenceByRule.BY_WEEK_NO, 0, 7)
recurrence.resize_by_array(ICalGLib.RecurrenceByRule.BY_WEEK_NO, 1)
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_WEEK_NO, 0) == 7
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_WEEK_NO, 1) == 0
array = recurrence.get_by_array(ICalGLib.RecurrenceByRule.BY_WEEK_NO, )
assert array[0] == 7
assert len(array) == 1
array = [700, 701]
assert len(array) == 2
recurrence.set_by_array(ICalGLib.RecurrenceByRule.BY_WEEK_NO, array)
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_WEEK_NO, 0) == 700
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_WEEK_NO, 1) == 701
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_WEEK_NO, 2) == 0
array = recurrence.get_by_array(ICalGLib.RecurrenceByRule.BY_WEEK_NO, )
assert array[0] == 700
assert array[1] == 701
assert len(array) == 2

recurrence.set_by(ICalGLib.RecurrenceByRule.BY_MONTH, 0, 8)
recurrence.resize_by_array(ICalGLib.RecurrenceByRule.BY_MONTH, 1)
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_MONTH, 0) == 8
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_MONTH, 1) == 0
array = recurrence.get_by_array(ICalGLib.RecurrenceByRule.BY_MONTH, )
assert array[0] == 8
assert len(array) == 1
array = [800, 801]
assert len(array) == 2
recurrence.set_by_array(ICalGLib.RecurrenceByRule.BY_MONTH, array)
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_MONTH, 0) == 800
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_MONTH, 1) == 801
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_MONTH, 2) == 0
array = recurrence.get_by_array(ICalGLib.RecurrenceByRule.BY_MONTH, )
assert array[0] == 800
assert array[1] == 801
assert len(array) == 2

recurrence.set_by(ICalGLib.RecurrenceByRule.BY_SET_POS, 0, 9)
recurrence.resize_by_array(ICalGLib.RecurrenceByRule.BY_SET_POS, 1)
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_SET_POS, 0) == 9
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_SET_POS, 1) == 0
array = recurrence.get_by_array(ICalGLib.RecurrenceByRule.BY_SET_POS, )
assert array[0] == 9
assert len(array) == 1
array = [900, 901]
assert len(array) == 2
recurrence.set_by_array(ICalGLib.RecurrenceByRule.BY_SET_POS, array)
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_SET_POS, 0) == 900
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_SET_POS, 1) == 901
assert recurrence.get_by(ICalGLib.RecurrenceByRule.BY_SET_POS, 2) == 0
array = recurrence.get_by_array(ICalGLib.RecurrenceByRule.BY_SET_POS, )
assert array[0] == 900
assert array[1] == 901
assert len(array) == 2

recurrence.set_by(ICalGLib.RecurrenceByRule.BY_SECOND, 0, 13)
by_second = recurrence.get_by_array(ICalGLib.RecurrenceByRule.BY_SECOND)
assert by_second[0] == 13

recurrence = ICalGLib.Recurrence.new_from_string(string)

assert ICalGLib.Recurrence.weekday_from_string("MO") == ICalGLib.RecurrenceWeekday.MONDAY_WEEKDAY

start = 100000
result = ICalGLib.recur_expand_recurrence(string, start, 10)
secsPerDay = 24*60*60
for i in range(0, 9):
    assert result[i] == start + i*secsPerDay

string = "19970101T183248Z/19970102T071625Z"

period = ICalGLib.Period.new_from_string(string)
start = period.get_start()

iterator = ICalGLib.RecurIterator.new(recurrence, start)
timetype = iterator.next()
day = timetype.get_day()
ref = 1
while day != 0:
    assert day == ref
    ref += 1
    timetype = iterator.next()
    day = timetype.get_day()

recurrence = ICalGLib.Recurrence.new_from_string("FREQ=YEARLY;COUNT=10")
start = ICalGLib.Time.new_from_string("20161224T000000Z")

iterator = ICalGLib.RecurIterator.new(recurrence, start)

start = ICalGLib.Time.new_from_string("20181224T000000Z")

assert ICalGLib.RecurIterator.set_start(iterator, start) == 0

recurrence = ICalGLib.Recurrence.new_from_string("FREQ=YEARLY")
start = ICalGLib.Time.new_from_string("20161224T000000Z")

iterator = ICalGLib.RecurIterator.new(recurrence, start)

start = ICalGLib.Time.new_from_string("20181224T000000Z")

assert ICalGLib.RecurIterator.set_start(iterator, start) == 1
timetype = iterator.next()

assert timetype.get_year() == 2018

timetype = iterator.next()
assert timetype.get_year() == 2019
