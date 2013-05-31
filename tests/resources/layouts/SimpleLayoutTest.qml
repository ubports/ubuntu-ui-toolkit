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

        layouts: [
            ConditionalLayout {
                name: "column"
                when: layouts.width > units.gu(30) && layouts.width <= units.gu(40)
                Column {
                    anchors.fill: parent
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
                anchors {
                    left: parent.left
                    top: parent.top
                    bottom: parent.bottom
                }
                width: units.gu(15)
                height: units.gu(30)
                ConditionalLayout.name: "item1"
                color: "red"
            }
            Rectangle {
                objectName: "item2"
                id: label2
                anchors {
                    top: parent.top
                    left: label1.right
                    right: parent.right
                }
                x: units.gu(15)
                height: units.gu(15)
                width: units.gu(15);
                ConditionalLayout.name: "item2"
                color: "green"
            }
            Rectangle {
                objectName: "item3"
                id: label3
                anchors{
                    left: label1.right
                    right: parent.right
                    bottom: parent.bottom
                }
                x: units.gu(15)
                y: units.gu(15)
                height: units.gu(15)
                width: units.gu(15);
                ConditionalLayout.name: "item3"
                color: "blue"
            }
        }
    }
}
