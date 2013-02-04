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
    to the font's pixel size. The maximumLineCount specifies how much the editor should be
    expanded. If this value is set to 0, the area will always expand vertically to fit the
    content. When autoExpand is set, the contentHeight property value is ignored, and the
    expansion only happens vertically.

    \qml
    TextArea {
        width: units.gu(20)
        height: units.gu(12)
        contentWidth: units.gu(30)
        autoExpand: true
        maximumLineCount: 0
    }
    \endqml

    TextArea comes with 30 grid-units implicit width and one line height on auto-expanding
    mode and 4 lines on fixed-mode. The line size is calculated from the font size and the
    ovarlay and frame spacing specified in the style.

    Scrolling the editing area can happen when the size is fixed or in auto-expand mode when
    the content size is bigger than the visible area. The scrolling is realized by swipe
    gestures, or by navigating the cursor.

    The item enters in selection mode when the user performs a long tap (or long mouse press)
    or a double tap/press on the text area. The mode is visualized by two selection cursors
    (pins) which can be used to select the desired text. The text can also be selected by
    moving the finger/mouse towards the desired area right after entering in selection mode.
    The way the text is selected is driven by the mouseSelectionMode value, which is either
    character or word. The editor leaves the selection mode by pressing/tapping again on it
    or by losing focus.

    The default styling uses \b textarea selector.

    \b{This component is under heavy development.}
  */

