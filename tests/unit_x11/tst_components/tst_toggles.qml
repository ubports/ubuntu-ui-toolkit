/*
 * Copyright 2014 Canonical Ltd.
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
import Ubuntu.Components 1.3

MainView {
    id: main

    width: units.gu(40)
    height: units.gu(71)

    Column {
        Switch {
            id: testSwitch
            checked: true
            property bool checkedNow: true
            onClicked: checkedNow = checked
        }

        CheckBox {
            id: testCheck
            checked: true
            property bool checkedNow: true
            onClicked: checkedNow = checked
        }
    }

    UbuntuTestCase {
        name: "Toggles13"
        when: windowShown

        SignalSpy {
            id: clickedSpy
            signalName: "clicked"
        }

        function cleanup() {
            clickedSpy.clear();
            clickedSpy.target = null;
        }

        function test_toggle_checked_delayed_bug1524234_data() {
            return [
                {tag: "CheckBox", testItem: testCheck, mouse: true},
                {tag: "Switch", testItem: testSwitch, mouse: true},
                {tag: "CheckBox, space key", testItem: testCheck, key: Qt.Key_Space},
                {tag: "Switch, space key", testItem: testSwitch, key: Qt.Key_Space},
            ];
        }
        function test_toggle_checked_delayed_bug1524234(data) {
            data.testItem.checkedNow = data.testItem.checked;
            data.testItem.forceActiveFocus();
            clickedSpy.target = data.testItem;
            if (data.key) {
                keyClick(data.key);
            } else {
                mouseClick(data.testItem, centerOf(data.testItem).x, centerOf(data.testItem).y);
            }
            clickedSpy.wait(400);
            compare(data.testItem.checkedNow, data.testItem.checked);
        }
    }
}

