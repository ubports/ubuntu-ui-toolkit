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
    width: 800
    height: 600

    color: "#e6e6e6"

    Item {
        id: page

        anchors.fill: parent
        anchors.margins: 35

        Item {
            id: header

            height: childrenRect.height
            anchors.left: parent.left
            anchors.right: parent.right

            TextCustom {
                id: title

                fontSize: "x-large"
                text: "Tabs"
                color: "#757373"
                style: Text.Raised
                styleColor: "white"
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

        Item {
            id: content

            anchors.top: header.bottom
            anchors.topMargin: page.anchors.margins
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right

            Tabs {
                id: tabs

                anchors.fill: parent

                Tab {
                    text: "Tab 1"
                    page: Rectangle {
                        color: "#eeeeee"
                        anchors.fill: parent
                        TextCustom {
                            anchors.centerIn: parent
                            text: "This is the first tab."
                            color: "#757373"
                        }
                    }
                }
                Tab {
                    iconSource: "call_icon.png"
                    page: Rectangle {
                        color: "#e4e4e4"
                        anchors.fill: parent
                        TextCustom {
                            anchors.centerIn: parent
                            text: "This is the second tab."
                            color: "#757373"
                        }
                    }
                }
                Tab {
                    text: "Tab 3"
                    iconSource: "call_icon.png"
                    page:  Rectangle {
                        color: "#dddddd"
                        anchors.fill: parent
                        TextCustom {
                            anchors.centerIn: parent
                            text: "This is the third tab."
                            color: "#757373"
                        }
                    }
                }
            } // Tabs
        }
    }
}
