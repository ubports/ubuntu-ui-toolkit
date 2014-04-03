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
import Ubuntu.Components 0.1 as Ubuntu

Item {
    // the root control
    property Item main
    // the input instance
    property Item input
    // the Flickable holdiong the input instance
    property Flickable flickable
    // selection cursor mode
    property bool selectionCursor: input && input.selectedText !== ""
    // True if mouse handlig is enabled, false if flicking mode is enabled
    readonly property bool mouseHandlingEnabled: !flickable.interactive

    // signal triggered when popup shoudl be opened
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

    // ensures the text cusrorRectangle is always in the Flickable's visible area
    function ensureVisible()
    {
        var rect = input.cursorRectangle;
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
    // returns the mouse position
    function mousePosition(mouse) {
        return singleLine ? input.positionAt(mouse.x) : input.positionAt(mouse.x, mouse.y);
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

    // states
    states: [
        // override default state to turn on the saved Flickable interactive mode
        State {
            name: ""
            StateChangeScript {
                // restore interactive for all Flickable parents
                script: {
                    while (flickableList.length > 0) {
                        var p = flickableList.pop();
                        p.interactive = true;
                    }
                }
            }
        },

        State {
            name: "scrolling"
            StateChangeScript {
                script: {
                    selectionTimeout.running = false;
                }
            }
        },
        State {
            name: "select"
            // during select state the Flickable is not interactive
            StateChangeScript {
                script: {
                    // turn off interactive for all parent flickables
                    var p = input.parent;
                    while (p) {
                        if (p.hasOwnProperty("flicking")) {
                            if (p.interactive) {
                                flickableList.push(p);
                                p.interactive = false;
                            } else {
                                break;
                            }
                        }
                        p = p.parent;
                    }

                    if (!positionInSelection(pressedPosition)) {
                        input.cursorPosition = pressedPosition;
                    }
                }
            }
        }
    ]

    Connections {
        target: main
        onFocusChanged: if (!main.focus) state = "";
    }

    Connections {
        target: input
        onCursorRectangleChanged: ensureVisible()
        onTextChanged: textChanged = true;
    }

    Connections {
        target: flickable
        // turn scrolling state on
        onFlickStarted: state = "scrolling"
        onMovementStarted: state = "scrolling"
        // reset to default state
        onMovementEnded: state = ""
    }

    // switches the
    Timer {
        id: selectionTimeout
        interval: 300
        onTriggered: {
            if (!flickable.moving) {
                state = "select";
            }
        }
    }

    // Mouse handling
    Ubuntu.Mouse.forwardTo: [main]
    Ubuntu.Mouse.clickAndHoldThreshold: units.gu(2)
    Ubuntu.Mouse.onPressed: {
        if (input.activeFocus) {
            // start selection timeout
            selectionTimeout.restart();
        }
        // remember pressed position as we need it when entering into selection state
        pressedPosition = mousePosition(mouse);
        // consume event so it does not get forwarded to the input
        mouse.accepted = true;
    }
    Ubuntu.Mouse.onReleased: {
        if (!main.focus && !main.activeFocusOnPress) {
            return;
        }

        activateInput();
        // stop text selection timer
        selectionTimeout.running = false;
        if (/*!mouseInSelection(mouse) && */(state === "")) {
            input.cursorPosition = mousePosition(mouse);
        }
        moveStarts = moveEnds = -1;
        state = "";
    }
    Ubuntu.Mouse.onPositionChanged: {
        // leave if not focus, not the left button or not in select state
        if (!input.activeFocus || (mouse.button !== Qt.LeftButton) || (state !== "select") || !main.selectByMouse) {
            return;
        }
        // stop text selection timer
        selectionTimeout.running = false;
        selectText(mouse);
    }
    Ubuntu.Mouse.onDoubleClicked: {
        if (main.selectByMouse) {
            input.selectWord();
            // turn selection state temporarily so the selection is not cleared on release
            state = "selection";
        }
    }
    Ubuntu.Mouse.onPressAndHold: {
        if ((state === "select") && mouseInSelection(mouse)) {
            // trigger clipboard popover
            pressAndHold(input.cursorPosition);
        }
    }
}
