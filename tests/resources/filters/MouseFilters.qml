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

MainView {
    width: units.gu(40)
    height: units.gu(71)

    Column {
        anchors.fill: parent

        Rectangle {
            width: parent.width
            height: units.gu(5)
            TextInput {
                activeFocusOnPress: true
                anchors.fill: parent
                anchors.margins: units.gu(0.5)
                InverseMouse.onPressed: print("Inverse pressed")
//                Mouse.hoverEnabled: true
//                Mouse.onPressed: {
//                    print(mouse.button + " pressed intercepted, accepted buttons:" + Mouse.acceptedButtons + ", hoverEnabled: " + Mouse.hoverEnabled)
//                }
//                Mouse.onClicked: print("Clicked")
//                Mouse.onDoubleClicked: print("DoubleClick")
//                MouseArea {
//                    anchors.fill: parent
//                    acceptedButtons: Qt.LeftButton | Qt.RightButton
//                    onPressed: print(mouse.button + " Pressed")
//                    onClicked: print(mouse.button + " Clicked")
//                    hoverEnabled: true
//                    onEntered: print("entered")
//                    onExited: print("exited")
//                }
            }
        }
    }
}
