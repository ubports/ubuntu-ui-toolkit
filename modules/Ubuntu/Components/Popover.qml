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
import "popoverUtils.js" as PopoverUtils

Item {
    id: rootArea
    anchors.fill: parent ? parent : undefined
    default property alias container: containerItem.data
    property alias caller: popover.caller;

    onWidthChanged: internal.detectScreen()
    onHeightChanged: internal.detectScreen()

    Item {
        // TODO: use proper theming for this
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

    QtObject {
        id: internal

        property bool smallScreen: false;
        // TODO: add property for screen orientation

        function detectScreen() {
            if (Math.min(rootArea.width, rootArea.height) <= popover.requestedWidth + 2*theme.edgeMargins) {
                smallScreen = true;
            } else {
                smallScreen = false;
            }
            popover.updatePosition();
        }
    }

    Item {
        id: popover

        width: Math.min(rootArea.width, requestedWidth)
        height: MathUtils.clamp(containerItem.totalHeight, minHeight, maxHeight)

        property real maxHeight: rootArea ? 3*rootArea.height/4 : Number.MAX_VALUE
        property real minHeight: units.gu(32)
        property real requestedWidth: units.gu(40)

        property Item caller

        onWidthChanged: updatePosition()
        onHeightChanged: updatePosition()
        onCallerChanged: updatePosition()

        function updatePosition() {
            var pos = new PopoverUtils.Positioning(popover, rootArea, caller, theme.edgeMargins, theme.callerMargins);

            var coords;
            if (internal.smallScreen || !caller) {
                coords = pos.center();
            } else {
                coords = pos.auto();
            }

            popover.x = coords.x;
            popover.y = coords.y;
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
