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
    id: textItem
    width: 200; height: 200
    TestCase {
        name: "TextFieldAPI"
        when: windowShown

        TextField {
            id: textField
            SignalSpy {
                id: signalSpy
                target: parent
            }

            property int keyPressData
            property int keyReleaseData
            Keys.onPressed: keyPressData = event.key
            Keys.onReleased: keyReleaseData = event.key
        }

        function initTestCase() {
            textField.forceActiveFocus();
            compare(textField.focus, true, "TextField is focused");
        }

        function test_0_highlighted() {
            compare(textField.highlighted, textField.focus, "highlighted is the same as focused");
        }

        function test_0_acceptableInput() {
            compare(textField.acceptableInput,true,"acceptableInput true by default")
        }

        function test_0_cursorPosition() {
            compare(textField.cursorPosition,0,"cursorPosition 0 by default")
        }

        function test_0_customSoftwareInputPanel() {
            compare(textField.customSoftwareInputPanel,null,"customSoftwareInputPanel is null by default")
        }

        function test_0_echoMode() {
            compare(textField.echoMode, TextInput.Normal,"echoMode is TextInput.Normal by default")
        }

        function test_0_errorHighlight() {
            compare(textField.errorHighlight, false,"errorHighlight is false by default")
            textField.errorHighlight = true
            compare(textField.errorHighlight,true,"set/get")
        }

        function test_0_font() {
            verify((textField.font),"font is set")
        }

        function test_hasClearButton() {
            compare(textField.hasClearButton, true, "hasClearButton is false by default")
            textField.hasClearButton = false
            compare(textField.hasClearButton, false, "set/get")
        }

        function test_0_inputMask() {
            compare(textField.inputMask, "", "inputMask is undefined by default")
        }

        function test_0_inputMethodComposing() {
            compare(textField.inputMethodComposing, false, "inputMethodComposing is false by default")
        }

        function test_0_maximumLength() {
            compare(textField.maximumLength, 32767, "maximumLength is 32767 by default")
        }

        function test_0_placeholderText() {
            compare(textField.placeholderText, "", "placeholderText is '' by default")
        }

        function test_0_primaryItem() {
            expectFail("","https://bugs.launchpad.net/tavastia/+bug/1076768")
            compare(textField.primaryItem, undefined, "primaryItem is undefined by default")
        }

        function test_0_readOnly() {
            compare(textField.readOnly, false, "readOnly is false by default")
            textField.readOnly = true
            compare(textField.readOnly, true, "set/get")
        }

        function test_0_secondaryItem() {
            expectFail("","https://bugs.launchpad.net/tavastia/+bug/1076768")
            compare(textField.secondaryItem, undefined, "secondaryItem is undefined by default")
        }

        function test_0_selectedText() {
            compare(textField.selectedText, "", "selectedText is '' by default")
        }

        function test_0_selectionEnd() {
            compare(textField.selectionEnd, 0, "selectionEnd is 0 by default")
        }

        function test_0_selectionStart() {
            compare(textField.selectionStart, 0, "selectionStart is 0 by default")
        }

        function test_0_text() {
            compare(textField.text, "", "text is '' by default")
            var newText = "Hello World!"
            textField.text = newText
            compare(textField.text, newText, "set/get")
        }

        function test_0_validator() {
            compare(textField.validator, null, "validator is null by default")
            textField.validator = regExpValidator
            compare(textField.validator, regExpValidator, "set/get")
        }

        function test_validator_and_acceptableInput_with_invalid_value() {
            textField.validator = null
            compare(textField.acceptableInput,true,"acceptableInput should be true")
            textField.validator = regExpValidator
            textField.text = "012345"
            compare(textField.acceptableInput,false,"with validator failure the acceptableInput should be false")
        }

        function test_0_accepted() {
            signalSpy.signalName = "accepted";
            compare(signalSpy.valid,true,"accepted signal exists")
        }

        function test_0_visible() {
            textField.visible = false;
            compare(textField.visible, false, "TextField is inactive");
        }

        function test_keyPressAndReleaseFilter() {
            textField.visible = true;
            textField.forceActiveFocus();
            textField.readOnly = false;
            textField.keyPressData = 0;
            textField.keyReleaseData = 0;
            keyClick(Qt.Key_Control, Qt.NoModifier, 200);
            compare(textField.keyPressData, Qt.Key_Control, "Key press filtered");
            compare(textField.keyReleaseData, Qt.Key_Control, "Key release filtered");
        }

        function test_cut() {
            Clipboard.clear();
            textField.readOnly = false;
            textField.text = "test text";
            textField.cursorPosition = textField.text.indexOf("text");
            textField.selectWord();
            textField.cut();
            compare(textField.text, "test ", "Text cut properly");
            compare(Clipboard.data.text, "text", "Clipboard has the text cut");
            // we should have the "text" only ones
            var plainTextCount = 0;
            for (var i in Clipboard.data.formats) {
                if (Clipboard.data.formats[i] === "text/plain")
                    plainTextCount++;
            }
            compare(plainTextCount, 1, "Clipboard is correct");
        }

        function test_paste() {
            textField.readOnly = false;
            textField.text = "test";
            textField.cursorPosition = textField.text.length;
            textField.paste(" text");
            compare(textField.text, "test text", "Data pasted");
        }

        RegExpValidator {
            id: regExpValidator
            regExp: /[a-z]*/
        }
    }
}
