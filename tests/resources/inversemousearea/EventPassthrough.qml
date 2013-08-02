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
    width: units.gu(50)
    height: units.gu(50)

    Rectangle {
        color: "gray"
        id: nil
        width: units.gu(10)
        height: units.gu(10)

    }
    InverseMouseArea {
        anchors.fill: nil
        onClicked: color.color = "red" // FAIL
    }


    Rectangle {
        id: color
        anchors.fill: parent
        color: "blue"
        opacity: 0.5
//        MouseArea {
//            onClicked: parent.color = "green" // PASS
//            anchors.fill: parent
//        }
    }
}
