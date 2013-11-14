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
import Ubuntu.Components.ListItems 0.1

PickerStyle {
    id: styleItem
//    frameWidth: 0
    backgroundColor: "#00000000"
    backgroundOpacity: 0.2
    overlay: false

    highlight: highlightComponent
    Component {
        id: highlightComponent
        Rectangle {
            color: "white"
            width: parent ? parent.width : 0
            height: (parent && parent.currentItem) ? parent.currentItem.height : units.gu(4);
            ThinDivider { anchors.top: parent.top }
            ThinDivider { anchors.top: parent.bottom }

            Magnifier {
                anchors.fill: parent
                targetWidth: parent.width
                targetHeight: parent.height
//                posX: targetWidth / 2
//                posY: targetHeight / 2
                source: shaderSource
            }
            ShaderEffectSource {
                id: shaderSource
                anchors.fill: parent
                sourceItem: highlightItem
            }
        }
    }
}
