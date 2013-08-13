/*
 * Copyright 2012 Canonical Ltd.
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
import Ubuntu.Components 0.1

TestCase {
    name: "AlarmAPI"

    function cleanupTestCase() {

    }

    function test_createOneTimeFail() {
        var dt = new Date();

        verify(!Alarms.set(dt), 'alarm date must be greater than the current time');
    }

    function test_createOneTimePass() {
        var dt = new Date();
        dt.setMinutes(dt.getMinutes() + 10);

        verify(Alarms.set(dt, "test_createOneTimePass"), "one time alarm");
    }

    function test_stop() {
        verify(false, "stop");
    }
}
