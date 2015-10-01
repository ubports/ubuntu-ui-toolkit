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
import Ubuntu.Test 1.0
import Ubuntu.Components 1.3

Item {
    width: units.gu(40)
    height: units.gu(71)

    Column {
        AbstractButton {
            id: absButton
            width: units.gu(10)
            height: units.gu(10)
        }
        AbstractButton {
            id: absLongTap
            width: units.gu(10)
            height: width
            onPressAndHold: {}
        }
        AbstractButton {
            id: suppressTrigger
            width: units.gu(10)
            height: width
            function trigger() {}
        }
        Loader {
            id: loader
            width: units.gu(10)
            height: units.gu(10)
            sourceComponent: AbstractButton { objectName: "dynamic"}
            property bool click: false
            property bool longPress: false
        }
    }

    Connections {
        id: test
        target: loader.item
        onClicked: loader.click = true
        onPressAndHold: loader.longPress = true
    }

    Action {
        id: action1
        property int triggerCount: 0
        onTriggered: triggerCount++
    }

    SignalSpy {
        id: signalSpy
        target: absButton
        signalName: "clicked"
    }

    SignalSpy {
        id: pressAndHoldSpy
        target: absLongTap
        signalName: "pressAndHold"
    }

    SignalSpy {
        id: triggeredSpy
        signalName: "triggered"
    }

    UbuntuTestCase {
        name: "AbstractButtonAPI"
        when: windowShown

        function cleanup() {
            signalSpy.clear();
            triggeredSpy.clear();
            loader.click = false;
            loader.longPress = false;
        }

        function test_action() {
            compare(absButton.action, null,"Action is null by default")
            absButton.action = action1
            compare(absButton.action, action1, "Action can be set")
            var numTriggers = action1.triggerCount
            absButton.clicked()
            compare(action1.triggerCount, numTriggers+1, "Button clicked triggers action")
            absButton.action = null
        }

        function test_custom_trigger_function() {
            suppressTrigger.action = action1;
            triggeredSpy.target = action1;
            mouseClick(suppressTrigger, centerOf(suppressTrigger).x, centerOf(suppressTrigger).y);
            compare(triggeredSpy.count, 0, "Trigger should be overridden");
        }

        // fixing bugs 1365471 and 1458028
        function test_no_pressAndHold_connected_clicks_bug1365471_bug1458028() {
            signalSpy.target = absButton;
            mouseLongPress(absButton, centerOf(absButton).x, centerOf(absButton).y);
            mouseRelease(absButton, centerOf(absButton).x, centerOf(absButton).y);
            signalSpy.wait();
        }

        // fixing bugs 1365471 and 1458028
        function test_pressAndHold_connected_suppresses_clicks_bug1365471_bug1458028() {
            function testFunc() {}
            signalSpy.target = absButton;
            absLongTap.pressAndHold.connect(testFunc);
            mouseLongPress(absLongTap, centerOf(absLongTap).x, centerOf(absLongTap).y);
            absLongTap.pressAndHold.disconnect(testFunc);
            pressAndHoldSpy.wait();
            mouseRelease(absLongTap, centerOf(absLongTap).x, centerOf(absLongTap).y);
            compare(signalSpy.count, 0, "click() must be suppressed when pressAndHold handler is implemented");
        }

        function test_pressAndHold_emitted_on_connections_bug1495554() {
            mouseLongPress(loader.item, centerOf(loader.item).x, centerOf(loader.item).y);
            mouseRelease(loader.item, centerOf(loader.item).x, centerOf(loader.item).y);
            compare(loader.click, false, "clicked should not be emitted");
            compare(loader.longPress, true, "pressAndHold not captured by Connection");
        }
        function test_clicked_emitted_on_connections_bug1495554() {
            mouseClick(loader.item, centerOf(loader.item).x, centerOf(loader.item).y);
            compare(loader.click, true, "clicked not captured by Connection");
        }
    }
}
