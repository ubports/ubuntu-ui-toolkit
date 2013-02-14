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

                tools: toolsSwitch.checked ? toolbarActions2 : toolbarActions1
                ToolbarActions {
                    id: toolbarActions1
                    Action {
                        text: "action 1"
                        iconSource: Qt.resolvedUrl("avatar_contacts_list.png")
                    }
                    Action {
                        text: "action 2"
                        iconSource: Qt.resolvedUrl("call_icon.png")
                    }
                    lock: lockSwitch.checked

                    property Item leftItem: Button {
                        text: "cancel"
                        onClicked: toolbarActions1.active = false
                        anchors.verticalCenter: parent ? parent.verticalCenter : undefined
                    }
                }
                ToolbarActions {
                    id: toolbarActions2
                    Action {
                        text: "action 3"
                        iconSource: Qt.resolvedUrl("call_icon.png")
                    }
                    Action {
                        text: "disabled"
                        iconSource: Qt.resolvedUrl("call_icon.png")
                        enabled: false
                    }
                    Action {
                        text: "invisible"
                        iconSource: Qt.resolvedUrl("call_icon.png")
                        visible: false
                    }

                    lock: lockSwitch.checked
                    back {
                        visible: true
                        onTriggered: toolsSwitch.checked = false
                    }
                }

                Column {
                    anchors.centerIn: parent
                    spacing: units.gu(1)
                    width: units.gu(30)

                    Button {
                        width: parent.width
                        text: "Show toolbar"
                        onClicked: mainView.tools.active = true
                    }
                    Button {
                        width: parent.width
                        text: "Hide toolbar"
                        onClicked: mainView.tools.active = false
                    }
                    Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: units.gu(2)
                        Switch {
                            id: lockSwitch
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
                    Label {
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                        text: lockSwitch.checked ?
                                  "Swiping will not show/hide the toolbar,\nbut the buttons above still work." :
                                  "Swipe up/down near the bottom of this\nview to show/hide the toolbar."
                    }
                    Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: units.gu(2)
                        Switch {
                            id: toolsSwitch
                        }
                        Label {
                            anchors {
                                top: parent.top
                                bottom: parent.bottom
                            }
                            verticalAlignment: Text.AlignVCenter
                            text: toolsSwitch.checked ? "Tools 2" : "Tools 1"
                        }
                    }
                }
            }
        }
    }
}
