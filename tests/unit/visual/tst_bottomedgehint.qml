/*
 * Copyright 2015 Canonical Ltd.
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
    id: mainView
    width: units.gu(40)
    height: units.gu(71)

    ListView {
        id: listView
        anchors.fill: parent
        model: 100
        delegate: Label {
            height: units.gu(5)
            text: "Item #" + index
        }
    }

    BottomEdgeHint {
        id: bottomEdgeHint
        property int triggerCount: 0
        function trigger(v)
        {
            triggerCount++;
        }
    }
    BottomEdgeHint {
        id: floatingHint
        anchors.bottom: parent.top
    }
    Item {
        id: floatingItem
    }

    UbuntuTestCase {
        name: "BottomEdgeHint"
        when: windowShown

        // FIXME: the criteria must be adjusted when QSystemInfo will report
        // attached mouses, till then we stick to the touch presence
        property bool hasMouseAttached: QuickUtils.mouseAttached

        SignalSpy {
            id: clickSpy
            target: bottomEdgeHint
            signalName: "onClicked"
        }

        function initTestCase() {
            // register test touch device
            TestExtras.registerTouchDevice();

            // FIXME: this test case must be adjusted after we get the QInputDeviceInfo
            // available to detect attached mouse
            // the test must be executed before we register touch device
            if (!hasMouseAttached) {
                // we don't have mouse attached, so we should be able to lock/unlock
                compare(bottomEdgeHint.status, BottomEdgeHint.Inactive, "Wrong initial status when no mouse attached");
                bottomEdgeHint.status = BottomEdgeHint.Locked;
                compare(bottomEdgeHint.status, BottomEdgeHint.Locked, "Could not toggle status");
            } else {
                // we have the mouse attached, should not be able to unlock it
                compare(bottomEdgeHint.status, BottomEdgeHint.Locked, "Wrong initial status when mouse attached");
                bottomEdgeHint.status = BottomEdgeHint.Inactive;
                compare(bottomEdgeHint.status, BottomEdgeHint.Locked, "The bottom edge must not be unlockable as long as mouse is attached!");
            }
            QuickUtils.mouseAttached = !QuickUtils.mouseAttached;

            // and then turn locked off if possible
            bottomEdgeHint.status = BottomEdgeHint.Inactive;
            if (!hasMouseAttached) {
                compare(bottomEdgeHint.status, BottomEdgeHint.Inactive, "Cannot unlock hint!");
            }

            // defaults
            compare(bottomEdgeHint.iconName, "");
            compare(bottomEdgeHint.text, "");
            compare(bottomEdgeHint.width, mainView.width);
            compare(bottomEdgeHint.height, units.gu(4));
            compare(bottomEdgeHint.y, mainView.height - bottomEdgeHint.height);
            compare(bottomEdgeHint.flickable, null, "No flickable");
            compare(clickSpy.count, 0, "The BottomEdgeHint should not have received a click.");
            compare(bottomEdgeHint.deactivateTimeout, 800, "default deactivationTimeout");

            // set the flickable
            bottomEdgeHint.flickable = listView;
        }

        function cleanup() {
            mainView.forceActiveFocus();
            listView.positionViewAtBeginning();
            bottomEdgeHint.visible = true;
            bottomEdgeHint.iconName = "";
            bottomEdgeHint.status = BottomEdgeHint.Inactive;
            clickSpy.clear();
            wait(400);
        }

        function test_hiding() {
            var flickDy = listView.height - units.gu(5);
            flick(listView, centerOf(listView).x, flickDy, centerOf(listView).x, -flickDy, 0, 6);
            if (hasMouseAttached) {
                expectFailContinue("", "No hiding when mouse attached");
            }
            tryCompare(bottomEdgeHint, "status", BottomEdgeHint.Hidden);
        }

        function test_no_hiding_when_locked() {
            var flickDy = listView.height - units.gu(10);
            bottomEdgeHint.status = BottomEdgeHint.Locked;
            flick(listView, centerOf(listView).x, flickDy, centerOf(listView).x, -flickDy, 0, 6);
            expectFailContinue("", "No hiding when Locked");
            tryCompare(bottomEdgeHint, "status", BottomEdgeHint.Hidden, 500);
        }

        function test_clicking_data() {
            return [
                {tag: "when Locked", status: BottomEdgeHint.Locked, statusFail: false, xfail: false},
                {tag: "when Active", status: BottomEdgeHint.Active, statusFail: hasMouseAttached, xfail: false},
                {tag: "when Inactive", status: BottomEdgeHint.Inactive, statusFail: hasMouseAttached, xfail: !hasMouseAttached},
                {tag: "when Hidden", status: BottomEdgeHint.Hidden, statusFail: hasMouseAttached, xfail: !hasMouseAttached},
            ];
        }
        function test_clicking(data) {
            bottomEdgeHint.status = data.status;
            // states cannot be set when mouse attached
            if (!data.statusFail) {
                compare(bottomEdgeHint.status, data.status);
            }
            mouseClick(bottomEdgeHint, centerOf(bottomEdgeHint).x, centerOf(bottomEdgeHint).y);
            if (data.xfail) {
                expectFailContinue(data.tag, "No click is expected");
            }
            clickSpy.wait(500);
        }

        function test_alter_deprecated_state_data() {
            return [
                {tag: "Hidden", status: hasMouseAttached ? BottomEdgeHint.Locked : BottomEdgeHint.Hidden},
                {tag: "Visible", status: hasMouseAttached ? BottomEdgeHint.Locked : BottomEdgeHint.Inactive},
            ];
        }
        function test_alter_deprecated_state(data) {
            ignoreWarning(warningFormat(37, 5, "QML BottomEdgeHint: Overloaded 'state' property deprecated, will be removed from 1.3 release. Use 'status' instead."));
            bottomEdgeHint.state = data.tag;
            compare(bottomEdgeHint.status, data.status, "Wrong component status: " + data.status);
        }

        function test_anchoring() {
            compare(floatingHint.anchors.bottom, mainView.top, "Anhors are broken");
            floatingHint.parent = floatingItem;
            compare(floatingHint.anchors.bottom, floatingItem.top, "Anhors are broken after reparenting");
        }

        function test_no_clicking_data() {
            return [
                {tag: "when hidden", property: "visible"},
                {tag: "when disabled", property: "enabled"},
            ];
        }
        function test_no_clicking(data) {
            bottomEdgeHint.status = BottomEdgeHint.Locked;
            bottomEdgeHint[data.property] = false;
            mouseClick(bottomEdgeHint, centerOf(bottomEdgeHint).x, centerOf(bottomEdgeHint).y);
            expectFailContinue("", "No click " + data.tag);
            clickSpy.wait(400);
        }

        function test_activate_by_key_data() {
            return [
                {tag: "enter and unlocked", key: Qt.Key_Return, status: BottomEdgeHint.Inactive},
                {tag: "return and unlocked", key: Qt.Key_Enter, status: BottomEdgeHint.Inactive},
                {tag: "space and unlocked", key: Qt.Key_Space, status: BottomEdgeHint.Inactive},
                {tag: "enter and locked", key: Qt.Key_Return, status: BottomEdgeHint.Locked},
                {tag: "return and locked", key: Qt.Key_Enter, status: BottomEdgeHint.Locked},
                {tag: "space and locked", key: Qt.Key_Space, status: BottomEdgeHint.Locked},
            ];
        }
        function test_activate_by_key(data) {
            switch (data.status) {
            case BottomEdgeHint.Inactive:
                QuickUtils.mouseAttached = false; break;
            case BottomEdgeHint.Locked:
                QuickUtils.mouseAttached = true; break;
            default:
                fail("Unexpected status %1".arg(data.status));
            }
            bottomEdgeHint.status = data.status;
            bottomEdgeHint.forceActiveFocus();
            verify(bottomEdgeHint.activeFocus, "Hint doesn't have the focus");
            keyPress(data.key);
            clickSpy.wait(400);
            keyRelease(data.key);
        }

        // FIXME: must be executed before the test_hiding as flick with mouse affects
        // the touch drag on ListView for some unknown reason
        function test_0_touch_gesture() {
            bottomEdgeHint.text = "Touch Activated";
            var gestureStartPoint = Qt.point(centerOf(bottomEdgeHint).x, bottomEdgeHint.height - 1);
            TestExtras.touchDrag(0, bottomEdgeHint, gestureStartPoint, Qt.point(0, -units.gu(8)), 6);
            var expectedStatus = hasMouseAttached ? BottomEdgeHint.Locked : BottomEdgeHint.Active;
            tryCompare(bottomEdgeHint, "status", expectedStatus, 400);
            // then wait till we get back to Idle
            expectedStatus = hasMouseAttached ? BottomEdgeHint.Locked : BottomEdgeHint.Inactive;
            tryCompare(bottomEdgeHint, "status", expectedStatus, 1000);
        }

        function test_custom_trigger_on_clicked() {
            bottomEdgeHint.status = BottomEdgeHint.Locked;
            var prevCount = bottomEdgeHint.triggerCount;
            mouseClick(bottomEdgeHint, centerOf(bottomEdgeHint).x, centerOf(bottomEdgeHint).y);
            clickSpy.wait(500);
            compare(bottomEdgeHint.triggerCount, prevCount + 1, "Overloaded trigger not called");
        }

        function test_active_focus_on_press_bug1517777() {
            var testItem = bottomEdgeHint;
            testItem.status = BottomEdgeHint.Active;
            testItem.activeFocusOnPress = true;
            // make sure the test item is not active focus
            floatingHint.forceActiveFocus();
            verify(testItem.status >= BottomEdgeHint.Active);
            compare(testItem.activeFocus, false, "BottomEdgeHint is focus before the test!");

            mouseClick(testItem, centerOf(testItem).x, centerOf(testItem).y);
            compare(testItem.activeFocus, true, "BottomEdgeHint is not focus");
        }
    }
}
