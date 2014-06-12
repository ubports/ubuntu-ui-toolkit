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
import QtQuick.Window 2.0
import Ubuntu.Components 1.1

Item{
    id: root
    property string log: ""
//    onLogChanged: print("LOG", log)
    width: units.gu(100)
    height: units.gu(100)

    Window {
        objectName: "isawindow"
        width: units.gu(50)
        height: units.gu(50)
        visible: true

        Item {
            id: clickArea
            objectName: "clickArea"
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }
            height: units.gu(10)
            Label {
                anchors.centerIn: parent
                text: "click me"
            }
        }

        Rectangle {
            id: rect
            color: "red"
            anchors {
                left: parent.left
                right: parent.right
                top: clickArea.bottom
                bottom: parent.bottom
            }
        }

        InverseMouseArea {
            objectName: "ima"
            id: ima
            anchors.fill: rect
            onClicked: {
                root.log = "IMA"
                rect.color = "green"
            }
        }
    }
}
