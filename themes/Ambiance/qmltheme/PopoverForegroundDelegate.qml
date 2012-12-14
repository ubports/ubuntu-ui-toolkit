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
    anchors {
        left: parent ? parent.left : undefined
        right: parent ? parent.right : undefined
        top: parent ? parent.top : undefined
    }
    height: Math.min(outer.height, item.maxHeight)
    property alias contentItem: inner

    // Avoid mouse events being sent to any MouseAreas that are behind the popover
    MouseArea {
        anchors.fill: parent
    }

    Rectangle {
        id: outer
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: inner.height + 2*inner.anchors.margins

        color: "white"

        Rectangle {
            id: inner
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }
            color: "white"
            height: childrenRect.height
        }
    }
}
