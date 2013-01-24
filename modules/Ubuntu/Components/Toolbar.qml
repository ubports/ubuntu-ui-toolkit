/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

import QtQuick 2.0

// TODO: documentation
GenericToolbar {
    id: toolbar

    /*!
      The page of which the tools must be displayed on the toolbar,
      and which can have a pageStack that is popped when the back button
      is clicked.
     */
    property Item page: null

    Item {
        id: contents
        anchors {
            left: parent.left
            right: parent.right
        }
        height: parent.height

        Rectangle { // TODO: make background themable?
            anchors.fill: parent
            color: "white"

            MouseArea {
                // don't let mouse events go through the toolbar
                anchors.fill: parent
                // FIXME: Bug in qml? Without onClicked below, this MouseArea
                //      seems disabled.
                onClicked: { }
            }
        }

        Item {
            id: leftItem
            anchors {
                left: parent.left
                top:parent.top
                bottom: parent.bottom
            }
            width: backButton ? backButton.width : 0

            property Item backButton: ChromeButton {
                parent: leftItem
                anchors.centerIn: parent
                icon: Qt.resolvedUrl("artwork/back.png") // TODO: make themable in Page
                text: "Back"    // TODO: make themable in Page

                visible: toolbar.page && toolbar.page.hasOwnProperty("pageStack")
                         && toolbar.page.pageStack && toolbar.page.pageStack.depth > 1

                onClicked: toolbar.page.pageStack.pop()
            }
        }

        Row {
            id: rightItem
            anchors {
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }
            width: childrenRect.width
            property var tools: toolbar.page ? toolbar.page.tools : null

            Repeater {
                model: rightItem.tools ? rightItem.tools : 0
                ChromeButton {
                    anchors {
                        bottom: parent.bottom
                        top: parent.top
                    }
                    text: modelData.text
                    icon: modelData.icon ? modelData.icon : ""
                    onClicked: modelData.triggered()
                }
            }
        }
    }
}
