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
    width: units.gu(50)
    height: units.gu(100)

    Column {
        anchors.fill: parent
        Button {
            text: "Reset"
            onClicked: forceActiveFocus()
        }

        FocusScope {
            id: passiveScope
            width: units.gu(50)
            height: units.gu(50)

            Rectangle {
                width: height
                height: units.gu(20)
                color: "red"
                anchors.centerIn: parent

                FocusScope {
                    objectName: "active_scope"
                    activeFocusOnMousePress: true
                    anchors.fill: parent
                }
            }
        }
        FocusScope {
            id: activeScope
            width: units.gu(50)
            height: units.gu(50)
            activeFocusOnMousePress: true

            Rectangle {
                width: height
                height: units.gu(20)
                color: "green"
                anchors.centerIn: parent

                FocusScope {
                    objectName: "active_scope"
                    activeFocusOnMousePress: true
                    anchors.fill: parent
                }
            }
        }
    }

    Connections {
        target: window
        onActiveFocusItemChanged: print("Active focus is", window.activeFocusItem)
    }
}
