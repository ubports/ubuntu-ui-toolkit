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
    title: i18n.tr("Page Stack")
    objectName: "Page Stack"

    Item {
        MainView {
            id: mainView
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }
            width: units.gu(38)
            height: units.gu(50)

            PageStack {
                id: pageStack
//                anchors.fill: parent
                Component.onCompleted: push(page0)

                Page {
                    id: page0
                    title: i18n.tr("Root page")
                    visible: false

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

                    tools: ToolbarActions {
                        Action {
                            text: "action 1"
                            iconSource: Qt.resolvedUrl("avatar_contacts_list.png")
                        }
                        Action {
                            text: "action 2"
                            iconSource: Qt.resolvedUrl("call_icon.png")
                        }
                    }
                }

                Page {
                    id: page1
                    title: i18n.tr("First page")
                    visible: false

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
