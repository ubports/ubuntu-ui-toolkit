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
import Ubuntu.Components 1.1

Item {
    id: root
    width: units.gu(50); height: units.gu(100)

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
                text: "1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1"
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

        function init() {
            waitForRendering(flickable, 1000);
        }

        function cleanup() {
            flickable.contentY = 0;
            moveSpy.clear();
            inFlickable.focus = false;
            inFlickable.cursorPosition = 0;
            // empty event buffer caused by the flick() events
            wait(400);
        }

        function test_select_state_locks_outer_flickable() {
            var handler = findChild(inFlickable, "input_handler");
            inFlickable.focus = true;
            // select text
            flick(inFlickable, 50, 50, -50, -50, handler.selectionModeTimeout+ 50);
            compare(moveSpy.count, 0, "The Flickable has moved while the TextArea was in selection mode");
            verify(inFlickable.selectedText !== "", "No text selected");
        }

        function test_scrolling_input_with_selected_text() {
            var handler = findChild(inFlickable, "input_handler");
            inFlickable.focus = true;
            // select text
            inFlickable.select(0, 10);
            verify(inFlickable.selectedText !== "", "No text selected");

            // not scrollable when focused, focus locks scrolling with mouse
            moveSpy.clear();
            mouseWheel(inFlickable, 50, 20, 0, -100);
            // wait till the move ends
            expectFailContinue("", "cannot scroll when scroller is locked");
            moveSpy.wait(500);
            verify(inFlickable.selectedText !== "", "There is still text selected");
        }
    }
}
