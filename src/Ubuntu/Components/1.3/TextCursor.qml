/*
 * Copyright 2015 Canonical Ltd.
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
import Ubuntu.Components 1.3 as Ubuntu
import Ubuntu.Components.Popups 1.3

Ubuntu.StyledItem {
    id: cursorItem

    height: cursorRectangle.height

    /*
      Property holding the text input's cursor position property. Can be one of
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

    styleName: "TextCursorStyle"

    objectName: "textCursor"
    //Caret instance from the style.
    property Item caret: __styleInstance.caret
    property int absX: {
        return fakeCursor.parent.mapFromItem(handler.main, cursorItem.x, cursorItem.y).x
    }
    property int absY: {
        // Take parent flickable movement into account
        var flickable = handler.main;
        do {
            flickable = flickable.parent;
        } while (flickable && !flickable.contentY && flickable != fakeCursor.parent);
        return fakeCursor.parent.mapFromItem(handler.main, cursorItem.x, cursorItem.y).y
    }

    // Returns "x" or "y" relative to the item handlers are a child of
    function mappedCursorPosition(coordinate) {
        var cpos = cursorItem["abs" + coordinate.toUpperCase()];
        cpos += handler.frameDistance[coordinate];
        cpos += handler.input[coordinate];
        cpos -= handler.flickable["content" + coordinate.toUpperCase()];
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

    Binding {
        target: handler
        property: "cursorPositionCursor"
        value: cursorItem
    }

    function openPopover() {
        if (!visible
         || opacity === 0.0
         || dragger.dragActive) {
            return;
        }

        if (handler.popover != null) {
            PopupUtils.close(handler.popover);
        }

        var component = handler.main.popover;
        if (component === undefined)
            component = Qt.resolvedUrl("TextInputPopover.qml");

        // if the cursor is out of the visible viewport, anchor the
        // contextual menu to the input field
        var anchor = caret.visible ? draggedItem : handler.main
        var popup = PopupUtils.open(component, anchor, {
            "target": handler.main,
        });
        // hide the arrow
        popup.__foreground.direction = "none";
        cursorItem.Component.onDestruction.connect(popup.__closePopup);
        contextMenuVisible = true;
        popup.onVisibleChanged.connect(contextMenuHidden.bind(undefined, popup));
        // do not grab focus!
        popup.__foreground.activeFocusOnPress = false;
        handler.popover = popup;
    }

    visible: handler.main.cursorVisible &&
     !(positionProperty === "cursorPosition" && handler.main.selectedText !== "")

    // cursor visual loader
    Loader {
        id: cursorLoader
        sourceComponent: cursorDelegate
        height: parent.height
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
         && handler.main.text !== ""
    }
    property bool typing: false
    property bool contextMenuVisible: false
    property bool readOnly: handler.main.readOnly
    function contextMenuHidden(p) {
        contextMenuVisible = false
    }

    //dragged item
    property Item draggedItem: Item {
        objectName: cursorItem.positionProperty + "_draggeditem"
        width: caret.width + units.gu(4)
        onWidthChanged: draggedItem.moveToCaret()
        height: caret.height + units.gu(4)
        parent: fakeCursor.parent
        visible: caret.visible

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
            cursorShape: Qt.IBeamCursor
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
            Ubuntu.Mouse.enabled: enabled

            // Visible touch target area for debugging purposes
            Rectangle {
                anchors.fill: parent
                color: 'red'
                opacity: 0.1
                visible: false // draggedItemMouseArea.enabled
            }
        }

        // aligns the draggedItem to the caret and resets the dragger
        function moveToCaret() {
            if (!caret) {
                return;
            }
            draggedItem.x = fakeCursor.x - draggedItem.width / 2;
            draggedItem.y = fakeCursor.y + caret.y - caret.height / 2;
        }
        // positions caret to the dragged position
        function positionCaret() {
            if (dragger.dragActive) {
                var dx = dragger.dragStartX + dragger.dragAmountX + handler.flickable.contentX;
                var dy = dragger.dragStartY + dragger.dragAmountY + handler.flickable.contentY;
                dx -= handler.frameDistance.x;
                dy -= handler.frameDistance.y;
                dy -= draggedItem.height / 2;
                handler.positionCaret(positionProperty, dx, dy);
            }
        }
    }
    MouseArea {
        id: dragger
        objectName: cursorItem.positionProperty + "_dragger"
        cursorShape: Qt.IBeamCursor
        // fill the entire component area
        parent: handler.main
        anchors.fill: parent
        enabled: draggedItemMouseArea.enabled && draggedItemMouseArea.pressed && caret.visible
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
        objectName: positionProperty + "FakeCursor"
        parent: QuickUtils.rootItem(handler.main)
        width: cursorItem.width
        height: cursorItem.height
        Component.onCompleted: caret.parent = fakeCursor

        x: mappedCursorPosition("x")
        y: mappedCursorPosition("y")
        onXChanged: draggedItem.moveToCaret()
        onYChanged: draggedItem.moveToCaret()

        // manual clipping: the caret should be visible only while the cursor's
        // top/bottom falls into the text area
        visible: {
            if (!caret || !cursorItem.visible || cursorItem.opacity < 1.0)
                return false;

            var pos = handler.main.mapFromItem(fakeCursor.parent, fakeCursor.x, fakeCursor.y);
            var leftTop = Qt.point(pos.x - handler.frameDistance.x, pos.y + handler.frameDistance.y + handler.lineSpacing);
            var rightBottom = Qt.point(pos.x - handler.frameDistance.x, pos.y + height - handler.frameDistance.y - handler.lineSpacing);
            return (handler.visibleArea.contains(leftTop) || handler.visibleArea.contains(rightBottom));
        }
    }
}
