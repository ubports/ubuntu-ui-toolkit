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
import "popoverUtils.js" as PopoverUtils

PopupBase {
    id: rootArea
    default property alias container: containerItem.data

    onCallerChanged: popover.updatePosition()

    Background {
        anchors.fill: rootArea
        onPressed: rootArea.destroy()
    }

    Foreground {
        id: popover

//        property real maxHeight: rootArea ? 3*rootArea.height/4 : Number.MAX_VALUE
//        property real minHeight: units.gu(40)
//        property real requestedWidth: units.gu(40)

        onWidthChanged: updatePosition()
        onHeightChanged: updatePosition()

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
