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
    title: "List Items"
    Item {
        Rectangle {
            color: "#eeeeee"
            width: childrenRect.width
            height: childrenRect.height
            id: typesRectangle
            anchors {
                top: parent.top
                left: parent.left
                margins: 10
            }
            ListItem.Container {
                width: 250
                id: listItemTypes

                property string selectedType: "Standard"

                ListItem.Header { text: "Types of list items" }

                // FIXME: It would be nicer to generate the list below
                // using a ListView or Repeater, but that won't work well until
                // the following bug is fixed:
                // https://bugs.launchpad.net/tavastia/+bug/1030272
                ListItem.Standard {
                    text: "Standard"
                    progression: true
                    selected: listItemTypes.selectedType === text
                    onClicked: listItemTypes.selectedType = text
                }
                ListItem.Standard {
                    text: "Single value"
                    progression: true
                    selected: listItemTypes.selectedType === text
                    onClicked: listItemTypes.selectedType = text
                }
                ListItem.Standard {
                    text: "Multiple values"
                    progression: true
                    selected: listItemTypes.selectedType === text
                    onClicked: listItemTypes.selectedType = text
                }
                ListItem.Standard {
                    text: "Subtitled"
                    progression: true
                    selected: listItemTypes.selectedType === text
                    onClicked: listItemTypes.selectedType = text
                }
                ListItem.Standard {
                    text: "Controls"
                    progression: true
                    selected: listItemTypes.selectedType === text
                    onClicked: listItemTypes.selectedType = text
                }
                ListItem.Standard {
                    text: "Grouping and caption"
                    progression: true
                    selected: listItemTypes.selectedType === text
                    onClicked: listItemTypes.selectedType = text
                }
                ListItem.Standard {
                    text: "Selector"
                    progression: true
                    selected: listItemTypes.selectedType === text
                    onClicked: listItemTypes.selectedType = text
                }
            }
        }
        Rectangle {
            color: "#eeeeee"
            width: childrenRect.width
            height: childrenRect.height
            visible: listItemTypes.selectedType === "Standard"
            anchors {
                top: parent.top
                left: typesRectangle.right
                margins: 10
            }
            ListItem.Container {
                width: 250

                ListItem.Header { text: "Standard" }
                ListItem.Standard {
                    text: "Default"
                }
                ListItem.Standard {
                    text: "Disabled"
                    enabled: false
                }
                ListItem.Standard {
                    text: "Selected"
                    selected: true
                }
                ListItem.Standard {
                    text: "Progression"
                    progression: true
                }
                ListItem.Standard {
                    text: "Icon"
                    iconSource: "avatar_contacts_list.png"
                }
            }
        }
        Rectangle {
            color: "#eeeeee"
            width: childrenRect.width
            height: childrenRect.height
            visible: listItemTypes.selectedType === "Single value"
            anchors {
                top: parent.top
                left: typesRectangle.right
                margins: 10
            }
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
            visible: listItemTypes.selectedType === "Multiple values"
            anchors {
                top: parent.top
                left: typesRectangle.right
                margins: 10
            }
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

        Rectangle {
            color: "#eeeeee"
            width: childrenRect.width
            height: childrenRect.height
            visible: listItemTypes.selectedType === "Subtitled"
            anchors {
                top: parent.top
                left: typesRectangle.right
                margins: 10
            }
            ListItem.Container {
                width: 250
                ListItem.Header { text: "Subtitled" }
                ListItem.Subtitled {
                    text: "Default"
                    subText: "Secondary label"
                }
                ListItem.Subtitled {
                    text: "Disabled"
                    enabled: false
                    subText: "Secondary label"
                }
                ListItem.Subtitled {
                    text: "Selected"
                    selected: true
                    subText: "Secondary label"
                }
                ListItem.Subtitled {
                    text: "Progression"
                    subText: "Secondary label"
                    progression: true
                }
                ListItem.Subtitled {
                    text: "Icon"
                    subText: "Secondary label"
                    iconSource: "avatar_contacts_list.png"
                }
                ListItem.Subtitled {
                    text: "Multiple lines"
                    subText: "This is a multi-line subText.\nUp to 5 lines are supported."
                }
                ListItem.Subtitled {
                    text: "Multiple lines"
                    subText: "It also works well with icons and progression."
                    iconSource: "avatar_contacts_list.png"
                    progression: true
                }
            }
        }
        Rectangle {
            color: "#eeeeee"
            width: childrenRect.width
            height: childrenRect.height
            visible: listItemTypes.selectedType === "Controls"
            anchors {
                top: parent.top
                left: typesRectangle.right
                margins: 10
            }
            ListItem.Container {
                width: 250
                ListItem.Header { text: "Controls" }
                ListItem.Standard {
                    text: "Label"
                    control: Button {
                        width: 100
                        text: "control"
                        anchors.centerIn: parent
                        onClicked: print("clicked button")
                    }
                    onClicked: control.clicked()
                }
                ListItem.Standard {
                    enabled: false
                    text: "Disabled"
                    control: Button {
                        width: 100
                        text: "control"
                        anchors.centerIn: parent
                    }
                }
                ListItem.Standard {
                    selected: true
                    text: "Selected"
                    control: Button {
                        width: 100
                        text: "control"
                        anchors.centerIn: parent
                    }
                }
                ListItem.Standard {
                    text: "Split"
                    control: Button {
                        width: 100
                        text: "control"
                        anchors.centerIn: parent
                    }
                    progression: true
                    onClicked: control.clicked()
                }
                ListItem.Standard {
                    text: "Icon"
                    iconSource: "avatar_contacts_list.png"
                    control: Button {
                        width: 100
                        text: "control"
                        anchors.centerIn: parent
                    }
                }
                ListItem.Standard {
                    control: Button {
                        text: "Action"
                        anchors.fill: parent
                    }
                }
            }
        }

        Rectangle {
            color: "#eeeeee"
            width: childrenRect.width
            height: childrenRect.height
            visible: listItemTypes.selectedType === "Grouping and caption"
            anchors {
                top: parent.top
                left: typesRectangle.right
                margins: 10
            }
            ListItem.Container {
                width: 250
                ListItem.Header { text: "Grouping" }
                ListItem.Header { text: "Header" }
                ListItem.Standard { text: "Item 1a" }
                ListItem.Standard { text: "Item 1b" }
                ListItem.Divider { }
                ListItem.Standard { text: "Item 2a" }
                ListItem.Standard { text: "Item 2b" }
                ListItem.Header { text: "Caption" }
                ListItem.Caption {
                    text: "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Mauris hendrerit, erat ut vulputate dignissim, augue est vulputate ante, non tempus nulla est quis purus."
                }
            }
        }

        Rectangle {
            color: "#eeeeee"
            width: childrenRect.width
            height: childrenRect.height
            visible: listItemTypes.selectedType === "Selector"
            anchors {
                top: parent.top
                left: typesRectangle.right
                margins: 10
            }
            ListItem.Container {
                width: 250
                ListItem.Header { text: "Selectors" }
                ListItem.Selector {
                    text: "Default"
                    values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                }
                ListItem.Selector {
                    text: "Disabled"
                    values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                    enabled: false
                }
                ListItem.Selector {
                    text: "Expanded"
                    values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                    selectedIndex: 1
                    expanded: true
                }
                ListItem.Selector {
                    text: "Icon"
                    iconSource: "avatar_contacts_list.png"
                    values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                }
            }
        }
    }
}
