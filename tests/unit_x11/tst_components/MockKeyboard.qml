/*
 * Copyright 2015 Canonical Ltd.
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

import QtQuick 2.4
import Ubuntu.Components 1.2

Object {
    id: inputMethod
    function show() { visible = true; }
    function commit() { }
    property bool visible: false
    property rect keyboardRectangle: Qt.rect(0, 0, 0, units.gu(20))

    Rectangle {
        x: parent.x
        y: parent.height - height
        width: parent.width
        height: inputMethod.keyboardRectangle.height
        visible: inputMethod.visible

        parent: QuickUtils.rootItem(inputMethod)
        color: UbuntuColors.silk
        opacity: 0.5
        // Always on top, even modal Dialog overlay
        z: 100

        Flow {
            anchors.fill: parent
            anchors.margins: units.dp(2)

            Repeater {
                model: [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 0,
                         'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
                         'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
                         'z', 'x', 'c', 'v', ' ', 'b', 'n', 'm', ',', '.'  ]

                Button {
                    text: modelData
                    color: UbuntuColors.blue
                    height: (parent.height - units.dp(2)) / 4
                    width: (parent.width - units.dp(2)) / 10
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: parent.forceActiveFocus(Qt.MouseFocusReason)
        }
    }
}
