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

TestCase {
     name: "TextAreaAPI"

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
         compare(textArea.cursorDelegate,textEdit.cursorDelegate,"TextArea.cursorDelegate is same as TextEdit.cursorDelegate")
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
         compare(textArea.mouseSelectionMode,textEdit.mouseSelectionMode,"TextArea.mouseSelectionMode is same as TextEdit.mouseSelectionMode")
     }

     function test_0_persistentSelection() {
         compare(textArea.persistentSelection,textEdit.persistentSelection,"TextArea.persistentSelection is same as TextEdit.persistentSelection")
     }

     function test_0_readOnly() {
         compare(textArea.readOnly,textEdit.readOnly,"TextArea.readOnly is same as TextEdit.readOnly")
     }

     function test_0_renderType() {
         compare(textArea.renderType,textEdit.renderType,"TextArea.renderType is same as TextEdit.renderType")
     }

     function test_0_selectByMouse() {
         compare(textArea.selectByMouse,textEdit.selectByMouse,"TextArea.selectByMouse is same as TextEdit.selectByMouse")
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
         compare(textArea.wrapMode,textEdit.wrapMode,"TextArea.wrapMode is same as TextEdit.wrapMode")
     }

// TextArea specific properties
     function test_contentHeight() {
         compare(textArea.contentHeight,-1,"contentHeight is TODO on default")
         var newValue = 200;
         textArea.contentHeight = newValue;
         compare(textArea.contentHeight,newValue,"set/get");
     }

     function test_contentWidth() {
         compare(textArea.contentWidth,-1,"contentWidth is TODO on default")
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

     function test_autoExpand() {
         compare(textArea.autoExpand,false,"TextArea.autoExpand is set to false");
         var newValue = true;
         textArea.autoExpand = newValue;
         compare(textArea.autoExpand,newValue,"set/get");
     }

     function test_baseUrl() {
         skip("TODO")
         compare(textArea.baseUrl,"tst_textarea.qml","baseUrl is QML file instantiating the TextArea item on default")
     }

     function test_displayText() {
         compare(textArea.displayText,"","displayText is '' on default")
         var newValue = "Hello Display Text";
         textArea.displayText = newValue;
         compare(textArea.displayText,newValue,"set/get");
     }

     function test_maximumLineCount() {
         compare(textArea.maximumLineCount,0,"maximumLineCount is 0 on default")
         var newValue = 10;
         textArea.maximumLineCount = newValue;
         compare(textArea.maximumLineCount,newValue,"set/get");
     }

// functions
     function test_copy() {
         textArea.copy();
     }

     function test_cut() {
         textArea.cut();
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
         textArea.paste();
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

// objects
     TextArea {
         id: textArea
         SignalSpy {
             id: signalSpy
             target: parent
         }
     }

     TextEdit {
         id: textEdit
     }
}
