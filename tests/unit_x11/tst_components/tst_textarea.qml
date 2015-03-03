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
import Ubuntu.Components.ListItems 1.0 as ListItem

Item {
    id: main
    width: units.gu(50); height: units.gu(100)

    property bool hasOSK: false

    Column {
        TextArea {
            id: textArea
            SignalSpy {
                id: signalSpy
                target: parent
            }

            property int keyPressData
            property int keyReleaseData
            Keys.onPressed: keyPressData = event.key
            Keys.onReleased: keyReleaseData = event.key
        }

        TextArea {
            id: colorTest
            color: colorTest.text.length < 4 ? "#0000ff" : "#00ff00"
        }

        TextEdit {
            id: textEdit
        }

        ListItem.Empty {
            id: listItem
            height: 200
            anchors.left: parent.left

            anchors.right: parent.right
            SignalSpy {
                id: listItemSpy
                signalName: "clicked"
                target: listItem
            }

            TextArea {
                id: input
                anchors.fill: parent
                Component.onCompleted: forceActiveFocus()
            }
        }

        TextArea {
            id: t1
            objectName: "t1"
        }
        TextArea {
            id: t2
            objectName: "t2"
        }

        TextArea {
            id: longText
            text: "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book."
        }
    }

    UbuntuTestCase {
        name: "TextAreaAPI"
        when: windowShown

        function init() {
            main.hasOSK = QuickUtils.inputMethodProvider !== ""
        }

        function cleanup() {
            textArea.focus =
            colorTest.focus =
            textEdit.focus =
            input.focus =
            t1.focus =
            t2.focus =
            longText.focus = false;
            longText.cursorPosition = 0;
            var scroller = findChild(longText, "input_scroller");
            scroller.contentY = 0;
            scroller.contentX = 0;

            textArea.text = "";
            textArea.textFormat = TextEdit.PlainText;
            input.textFormat = TextEdit.PlainText;
            input.text = "";
            input.cursorPosition = 0;

            // empty event buffer
            wait(200);
        }


        function test_activate() {
            textArea.forceActiveFocus();
            compare(textArea.activeFocus, true, "TextArea is active");
        }

        // TextEdit shared properties
        function test_0_activeFocusOnPress() {
            compare(textArea.activeFocusOnPress,textEdit.activeFocusOnPress,"TextArea.activeFocusOnPress is same as TextEdit.activeFocusOnPress")
        }

        function test_0_canPaste() {
            compare(textArea.canPaste,textEdit.canPaste,"TextArea.canPaste is same as TextEdit.canPaste")
        }

        function test_0_canRedo() {
            compare(textArea.canRedo,textEdit.canRedo,"TextArea.canRedo is same as TextEdit.canRedo")
        }

        function test_0_canUndo() {
            compare(textArea.canUndo,textEdit.canUndo,"TextArea.canUndo is same as TextEdit.canUndo")
        }

        function test_0_color() {
            compare(textArea.color,textEdit.color,"TextArea.color is same as TextEdit.canUndo")
        }

        function test_0_cursorDelegate() {
            verify(textArea.cursorDelegate === null, "TextArea.cursorDelegate is not null")
        }

        function test_0_cursorPosition() {
            compare(textArea.cursorPosition,textEdit.cursorPosition,"TextArea.cursorPosition is same as TextEdit.cursorPosition")
        }

        function test_0_cursorRectangle() {
            compare(textArea.cursorRectangle,textEdit.cursorRectangle,"TextArea.cursorRectangle is same as TextEdit.cursorRectangle")
        }

        function test_0_cursorVisible() {
            compare(textArea.cursorVisible,textEdit.cursorVisible,"TextArea.cursorVisible is same as TextEdit.cursorVisible")
        }

        function test_0_effectiveHorizontalAlignment() {
            compare(textArea.effectiveHorizontalAlignment,textEdit.effectiveHorizontalAlignment,"TextArea.effectiveHorizontalAlignment is same as TextEdit.effectiveHorizontalAlignment")
        }

        function test_0_font() {
            compare(textArea.font,textEdit.font,"TextArea.font is same as TextEdit.font")
        }

        function test_0_horizontalAlignment() {
            compare(textArea.horizontalAlignment,textEdit.horizontalAlignment,"TextArea.horizontalAlignment is same as TextEdit.horizontalAlignment")
        }

        function test_0_inputMethodComposing() {
            compare(textArea.inputMethodComposing,textEdit.inputMethodComposing,"TextArea.inputMethodComposing is same as TextEdit.inputMethodComposing")
        }

        function test_0_inputMethodHints() {
            compare(textArea.inputMethodHints,textEdit.inputMethodHints,"TextArea.inputMethodHints is same as TextEdit.inputMethodHints")
        }

        function test_0_length() {
            compare(textArea.length,textEdit.length,"TextArea.length is same as TextEdit.length")
        }

        function test_0_lineCount() {
            compare(textArea.lineCount,textEdit.lineCount,"TextArea.lineCount is same as TextEdit.lineCount")
        }

        function test_0_mouseSelectionMode() {
            compare(textArea.mouseSelectionMode, TextEdit.SelectWords,"TextArea.mouseSelectionMode is SelectWords")
        }

        function test_0_persistentSelection() {
            compare(textArea.persistentSelection,textEdit.persistentSelection,"TextArea.persistentSelection is same as TextEdit.persistentSelection")
        }

        function test_0_readOnly() {
            compare(textArea.readOnly,textEdit.readOnly,"TextArea.readOnly is same as TextEdit.readOnly")

            textArea.text = "ab";
            textArea.cursorPosition = 1;
            textArea.textFormat = TextEdit.PlainText;
            keyClick(Qt.Key_Return);
            compare("ab", textArea.text, "No split occurred in plain area");

            textArea.textFormat = TextEdit.RichText;
            keyClick(Qt.Key_Return);
            compare(textArea.text.indexOf("<br />"), -1, "No split occurred in rich area");
        }

        function test_0_renderType() {
            compare(textArea.renderType,textEdit.renderType,"TextArea.renderType is same as TextEdit.renderType")
        }

        function test_0_selectByMouse() {
            compare(textArea.selectByMouse,true,"TextArea.selectByMouse is true")
        }

        function test_0_selectedText() {
            compare(textArea.selectedText,textEdit.selectedText,"TextArea.selectedText is same as TextEdit.selectedText")
        }

        function test_0_selectedTextColor() {
            compare(textArea.selectedTextColor,textEdit.selectedTextColor,"TextArea.selectedTextColor is same as TextEdit.selectedTextColor")
        }

        function test_0_selectionColor() {
            compare(textArea.selectionColor,textEdit.selectionColor,"TextArea.selectionColor is same as TextEdit.selectionColor")
        }

        function test_0_selectionEnd() {
            compare(textArea.selectionEnd,textEdit.selectionEnd,"TextArea.selectionEnd is same as TextEdit.selectionEnd")
        }

        function test_0_selectionStart() {
            compare(textArea.selectionStart,textEdit.selectionStart,"TextArea.selectionStart is same as TextEdit.selectionStart")
        }

        function test_0_text() {
            compare(textArea.text,textEdit.text,"TextArea.text is same as TextEdit.text")
        }

        function test_0_textFormat() {
            compare(textArea.textFormat,textEdit.textFormat,"TextArea.textFormat is same as TextEdit.textFormat")
        }

        function test_0_verticalAlignment() {
            compare(textArea.verticalAlignment,textEdit.verticalAlignment,"TextArea.verticalAlignment is same as TextEdit.verticalAlignment")
        }

        function test_0_wrapMode() {
            compare(textArea.wrapMode,TextEdit.Wrap,"TextArea.wrapMode is TextEdit.Wrap")
        }

        // TextArea specific properties
        function test_0_highlighted() {
            compare(textArea.highlighted, textArea.focus, "highlighted is the same as focused");
        }

        function test_0_contentHeight() {
            verify(textArea.contentHeight > 0, "contentHeight over 0 units on default")
            var newValue = 200;
            textArea.contentHeight = newValue;
            compare(textArea.contentHeight,newValue,"set/get");
        }

        function test_0_contentWidth() {
            var style = findChild(textArea, "textarea_style");
            compare(textArea.contentWidth, units.gu(30) - 2 * style.frameSpacing, "contentWidth is the implicitWidth - 2 times the frame size on default")
            var newValue = 200;
            textArea.contentWidth = newValue;
            compare(textArea.contentWidth,newValue,"set/get");
        }

        function test_placeholderText() {
            compare(textArea.placeholderText,"","placeholderText is '' on default")
            var newValue = "Hello Placeholder";
            textArea.placeholderText = newValue;
            compare(textArea.placeholderText,newValue,"set/get");
        }

        function test_autoSize() {
            compare(textArea.autoSize,false,"TextArea.autoSize is set to false");
            var newValue = true;
            textArea.autoSize = newValue;
            compare(textArea.autoSize, newValue,"set/get");
        }

        function test_0_baseUrl() {
            expectFail("","TODO")
            compare(textArea.baseUrl,"tst_textarea.qml","baseUrl is QML file instantiating the TextArea item on default")
        }

        function test_displayText() {
            compare(textArea.displayText,"","displayText is '' on default")
            var newValue = "Hello Display Text";
            try {
                textArea.displayText = newValue;
                compare(false,true,"Unable to set value as it is read-only");
            } catch (e) {
                compare(true,true,"Unable to set value as it is read-only");
            }

        }

        function test_0_popover() {
            compare(textArea.popover, undefined, "Uses default popover");
        }

        function test_maximumLineCount() {
            compare(textArea.maximumLineCount,5,"maximumLineCount is 0 on default")
            var newValue = 10;
            textArea.maximumLineCount = newValue;
            compare(textArea.maximumLineCount,newValue,"set/get");
        }

        function test_0_visible() {
            textArea.visible = false;
            compare(textArea.activeFocus, false, "TextArea is inactive");
        }

        // functions
        function test_copy() {
            textArea.copy();
        }

        function test_cut() {
            Clipboard.clear();
            textArea.readOnly = false;
            textArea.text = "test text";
            textArea.cursorPosition = textArea.text.indexOf("text");
            textArea.selectWord();
            textArea.cut();
            compare(textArea.text, "test ", "Text cut properly");
            compare(Clipboard.data.text, "text", "Clipboard contains cut text");
            // we should have the "text" only ones
            var plainTextCount = 0;
            for (var i in Clipboard.data.formats) {
                if (Clipboard.data.formats[i] === "text/plain")
                    plainTextCount++;
            }
            compare(plainTextCount, 1, "Clipboard is correct");
        }

        function test_deselect() {
            textArea.deselect();
        }

        function test_getFormattedText() {
            textArea.getFormattedText(0,0);
        }

        function test_getText() {
            textArea.getText(0,0);
        }

        function test_insert() {
            textArea.insert(0,"Hello");
        }

        function test_isRightToLeft() {
            textArea.isRightToLeft(0,0);
        }

        function test_moveCursorSelection() {
            textArea.moveCursorSelection(0,0);
        }

        function test_paste() {
            textArea.readOnly = false;
            textArea.text = "test";
            textArea.cursorPosition = textArea.text.length;
            textArea.paste(" text");
            compare(textArea.text, "test text", "Data pasted");
        }

        function test_positionAt() {
            textArea.positionAt(0,1);
        }

        function test_positionToRectangle() {
            textArea.positionToRectangle(0);
        }

        function test_redo() {
            textArea.redo();
        }

        function test_remove() {
            textArea.remove(0,0);
        }

        function test_select() {
            textArea.select(0,0);
        }

        function test_selectAll() {
            textArea.selectAll();
        }

        function test_selectWord() {
            textArea.selectWord();
        }

        function test_undo() {
            textArea.undo();
        }


        // signals
        function test_linkActivated() {
            signalSpy.signalName = "linkActivated";
            compare(signalSpy.valid,true,"linkActivated signal exists")
        }

        // filters
        function test_keyPressAndReleaseFilter() {
            textArea.visible = true;
            textArea.forceActiveFocus();
            textArea.readOnly = false;
            textArea.keyPressData = 0;
            textArea.keyReleaseData = 0;
            keyClick(Qt.Key_T, Qt.NoModifier, 100);
            compare(textArea.keyPressData, Qt.Key_T, "Key press filtered");
            compare(textArea.keyReleaseData, Qt.Key_T, "Key release filtered");
        }

        function test_TextAreaInListItem_EnterCaptured() {
            input.forceActiveFocus();
            input.textFormat = TextEdit.PlainText;
            input.text = "";
            keyClick(Qt.Key_T);
            keyClick(Qt.Key_E);
            keyClick(Qt.Key_S);
            keyClick(Qt.Key_T);
            keyClick(Qt.Key_Enter);
            compare(input.text, "test\n", "Keys");
        }
        function test_TextAreaInListItem_EnterDoesNotProduceClick() {
            input.forceActiveFocus();
            input.textFormat = TextEdit.PlainText;
            input.text = "";
            listItemSpy.clear();
            keyClick(Qt.Key_Enter);
            tryCompare(listItemSpy, "count", 0, 100);
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

        function test_OSK_ShownWhenNextTextAreaIsFocused() {
            if (!hasOSK)
                expectFail("", "OSK can be tested only when present");
            t1.focus = true;
            compare(Qt.inputMethod.visible, true, "OSK is shown for the first TextArea");
            t2.focus = true;
            compare(Qt.inputMethod.visible, true, "OSK is shown for the second TextArea");
        }

        function test_RemoveOSKWhenFocusLost() {
            if (!hasOSK)
                expectFail("", "OSK can be tested only when present");
            t1.focus = true;
            compare(Qt.inputMethod.visible, true, "OSK is shown when TextArea gains focus");
            t1.focus = false;
            compare(Qt.inputMethod.visible, false, "OSK is hidden when TextArea looses focus");
        }

        function test_ReEnabledInput() {
            textArea.forceActiveFocus();
            textArea.enabled = false;
            compare(textArea.enabled, false, "textArea is disabled");
            compare(textArea.focus, true, "textArea is focused");
            compare(textArea.activeFocus, false, "textArea is not active focus");
            compare(Qt.inputMethod.visible, false, "OSK removed");

            textArea.enabled = true;
            compare(textArea.enabled, true, "textArea is enabled");
            compare(textArea.focus, true, "textArea is focused");
            compare(textArea.activeFocus, true, "textArea is active focus");
            if (!hasOSK)
                expectFail("", "OSK can be tested only when present");
            compare(Qt.inputMethod.visible, true, "OSK shown");
        }

        function test_TextareaInListItem_RichTextEnterCaptured() {
            textArea.text = "a<br />b";
            textArea.textFormat = TextEdit.RichText;
            input.forceActiveFocus();
            input.textFormat = TextEdit.RichText;
            input.text = "ab";
            input.cursorPosition = 1;
            keyClick(Qt.Key_Return);
            compare(input.text, textArea.text, "Formatted text split");
        }
    }
}
