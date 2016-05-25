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

import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

/*
  This component is a unified text selection and scrolling handler for both
  TextField and TextArea components.
  */

MultiPointTouchArea {
    id: inputHandler
    objectName: "input_handler"

    // the root control
    property Item main
    // the input instance
    property Item input
    // the Flickable holding the input instance
    property Flickable flickable

    // item filling the visible text input area used to check handler visibility
    property Item visibleArea: Item {
        parent: flickable
        anchors.fill: parent
    }

    // line size and spacing
    property real lineSpacing: units.dp(3)
    property real lineSize: input.font.pixelSize + lineSpacing
    // input x/y distance from the frame
    property point frameDistance: Qt.point(flickable.x, flickable.y)

    // signal triggered when popup should be opened
    signal pressAndHold(int pos, bool fromTouch)
    signal tap(int pos)
    property string oldText: ""
    signal textModified()

    function activateInput() {
        if (!input.activeFocus) {
            input.forceActiveFocus();
        }
        showInputPanel();
    }

    function showInputPanel() {
        if (!UbuntuApplication.inputMethod.visible) {
            UbuntuApplication.inputMethod.show();
        }
        textChanged = false;
    }

    // internal properties/functions
    readonly property bool singleLine: input.hasOwnProperty("validator")
    property var flickableList: new Array()
    property bool textChanged: false
    property var popover
    property bool suppressReleaseEvent: false
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

    // ensures the text cursorRectangle is always in the internal Flickable's visible area
    function ensureVisible(rect)
    {
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
    function unadulteratedCursorPosition(x, y) {
        return singleLine ? input.positionAt(x, TextInput.CursorOnCharacter) : input.positionAt(x, y, TextInput.CursorOnCharacter);
    }
    // returns the cursor position taking frame into account
    function cursorPosition(x, y) {
        var frameSpacing = main.__styleInstance.frameSpacing;
        var cursorPosition = unadulteratedCursorPosition(x, y);
        if (cursorPosition == 0)
            cursorPosition = unadulteratedCursorPosition(x + frameSpacing, y + frameSpacing);
        if (cursorPosition == text.length)
            cursorPosition = unadulteratedCursorPosition(x - frameSpacing, y - frameSpacing);
        return cursorPosition
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
        state = "select";
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
    function openContextMenu(mouse, noAutoselect) {
        var pos = mousePosition(mouse);
        if (!main.focus || !mouseInSelection(mouse)) {
            activateInput();
            input.cursorPosition = pressedPosition = mousePosition(mouse);
            if (!noAutoselect) {
                input.selectWord();
            }
        }
        // open context menu at the cursor position
        inputHandler.pressAndHold(input.cursorPosition, mouse.touch);
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

    // moves the cursor one page forward with or without positioning the cursor
    function movePage(forward) {
        var cx = input.cursorRectangle.x;
        var cy = input.cursorRectangle.y;
        if (forward) {
            if (singleLine) {
                cx += visibleArea.width;
            } else {
                cy += visibleArea.height;
            }
        } else {
            if (singleLine) {
                cx -= visibleArea.width;
            } else {
                cy -= visibleArea.height;
            }
        }
        input.cursorPosition = cursorPosition(cx, cy);
    }

    Component.onCompleted: {
        state = (main.focus) ? "" : "inactive";
        // FIXME: Qt5.3 related! mouseEnabled is a 5.3 related property which has a positive
        // default value. That value messes up teh current understanding (5.2) of the
        // MultiPointTouchArea functioning. We need to set it to false until 5.3 will be the
        // default supported Qt version, when we can have a clean property value assignment
        if (inputHandler.hasOwnProperty("mouseEnabled")) {
            inputHandler.mouseEnabled = false;
        }
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
                    // stop touch timers
                    touchPoint.reset();
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

    // brings the state back to default when the component looses focus
    // and ensures input has active focus when component regains focus
    Connections {
        target: main
        ignoreUnknownSignals: true
        onActiveFocusChanged: {
            input.focus = main.activeFocus;
        }
        onKeyNavigationFocusChanged: {
            if (main.keyNavigationFocus) {
                input.forceActiveFocus();
            }
        }
        onFocusChanged: {
            UbuntuApplication.inputMethod.commit()
            state = (main.focus) ? "" : "inactive";
            if (main.focus) {
                input.forceActiveFocus()
            }
        }
        onVisibleChanged: {
            if (!main.visible)
                main.focus = false;
        }
    }

    // input specific signals
    Connections {
        target: input
        onCursorRectangleChanged: ensureVisible(input.cursorRectangle)
        onTextChanged: {
            textChanged = true;
            if (oldText != input.text) {
                textModified()
                oldText = text
            }
        }
        // make sure we show the OSK
        onActiveFocusChanged: {
            if (!input.activeFocus && popover)
                PopupUtils.close(popover);
            showInputPanel();
        }
    }

    // inner or outer Flickable controlling
    Connections {
        target: scroller
        // turn scrolling state on
        onFlickStarted: toggleScrollingState(true)
        onMovementStarted: toggleScrollingState(true)
        // reset to default state
        onMovementEnded: toggleScrollingState(false)

        function toggleScrollingState(turnOn) {
            if (!main.focus) {
                return;
            }
            inputHandler.state = (turnOn) ? "scrolling" : ""
        }
    }

    // PageUp and PageDown handling
    Keys.onPressed: {
        if (event.key === Qt.Key_PageUp && event.modifiers === Qt.NoModifier) {
            movePage(false);
        } else if (event.key === Qt.Key_PageDown && event.modifiers === Qt.NoModifier) {
            movePage(true);
        }
    }

    // touch and mous handling
    function handlePressed(event) {
        if (event.touch) {
            // we do not have longTap or double tap, therefore we need to generate those
            event.touch();
        } else {
            // consume event so it does not get forwarded to the input
            event.accepted = true;
        }
        // remember pressed position as we need it when entering into selection state
        pressedPosition = mousePosition(event);
    }
    function handleReleased(event) {
        if (event.touch) {
            event.untouch();
        }
        if ((!main.focus && !main.activeFocusOnPress) || suppressReleaseEvent === true) {
            suppressReleaseEvent = false;
            return;
        }

        activateInput();
        if (state === "" || event.touch) {
            input.cursorPosition = mousePosition(event);
        }
        moveStarts = moveEnds = -1;
        state = "";
        // check if we get right-click from the frame or the area that has no text
        if (event.button === Qt.RightButton) {
            // open the popover
            inputHandler.pressAndHold(input.cursorPosition, event.touch);
        } else {
            inputHandler.tap(input.cursorPosition);
        }
    }
    function handleMove(event) {
        // leave if not focus, not the left button or not in select state
        if (!input.activeFocus || (!event.touch && event.button !== Qt.LeftButton) || !main.selectByMouse) {
            return;
        }
        selectText(event);
    }
    function handleDblClick(event) {
        if (main.selectByMouse) {
            openContextMenu(event, false);
            // turn selection state temporarily so the selection is not cleared on release
            state = "selection";
            suppressReleaseEvent = true;
        }
    }

    // Mouse handling
    Mouse.forwardTo: [main]
    Mouse.onPressed: handlePressed(mouse)
    Mouse.onReleased: handleReleased(mouse)
    Mouse.onPositionChanged: handleMove(mouse)
    Mouse.onDoubleClicked: handleDblClick(mouse)

    // right button handling
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        // trigger pressAndHold
        onReleased: openContextMenu(mouse, true)
        cursorShape: Qt.IBeamCursor
    }
    Keys.onMenuPressed: inputHandler.pressAndHold(input.cursorPosition, false);

    // touch handling
    touchPoints: TouchPoint {
        id: touchPoint
        function touch() {
            longTap.restart();
            if (!doubleTap.running) {
                doubleTap.restart();
            } else if (doubleTap.tapCount > 0) {
                doubleTap.running = false;
                handleDblClick(touchPoint, true);
            }
        }
        function untouch() {
            longTap.running = false;
        }
        function reset() {
            longTap.running = false;
            doubleTap.running = false;
        }
    }
    Timer {
        id: longTap
        // sync with QQuickMouseArea constant
        interval: 800
        onTriggered: {
            // do not open context menu if the input is not focus
            if (!main.focus) {
                return;
            }

            // do not open context menu if this is scrolling
            if (touchPoint.startY - touchPoint.y < -units.gu(2))
                return;

            openContextMenu(touchPoint, false);
            suppressReleaseEvent = true;
        }
    }

    property bool doubleTapInProgress: doubleTap.running
    Timer {
        id: doubleTap
        property int tapCount: 0
        interval: 400
        onRunningChanged: {
            tapCount = running;
        }
    }
    onPressed: handlePressed(touchPoints[0])
    onReleased: handleReleased(touchPoints[0])

    property Item cursorPositionCursor: null
    property Item selectionStartCursor: null
    property Item selectionEndCursor: null

    // cursors to use when text is selected
    Connections {
        target: input
        onSelectedTextChanged: {
            if (selectedText !== "") {
                if (!selectionStartCursor) {
                    selectionStartCursor = input.cursorDelegate.createObject(
                                input, {
                                    "positionProperty": "selectionStart",
                                    "handler": inputHandler,
                                    }
                                );
                    moveSelectionCursor(selectionStartCursor);
                    selectionEndCursor = input.cursorDelegate.createObject(
                                input, {
                                    "positionProperty": "selectionEnd",
                                    "handler": inputHandler,
                                    }
                                );
                    moveSelectionCursor(selectionEndCursor);
                }
            } else {
                if (selectionStartCursor) {
                    selectionStartCursor.destroy();
                    selectionStartCursor = null;
                    selectionEndCursor.destroy();
                    selectionEndCursor = null;
                }
            }
        }
        onSelectionStartChanged: moveSelectionCursor(selectionStartCursor, true);
        onSelectionEndChanged: moveSelectionCursor(selectionEndCursor, true);

        function moveSelectionCursor(cursor, updateProperty) {
            if (!cursor) {
                return;
            }
            // workaround for https://bugreports.qt-project.org/browse/QTBUG-38704
            // selectedTextChanged signal is not emitted for TextEdit when selectByMouse is false
            if (updateProperty && QuickUtils.className(input) === "QQuickTextEdit") {
                input.selectedTextChanged();
            }

            var pos = input.positionToRectangle(input[cursor.positionProperty]);
            cursor.x = pos.x;
            cursor.y = pos.y;
            cursor.height = pos.height;
            ensureVisible(pos);
        }
    }
}
