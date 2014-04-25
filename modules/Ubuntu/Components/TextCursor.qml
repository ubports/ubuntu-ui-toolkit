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
import Ubuntu.Components 1.0
import Ubuntu.Components.Popups 1.0

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
      The property holding the cursor component of the input
      */
    property Component cursorDelegate

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
        handler.pressAndHold.connect(cursorItem.openPopover);
    }

    /*
     * Caret dragging handling. We need a separate item which is dragged along the
     * component's area, which can move freely and not attached to the caret itself.
     * This area will then be used to update the caret position.
     */
    onXChanged: if (draggedItem.state === "") draggedItem.moveToCaret()
    onYChanged: if (draggedItem.state === "") draggedItem.moveToCaret()

    //dragged item
    Rectangle { opacity: 0.5; color: "blue"
        id: draggedItem
        width: __styleInstance.caretHandler ? __styleInstance.caretHandler.width : 0
        height: __styleInstance.caretHandler ? __styleInstance.caretHandler.height : 0
        parent: handler.input

        onStateChanged: print('dragstate=', state)

        /*
          Mouse area to turn on dragging or selection mode when pressed
          on the handler area. The drag mode is turned off when the drag
          gets inactive or when the LeftButton is released.
          */
        MouseArea {
            anchors.fill: parent
            preventStealing: true
            enabled: parent.width && parent.height

            onPressed: {
                draggedItem.moveToCaret(mouse.x, mouse.y);
                draggedItem.state = "dragging";
            }
            Mouse.forwardTo: [dragger]
            /*
              As we are forwarding the events to the upper mouse area, the release
              will not get into the normal MosueArea onRelease signal as the preventStealing
              will not have any effect on the handling. However due to the mouse
              filter's nature, we will still be able to grab mouse events and we
              can stop dragging. We only handle the release in case the drag hasn't
              been active at all, otherwise the drag will not be deactivated and we
              will end up in a binding loop on the moveToCaret() next time the caret
              handler is grabbed.
              */
            Mouse.onReleased: if (!dragger.drag.active) draggedItem.state = ""
        }

        // aligns the draggedItem to the caret and resets the dragger
        function moveToCaret(cx, cy) {
            if (cx === undefined && cy === undefined) {
                cx = cursorItem.x + (__styleInstance.caretHandler ? __styleInstance.caretHandler.x : 0);
                cy = cursorItem.y + (__styleInstance.caretHandler ? __styleInstance.caretHandler.y : 0);
            } else {
                cx += draggedItem.x;
                cy += draggedItem.y;
            }

            draggedItem.x = cx;
            draggedItem.y = cy;
            dragger.resetDrag();
        }
        // positions caret to the dragged position
        function positionCaret() {
            if (dragger.drag.active) {
                handler.positionCaret(positionProperty,
                                      dragger.thumbStartX + dragger.dragAmountX,
                                      dragger.thumbStartY + dragger.dragAmountY);
            }
        }
    }
    MouseArea {
        id: dragger
        // fill the entire component area
        parent: handler.main
        anchors.fill: parent
        hoverEnabled: true
        preventStealing: drag.active
        enabled: draggedItem.enabled && draggedItem.state === "dragging"

        property int thumbStartX
        property int dragStartX
        property int dragAmountX: dragger.drag.target.x - dragStartX
        property int thumbStartY
        property int dragStartY
        property int dragAmountY: dragger.drag.target.y - dragStartY

        function resetDrag() {
            thumbStartX = cursorItem.x + (__styleInstance.caretHandler ? __styleInstance.caretHandler.x : 0);
            thumbStartY = cursorItem.y + (__styleInstance.caretHandler ? __styleInstance.caretHandler.y : 0);
            dragStartX = drag.target.x;
            dragStartY = drag.target.y;
        }

        // do not set minimum/maximum so we can drag outside of the Flickable area
        drag {
            target: draggedItem
            axis: handler.singleLine ? Drag.XAxis : Drag.XAndYAxis
            // deactivate dragging
            onActiveChanged: {
                if (!drag.active) {
                    draggedItem.state = "";
                }
            }
        }

        onDragAmountXChanged: draggedItem.positionCaret()
        onDragAmountYChanged: draggedItem.positionCaret()
    }
}
