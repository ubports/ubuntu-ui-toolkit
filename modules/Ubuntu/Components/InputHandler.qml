/*
 * Copyright 2014 Canonical Ltd.
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
import Ubuntu.Components 1.1

/*
  This component is a unified text selection and scrolling handler for both
  TextField and TextArea components.
  */

Item {
    id: inputHandler
    objectName: "input_handler"
    // the root control
    property Item main
    // the input instance
    property Item input
    // the Flickable holding the input instance
    property Flickable flickable
    // selection cursor mode
    property bool selectionCursor: input && input.selectedText !== ""
    // True if mouse handlig is enabled, false if flicking mode is enabled
    readonly property bool mouseHandlingEnabled: !flickable.interactive
    // property holding the selection mode timeout
    property int selectionModeTimeout: 200

    // line size and spacing
    property real lineSpacing: units.dp(3)
    property real lineSize: input.font.pixelSize + lineSpacing
    // input x/y distance from the frame
    property point frameDistance: Qt.point(0,0)

    // signal triggered when popup should be opened
    signal pressAndHold(int pos)

    function activateInput() {
        if (!input.activeFocus) {
            input.forceActiveFocus();
        } else {
            showInputPanel();
        }
    }

    function showInputPanel() {
        if (!Qt.inputMethod.visible) {
            Qt.inputMethod.show();
        }
        textChanged = false;
    }
    function hideInputPanel() {
        Qt.inputMethod.hide();
        // emit accepted signal if changed
        if (textChanged && input.hasOwnProperty("accepted")) {
            input.accepted();
        }
    }

    // internal properties/functions
    readonly property bool singleLine: input && input.hasOwnProperty("validator")
    property var flickableList: new Array()
    property bool textChanged: false
    property int pressedPosition: -1
    // move properties
    property int moveStarts: -1
    property int moveEnds: -1
    // set scroller to the first Flickable that scrolls the input
    // this can be the internal Flickable if the full autosize is disabled
    // or one of the input's parent Flickable
    readonly property bool scrollingDisabled: main && main.hasOwnProperty("autoSize") ?
                                                  (main.autoSize && (main.maximumLineCount <= 0)) : false
    onScrollingDisabledChanged: if (state == "") flickable.interactive = !scrollingDisabled
    readonly property Flickable grandScroller: firstFlickableParent(main)
    readonly property Flickable scroller: (scrollingDisabled && grandScroller) ? grandScroller : flickable

    // ensures the text cusrorRectangle is always in the internal Flickable's visible area
    function ensureVisible(rect)
    {
        if (rect === undefined) {
            rect = input.cursorRectangle;
        }
        if (flickable.moving || flickable.flicking)
            return;
        if (flickable.contentX >= rect.x)
            flickable.contentX = rect.x;
        else if (flickable.contentX + flickable.width <= rect.x + rect.width)
            flickable.contentX = rect.x + rect.width - flickable.width;
        if (flickable.contentY >= rect.y)
            flickable.contentY = rect.y;
        else if (flickable.contentY + flickable.height <= rect.y + rect.height)
            flickable.contentY = rect.y + rect.height - flickable.height;
    }
    // returns the cursor position from x,y pair
    function cursorPosition(x, y) {
        return singleLine ? input.positionAt(x, TextInput.CursorOnCharacter) : input.positionAt(x, y, TextInput.CursorOnCharacter);
    }

    // returns the mouse position
    function mousePosition(mouse) {
        return cursorPosition(mouse.x, mouse.y);
    }
    // checks whether the position is in the selected text
    function positionInSelection(pos) {
        return (input.selectionStart !== input.selectionEnd)
                && (pos >= Math.min(input.selectionStart, input.selectionEnd))
                && (pos <= Math.max(input.selectionStart, input.selectionEnd));
    }

    // check whether the mouse is inside a selected text area
    function mouseInSelection(mouse) {
        var pos = mousePosition(mouse);
        return positionInSelection(pos);
    }
    // selects text
    function selectText(mouse) {
        moveEnds = mousePosition(mouse);
        if (moveStarts < 0) {
            moveStarts = moveEnds;
        }
        input.select(moveStarts, moveEnds);
    }
    // returns the first Flickable parent of a given item
    function firstFlickableParent(item) {
        var p = item ? item.parent : null;
        while (p && !p.hasOwnProperty("flicking")) {
            p = p.parent;
        }
        return p;
    }
    // focuses the input if not yet focused, and shows the context menu
    function openContextMenu(mouse) {
        var pos = mousePosition(mouse);
        if (!main.focus || !mouseInSelection(mouse)) {
            activateInput();
            input.cursorPosition = pressedPosition = mousePosition(mouse);
        }
        // open context menu at the cursor position
        inputHandler.pressAndHold(input.cursorPosition);
        // if opened with left press (touch falls into this criteria as well), we need to set state to inactive
        // so the mouse moves won't result in selected text loss/change
        if (mouse.button === Qt.LeftButton) {
            state = "inactive";
        }
    }

    // disables interactive Flickable parents, stops at the first non-interactive flickable.
    function toggleFlickablesInteractive(turnOn) {
        var p;
        if (!turnOn) {
            // handle the scroller separately
            p = firstFlickableParent(scroller)
            while (p) {
                if (p.interactive) {
                    flickableList.push(p);
                    p.interactive = false;
                } else {
                    break;
                }
                p = firstFlickableParent(p);
            }
        } else {
            while (flickableList.length > 0) {
                p = flickableList.pop();
                p.interactive = true;
            }
        }
    }

    // moves the specified position, called by the cursor handler
    // positioner = "currentPosition/selectionStart/selectionEnd"
    function positionCaret(positioner, x, y) {
        if (positioner === "cursorPosition") {
            input[positioner] = cursorPosition(x, y);
        } else {
            var pos = cursorPosition(x, y);
            if (positioner === "selectionStart" && (pos < input.selectionEnd)) {
                input.select(pos, input.selectionEnd);
            } else if (positioner === "selectionEnd" && (pos > input.selectionStart)) {
                input.select(input.selectionStart, pos);
            }
        }
    }

    Component.onCompleted: {
        state = (main.focus) ? "" : "inactive";
    }

    // states
    states: [
        // override default state to turn on the saved Flickable interactive mode
        State {
            name: ""
            StateChangeScript {
                // restore interactive for all Flickable parents
                script: toggleFlickablesInteractive(true);
            }
        },
        State {
            name: "inactive"
            // we do not disable scroller here as in case the internal scrolling
            // is disabled (scrollingDisabled = true) the outer scroller (grandScroller)
            // would be blocked as well, which we don't want to
            PropertyChanges {
                target: flickable
                interactive: false
            }
        },
        State {
            name: "scrolling"
            StateChangeScript {
                script: {
                    // stop scrolling all the parents
                    toggleFlickablesInteractive(false);
                    // stop selection timeout
                    selectionTimeout.running = false;
                }
            }
        },
        State {
            name: "select"
            // during select state all the flickables are blocked (interactive = false)
            // we can use scroller here as we need to disable the outer scroller too!
            PropertyChanges {
                target: scroller
                interactive: false
            }
            StateChangeScript {
                script: {
                    // turn off interactive for all parent flickables
                    toggleFlickablesInteractive(false);
                    if (!positionInSelection(pressedPosition)) {
                        input.cursorPosition = pressedPosition;
                    }
                }
            }
        }
    ]

    // brings the state back to default when the component looses focuse
    Connections {
        target: main
        ignoreUnknownSignals: true
        onFocusChanged: {
            state = (main.focus) ? "" : "inactive";
        }
    }

    // input specific signals
    Connections {
        target: input
        onCursorRectangleChanged: if (!scrollingDisabled) ensureVisible()
        onTextChanged: textChanged = true;
    }

    // inner or outer Flickable controlling
    Connections {
        target: scroller
        // turn scrolling state on
        onFlickStarted: if (!scrollingDisabled) state = "scrolling"
        onMovementStarted: if (!scrollingDisabled) state = "scrolling"
        // reset to default state
        onMovementEnded: state = ""
    }

    // switches the state to selection
    Timer {
        id: selectionTimeout
        interval: selectionModeTimeout
        onTriggered: {
            if (scroller && !scroller.moving) {
                state = "select";
            }
        }
    }

    // Mouse handling
    Mouse.forwardTo: [main]
    Mouse.onPressed: {
        if (input.activeFocus) {
            // start selection timeout
            selectionTimeout.restart();
        }
        // remember pressed position as we need it when entering into selection state
        pressedPosition = mousePosition(mouse);
        // consume event so it does not get forwarded to the input
        mouse.accepted = true;
    }
    Mouse.onReleased: {
        if (!main.focus && !main.activeFocusOnPress) {
            return;
        }

        activateInput();
        // stop text selection timer
        selectionTimeout.running = false;
        if (state === "") {
            input.cursorPosition = mousePosition(mouse);
        }
        moveStarts = moveEnds = -1;
        state = "";
        // check if we get right-click from the frame or the area that has no text
        if (mouse.button === Qt.RightButton) {
            // open the popover
            inputHandler.pressAndHold(input.cursorPosition);
        }
    }
    Mouse.onPositionChanged: {
        // leave if not focus, not the left button or not in select state
        if (!input.activeFocus || (mouse.button !== Qt.LeftButton) || (state !== "select") || !main.selectByMouse) {
            return;
        }
        // stop text selection timer
        selectionTimeout.running = false;
        selectText(mouse);
    }
    Mouse.onDoubleClicked: {
        if (main.selectByMouse) {
            input.selectWord();
            // turn selection state temporarily so the selection is not cleared on release
            state = "selection";
        }
    }
    Mouse.onPressAndHold: openContextMenu(mouse)

    // right button handling
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        // trigger pressAndHold
        onReleased: openContextMenu(mouse)
    }

    // cursors to use when text is selected
    Connections {
        property Item selectionStartCursor: null
        property Item selectionEndCursor: null
        target: input
        onSelectedTextChanged: {
            if (selectedText !== "") {
                if (!selectionStartCursor) {
                    selectionStartCursor = input.cursorDelegate.createObject(
                                input, {
                                    "positionProperty": "selectionStart",
                                    "height": lineSize,
                                    "handler": inputHandler,
                                    }
                                );
                    moveSelectionCursor(selectionStartCursor);
                }
                if (!selectionEndCursor) {
                    selectionEndCursor = input.cursorDelegate.createObject(
                                input, {
                                    "positionProperty": "selectionEnd",
                                    "height": lineSize,
                                    "handler": inputHandler,
                                    }
                                );
                    moveSelectionCursor(selectionEndCursor);
                }
            } else {
                if (selectionStartCursor) {
                    selectionStartCursor.destroy();
                }
                if (selectionEndCursor) {
                    selectionEndCursor.destroy();
                }
            }
        }
        onSelectionStartChanged: moveSelectionCursor(selectionStartCursor);
        onSelectionEndChanged: moveSelectionCursor(selectionEndCursor);

        function moveSelectionCursor(cursor) {
            if (!cursor) {
                return;
            }
            var pos = input.positionToRectangle(input[cursor.positionProperty]);
            cursor.x = pos.x;
            cursor.y = pos.y;
            ensureVisible(pos);
        }
    }
}
