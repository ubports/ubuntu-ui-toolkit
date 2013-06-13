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
import Ubuntu.Components 0.1

MainView {
    width: units.gu(50)
    height: units.gu(80)

    Action {
        id: action1
        text: "action 1"
        onTriggered: print("one!")
    }
    Action {
        id: action2
        text: "action 2"
        onTriggered: print("two!")
        iconSource: Qt.resolvedUrl("../../../examples/ubuntu-ui-toolkit-gallery/call_icon.png")
    }

    Page {
        title: "test page"

        Label {
            anchors.centerIn: parent
            text: "Hello, world"
        }

        tools: ToolbarItems {
            Button {
                anchors.verticalCenter: parent.verticalCenter
                width: units.gu(10)
                text: "yeah"
                onClicked: print("upa")
            }
            ActionItem {
                ItemStyle.class: "toolbar-button"
                iconSource: Qt.resolvedUrl("../../../examples/ubuntu-ui-toolkit-gallery/small_avatar.png")
                height: parent.height
                width: units.gu(5)
                text: "oh"
                onTriggered: print("lala")
            }
            ActionItem {
                ItemStyle.class: "toolbar-button"
                action: action2
                width: units.gu(5)
                height: parent.height
            }
            Rectangle {
                width: 50
                height: 50
                color: "navy"
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }
}