FocusScope {
    id: control
    implicitWidth: units.gu(30)
    implicitHeight: (autoExpand) ? internal.frameLinesHeight(1) : internal.frameLinesHeight(4)

    // new properties
    /*!
      Text that appears when there is no focus and no content in the component
      (hint text). The hint style can be customized by defining a style selector which
      styles a label. For example the default styling defines the hint styling using
      \b{.textarea .label} selector.

      \code
      .textarea .label {
           italic: true;
           color: "#B6B6B6";
           fontSize: "small";
           elide: Text.ElideRight;
           wrapMode: Text.WordWrap;
      }
      \endcode
      */
    property alias placeholderText: hint.text

    /*!
      This property contains the text that is displayed on the screen. May differ
      from the text property value when TextEdit.RichText format is selected.
      */
    readonly property alias displayText: internal.displayText

    /*!
      The property drives whether text selection should happen with the mouse or
      not. The default value is true.
      */
    property bool selectByMouse: true

    /*!
      This property specifies whether the text area expands following the entered
      text or not. The default value is false.
      */
    property bool autoExpand: false

    /*!
      The property holds the maximum amount of lines to expand when autoExpand is
      enabled. The value of 0 does not put any upper limit and the control will
      expand forever.

      The default value is 5 lines.
      */
    property int maximumLineCount: 5

    // altered TextEdit properties
    /*!
      The property folds the width of the text editing content. This can be equal or
      bigger than the frame width.
      */
    property alias contentWidth: editor.width

    /*!
      The property folds the height of the text editing content. This can be equal or
      bigger than the frame height.
      */
    property alias contentHeight: editor.height

    // forwarded properties
    /*!
      Whether the TextArea should gain active focus on a mouse press. By default this
      is set to true.
      */
    property bool activeFocusOnPress: true

    /*!
      This property specifies a base URL which is used to resolve relative URLs within
      the text. The default value is the url of the QML file instantiating the TextArea
      item.
      */
    property alias baseUrl: editor.baseUrl

    /*!
      Returns true if the TextArea is writable and the content of the clipboard is
      suitable for pasting into the TextArea.
      */
    property alias canPaste: editor.canPaste

    /*!
      Returns true if the TextArea is writable and there are undone operations that
      can be redone.
      */
    property alias canRedo: editor.canRedo

    /*!
      Returns true if the TextArea is writable and there are previous operations
      that can be undone.
      */
    property alias canUndo: editor.canUndo

    /*!
      The text color.
      */
    property alias color: editor.color

    /*!
      The delegate for the cursor in the TextArea.

      If you set a cursorDelegate for a TextArea, this delegate will be used for
      drawing the cursor instead of the standard cursor. An instance of the delegate
      will be created and managed by the text edit when a cursor is needed, and
      the x and y properties of delegate instance will be set so as to be one pixel
      before the top left of the current character.

      Note that the root item of the delegate component must be a QQuickItem or
      QQuickItem derived item.
      */
    property alias cursorDelegate: editor.cursorDelegate

    /*!
      The position of the cursor in the TextArea.
      */
    property alias cursorPosition: editor.cursorPosition

    /*!
      The rectangle where the standard text cursor is rendered within the text
      edit. Read-only.

      The position and height of a custom cursorDelegate are updated to follow
      the cursorRectangle automatically when it changes. The width of the delegate
      is unaffected by changes in the cursor rectangle.
      */
    property alias cursorRectangle: editor.cursorRectangle

    /*!
      If true the text edit shows a cursor.

      This property is set and unset when the text edit gets active focus, but it
      can also be set directly (useful, for example, if a KeyProxy might forward
      keys to it).
      */
    property alias cursorVisible: editor.cursorVisible

    /*!
      Presents the effective horizontal alignment that can be different from the one
      specified at horizontalAlignment due to layout mirroring.
      */
    property alias effectiveHorizontalAlignment: editor.effectiveHorizontalAlignment

    /*!
      The property holds the font used by the editing.
      */
    property alias font: editor.font

    /*!
      Sets the horizontal alignment of the text within the TextAre item's width
      and height. By default, the text alignment follows the natural alignment
      of the text, for example text that is read from left to right will be
      aligned to the left.

      Valid values for effectiveHorizontalAlignment are:
        \list
        \li TextEdit.AlignLeft (default)
        \li TextEdit.AlignRight
        \li TextEdit.AlignHCenter
        \li TextEdit.AlignJustify
        \endlist
      */
    property alias horizontalAlignment: editor.horizontalAlignment

    /*!
      This property holds whether the TextArea has partial text input from an
      input method.

      While it is composing an input method may rely on mouse or key events
      from the TextArea to edit or commit the partial text. This property can
      be used to determine when to disable events handlers that may interfere
      with the correct operation of an input method.
      */
    property alias inputMethodComposing: editor.inputMethodComposing

    /*!
    Provides hints to the input method about the expected content of the text
    edit and how it should operate.

    The value is a bit-wise combination of flags or Qt.ImhNone if no hints are set.

    Flags that alter behaviour are:
    \list
    \li Qt.ImhHiddenText - Characters should be hidden, as is typically used when entering passwords.
    \li Qt.ImhSensitiveData - Typed text should not be stored by the active input method in any persistent storage like predictive user dictionary.
    \li Qt.ImhNoAutoUppercase - The input method should not try to automatically switch to upper case when a sentence ends.
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
      */
    property alias inputMethodHints: editor.inputMethodHints

    /*!
      Returns the total number of plain text characters in the TextArea item.

      As this number doesn't include any formatting markup it may not be the
      same as the length of the string returned by the text property.

      This property can be faster than querying the length the text property
      as it doesn't require any copying or conversion of the TextArea's internal
      string data.
      */
    property alias length: editor.length

    /*!
      Returns the total number of lines in the TextArea item.
      */
    property alias lineCount: editor.lineCount

    /*!
      Specifies how text should be selected using a mouse.
        \list
        \li TextEdit.SelectCharacters - The selection is updated with individual characters. (Default)
        \li TextEdit.SelectWords - The selection is updated with whole words.
        \endlist
      This property only applies when selectByMouse is true.
      */
    property alias mouseSelectionMode: editor.mouseSelectionMode

    /*!
      Whether the TextArea should keep the selection visible when it loses active
      focus to another item in the scene. By default this is set to true;
      */
    property alias persistentSelection: editor.persistentSelection

    /*!
      Whether the user can interact with the TextArea item. If this property is set
      to true the text cannot be edited by user interaction.

      By default this property is false.
      */
    property alias readOnly: editor.readOnly

    /*!
      Override the default rendering type for this component.

      Supported render types are:
        \list
        \li Text.QtRendering - the default
        \li Text.NativeRendering
        \endlist
      Select Text.NativeRendering if you prefer text to look native on the target
      platform and do not require advanced features such as transformation of the
      text. Using such features in combination with the NativeRendering render type
      will lend poor and sometimes pixelated results.
      */
    property alias renderType: editor.renderType

    /*!
      This read-only property provides the text currently selected in the text edit.
      */
    property alias selectedText: editor.selectedText

    /*!
      The selected text color, used in selections.
      */
    property alias selectedTextColor: editor.selectedTextColor

    /*!
      The text highlight color, used behind selections.
      */
    property alias selectionColor: editor.selectionColor

    /*!
      The cursor position after the last character in the current selection.

      This property is read-only. To change the selection, use select(start, end),
      selectAll(), or selectWord().

      See also selectionStart, cursorPosition, and selectedText.
      */
    property alias selectionEnd: editor.selectionEnd

    /*!
      The cursor position before the first character in the current selection.

      This property is read-only. To change the selection, use select(start, end),
      selectAll(), or selectWord().

      See also selectionEnd, cursorPosition, and selectedText.
      */
    property alias selectionStart: editor.selectionStart

    /*!
      The text to display. If the text format is AutoText the text edit will
      automatically determine whether the text should be treated as rich text.
      This determination is made using Qt::mightBeRichText().
      */
    property alias text: editor.text

    /*!
      The way the text property should be displayed.
        \list
        \li TextEdit.AutoText
        \li TextEdit.PlainText
        \li TextEdit.RichText
        \endlist
      The default is TextEdit.PlainText. If the text format is TextEdit.AutoText
      the text edit will automatically determine whether the text should be treated
      as rich text. This determination is made using Qt::mightBeRichText().
      */
    property alias textFormat: editor.textFormat

    /*!
      Sets the vertical alignment of the text within the TextAres item's width
      and height. By default, the text alignment follows the natural alignment
      of the text.

      Valid values for verticalAlignment are:

        \list
        \li TextEdit.AlignTop (default)
        \li TextEdit.AlignBottom
        \li TextEdit.AlignVCenter
        \endlist
      */
    property alias verticalAlignment: editor.verticalAlignment

    /*!
      Set this property to wrap the text to the TextEdit item's width. The text
      will only wrap if an explicit width has been set.
        \list
        \li TextEdit.NoWrap - no wrapping will be performed. If the text contains
            insufficient newlines, then implicitWidth will exceed a set width.
        \li TextEdit.WordWrap - wrapping is done on word boundaries only. If a word
            is too long, implicitWidth will exceed a set width.
        \li TextEdit.WrapAnywhere - wrapping is done at any point on a line, even
            if it occurs in the middle of a word.
        \li TextEdit.Wrap - if possible, wrapping occurs at a word boundary; otherwise
            it will occur at the appropriate point on the line, even in the middle of a word.
        \endlist
       The default is TextEdit.Wrap
      */
    property alias wrapMode:editor.wrapMode

    // signals
    /*!
      This handler is called when the user clicks on a link embedded in the text.
      The link must be in rich text or HTML format and the link string provides
      access to the particular link.
      */
    signal linkActivated(string link)

    // functions
    /*!
      Copies the currently selected text to the system clipboard.
      */
    function copy()
    {
        editor.copy();
    }

    /*!
      Moves the currently selected text to the system clipboard.
      */
    function cut()
    {
        editor.cut();
    }

    /*!
      Removes active text selection.
      */
    function deselect()
    {
        editor.deselect();
    }

    /*!
      Inserts text into the TextArea at position.
      */
    function insert(position, text)
    {
        editor.insert(position, text);
    }

    /*!
      Returns the text position closest to pixel position (x, y).

      Position 0 is before the first character, position 1 is after the first
      character but before the second, and so on until position text.length,
      which is after all characters.
      */
    function positionAt(x, y)
    {
        return editor.positionAt(x, y);
    }

    /*!
      Returns true if the natural reading direction of the editor text found
      between positions start and end is right to left.
      */
    function isRightToLeft(start, end)
    {
        return editor.isRightToLeft(start, end)
    }

    /*!
      Moves the cursor to position and updates the selection according to the
      optional mode parameter. (To only move the cursor, set the cursorPosition
      property.)

      When this method is called it additionally sets either the selectionStart
      or the selectionEnd (whichever was at the previous cursor position) to the
      specified position. This allows you to easily extend and contract the selected
      text range.

      The selection mode specifies whether the selection is updated on a per character
      or a per word basis. If not specified the selection mode will default to whatever
      is given in the mouseSelectionMode property.
      */
    function moveCursorSelection(position, mode)
    {
        if (mode === undefined)
            editor.moveCursorSelection(position, mouseSelectionMode);
        else
            editor.moveCursorSelection(position, mode);
    }

    /*!
      \preliminary
      Places the clipboard or the data given as parameter into the text input.
      The selected text will be replaces with the data.
    */
    function paste(data) {
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
      Returns the rectangle at the given position in the text. The x, y, and height
      properties correspond to the cursor that would describe that position.
      */
    function positionToRectangle(position)
    {
        return editor.positionToRectangle(position);
    }

    /*!
      Redoes the last operation if redo is \l{canRedo}{available}.
      */
    function redo()
    {
        editor.redo();
    }

    /*!
      Causes the text from start to end to be selected.

      If either start or end is out of range, the selection is not changed.

      After calling this, selectionStart will become the lesser and selectionEnd
      will become the greater (regardless of the order passed to this method).

      See also selectionStart and selectionEnd.
      */
    function select(start, end)
    {
        editor.select(start, end);
    }

    /*!
      Causes all text to be selected.
      */
    function selectAll()
    {
        editor.selectAll();
    }

    /*!
      Causes the word closest to the current cursor position to be selected.
      */
    function selectWord()
    {
        editor.selectWord();
    }

    /*!
      Returns the section of text that is between the start and end positions.

      The returned text will be formatted according the textFormat property.
      */
    function getFormattedText(start, end)
    {
        return editor.getFormattedText(start, end);
    }

    /*!
      Returns the section of text that is between the start and end positions.

      The returned text does not include any rich text formatting. A getText(0, length)
      will result in the same value as displayText.
      */
    function getText(start, end)
    {
        return editor.getText(start, end);
    }

    /*!
      Removes the section of text that is between the start and end positions
      from the TextArea.
      */
    function remove(start, end)
    {
        return editor.remove(start, end);
    }

    /*!
      Undoes the last operation if undo is \l{canUndo}{available}. Deselects
      any current selection, and updates the selection start to the current
      cursor position.
      */
    function undo()
    {
        editor.undo();
    }

    // logic
    /*!\internal - to remove warnings */
    Component.onCompleted: {
        editor.linkActivated.connect(control.linkActivated);
        internal.prevShowCursor = control.cursorVisible;
    }

    // activation area on mouse click
    // the editor activates automatically when pressed in the editor control,
    // however that one can be slightly spaced to the main control area

    // styling
    Theming.ItemStyle.class: "textarea"
    //internals

    /*!\internal */
    onVisibleChanged: {
        if (!visible)
            control.focus = false;
    }

    QtObject {
        id: internal
        // public property locals enabling aliasing
        property string displayText: editor.getText(0, editor.length)
        property real spacing: ComponentUtils.style(control, "overlaySpacing", units.gu(0.5))
        property real lineSpacing: units.dp(3)
        property real frameSpacing: ComponentUtils.style(control, "frameSpacing", units.gu(0.35))
        property real lineSize: editor.font.pixelSize + lineSpacing
        property real frameLineHeight: editor.font.pixelSize + 2 * spacing + frameSpacing
        //selection properties
        property bool draggingMode: false
        property bool selectionMode: false
        property bool prevShowCursor

        onDraggingModeChanged: {
            if (draggingMode) selectionMode = false;
        }
        onSelectionModeChanged: {
            if (selectionMode)
                draggingMode = false;
            else {
                toggleSelectionCursors(false);
            }
        }

        function toggleSelectionCursors(show)
        {
            if (!show) {
                leftCursorLoader.sourceComponent = undefined;
                rightCursorLoader.sourceComponent = undefined;
                editor.cursorVisible = prevShowCursor;
            } else {
                prevShowCursor = editor.cursorVisible;
                editor.cursorVisible = false;
                leftCursorLoader.sourceComponent = cursor;
                rightCursorLoader.sourceComponent = cursor;
            }
        }

        function activateEditor()
        {
            if (!control.activeFocus)
                control.forceActiveFocus();
        }

        function showInputPanel()
        {
            Qt.inputMethod.show();
        }
        function hideInputPanel()
        {
            Qt.inputMethod.hide();
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
                if ((control.maximumLineCount <= 0) || (control.maximumLineCount > 0) && (control.lineCount <= control.maximumLineCount)) {
                    // check if the control height is bigger than the value we have to set
                    var h = frameLinesHeight(control.lineCount);
                    if (h > control.height)
                        control.height = h;
                }
            }
        }

        function enterSelectionMode(x, y)
        {
            if (undefined !== x && undefined !== y) {
                control.cursorPosition = control.positionAt(x, y);
                control.moveCursorSelection(control.cursorPosition + 1);
            }
            toggleSelectionCursors(true);
        }
    }

    // cursor is FIXME: move in a separate element and align with TextField
    Component {
        id: cursor
        Item {
            id: cursorItem
            // new properties
            property var editorItem: control
            property string positionProperty

            Theming.ItemStyle.class: "cursor"
            height: internal.lineSize
        }
    }
    // selection cursor loader
    Loader {
        id: leftCursorLoader
        onStatusChanged: {
            if (status == Loader.Ready && item) {
                item.Theming.ItemStyle.class = "left-pin";
                item.positionProperty = "selectionStart";
                item.parent = editor;
            }
        }
    }
    Loader {
        id: rightCursorLoader
        onStatusChanged: {
            if (status == Loader.Ready && item) {
                item.Theming.ItemStyle.class = "right-pin";
                item.positionProperty = "selectionEnd";
                item.parent = editor;
            }
        }
    }

    // holding default values
    Label { id: fontHolder }
    SystemPalette { id: systemColors }

    //hint
    Label {
        id: hint
        anchors {
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
            // forward keys to the root element so it can be captured outside of it
            Keys.forwardTo: [control]

            // styling
            Theming.ItemStyle.style: control.Theming.ItemStyle.style
            color: ComponentUtils.style(editor, "color", "black")
            selectedTextColor: ComponentUtils.style(editor, "selectedTextColor", systemColors.highlightedText)
            selectionColor: ComponentUtils.style(editor, "selectionColor", systemColors.highlight)
            font: ComponentUtils.style(editor, "font", fontHolder.font)

            // autoexpand handling
            onLineCountChanged: internal.frameSize()

            // virtual keyboard handling
            activeFocusOnPress: false
            onActiveFocusChanged: {
                if (activeFocus) {
                    internal.showInputPanel();
                } else {
                    internal.hideInputPanel();
                }
            }

            // remove selection when typing starts or input method start entering text
            onInputMethodComposingChanged: {
                if (inputMethodComposing)
                    internal.selectionMode = false;
            }
            Keys.onPressed: {
                if ((event.text !== ""))
                    internal.selectionMode = false;
            }
            Keys.onReleased: {
                // selection positioners are updated after the keypress
                if (selectionStart == selectionEnd)
                    internal.selectionMode = false;
            }

            // handling text selection
            MouseArea {
                id: handler
                enabled: control.enabled && control.activeFocusOnPress
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
                    internal.selectionMode = control.selectByMouse;
                    if (internal.selectionMode && control.selectByMouse) {
                        internal.enterSelectionMode(mouse.x, mouse.y);
                    }
                }
                onReleased: {
                    internal.draggingMode = false;
                }
                onPositionChanged: {
                    if (internal.draggingMode) {
                        internal.draggingMode = false;
                        mouse.accepted = false;
                        return;
                    }
                    if (internal.selectionMode && control.selectByMouse) {
                        control.moveCursorSelection(editor.positionAt(mouse.x, mouse.y))
                    }
                }
                onDoubleClicked: {
                    internal.activateEditor()
                    internal.selectionMode = control.selectByMouse;
                    if (internal.selectionMode && control.selectByMouse) {
                        control.selectWord()
                        internal.enterSelectionMode();
                    }
                }
                onClicked: {
                    internal.activateEditor()
                    control.cursorPosition = control.positionAt(mouse.x, mouse.y)
                }
            }
        }
    }
}
