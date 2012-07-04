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

import QtQuick 1.1
import "../Components"

Rectangle {
    width: page.width + 2 * page.anchors.margins
    height: page.height + 2 * page.anchors.margins

    color: "#cccccc"

    Item {
        id: page

        x: anchors.margins
        y: anchors.margins
        anchors.margins: 20
        width: childrenRect.width
        height: childrenRect.height

        Item {
            id: header

            height: childrenRect.height
            anchors.left: parent.left
            anchors.right: parent.right

            TextCustom {
                id: title

                fontSize: "x-large"
                text: "Buttons"
                color: "#757373"
            }

            Rectangle {
                id: underline

                anchors.top: title.bottom
                anchors.topMargin: 5
                anchors.left: parent.left
                anchors.right: parent.right
                height: 1
                color: "#757373"
            }
        }

        Column {
            id: content

            anchors.top: header.bottom
            anchors.topMargin: 35
            width: 600
            spacing: 30

            Row {
                spacing: 10
                height: 50

                TextCustom {
                    text: "Standard"
                    color: "#757373"
                    width: 80
                    anchors.verticalCenter: parent.verticalCenter
                }

                Button {
                    text: "Call"
                    anchors.verticalCenter: parent.verticalCenter
                }

                Button {
                    text: "Call"
                    enabled: false
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            Row {
                spacing: 10

                TextCustom {
                    text: "Colors"
                    color: "#757373"
                    width: 80
                    anchors.verticalCenter: parent.verticalCenter
                }

                Button {
                    text: "Call"
                    color: "#37b301"
                    anchors.verticalCenter: parent.verticalCenter
                }

                Button {
                    text: "Call"
                    pressedColor: "#dd4f22"
                    anchors.verticalCenter: parent.verticalCenter
                }

                Rectangle {
                    id: darkBackground
                    width: childrenRect.width + 20
                    height: childrenRect.height + 20
                    color: "#3a3c41"
                    anchors.verticalCenter: parent.verticalCenter

                    Button {
                        text: "Call"
                        color: "#dd4f22"
                        darkBorder: true
                        anchors.centerIn: parent
                    }
                }
            }

            Row {
                spacing: 10

                TextCustom {
                    text: "Content"
                    color: "#757373"
                    width: 80
                    anchors.verticalCenter: parent.verticalCenter
                }

                Button {
                    text: "Call"
                    anchors.verticalCenter: parent.verticalCenter
                }

                Button {
                    iconSource: "call_icon.png"
                    anchors.verticalCenter: parent.verticalCenter
                }

                Button {
                    text: "Call"
                    iconSource: "call_icon.png"
                    anchors.verticalCenter: parent.verticalCenter
                }

                Button {
                    text: "Call"
                    iconSource: "call_icon.png"
                    iconPosition: "right"
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            Row {
                spacing: 10

                TextCustom {
                    text: "Scalability"
                    color: "#757373"
                    width: 80
                    anchors.verticalCenter: parent.verticalCenter
                }

                Button {
                    text: "Call"
                    anchors.verticalCenter: parent.verticalCenter
                }

                Button {
                    text: "Call"
                    width: 140
                    height: 90
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
    }
}
