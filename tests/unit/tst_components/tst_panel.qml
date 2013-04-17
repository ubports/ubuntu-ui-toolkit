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

        function test_swipe_alignBottom() {
            compare(panel.active, false, "Panel initially not active")

            var x = root.width / 2; var y = root.height - 1;
            var dx = 0;
            var dy = -panel.height / 2;
            mousePress(root, x, y, Qt.LeftButton);
            mouseMove(root, x + dx, x + dy);
            mouseRelease(root, x + dx, y + dy, Qt.LeftButton);
            compare(panel.active, true, "Panel activated by swiping up")
            x = panel.width / 2;
            y = 10;
            mousePress(panel, x, y, Qt.LeftButton);
            mouseMove(panel, x - dx, y - dy);
            mouseRelease(panel, x - dx, y - dy, Qt.LeftButton);
            compare(panel.active, false, "Panel deactivated by swiping down")
        }

        function test_swipe_alignLeft() {
            panel.align = Qt.AlignLeft;
            compare(panel.active, false, "Panel does not get activated by changing alignment to left");
            var x = 1;
            var y = 3 * root.height / 4;
            var dx = panel.width / 2;
            var dy = 0;
            mousePress(root, x, y, Qt.LeftButton);
            mouseMove(root, x + dx, y + dy);
            mouseRelease(root, x + dx, y + dy, Qt.Leftbutton);
            compare(panel.active, true, "Left-aligned panel activated by swiping to the right");
            x = 3 * panel.width / 4;
            y = panel.height / 2;
            mousePress(panel, x, y, Qt.LeftButton);
            mouseMove(root, x - dx, y - dy);
            mouseRelease(panel, x - dx, y - dy, Qt.LeftButton);
            compare(panel.active, false, "Left-aligned panel deactivated by swiping to the left");
        }

        function test_swipe_alignRight() {
            panel.align = Qt.AlignRight;
            compare(panel.active, false, "Panel does not get activated by changing alignment to right");
            var x = root.width - 1;
            var y = 3 * root.height / 4;
            var dx = -panel.width / 2;
            var dy = 0;
            mousePress(root, x, y, Qt.LeftButton);
            mouseMove(root, x + dx, y + dy);
            mouseRelease(root, x + dx, y + dy, Qt.Leftbutton);
            compare(panel.active, true, "Right-aligned panel activated by swiping to the left");
            x = 1;
            y = panel.height / 2;
            mousePress(panel, x, y, Qt.LeftButton);
            mouseMove(panel, -dx, -dy);
            mouseRelease(panel, x - dx, y - dy, Qt.LeftButton);
            compare(panel.active, false, "Right-aligned panel deactivating by swiping to the right");
        }

        function test_swipe_alignTop() {
            panel.anchors.top = root.top;
            panel.anchors.bottom = undefined;
            panel.align = Qt.AlignTop;
            compare(panel.active, false, "Panel does not get activated by changing alignment to top");
            var x = root.width / 2;
            var y = 1;
            var dx = 0;
            var dy = panel.height / 2;
            mousePress(root, x, y, Qt.LeftButton);
            mouseMove(root, x + dx, y + dy);
            mouseRelease(root, x + dx, y + dy, Qt.LeftButton);
            compare(panel.active, true, "Top-aligned panel activated by swiping down");
            x = panel.width / 2;
            y = panel.height - 1;
            mousePress(panel, x, y, Qt.LeftButton);
            mouseMove(panel, x - dx, y - dy);
            mouseRelease(panel, x - dx, y - dy, Qt.LeftButton);
            compare(panel.active, false, "Top-aligned panel deactivated by swiping up");

            panel.anchors.bottom = root.bottom;
            panel.anchors.top = undefined;
            panel.align = Qt.AlignBottom;
            compare(panel.active, false, "Panel does not get activated by changing alignment back to bottom")
        }

        function test_clickToDeactivate() {
            panel.active = true;
            compare(panel.active && panel.align === Qt.AlignBottom, true, "Panel is active and bottom-aligned");
            mouseClick(root, root.width / 2, 5, Qt.LeftButton);
            compare(panel.active, false, "Panel is deactivated by clicking in the view outside of the panel");
        }
    }
}
