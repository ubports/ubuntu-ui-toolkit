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

    //    width: units.gu(40)
    //    height: Math.max(minHeight, Math.min(containerLayout.totalHeight, maxHeight))

    // TODO: special case for phone screen
    width: units.gu(40)
    height: MathUtils.clamp(containerItem.totalHeight, minHeight, maxHeight)

    property real maxHeight: overlay ? 3*overlay.height/4 : Number.MAX_VALUE
    property real minHeight: units.gu(32)

    property Item overlay
    property Item caller

    onHeightChanged: __updatePosition()
    onCallerChanged: __updatePosition()
    onOverlayChanged: {
        parent = overlay
        overlay.widthChanged.connect(popover.__updatePosition);
        overlay.heightChanged.connect(popover.__updatePosition);
        __updatePosition();
    }

    default property alias container: containerLayout.data

    /*!
      \preliminary
      The minimum distance that the popover must keep from the edges of the overlay
     */
    property real edgeMargins: units.gu(2)

    /*!
      \preliminary
      The distance to keep from the calling Item
     */
    property real callerMargins: units.gu(0.5)

    // priority: above, beside, below
    property string relativePosition: "above"

    function __updatePosition() {
        if (!overlay || !caller) return;

        // TODO: figure out what to do when the overlay is too small to fit the popover.
        //        if (popover.width > overlay.width) {
        //            __updatePositionCenterInOverlay();
        //            return;
        //        }

        //        if (popover.height > overlay.height) {
        //            __updatePositionCenterInOverlay();
        //            return;
        //        }

        var coords = __positionAuto();
        popover.x = coords.x;
        popover.y = coords.y;
    }

    function __positionAuto() {
        var minX = edgeMargins;
        var maxX = overlay.width - edgeMargins - popover.width;
        var minY = edgeMargins;
        var maxY = overlay.height - edgeMargins - popover.height;

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
            coords = MathUtils.clamp(coords.x, minX, maxX);
            return coords;
        }

        return __positionCenter();
    }

    function __fixHorizontalMargins() {
        // TODO: margins may be computed with min/max
        // check the left margin
        if (popover.x < edgeMargins) popover.x = edgeMargins;

        // check the right margin
        if (popover.x + popover.width > overlay.width - edgeMargins) popover.x = overlay.width - popover.width - edgeMargins;
    }

    function __fixVerticalMargins() {
        if (popover.y < edgeMargins) popover.y = edgeMargins;
        if (popover.y + popover.height > overlay.height - edgeMargins) popover.y = overlay.height - popover.height - edgeMargins;
    }

    function __positionAbove() {
        var coords = new Qt.point(0, 0);
        var topCenter = overlay.mapFromItem(caller, caller.width/2, 0);
        print("x, y = " + topCenter.x+", "+topCenter.y);
        print("popover.height = "+popover.height);
        coords.x = topCenter.x - popover.width/2;
        coords.y = topCenter.y - popover.height - callerMargins;
        return coords;
    }

    function __positionBelow() {
        var coords = new Qt.point(0, 0);
        var bottomCenter = overlay.mapFromItem(caller, caller.width/2, caller.height);
        coords.x = bottomCenter.x - popover.width/2;
        coords.y = bottomCenter.y + callerMargins;
        return coords;
    }

    function __positionLeft() {
        var coords = new Qt.point(0, 0);
        var leftCenter = overlay.mapFromItem(caller, 0, caller.height/2);
        coords.x = leftCenter.x - popover.width - callerMargins;
        coords.y = leftCenter.y - popover.height/2;
        return coords;
    }

    function __positionRight() {
        var coords = new Qt.point(0, 0);
        var rightCenter = overlay.mapFromItem(caller, caller.width, caller.height/2);
        coords.x = rightCenter.x + callerMargins;
        coords.y = rightCenter.y - popover.height/2;
        return coords;
    }

    function __positionCenter() {
        var coords = new Qt.point(0, 0);
        coords.x = overlay.width/2 - popover.width/2;
        coords.y = overlay.height/2 - popover.height/2;
        return coords;
    }

    // Center the popover. Only as a fallback when none of the
    // other positions work.
    function __updatePositionCenterInOverlay() {
        popover.x = overlay.width/2 - popover.width/2;
        popover.y = overlay.height/2 - popover.height/2;
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: "silver"
        opacity: 0.7
        radius: units.gu(2)
    }

    // Avoid mouse events being sent to any MouseAreas that are behind the popover
    MouseArea {
        anchors.fill: parent
    }

    Rectangle {
        id: containerItem
        color: "pink"

        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
            margins: units.gu(0)
        }

        height: containerLayout.totalHeight //childrenRect.height
        property real totalHeight: height

        Column {
            id: containerLayout
            property real totalHeight: height + anchors.topMargin + anchors.bottomMargin
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
                margins: units.gu(2)
            }
            onHeightChanged: print("container layout height: "+height)
            onTotalHeightChanged: print("container layout total height changed to: "+totalHeight)
        }
    }
}
