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
    id: root
    width: units.gu(40)
    height: units.gu(71)
    objectName: "target"

    Mouse.enabled: true
    Column {
        TextInput {
            objectName: "FilterOwner"
            width: root.width
            height: units.gu(5)

            Mouse.forwardTo: [root, other]
            Mouse.onPressed: {print("this pressed"); mouse.accepted = false}
        }
        Rectangle {
            id: other
            width: root.width
            height: units.gu(5)
            color: "blue"

            MouseArea {
                anchors.fill: parent
                objectName: "other"
                onPressed: print("other pressed")
            }
        }
    }
}
