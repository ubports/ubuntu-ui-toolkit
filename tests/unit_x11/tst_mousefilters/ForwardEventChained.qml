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

Item {
    id: main
    objectName: "proxy2"
    width: units.gu(40)
    height: units.gu(71)

    Mouse.enabled: true

    Column {
        anchors.fill: parent
        spacing: units.gu(1)

        Rectangle {
            id: blueRect
            objectName: "proxy1"
            width: parent.width
            height: units.gu(20)
            color: "blue"
            Mouse.forwardTo: [main]
        }
        Rectangle {
            width: parent.width
            height: units.gu(20)
            color: "red"
            MouseArea {
                objectName: "host"
                anchors.fill: parent
                Mouse.forwardTo: [blueRect]
            }
        }
    }
}
