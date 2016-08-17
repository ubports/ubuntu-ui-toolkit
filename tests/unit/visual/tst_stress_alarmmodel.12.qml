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
import Ubuntu.Components 1.2

MainView {
    width: units.gu(40)
    height: units.gu(71)

    AlarmModel {
        id: model
    }

    Alarm {
        id: testAlarm
    }

    UbuntuTestCase {
        name: "AlarmModelStressTest"
        when: windowShown

        SignalSpy {
            id: modelSpy
            signalName: "modelReset"
            target: model
        }

        property string testAlarmMessage: "test_"

        function createAlarm(pattern, idx) {
            testAlarm.reset();
            testAlarm.message = pattern + idx;
            var dt = new Date();
            dt.setMinutes(dt.getMinutes() + 10);
            testAlarm.date = dt;
            modelSpy.signalName = "rowsInserted";
            testAlarm.save();
            modelSpy.wait();
        }

        function clearTestAlarms(pattern) {
            var i = 0;
            modelSpy.signalName = "rowsRemoved";
            while (i < model.count) {
                var alarm = model.get(i);
                // this will fail if get() returns NULL
                if (alarm.message.indexOf(pattern) == 0) {
                    alarm.cancel();
                    modelSpy.wait();
                    modelSpy.clear();
                    i = 0;
                } else {
                    i++;
                }
            }
        }

        // need to create a huge amount of alarms then cleanone part of it
        function initTestCase() {
            for (var i = 0; i < 20; i++) {
                createAlarm(testAlarmMessage, i)
            }
            for (var i = 0; i < 40; i++) {
                createAlarm("testAlarm_", i)
            }
        }
        function cleanupTestCase() {
            clearTestAlarms(testAlarmMessage);
        }

        function test_remove_alarms()
        {
            clearTestAlarms("testAlarm_");
        }
    }
}
