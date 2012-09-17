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
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
Template {
    title: "Drilldown"

    Item {
        PageStack {
            id: pageStack
            anchors {
                top: parent.top
                bottom: parent.bottom
                horizontalCenter: parent.horizontalCenter
            }
            width: 300

            initialPage: page0

            Page {
                id: page0
                title: "Root page"
                anchors.fill: parent

                Column {
                    anchors.fill: parent
                    ListItem.Standard {
                        text: "Page one"
                        onClicked: pageStack.push(page1)
                        progression: true
                    }
                    ListItem.Standard {
                        text: "Page two (external)"
                        onClicked: pageStack.push(Qt.resolvedUrl("MyCustomPage.qml"))
                        progression: true
                    }
                }
            }

            Page {
                id: page1
                title: "First page"
                anchors.fill: parent

                Column {
                    anchors.fill: parent
                    ListItem.Standard {
                        text: "External page"
                        onClicked: pageStack.push(Qt.resolvedUrl("MyCustomPage.qml"))
                        progression: true
                    }
                    ListItem.Standard {
                        text: "Root page (again)"
                        onClicked: pageStack.push(page0)
                        progression: true
                    }
                    ListItem.Standard {
                        text: "Red rectangle"
                        onClicked: pageStack.push(rect, {color: "red"})
                        progression: true
                    }
                }
            }

            Component {
                id: rect
                Rectangle {
                    id: rectangle
                    anchors.fill: parent
                    color: "green"
                }
            }
        }
    }
}
