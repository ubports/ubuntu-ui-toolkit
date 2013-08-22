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
import Ubuntu.Test 0.1
import Ubuntu.Components 0.1

Item {
    id: root
    width: 100
    height: 100

    Alarm {
        id: testAlarm
    }

    AlarmModel {
        id: testModel
    }

    ListView {
        id: roleTest
        model: testModel
        delegate: Item {
            objectName: "roleItem"
            property var roleModel: model
            property bool roleEnabled: model.enabled
            property string roleMessage: message
            property var roleDate: date
            property int roleType: type
            property int roleDaysOfWeek: daysOfWeek
            property url roleSound: sound
        }
    }

    UbuntuTestCase {
        id: testCase
        name: "AlarmAPI"
        when: windowShown

        function clean() {
            var loop = true
            while (loop) {
                loop = false;
                for (var i = 0; testModel.count; i++) {
                    var alarm = testModel.get(i);
                    if (alarm && alarm.message === "test") {
                        alarm.cancel();
                        wait(100);
                        loop = true;
                        break;
                    }
                }
            }
        }

        function initTestCase() {
            clean();
        }

        function cleanupTestCase() {
            clean();
        }

        function test_createOneTimeFail() {
            testAlarm.reset();
            testAlarm.date = new Date();

            testAlarm.save();
            compare(testAlarm.error, Alarm.EarlyDate, 'alarm date must be greater than the current time');
        }

        function test_createOneTimePass() {
            testAlarm.reset();
            testAlarm.message = "test"
            var dt = new Date();
            dt.setMinutes(dt.getMinutes() + 10);
            testAlarm.date = dt;

            testAlarm.save();
            compare(testAlarm.error, Alarm.NoError, 'alarm date must be greater than the current time');
        }

        function test_repeating_AutoDetect() {
            testAlarm.reset();
            testAlarm.message = "test"
            testAlarm.date = new Date();
            testAlarm.type = Alarm.AutoDetect;

            testAlarm.save();
            compare(testAlarm.error, Alarm.NoError, 'repating AutoDetect alarm');
        }

        function test_repeating_Daily() {
            testAlarm.reset();
            testAlarm.message = "test"
            testAlarm.date = new Date();
            testAlarm.type = Alarm.Daily;

            testAlarm.save();
            compare(testAlarm.error, Alarm.NoError, 'repating Daily alarm');
        }

        function test_repeating_givenDay() {
            testAlarm.reset();
            testAlarm.message = "test"
            testAlarm.date = new Date();
            testAlarm.type = Alarm.Repeating;
            testAlarm.daysOfWeek = Alarm.Monday;

            testAlarm.save();
            compare(testAlarm.error, Alarm.NoError, 'repating on a given day alarm');
        }

        function test_repeating_moreDays() {
            testAlarm.reset();
            testAlarm.message = "test"
            testAlarm.date = new Date();
            testAlarm.type = Alarm.Monday | Alarm.Friday;

            testAlarm.save();
            compare(testAlarm.error, Alarm.NoError, 'repating on multiple days alarm');
        }

        function test_setAlarmObject_WrongRecurence() {
            testAlarm.reset();
            testAlarm.message = "test";
            var dt = new Date();
            dt.setMinutes(dt.getMinutes() + 10);
            testAlarm.date = dt;
            testAlarm.type = Alarm.OneTime;
            testAlarm.daysOfWeek = Alarm.Monday | Alarm.Tuesday;

            testAlarm.save();
            compare(testAlarm.error, Alarm.OneTimeOnMoreDays, 'repating on multiple days alarm');
        }

        function test_cancel_Fail() {
            testAlarm.reset();

            testAlarm.cancel();
            compare(testAlarm.error, Alarm.InvalidEvent, "cannot cancel an unregistered alarm");
        }

        function test_cancel_Pass() {
            testAlarm.reset();
            testAlarm.message = "test";
            testAlarm.type = Alarm.OneTime;
            var dt = new Date();
            dt.setMinutes(dt.getMinutes() + 10);
            testAlarm.date = dt;

            testAlarm.save();
            wait(100);
            testAlarm.cancel();
            wait(100);
            compare(testAlarm.error, Alarm.NoError, "alarm canceled");
        }

        function test_updateAlarm_sameType() {
            testAlarm.reset();
            testAlarm.message = "test";
            testAlarm.type = Alarm.OneTime;
            var dt = new Date();
            dt.setMinutes(dt.getMinutes() + 10);
            testAlarm.date = dt;

            testAlarm.save();
            compare(testAlarm.error, Alarm.NoError, "fist alarm added");

            dt.setDate(dt.getDate() + 2);
            testAlarm.date = dt;
            compare(testAlarm.error, Alarm.NoError, "updated alarm");
        }

        function test_updateAlarm_differentType() {
            testAlarm.reset();
            testAlarm.message = "test";
            testAlarm.type = Alarm.OneTime;
            var dt = new Date();
            dt.setMinutes(dt.getMinutes() + 10);
            testAlarm.date = dt;

            testAlarm.save();
            compare(testAlarm.error, Alarm.NoError, "fist alarm added");
            testAlarm.type = Alarm.Repeating;
            compare(testAlarm.error, Alarm.NoError, "updated alarm");
        }

        function test_modelRoles() {
            clean();
            testAlarm.reset();
            testAlarm.message = "test";
            var dt = new Date();
            dt.setMinutes(dt.getMinutes() + 10);
            testAlarm.date = dt;
            testAlarm.type = Alarm.Repeating;
            testAlarm.daysOfWeek = Alarm.Monday;

            testAlarm.save();
            wait(100);
            verify(testModel.count > 0, "alarm added");
            var item = findChild(roleTest, "roleItem");
            verify(item, "has children");
            verify(item.roleModel, "model is defined");
            verify(item.roleEnabled, "enabled is defined");
            verify(item.roleMessage, "message is defined");
            verify(item.roleDate, "date is defined");
            verify(item.roleType, "type is defined");
            verify(item.roleDaysOfWeek, "daysOfWeek is defined");
            verify(item.roleSound, "sound is defined");
        }
    }
}
