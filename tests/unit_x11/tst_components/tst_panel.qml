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
import Ubuntu.Components 1.1
import Ubuntu.Test 1.0

Item {
    width: 200
    height: 200
    id: root

    Panel {
        id: panel
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        height: parent.height / 2
        Rectangle {
            color: "pink"
            anchors.fill: parent
        }
    }

    UbuntuTestCase {
        id: testCase
        name: "PanelAPI"
        when: windowShown

        function initTestCase() {
            compare(panel.align, Qt.AlignBottom, "Panel initially aligned at bottom");
            compare(panel.opened, false, "Panel initially not opened");
            compare(panel.locked, false, "Panel initially not locked");
            compare(panel.hintSize, units.gu(2), "Default hintSize is 2 grid units");
            compare(panel.triggerSize, units.gu(2), "Default triggerSize is 2 grid units");
        }

        function test_align() {
            panel.align = Qt.AlignTop;
            compare(panel.align, Qt.AlignTop, "Can set align to top");
            panel.align = Qt.AlignLeft;
            compare(panel.align, Qt.AlignLeft, "Can set align to left");
            panel.align = Qt.AlignRight;
            compare(panel.align, Qt.AlignRight, "Can set align to right");
            panel.align = Qt.AlignBottom;
            compare(panel.align, Qt.AlignBottom, "Can set align to bottom");
        }

        function test_height() {
            var bar = findChild(panel, "bar_item");
            // panel is not opened
            panel.height = 123;
            compare(bar.y, 123, "Panel is properly closed after changing height");
            panel.open(); wait(500);
            compare(bar.y, 0, "Panel opens properly after changing height");
            panel.height = 78;
            compare(bar.y, 0, "Panel stays propery opened after changing height");
            panel.close(); wait(500);
            compare(bar.y, 78, "Panel closes properly after changing height");
            panel.height = root.height / 2;
            compare(bar.y, root.height/2, "Panel stays closed properly after changing height");
        }

        function test_opened() {
            panel.open();
            compare(panel.opened, true, "Can set opened");
            panel.close();
            compare(panel.opened, false, "Can unset opened");
        }

        function test_locked() {
            panel.locked = true;
            compare(panel.locked, true, "Can set locked");
            panel.locked = false;
            compare(panel.locked, false, "Can unset locked");
        }

        function test_hintSize() {
            panel.hintSize = units.gu(1);
            compare(panel.hintSize, units.gu(1), "Can set hintSize");
            panel.hintSize = units.gu(2);
        }

        function test_triggerSize() {
            panel.triggerSize = units.gu(5);
            compare(panel.triggerSize, units.gu(5), "Can set triggerSize");
            panel.triggerSize = units.gu(2);
        }

        function test_swipeBottomPanel() {
            // swipe bottom-aligned panel in and out
            swipeTests.slowMouseMove = false;
            swipeTests.swipeUpDown();
            swipeTests.slowMouseMove = true;
            swipeTests.swipeUpDown();
            swipeTests.slowMouseMove = false;
        }

        function test_swipeLeftPanel() {
            // swipe a left-aligned panel in and out
            panel.align = Qt.AlignLeft;
            swipeTests.slowMouseMove = false;
            swipeTests.swipeRightLeft();
            swipeTests.slowMouseMove = true;
            swipeTests.swipeRightLeft();
            swipeTests.slowMouseMove = false;
            panel.align = Qt.AlignBottom;
        }

        function test_swipeRightPanel() {
            // swipe right-aligned panel in and out
            panel.align = Qt.AlignRight;
            swipeTests.slowMouseMove = false;
            swipeTests.swipeLeftRight();
            swipeTests.slowMouseMove = true;
            swipeTests.swipeLeftRight();
            swipeTests.slowMouseMove = false;
            panel.align = Qt.AlignBottom;
        }

        function test_swipeLeadingPanel() {
            // swipe leading-aligned panel in and out
            panel.align = Qt.AlignLeading;
            swipeTests.swipeRightLeft();
            panel.LayoutMirroring.enabled = true;
            panel.LayoutMirroring.childrenInherit = true;
            swipeTests.slowMouseMove = false;
            swipeTests.swipeLeftRight();
            swipeTests.slowMouseMove = true;
            swipeTests.swipeLeftRight();
            swipeTests.slowMouseMove = false;
            panel.LayoutMirroring.enabled = false;
            panel.align = Qt.AlignBottom;
        }

        function test_swipeTrailingPanel() {
            // swipe trailing-aligned panel in and out
            panel.align = Qt.AlignTrailing;
            swipeTests.swipeLeftRight();
            panel.LayoutMirroring.enabled = true;
            panel.LayoutMirroring.childrenInherit = true;
            swipeTests.slowMouseMove = false;
            swipeTests.swipeRightLeft();
            swipeTests.slowMouseMove = true;
            swipeTests.swipeRightLeft();
            swipeTests.slowMouseMove = false;
            panel.LayoutMirroring.enabled = false;
            panel.align = Qt.AlignBottom;
        }

        function test_swipeTopPanel() {
            // swipe top-aligned panel in and out
            panel.anchors.top = root.top;
            panel.anchors.bottom = undefined;
            panel.align = Qt.AlignTop;

            swipeTests.slowMouseMove = false;
            swipeTests.swipeDownUp();
            swipeTests.slowMouseMove = true;
            swipeTests.swipeDownUp();
            swipeTests.slowMouseMove = false;

            // revert to original state
            panel.anchors.bottom = root.bottom;
            panel.anchors.top = undefined;
            panel.align = Qt.AlignBottom;
        }

        function test_clickToDeactivate() {
            // deprecated test. This functionality is only there for backwards compatibility
            panel.__closeOnContentsClicks = true;
            panel.open();
            compare(panel.opened && panel.align === Qt.AlignBottom, true, "Panel is opened and bottom-aligned");
            mouseClick(root, root.width / 2, 5, Qt.LeftButton);
            compare(panel.opened, false, "Panel is deactivated by clicking in the view outside of the panel");
            // reset property to default value
            panel.__closeOnContentsClicks = false;
        }

        function test_hideTimeout_bug1249031() {
            compare(panel.hideTimeout, -1, "Panel hide timeout is initially negative (no timeout)");
            panel.hideTimeout = 2000;
            panel.open();
            compare(panel.opened, true, "Panel can be made opened");
            wait(panel.hideTimeout + 500); // add 500 ms margin
            compare(panel.opened, false, "Panel automatically closes after timeout");

            // now, wait in total more than hideTimeout, but less than 2*hideTimeout,
            //  and have user interaction half-way to verify that the interaction
            //  resets the timer and the panel is not closed.
            panel.open();
            wait(0.6*panel.hideTimeout);
            mouseClick(panel, panel.width/2, panel.height/2);
            wait(0.6*panel.hideTimeout);
            compare(panel.opened, true, "Interacting with panel contents resets the hide timer");
            // verify that the timer is still running by waiting a bit longer:
            wait(0.6*panel.hideTimeout);
            compare(panel.opened, false, "Interacting with the panel contents does not stop the timer")
            panel.hideTimeout = -1;
        }

        QtObject {
            id: swipeTests

            // waiting time in ms after move, to simulate slow drag vs fast swipe
            property int moveDelay: slowMouseMove ? 400 : 0
            property bool slowMouseMove: false

            function swipeUpDown() {
                testCase.compare(panel.opened, false, "Panel initially not opened")
                var x = root.width / 2; var y = root.height - 1;
                var dx = 0;
                var dy = -panel.height / 2;
                testCase.mousePress(root, x, y, Qt.LeftButton);
                testCase.mouseMove(root, x + dx, y + dy, moveDelay);
                testCase.mouseRelease(root, x + dx, y + dy, Qt.LeftButton);
                testCase.compare(panel.opened, true, "Panel activated by swiping up (delay: "+moveDelay+")")
                x = panel.width / 2;
                y = 10;
                testCase.mousePress(panel, x, y, Qt.LeftButton);
                testCase.mouseMove(panel, x - dx, y - dy, moveDelay);
                testCase.mouseRelease(panel, x - dx, y - dy, Qt.LeftButton);
                testCase.compare(panel.opened, false, "Panel deactivated by swiping down (delay: "+moveDelay+")")
            }

            function swipeRightLeft() {
                testCase.compare(panel.opened, false, "Panel initially not opened")
                var x = 1;
                var y = 3 * root.height / 4;
                var dx = panel.width / 2;
                var dy = 0;
                testCase.mousePress(root, x, y, Qt.LeftButton);
                testCase.mouseMove(root, x + dx, y + dy, moveDelay);
                testCase.mouseRelease(root, x + dx, y + dy, Qt.Leftbutton);
                testCase.compare(panel.opened, true, "Left-aligned panel activated by swiping to the right (delay: "+moveDelay+")");
                x = 3 * panel.width / 4;
                y = panel.height / 2;
                testCase.mousePress(panel, x, y, Qt.LeftButton);
                testCase.mouseMove(root, x - dx, y - dy, moveDelay);
                testCase.mouseRelease(panel, x - dx, y - dy, Qt.LeftButton);
                testCase.compare(panel.opened, false, "Left-aligned panel deactivated by swiping to the left (delay: "+moveDelay+"");
            }

            function swipeLeftRight() {
                testCase.compare(panel.opened, false, "Panel initially not opened")
                var x = root.width - 1;
                var y = 3 * root.height / 4;
                var dx = -panel.width / 2;
                var dy = 0;
                testCase.mousePress(root, x, y, Qt.LeftButton);
                testCase.mouseMove(root, x + dx, y + dy, moveDelay);
                testCase.mouseRelease(root, x + dx, y + dy, Qt.Leftbutton);
                testCase.compare(panel.opened, true, "Right-aligned panel activated by swiping to the left (delay: "+moveDelay+"");
                x = 1;
                y = panel.height / 2;
                testCase.mousePress(panel, x, y, Qt.LeftButton);
                testCase.mouseMove(panel, -dx, -dy, moveDelay);
                testCase.mouseRelease(panel, x - dx, y - dy, Qt.LeftButton);
                testCase.compare(panel.opened, false, "Right-aligned panel deactivating by swiping to the right (delay: "+moveDelay+"");
            }

            function swipeDownUp() {
                testCase.compare(panel.opened, false, "Panel initially not opened")
                var x = root.width / 2;
                var y = 1;
                var dx = 0;
                var dy = panel.height / 2;
                testCase.mousePress(root, x, y, Qt.LeftButton);
                testCase.mouseMove(root, x + dx, y + dy, moveDelay);
                testCase.mouseRelease(root, x + dx, y + dy, Qt.LeftButton);
                testCase.compare(panel.opened, true, "Top-aligned panel activated by swiping down (delay: "+moveDelay+"");
                x = panel.width / 2;
                y = panel.height - 1;
                testCase.mousePress(panel, x, y, Qt.LeftButton);
                testCase.mouseMove(panel, x - dx, y - dy, moveDelay);
                testCase.mouseRelease(panel, x - dx, y - dy, Qt.LeftButton);
                testCase.compare(panel.opened, false, "Top-aligned panel deactivated by swiping up (delay: "+moveDelay+"");
            }
        }
    }
}
