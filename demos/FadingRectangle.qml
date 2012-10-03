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

Rectangle {
    id: rect
    color: "#eeeeee"
    property bool selected: false

    opacity: selected ? 1 : 0
    anchors {
        top: parent ? parent.top : undefined
        left: parent ? parent.left : undefined
    }
    width: childrenRect.width
    height: childrenRect.height

    states: [ State {
            name: "selected"
            when: rect.selected
            PropertyChanges {
                target: rect
                opacity: 1
                z: 0
            }
        }, State {
            name: "unselected"
            when: !rect.selected
            PropertyChanges {
                target: rect
                opacity: 0
                z: -1
            }
        }
    ]
    transitions: Transition {
        NumberAnimation {
            properties: "opacity"
            duration: 200
        }
    }
}
