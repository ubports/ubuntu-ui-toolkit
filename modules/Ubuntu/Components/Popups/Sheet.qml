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
    id: sheet
    default property alias container: containerItem.data

    Background {
        dim: false
        ephemeral: true
    }

    Foreground {
//        maxHeight: sheet.height
//        minHeight: units.gu(40)

        // TODO minWidth: 50, maxWidth: sheet.width
//        requestedWidth: units.gu(60)

        width: MathUtils.clamp(childrenRect.width, units.gu(50), sheet.width)
        height: Math.min(units.gu(40), sheet.height)

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
