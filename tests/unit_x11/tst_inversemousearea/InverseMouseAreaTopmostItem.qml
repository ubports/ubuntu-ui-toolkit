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

//Item {
//    width: 300
//    height: 300
//    objectName: "ROOT"

//    Rectangle {
//        x: 10; y: 10
//        width: 100; height: 100
//        color: "blue"
//        InverseMouseArea {
//            anchors.fill: parent
//            objectName: "IMA"
//            topmostItem: true
//        }
//    }

//    Rectangle {
//        x: 110; y: 10
//        width: 100; height: 100
//        color: "red"
//        MouseArea {
//            anchors.fill: parent
//            objectName: "MA"
//        }
//    }
//}

Item {
    width: units.gu(40)
    height: units.gu(71)

    Rectangle {
        id: firstRect
        anchors {
            left: parent.left
            top: parent.top
            leftMargin: units.gu(10)
            topMargin: units.gu(10)
        }
        width: units.gu(15)
        height: width

        color: "blue"
        InverseMouseArea {
            anchors.fill: parent
            objectName: "IMA"
            topmostItem: true
            onPressed: print("IMA")
        }
    }

    Rectangle {
        anchors {
            left: firstRect.right
            top: firstRect.bottom
        }

        width: units.gu(10)
        height: width
        color: "red"
        MouseArea {
            anchors.fill: parent
            objectName: "MA"
            onPressed: print("MA")
        }
    }
}
