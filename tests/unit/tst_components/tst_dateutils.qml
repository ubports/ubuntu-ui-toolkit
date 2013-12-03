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
import Ubuntu.Test 0.1
import Ubuntu.Components 0.1

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

    function test_compare() {

    }

    function test_isValid() {

    }

    function test_daysInMonth() {

    }

    function test_leapYear() {

    }

    function test_monthsTo() {

    }

    function test_daysTo() {

    }

    function test_getWeek() {

    }
}
