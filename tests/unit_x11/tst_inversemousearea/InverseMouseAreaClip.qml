/*
 * Copyright 2014 Canonical Ltd.
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
    color: "yellow"
    width: units.gu(70)
    height: units.gu(70)

    property InverseMouseArea ima: imaItem

    Label {
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
        }
        text: "Root"
    }

    Rectangle {
        color: "green"
        width: units.gu(50)
        height: units.gu(50)
        anchors.centerIn: parent
        clip: true

        Label {
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
            }
            text: "Clipper"
        }

        Rectangle {
            width: units.gu(30)
            height: units.gu(30)
            color: "lightgrey"
            anchors.centerIn: parent

            Label {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    top: parent.top
                }
                text: "Inverse mouse area"
            }

            InverseMouseArea {
                id: imaItem
                objectName: "Test_IMA"
                anchors.fill: parent
                topmostItem: true
                clip: false
            }
        }
    }
}
