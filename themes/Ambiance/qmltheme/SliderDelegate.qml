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

/*
  This delegate is styled using the following properties:
  - backgroundColor: color for the slider bar
  - thumbColor: color for the thumb
  - thumbSpacing: spacing between the thumb and the bar
  - thumbLabelShown: controls visibility of the thumb label
  */

Item {
    id: main
    anchors.fill: parent

    property real normalizedValue: item.__normalizedValue

    property real thumbSpacing: StyleUtils.itemStyleProperty("thumbSpacing", units.dp(2))
    property real thumbSpace: backgroundShape.width - (2.0 * thumbSpacing + thumbWidth)
    property real thumbWidth: item.height - thumbSpacing

    UbuntuShape {
        id: backgroundShape
        Component.onCompleted: item.__background = backgroundShape

        anchors.fill: parent
        color: StyleUtils.itemStyleProperty("backgroundColor", "white")
    }

    UbuntuShape {
        id: thumbShape
        Component.onCompleted: item.__thumb = thumbShape

        x: backgroundShape.x + thumbSpacing + normalizedValue * thumbSpace
        y: backgroundShape.y + thumbSpacing
        width: thumbWidth
        height: backgroundShape.height - (2.0 * thumbSpacing)
        color: StyleUtils.itemStyleProperty("thumbColor", "darkgray")

        Label {
            id: thumbValue
            anchors {
                verticalCenter: thumbShape.verticalCenter
                left: thumbShape.left
                right: thumbShape.right
            }
            horizontalAlignment: Text.AlignHCenter
            text: item.formatValue(MathUtils.clamp(item.__liveValue, item.minimumValue, item.maximumValue))
        }
    }
}
