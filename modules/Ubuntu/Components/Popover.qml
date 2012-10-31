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
    id: popover

    // TODO: detect special case for phone screen
    width: Math.min(area.width, units.gu(40)) //smallScreen ? area.width : units.gu(40)
    height: MathUtils.clamp(containerItem.totalHeight, minHeight, maxHeight)

    property real maxHeight: area ? 3*area.height/4 : Number.MAX_VALUE
    property real minHeight: units.gu(32)
    property real requestedWidth: units.gu(40)

    // TODO: detect not only screen size but also orientation
    property bool smallScreen: false

    property Item area
    property Item caller

    function __detectScreen() {
        if (!area) {
            smallScreen = false;
            //            landscape = false;
            return;
        }

        if (Math.min(area.width, area.height) <= requestedWidth + 2*edgeMargins) {
            smallScreen = true;
            //            landscape = area.width > area.height;
        } else {
            smallScreen = false;
            //            landscape = false //ignored
        }

        __updatePosition();
    }


    onHeightChanged: __updatePosition()
    onCallerChanged: __detectScreen()
    onAreaChanged: {
        parent = area
        area.widthChanged.connect(popover.__detectScreen);
        area.heightChanged.connect(popover.__detectScreen);
        __detectScreen();
    }
    Component.onCompleted: __findRoot()

    function __findRoot() {
        var root = parent;
        do {
            root = root.parent;
        } while (!root.parent)

        area = root;
    }

    default property alias container: containerItem.data

    /*!
      \preliminary
      The minimum distance that the popover must keep from the edges of the area
     */
    property real edgeMargins: units.gu(2)

    /*!
      \preliminary
      The distance to keep from the calling Item
     */
    property real callerMargins: units.gu(0.5)

    function __updatePosition() {
        if (!area || !caller) return;

        var coords = __positionAuto();
        popover.x = coords.x;
        popover.y = coords.y;
    }

    function __positionAuto() {
        if (smallScreen) return __positionCenter();

        var minX = edgeMargins;
        var maxX = area.width - edgeMargins - popover.width;
        var minY = edgeMargins;
        var maxY = area.height - edgeMargins - popover.height;

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
        var topCenter = area.mapFromItem(caller, caller.width/2, 0);
        coords.x = topCenter.x - popover.width/2;
        coords.y = topCenter.y - popover.height - callerMargins;
        return coords;
    }

    function __positionBelow() {
        var coords = new Qt.point(0, 0);
        var bottomCenter = area.mapFromItem(caller, caller.width/2, caller.height);
        coords.x = bottomCenter.x - popover.width/2;
        coords.y = bottomCenter.y + callerMargins;
        return coords;
    }

    function __positionLeft() {
        var coords = new Qt.point(0, 0);
        var leftCenter = area.mapFromItem(caller, 0, caller.height/2);
        coords.x = leftCenter.x - popover.width - callerMargins;
        coords.y = leftCenter.y - popover.height/2;
        return coords;
    }

    function __positionRight() {
        var coords = new Qt.point(0, 0);
        var rightCenter = area.mapFromItem(caller, caller.width, caller.height/2);
        coords.x = rightCenter.x + callerMargins;
        coords.y = rightCenter.y - popover.height/2;
        return coords;
    }

    function __positionCenter() {
        var coords = new Qt.point(0, 0);
        coords.x = area.width/2 - popover.width/2;
        coords.y = area.height/2 - popover.height/2;
        return coords;
    }

    // Center the popover. Only as a fallback when none of the
    // other positions work.
    function __updatePositionCenterInarea() {
        popover.x = area.width/2 - popover.width/2;
        popover.y = area.height/2 - popover.height/2;
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: "silver"
        opacity: 0.8
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
