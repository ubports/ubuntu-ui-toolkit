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

import QtQuick 2.4
import Ubuntu.Components 1.2 as Ubuntu
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
    style: styleSet.createStyleComponent(handler.textCursorStyle(positionProperty), cursorItem);

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
        target: handler
        onPressAndHold: {
            // open context menu only for cursorPosition or selectionStart as to
            // ensure that only one popover gets opened
            if (positionProperty !== "selectionEnd") {
                openPopover(fromTouch);
            }
        }
        onTextModified: typing = true
        onTap: {
            typing = false
            if (handler.popover != null) {
                PopupUtils.close(handler.popover);
            }
        }
    }

    function openPopover() {
        if (!visible || opacity === 0.0 || dragger.dragActive) {
            return;
        }

        if (contextMenuVisible) {
            return;
        }

        if (handler.popover != null) {
            PopupUtils.close(handler.popover);
        }

        var component = handler.main.popover;
        if (component === undefined)
            component = Qt.resolvedUrl("TextInputPopover.qml");

        var popup;
        if (fakeCursor.visible) {
            popup = PopupUtils.open(component, cursorItem, {
                "target": handler.main,
            });
        } else {
            // if the cursor is out of the visible viewport, anchor the
            // contextual menu to the input field
            popup = PopupUtils.open(component, handler.main, {
                "target": handler.main,
            });
            cursorItem.Component.onDestruction.connect(popup.__closePopup);
        }
        contextMenuVisible = true;
        popup.onVisibleChanged.connect(contextMenuHidden.bind(undefined, popup));
        // do not grab focus!
        popup.__foreground.activeFocusOnPress = false;
        handler.popover = popup;
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

    onXChanged: if (!draggedItemMouseArea.pressed) draggedItem.moveToCaret()
    onYChanged: if (!draggedItemMouseArea.pressed) draggedItem.moveToCaret()
    Component.onCompleted: draggedItem.moveToCaret()

    //dragged item
    Item {
        id: draggedItem
        objectName: cursorItem.positionProperty + "_draggeditem"
        width: caret ? units.gu(4) : 0
        height: caret ? cursorItem.height : 0
        parent: handler.main
        visible: cursorItem.visible && (cursorItem.opacity > 0.0) && QuickUtils.touchScreenAvailable

        /*
          Mouse area to turn on dragging or selection mode when pressed
          on the handler area. The drag mode is turned off when the drag
          gets inactive or when the LeftButton is released.
          */
        MouseArea {
            id: draggedItemMouseArea
            objectName: cursorItem.positionProperty + "_activator"
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            preventStealing: true
            enabled: parent.width && parent.height && parent.visible && !handler.doubleTapInProgress
            onPressedChanged: {
                if (!pressed) {
                    // when the dragging ends, reposition the dragger back to caret
                    draggedItem.moveToCaret();
                }
            }
            Ubuntu.Mouse.forwardTo: [dragger]
            Ubuntu.Mouse.onClicked: openPopover()
            Ubuntu.Mouse.onPressAndHold: {
                handler.main.selectWord();
                handler.pressAndHold(-1, false);
            }
            Ubuntu.Mouse.onDoubleClicked: handler.main.selectWord()
            Ubuntu.Mouse.clickAndHoldThreshold: units.gu(2)
            Ubuntu.Mouse.enabled: enabled
        }

        // aligns the draggedItem to the caret and resets the dragger
        function moveToCaret() {
            draggedItem.x = mappedCursorPosition("x") - draggedItem.width / 2;
            draggedItem.y = mappedCursorPosition("y");
        }
        // positions caret to the dragged posinotion
        function positionCaret() {
            if (dragger.dragActive) {
                var dx = dragger.dragStartX + dragger.dragAmountX + handler.flickable.contentX;
                var dy = dragger.dragStartY + dragger.dragAmountY + handler.flickable.contentY;
                // consider only the x-distance because of the overlays
                dx -= handler.frameDistance.x;
                dy -= handler.frameDistance.y;
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
        enabled: draggedItemMouseArea.enabled && draggedItemMouseArea.pressed && QuickUtils.touchScreenAvailable
        onEnabledChanged: {
            if (enabled) {
                dragAmountX = 0;
                dragAmountY = 0;
                firstMouseXChange = true;
                firstMouseYChange = true;
            } else {
                dragActive = false;
            }
        }

        property int dragStartX
        property int dragAmountX
        property int dragStartY
        property int dragAmountY
        property bool dragActive: false
        property int dragThreshold: units.gu(2)
        property bool firstMouseXChange: true
        property bool firstMouseYChange: true

        onMouseXChanged: {
            if (firstMouseXChange) {
                dragStartX = mouseX;
                firstMouseXChange = false;
            } else {
                var amount = mouseX - dragStartX;
                if (Math.abs(amount) >= dragThreshold) {
                    dragActive = true;
                }
                if (dragActive) {
                    dragAmountX = amount;
                    draggedItem.positionCaret();
                }
            }
        }

        onMouseYChanged: {
            if (firstMouseYChange) {
                dragStartY = mouseY;
                firstMouseYChange = false;
            } else {
                var amount = mouseY - dragStartY;
                if (Math.abs(amount) >= dragThreshold) {
                    dragActive = true;
                }
                if (dragActive) {
                    dragAmountY = amount;
                    draggedItem.positionCaret()
                }
            }
        }

        onDragActiveChanged: {
            // close contextual menu when dragging and reopen it at the end of the drag
            if (dragActive) {
                if (handler.popover != null) {
                    PopupUtils.close(handler.popover);
                }
            } else {
                handler.pressAndHold(-1, false);
            }
        }
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
