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
    title: i18n.tr("Toolbars")
    objectName: "Toolbars"

    Item {
        MainView {
            id: mainView
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }
            width: units.gu(50)
            height: units.gu(50)

            Page {
                anchors.fill: parent
                title: "Toolbar demo"
                Rectangle {
                    color: "black"
                    opacity: 0.1
                    anchors.fill: parent
                }

                Column {
                    anchors.centerIn: parent
                    spacing: units.gu(1)
                    width: units.gu(30)

                    Button {
                        width: parent.width
                        text: "Show toolbar"
                    }
                    Button {
                        width: parent.width
                        text: "Hide toolbar"
                    }
                    Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: units.gu(2)
                        Switch {
                        }
                        Label {
                            anchors {
                                top: parent.top
                                bottom: parent.bottom
                            }
                            verticalAlignment: Text.AlignVCenter
                            text: "Lock position"
                        }
                    }
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
    }
}
