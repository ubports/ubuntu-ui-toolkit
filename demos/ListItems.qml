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
import "../Components/ListItems" as ListItem

Template {
    title: "List Items"

    Row {
        spacing: 10

        Rectangle {
            color: "#eeeeee"
            width: childrenRect.width
            height: childrenRect.height

            ListItem.Container {
                width: 250

                ListItem.Header { text: "Standard" }

                ListItem.Default {
                    text: "Default"
                }
                ListItem.Default {
                    text: "Disabled"
                    enabled: false
                }
                ListItem.Default {
                    text: "Selected"
                    selected: true
                }
                ListItem.Default {
                    text: "Progression"
                    progression: true
                }
                ListItem.Default {
                    text: "Icon"
                    iconSource: "avatar_contacts_list.png"
                }
                ListItem.Header { text: "Actions" }
                ListItem.Base {
                    height: 50
                    Button {
                        anchors.centerIn: parent
                        width: parent.width - 20
                        text:"click me"
                    }
                }
                ListItem.Base {
                    height: 100
                    Tabs {
                        height: parent.height - 20
                        width: parent.width - 20
                        anchors.centerIn: parent
                        Tab {
                            text: "Tab 1"
                            page: TextCustom {
                                anchors.centerIn: parent
                                text: "Page one"
                                color: Qt.rgba(0.4, 0.4, 0.4, 1.0)
                            }
                        }
                        Tab {
                            text: "Tab 2"
                            page: TextCustom {
                                anchors.centerIn: parent
                                text: "Page two"
                                color: Qt.rgba(0.4, 0.4, 0.4, 1.0)
                            }
                        }
                        Tab {
                            text: "Tab 3"
                            page: TextCustom {
                                anchors.centerIn: parent
                                text: "Page three"
                                color: Qt.rgba(0.4, 0.4, 0.4, 1.0)
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            color: "#eeeeee"
            width: childrenRect.width
            height: childrenRect.height

            ListItem.Container {
                width: 250

                ListItem.Header { text: "Single value" }

                ListItem.SingleValue {
                    text: "Default"
                    value: "Value"
                }
                ListItem.SingleValue {
                    text: "Disabled"
                    value: "Value"
                    enabled: false
                }
                ListItem.SingleValue {
                    text: "Selected"
                    value: "Value"
                    selected: true
                }
                ListItem.SingleValue {
                    text: "Progression"
                    value: "Value"
                    progression: true
                }
                ListItem.SingleValue {
                    text: "Icon"
                    value: "Value"
                    iconSource: "avatar_contacts_list.png"
                }
            }
        }

        Rectangle {
            color: "#eeeeee"
            width: childrenRect.width
            height: childrenRect.height

            ListItem.Container {
                width: 250

                ListItem.Header { text: "Multiple values" }

                ListItem.MultiValue {
                    text: "Default"
                    values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                }
                ListItem.MultiValue {
                    text: "Disabled"
                    values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                    enabled: false
                }
                ListItem.MultiValue {
                    text: "Selected"
                    values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                    selected: true
                }
                ListItem.MultiValue {
                    text: "Progression"
                    values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                    progression: true
                }
                ListItem.MultiValue {
                    text: "Icon"
                    values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                    iconSource: "avatar_contacts_list.png"
                }
            }
        }
    }
}
