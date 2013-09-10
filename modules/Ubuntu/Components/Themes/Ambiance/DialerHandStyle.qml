/*
 * Copyright 2013 Canonical Ltd.
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

Rectangle {
    id: handItem
    anchors.fill: parent

    radius: width / 2
    color: "#00000000"
    transformOrigin: Item.Center

    Rectangle {
        id: hand
        x: (parent.width - width) / 2
        y: (parent.height / 2) -  height
        width: styledItem.size.width
        height: styledItem.size.height + (styledItem.dialer.height - styledItem.dialer.handSpace) / 2
        onHeightChanged: print(height)
        radius: units.gu(1)
        color: styledItem.isHandVisible ? "white" : "#00000000"
        antialiasing: true
    }

    Behavior on rotation {
        enabled: !styledItem.draggable
        RotationAnimation { direction: RotationAnimation.Shortest }
    }
}
