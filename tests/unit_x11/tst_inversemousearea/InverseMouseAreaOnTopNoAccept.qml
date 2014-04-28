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
import Ubuntu.Components 1.1

Rectangle {
    id: page
    width: 300
    height: 300
    color: "lightgray"

    objectName: "ROOT"

    Rectangle {
        id: label
        objectName: "RECT1"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        height: 40
        width: parent.width
        color: Qt.rgba(1, 0, 0, 0.4)

        MouseArea {
            objectName: "MA1"
            anchors.fill: parent
            z: 1
        }
    }

    Rectangle {
        anchors.top: label.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        height: 60
        width: parent.width
        color: Qt.rgba(0, 0, 1, 0.4)
        objectName: "RECT2"

        Button {
            id: button
            objectName: "button"
            x: 20
            y: 10
            text: "I'm a button, I do nothing."
        }

        MouseArea {
            objectName: "MA2"
            anchors.fill: parent
        }

        InverseMouseArea {
            objectName: "IMA"
            anchors.fill: button
            onPressed: mouse.accepted = false
        }
    }
}
