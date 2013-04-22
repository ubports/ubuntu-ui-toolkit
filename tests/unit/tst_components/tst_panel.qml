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
    }

    TestCase {
        id: testCase
        name: "PanelAPI"
        when: windowShown

        function initTestCase() {
            compare(panel.align, Qt.AlignBottom, "Panel initially aligned at bottom");
            compare(panel.active, false, "Panel initially not active");
            compare(panel.lock, false, "Panel initially not locked");
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

        function test_active() {
            panel.active = true;
            compare(panel.active, true, "Can set active");
            panel.active = false;
            compare(panel.active, false, "Can unset active");
        }

        function test_lock() {
            panel.lock = true;
            compare(panel.lock, true, "Can set lock");
            panel.lock = false;
            compare(panel.lock, false, "Can unset lock");
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

        // FIXME: Add tests for slow swipes, see
        // https://code.launchpad.net/~tpeeters/ubuntu-ui-toolkit/panel/+merge/158399/comments/351927
        function test_swipeBottomPanel() {
            // swipe bottom-aligned panel in and out
            swipeTests.swipeUpDown();
        }

        function test_swipeLeftPanel() {
            // swipe a left-aligned panel in and out
            panel.align = Qt.AlignLeft;
            swipeTests.swipeRightLeft();
            panel.align = Qt.AlignBottom;
        }

        function test_swipeRightPanel() {
            // swipe right-aligned panel in and out
            panel.align = Qt.AlignRight;
            swipeTests.swipeLeftRight();
            panel.align = Qt.AlignBottom;
        }

        function test_swipeLeadingPanel() {
            // swipe leading-aligned panel in and out
            panel.align = Qt.AlignLeading;
            swipeTests.swipeRightLeft();
            panel.LayoutMirroring.enabled = true;
            panel.LayoutMirroring.childrenInherit = true;
            swipeTests.swipeLeftRight();
            panel.LayoutMirroring.enabled = false;
            panel.align = Qt.AlignBottom;
        }

        function test_swipeTrailingPanel() {
            // swipe trailing-aligned panel in and out
            panel.align = Qt.AlignTrailing;
            swipeTests.swipeLeftRight();
            panel.LayoutMirroring.enabled = true;
            panel.LayoutMirroring.childrenInherit = true;
            swipeTests.swipeRightLeft();
            panel.LayoutMirroring.enabled = false;
            panel.align = Qt.AlignBottom;
        }

        function test_swipeTopPanel() {
            // swipe top-aligned panel in and out
            panel.anchors.top = root.top;
            panel.anchors.bottom = undefined;
            panel.align = Qt.AlignTop;
            swipeTests.swipeDownUp();

            // revert to original state
            panel.anchors.bottom = root.bottom;
            panel.anchors.top = undefined;
            panel.align = Qt.AlignBottom;
        }

        QtObject {
            id: swipeTests

            function swipeUpDown() {
                testCase.compare(panel.active, false, "Panel initially not active")
                var x = root.width / 2; var y = root.height - 1;
                var dx = 0;
                var dy = -panel.height / 2;
                testCase.mousePress(root, x, y, Qt.LeftButton);
                testCase.mouseMove(root, x + dx, x + dy);
                testCase.mouseRelease(root, x + dx, y + dy, Qt.LeftButton);
                testCase.compare(panel.active, true, "Panel activated by swiping up")
                x = panel.width / 2;
                y = 10;
                testCase.mousePress(panel, x, y, Qt.LeftButton);
                testCase.mouseMove(panel, x - dx, y - dy);
                testCase.mouseRelease(panel, x - dx, y - dy, Qt.LeftButton);
                testCase.compare(panel.active, false, "Panel deactivated by swiping down")
            }

            function swipeRightLeft() {
                testCase.compare(panel.active, false, "Panel initially not active")
                var x = 1;
                var y = 3 * root.height / 4;
                var dx = panel.width / 2;
                var dy = 0;
                testCase.mousePress(root, x, y, Qt.LeftButton);
                testCase.mouseMove(root, x + dx, y + dy);
                testCase.mouseRelease(root, x + dx, y + dy, Qt.Leftbutton);
                testCase.compare(panel.active, true, "Left-aligned panel activated by swiping to the right");
                x = 3 * panel.width / 4;
                y = panel.height / 2;
                testCase.mousePress(panel, x, y, Qt.LeftButton);
                testCase.mouseMove(root, x - dx, y - dy);
                testCase.mouseRelease(panel, x - dx, y - dy, Qt.LeftButton);
                testCase.compare(panel.active, false, "Left-aligned panel deactivated by swiping to the left");
            }

            function swipeLeftRight() {
                testCase.compare(panel.active, false, "Panel initially not active")
                var x = root.width - 1;
                var y = 3 * root.height / 4;
                var dx = -panel.width / 2;
                var dy = 0;
                testCase.mousePress(root, x, y, Qt.LeftButton);
                testCase.mouseMove(root, x + dx, y + dy);
                testCase.mouseRelease(root, x + dx, y + dy, Qt.Leftbutton);
                testCase.compare(panel.active, true, "Right-aligned panel activated by swiping to the left");
                x = 1;
                y = panel.height / 2;
                testCase.mousePress(panel, x, y, Qt.LeftButton);
                testCase.mouseMove(panel, -dx, -dy);
                testCase.mouseRelease(panel, x - dx, y - dy, Qt.LeftButton);
                testCase.compare(panel.active, false, "Right-aligned panel deactivating by swiping to the right");
            }

            function swipeDownUp() {
                testCase.compare(panel.active, false, "Panel initially not active")
                var x = root.width / 2;
                var y = 1;
                var dx = 0;
                var dy = panel.height / 2;
                testCase.mousePress(root, x, y, Qt.LeftButton);
                testCase.mouseMove(root, x + dx, y + dy);
                testCase.mouseRelease(root, x + dx, y + dy, Qt.LeftButton);
                testCase.compare(panel.active, true, "Top-aligned panel activated by swiping down");
                x = panel.width / 2;
                y = panel.height - 1;
                testCase.mousePress(panel, x, y, Qt.LeftButton);
                testCase.mouseMove(panel, x - dx, y - dy);
                testCase.mouseRelease(panel, x - dx, y - dy, Qt.LeftButton);
                testCase.compare(panel.active, false, "Top-aligned panel deactivated by swiping up");
            }

            function test_clickToDeactivate() {
                panel.active = true;
                compare(panel.active && panel.align === Qt.AlignBottom, true, "Panel is active and bottom-aligned");
                mouseClick(root, root.width / 2, 5, Qt.LeftButton);
                compare(panel.active, false, "Panel is deactivated by clicking in the view outside of the panel");
            }
        }
    }
}
