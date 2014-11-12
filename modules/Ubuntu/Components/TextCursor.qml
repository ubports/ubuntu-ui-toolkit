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
import Ubuntu.Components 1.1 as Ubuntu
import Ubuntu.Components.Popups 1.0

Ubuntu.StyledItem {
    id: cursorItem

    width: units.dp(1)

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
      Cursor delegate used. This is the visual component from the main
      */
    readonly property Component cursorDelegate: handler.main.cursorDelegate ?
                                           handler.main.cursorDelegate :
                                           __styleInstance.cursorDelegate

    // depending on the positionProperty, we chose different styles
    style: Theme.createStyleComponent(handler.textCursorStyle(positionProperty), cursorItem);

    objectName: "textCursor"
    //Caret instance from the style.
    property Item caret: __styleInstance.caret
    property real caretX: caret ? caret.x : 0
    property real caretY: caret ? caret.y : 0

    // returns the mapped cursor position to a position relative to the main component
    function mappedCursorPosition(pos) {
        var cpos = cursorItem[pos];
        if (handler) {
            cpos += handler.frameDistance[pos] - handler.flickable["content"+pos.toUpperCase()];
        }
        return cpos;
    }
    /*
        The function opens the text input popover setting the text cursor as caller.
      */
    Connections {
        target: inputHandler
        onPressAndHold: openPopover()
        onTextModified: typing = true
        onTap: typing = false
    }

    function openPopover() {
        if (!visible || opacity === 0.0 || dragger.drag.active) {
            return;
        }
        // open context menu only for cursorPosition or selectionEnd
        if (positionProperty !== "selectionStart") {
            if (inputHandler.popover != null)
                inputHandler.popover.hide();

            var component = handler.main.popover;
            if (component === undefined)
                component = Qt.resolvedUrl("TextInputPopover.qml");
            var popup = PopupUtils.open(component, cursorItem, {
                "target": handler.main
            });
            contextMenuVisible = true;
            popup.onVisibleChanged.connect(contextMenuHidden.bind(undefined, popup));
            // do not grab focus!
            popup.__foreground.activeFocusOnPress = false;
            inputHandler.popover = popup;
        }
    }

    visible: handler.main.cursorVisible
    // change opacity to 0 if text is selected and the positionProperty is cursorPosition
    // note: we should not touch visibility as cursorVisible alters that!
    opacity: (positionProperty === "cursorPosition") && (handler.main.selectedText !== "") ? 0.0 : 1.0

    // cursor visual loader
    Loader {
        id: cursorLoader
        sourceComponent: cursorDelegate
        height: parent.height
        onItemChanged: {
            if (item) {
                cursorItem.width = item.width;
            }
        }
        // bind the cursor height as it may change depending on the text size
        Binding {
            target: cursorLoader.item
            property: "height"
            value: cursorLoader.height
            when: cursorLoader.item
        }
    }

    /*
     * Caret dragging handling. We need a separate item which is dragged along the
     * component's area, which can move freely and not attached to the caret itself.
     * This area will then be used to update the caret position.
     */
    Binding {
        target: caret
        when: caret
        property: "visible"
        value: QuickUtils.touchScreenAvailable
         && (contextMenuVisible || !typing)
    }
    property bool typing: false
    property bool contextMenuVisible: false
    property bool readOnly: handler.main.readOnly
    function contextMenuHidden(p) {
        contextMenuVisible = false
    }
    onXChanged: if (draggedItem.state === "") draggedItem.moveToCaret()
    onYChanged: if (draggedItem.state === "") draggedItem.moveToCaret()
    Component.onCompleted: draggedItem.moveToCaret()

    //dragged item
    Item {
        id: draggedItem
        objectName: cursorItem.positionProperty + "_draggeditem"
        width: caret ? Math.max(caret.width, units.gu(2)) : 0
        height: caret ? Math.max(caret.height, units.gu(2)) : 0
        parent: handler.main
        visible: cursorItem.visible && (cursorItem.opacity > 0.0) && QuickUtils.touchScreenAvailable

        // when the dragging ends, reposition the dragger back to caret
        onStateChanged: {
            if (state === "") {
                draggedItem.moveToCaret();
            }
        }

        /*
          Mouse area to turn on dragging or selection mode when pressed
          on the handler area. The drag mode is turned off when the drag
          gets inactive or when the LeftButton is released.
          */
        MouseArea {
            objectName: cursorItem.positionProperty + "_activator"
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            preventStealing: true
            enabled: parent.width && parent.height && parent.visible

            onPressed: {
                draggedItem.moveToCaret(mouse.x, mouse.y);
                draggedItem.state = "dragging";
            }
            Ubuntu.Mouse.forwardTo: [dragger]
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
            Ubuntu.Mouse.onReleased: {
                if (!dragger.drag.active) {
                    draggedItem.state = "";
                }
            }
        }

        // aligns the draggedItem to the caret and resets the dragger
        function moveToCaret(cx, cy) {
            if (cx === undefined && cy === undefined) {
                cx = mappedCursorPosition("x") + caretX;
                cy = mappedCursorPosition("y") + caretY;
            } else {
                // move mouse position to caret
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
                var dx = dragger.thumbStartX + dragger.dragAmountX + handler.flickable.contentX;
                var dy = dragger.thumbStartY + dragger.dragAmountY + handler.flickable.contentY;
                // consider only the x-distance because of the overlays
                dx -= handler.frameDistance.x;
                handler.positionCaret(positionProperty, dx, dy);
            }
        }
    }
    MouseArea {
        id: dragger
        objectName: cursorItem.positionProperty + "_dragger"
        // fill the entire component area
        parent: handler.main
        anchors.fill: parent
        hoverEnabled: true
        preventStealing: drag.active
        enabled: draggedItem.enabled && draggedItem.state === "dragging" && QuickUtils.touchScreenAvailable

        property int thumbStartX
        property int dragStartX
        property int dragAmountX: dragger.drag.target.x - dragStartX
        property int thumbStartY
        property int dragStartY
        property int dragAmountY: dragger.drag.target.y - dragStartY

        function resetDrag() {
            thumbStartX = mappedCursorPosition("x");
            thumbStartY = mappedCursorPosition("y");
            dragStartX = drag.target ? drag.target.x : 0;
            dragStartY = drag.target ? drag.target.y : 0;
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

    // fake cursor, caret is reparented to it to avoid caret clipping
    Item {
        id: fakeCursor
        parent: handler.main
        width: cursorItem.width
        height: cursorItem.height
        //reparent caret to it
        Component.onCompleted: if (caret) caret.parent = fakeCursor

        x: mappedCursorPosition("x")
        y: mappedCursorPosition("y")

        // manual clipping: the caret should be visible only while the cursor's
        // top/bottom falls into the text area
        visible: {
            if (!caret || !cursorItem.visible || cursorItem.opacity < 1.0)
                return false;

            var leftTop = Qt.point(x - handler.frameDistance.x, y + handler.frameDistance.y + handler.lineSpacing);
            var rightBottom = Qt.point(x - handler.frameDistance.x, y + height - handler.frameDistance.y - handler.lineSpacing);
            return (handler.visibleArea.contains(leftTop) || handler.visibleArea.contains(rightBottom));
        }
    }
}
