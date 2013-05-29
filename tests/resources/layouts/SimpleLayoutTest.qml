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

    Layouts {
        objectName: "layouts"
        id: layouts
        anchors.fill: parent
        onCurrentLayoutChanged: {
//            if (currentLayout === "column") {
//                label1.anchors.top = undefined;
//                label2.anchors.top = undefined;
//                label3.anchors.top = undefined;
//                label1.anchors.bottom = undefined;
//                label2.anchors.bottom = undefined;
//                label3.anchors.bottom = undefined;
//            }
        }

        layouts: [
            ConditionalLayout {
                name: "column"
                when: layouts.width > units.gu(30) && layouts.width <= units.gu(40)
                Column {
                    //anchors.fill: parent
                    ConditionalLayout.items: ["item1", "item2", "item3"]
                }
            },
            ConditionalLayout {
                name: "flow"
                when: layouts.width > units.gu(40) && layouts.width <= units.gu(60)
                Flow {
                    anchors.fill: parent
                    ConditionalLayout.items: ["item1", "item2", "item3"]
                }
            },
            ConditionalLayout {
                name: "row"
                when: layouts.width > units.gu(60)
                Row {
                    anchors.fill: parent
                    ConditionalLayout.items: ["item1", "item2", "item3"]
                }
            }
        ]

        // default layout
        Item {
            objectName: "defaultLayout"
            anchors.fill: parent
            Rectangle {
                objectName: "item1"
                id: label1
                anchors.left: parent.left
                anchors.onTopChanged: if (anchors.top == undefined) print("label1.top is "+anchors.top)
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: units.gu(15)
                ConditionalLayout.name: "item1"
                color: "red"
            }
            Rectangle {
                objectName: "item2"
                id: label2
                anchors.top: parent.top
                anchors.left: label1.right
                anchors.onTopChanged: if (anchors.top == undefined) print("label2.top is "+anchors.top)
                anchors.right: parent.right
                height: units.gu(15)
                ConditionalLayout.name: "item2"
                color: "green"
            }
            Rectangle {
                objectName: "item3"
                id: label3
                anchors.left: label1.right
                anchors.onTopChanged: if (anchors.top == undefined) print("label3.top is "+anchors.top)
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: units.gu(15)
                ConditionalLayout.name: "item3"
                color: "blue"
            }
        }
    }
}
