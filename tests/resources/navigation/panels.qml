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

Rectangle {
    color: "grey"
    width: units.gu(80)
    height: units.gu(80)

    LayoutMirroring.enabled: true
    LayoutMirroring.childrenInherit: true

    Rectangle {
        width: units.gu(50)
        height: units.gu(50)
        color: "lightgrey"
        anchors.centerIn: parent

        Label {
            text: "Panels everywhere!"
            anchors.centerIn: parent
        }

        Panel {
            align: Qt.AlignLeft
            anchors {
                left: parent.left
                bottom: parent.bottom
                top: parent.top
            }
            width: units.gu(20)

            Rectangle {
                anchors.fill: parent
                color: "black"
                opacity: 1.0
            }
            Column {
                width: parent.width
                height: childrenRect.height
                anchors.verticalCenter: parent.verticalCenter

                Slider {
                    width: parent.width / 1.2
                    anchors.horizontalCenter: parent.horizontalCenter
                    value: 50
                }
                Button {
                    width: parent.width
                    text: "click me!!"
                    onClicked: print("click!")
                }
            }
        }

        Panel {
            align: Qt.AlignRight
            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            height: units.gu(20)
            width: units.gu(10)

            Rectangle {
                anchors.fill: parent
                color: "yellow"
            }
        }

        Panel {
            id: bottomLeftPanel
            align: Qt.AlignBottom
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: rightPanel.left
            }
            height: toolbar.height

            StyledItem {
                id: toolbar
                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                }
                height: units.gu(8)
                property bool opened: bottomLeftPanel.opened
                property bool animating: bottomLeftPanel.animating
                style: Theme.createStyleComponent("ToolbarStyle.qml", toolbar)
                Label {
                    anchors.centerIn: parent
                    text: "This looks like a standard toolbar"
                }
            }
        }

        Panel {
            id: rightPanel
            align: Qt.AlignBottom
            anchors {
                bottom: parent.bottom
                right: parent.right
                top: parent.top
            }
            width: units.gu(15)

            Rectangle {
                color: "pink"
                anchors.fill: parent
            }
        }

        Panel {
            align: Qt.AlignTop
            anchors {
                top: parent.top
                right: parent.right
            }
            width: units.gu(20)
            height: units.gu(20)

            Rectangle {
                color: "red"
                anchors.fill: parent
            }
        }

        Panel {
            align: Qt.AlignTop
            anchors {
                top: parent.top
                left: parent.left
            }
            width: units.gu(20)
            height: units.gu(20)

            Rectangle {
                color: "orange"
                anchors.fill: parent
            }
        }
    }
}
