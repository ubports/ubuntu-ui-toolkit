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
import Ubuntu.Components 0.1 as Ubuntu
import "Popups" 0.1

StyledItem {
    id: cursorItem

    width: units.dp(1)

    /*
      Property holding the text input item instance.
      */
    property var editorItem

    /*
      Property holding the text input's custor position property. Can be one of
      the following ones: cursorPosition, selectionStart and selectionEnd.
      */
    property string positionProperty: "cursorPosition"

    /*
      Input handler instance
      */
    property InputHandler handler

    /*
      The property contains the custom popover to be shown.
      */
    property var popover

    /*
        The function opens the text input popover setting the text cursor as caller.
      */
    function openPopover() {
        if (!visible)
            return;
        if (popover === undefined) {
            // open the default one
            PopupUtils.open(Qt.resolvedUrl("TextInputPopover.qml"), cursorItem,
                            {
                                "target": editorItem
                            })
        } else {
            PopupUtils.open(popover, cursorItem,
                            {
                                "target": editorItem
                            })
        }
    }

    style: Theme.createStyleComponent("TextCursorStyle.qml", cursorItem)

    Component.onCompleted: {
        draggedItem.moveToCaret();
        handler.pressAndHold.connect(cursorItem.openPopover);
    }

    /*
     * Caret handling
     */
    onXChanged: if (draggedItem.state === "") draggedItem.moveToCaret()
    onYChanged: if (draggedItem.state === "") draggedItem.moveToCaret()

    //dragged item
    Rectangle { opacity: 0.5; color: "blue"
        id: draggedItem
        width: __styleInstance.caretHandler.width
        height: __styleInstance.caretHandler.height
        parent: handler

        // mouse area to turn on dragging or selection mode
        MouseArea {
            anchors.fill: parent
            preventStealing: true

            onPressed: {
                draggedItem.moveToCaret();
                draggedItem.state = "dragging";
            }
            Ubuntu.Mouse.forwardTo: [dragger]

        }

        onXChanged: if (state === "dragging") draggedItem.positionHandler()
        onYChanged: if (state === "dragging") draggedItem.positionHandler()

        function moveToCaret() {
            draggedItem.x = cursorItem.x + __styleInstance.caretHandler.x;
            draggedItem.y = cursorItem.y + __styleInstance.caretHandler.y;
            print("REPOS", draggedItem.x, draggedItem.y)
        }
        function positionHandler() {
            var cx = draggedItem.x - handler.flickable.contentX;
            var cy = draggedItem.y - handler.flickable.contentY;
            print(cx, cy)
            handler.positionCaret(positionProperty, cx, cy);
        }
    }
    MouseArea {
        id: dragger
        // fill the entire input area
        parent: draggedItem.parent
        anchors.fill: parent
        hoverEnabled: false
        preventStealing: drag.active
        enabled: draggedItem.state === "dragging"

        drag {
            target: draggedItem
            axis: handler.singleLine ? Drag.XAxis : Drag.XAndYAxis
            onActiveChanged: {
                if (!drag.active) {
                    draggedItem.state = "";
                } else {
                    // position the pin
                    print("HUHUUUU")
                }
            }
        }
    }
}
