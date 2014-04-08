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
import Ubuntu.Test 0.1
import Ubuntu.Components 0.1

Item {
    id: root
    width: units.gu(50); height: units.gu(100)

    property bool hasOSK: QuickUtils.inputMethodProvider !== ""

    Flickable {
        id: flickable
        anchors.fill: parent
        contentHeight: column.childrenRect.height
        clip: true

        Column {
            id: column
            Text {
                text: "This is a simple label on top of the Flickable"
            }

            TextArea {
                id: inFlickable
                width: flickable.width
                autoSize: true
                maximumLineCount: 0
                text: "1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1"
            }
        }
    }

    SignalSpy {
        id: moveSpy
        target: flickable
        signalName: "onMovementEnded"
    }

    UbuntuTestCase {
        name: "TextAreaInFlickableAPI"
        when: windowShown

        function cleanup() {
            flickable.contentY = 0;
            moveSpy.clear();
            inFlickable.focus = false;
            // empty event buffer caused by the flick() events
            wait(200);
        }

        function test_DoNotStealFlickEvents() {
            mouseClick(inFlickable, 10, 10);
            flick(inFlickable, Qt.point(50, 50), Qt.point(50, 0), 10);
            tryCompare(moveSpy, "count", 1, 200);
        }

        function test_flicker_moves_when_inactive() {
            flick(flickable, Qt.point(50, 100), Qt.point(50, 0), 10)
            tryCompare(moveSpy, "count", 1, 200);
        }

        function test_select_state_locks_outer_flickable() {
            mouseClick(inFlickable, 10, 10);
            // select text
            flick(inFlickable, Qt.point(50, 50), Qt.point(50, 0), 10, 400);
            compare(moveSpy.count, 0, "The Flickable has moved while the TextArea was in selection mode");
            verify(inFlickable.selectedText !== "");
        }

        function test_z_scrolling_input_with_selected_text() {
            mouseClick(inFlickable, 10, 10);
            // select text
            flick(inFlickable, Qt.point(50, 50), Qt.point(50, 0), 10, 400);
            compare(moveSpy.count, 0, "The Flickable has moved while the TextArea was in selection mode");
            verify(inFlickable.selectedText !== "");

            // scroll
            moveSpy.clear();
            flick(inFlickable, Qt.point(50, 100), Qt.point(50, 0), 10);
            // wait till the move ends
            moveSpy.wait();
            verify(inFlickable.selectedText !== "");
        }
    }
}
