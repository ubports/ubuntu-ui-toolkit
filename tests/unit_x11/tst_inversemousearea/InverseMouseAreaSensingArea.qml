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
import Ubuntu.Components 1.1

Item {
    width: 300
    height: 300

    MouseArea {
        objectName: "MA"
        anchors.fill: parent
    }

    Rectangle {
        id: blueRect
        width: 200
        height: 200
        anchors.centerIn: parent
        color: "blue"

        Rectangle {
            width: 100
            height: 100
            anchors.centerIn: parent
            color: "red"
            InverseMouseArea {
                objectName: "IMA"
                anchors.fill: parent
                sensingArea: blueRect
            }
        }
    }
}
