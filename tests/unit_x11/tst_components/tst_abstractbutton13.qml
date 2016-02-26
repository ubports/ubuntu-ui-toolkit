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
        Item {
            // have enough space for the test subject
            width: units.gu(10)
            height: units.gu(10)
            AbstractButton {
                id: buttonWithSensing
                anchors.centerIn: parent
            }
        }
        Rectangle {
            color: "red"
            width: units.gu(10)
            height: units.gu(10)
            AbstractButton {
                id: increasedSensing
                anchors.centerIn: parent
                width: units.gu(4)
                height: units.gu(4)
                sensingMargins {
                    left: units.gu(2)
                    right: units.gu(2)
                    top: units.gu(2)
                    bottom: units.gu(2)
                }
                style: Item {
                    anchors.fill: parent
                    Rectangle {
                        color: "blue"
                        parent: styledItem.__mouseArea
                        anchors.fill: parent
                    }
                }
            }
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
            buttonWithSensing.sensingMargins.left = 0;
            buttonWithSensing.sensingMargins.top = 0;
            buttonWithSensing.sensingMargins.right = 0;
            buttonWithSensing.sensingMargins.bottom = 0;
            buttonWithSensing.sensingMargins.all = 0;
            buttonWithSensing.width = 0;
            buttonWithSensing.height = 0;
            signalSpy.target = absButton;
            signalSpy.clear();
            signalSpy.target = absButton;
            triggeredSpy.clear();
            loader.click = false;
            loader.longPress = false;
        }

        function initTestCase() {
            compare(buttonWithSensing.sensingMargins.left, 0);
            compare(buttonWithSensing.sensingMargins.right, 0);
            compare(buttonWithSensing.sensingMargins.top, 0);
            compare(buttonWithSensing.sensingMargins.bottom, 0);
            compare(buttonWithSensing.sensingMargins.all, 0);
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

        function test_sensing_area_data() {
            return [
                // margins is [left, top, right, bottom]
                {tag: "zero size, no margins, click in visual", sizeGU: [0, 0], clickGU: [0, 0], sensingGU: [4, 4]},
                {tag: "zero size, no margins, click in sensing", sizeGU: [0, 0], clickGU: [4, 4], sensingGU: [4, 4]},
                {tag: "zero size, 1GU margins, click in visual", sizeGU: [0, 0], marginsGU: [1, 1, 1, 1], clickGU: [0, 0], sensingGU: [4, 4]},
                {tag: "zero size, 1GU margins, click in sensing", sizeGU: [0, 0], marginsGU: [1, 1, 1, 1], clickGU: [4, 4], sensingGU: [4, 4]},
                {tag: "zero size, 3GU margins horizontal, click in sensing", sizeGU: [0, 0], marginsGU: [3, 0, 3, 0], clickGU: [4, 4], sensingGU: [6, 4]},
                {tag: "zero size, 3GU margins vertical, click in sensing", sizeGU: [0, 0], marginsGU: [0, 3, 0, 3], clickGU: [4, 4], sensingGU: [4, 6]},
                {tag: "zero size, 3GU margins around, click in sensing", sizeGU: [0, 0], marginsGU: [3, 3, 3, 3], clickGU: [4, 4], sensingGU: [6, 6]},

                {tag: "3x3GU size, no margins, click in visual", sizeGU: [3, 3], clickGU: [0, 0], sensingGU: [4, 4]},
                {tag: "3x3GU size, no margins, click in sensing", sizeGU: [3, 3], clickGU: [4, 4], sensingGU: [4, 4]},
                {tag: "3x3GU size, 1GU margins, click in visual", sizeGU: [3, 3], marginsGU: [1, 1, 1, 1], clickGU: [0, 0], sensingGU: [5, 5]},
                {tag: "3x3GU size, 1GU margins, click in sensing", sizeGU: [3, 3], marginsGU: [1, 1, 1, 1], clickGU: [4, 4], sensingGU: [5, 5]},
                {tag: "3x3GU size, 3GU margins horizontal, click in sensing", sizeGU: [3, 3], marginsGU: [3, 0, 3, 0], clickGU: [4, 4], sensingGU: [9, 4]},
                {tag: "3x3GU size, 3GU margins vertical, click in sensing", sizeGU: [3, 3], marginsGU: [0, 3, 0, 3], clickGU: [4, 4], sensingGU: [4, 9]},
                {tag: "3x3GU size, 3GU margins around, click in sensing", sizeGU: [3, 3], marginsGU: [3, 3, 3, 3], clickGU: [4, 4], sensingGU: [9, 9]},

                {tag: "5x5GU size, no margins, click in visual", sizeGU: [5, 5], clickGU: [0, 0], sensingGU: [5, 5]},
                {tag: "5x5GU size, no margins, click in sensing", sizeGU: [5, 5], clickGU: [4, 4], sensingGU: [5, 5]},
                {tag: "5x5GU size, 1GU margins, click in visual", sizeGU: [5, 5], marginsGU: [1, 1, 1, 1], clickGU: [0, 0], sensingGU: [7, 7]},
                {tag: "5x5GU size, 1GU margins, click in sensing", sizeGU: [5, 5], marginsGU: [1, 1, 1, 1], clickGU: [4, 4], sensingGU: [7, 7]},
                {tag: "5x5GU size, 3GU margins horizontal, click in sensing", sizeGU: [5, 5], marginsGU: [3, 0, 3, 0], clickGU: [4, 4], sensingGU: [11, 5]},
                {tag: "5x5GU size, 3GU margins vertical, click in sensing", sizeGU: [5, 5], marginsGU: [0, 3, 0, 3], clickGU: [4, 4], sensingGU: [5, 11]},
                {tag: "5x5GU size, 3GU margins around, click in sensing", sizeGU: [5, 5], marginsGU: [3, 3, 3, 3], clickGU: [4, 4], sensingGU: [11, 11]},

                {tag: "zero size, no margins, click out of sensing area", sizeGU: [0, 0], clickGU: [5, 5], sensingGU: [4, 4], fail: true},
                {tag: "2x2GU size, no margins, click out of sensing area", sizeGU: [2, 2], clickGU: [5, 5], sensingGU: [4, 4], fail: true},
                {tag: "4x4GU size, no margins, click out of sensing area", sizeGU: [4, 4], clickGU: [5, 5], sensingGU: [4, 4], fail: true},
                {tag: "2x2GU size, 1GU margins around, click out of sensing area", sizeGU: [2, 2], marginsGU: [1, 1, 1, 1], clickGU: [5, 5], sensingGU: [4, 4], fail: true},
                {tag: "4x4GU size, 1GU margins around, click out of sensing area", sizeGU: [4, 4], marginsGU: [1, 1, 1, 1], clickGU: [6.1, 6.1], sensingGU: [6, 6], fail: true},

                // test margins.all
                {tag: "zero size, 5GU margins.all, click in sensing area", sizeGU: [0, 0], marginsAll: units.gu(5), clickGU: [5, 5], sensingGU: [10, 10]},
                {tag: "2x2 size, 2GU margins.all, click in sensing area", sizeGU: [2, 2], marginsAll: units.gu(2), clickGU: [6, 6], sensingGU: [6, 6]},
                {tag: "zero size, 5GU margins.all, click out of sensing area", sizeGU: [0, 0], marginsAll: units.gu(5), clickGU: [10.1, 10.1], sensingGU: [10, 10], fail: true},
                {tag: "2x2 size, 2GU margins.all, click out of sensing area", sizeGU: [2, 2], marginsAll: units.gu(2), clickGU: [6.1, 6.1], sensingGU: [6, 6], fail: true},

                // test negative margins
                {tag: "zero size, -1GU margins.all, click in sensing area", sizeGU: [0, 0], marginsAll: -units.gu(1), clickGU: [4, 4], sensingGU: [4, 4]},
                {tag: "2x2 size, -1GU margins.all, click in sensing area", sizeGU: [2, 2], marginsAll: -units.gu(1), clickGU: [4, 4], sensingGU: [4, 4]},
                {tag: "zero size, -1GU margins horizontal, click in sensing area", sizeGU: [0, 0], marginsGU: [-1, 0, -1, 0], clickGU: [4, 4], sensingGU: [4, 4]},
                {tag: "zero size, -1GU margins vertical, click in sensing area", sizeGU: [0, 0], marginsGU: [0, -1, 0, -1], clickGU: [4, 4], sensingGU: [4, 4]},
                {tag: "2x2 size, -1GU margins horizontal, click in sensing area", sizeGU: [2, 2], marginsGU: [-1, 0, -1, 0], clickGU: [4, 4], sensingGU: [4, 4]},
                {tag: "2x2 size, -1GU margins vertical, click in sensing area", sizeGU: [2, 2], marginsGU: [0, -1, 0, -1], clickGU: [4, 4], sensingGU: [4, 4]},
                {tag: "4x4 size, -1GU margins.all, click in sensing area", sizeGU: [4, 4], marginsAll: -units.gu(1), clickGU: [4, 4], sensingGU: [4, 4]},
                {tag: "4x4 size, -1GU margins horizontal, click in sensing area", sizeGU: [4, 4], marginsGU: [-1, 0, -1, 0], clickGU: [4, 4], sensingGU: [4, 4]},
                {tag: "4x4 size, -1GU margins vertical, click in sensing area", sizeGU: [4, 4], marginsGU: [0, -1, 0, -1], clickGU: [4, 4], sensingGU: [4, 4]},

                // bigger size than minimum, decrease sensing area
                {tag: "5x5 size, -1GU margins.all, click in sensing area", sizeGU: [5, 5], marginsAll: -units.gu(1), clickGU: [4, 4], sensingGU: [4, 4]},
                {tag: "5x5 size, -1GU margins horizontal, click in sensing area", sizeGU: [5, 5], marginsGU: [-1, 0, -1, 0], clickGU: [4, 5], sensingGU: [4, 5]},
                {tag: "5x5 size, -1GU margins vertical, click in sensing area", sizeGU: [5, 5], marginsGU: [0, -1, 0, -1], clickGU: [5, 4], sensingGU: [5, 4]},
            ];
        }
        function test_sensing_area(data) {
            signalSpy.target = buttonWithSensing;
            buttonWithSensing.objectName = data.tag;
            buttonWithSensing.width = units.gu(data.sizeGU[0]);
            buttonWithSensing.height = units.gu(data.sizeGU[1]);
            if (data.marginsGU) {
                buttonWithSensing.sensingMargins.left = units.gu(data.marginsGU[0]);
                buttonWithSensing.sensingMargins.top = units.gu(data.marginsGU[1]);
                buttonWithSensing.sensingMargins.right = units.gu(data.marginsGU[2]);
                buttonWithSensing.sensingMargins.bottom = units.gu(data.marginsGU[3]);
            } else if (data.marginsAll) {
                buttonWithSensing.sensingMargins.all = data.marginsAll;
            }

            if (data.sensingGU) {
                compare(buttonWithSensing.__mouseArea.width, units.gu(data.sensingGU[0]), "unexpected horizontal sensing size");
                compare(buttonWithSensing.__mouseArea.height, units.gu(data.sensingGU[1]), "unexpected vertical sensing size");
            }
            if (data.fail) {
                expectFailContinue(data.tag, "no signal");
            }
            mouseClick(buttonWithSensing.__mouseArea, units.gu(data.clickGU[0]), units.gu(data.clickGU[1]));
            signalSpy.wait(500);
        }

        function test_predeclared_sensing_area() {
            var point = centerOf(increasedSensing.parent);
            // move the point to the edge of the sensing area
            point.x -= increasedSensing.sensingMargins.left;
            point.y -= increasedSensing.sensingMargins.top;
            // click
            signalSpy.target = increasedSensing;
            mouseClick(increasedSensing.parent, point.x, point.y);
            signalSpy.wait(500);
        }
    }
}
