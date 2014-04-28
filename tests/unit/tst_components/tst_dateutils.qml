/*
 * Copyright 2013 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Test 1.0
import Ubuntu.Components 1.1

/*
 Implementation note:
 We use Date(year, month, day) everywhere, Date(format) cannot be used as of Qt 5.2
 */

TestCase {
    name: "DateUtilsAPI"

    function test_midnight() {
        var midnight = Date.prototype.midnight.call(new Date);
        compare(midnight.getHours(), 0, "hour is zero");
        compare(midnight.getMinutes(), 0, "minutes is zero");
        compare(midnight.getSeconds(), 0, "seconds is zero");

        midnight = new Date();
        var date = midnight.midnight();
        compare(midnight.getHours(), 0, "hour is zero");
        compare(midnight.getMinutes(), 0, "minutes is zero");
        compare(midnight.getSeconds(), 0, "seconds is zero");

        compare(date.getHours(), 0, "hour is zero");
        compare(date.getMinutes(), 0, "minutes is zero");
        compare(date.getSeconds(), 0, "seconds is zero");
    }

    function test_getInvalidDate() {
        var date = Date.prototype.getInvalidDate.call();
        verify(date.getFullYear() < 0, "year is invalid");
    }

    function test_0isValid() {
        compare((new Date()).isValid(), true, "any date is valid");
        compare((new Date(-1, -1)).isValid(), false, "a year and month set to -1 is invalid");
        compare((Date.prototype.getInvalidDate.call()).isValid(), false, "getInvalidDate() returns an invalid date");

        compare(Date.prototype.isValid.call(null), false, "null date is invalid");
        compare(Date.prototype.isValid.call(undefined), false, "undefined date is invalid");
    }

    function test_daysInMonth_data() {
        var date = new Date;
        return [
            { "month": 0, "days": 31 },
            { "month": 1, "days": 28 + (date.leapYear() ? 1 : 0) },
            { "month": 2, "days": 31 },
            { "month": 3, "days": 30 },
            { "month": 4, "days": 31 },
            { "month": 5, "days": 30 },
            { "month": 6, "days": 31 },
            { "month": 7, "days": 31 },
            { "month": 8, "days": 30 },
            { "month": 9, "days": 31 },
            { "month": 10, "days": 30 },
            { "month": 11, "days": 31 },
        ];
    }
    function test_daysInMonth(data) {
        var date = new Date;
        date.setDate(1);
        date.setMonth(data.month);
        compare(date.daysInMonth(), data.days, "daysInMonth() failed for month " + (data.month + 1));
    }

    function test_leapYear_data() {
        return [
            {"year": 2013, "result": false},
            {"year": 2100, "result": false},
            {"year": 2000, "result": true},
            {"year": 2004, "result": true},
        ];
    }

    function test_leapYear(data) {
        var date = new Date(data.year, 0, 1)
        compare(date.leapYear(), data.result, "leapYear() failed for " + data.year);
    }

    function test_monthsTo_data() {
        return [
            {"from": new Date(2000, 0, 1), "to": new Date(2000, 0, 31), "result": 0},
            {"from": new Date(2000, 0, 1), "to": new Date(2000, 1, 1), "result": 1},
            {"from": new Date(2000, 0, 31), "to": new Date(2000, 1, 1), "result": 1},
            {"from": new Date(2000, 0, 31), "to": new Date(2001, 0, 1), "result": 12},
            {"from": new Date(2000, 0, 31), "to": new Date(2002, 0, 31), "result": 24},
        ];
    }
    function test_monthsTo(data) {
        compare(data.from.monthsTo(data.to), data.result, "monthsTo() failed for " +
                Qt.formatDate(data.from, "yyyy/MM/dd") + " - " + Qt.formatDate(data.to, "yyyy/MM/dd"));
    }

    function test_daysTo_data() {
        return [
            {"from": new Date(2000, 0, 1), "to": new Date(2000, 0, 31), "result": 30},
            {"from": new Date(2000, 0, 1), "to": new Date(2000, 1, 1), "result": 31},
            {"from": new Date(2000, 0, 31), "to": new Date(2000, 1, 1), "result": 1},
            {"from": new Date(2000, 0, 1), "to": new Date(2001, 0, 1), "result": 366}, // 2000 is a leap year
            {"from": new Date(2000, 0, 1), "to": new Date(2000, 0, 2), "result": 1},
            {"from": new Date(2000, 6, 1), "to": new Date(2000, 8, 1), "result": 62},
        ];
    }
    function test_daysTo(data) {
        compare(data.from.daysTo(data.to), data.result, "daysTo() failed for " +
                Qt.formatDate(data.from, "yyyy/MM/dd") + " - " + Qt.formatDate(data.to, "yyyy/MM/dd"));
    }

    function test_getWeek_data() {
        return [
            {"date": new Date(2013, 0, 1), "week": 1},
            {"date": new Date(2012, 11, 31), "week": 1},
            {"date": new Date(2013, 11, 3), "week": 49},
            {"date": new Date(2013, 11, 31), "week": 1},
            {"date": new Date(2011, 11, 31), "week": 52},
        ];
    }
    function test_getWeek(data) {
        compare(data.date.getWeek(), data.week, "getWeek() failed for " + Qt.formatDate(data.date, "yyyy/MM/dd"));
    }
}
