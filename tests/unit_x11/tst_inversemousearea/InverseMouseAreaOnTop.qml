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

Item {
    id: page
    width: 300
    height: 300

    property string log
    onLogChanged: print(log)

    Rectangle {
        id: label

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        height: 40
        width: parent.width
        color: Qt.rgba(1, 0, 0, 0.4)

        MouseArea {
            anchors.fill: parent
            z: 1
            onPressed: log = "MA1"
        }
    }

    Rectangle {
        anchors.top: label.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        height: 60
        width: parent.width
        color: Qt.rgba(0, 0, 1, 0.4)

        Button {
            id: button
            objectName: "button"
            x: 20
            y: 10
            text: "I'm a button, I do nothing."
        }

        InverseMouseArea {
            anchors.fill: button
            onPressed: log = "IMA"
        }

        MouseArea {
            anchors.fill: parent
            onPressed: log = "MA2"
        }
    }
}
