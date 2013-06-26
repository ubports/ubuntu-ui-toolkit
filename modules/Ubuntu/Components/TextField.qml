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

/*!
    \qmltype TextField
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief The TextField element displays a single line of editable plain text.
    Input constraints can be set through validator or inputMask. Setting echoMode
    to an appropriate value enables TextField to be used as password input field.

    \b{This component is under heavy development.}

    \l {http://design.ubuntu.com/apps/building-blocks/text-field}{See also the Design Guidelines on the Text Field}.

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
                Button {
                    height: parent.height
                    width: height
                    iconSource: "caps-lock.png"
                    onClicked: doSomething()
                }
                Button {
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

StyledItem {
    id: control

    implicitWidth: units.gu(25)
    implicitHeight: units.gu(4)
    /*!
      The property presents whether the TextField is highlighted or not. By
      default the TextField gets highlighted when gets the focus, so can accept
      text input. This property allows to control the highlight separately from
      the focused behavior.
      */
    property bool highlighted: focus

    /*!
      \preliminary
      Text that appears when there is no focus and no content in the component.

      \qmlproperty string placeholderText
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
      The property overrides the default popover of the TextField. When set, the TextField
      will open the given popover instead of the defaul tone defined. The popover can either
      be a component or a URL to be loaded.
      */
    property var popover

    /*!
      \preliminary
      Overlaid component that can be set for the fore side of the TextField,
      e.g.showing a magnifier to implement search functionality.

      \qmlproperty list<Object> primaryItem
    */
    property alias primaryItem: leftPane.data

    /*!
      \preliminary
      Overlaid component that can be set for the rear side of the TextField,
      e.g.showing a CAPS LOCK or NUM LOCK indication. The overlaid components
      will be placed right after the clear button.

      \qmlproperty list<Object> secondaryItem
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

      \qmlproperty bool acceptableInput
    */
    property alias acceptableInput: editor.acceptableInput

    /*!
      \preliminary
      The position of the cursor in the TextField.

      \qmlproperty int cursorPosition
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

        \qmlproperty enumeration echoMode
    */
    property alias echoMode: editor.echoMode

    /*!
      \preliminary
      Font used in the TextField.

      \qmlproperty font font
    */
    property alias font: editor.font

    /*!
      \preliminary
      Allows you to set an input mask on the TextField, restricting the  text
      inputs. See QLineEdit::inputMask for further details, as the exact same mask strings
      are used by TextField.

      \qmlproperty string inputMask
    */
    property alias inputMask: editor.inputMask

    /*!
      \preliminary
      This property holds whether the TextInput has partial text input from an input method.

      While it is composing an input method may rely on mouse or key events from the
      TextField to edit or commit the partial text. This property can be used to determine
      when to disable events handlers that may interfere with the correct operation
      of an input method.

      \qmlproperty bool inputMethodComposing
    */
    property alias inputMethodComposing: editor.inputMethodComposing

    /*!
      \preliminary
      Provides hints to the input method about the expected content of the text input and how it
      should operate.

      The value is a bit-wise combination of flags, or Qt.ImhNone if no hints are set.

      Flags that alter behaviour are:

      \list
      \li Qt.ImhHiddenText - Characters should be hidden, as is typically used when entering passwords.
              This is automatically set when setting echoMode to \c TextInput.Password.
      \li Qt.ImhSensitiveData - Typed text should not be stored by the active input method
              in any persistent storage like predictive user dictionary.
      \li Qt.ImhNoAutoUppercase - The input method should not try to automatically switch to upper case
              when a sentence ends.
      \li Qt.ImhPreferNumbers - Numbers are preferred (but not required).
      \li Qt.ImhPreferUppercase - Upper case letters are preferred (but not required).
      \li Qt.ImhPreferLowercase - Lower case letters are preferred (but not required).
      \li Qt.ImhNoPredictiveText - Do not use predictive text (i.e. dictionary lookup) while typing.

      \li Qt.ImhDate - The text editor functions as a date field.
      \li Qt.ImhTime - The text editor functions as a time field.
      \endlist

      Flags that restrict input (exclusive flags) are:

      \list
      \li Qt.ImhDigitsOnly - Only digits are allowed.
      \li Qt.ImhFormattedNumbersOnly - Only number input is allowed. This includes decimal point and minus sign.
      \li Qt.ImhUppercaseOnly - Only upper case letter input is allowed.
      \li Qt.ImhLowercaseOnly - Only lower case letter input is allowed.
      \li Qt.ImhDialableCharactersOnly - Only characters suitable for phone dialing are allowed.
      \li Qt.ImhEmailCharactersOnly - Only characters suitable for email addresses are allowed.
      \li Qt.ImhUrlCharactersOnly - Only characters suitable for URLs are allowed.
      \endlist

      Masks:

      \list
      \li Qt.ImhExclusiveInputMask - This mask yields nonzero if any of the exclusive flags are used.
      \endlist

      \qmlproperty enumeration inputMethodHints
    */
    property alias inputMethodHints: editor.inputMethodHints

    /*!
      \preliminary
      The maximum permitted length of the text in the TextField.

      If the text is too long, it is truncated at the limit.

      By default, this property contains a value of 32767.

      \qmlproperty int maximumLength
    */
    property alias maximumLength: editor.maximumLength

    /*!
      \preliminary
      Sets whether user input can modify the contents of the TextField.

      If readOnly is set to true, then user input will not affect the
      text property. Any bindings or attempts to set the text property
      will still work.

      \qmlproperty bool readOnly
    */
    property alias readOnly: editor.readOnly

    /*!
      \preliminary
      This read-only property provides the text currently selected in the text input.

      \qmlproperty string selectedText
    */
    property alias selectedText: editor.selectedText

    /*!
      \preliminary
      The cursor position before the first character in the current selection.

      This property is read-only. To change the selection, use select(start,end),
      selectAll(), or selectWord().

      \qmlproperty int selectionStart
    */
    property alias selectionStart: editor.selectionStart

    /*!
      \preliminary
      The cursor position after the last character in the current selection.

      This property is read-only. To change the selection, use select(start,end),
      selectAll(), or selectWord().

      \qmlproperty int selectionEnd
    */
    property alias selectionEnd: editor.selectionEnd

    /*!
      \preliminary
      The text in the TextField.

      \qmlproperty string text
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

      \qmlproperty Validator validator
    */
    property alias validator: editor.validator

    /*!
      \preliminary
      Sets the horizontal alignment of the text within the item's width and height.
      By default, the text alignment follows the natural alignment of the text, for
      example text that is read from left to right will be aligned to the left.

      TextInput does not have vertical alignment, as the natural height is exactly
      the height of the single line of text. If you set the height manually to something
      larger, TextInput will always be top aligned vertically. You can use anchors to
      align it however you want within another item.

      The valid values for horizontalAlignment are TextInput.AlignLeft,
      TextInput.AlignRight and TextInput.AlignHCenter.

      \qmlproperty enumeration horizontalAlignment
    */
    property alias horizontalAlignment: editor.horizontalAlignment

    /*!
      \internal
      FIXME: property added for styling purposes
      */
    property alias passwordCharacter: editor.passwordCharacter

    /*!
      \internal
      FIXME: property added for styling purposes
      */
    property alias color: editor.color

    /*!
      \internal
      FIXME: property added for styling purposes
      */
    property alias selectionColor: editor.selectionColor

    /*!
      \internal
      FIXME: property added for styling purposes
      */
    property alias selectedTextColor: editor.selectedTextColor

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
      Places the clipboard or the data given as parameter into the text input.
      The selected text will be replaces with the data.
    */
    function paste(data)
    {
        if ((data !== undefined) && (typeof data === "string") && !editor.readOnly) {
            var selTxt = editor.selectedText;
            var txt = editor.text;
            var pos = (selTxt !== "") ? txt.indexOf(selTxt) : editor.cursorPosition
            if (selTxt !== "") {
                editor.text = txt.substring(0, pos) + data + txt.substr(pos + selTxt.length);
            } else {
                editor.text = txt.substring(0, pos) + data + txt.substr(pos);
            }
            editor.cursorPosition = pos + data.length;
        } else
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
        internal.activateEditor();
    }

    // internals

    /*! internal */
    onVisibleChanged: {
        if (!visible)
            control.focus = false;
    }

    // grab clicks from the area between the frame and the input
    MouseArea {
        anchors.fill: parent
        // us it only when there is space between the frame and input
        enabled: internal.spacing > 0
        onClicked: internal.activateEditor()
    }

    Text { id: fontHolder }
    SystemPalette { id: systemColors }

    //internals
    /*! \internal */
    property alias __internal: internal
    QtObject {
        id: internal
        // array of borders in left, top, right, bottom order
        property bool textChanged: false
        property real spacing: control.delegate.overlaySpacing
        property real lineSpacing: units.dp(3)
        property real lineSize: editor.font.pixelSize + lineSpacing
        //selection properties
        property bool selectionMode: false
        property int selectionStart: 0
        property int selectionEnd: 0

        signal popupTriggered()

        function activateEditor()
        {
            if (!control.activeFocus)
                editor.forceActiveFocus();
            else
                showInputPanel();
        }

        function showInputPanel()
        {
            if (control.customSoftwareInputPanel != undefined) {
                // TODO implement once we have the SIP ready
            } else {
                if (!Qt.inputMethod.visible)
                    Qt.inputMethod.show();
            }
            textChanged = false;
        }
        function hideInputPanel()
        {
            if (control.customSoftwareInputPanel != undefined) {
                // TODO implement once we have the SIP ready
            } else {
                Qt.inputMethod.hide();
            }
            // emit accepted signal if changed
            if (textChanged)
                control.accepted();
        }
        // reset selection
        function resetEditorSelection(mouseX)
        {
            editor.cursorPosition = selectionStart = selectionEnd = editor.positionAt(mouseX);
        }

        // positions the cursor depending on whether there is a selection active or not
        function positionCursor(x) {
            var cursorPos = control.positionAt(x);
            if (internal.selectionEnd == internal.selectionStart)
                control.cursorPosition = control.positionAt(x);
            else
                control.select(internal.selectionStart, internal.selectionEnd);
        }
    }

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
        TextCursor {
            //FIXME: connect to root object once we have all TextInput properties exposed
            editorItem: editor
            height: internal.lineSize
            popover: control.popover
            visible: editor.cursorVisible

            Component.onCompleted: internal.popupTriggered.connect(openPopover)
        }
    }

    AbstractButton {
        id: clearButton
        property url iconSource: control.delegate.iconSource
        anchors {
            top: parent.top
            right: rightPane.left
            bottom: parent.bottom
            margins: internal.spacing
        }
        width: visible ? height : 0
        visible: control.hasClearButton &&
                    (control.activeFocus && ((editor.text != "") || editor.inputMethodComposing))

        Image {
            //anchors.fill: parent
            anchors.verticalCenter: parent.verticalCenter
            width: units.gu(3)
            height: width
            smooth: true
            source: control.hasClearButton ? clearButton.iconSource : ""
            onSourceChanged: print(source)
        }

        onClicked: editor.text = ""
    }

    // hint text
    Label {
        id: hint
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: editor.horizontalAlignment
        anchors {
            left: leftPane.right
            right: clearButton.left
            top: parent.top
            bottom: parent.bottom
            margins: internal.spacing
        }
        // hint is shown till user types something in the field
        visible: (editor.text == "") && !editor.inputMethodComposing
        color: Qt.rgba(0.5, 0.5, 0.5, 0.5)
        fontSize: "medium"
        elide: Text.ElideRight
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
        // get the control's style
        clip: true
        onTextChanged: internal.textChanged = true
        cursorDelegate: cursor
        color: control.delegate.color
        selectedTextColor: "#F3F3E7"
        selectionColor: "#19B6EE"
        font.pixelSize: FontUtils.sizeToPixels("medium")
        passwordCharacter: "\u2022"
        // forward keys to the root element so it can be captured outside of it
        Keys.forwardTo: [control]

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
                    internal.activateEditor();
                    // set cursor position if no selection was previously set
                    internal.positionCursor(mouse.x)
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

            onPressAndHold: {
                internal.activateEditor();
                internal.positionCursor(mouse.x);
                internal.popupTriggered();
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
                internal.activateEditor();
                if (internal.selectionEnd == internal.selectionStart) {
                    internal.resetEditorSelection(mouse.x);
                    internal.selectionMode = true;
                }
            }
            onReleased: {
                if (!containsMouse)
                    internal.selectionMode = false;
            }
        }
    }

    Component.onCompleted: {
        editor.accepted.connect(control.accepted);
        cursorPosition = 0;
    }

    style: Theme.createStyleComponent("TextAreaDelegate.qml", control)
}
