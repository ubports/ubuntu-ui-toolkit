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
  The default slider style consists of a bar and a thumb shape, where the thumb
  is having a label showing the actual value.

  This style is styled using the following properties:
  - backgroundColor: color for the slider bar
  - thumbColor: color for the thumb
  - thumbSpacing: spacing between the thumb and the bar
  - backgroundOpacity: transparency for the background
  - thumbOpacity: transparency for the thumb
  */

Item {
    id: main
    // styling properties

    property color backgroundColor: "#c0c0c0"
    property color thumbColor: "#bbbbbb"
    property real thumbSpacing: units.dp(2)
    property real backgroundOpacity: 0.1
    property real thumbOpacity: 1.0

    // visuals
    anchors.fill: parent

    // properties to be published:
    property Item bar: backgroundShape
    property Item thumb: thumbShape

    // private properties

    property real liveValue: SliderUtils.liveValue(styledItem)
    property real normalizedValue: SliderUtils.normalizedValue(styledItem)
    property real thumbSpace: backgroundShape.width - (2.0 * thumbSpacing + thumbWidth)
    property real thumbWidth: styledItem.height - thumbSpacing

    UbuntuShape {
        id: backgroundShape

        anchors.fill: parent
        color: main.backgroundColor
        opacity: main.backgroundOpacity
    }

    UbuntuShape {
        id: thumbShape

        x: backgroundShape.x + thumbSpacing + normalizedValue * thumbSpace
        y: backgroundShape.y + thumbSpacing
        width: thumbWidth
        height: backgroundShape.height - (2.0 * thumbSpacing)
        color: main.thumbColor
        opacity: main.thumbOpacity
    }

    Label {
        id: thumbValue
        anchors {
            verticalCenter: thumbShape.verticalCenter
            left: thumbShape.left
            right: thumbShape.right
        }
        horizontalAlignment: Text.AlignHCenter
        text: styledItem.formatValue(MathUtils.clamp(liveValue, styledItem.minimumValue, styledItem.maximumValue))
        fontSize: "medium"
        color: "white"
        font.weight: Font.Bold
    }
}
