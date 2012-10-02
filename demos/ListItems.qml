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
            }
            Column {
                width: 250
                id: listItemTypes

                property variant typeList: ["Standard", "Single value", "Multiple values",
                    "Value selector", "Subtitled", "Controls", "Captions and Dividers",
                    "Grouped List"]
                property string selectedType: "Standard"

                ListItem.Header { text: "Types of list items" }
                Repeater {
                    model: listItemTypes.typeList.length
                    ListItem.Standard {
                        text: listItemTypes.typeList[index]
                        selected: listItemTypes.selectedType === text
                        onClicked: listItemTypes.selectedType = text
                    }
                }
            }
        }
        Item {
            anchors {
                top: parent.top
                left: typesRectangle.right
                leftMargin: 15
            }
            FadingRectangle {
                selected: listItemTypes.selectedType === "Standard"
                Column {
                    width: 250

                    ListItem.Header { text: "Standard" }
                    ListItem.Standard {
                        text: "Idle"
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
                        icon: Qt.resolvedUrl("avatar_contacts_list.png")
                    }
                    ListItem.Standard {
                        text: "Custom icon"
                        icon: Rectangle {
                            anchors.margins: 5
                            width: height
                            color: "darkgrey"
                            Rectangle {
                                anchors {
                                    fill: parent
                                    margins: 5
                                }
                                color: "lightgrey"
                                Rectangle {
                                    anchors {
                                        fill: parent
                                        margins: 5
                                    }
                                    color: "darkgrey"
                                    Rectangle {
                                        anchors {
                                            fill: parent
                                            margins: 5
                                        }
                                        color: "lightgrey"
                                    }
                                }
                            }
                        }
                    }
                }
            }
            FadingRectangle {
                id: container
                selected: listItemTypes.selectedType === "Grouped List"
                Column {
                    width: 250

                    ListModel {
                        id: testModel
                        ListElement { name: "Parsley" }
                        ListElement { name: "Orange"; type: "fruit"}
                        ListElement { name: "Apple"; type: "fruit" }
                        ListElement { name: "Tomato"; type: "fruit" }

                        ListElement { name: "Carrot"; type: "veg" }
                        ListElement { name: "Potato"; type: "veg" }
                    }

                    ListView {
                        id: groupedList
                        model: testModel
                        width: parent.width
                        /* FIXME: This is workaround for QML1 ListView bugs - all fixed in QML2:
                             https://bugreports.qt-project.org/browse/QTBUG-17057
                             https://bugreports.qt-project.org/browse/QTBUG-19941
                         * We need to explicitly calculate the contentHeight for QML1
                         * So we have all but 2 items that are 50 pixels height, the others are 48
                         * high and there are 3 headings at 24 pixels high
                         */
                        contentHeight: 50 * count - 2 * 2 + 3 * 24
                        interactive: false

                        delegate: ListItem.Standard {
                            text: name
                        }
                        header: ListItem.Header { text: "Grouped List" }
                        section.property: "type"
                        section.criteria: ViewSection.FullString
                        section.delegate: ListItem.Header { text: section }
                    }
                    Component.onCompleted: {
                        groupedList.height = groupedList.contentHeight;
                    }
                }
            }
            FadingRectangle {
                selected: listItemTypes.selectedType === "Single value"
                Column {
                    width: 250

                    ListItem.Header { text: "Single value" }
                    ListItem.SingleValue {
                        text: "Idle"
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
                        icon: Qt.resolvedUrl("avatar_contacts_list.png")
                    }
                    ListItem.SingleValue {
                        text: "Custom icon"
                        value: "Value"
                        icon: Rectangle {
                            anchors.margins: 5
                            width: height
                            color: "darkgrey"
                            Rectangle {
                                anchors {
                                    fill: parent
                                    margins: 5
                                }
                                color: "lightgrey"
                                Rectangle {
                                    anchors {
                                        fill: parent
                                        margins: 5
                                    }
                                    color: "darkgrey"
                                    Rectangle {
                                        anchors {
                                            fill: parent
                                            margins: 5
                                        }
                                        color: "lightgrey"
                                    }
                                }
                            }
                        }
                    }
                }
            }
            FadingRectangle {
                selected: listItemTypes.selectedType === "Multiple values"
                Column {
                    width: 250
                    ListItem.Header { text: "Multiple values" }
                    ListItem.MultiValue {
                        text: "Idle"
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
                        icon: Qt.resolvedUrl("avatar_contacts_list.png")
                    }
                }
            }
            FadingRectangle {
                selected: listItemTypes.selectedType === "Subtitled"
                Column {
                    width: 250
                    ListItem.Header { text: "Subtitled" }
                    ListItem.Subtitled {
                        text: "Idle"
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
                        icon: Qt.resolvedUrl("avatar_contacts_list.png")
                    }
                    ListItem.Subtitled {
                        text: "Multiple lines"
                        subText: "This is a multi-line subText.\nUp to 5 lines are supported."
                    }
                    ListItem.Subtitled {
                        text: "Multiple lines"
                        subText: "It also works well with icons and progression."
                        icon: Qt.resolvedUrl("avatar_contacts_list.png")
                        progression: true
                    }
                }
            }
            FadingRectangle {
                selected: listItemTypes.selectedType === "Controls"
                Component {
                    id: controlExample
                    Switch {
                        anchors {
                            top: parent.top
                            bottom: parent.bottom
                        }
                    }
                }
                Column {
                    width: 250
                    ListItem.Header { text: "Controls" }
                    ListItem.Standard {
                        text: "Label"
                        control: controlExample.createObject(parent)
                    }
                    ListItem.Standard {
                        enabled: false
                        text: "Disabled"
                        control: controlExample.createObject(parent)
                    }
                    ListItem.Standard {
                        selected: true
                        text: "Selected"
                        control: controlExample.createObject(parent)
                    }
                    ListItem.Standard {
                        text: "Split"
                        control: controlExample.createObject(parent)
                        progression: true
                        onClicked: print("Progression clicked!")
                    }
                    ListItem.Standard {
                        text: "Icon"
                        icon: Qt.resolvedUrl("avatar_contacts_list.png")
                        control: controlExample.createObject(parent)
                        showDivider: false
                    }
                    ListItem.Header { text: "Single control" }

                    // TODO: Add more single controls (TextField, Slider) when they become available
                    ListItem.SingleControl {
                        control: Button {
                            text: "Button"
                            anchors {
                                margins: 10
                                fill: parent
                            }
                            onClicked: print("Large button clicked")
                        }
                    }
                }
            }
            FadingRectangle {
                selected: listItemTypes.selectedType === "Captions and Dividers"
                Column {
                    width: 250
                    ListItem.Header { text: "Captions and Dividers" }
                    ListItem.Standard { text: "Item 1a" }
                    ListItem.Standard {
                        text: "Item 1b"
                        showDivider: false
                    }
                    ListItem.Divider { }
                    ListItem.Standard { text: "Item 2a" }
                    ListItem.Standard { text: "Item 2b" }
                    ListItem.Caption {
                        text: "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Mauris hendrerit, erat ut vulputate dignissim, augue est vulputate ante, non tempus nulla est quis purus."
                    }
                }
            }
            FadingRectangle {
                selected: listItemTypes.selectedType === "Value selector"
                Column {
                    width: 250
                    ListItem.Header { text: "Value selectors" }
                    ListItem.ValueSelector {
                        text: "Idle"
                        values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                    }
                    ListItem.ValueSelector {
                        text: "Disabled"
                        values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                        enabled: false
                    }
                    ListItem.ValueSelector {
                        text: "Expanded"
                        values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                        selectedIndex: 1
                        expanded: true
                    }
                    ListItem.ValueSelector {
                        text: "Icon"
                        icon: Qt.resolvedUrl("avatar_contacts_list.png")
                        values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                    }
                }
            }
        }
    }
}
