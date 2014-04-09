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
import Ubuntu.Unity.Action 1.1 as UnityActions

Item {
    id: textItem
    width: units.gu(50); height: units.gu(70)

    property bool hasOSK: QuickUtils.inputMethodProvider !== ""

    function reset() {
        colorTest.focus = false;
        textField.focus = false;
        t1.focus = false;
        t2.focus = false;
    }

    Column {
        TextField {
            id: colorTest
            color: colorTest.text.length < 4 ? "#0000ff" : "#00ff00"
            text: "colorTest"
        }

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
            action: Action {
                enabled: true
                name: 'spam'
                text: 'Spam'
            }
        }

        TextField {
            id: t1
            text: "t1"
        }
        TextField {
            id: t2
            text: "t2"
        }

        TextField {
            id: enabledTextField
            enabled: true
            text: "enabledTextField"
        }

        TextField {
            id: disabledTextField
            enabled: false
            text: "disabledTextField"
        }
        TextField {
            id: longText
            text: "The orange (specifically, the sweet orange) is the fruit of the citrus species Citrus × ​sinensis in the family Rutaceae."
        }
    }

    UbuntuTestCase {
        name: "TextFieldAPI"
        when: windowShown

        // initialize test objects
        function init() {
            longText.cursorPosition = 0;
        }

        // empty event buffer
        function cleanup() {
            colorTest.focus =
            textField.focus =
            t1.focus =
            t2.focus =
            enabledTextField.focus =
            longText.focus = false;
            var scroller = findChild(longText, "textfield_scroller");
            scroller.contentX = 0;
            wait(200);
        }

        function initTestCase() {
            textField.forceActiveFocus();
            compare(textField.focus, true, "TextField is focused");
            // clear clipboard
            Clipboard.clear();
        }

        function test_0_popover() {
            compare(textField.popover, undefined, "No poppover defined by default.");
        }

        function test_0_highlighted() {
            compare(textField.highlighted, textField.focus, "highlighted is the same as focused");
        }

        function test_0_acceptableInput() {
            compare(textField.acceptableInput,true,"acceptableInput true by default")
        }

        function test_0_activeFocusOnPress() {
            compare(textField.activeFocusOnPress, true,"activeFocusOnPress true by default")
        }

        function test_0_autoScroll() {
            compare(textField.autoScroll, true,"autoScroll true by default")
        }

        function test_0_canPaste() {
            compare(textField.canPaste, false,"calPaste false when clipboard is empty")
        }

        function test_0_canRedo() {
            compare(textField.canRedo, false,"calRedo false when no data was entered")
        }

        function test_0_canUndo() {
            compare(textField.canUndo, false,"calUndo false when no data entered")
        }

        function test_0_contentWidth() {
            compare(textField.contentWidth, 0,"contentWidth by default")
        }

        function test_0_contentHeight() {
            // line size is the font pixel size + 3 dp
            var lineSize = textField.font.pixelSize + units.dp(3)
            compare(textField.contentHeight, lineSize,"contentHeight by default")
        }

        function test_0_cursorDelegate() {
            verify(textField.cursorDelegate, "cursorDelegate set by default")
        }

        function test_0_cursorPosition() {
            compare(textField.cursorPosition, 0, "cursorPosition 0 by default")
        }

        function test_0_cursorRectangle() {
            compare(textField.cursorRectangle, Qt.rect(0, 0, 0, 0), "cursorRectangle 0 by default")
        }

        function test_0_cursorVisible() {
            compare(textField.cursorVisible, false, "cursorVisible false by default when inactive");
            textField.focus = true;
            compare(textField.cursorVisible, true, "cursorVisible true by default when active");
        }

        function test_0_customSoftwareInputPanel() {
            compare(textField.customSoftwareInputPanel,null,"customSoftwareInputPanel is null by default")
        }

        function test_0_displayText() {
            compare(textField.displayText, "", "displayText empty by default")
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

        function test_0_alignments() {
            compare(textField.horizontalAlignment, TextInput.AlignLeft, "horizontalAlignmen is Left by default")
            compare(textField.effectiveHorizontalAlignment, TextInput.AlignLeft, "effectiveHorizontalAlignmen is Left by default")
            compare(textField.verticalAlignment, TextInput.AlignTop, "verticalAlignmen is Top by default")
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

        function test_0_inputMethodHints() {
            compare(textField.inputMethodHints, Qt.ImhNone, "inputMethodHints is Qt.ImhNone by default")
        }

        function test_0_length() {
            compare(textField.length, 0, "length is 0 by default")
        }

        function test_0_maximumLength() {
            compare(textField.maximumLength, 32767, "maximumLength is 32767 by default")
        }

        function test_0_mouseSelectionMode() {
            compare(textField.mouseSelectionMode, TextInput.SelectCharacters, "mouseSelectionMode default")
        }

        function test_0_passwordCharacter() {
            compare(textField.passwordCharacter, "\u2022", "passwordCharacter default")
        }

        function test_0_persistentSelection() {
            compare(textField.persistentSelection, false, "persistentSelection default")
        }

        function test_0_renderType() {
            compare(textField.renderType, Text.QtRendering, "renderType default")
        }

        function test_0_selectByMouse() {
            compare(textField.selectByMouse, true, "selectByMouse default")
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

        function test_undo_redo() {
            textField.readOnly = false;
            textField.text = "";
            textField.focus = true;
            keyClick(Qt.Key_T); keyClick(Qt.Key_E); keyClick(Qt.Key_S); keyClick(Qt.Key_T);
            compare(textField.text, "test", "new text");
            if (!textField.canUndo) expectFail("", "undo is not allowed in this input");
            textField.undo();
            compare(textField.text, "", "undone");
            textField.redo();
            compare(textField.text, "test", "redone");
        }

        function test_getText() {
            textField.text = "this is a longer text";
            compare(textField.getText(0, 10), "this is a ", "getText(0, 10)");
            compare(textField.getText(10, 0), "this is a ", "getText(10, 0)");
            compare(textField.getText(0), "", "getText(0)");
            compare(textField.getText(4, 0), "this", "getText(4, 0)");
        }

        function test_removeText() {
            textField.text = "this is a longer text";
            textField.remove(0, 10);
            compare(textField.text, "longer text", "remove(0, 10)");

            textField.text = "this is a longer text";
            textField.remove(10, 0);
            compare(textField.text, "longer text", "remove(0, 10)");

            textField.text = "this is a longer text";
            textField.remove(0);
            compare(textField.text, "this is a longer text", "remove(0)");

            textField.text = "this is a longer text";
            textField.remove(4, 0);
            compare(textField.text, " is a longer text", "remove(4, 0)");

            textField.text = "this is a longer text";
            textField.select(0, 4);
            textField.remove();
            compare(textField.text, "this is a longer text", "select(0, 4) && remove()");
        }

        function test_moveCursorSelection() {
            textField.text = "this is a longer text";
            textField.cursorPosition = 5;
            textField.moveCursorSelection(9, TextInput.SelectCharacters);
            compare(textField.selectedText, "is a", "moveCursorSelection from 5 to 9, selecting the text");
        }

        function test_isRightToLeft() {
            textField.text = "this is a longer text";
            compare(textField.isRightToLeft(0), false, "isRightToLeft(0)");
            compare(textField.isRightToLeft(0, 0), false, "isRightToLeft(0, 0)");
            compare(textField.isRightToLeft(5, 10), false, "isRightToLeft(5, 10)");
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

        function test_colorCollisionOnDelegate() {
            // fixes bug lp:1169601
            colorTest.text = "abc";
            compare(colorTest.color, "#0000ff", "Color when text length < 4");
            colorTest.text = "abcd";
            compare(colorTest.color, "#00ff00", "Color when text length >= 4");
        }

        function test_OneActiveFocus() {
            t1.focus = true;
            compare(t1.activeFocus, true, "T1 has activeFocus");
            compare(t2.activeFocus, false, "T1 has activeFocus");
            t2.focus = true;
            compare(t1.activeFocus, false, "T1 has activeFocus");
            compare(t2.activeFocus, true, "T1 has activeFocus");
        }

        // need to make the very first test case, otherwise OSK detection fails on phablet
        function test_OSK_ShownWhenNextTextFieldIsFocused() {
            if (!hasOSK)
                expectFail("", "OSK can be tested only when present");
            t1.focus = true;
            compare(Qt.inputMethod.visible, true, "OSK is shown for the first TextField");
            t2.focus = true;
            compare(Qt.inputMethod.visible, true, "OSK is shown for the second TextField");
        }

        function test_RemoveOSKWhenFocusLost() {
            if (!hasOSK)
                expectFail("", "OSK can be tested only when present");
            t1.focus = true;
            compare(Qt.inputMethod.visible, true, "OSK is shown when TextField gains focus");
            t1.focus = false;
            compare(Qt.inputMethod.visible, false, "OSK is hidden when TextField looses focus");
        }

        function test_ReEnabledInput() {
            textField.forceActiveFocus();
            textField.enabled = false;
            compare(textField.enabled, false, "textField is disabled");
            compare(textField.focus, true, "textField is focused");
            compare(textField.activeFocus, false, "textField is not active focus");
            compare(Qt.inputMethod.visible, false, "OSK removed");

            textField.enabled = true;
            compare(textField.enabled, true, "textField is enabled");
            compare(textField.focus, true, "textField is focused");
            compare(textField.activeFocus, true, "textField is active focus");
            if (!hasOSK)
                expectFail("", "OSK can be tested only when present");
            compare(Qt.inputMethod.visible, true, "OSK shown");
        }

        function test_Trigger() {
            signalSpy.signalName = 'accepted'
            textField.enabled = true
            textField.text = 'eggs'
            textField.accepted()
            signalSpy.wait()
        }

        function test_ActionInputMethodHints() {
            // Preset digit only for numbers
            textField.inputMethodHints = Qt.ImhNone
            textField.action.parameterType = UnityActions.Action.Integer
            compare(textField.inputMethodHints, Qt.ImhDigitsOnly)

            textField.inputMethodHints = Qt.ImhNone
            textField.action.parameterType = UnityActions.Action.Real
            compare(textField.inputMethodHints, Qt.ImhDigitsOnly)

            // No preset for strings
            textField.inputMethodHints = Qt.ImhNone
            textField.action.parameterType = UnityActions.Action.String
            compare(textField.inputMethodHints, Qt.ImhNone)

            // Never interfere with a manual setting
            textField.inputMethodHints = Qt.ImhDate
            textField.action.parameterType = UnityActions.Action.Integer
            compare(textField.inputMethodHints, Qt.ImhDate)
        }

        RegExpValidator {
            id: regExpValidator
            regExp: /[a-z]*/
        }

        function test_click_enabled_textfield_must_give_focus() {
            textField.forceActiveFocus();
            compare(enabledTextField.focus, false, 'enabledTextField is not focused');
            mouseClick(enabledTextField, enabledTextField.width/2, enabledTextField.height/2);
            compare(enabledTextField.focus, true, 'enabledTextField is focused');
        }

        function test_click_disabled_textfield_must_not_give_focus() {
            mouseClick(disabledTextField, disabledTextField.width/2, disabledTextField.height/2);
            compare(textField.focus, false, 'disabledTextField is not focused');
        }


        // text selection
        SignalSpy {
            id: flickSpy
            signalName: "onFlickEnded"
        }

        function test_scroll_when_not_focused() {
            var handler = findChild(longText, "input_handler");
            verify(handler);

            flickSpy.target = findChild(longText, "textfield_scroller");
            flickSpy.clear();
            // scroll when inactive
            verify(longText.focus == false);
            var x = longText.width / 2;
            var y = longText.height / 2;
            var dx = x;
            flick(longText, x, y, -dx, 0);
            verify(longText.focus);
            compare(flickSpy.count, 0, "The input had scrolled while inactive");
        }

        function test_scroll_when_focused() {
            longText.focus = true;
            var handler = findChild(longText, "input_handler");
            verify(handler);

            flickSpy.target = findChild(longText, "textfield_scroller");
            flickSpy.clear();
            var x = longText.width / 2;
            var y = longText.height / 2;

            compare(handler.state, "", "The input is not in default state before selection");
            flick(longText, x, y, - x, 0);
            flickSpy.wait();
            compare(handler.state, "", "The input has not returned to default state.");
        }

        function test_scroll_with_selected_text() {
            longText.focus = true;
            var handler = findChild(longText, "input_handler");
            verify(handler);
            var y = longText.height / 2;
            flickSpy.target = findChild(longText, "textfield_scroller");
            flickSpy.clear();

            // select text
            compare(handler.state, "", "The input is not in default state before selection");
            flick(longText, 0, y, units.gu(8), 0, 400);
            verify(longText.selectedText !== "");
            compare(handler.state, "", "The input has not returned to default state.");

            // flick
            var dx = longText.width / 2;
            flick(longText, dx, y, -dx, 0);
            flickSpy.wait();
            compare(handler.state, "", "The input has not returned to default state.");
            verify(longText.selectedText !== "");
        }

        function test_select_by_pressAndDrag() {
            longText.focus = true;
            var handler = findChild(longText, "input_handler");
            verify(handler);
            var dx = longText.width / 4;
            var x = units.gu(5);
            var y = longText.height / 2;
            compare(handler.state, "", "The input is not in default state before selection");
            flick(longText, x, y, 2*dx, 0, 400);
            verify(longText.selectedText !== "");
            compare(handler.state, "", "The input has not returned to default state.");
        }

        function test_select_text_doubletap() {
            longText.focus = true;
            var x = units.gu(2);
            var y = longText.height / 4;
            mouseDoubleClick(longText, x, y);
            expectFail("", "mouseDoubleClick fails to trigger");
            verify(longText.selectedText !== "");
        }

        function test_press_and_hold_moves_cursor_position() {
            longText.focus = true;
            var handler = findChild(longText, "input_handler");
            var y = longText.height / 2;
            flickSpy.target = findChild(longText, "textfield_scroller");
            flickSpy.clear();

            // long press
            compare(handler.state, "", "The input is not in default state before long press");
            mouseLongPress(longText, units.gu(8), y);
            verify(longText.cursorPosition != 0);
            compare(handler.state, "select", "The input is not in selection state");

            // cleanup, release the mouse, that should bring the handler back to defautl state
            mouseRelease(longText, units.gu(2), y);
            compare(handler.state, "", "The input has not returned to default state.");
        }

        SignalSpy {
            id: popoverSpy
            signalName: "onPressAndHold"
        }

        function test_press_and_hold_over_selected_text() {
            longText.focus = true;
            var handler = findChild(longText, "input_handler");
            var y = longText.height / 2;
            flickSpy.target = findChild(longText, "textfield_scroller");
            flickSpy.clear();
            popoverSpy.target = handler;
            popoverSpy.clear();

            // select text
            compare(handler.state, "", "The input is not in default state before long press");
            flick(longText, 0, y, units.gu(8), 0, 400);
            verify(longText.selectedText !== "");
            compare(handler.state, "", "The input has not returned to default state.");

            mouseLongPress(longText, units.gu(7), y);
            compare(handler.state, "select", "The input is not in selection state");
            // wait till popover is shown
            waitForRendering(longText);
            popoverSpy.wait();
            // check if we still have the text selected
            verify(longText.selectedText !== "");

            // cleanup, release the mouse, that should bring the handler back to default state
            mouseRelease(textItem, 0, 0);
            compare(handler.state, "", "The input has not returned to default state.");
            mouseClick(longText, 10, 10);
            // but we should still have the text selected
            verify(longText.selectedText !== "");
        }

        function test_clear_selection_by_click_on_selection() {
            longText.focus = true;
            var handler = findChild(longText, "input_handler");
            var y = longText.height / 2;
            flickSpy.target = findChild(longText, "textfield_scroller");
            flickSpy.clear();

            // select text
            compare(handler.state, "", "The input is not in default state before long press");
            flick(longText, 0, y, units.gu(8), 0, 400);
            compare(handler.state, "", "The input has not returned to default state.");
            verify(longText.selectedText !== "");

            // click on selection
            mouseClick(longText, units.gu(4), y);
            verify(longText.selectedText === "");
        }

        function test_clear_selection_by_click_beside_selection() {
            longText.focus = true;
            var handler = findChild(longText, "input_handler");
            var y = longText.height / 2;
            flickSpy.target = findChild(longText, "textfield_scroller");
            flickSpy.clear();

            // select text
            compare(handler.state, "", "The input is not in default state before long press");
            print(1)
            flick(longText, 0, y, units.gu(8), 0, 400);
            print(2)
            compare(handler.state, "", "The input has not returned to default state.");
            verify(longText.selectedText !== "");

            // click on selection
            mouseClick(longText, units.gu(10), y);
            verify(longText.selectedText === "");
        }

        function test_rightclick_does_not_open_popover_when_not_focused() {
            var handler = findChild(longText, "input_handler");
            var x = longText.width / 2;
            var y = longText.height / 2;
            popoverSpy.target = handler;
            popoverSpy.clear();

            // rclick should not bring popover in
            mouseClick(longText, x, y, Qt.RightButton);
            compare(popoverSpy.count, 0, "Right click should not open popover when the input is not focused.");
            compare(handler.state, "inactive", "The input is not in inactive state.");
        }

        function test_rightclick_opens_popover_when_focused() {
            longText.focus = true;
            var handler = findChild(longText, "input_handler");
            var x = longText.width / 2;
            var y = longText.height / 2;
            popoverSpy.target = handler;
            popoverSpy.clear();

            // rclick should bring popover in
            mouseClick(longText, x, y, Qt.RightButton);
            waitForRendering(longText);
            popoverSpy.wait();
            compare(handler.state, "", "The input is not in default state.");

            // take the popover away, that should bring the handler back to default state
            mouseClick(textItem, 0, 0);
            compare(handler.state, "", "The input has not returned to default state.");
        }
    }
}
