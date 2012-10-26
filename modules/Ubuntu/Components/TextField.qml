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
import "fontUtils.js" as FontUtils

/*!
    \qmltype TextField
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief The TextField element displays a single line of editable plain text.
    Input constraints can be set through validator or inputMask. Setting echoMode
    to an appropriate value enables TextField to be used as password input field.

    \b{This component is under heavy development.}

    Example:
    \qml
    Item {
        TextField {
            placeholderText: "hint text"
        }

        TextField {
            placeholderText: "without clear sign"
            hasClearButton: false
        }

        TextField {
            placeholderText: "password"
            echoMode: TextInput.Password
        }

        TextField {
            placeholderText: "overlaid in front"
            primaryItem: Image {
                height: parent.height
                width: height
                source: "magnifier.png"
            }
            secondaryItem: Row {
                ButtonWithForeground {
                    height: parent.height
                    width: height
                    iconSource: "caps-lock.png"
                    onClicked: doSomething()
                }
                ButtonWithForeground {
                    height: parent.height
                    width: height
                    iconSource: "num-lock.png"
                    onClicked: doSomething()
                }
            }
        }
    }
    \endqml
*/
FocusScope {
    id: control

    implicitWidth: 200
    implicitHeight: 25
    /*!
      \preliminary
      Text that appears when there is no focus and no content in the component.
    */
    property alias placeholderText: hint.text

    /*!
      \preliminary
      Specifies whether the control has a clear button or not.
    */
    property bool hasClearButton: true

    /*!
      \preliminary
      Component to be shown and used instead of the default On Screen Keyboard.
    */
    property Component customSoftwareInputPanel

    /*!
      \preliminary
      Overlaid component that can be set for the fore side of the TextField,
      e.g.showing a magnifier to implement search functionality.
    */
    property alias primaryItem: leftPane.data

    /*!
      \preliminary
      Overlaid component that can be set for the rear side of the TextField,
      e.g.showing a CAPS LOCK or NUM LOCK indication. The overlaid components
      will be placed right after the clear button.
    */
    property alias secondaryItem: rightPane.data

    /*!
      \preliminary
      Allows highlighting errors in the TextField.
    */
    property bool errorHighlight: !acceptableInput

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
      The position of the cursor in the TextField.
    */
    property alias cursorPosition: editor.cursorPosition

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
      Font used in the TextField.
    */
    property alias font: editor.font

    /*!
      \preliminary
      Allows you to set an input mask on the TextField, restricting the  text
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
      Sets whether user input can modify the contents of the TextField.

      If readOnly is set to true, then user input will not affect the
      text property. Any bindings or attempts to set the text property
      will still work.
    */
    property alias readOnly: editor.readOnly

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
      import QtQuick 2.0
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
        editor.copy();
    }

    /*!
      \preliminary
      Moves the currently selected text to the system clipboard.
    */
    function cut()
    {
        editor.cut();
    }

    /*!
      \preliminary
      Replaces the currently selected text by the contents of the system clipboard.
    */
    function paste()
    {
        editor.paste();
    }

    /*!
      \preliminary
      Removes active text selection.
    */
    function deselect()
    {
        editor.deselect();
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
            return editor.positionAt(x);
        else
            return editor.positionAt(x, position);
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
        return editor.positionToRectangle(pos);
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
        editor.select(start, end);
    }

    /*!
      \preliminary
      Causes all text to be selected.
    */
    function selectAll()
    {
        editor.selectAll();
    }

    /*!
      \preliminary
      Causes the word closest to the current cursor position to be selected.
    */
    function selectWord()
    {
        editor.selectWord();
    }

    /*!
      \internal
       Ensure focus propagation
    */
    function forceActiveFocus()
    {
        editor.forceActiveFocus();
    }

    //internals
    QtObject {
        id: internal
        property url frameImage: selectFrameSource()
        // array of borders in left, top, right, bottom order
        property url clearImage: Qt.resolvedUrl("artwork/TextFieldClear.png")
        property color textColor: (editor.enabled) ? "#757373" : "darkgray"
        property color hintColor: "#B6B6B6"
        property string fontSize: "small"
        property real clearButtonSpacing: 3.5
        property bool textChanged: false
        property real spacing: 5
        //selection properties
        property bool selectionMode: false
        property int selectionStart: 0
        property int selectionEnd: 0

        function selectFrameSource()
        {
            if (control.errorHighlight && !editor.acceptableInput) {
                if (editor.activeFocus && editor.enabled)
                    return Qt.resolvedUrl("artwork/TextFieldFrameError.sci");
                return Qt.resolvedUrl("artwork/TextFieldFrameIdleError.sci");
            }
            if (editor.activeFocus && editor.enabled)
                return Qt.resolvedUrl("artwork/TextFieldFrame.sci");
            return Qt.resolvedUrl("artwork/TextFieldFrameIdle.sci");
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
        // reset selection
        function resetEditorSelection(mouseX)
        {
            editor.cursorPosition = selectionStart = selectionEnd = editor.positionAt(mouseX);
        }
    }

    // frame
    BorderImage {
        id: frame
        anchors.fill: parent
        source: internal.frameImage
        smooth: true

        //left pane
        Item {
            id: leftPane
            anchors {
                left: parent.left
                leftMargin: internal.spacing
                verticalCenter: parent.verticalCenter
            }
            // the left pane width depends on its children width
            height: parent.height
            width: childrenRect.width
            onChildrenChanged: {
                // reparenting
                for (var i = 0; i < children.length; i++) {
                    children[i].parent = leftPane;
                    children[i].anchors.verticalCenter = verticalCenter;
                }
            }
        }

        // right pane
        Item {
            id: rightPane
            anchors {
                right: parent.right
                rightMargin: internal.spacing
                verticalCenter: parent.verticalCenter
            }
            // the right pane width depends on its children width
            height: parent.height
            width: childrenRect.width
            onChildrenChanged: {
                // reparenting
                for (var i = 0; i < children.length; i++) {
                    children[i].parent = rightPane;
                    children[i].anchors.verticalCenter = verticalCenter;
                }
            }
        }

        // cursor
        Component {
            id: cursor
            Rectangle {
                property bool timerShowCursor: true

                id: customCursor
                color: Qt.rgba(0.4, 0.4, 0.4, 1.0)
                height: parent.height - (parent.topMargin + parent.bottomMargin)
                width: 1
                visible: (customCursor.parent.forceCursorVisible || parent.activeFocus) && timerShowCursor
                Timer {
                    interval: 800
                    running: (customCursor.parent.forceCursorVisible || customCursor.parent.activeFocus)
                    repeat: true
                    onTriggered: {
                        interval = (interval == 800) ? 400 : 800;
                        customCursor.timerShowCursor = !customCursor.timerShowCursor;
                    }
                }
            }
        }

        ButtonWithForeground {
            id: clearButton
            anchors {
                right: rightPane.left
                rightMargin: internal.spacing
                verticalCenter: parent.verticalCenter
            }
            iconSource: (control.hasClearButton) ? internal.clearImage : ""
            width: visible ? 19 : 0 // TODO: no way to set the image size :(
            visible: control.hasClearButton &&
                        (control.activeFocus && ((editor.text != "") || editor.inputMethodComposing))
            onClicked: editor.text = ""
        }

        // hint text
        TextCustom {
            id: hint
            color: internal.hintColor
            font.italic: true
            verticalAlignment: Text.AlignVCenter
            fontSize: "small"
            anchors {
                left: leftPane.right
                right: clearButton.left
                top: parent.top
                bottom: parent.bottom
                margins: internal.spacing
            }
            elide: Text.ElideRight
            font.pixelSize: editor.font.pixelSize
            // hint is shown till user types something in the field
            visible: (editor.text == "") && !editor.inputMethodComposing
        }

        // text input
        TextInput {
            id: editor
            // FocusScope will forward focus to this component
            focus: true
            anchors {
                left: leftPane.right
                right: clearButton.left
                margins: internal.spacing
                verticalCenter: parent.verticalCenter
            }
            color: internal.textColor
            clip: true
            passwordCharacter: "\u2022"
            font.pixelSize: FontUtils.sizeToPixels(internal.fontSize)
            onTextChanged: internal.textChanged
            cursorDelegate: cursor

            // virtual keyboard/software input panel handling
            activeFocusOnPress: false
            onActiveFocusChanged: {
                if (activeFocus) {
                    internal.showInputPanel();
                    internal.textChanged = false;
                } else {
                    internal.hideInputPanel();
                    // emit accepted signal if changed
                    if (internal.textChanged)
                        control.accepted();
                }
            }

            // handle virtual keyboard and cursor positioning, as the MouseArea overrides
            // those functionalities of the TextInput
            MouseArea {
                id: virtualKbdHandler
                anchors.fill: parent
                hoverEnabled: true
                preventStealing: true

                onClicked: {
                    // activate control
                    if (!control.activeFocus) {
                        control.forceActiveFocus();
                        // set cursor position if no selection was previously set
                        if (internal.selectionEnd == internal.selectionStart)
                            editor.cursorPosition = editor.positionAt(mouse.x);
                        else
                            editor.select(internal.selectionStart, internal.selectionEnd);
                    } else if (!internal.selectionMode){
                        // reset selection and move cursor unde mouse click
                        internal.resetEditorSelection(mouse.x);
                    } else if (internal.selectionMode) {
                        // reset selection mode (onReleased is triggered prior to onClicked
                        // and resetting selection mode there would cause to enter in the\
                        // previous if-clause
                        internal.selectionMode = false;
                    }
                }

                onDoubleClicked: {
                    // select word under doubletap
                    if (!control.activeFocus)
                        return;
                    editor.selectWord();
                    // update selection boundaries, except cursorPosition
                    internal.selectionEnd = editor.selectionEnd;
                    internal.selectionStart = editor.selectionStart;
                    internal.selectionMode = false;
                }
                onPressed: {
                    // don't do anything while the control is inactive
                    if (!control.activeFocus || (pressedButtons != Qt.LeftButton))
                        return;
                    if (internal.selectionEnd == internal.selectionStart) {
                        internal.resetEditorSelection(mouse.x);
                        internal.selectionMode = true;
                    }
                }
                onReleased: {
                    if (!containsMouse)
                        internal.selectionMode = false;
                }

                onPositionChanged: {
                    if (!editor.activeFocus || !internal.selectionMode)
                        return;
                    // update selectionEnd
                    internal.selectionEnd = editor.positionAt(mouse.x);
                    editor.select(internal.selectionStart, internal.selectionEnd);
                }
            }
        }
    }
    Component.onCompleted: {
        editor.accepted.connect(control.accepted);
        cursorPosition = 0;
    }
}
