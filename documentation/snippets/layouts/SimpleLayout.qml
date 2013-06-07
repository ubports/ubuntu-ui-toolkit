/*
 * Copyright 2013 Canonical Ltd.
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
import Ubuntu.Layouts 0.1

Item {
    id: root
    width: units.gu(30)
    height: units.gu(30)

    //! [0]
    Layouts {
        objectName: "layouts"
        id: layouts
        anchors.fill: parent

        layouts: [
            //![column layout]
            ConditionalLayout {
                name: "column"
                when: layouts.width > units.gu(50) && layouts.width <= units.gu(70)
                Flickable {
                    contentWidth: column.width
                    Row {
                        id: column
                        width: childrenRect.width
                        height: parent.height
                        ConditionalLayout.items: ["red", "green", "blue"]
                        ConditionalLayout.height: column.height
                    }
                }
            },
            //![column layout]
            ConditionalLayout {
                name: "flow"
                when: layouts.width > units.gu(70) && layouts.width <= units.gu(90)
                Flickable {
                    contentHeight: flow.height
                    Flow {
                        id: flow
                        width: parent.width
                        height: childrenRect.height
                        ConditionalLayout.items: ["red", "green", "blue"]
                        ConditionalLayout.width: units.gu(30)
                        ConditionalLayout.height: units.gu(20)
                        ConditionalLayout.rotation: 75
                        ConditionalLayout.scale: 0.5
                    }
                }
            },
            ConditionalLayout {
                name: "row"
                when: layouts.width > units.gu(90) && layouts.width < units.gu(100)
                Row {
                    spacing: units.dp(2)
                    ConditionalLayout.items: ["blue", "green", "red"]
                    ConditionalLayout.width: units.gu(20)
                    ConditionalLayout.height: units.gu(20)
                }
            },
            ConditionalLayout {
                name: "composit"
                when: layouts.width >= units.gu(100)
                Row {
                    spacing: units.dp(4)
                    ItemLayout {
                        item: "green"
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        width: units.gu(25)
                    }
                    ConditionalLayout.items: ["red", "blue"]
                    ConditionalLayout.width: units.gu(30)
                    ConditionalLayout.height: units.gu(10)
                }
            }

        ]

        //![default layout]
        Button {
            id: button1
            text: "Item #1"
            color: "red"
            ConditionalLayout.item: "red"
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }
            width: units.gu(15)
        }
        Button {
            id: button2
            text: "Item #2"
            color: "green"
            ConditionalLayout.item: "green"
            anchors {
                top: parent.top
                left: button1.right
                right: parent.right
            }
            height: units.gu(10)
        }
        Button {
            id: nolayout
            text: "Non-laid out"
            color: "brown"
            anchors {
                top: button2.bottom
                left: button1.right
                right: parent.right
                bottom: button3.top
            }
        }
        Button {
            id: button3
            text: "Item #3"
            color: "blue"
            ConditionalLayout.item: "blue"
            anchors{
                left: button1.right
                right: parent.right
                bottom: parent.bottom
            }
            height: units.gu(10)
        }
        //![default layout]
    }
    //! [0]
}
