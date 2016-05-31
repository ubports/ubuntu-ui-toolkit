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
 *
 */

import QtQuick 2.4
import Ubuntu.Components 1.3

Item {

    function reset() { launcher.y = -launcher.height }

    Rectangle {
        id: launcher
        color: "blue"
        width: parent.width
        height: units.gu(15)
        x: 0
        y: followDragArea()

        function followDragArea() {
            return swipeArea.distance < height ? -height + swipeArea.distance : 0
        }
    }

    Rectangle {
        id: dragAreaRect
        opacity: swipeArea.dragging ? 0.5 : 0.0
        color: "green"
        anchors.fill: swipeArea
    }

    SwipeArea {
        id: swipeArea
        objectName: "vpDragArea"

        height: units.gu(5)

        direction: SwipeArea.Downwards

        onDraggingChanged: {
            if (dragging) {
                launcher.y = Qt.binding(launcher.followDragArea)
            }
        }

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
    }

    SwipeArea {
        id: swipeAreaNoGrab
        objectName: "vpDragAreaNoGrab"

        grabGesture: false
        height: swipeArea.height
        direction: swipeArea.direction
        anchors {
            left: swipeArea.left
            right: swipeArea.right
            top: swipeArea.top
        }
    }

    Label {
        text: "Downwards"
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: units.gu(1)
    }
}
