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
    implicitWidth: 2*thumb.width + 3*thumb.spacing
    implicitHeight: thumb.height + 2*thumb.spacing
    opacity: item.enabled ? 1.0 : 0.5

    UbuntuShape {
        id: background
        anchors.fill: parent
        color: StyleUtils.itemStyleProperty("backgroundColor", Qt.rgba(0.5, 0.5, 0.5, 0.5))
        gradientColor: "transparent"

        UbuntuShape {
            id: thumb

            // Thumb spacing is used a lot, so import it here once
            property real spacing: StyleUtils.itemStyleProperty("thumbSpacing", units.dp(1))

            width: StyleUtils.itemStyleProperty("thumbHeight", units.gu(4))
            height: StyleUtils.itemStyleProperty("thumbWidth", units.gu(4))
            x: item.checked ? rightThumbPosition.x : leftThumbPosition.x
            y: leftThumbPosition.y

            color: item.checked ? StyleUtils.itemStyleProperty("checkedThumbColor", "grey")
                                : StyleUtils.itemStyleProperty("uncheckedThumbColor", "grey")
            gradientColor: "transparent"

            Behavior on x { NumberAnimation { duration: 100; easing.type: Easing.OutQuad } }
            Behavior on color { ColorAnimation { duration: 100; easing.type: Easing.OutQuad } }
        }

        Item {
            id: leftThumbPosition
            anchors {
                left: parent.left
                top: parent.top
                leftMargin: thumb.spacing
                topMargin: thumb.spacing
            }
            height: thumb.height
            width: thumb.width

            Image {
                anchors.centerIn: parent
                opacity: item.checked ? StyleUtils.itemStyleProperty("unselectedImageOpacity", 1.0)
                                      : StyleUtils.itemStyleProperty("selectedImageOpacity", 1.0)
                source: StyleUtils.itemStyleProperty("crossSource", "")
            }
        }

        Item {
            id: rightThumbPosition
            anchors {
                right: parent.right
                top: parent.top
                rightMargin: thumb.spacing
                topMargin: thumb.spacing
            }
            height: thumb.height
            width: thumb.width

            Image {
                anchors.centerIn: parent
                opacity: item.checked ? StyleUtils.itemStyleProperty("selectedImageOpacity", 1.0)
                                      : StyleUtils.itemStyleProperty("unselectedImageOpacity", 1.0)
                source: StyleUtils.itemStyleProperty("checkMarkSource", "")
            }
        }
    }
}
