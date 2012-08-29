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

import QtQuick 1.1
import "fontUtils.js" as FontUtils

/*!
    \qmlclass TextField
    \inqmlmodule Ubuntu.Components 0.1
    \brief The ActivityIndicator component is DOCME.

    \b{This component is under heavy development.}

    Example:
    \qml
    Item {
        TextField {
            id: input
            placeholderText: "hint text"
        }

        TextField {
            id: inputWithClear
            hasClearButton: true
            alwaysShowClearButton: false
        }

        TextField {
            id: autoClear
            clearContentOnEditingStart: true
        }
    }
    \endqml
*/
FocusScope {
    id: control

    implicitWidth: 200
    implicitHeight: 30

    /*!
      \preliminary
    */
    property alias placeholderText: hint.text

    /*!
      \preliminary
    */
    property bool hasClearButton: false

    /*!
      \preliminary
    */
    property bool alwaysShowClearButton: true

    /*!
      \preliminary
    */
    property bool clearContentOnEditingStart: false

    /*!
      \preliminary
    */
    property Item customInputPanel

    // aliased properties from TextInput
    property alias acceptableInput: editor.acceptableInput
    property alias autoScroll: editor.autoScroll
    property alias canPaste: editor.canPaste
    property alias color: editor.color
    property alias cursorDelegate: editor.cursorDelegate
    property alias cursorPosition: editor.cursorPosition
    property alias cursorVisible: editor.cursorVisible
    property alias displayText: editor.displayText
    property alias echoMode: editor.echoMode
    property alias horizontalAlignment: editor.horizontalAlignment
    property alias inputMask: editor.inputMask
    property alias inputMethodComposing: editor.inputMethodComposing
    property alias maximumLength: editor.maximumLength
    property alias mouseSelectionMode: editor.mouseSelectionMode
    property alias passwordCharacter: editor.passwordCharacter
    property alias readOnly: editor.readOnly
    property alias selectByMouse: editor.selectByMouse
    property alias selectedText: editor.selectedText
    property alias selectedTextColor: editor.selectedTextColor
    property alias selectionColor: editor.selectionColor
    property alias selectionStart: editor.selectionStart
    property alias selectionEnd: editor.selectionEnd
    property alias smooth: editor.smooth
    property alias text: editor.text
    property alias validator: editor.validator

    signal accepted()

    function copy()
    {
        editor.copy()
    }
    function cut()
    {
        editor.cut()
    }
    function paste()
    {
        editor.paste()
    }

    function deselect()
    {
        editor.deselect()
    }
    function isRightToLeft(start, end)
    {
        return editor.isRightToLeft(start, end)
    }
    function moveCursorSelection(position, mode)
    {
        if (mode == undefined)
            editor.moveCursorSelection(position)
        else
            editor.moveCursorSelection(position, mode)
    }
    function positionAt(x, position)
    {
        if (position == undefined)
            return editor.positionAt(x)
        else
            return editor.positionAt(x, position)
    }
    function positionToRectangle(pos)
    {
        return editor.positionToRectangle(pos)
    }
    function select(start, end)
    {
        editor.select(start, end)
    }
    function selectAll()
    {
        editor.selectAll()
    }
    function selectWord()
    {
        editor.selectWord()
    }

    // ensure focus propagation
    function forceActiveFocus()
    {
        editor.forceActiveFocus()
    }


    //internals
    QtObject {
        id: internal
        property url frameImage: Qt.resolvedUrl("artwork/TextFieldFrame.png")
        // array of borders in left, top, right, bottom order
        property variant frameBorders: [10, 5, 10, 5]
        property url clearImage: Qt.resolvedUrl("artwork/TextFieldClear.png")
        property color textColor: "#757373"
        property string fontFize: "medium"
        property real clearButtonSpacing: 4.0

        function rightMargin()
        {
            return (clearButton.visible) ? clearButton.width + clearButtonSpacing : 0
        }

        function showInputPanel()
        {
            if (control.customInputPanel != undefined) {
                // TODO implement once we have the SIP ready
            } else {
                editor.openSoftwareInputPanel()
            }
        }
        function hideInputPanel()
        {
            if (control.customInputPanel != undefined) {
                // TODO implement once we have the SIP ready
            } else {
                editor.closeSoftwareInputPanel()
            }
        }
    }

    BorderImage {
        id: frame
        anchors.fill: parent
        source: internal.frameImage
        smooth: true
        border {
            left: internal.frameBorders[0]
            top: internal.frameBorders[1]
            right: internal.frameBorders[2]
            bottom: internal.frameBorders[3]
        }

        //clear button
        Image {
            id: clearButton
            z: Number.MAX_VALUE
            anchors {
                right: parent.right
                rightMargin: 5
                verticalCenter: parent.verticalCenter
            }
            source: (control.hasClearButton) ? internal.clearImage : ""
            visible: control.hasClearButton && (control.alwaysShowClearButton || (control.activeFocus && (editor.text != "")))
            smooth: true
            fillMode: Image.PreserveAspectFit
            width: sourceSize.width

            MouseArea {
                enabled: control.hasClearButton
                anchors{
                    fill: parent
                    // need to enlarge the sensing area
                    margins: -5
                }
                onClicked:editor.text = ""
            }
        }

        // hint text
        TextCustom {
            id: hint
            color: Qt.lighter(internal.textColor, 1.3)
            font.italic: true
            verticalAlignment: Text.AlignVCenter
            fontSize: "medium"
            anchors {
                fill: parent
                leftMargin: internal.frameBorders[0]
                topMargin: internal.frameBorders[1]
                rightMargin: internal.frameBorders[2] + internal.rightMargin()
                bottomMargin: internal.frameBorders[3]
            }
            elide: Text.ElideRight
            visible: editor.text == ""
        }

        // text input
        TextInput {
            id: editor
            // FocusScope will forward focus to this component
            focus: true
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: internal.frameBorders[0]
                rightMargin: internal.frameBorders[2] + internal.rightMargin()
                verticalCenter: parent.verticalCenter
            }
            color: internal.textColor
            clip: true
            // TODO: font family to be defined
            font.pixelSize: FontUtils.sizeToPixels(internal.fontFize)

            // OSK/SIP handling
            activeFocusOnPress: false
            onActiveFocusChanged: {
                if (activeFocus) {
                    internal.showInputPanel()
                    if (clearContentOnEditingStart)
                        editor.text = ""
                } else
                    internal.hideInputPanel()
            }
            MouseArea {
                id: virtualKbdHandler
                anchors.fill: parent
                onClicked: if (!control.activeFocus) control.forceActiveFocus()
            }
        }
    }

    Component.onCompleted: editor.accepted.connect(control.accepted)
}
