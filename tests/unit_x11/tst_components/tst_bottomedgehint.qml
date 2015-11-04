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
        property bool hasMouseAttached: !TestExtras.touchPresent

        SignalSpy {
            id: clickSpy
            target: bottomEdgeHint
            signalName: "onClicked"
        }

        function initTestCase() {
            // FIXME: this test case must be adjusted after we get the QSystemInfo
            // available to detect attached mouse
            // the test must be executed before we register touch device
            var prevValue = bottomEdgeHint.locked;
            if (!hasMouseAttached) {
                // we don't have mouse attached, so we should be able to lock/unlock
                bottomEdgeHint.locked = !bottomEdgeHint.locked;
                compare(bottomEdgeHint.locked, !prevValue, "Could not toggle locked");
            } else {
                // we have the mouse attached, should not be able to unlock it
                compare(bottomEdgeHint.locked, true, "The bottom edge is not locked!");
                bottomEdgeHint.locked = false;
                compare(bottomEdgeHint.locked, true, "The bottom edge must not be unlockable as long as mouse is attached!");
            }

            // register test touch device
            TestExtras.registerTouchDevice();
            // and then turn locked off if possible
            bottomEdgeHint.locked = false;

            // defaults
            compare(bottomEdgeHint.iconName, "");
            compare(bottomEdgeHint.text, "");
            compare(bottomEdgeHint.width, mainView.width);
            compare(bottomEdgeHint.height, units.gu(4));
            compare(bottomEdgeHint.y, mainView.height - bottomEdgeHint.height);
            compare(bottomEdgeHint.flickable, null, "No flickable");
            compare(clickSpy.count, 0, "The BottomEdgeHint should not have received a click.");

            // set the flickable
            bottomEdgeHint.flickable = listView;
        }

        function cleanup() {
            listView.positionViewAtBeginning();
            bottomEdgeHint.visible = true;
            bottomEdgeHint.iconName = "";
            if (!hasMouseAttached) {
                bottomEdgeHint.locked = false;
            }
            clickSpy.clear();
        }

        function test_hiding() {
            var flickDy = listView.height - units.gu(5);
            flick(listView, centerOf(listView).x, flickDy, centerOf(listView).x, -flickDy);
            tryCompare(bottomEdgeHint, "opacity", 0.0);
        }

        function test_clicking() {
            bottomEdgeHint.locked = true;
            mouseClick(bottomEdgeHint, centerOf(bottomEdgeHint).x, centerOf(bottomEdgeHint).y);
            clickSpy.wait();
        }

        function test_no_clicking_while_unlocked() {
            if (hasMouseAttached) {
                skip("the test needs mouse not to be attached");
            }
            mouseClick(bottomEdgeHint, centerOf(bottomEdgeHint).x, centerOf(bottomEdgeHint).y);
            expectFail("", "No click if not Locked");
            clickSpy.wait(200);
        }

        function test_deprecated_state() {
            ignoreWarning(warningFormat(37, 5, "QML BottomEdgeHint: 'state' property deprecated, will be removed from 1.3 release. Use 'locked' property to lock the visuals"));
            bottomEdgeHint.state = "Hidden";
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
            bottomEdgeHint.locked = true;
            bottomEdgeHint[data.property] = false;
            mouseClick(bottomEdgeHint, centerOf(bottomEdgeHint).x, centerOf(bottomEdgeHint).y);
            expectFailContinue("", "No click " + data.tag);
            clickSpy.wait(400);
        }

        function test_activate_by_key_data() {
            return [
                {tag: "enter and unlocked", key: Qt.Key_Return, locked: false},
                {tag: "return and unlocked", key: Qt.Key_Enter, locked: false},
                {tag: "enter and locked", key: Qt.Key_Return, locked: true},
                {tag: "return and locked", key: Qt.Key_Enter, locked: true},
            ];
        }
        function test_activate_by_key(data) {
            if (hasMouseAttached && !data.locked) {
                skip(data.tag, "Test requires ability to unlock");
            }
            bottomEdgeHint.locked = data.locked;
            bottomEdgeHint.forceActiveFocus();
            keyPress(data.key);
            if (!bottomEdgeHint.locked) {
                expectFailContinue(data.tag, "should fail");
            }
            clickSpy.wait(400);
            keyRelease(data.key);
        }
    }
}
