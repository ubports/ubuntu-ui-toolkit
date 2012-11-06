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

PopupBase {
    id: popover
    default property alias container: containerItem.data

    Background {
        dim: false
        ephemeral: true
    }

    Foreground {
        maxHeight: 3*popover.height/4
        minHeight: units.gu(40)
        requestedWidth: units.gu(40)

        // TODO: make item after testing.
        Rectangle {
            id: containerItem
            color: "white"

            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
                margins: units.gu(2)
            }

            height: childrenRect.height + anchors.topMargin + anchors.bottomMargin
        }
    }
}
