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

MainView {
    width: units.gu(40)
    height: units.gu(71)

    Column {
        anchors.fill: parent
        spacing: units.gu(1)
        Rectangle {
            width: parent.width
            height: units.gu(5)
            TextInput {
                activeFocusOnPress: true
                anchors.fill: parent
                anchors.margins: units.gu(0.5)
                MouseArea {
                    anchors.fill: parent
                    preventStealing: false
                    propagateComposedEvents: true
                    onPressed: mouse.accepted = false
                    onReleased: print("released")
                }
            }
        }
        Button {
            text: "Button"
            Mouse.onPressed: print("Button pressed")
        }

        Rectangle {
            width: parent.width
            height: units.gu(5)
            color: "green"
            Mouse.onPressed: print("Rectangle pressed")
        }
    }
}
