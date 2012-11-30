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
    \brief The TextArea element is a multiline text editor.

    \b{This component is under heavy development.}
  */

FocusScope {
    id: control
    implicitWidth: units.gu(30)
    implicitHeight: (autoExpand) ? units.gu(3) : units.gu(12)

    // new properties
    property alias placeholderText: hint.text
    property string displayText: editor.getText(0, editor.length)
    property bool selectByMouse: true
    property bool autoExpand: false
    property int maximumLineCount: 5

    // altered TextEdit properties
    property alias contentWidth: editor.width
    property alias contentHeight: editor.height

    // forwarded properties
    property alias activeFocusOnPress: editor.activeFocusOnPress
    property alias baseUrl: editor.baseUrl
    property alias canPaste: editor.canPaste
    property alias canRedo: editor.canRedo
    property alias canUndo: editor.canUndo
    property alias color: editor.color
    property alias cursorDelegate: editor.cursorDelegate
    property alias cursorPosition: editor.cursorPosition
    property alias cursorRectangle: editor.cursorRectangle
    property alias cursorVisible: editor.cursorVisible
    property alias effectiveHorizontalAlignment: editor.effectiveHorizontalAlignment
    property alias font: editor.font
    property alias horizontalAlignment: editor.horizontalAlignment
    property alias inputMethodComposing: editor.inputMethodComposing
    property alias inputMethodHints: editor.inputMethodHints
    property alias length: editor.length
    property alias lineCount: editor.lineCount
    property alias mouseSelectionMode: editor.mouseSelectionMode
    property alias persistentSelection: editor.persistentSelection
    property alias readOnly: editor.readOnly
    property alias renderType: editor.renderType
    property alias selectedTExt: editor.selectedText
    property alias selectedTextColor: editor.selectedTextColor
    property alias selectionColor: editor.selectionColor
    property alias selectionEnd: editor.selectionEnd
    property alias selectionStart: editor.selectionStart
    property alias text: editor.text
    property alias textFormat: editor.textFormat
    property alias verticalAlignment: editor.verticalAlignment
    property alias wrapMode:editor.wrapMode

    // signals
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
        property bool useTextEditMouse: true
        property bool textChanged: false
        property real spacing: units.gu(0.5)
        property real lineSize: editor.font.pixelSize + units.dp(3)
        //selection properties
        property bool draggingMode: false
        property bool selectionMode: false
        property int selectionStart: 0
        property int selectionEnd: 0

        onDraggingModeChanged: {
            print(draggingMode);
            if (draggingMode) selectionMode = false;
        }
        onSelectionModeChanged: {
            print(selectionMode);
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

    Text {
        id: fontHolder
    }
    SystemPalette {
        id: systemColors
    }

    //hint
    TextCustom {
        id: hint
        anchors{
            fill: parent
            margins: internal.spacing
        }
        // hint is shown till user types something in the field
        visible: (editor.getText(0, editor.length) == "") && !editor.inputMethodComposing
    }

    //scrollbars
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
        //onFlickingChanged: if (flicking) handler.cancelled = true
        //onMovingChanged: if (moving) handler.cancelled = true

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
            selectByMouse: false//handler.selectionMode
            cursorDelegate: cursor

            Theming.ItemStyle.style: control.Theming.ItemStyle.style
            color: StyleUtils.style(editor, "color", "black")
            selectedTextColor: StyleUtils.style(editor, "selectedTextColor", systemColors.highlightedText)
            selectionColor: StyleUtils.style(editor, "selectionColor", systemColors.highlight)
            font: StyleUtils.style(editor, "font", fontHolder.font)

            MouseArea {
                id: handler
                anchors.fill: parent
                propagateComposedEvents: true
                preventStealing: true

                onPressed: {
                    print()
                    internal.activateEditor()
                    internal.draggingMode = true
                }
                onPressAndHold: {
                    print()
                    // move mode gets false if there was a mouse move after the press;
                    // this is needed as Flickable will send a pressAndHold in case of
                    // press -> move-pressed ->stop-and-hold-pressed gesture is performed
                    if (!internal.draggingMode)
                        return;
                    internal.selectionMode = true;
                    editor.cursorPosition = editor.positionAt(mouse.x, mouse.y)
                }
                onReleased: {
                    print()
                    internal.draggingMode = internal.selectionMode = false;
                }
                onPositionChanged: {
                    print()
                    if (internal.draggingMode) {
                        internal.draggingMode = false;
                        mouse.accepted = false;
                        return;
                    }
                    if (internal.selectionMode) editor.moveCursorSelection(editor.positionAt(mouse.x, mouse.y))
                }
                onDoubleClicked: {
                    print()
                    internal.activateEditor()
                    editor.selectWord()
                }

                onClicked: {
                    print()
                    internal.activateEditor()
                    editor.cursorPosition = editor.positionAt(mouse.x, mouse.y)
                }
            }
        }
    }
}
