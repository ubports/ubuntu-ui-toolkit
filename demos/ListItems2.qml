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
    title: "Advanced List Items"

    Row {
        spacing: 10
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
                ListItem.Header { text: "Caption" }
                ListItem.Base {
                    height: framing.height + 10
                    Rectangle {
                        id: framing
                        color: "#e0e0e0e0"
                        width: parent.width - 10
                        height: captionText.height + 10
                        anchors.centerIn: parent
                        radius: 5
                        TextCustom {
                            id: captionText
                            anchors.centerIn: parent
                            text: "Any type of Item can be inserted in a List Item, which gives us a lot of flexibility."
                            width: parent.width - 10
                            wrapMode: Text.Wrap
                            color: Qt.rgba(0.4, 0.4, 0.4, 1.0)
                            height: paintedHeight
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

                ListItem.Header { text: "Button" }
                ListItem.Base {
                    height: 50
                    Button {
                        anchors.centerIn: parent
                        width: parent.width - 20
                        text:"click me"
                    }
                }
                ListItem.Header { text: "Tabs" }
                ListItem.Base {
                    height: 145
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
                ListItem.Header { text: "Subtitled" }
                ListItem.Subtitled {
                    text: "Selected"
                    selected: true
                    subText: "One line of text"

                }
                ListItem.Subtitled {
                    text: "Progression"
                    subText: "This is a subtitled list item.\nUp to five lines of text can be used."
                    progression: true
                }
                ListItem.Subtitled {
                    text: "Icon (Disabled)"
                    subText: "This is a multi-line subtitle."//" The icon is aligned at top."
                    iconSource: "avatar_contacts_list.png"
                    enabled: false
                }

            }
        }

    }
}
