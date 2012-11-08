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

    Background {
        dim: false
        ephemeral: true
    }

    Foreground {
        width: Math.min(units.gu(40), popover.width)
        height: MathUtils.clamp(childrenRect.height, units.gu(40), 3*popover.height/4)

        // TODO: Make height of Foreground depend on containerItem height + margins?
        // TODO: make item after testing.
        Rectangle {
            id: containerItem
            color: "transparent"

            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
                margins: units.gu(2)
            }

            height: childrenRect.height + anchors.margins //anchors.topMargin + anchors.bottomMargin
        }
    }
}
