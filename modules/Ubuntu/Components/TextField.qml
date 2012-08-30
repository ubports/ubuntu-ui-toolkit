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
    \brief The TextField element displays a single line of editable plain text.
    Input constraints can be set through validator or inputMask. Setting echoMode
    to an apropriate value enables TextInput to be used as password input field.

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
            clearsOnBeginEditing: true
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
      Text that appear if there is no focus and no content on the component.
    */
    property alias placeholderText: hint.text

    /*!
      \preliminary
      Specifies whether the control has a clear button on the right corner or not.
    */
    property bool hasClearButton: false

    /*!
      \preliminary
      This property controls the visibility of the clear button. If set, the button
      will be shown no matter of its content and state. If reset, the button will be
      shown only if the control is focused and there is some text in it.

      Default is true.
    */
    property bool alwaysShowClearButton: true

    /*!
      \preliminary
      Specifies whether to clear the editor's content when focused.

      Default is false.
    */
    property bool clearsOnBeginEditing: false

    /*!
      \preliminary
      Component to be shown and used instead of the default On Screen Keyboard.
    */
    property Component customInputPanel

    // aliased properties from TextInput
    /*!
      \preliminary
      This property is always true unless a validator or input mask has been set.
      If a validator or input mask has been set, this property will only be true
      if the current text is acceptable to the validator or input mask as a final
      string (not as an intermediate string).
    */
    property alias acceptableInput: editor.acceptableInput

    /*!
      \preliminary
      Returns true if the TextField is writable and the content of the clipboard
      is suitable for pasting into the TextField.
    */
    property alias canPaste: editor.canPaste

    /*!
      \preliminary
      The position of the cursor in the TextField.
    */
    property alias cursorPosition: editor.cursorPosition

    /*!
      \preliminary
      Set to true when the TextField shows a cursor.
    */
    property alias cursorVisible: editor.cursorVisible

    /*!
      \preliminary
      This is the text displayed in the TextField.

      If echoMode is set to TextInput::Normal, this holds the same value as the
      TextInput::text property. Otherwise, this property holds the text visible to
      the user, while the text property holds the actual entered text.
    */
    property alias displayText: editor.displayText

    /*!
      \preliminary
      Specifies how the text should be displayed in the TextField.

      - TextInput.Normal - Displays the text as it is. (Default)
      - TextInput.Password - Displays asterixes instead of characters.
      - TextInput.NoEcho - Displays nothing.
      - TextInput.PasswordEchoOnEdit - Displays characters as they are entered while
        editing, otherwise displays asterisks.
    */
    property alias echoMode: editor.echoMode

    /*!
      \preliminary
      Sets the horizontal alignment of the text within the TextField item's width and
      height. By default, the text alignment follows the natural alignment of the text,
      for example text that is read from left to right will be aligned to the left.
    */
    property alias horizontalAlignment: editor.horizontalAlignment

    /*!
      \preliminary
      Allows you to set an input mask on the TextField, restricting the allowable text
      inputs. See QLineEdit::inputMask for further details, as the exact same mask strings
      are used by TextField.
    */
    property alias inputMask: editor.inputMask

    /*!
      \preliminary
      This property holds whether the TextInput has partial text input from an input method.

      While it is composing an input method may rely on mouse or key events from the
      TextField to edit or commit the partial text. This property can be used to determine
      when to disable events handlers that may interfere with the correct operation
      of an input method.
    */
    property alias inputMethodComposing: editor.inputMethodComposing

    /*!
      \preliminary
      The maximum permitted length of the text in the TextField.

      If the text is too long, it is truncated at the limit.

      By default, this property contains a value of 32767.
    */
    property alias maximumLength: editor.maximumLength

    /*!
      \preliminary
      Specifies how text should be selected using a mouse.

      - TextInput.SelectCharacters - The selection is updated with individual
        characters. (Default)
      - TextInput.SelectWords - The selection is updated with whole words.

      This property only applies when selectByMouse is true.
    */
    property alias mouseSelectionMode: editor.mouseSelectionMode

    /*!
      \preliminary
      This is the character displayed when echoMode is set to Password or
      PasswordEchoOnEdit. By default it is an asterisk.

      If this property is set to a string with more than one character, the
      first character is used. If the string is empty, the value is ignored
      and the property is not set.
    */
    property alias passwordCharacter: editor.passwordCharacter

    /*!
      \preliminary
      Sets whether user input can modify the contents of the TextField.

      If readOnly is set to true, then user input will not affect the
      text property. Any bindings or attempts to set the text property
      will still work.
    */
    property alias readOnly: editor.readOnly

    /*!
      \preliminary
      Defaults to false.

      If true, the user can use the mouse to select text in some platform-specific
      way. Note that for some platforms this may not be an appropriate interaction
      (eg. may conflict with how the text needs to behave inside a Flickable).
    */
    property alias selectByMouse: editor.selectByMouse

    /*!
      \preliminary
      This read-only property provides the text currently selected in the text input.
    */
    property alias selectedText: editor.selectedText

    /*!
      \preliminary
      The cursor position before the first character in the current selection.

      This property is read-only. To change the selection, use select(start,end),
      selectAll(), or selectWord().
    */
    property alias selectionStart: editor.selectionStart

    /*!
      \preliminary
      The cursor position after the last character in the current selection.

      This property is read-only. To change the selection, use select(start,end),
      selectAll(), or selectWord().
    */
    property alias selectionEnd: editor.selectionEnd

    /*!
      \preliminary
      This property holds whether the text is smoothly scaled or transformed.

      Smooth filtering gives better visual quality, but is slower. If the item
      is displayed at its natural size, this property has no visual or performance
      effect.

      Note: Generally scaling artifacts are only visible if the item is stationary
      on the screen. A common pattern when animating an item is to disable smooth
      filtering at the beginning of the animation and reenable it at the conclusion.
    */
    property alias smooth: editor.smooth

    /*!
      \preliminary
      The text in the TextField.
    */
    property alias text: editor.text

    /*!
      \preliminary
      Allows you to set a validator on the TextInput. When a validator is set the
      TextField will only accept input which leaves the text property in an acceptable
      or intermediate state. The accepted signal will only be sent if the text is in
      an acceptable state when enter is pressed.

      Currently supported validators are IntValidator, DoubleValidator and RegExpValidator.
      An example of using validators is shown below, which allows input of integers
      between 11 and 31 into the text input:

      \qml
      import QtQuick 1.0
      TextInput{
          validator: IntValidator{bottom: 11; top: 31;}
          focus: true
      }
      \endqml
    */
    property alias validator: editor.validator


    /*!
      \preliminary
      This handler is called when the Return or Enter key is pressed. Note that if
      there is a validator or inputMask set on the text input, the handler will only
      be emitted if the input is in an acceptable state.
    */
    signal accepted()


    /*!
      \preliminary
      Copies the currently selected text to the system clipboard.
    */
    function copy()
    {
        editor.copy()
    }

    /*!
      \preliminary
      Moves the currently selected text to the system clipboard.
    */
    function cut()
    {
        editor.cut()
    }

    /*!
      \preliminary
      Replaces the currently selected text by the contents of the system clipboard.
    */
    function paste()
    {
        editor.paste()
    }

    /*!
      \preliminary
      Removes active text selection.
    */
    function deselect()
    {
        editor.deselect()
    }

    /*!
      \preliminary
      Returns true if the natural reading direction of the editor text found between
      positions start and end is right to left.
    */
    function isRightToLeft(start, end)
    {
        return editor.isRightToLeft(start, end)
    }

    /*!
      \preliminary
      Moves the cursor to position and updates the selection according to the optional
      mode parameter. (To only move the cursor, set the cursorPosition property.)
    */
    function moveCursorSelection(position, mode)
    {
        if (mode == undefined)
            editor.moveCursorSelection(position)
        else
            editor.moveCursorSelection(position, mode)
    }

    /*!
      \preliminary
      This function returns the character position at x pixels from the left of
      the TextField. Position 0 is before the first character, position 1 is after
      the first character but before the second, and so on until position text.length,
      which is after all characters.

      This means that for all x values before the first character this function returns 0,
      and for all x values after the last character this function returns text.length.

      The cursor position type specifies how the cursor position should be resolved.

      - TextInput.CursorBetweenCharacters - Returns the position between characters
        that is nearest x.
      - TextInput.CursorOnCharacter - Returns the position before the character that
        is nearest x.
    */
    function positionAt(x, position)
    {
        if (position == undefined)
            return editor.positionAt(x)
        else
            return editor.positionAt(x, position)
    }

    /*!
      \preliminary
      This function takes a character position and returns the rectangle that the
      cursor would occupy, if it was placed at that character position.

      This is similar to setting the cursorPosition, and then querying the cursor
      rectangle, but the cursorPosition is not changed.
    */
    function positionToRectangle(pos)
    {
        return editor.positionToRectangle(pos)
    }

    /*!
      \preliminary
      Causes the text from start to end to be selected.

      If either start or end is out of range, the selection is not changed.

      After calling this, selectionStart will become the lesser and selectionEnd
      will become the greater (regardless of the order passed to this method).
    */
    function select(start, end)
    {
        editor.select(start, end)
    }

    /*!
      \preliminary
      Causes all text to be selected.
    */
    function selectAll()
    {
        editor.selectAll()
    }

    /*!
      \preliminary
      Causes the word closest to the current cursor position to be selected.
    */
    function selectWord()
    {
        editor.selectWord()
    }

    /*!
      \internal
       Ensure focus propagation
    */
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
        property bool textChanged: false

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

    // frame
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
            visible: control.hasClearButton &&
                        (control.alwaysShowClearButton ||
                            (control.activeFocus && ((editor.text != "") || editor.inputMethodComposing)))
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
            visible: (editor.text == "") && !editor.activeFocus
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

            onTextChanged: internal.textChanged = true

            // OSK/SIP handling
            activeFocusOnPress: false
            onActiveFocusChanged: {
                if (activeFocus) {
                    internal.showInputPanel()
                    internal.textChanged = false
                    if (clearsOnBeginEditing)
                        editor.text = ""
                } else {
                    internal.hideInputPanel()
                    // emit accepted signal is changed
                    if (internal.textChanged)
                        control.accepted()
                }
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
