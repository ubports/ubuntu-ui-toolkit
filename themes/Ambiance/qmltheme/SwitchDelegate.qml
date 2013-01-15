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
import Ubuntu.Components 0.1

Item {
    anchors.fill: parent
    opacity: item.enabled ? 1.0 : 0.5

    implicitWidth: 2*thumb.width + 3*thumb.spacing
    implicitHeight: thumb.height + 2*thumb.spacing

    UbuntuShape {
        id: background

        anchors.fill: parent
        color: StyleUtils.itemStyleProperty("backgroundColor")
        gradientColor: "transparent"

        UbuntuShape {
            id: thumb

            // Thumb spacing is used a lot, so import it here once
            property real spacing: StyleUtils.itemStyleProperty("thumbSpacing", units.dp(1))

            width: StyleUtils.itemStyleProperty("thumbHeight", units.gu(4))
            height: StyleUtils.itemStyleProperty("thumbWidth", units.gu(4))

            y: background.y + thumb.spacing
            x: item.checked ? rightItem.x : leftItem.x
            color: item.checked ? StyleUtils.itemStyleProperty("checkedThumbColor")
                                   : StyleUtils.itemStyleProperty("uncheckedThumbColor")
            gradientColor: "transparent"

            Behavior on x { NumberAnimation { duration: 100; easing.type: Easing.OutQuad } }
            Behavior on color { ColorAnimation { duration: 100; easing.type: Easing.OutQuad } }
        }

        Item {
            id: leftItem
            anchors {
                left: parent.left
                top: parent.top
                leftMargin: thumb.spacing
                topMargin: thumb.spacing
            }
            height: thumb.height
            width: thumb.width

            Image {
                id: crossImage
                anchors.centerIn: parent
                opacity: item.checked ? StyleUtils.itemStyleProperty("unselectedImageOpacity")
                                      : StyleUtils.itemStyleProperty("selectedImageOpacity")

                source: StyleUtils.itemStyleProperty("crossSource")
            }
        }

        Item {
            id: rightItem
            anchors {
                right: parent.right
                top: parent.top
                rightMargin: thumb.spacing
                topMargin: thumb.spacing
            }
            height: thumb.height
            width: thumb.width

            Image {
                id: checkMarkImage
                anchors.centerIn: parent
                opacity: item.checked ? StyleUtils.itemStyleProperty("selectedImageOpacity")
                                      : StyleUtils.itemStyleProperty("unselectedImageOpacity")
                source: StyleUtils.itemStyleProperty("checkMarkSource")
            }
        }
    }
}
