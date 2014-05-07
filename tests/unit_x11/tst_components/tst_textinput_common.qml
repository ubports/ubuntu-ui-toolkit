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
    id: testMain
    width: units.gu(40)
    height: units.gu(71)

    Column {
        spacing: units.gu(1)
        TextField {
            id: textField
        }
        TextArea {
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
    SignalSpy {
        id: popupSpy
        signalName: "pressAndHold"
    }
    SignalSpy {
        id: movementXSpy
        signalName: "onContentXChanged"
    }
    SignalSpy {
        id: movementYSpy
        signalName: "contentYChanged"
    }

    UbuntuTestCase {
        name: "TextInputCaretTest"
        when: windowShown

        function init() {
            textField.text = "This is a single line text input called TextField.";
            textArea.text = "This is a multiline text input component called TextArea. It supports fix size as well as auto-expanding behavior.";
            textField.cursorPosition = 0;
            textArea.cursorPosition = 0;
            waitForRendering(textField, 500);
            waitForRendering(textArea, 500);
        }

        function cleanup() {
            textField.focus = false;
            textArea.focus = false;
            cursorPositionSpy.clear();
            selectionStartSpy.clear();
            selectionEndSpy.clear();
            selectedTextSpy.clear();
            popupSpy.clear();
            movementXSpy.clear();
            movementYSpy.clear();
            cursorRectSpy.clear();
        }

        function test_textfield_grab_caret_data() {
            return [
                {input: textField},
                {input: textArea},
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
                {input: textField, cursorSpy: selectionStartSpy, dragPrefix: "selectionStart", startDx: 0, startDy: 0, dragDx: 100, dragDy: 0, movingSpy: null},
                {input: textArea, cursorSpy: selectionStartSpy, dragPrefix: "selectionStart", startDx: 0, startDy: 0, dragDx: 100, dragDy: 0, movingSpy: null},
                {input: textField, cursorSpy: selectionEndSpy, dragPrefix: "selectionEnd", startDx: 0, startDy: 0, dragDx: 200, dragDy: 100, movingSpy: movementXSpy, flicker: "textfield_scroller"},
                {input: textArea, cursorSpy: selectionEndSpy, dragPrefix: "selectionEnd", startDx: 0, startDy: 0, dragDx: 200, dragDy: 100, movingSpy: movementYSpy, flicker: "textarea_scroller"},
            ];
        }
        function test_textfield_grab_selection_cursors(data) {
            if (data.movingSpy) {
                var flicker = findChild(data.input, data.flicker)
                data.movingSpy.target = flicker;
            }
            data.input.focus = true;
            // make sure the text is long enough
            data.input.text += data.input.text;

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
            mouseMoveSlowly(dragger, x, y, data.dragDx, data.dragDy, 15, 100);
            mouseRelease(dragger, x + data.dragDx, y + data.dragDy);
            waitForRendering(data.input, 500);
            compare(draggedItem.state, "", "the caret hasn't been deactivated");
            data.cursorSpy.wait();
            selectedTextSpy.wait();
            if (data.movingSpy) {
                data.movingSpy.wait();
            }
        }

        function test_clear_text_using_popover_data() {
            return [
                {input: textField},
                {input: textArea},
            ];
        }

        function test_clear_text_using_popover(data) {
            var handler = findChild(data.input, "input_handler");
            popupSpy.target = handler;
            data.input.focus = true;

            // invoke popover
            var x = data.input.width / 2;
            var y = data.input.height / 2;
            mouseLongPress(data.input, x, y);
            popupSpy.wait();
            mouseRelease(data.input, x, y);
            var popover = findChild(testMain, "text_input_popover");
            verify(popover, "Cannot retrieve default TextInputPopover");
            waitForRendering(popover);
            // select all
            var selectAll = findChildWithProperty(popover, "text", "Select All");
            verify(selectAll, "Select All item not found");
            mouseClick(selectAll, selectAll.width / 2, selectAll.height / 2);
            waitForRendering(data.input, 1000);
            compare(data.input.text, data.input.selectedText, "Not all the text is selected");
            // delete with key press
            keyClick(Qt.Key_Backspace);
            waitForRendering(data.input, 1000);
            compare(data.input.text, "", "The text has not been deleted");

            // dismiss popover
            mouseClick(testMain, testMain.width / 2, testMain.height / 2);
            wait(200);
        }

        SignalSpy {
            id: cursorRectSpy
            signalName: "cursorRectangleChanged"
        }

        function test_input_pageup_pagedown_data() {
            return [
                {tag: "PageUp in TextField", input: textField, moveToEnd: true, key: Qt.Key_PageUp, modifier: Qt.NoModifier, xfail: false},
                {tag: "PageDown in TextField", input: textField, moveToEnd: false, key: Qt.Key_PageDown, modifier: Qt.NoModifier, xfail: false},
                {tag: "PageUp in TextArea", input: textArea, moveToEnd: true, key: Qt.Key_PageUp, modifier: Qt.NoModifier, xfail: false},
                {tag: "PageDown in TextArea", input: textArea, moveToEnd: false, key: Qt.Key_PageDown, modifier: Qt.NoModifier, xfail: false},
                {tag: "Ctrl+PageUp in TextField", input: textField, moveToEnd: true, key: Qt.Key_PageUp, modifier: Qt.ControlModifier, xfail: true},
                {tag: "Ctrl+PageDown in TextField", input: textField, moveToEnd: false, key: Qt.Key_PageDown, modifier: Qt.ControlModifier, xfail: true},
                {tag: "Ctrl+PageUp in TextArea", input: textArea, moveToEnd: true, key: Qt.Key_PageUp, modifier: Qt.ControlModifier, xfail: true},
                {tag: "Ctrl+PageDown in TextArea", input: textArea, moveToEnd: false, key: Qt.Key_PageDown, modifier: Qt.ControlModifier, xfail: true},
                {tag: "Shift+PageUp in TextField", input: textField, moveToEnd: true, key: Qt.Key_PageUp, modifier: Qt.ShiftModifier, xfail: true},
                {tag: "Shift+PageDown in TextField", input: textField, moveToEnd: false, key: Qt.Key_PageDown, modifier: Qt.ShiftModifier, xfail: true},
                {tag: "Shift+PageUp in TextArea", input: textArea, moveToEnd: true, key: Qt.Key_PageUp, modifier: Qt.ShiftModifier, xfail: true},
                {tag: "Shift+PageDown in TextArea", input: textArea, moveToEnd: false, key: Qt.Key_PageDown, modifier: Qt.ShiftModifier, xfail: true},
                {tag: "Alt+PageUp in TextField", input: textField, moveToEnd: true, key: Qt.Key_PageUp, modifier: Qt.AltModifier, xfail: true},
                {tag: "Alt+PageDown in TextField", input: textField, moveToEnd: false, key: Qt.Key_PageDown, modifier: Qt.AltModifier, xfail: true},
                {tag: "Alt+PageUp in TextArea", input: textArea, moveToEnd: true, key: Qt.Key_PageUp, modifier: Qt.AltModifier, xfail: true},
                {tag: "Alt+PageDown in TextArea", input: textArea, moveToEnd: false, key: Qt.Key_PageDown, modifier: Qt.AltModifier, xfail: true},
            ];
        }
        function test_input_pageup_pagedown(data) {
            var handler = findChild(data.input, "input_handler");
            data.input.focus = true;

            // move the cursor to the end
            if (data.moveToEnd) {
                keyClick(Qt.Key_End);
                waitForRendering(data.input, 500);
                verify(data.input.cursorPosition > 0, "The cursor wasn't moved");
            }
            cursorRectSpy.target = data.input;
            keyClick(data.key, data.modifier);
            waitForRendering(data.input, 500);
            if (data.xfail) {
                expectFailContinue(data.tag, "With modifier");
            }
            cursorRectSpy.wait(500);
            cursorRectSpy.target = null;
        }
    }
}
