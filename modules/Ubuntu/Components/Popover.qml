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

    width: units.gu(40)
    height: Math.max(minHeight, Math.min(containerLayout.totalHeight, maxHeight))

    property real maxHeight: overlay ? 3*overlay.height/4 : Number.MAX_VALUE
    property real minHeight: units.gu(32)

    property Item overlay
    property Item caller

    onCallerChanged: updatePosition()
    onOverlayChanged: updatePosition()

    default property alias container: containerLayout.data


    // priority: above, beside, below
    property string relativePosition: "above"

    function updatePosition() {
        if (!overlay || !caller) return;
        popover.x = caller.x - units.gu(5);
        popover.y = caller.y + units.gu(5);
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
