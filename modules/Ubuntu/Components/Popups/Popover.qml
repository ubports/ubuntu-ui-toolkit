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
import "../mathUtils.js" as MathUtils

PopupBase {
    id: popover
    default property alias container: containerItem.data

    // theme
    property real edgeMargins: units.gu(2)
    property real callerMargins: units.gu(0.5)

    property bool portrait: width < height
//    property bool smallScreen: Math.min(width, height) < units.gu(40)

    // private
    function updatePosition(item) {
        // FIXME: assuming that popover has a caller.
        // FIXME: if the edgeMargins are larger than caller width/height+callerMargins,
        //          then we can run into problems.

        var pos = new PopupUtils.Positioning(item, popover, caller, edgeMargins, callerMargins);

        var coords = new Qt.point(0, 0);
        if (foreground.width >= popover.width - 2*edgeMargins) {
            // the popover uses (almost) the full width of the screen
            coords.x = pos.horizontalCenter();
            coords.y = pos.above();
            if (!pos.checkVerticalPosition(coords.y, 0, popover.height/4)) {
                coords.y = pos.below();
                if (!pos.checkVerticalPosition(coords.y, 0, popover.height/4)) {
                    // position at the top of the screen:
                    coords.y = 0;
                }
            }
        } else if (foreground.height >= popover.height - 2*edgeMargins) {
            // the popover uses (almost) the full height of the screen
            coords.y = pos.verticalCenter();
            coords.x = pos.left();
            if (!pos.checkHorizontalPosition(coords.x, 0, popover.width/4)) {
                coords.x = pos.right();
                if (!pos.checkHorizontalPosition(coords.x, 0, popover.width/4)) {
                    // position at the left of the screen
                    coords.x = 0;
                }
            }
        } else {
            // position with the following priorities: above, left, right, below.
            coords.y = pos.above();
            if (pos.checkVerticalPosition(coords.y, edgeMargins, 0)) {
                coords.x = pos.horizontalAlign();
            } else {
                coords.x = pos.left();
                if (pos.checkHorizontalPosition(coords.x, edgeMargins, 0)) {
                    coords.y = pos.verticalAlign();
                } else {
                    coords.x = pos.right();
                    if (pos.checkHorizontalPosition(coords.x, edgeMargins, 0)) {
                        coords.y = pos.verticalAlign();
                    } else {
                        coords.y = pos.below();
                        if (pos.checkVerticalPosition(coords.y, edgeMargins, 0)) {
                            coords.x = pos.horizontalAlign();
                        } else {
                            // not enough space on any of the sides to fit within the margins.
                            coords.x = pos.horizontalCenter();
                            coords.y = pos.verticalCenter();
                        }
                    }
                }
            }
        }
        item.x = coords.x;
        item.y = coords.y;
    }

    Background {
        dim: false
        ephemeral: true
    }

    Foreground {
        id: foreground

        property real maxWidth: portrait ? popover.width : popover.width * 3/4
        property real maxHeight: portrait ? popover.height * 3/4 : popover.height
        width: Math.min(units.gu(40), maxWidth)
        height: MathUtils.clamp(containerItem.height + containerItem.anchors.margins*2, units.gu(32), maxHeight)

        // TODO: Make height of Foreground depend on containerItem height + margins?
        // TODO: make item after testing.
        Rectangle {
            id: containerItem
            color: "white"

            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
                margins: units.gu(1)
            }

            height: childrenRect.height
        }

        onWidthChanged: popover.updatePosition(foreground)
        onHeightChanged: popover.updatePosition(foreground)
    }

//    onCallerChanged: updatePosition(foreground)
    onWidthChanged: updatePosition(foreground)
    onHeightChanged: updatePosition(foreground)
}
