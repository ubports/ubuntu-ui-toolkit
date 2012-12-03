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
// FIXME: When a module contains QML, C++ and JavaScript elements exported,
// we need to use named imports otherwise namespace collision is reported
// by the QML engine. As workaround, we use Theming named import.
// Bug to watch: https://bugreports.qt-project.org/browse/QTBUG-27645
import "." 0.1 as Theming

/*!
    \qmltype TextArea
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief The TextArea item displays a block of editable, scrollable, formatted
    text.

    The TextArea supports fix-size and auto-expanding modes. In fix-size mode the
    content is scrolled when exceeds the boundaries and can be scrolled both horizontally
    and vertically, depending on the contentWidth and contentHeight set. The following
    example will scroll the editing area both horizontally and vertically:

    \qml
    TextArea {
        width: units.gu(20)
        height: units.gu(12)
        contentWidth: units.gu(30)
        contentHeight: units.gu(60)
    }
    \endqml

    The auto-expand mode is realized using two properties: autoExpand and maximumLineCount.
    Setting autoExpand will set the implicit height to one line, and the height is aligned
    to the font's pixel size. If the

    \b{This component is under heavy development.}
  */

FocusScope {
    id: control
    implicitWidth: units.gu(30)
    implicitHeight: (autoExpand) ? internal.frameLinesHeight(1) : internal.frameLinesHeight(4)

    // new properties
    /*!
      */
    property alias placeholderText: hint.text

    /*!
      */
    property alias displayText: internal.displayText

    /*!
      */
    property bool selectByMouse: true

    /*!
      */
    property bool autoExpand: false

    /*!
      */
    property int maximumLineCount: 5

    // altered TextEdit properties
    /*!
      */
    property alias contentWidth: editor.width

    /*!
      */
    property alias contentHeight: editor.height

    // forwarded properties
    /*!
      */
    property alias activeFocusOnPress: editor.activeFocusOnPress

    /*!
      */
    property alias baseUrl: editor.baseUrl

    /*!
      */
    property alias canPaste: editor.canPaste

    /*!
      */
    property alias canRedo: editor.canRedo

    /*!
      */
    property alias canUndo: editor.canUndo

    /*!
      */
    property alias color: editor.color

    /*!
      */
    property alias cursorDelegate: editor.cursorDelegate

    /*!
      */
    property alias cursorPosition: editor.cursorPosition

    /*!
      */
    property alias cursorRectangle: editor.cursorRectangle

    /*!
      */
    property alias cursorVisible: editor.cursorVisible

    /*!
      */
    property alias effectiveHorizontalAlignment: editor.effectiveHorizontalAlignment

    /*!
      */
    property alias font: editor.font

    /*!
      */
    property alias horizontalAlignment: editor.horizontalAlignment

    /*!
      */
    property alias inputMethodComposing: editor.inputMethodComposing

    /*!
      */
    property alias inputMethodHints: editor.inputMethodHints

    /*!
      */
    property alias length: editor.length

    /*!
      */
    property alias lineCount: editor.lineCount

    /*!
      */
    property alias mouseSelectionMode: editor.mouseSelectionMode

    /*!
      */
    property alias persistentSelection: editor.persistentSelection

    /*!
      */
    property alias readOnly: editor.readOnly

    /*!
      */
    property alias renderType: editor.renderType

    /*!
      */
    property alias selectedTExt: editor.selectedText

    /*!
      */
    property alias selectedTextColor: editor.selectedTextColor

    /*!
      */
    property alias selectionColor: editor.selectionColor

    /*!
      */
    property alias selectionEnd: editor.selectionEnd

    /*!
      */
    property alias selectionStart: editor.selectionStart

    /*!
      */
    property alias text: editor.text

    /*!
      */
    property alias textFormat: editor.textFormat

    /*!
      */
    property alias verticalAlignment: editor.verticalAlignment

    /*!
      */
    property alias wrapMode:editor.wrapMode

    // signals
    /*!
      */
    signal linkActivated(string link)

    // functions
    /*!
      */
    function copy()
    {
        editor.copy();
    }

    /*!
      */
    function cut()
    {
        editor.cut();
    }

    /*!
      */
    function deselect()
    {
        editor.deselect();
    }

    /*!
      */
    function insert(position, text)
    {
        editor.insert(position, text);
    }

    /*!
      */
    function positionAt(x, y)
    {
        return editor.positionAt(x, y);
    }

    /*!
      */
    function isRightToLeft(start, end)
    {
        return editor.isRightToLeft(start, end)
    }

    /*!
      */
    function moveCursorSelection(position, mode)
    {
        if (mode === undefined)
            editor.moveCursorSelection(position, mouseSelectionMode);
        else
            editor.moveCursorSelection(position, mode);
    }

    /*!
      */
    function paste()
    {
        editor.paste();
    }

    /*!
      */
    function positionToRectangle(position)
    {
        return editor.positionToRectangle(position);
    }

    /*!
      */
    function redo()
    {
        editor.redo();
    }

    /*!
      */
    function select(start, end)
    {
        editor.select(start, end);
    }

    /*!
      */
    function selectAll()
    {
        editor.selectAll();
    }

    /*!
      */
    function selectWord()
    {
        editor.selectWord();
    }

    /*!
      */
    function getFormattedText(start, end)
    {
        return editor.getFormattedText(start, end);
    }

    /*!
      */
    function getText(start, end)
    {
        return editor.getText(start, end);
    }

    /*!
      */
    function remove(start, end)
    {
        return editor.remove(start, end);
    }

    /*!
      */
    function undo()
    {
        editor.undo();
    }

    // logic
    Component.onCompleted: {
        editor.linkActivated.connect(control.linkActivated)
    }

    // styling
    Theming.ItemStyle.class: "textarea"
    //internals
    QtObject {
        id: internal
        // public property locals enabling aliasing
        property string displayText: editor.getText(0, editor.length)
        property bool useTextEditMouse: true
        property bool textChanged: false
        property real spacing: StyleUtils.style(control, "overlaySpacing", units.gu(0.5))
        property real lineSpacing: units.dp(3)
        property real frameSpacing: StyleUtils.style(control, "frameSpacing", units.gu(0.35))
        property real lineSize: editor.font.pixelSize + lineSpacing
        property real frameLineHeight: editor.font.pixelSize + 2 * spacing + frameSpacing
        //selection properties
        property bool draggingMode: false
        property bool selectionMode: false
        property int selectionStart: 0
        property int selectionEnd: 0

        onLineSizeChanged: print(lineSize)
        onFrameLineHeightChanged: print(frameLineHeight)


        onDraggingModeChanged: {
            if (draggingMode) selectionMode = false;
        }
        onSelectionModeChanged: {
            if (selectionMode) draggingMode = false;
        }

        function activateEditor()
        {
            if (!control.activeFocus)
                control.forceActiveFocus();
        }

        function showInputPanel()
        {
            if (control.customSoftwareInputPanel != undefined) {
                // TODO implement once we have the SIP ready
            } else {
                Qt.inputMethod.show();
            }
        }
        function hideInputPanel()
        {
            if (control.customSoftwareInputPanel != undefined) {
                // TODO implement once we have the SIP ready
            } else {
                Qt.inputMethod.hide();
            }
        }

        function lineHeight(lines)
        {
            return editor.font.pixelSize * lines + lineSpacing * (lines - 1);
        }

        function frameLinesHeight(lines)
        {
            return lineHeight(lines) + 2 * spacing + frameSpacing;
        }

        function frameSize()
        {
            if (control.autoExpand) {
                if (!control.maximumLineCount || (control.maximumLineCount > 0) && (control.lineCount <= control.maximumLineCount))
                    control.height = frameLinesHeight(control.lineCount);
            }
        }
    }

    // cursor
    Component {
        id: cursor
        Item {
            id: customCursor
            property bool showCursor: (editor.forceCursorVisible || editor.activeFocus)
            property bool timerShowCursor: true

            Theming.ItemStyle.class: "cursor"
            height: internal.lineSize
            visible: showCursor && timerShowCursor
        }
    }

    // holding default values
    Label { id: fontHolder }
    SystemPalette { id: systemColors }

    //hint
    Label {
        id: hint
        anchors{
            fill: parent
            margins: internal.spacing
        }
        // hint is shown till user types something in the field
        visible: (editor.getText(0, editor.length) == "") && !editor.inputMethodComposing
    }

    //scrollbars and flickable
    Scrollbar {
        id: rightScrollbar
        flickableItem: flicker
    }
    Scrollbar {
        id: bottomScrollbar
        flickableItem: flicker
        align: Qt.AlignBottom
    }
    Flickable {
        id: flicker
        anchors {
            fill: parent
            margins: internal.spacing
        }
        clip: true
        contentWidth: editor.paintedWidth
        contentHeight: editor.paintedHeight
        // do not allow rebounding
        boundsBehavior: Flickable.StopAtBounds
        pressDelay: 200

        function ensureVisible(r)
        {
            if (moving || flicking)
                return;
            if (contentX >= r.x)
                contentX = r.x;
            else if (contentX+width <= r.x+r.width)
                contentX = r.x+r.width-width;
            if (contentY >= r.y)
                contentY = r.y;
            else if (contentY+height <= r.y+r.height)
                contentY = r.y+r.height-height;
        }

        // editor
        // Images are not shown when text contains <img> tags
        // bug to watch: https://bugreports.qt-project.org/browse/QTBUG-27071
        TextEdit {
            readOnly: false
            id: editor
            focus: true
            onCursorRectangleChanged: flicker.ensureVisible(cursorRectangle)
            width: Math.max(control.width, editor.contentWidth)
            height: Math.max(control.height, editor.contentHeight)
            wrapMode: TextEdit.WrapAtWordBoundaryOrAnywhere
            mouseSelectionMode: TextEdit.SelectCharacters
            selectByMouse: false
            cursorDelegate: cursor

            // styling
            Theming.ItemStyle.style: control.Theming.ItemStyle.style
            color: StyleUtils.style(editor, "color", "black")
            selectedTextColor: StyleUtils.style(editor, "selectedTextColor", systemColors.highlightedText)
            selectionColor: StyleUtils.style(editor, "selectionColor", systemColors.highlight)
            font: StyleUtils.style(editor, "font", fontHolder.font)

            // autoexpand handling
            onLineCountChanged: internal.frameSize()

            MouseArea {
                id: handler
                enabled: control.selectByMouse && control.enabled
                anchors.fill: parent
                propagateComposedEvents: true
                preventStealing: true

                onPressed: {
                    internal.activateEditor()
                    internal.draggingMode = true
                }
                onPressAndHold: {
                    // move mode gets false if there was a mouse move after the press;
                    // this is needed as Flickable will send a pressAndHold in case of
                    // press -> move-pressed ->stop-and-hold-pressed gesture is performed
                    if (!internal.draggingMode)
                        return;
                    internal.selectionMode = true;
                    editor.cursorPosition = editor.positionAt(mouse.x, mouse.y)
                }
                onReleased: {
                    internal.draggingMode = internal.selectionMode = false;
                }
                onPositionChanged: {
                    if (internal.draggingMode) {
                        internal.draggingMode = false;
                        mouse.accepted = false;
                        return;
                    }
                    if (internal.selectionMode) editor.moveCursorSelection(editor.positionAt(mouse.x, mouse.y))
                }
                onDoubleClicked: {
                    internal.activateEditor()
                    editor.selectWord()
                }

                onClicked: {
                    internal.activateEditor()
                    editor.cursorPosition = editor.positionAt(mouse.x, mouse.y)
                }
            }
        }
    }
}
