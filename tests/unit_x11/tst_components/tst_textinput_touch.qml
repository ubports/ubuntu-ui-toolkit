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
import TestExtras 1.0

Item {
    id: testMain
    width: units.gu(40)
    height: units.gu(71)

    Column {
        spacing: units.gu(1)
        TextField {
            id: textField
            text: "This is a single line text input called TextField."
        }
        TextArea {
            id: textArea
            text: "This is a multiline text input component called TextArea. It supports fix size as well as auto-expanding behavior. The content is scrollable only if it exceeds the visible area."
        }
    }

    UbuntuTestCase {
        name: "TextInputTouchTests"
        when: windowShown

        function initTestCase() {
            TestExtras.registerTouchDevice();
        }

        function init() {
            textField.cursorPosition = 0;
            textArea.cursorPosition = 0;
        }
        function cleanup() {
            textField.focus = false;
            textArea.focus = false;
        }

        function test_has_caret_when_touchscreen_data() {
            return [
                {tag: "TextField", input: textField},
                {tag: "TextArea", input: textArea},
            ];
        }
        function test_has_caret_when_touchscreen(data) {
            data.input.focus = true;
            waitForRendering(data.input);

            var cursor = findChild(data.input, "textCursor");
            verify(cursor, "Cursor not accessible, FAIL");
            verify(cursor.caret, "No caret is set");
            compare(cursor.caret.visible, true, "Caret is not visible")
        }
    }
}
