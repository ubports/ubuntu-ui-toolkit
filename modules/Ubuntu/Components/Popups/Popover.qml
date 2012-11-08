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

    // private
    function updatePosition(item) {
        // FIXME: assuming that popover has a caller.
        // FIXME: if the edgeMargins are larger than caller width/height+callerMargins,
        //          then we can run into problems.
        var pos = new PopupUtils.Positioning(item, popover, caller, edgeMargins, callerMargins);
        var coords = pos.auto();
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
        height: MathUtils.clamp(containerItem.totalHeight, units.gu(32), maxHeight)

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
            property real totalHeight: height + anchors.topMargin + anchors.bottomMargin
        }

        onWidthChanged: popover.updatePosition(foreground)
        onHeightChanged: popover.updatePosition(foreground)
    }

//    onCallerChanged: updatePosition(foreground)
    onWidthChanged: updatePosition(foreground)
    onHeightChanged: updatePosition(foreground)
}
