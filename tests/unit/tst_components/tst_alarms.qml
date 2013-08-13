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
    id: testCase
    name: "AlarmAPI"

    property Alarm alarm
    property var alarms: AlarmManager.alarms

    SignalSpy {
        id: spy
        signalName: "alarmsChanged"
        target: AlarmManager
    }

    Alarm {
        id: testAlarm
    }

    function containsAlarm(alarm) {
        for (var i = 0; AlarmManager.alarms.length; i++) {
            if (AlarmManager.alarms[i] === alarm) {
                return true;
            }
        }
        return false;
    }

    function initTestCase() {
        spy.clear();
    }

    function cleanupTestCase() {
        var loop = true
        while (loop) {
            loop = false;
            for (var i = 0; testCase.alarms.length; i++) {
                if (testCase.alarms[i].message === "test") {
                    AlarmManager.cancel(testCase.alarms[i]);
                    loop = true;
                    break;
                }
            }
        }
    }

    function test_createOneTimeFail() {
        var dt = new Date();

        verify(!AlarmManager.setOneTime(dt, "test"), 'alarm date must be greater than the current time');
    }

    function test_createOneTimePass() {
        var dt = new Date();
        dt.setMinutes(dt.getMinutes() + 10);
        spy.clear();

        verify(AlarmManager.setOneTime(dt, "test"), "one time alarm");
        tryCompare(spy, "count", 2, 100); // ???
    }

    function test_repeating_AutoDetect() {
        var dt = new Date();
        spy.clear();

        verify(AlarmManager.setRepeating(dt, Alarm.AutoDetect, "test"), "repating AutoDetect alarm");
        tryCompare(spy, "count", 2, 100); // ???
    }

    function test_repeating_Daily() {
        var dt = new Date();
        spy.clear();

        verify(AlarmManager.setRepeating(dt, Alarm.Daily, "test"), "repating Daily alarm");
        tryCompare(spy, "count", 2, 100); // ???
    }

    function test_repeating_givenDay() {
        var dt = new Date();
        spy.clear();

        verify(AlarmManager.setRepeating(dt, Alarm.Monday, "test"), "repating on given day alarm");
        tryCompare(spy, "count", 2, 100); // ???
    }

    function test_repeating_moreDays() {
        var dt = new Date();
        spy.clear();

        verify(AlarmManager.setRepeating(dt, Alarm.Monday | Alarm.Friday, "test"), "repating on multiple days alarm");
        tryCompare(spy, "count", 2, 100); // ???
    }

    function test_oneTime_Object_Fail() {
        testAlarm.reset();
        testAlarm.message = "test";

        verify(!AlarmManager.set(testAlarm), "cannot create one time alarm");
    }

    function test_oneTime_Object_Pass() {
        testAlarm.reset();
        testAlarm.message = "test";
        var dt = new Date();
        dt.setMinutes(dt.getMinutes() + 10);
        testAlarm.date = dt;
        spy.clear();

        verify(AlarmManager.set(testAlarm), "cannot create one time alarm");
        tryCompare(spy, "count", 2, 100); // ???
    }

    function test_setAlarmObject_WrongRecurence() {
        testAlarm.reset();
        testAlarm.message = "test";
        var dt = new Date();
        dt.setMinutes(dt.getMinutes() + 10);
        testAlarm.date = dt;
        testAlarm.type = Alarm.OneTime;
        testAlarm.daysOfWeek = Alarm.Monday | Alarm.Tuesday;

        verify(!AlarmManager.set(testAlarm), "wrong alarm type");
    }

    function test_setAlarmObject_RepeatingDaily() {
        testAlarm.reset();
        testAlarm.message = "test";
        testAlarm.type = Alarm.Repeating;
        testAlarm.daysOfWeek = Alarm.Daily;
        spy.clear();

        verify(AlarmManager.set(testAlarm), "creating daily repeating alarm");
        tryCompare(spy, "count", 2, 100); // ???
    }

    function test_setAlarmObject_RepeatingOnAGivenDay() {
        testAlarm.reset();
        testAlarm.message = "test";
        testAlarm.type = Alarm.Repeating;
        testAlarm.daysOfWeek = Alarm.Monday;
        spy.clear();

        verify(AlarmManager.set(testAlarm), "creating repeating alarm on a given day");
        tryCompare(spy, "count", 2, 100); // ???
    }

    function test_setAlarmObject_RepeatingOnMultipleDays() {
        testAlarm.reset();
        testAlarm.message = "test";
        testAlarm.type = Alarm.Repeating;
        testAlarm.daysOfWeek = Alarm.Monday | Alarm.Tuesday | Alarm.Friday;
        spy.clear();

        verify(AlarmManager.set(testAlarm), "creating repeating alarm on multiple days");
        tryCompare(spy, "count", 2, 100); // ???
    }

    function test_cancel_Fail() {
        testAlarm.reset();

        verify(!AlarmManager.cancel(testAlarm), "cannot cancel an unregistered alarm");
    }

    function test_cancel_Pass() {
        testAlarm.reset();
        testAlarm.message = "test";
        testAlarm.type = Alarm.OneTime;
        var dt = new Date();
        dt.setMinutes(dt.getMinutes() + 10);
        testAlarm.date = dt;

        verify(AlarmManager.set(testAlarm), "alarm added to be removed");
        verify(AlarmManager.cancel(testAlarm), "alarm cancelled");
    }

    function test_updateAlarm_sameType() {
        testAlarm.reset();
        testAlarm.message = "test";
        testAlarm.type = Alarm.OneTime;
        var dt = new Date();
        dt.setMinutes(dt.getMinutes() + 10);
        testAlarm.date = dt;

        verify(AlarmManager.set(testAlarm), "fist alarm added");
        dt.setDate(dt.getDate() + 2);
        verify(AlarmManager.set(testAlarm), "updated alarm");
    }

    function test_updateAlarm_differentType() {
        testAlarm.reset();
        testAlarm.message = "test";
        testAlarm.type = Alarm.OneTime;
        var dt = new Date();
        dt.setMinutes(dt.getMinutes() + 10);
        testAlarm.date = dt;

        verify(AlarmManager.set(testAlarm), "fist alarm added");
        testAlarm.type = Alarm.Repeating;
        verify(AlarmManager.set(testAlarm), "updated alarm");
    }
}
