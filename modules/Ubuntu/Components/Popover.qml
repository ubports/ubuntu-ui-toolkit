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
import "mathUtils.js" as MathUtils

Item {
    id: rootArea
    anchors.fill: parent ? parent : undefined
    default property alias container: containerItem.data
    property alias caller: popover.caller;

    onWidthChanged: popover.__detectScreen()
    onHeightChanged: popover.__detectScreen()

    Item {
        id: theme

        property real edgeMargins: units.gu(2)
        property real callerMargins: units.gu(0.5)
    }

    Rectangle {
        // darken the background
        anchors.fill: parent
        color: "black"
        opacity: 0.3
    }

    MouseArea {
        anchors.fill: parent

        onPressed: {
            // mouse events inside the popover area area captured by another mouse area below
            // so it is always okay to delete the rootArea here.
            rootArea.destroy();
        }
    }

    Item {
        id: popover

        // TODO: detect special case for phone screen
        width: Math.min(rootArea.width, units.gu(40))
        height: MathUtils.clamp(containerItem.totalHeight, minHeight, maxHeight)

        property real maxHeight: area ? 3*rootArea.height/4 : Number.MAX_VALUE
        property real minHeight: units.gu(32)
        property real requestedWidth: units.gu(40)

        // TODO: detect not only screen size but also orientation
        property bool smallScreen: false

        property Item area: rootArea
        property Item caller

        function __detectScreen() {
            if (Math.min(rootArea.width, rootArea.height) <= requestedWidth + 2*theme.edgeMargins) {
                smallScreen = true;
            } else {
                smallScreen = false;
            }
            __updatePosition();
        }

        onHeightChanged: __updatePosition()
        onCallerChanged: __detectScreen() // TODO: remove or call __updatePosition

        function __updatePosition() {
            var coords = __positionAuto();
            popover.x = coords.x;
            popover.y = coords.y;
        }

        function __positionAuto() {
            if (smallScreen || !caller) return __positionCenter();

            var minX = theme.edgeMargins;
            var maxX = rootArea.width - theme.edgeMargins - popover.width;
            var minY = theme.edgeMargins;
            var maxY = rootArea.height - theme.edgeMargins - popover.height;

            var coords = __positionAbove();
            if (coords.y >= minY) {
                coords.x = MathUtils.clamp(coords.x, minX, maxX);
                return coords;
            }

            coords = __positionLeft();
            if (coords.x >= minX) {
                coords.y = MathUtils.clamp(coords.y, minY, maxY);
                return coords;
            }

            coords = __positionRight();
            if (coords.x <= maxX) {
                coords.y = MathUtils.clamp(coords.y, minY, maxY);
                return coords;
            }

            coords = __positionBelow();
            if (coords.y <= maxY) {
                coords.x = MathUtils.clamp(coords.x, minX, maxX);
                return coords;
            }
            return __positionCenter();
        }

        function __positionAbove() {
            var coords = new Qt.point(0, 0);
            var topCenter = rootArea.mapFromItem(caller, caller.width/2, 0);
            coords.x = topCenter.x - popover.width/2;
            coords.y = topCenter.y - popover.height - theme.callerMargins;
            return coords;
        }

        function __positionBelow() {
            var coords = new Qt.point(0, 0);
            var bottomCenter = rootArea.mapFromItem(caller, caller.width/2, caller.height);
            coords.x = bottomCenter.x - popover.width/2;
            coords.y = bottomCenter.y + theme.callerMargins;
            return coords;
        }

        function __positionLeft() {
            var coords = new Qt.point(0, 0);
            var leftCenter = rootArea.mapFromItem(caller, 0, caller.height/2);
            coords.x = leftCenter.x - popover.width - theme.callerMargins;
            coords.y = leftCenter.y - popover.height/2;
            return coords;
        }

        function __positionRight() {
            var coords = new Qt.point(0, 0);
            var rightCenter = rootArea.mapFromItem(caller, caller.width, caller.height/2);
            coords.x = rightCenter.x + theme.callerMargins;
            coords.y = rightCenter.y - popover.height/2;
            return coords;
        }

        function __positionCenter() {
            var coords = new Qt.point(0, 0);
            coords.x = rootArea.width/2 - popover.width/2;
            coords.y = rootArea.height/2 - popover.height/2;
            return coords;
        }

        // Center the popover. Only as a fallback when none of the
        // other positions work.
        function __updatePositionCenterInarea() {
            popover.x = rootArea.width/2 - popover.width/2;
            popover.y = rootArea.height/2 - popover.height/2;
        }

        Rectangle {
            id: background
            anchors.fill: parent
            color: "silver"
            opacity: 0.9
            radius: units.gu(2)
        }

        // Avoid mouse events being sent to any MouseAreas that are behind the popover
        MouseArea {
            anchors.fill: parent
        }

        Rectangle {
            id: containerItem
            color: "transparent"

            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
                margins: units.gu(2)
            }

            height: containerLayout.totalHeight
            property real totalHeight: height + anchors.topMargin + anchors.bottomMargin
        }
    }
}
