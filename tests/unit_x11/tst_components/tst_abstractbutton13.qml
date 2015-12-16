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
        AbstractButton {
            id: suppressTrigger2
            width: units.gu(10)
            height: width
            function trigger(v) {}
        }
        AbstractButton {
            id: suppressTrigger3
            width: units.gu(10)
            height: width
            function trigger(v) { triggered(v) }
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

    Action {
        id: override
        function trigger() {}
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
            signalSpy.target = absButton;
            triggeredSpy.clear();
            loader.click = false;
            loader.longPress = false;
        }

        function test_action() {
            compare(absButton.action, null,"Action is null by default")
            absButton.action = action1
            compare(absButton.action, action1, "Action can be set")
            var numTriggers = action1.triggerCount
            triggeredSpy.target = absButton.action;
            mouseClick(absButton, centerOf(absButton).x, centerOf(absButton).y);
            triggeredSpy.wait(500);
            compare(action1.triggerCount, numTriggers+1, "Button clicked triggers action")
            absButton.action = null
        }

        function test_custom_trigger_function_data() {
            return [
                {tag: "parameterless trigger", testItem: suppressTrigger},
                {tag: "parameted trigger", testItem: suppressTrigger2},
            ];
        }
        function test_custom_trigger_function(data) {
            data.testItem.action = action1;
            triggeredSpy.target = action1;
            mouseClick(data.testItem, centerOf(data.testItem).x, centerOf(data.testItem).y);
            compare(triggeredSpy.count, 0, "Trigger should be overridden");
        }

        function test_custom_trigger_overrides_triggered_bug1524234_data() {
            return [
                {tag: "parameterless trigger", testItem: suppressTrigger, action: action1, triggeredWatch: action1, triggeredCount: 0 },
                {tag: "parameted trigger", testItem: suppressTrigger2, action: action1, triggeredWatch: action1, triggeredCount: 0 },
                {tag: "button trigger fired", testItem: suppressTrigger3, action: action1, triggeredWatch: suppressTrigger3, triggeredCount: 1 },
                {tag: "action trigger fired", testItem: suppressTrigger3, action: action1, triggeredWatch: action1, triggeredCount: 1 },
                {tag: "action trigger overridden", testItem: suppressTrigger3, action: override, triggeredWatch: override, triggeredCount: 0 },
            ];
        }
        function test_custom_trigger_overrides_triggered_bug1524234(data) {
            data.testItem.action = data.action;
            triggeredSpy.target = data.triggeredWatch;
            signalSpy.target = data.testItem;
            data.testItem.forceActiveFocus();
            keyClick(Qt.Key_Space);
            signalSpy.wait(200);
            compare(triggeredSpy.count, data.triggeredCount);
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
