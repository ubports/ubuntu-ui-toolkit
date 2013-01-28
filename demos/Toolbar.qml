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

import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
Template {
    title: i18n.tr("Drilldown")

    Item {
        Rectangle {
            color: "#eeeeee"
            anchors {
                top: parent.top
                bottom: parent.bottom
                bottomMargin: units.gu(10)
                horizontalCenter: parent.horizontalCenter
            }
            width: units.gu(38)
            border.width: 2

            PageStack {
                id: pageStack
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    horizontalCenter: parent.horizontalCenter
                }
                anchors.fill: parent
                width: units.gu(38)

                Component.onCompleted: push(page0)

                Page {
                    id: page0
                    title: i18n.tr("Root page")

                    tools: [
                        Action {
                            text: "one"
                            iconSource: Qt.resolvedUrl("back.png")
                            onTriggered: print("First action")
                        },
                        Action {
                            text: "two"
                            iconSource: Qt.resolvedUrl("back.png")
                            onTriggered: print("Second action")
                        },
                        Action {
                            text: "three"
                            iconSource: Qt.resolvedUrl("back.png")
                            onTriggered: print("Third action")
                        }
                    ]

                    Column {
                        anchors.fill: parent
                        ListItem.Standard {
                            text: i18n.tr("Page one")
                            onClicked: pageStack.push(page1)
                            progression: true
                        }
                        ListItem.Standard {
                            text: i18n.tr("Page two (external)")
                            onClicked: pageStack.push(Qt.resolvedUrl("MyCustomPage.qml"))
                            progression: true
                        }
                    }
                }

                Page {
                    id: page1
                    title: i18n.tr("First page")

                    tools: [
                        Action { text: "Click!"; onTriggered: print("Triggered!") }
                    ]

                    Column {
                        anchors.fill: parent
                        ListItem.Standard {
                            text: i18n.tr("External page")
                            onClicked: pageStack.push(Qt.resolvedUrl("MyCustomPage.qml"))
                            progression: true
                        }
                        ListItem.Standard {
                            text: i18n.tr("Root page (again)")
                            onClicked: pageStack.push(page0)
                            progression: true
                        }
                        ListItem.Standard {
                            text: i18n.tr("Red rectangle")
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
}
