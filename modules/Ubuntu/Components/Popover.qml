/*
 * Copyright (C) 2011-2012 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Lucas Beeler <lucas@yorba.org>
 */

import QtQuick 2.0

Item {
    id: popover

//    width: units.gu(40)
//    height: Math.max(minHeight, Math.min(containerLayout.totalHeight, maxHeight))

    width: __getWidth()
    height: __getHeight()

    function __getWidth() {
        if (!overlay) return units.gu(40);
        return Math.min(units.gu(40), overlay.width - 2*edgeMargins);
    }

    function __getHeight() {
        var h;
        if (!overlay) h = units.gu(40);
        else {
            h = Math.max(minHeight, Math.min(containerLayout.totalHeight, maxHeight));
            h = Math.min(h, overlay.height - 2*edgeMargins);
        }
        return h;
    }

    property real maxHeight: overlay ? 3*overlay.height/4 : Number.MAX_VALUE
    property real minHeight: units.gu(32)

    property Item overlay
    property Item caller

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

        __updatePositionAbove();
        if (popover.y < edgeMargins) __updatePositionLeft();
        if (popover.x < edgeMargins) __updatePositionRight();
        if (popover.x + popover.width > overlay.width - edgeMargins) __updatePositionBelow();
        if (popover.y + popover.height > overlay.height - edgeMargins) __updatePositionCenterInOverlay();
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

    function __updatePositionAbove() {
        var coords = new Qt.point(0,0);// = [];
        var topCenter = overlay.mapFromItem(caller, caller.width/2, 0);
        coords.x = topCenter.x - popover.width/2;
        coords.y = topCenter.y - popover.height;
        popover.x = coords.x;
        popover.y = coords.y;
        __fixHorizontalMargins()
    }

    function __updatePositionBelow() {
        var bottomCenter = overlay.mapFromItem(caller, caller.width/2, caller.height);
        popover.x = bottomCenter.x - popover.width/2;
        popover.y = bottomCenter.y;
        __fixHorizontalMargins();
    }

    function __updatePositionLeft() {
        var leftCenter = overlay.mapFromItem(caller, 0, caller.height/2);
        popover.x = leftCenter.x - popover.width;
        popover.y = leftCenter.y - popover.height/2;
        __fixVerticalMargins();
    }

    function __updatePositionRight() {
        var rightCenter = overlay.mapFromItem(caller, caller.width, caller.height/2);
        popover.x = rightCenter.x;
        popover.y = rightCenter.y - popover.height/2;
        __fixVerticalMargins();
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

    Column {
        id: containerLayout
        property real totalHeight: height + anchors.topMargin + anchors.bottomMargin
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
            margins: units.gu(2)
        }
    }
}
