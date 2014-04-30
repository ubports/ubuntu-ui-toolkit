/*
 * Copyright 2014 Canonical Ltd.
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
    width: units.gu(40)
    height: units.gu(71)

    Column {
        spacing: units.gu(1)
        TextField {
            text: "This is a single line text input called TextField."
            id: textField
        }
        TextArea {
            text: "This is a multiline text input component called TextArea. It supports fix size as well as auto-expanding behavior."
            id: textArea
        }
    }

    SignalSpy {
        id: cursorPositionSpy
        signalName: "onCursorPositionChanged"
    }
    SignalSpy {
        id: selectionStartSpy
        signalName: "onSelectionStartChanged"
    }
    SignalSpy {
        id: selectionEndSpy
        signalName: "onSelectionEndChanged"
    }
    SignalSpy {
        id: selectedTextSpy
        signalName: "onSelectedTextChanged"
    }

    UbuntuTestCase {
        name: "TextInputCaretTest"
        when: windowShown

        function init() {
            waitForRendering(textField, 1000);
            waitForRendering(textArea, 1000);
        }

        function cleanup() {
            textField.focus = false;
            textArea.focus = false;
            cursorPositionSpy.clear();
            selectionStartSpy.clear();
            selectionEndSpy.clear();
            selectedTextSpy.clear();
        }

        function test_textfield_grab_caret_data() {
            return [
                {"input": textField},
                {"input": textArea},
            ];
        }

        function test_textfield_grab_caret(data) {
            data.input.focus = true;

            var draggedItem = findChild(data.input, "cursorPosition_draggeditem");
            var drag_activator = findChild(data.input, "cursorPosition_activator");
            var dragger = findChild(data.input, "cursorPosition_dragger");

            // drag the cursor, observe cursorPositionChanged
            cursorPositionSpy.target = data.input;
            var x = drag_activator.width / 2;
            var y = drag_activator.height / 2;
            mousePress(drag_activator, x, y);
            compare(draggedItem.state, "dragging", "the caret hasn't been activated");
            // drag the mouse
            mouseMoveSlowly(dragger, x, y, 100, 0, 10, 0);
            mouseRelease(dragger, x + 100, y);
            waitForRendering(data.input, 1000);
            compare(draggedItem.state, "", "the caret hasn't been deactivated");
            cursorPositionSpy.wait();
        }

        function test_textfield_grab_selection_cursors_data() {
            return [
                {"input": textField, "cursorSpy": selectionStartSpy, "dragPrefix": "selectionStart", startDx: 0, startDy: 0, dragDx: 100, dragDy: 0},
                {"input": textArea, "cursorSpy": selectionStartSpy, "dragPrefix": "selectionStart", startDx: 0, startDy: 0, dragDx: 100, dragDy: 0},
                {"input": textField, "cursorSpy": selectionEndSpy, "dragPrefix": "selectionEnd", startDx: 0, startDy: 0, dragDx: 100, dragDy: 100},
                {"input": textArea, "cursorSpy": selectionEndSpy, "dragPrefix": "selectionEnd", startDx: 0, startDy: 0, dragDx: 100, dragDy: 100},
            ];
        }
        function test_textfield_grab_selection_cursors(data) {
            data.input.focus = true;

            // select text in front
            data.input.select(0, 10);
            var draggedItem = findChild(data.input, data.dragPrefix + "_draggeditem");
            var drag_activator = findChild(data.input, data.dragPrefix + "_activator");
            var dragger = findChild(data.input, data.dragPrefix + "_dragger");
            data.cursorSpy.target = data.input;

            // grab the selection and drag it
            var x = drag_activator.width / 2 + data.startDx;
            var y = drag_activator.height / 2 + data.startDy;
            selectedTextSpy.target = data.input;
            mousePress(drag_activator, x, y);
            compare(draggedItem.state, "dragging", "the caret hasn't been activated");
            // drag the mouse
            mouseMoveSlowly(dragger, x, y, data.dragDx, data.dragDy, 10, 100);
            mouseRelease(dragger, x + data.dragDx, y + data.dragDy);
            waitForRendering(data.input, 1000);
            compare(draggedItem.state, "", "the caret hasn't been deactivated");
            data.cursorSpy.wait();
            selectedTextSpy.wait();
        }
    }
}
