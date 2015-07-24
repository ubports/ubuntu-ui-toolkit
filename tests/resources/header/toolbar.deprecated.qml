/*
 * Copyright (C) 2013 Canonical Ltd.
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
    width: units.gu(50)
    height: units.gu(80)

    Action {
        id: action1
        text: "action 1"
        onTriggered: print("one!")
        iconSource: Qt.resolvedUrl("../../../examples/ubuntu-ui-toolkit-gallery/call_icon.png")
    }

    Page {
        title: "test page"

        Label {
            anchors.centerIn: parent
            text: "Hello, world"
        }

        tools: ToolbarItems {
            Button { // normal-styled button.
                anchors.verticalCenter: parent.verticalCenter
                width: units.gu(10)
                text: "yeah"
                onClicked: print("upa")
            }
            ToolbarButton {
                iconSource: Qt.resolvedUrl("../../../examples/ubuntu-ui-toolkit-gallery/small_avatar.png")
                text: "oh"
                onTriggered: print("lala")
                enabled: false
            }
            ToolbarButton {
                action: action1
            }
            ToolbarButton {
                action: Action {
                    onTriggered: print("two!")
                    iconSource: Qt.resolvedUrl("../../../examples/ubuntu-ui-toolkit-gallery/small_avatar.png")
                    text: "Second action"
                }
                text: "action 2"
            }

            back: ToolbarButton {
                text: "cancel"
                iconSource: Qt.resolvedUrl("../../../qml/Ubuntu/Components/artwork/back.png")
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }
}
