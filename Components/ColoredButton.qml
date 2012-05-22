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

import QtQuick 1.1

AbstractButton {
    id: button
    property color color: "blue"
    property color borderColor
    property int borderWidth: 2.0
    property real radius: 3.0
    property color pressColor: color
    property color hoverColor: color

    Rectangle {
        id: background
        radius: parent.radius
        width: parent.width
        height: parent.height
        color: button.color
        border.color: parent.borderColor
        border.width: parent.borderWidth
    } // background

    states: [
        State {
            name: "default"
            PropertyChanges { target: background; color: button.color }
        },
        State {
            name: "pressed"
            PropertyChanges { target: background; color: button.pressColor }
        },
        State {
            name: "hovered"
            PropertyChanges { target: background; color: button.hoverColor }
        }
    ]
}
