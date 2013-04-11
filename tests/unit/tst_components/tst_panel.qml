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
    }
}
