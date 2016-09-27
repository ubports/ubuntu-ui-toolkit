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

FocusScope {
    id: root
    width: units.gu(40)
    height: units.gu(71)

    Flickable {
        anchors {
            fill: parent
            margins: units.gu(1)
        }
        contentWidth: body.width
        contentHeight: body.height
        clip: true

        Rectangle {
            color: "blue"
            id: body
            width: units.gu(100)
            height: units.gu(100)
            MouseArea {
                objectName: "host"
                anchors.fill: parent
                Mouse.forwardTo: [proxy]
                Item {
                    id: proxy
                    objectName: "proxy"
                    anchors.fill: parent
                    Mouse.enabled: true
                    // this threshold does not apply for the forwarded events!
                    Mouse.clickAndHoldThreshold: units.gu(2)
                }
            }

        }
    }
}
