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
    title: gallery.tr("List Items")

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

                ListItem.Header { text: gallery.tr("Types of list items") }
                Repeater {
                    model: listItemTypes.typeList.length
                    ListItem.Standard {
                        text: i18n.tr(listItemTypes.typeList[index])
                        selected: listItemTypes.selectedType === listItemTypes.typeList[index]
                        onClicked: listItemTypes.selectedType = listItemTypes.typeList[index]
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

                    ListItem.Header { text: gallery.tr("Standard") }
                    ListItem.Standard {
                        text: gallery.tr("Idle")
                    }
                    ListItem.Standard {
                        text: gallery.tr("Disabled")
                        enabled: false
                    }
                    ListItem.Standard {
                        text: gallery.tr("Selected")
                        selected: true
                    }
                    ListItem.Standard {
                        text: gallery.tr("Progression")
                        progression: true
                    }
                    ListItem.Standard {
                        text: gallery.tr("Icon")
                        icon: Qt.resolvedUrl("avatar_contacts_list.png")
                    }
                    ListItem.Standard {
                        text: gallery.tr("Custom icon")
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
                            text: i18n.tr(name)
                        }
                        header: ListItem.Header { text: gallery.tr("Grouped List") }
                        section.property: "type"
                        section.criteria: ViewSection.FullString
                        section.delegate: ListItem.Header { text: i18n.tr(section) }
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

                    ListItem.Header { text: gallery.tr("Single value") }
                    ListItem.SingleValue {
                        text: gallery.tr("Idle")
                        value: gallery.tr("Value")
                    }
                    ListItem.SingleValue {
                        text: gallery.tr("Disabled")
                        value: gallery.tr("Value")
                        enabled: false
                    }
                    ListItem.SingleValue {
                        text: gallery.tr("Selected")
                        value: gallery.tr("Value")
                        selected: true
                    }
                    ListItem.SingleValue {
                        text: gallery.tr("Progression")
                        value: gallery.tr("Value")
                        progression: true
                    }
                    ListItem.SingleValue {
                        text: gallery.tr("Icon")
                        value: gallery.tr("Value")
                        icon: Qt.resolvedUrl("avatar_contacts_list.png")
                    }
                    ListItem.SingleValue {
                        text: gallery.tr("Custom icon")
                        value: gallery.tr("Value")
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
                    ListItem.Header { text: gallery.tr("Multiple values") }
                    ListItem.MultiValue {
                        text: gallery.tr("Idle")
                        values: [gallery.tr("Value") + " 1", gallery.tr("Value") + " 2",
                            gallery.tr("Value") + " 3", gallery.tr("Value") + " 4"]
                    }
                    ListItem.MultiValue {
                        text: gallery.tr("Disabled")
                        values: [gallery.tr("Value") + " 1", gallery.tr("Value") + " 2",
                            gallery.tr("Value") + " 3", gallery.tr("Value") + " 4"]
                        enabled: false
                    }
                    ListItem.MultiValue {
                        text: gallery.tr("Selected")
                        values: [gallery.tr("Value") + " 1", gallery.tr("Value") + " 2",
                            gallery.tr("Value") + " 3", gallery.tr("Value") + " 4"]
                        selected: true
                    }
                    ListItem.MultiValue {
                        text: gallery.tr("Progression")
                        values: [gallery.tr("Value") + " 1", gallery.tr("Value") + " 2",
                            gallery.tr("Value") + " 3", gallery.tr("Value") + " 4"]
                        progression: true
                    }
                    ListItem.MultiValue {
                        text: gallery.tr("Icon")
                        values: [gallery.tr("Value") + " 1", gallery.tr("Value") + " 2",
                            gallery.tr("Value") + " 3", gallery.tr("Value") + " 4"]
                        icon: Qt.resolvedUrl("avatar_contacts_list.png")
                    }
                }
            }
            FadingRectangle {
                selected: listItemTypes.selectedType === "Subtitled"
                Column {
                    width: 250
                    ListItem.Header { text: gallery.tr("Subtitled") }
                    ListItem.Subtitled {
                        text: gallery.tr("Idle")
                        subText: gallery.tr("Secondary label")
                    }
                    ListItem.Subtitled {
                        text: gallery.tr("Disabled")
                        enabled: false
                        subText: gallery.tr("Secondary label")
                    }
                    ListItem.Subtitled {
                        text: gallery.tr("Selected")
                        selected: true
                        subText: gallery.tr("Secondary label")
                    }
                    ListItem.Subtitled {
                        text: gallery.tr("Progression")
                        subText: gallery.tr("Secondary label")
                        progression: true
                    }
                    ListItem.Subtitled {
                        text: gallery.tr("Icon")
                        subText: gallery.tr("Secondary label")
                        icon: Qt.resolvedUrl("avatar_contacts_list.png")
                    }
                    ListItem.Subtitled {
                        text: gallery.tr("Multiple lines")
                        subText: gallery.tr("This is a multi-line subText.\nUp to 5 lines are supported.")
                    }
                    ListItem.Subtitled {
                        text: gallery.tr("Multiple lines")
                        subText: gallery.tr("It also works well with icons and progression.")
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
                    ListItem.Header { text: gallery.tr("Controls") }
                    ListItem.Standard {
                        text: gallery.tr("Label")
                        control: controlExample.createObject(parent)
                    }
                    ListItem.Standard {
                        enabled: false
                        text: gallery.tr("Disabled")
                        control: controlExample.createObject(parent)
                    }
                    ListItem.Standard {
                        selected: true
                        text: gallery.tr("Selected")
                        control: controlExample.createObject(parent)
                    }
                    ListItem.Standard {
                        text: gallery.tr("Split")
                        control: controlExample.createObject(parent)
                        progression: true
                        onClicked: print("Progression clicked!")
                    }
                    ListItem.Standard {
                        text: gallery.tr("Icon")
                        icon: Qt.resolvedUrl("avatar_contacts_list.png")
                        control: controlExample.createObject(parent)
                        showDivider: false
                    }
                    ListItem.Header { text: gallery.tr("Single control") }

                    // TODO: Add more single controls (TextField, Slider) when they become available
                    ListItem.SingleControl {
                        control: Button {
                            text: gallery.tr("Button")
                            anchors {
                                margins: 10
                                fill: parent
                            }
                            onClicked: print(gallery.tr("Large button clicked"))
                        }
                    }
                    ListItem.SingleControl {
                        control: Slider {
                            anchors.margins: 5
                            width: parent.width - 10
                        }
                    }
                }
            }
            FadingRectangle {
                selected: listItemTypes.selectedType === "Captions and Dividers"
                Column {
                    width: 250
                    ListItem.Header { text: gallery.tr("Captions and Dividers") }
                    ListItem.Standard { text: gallery.tr("Item") + " 1a" }
                    ListItem.Standard {
                        text: gallery.tr("Item") + " 1b"
                        showDivider: false
                    }
                    ListItem.Divider { }
                    ListItem.Standard { text: gallery.tr("Item") + " 2a" }
                    ListItem.Standard { text: gallery.tr("Item") + " 2b" }
                    ListItem.Caption {
                        text: "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Mauris hendrerit, erat ut vulputate dignissim, augue est vulputate ante, non tempus nulla est quis purus."
                    }
                }
            }
            FadingRectangle {
                selected: listItemTypes.selectedType === "Value selector"
                Column {
                    width: 250
                    ListItem.Header { text: gallery.tr("Value selectors") }
                    ListItem.ValueSelector {
                        text: gallery.tr("Idle")
                        values: [gallery.tr("Value") + " 1", gallery.tr("Value") + " 2",
                            gallery.tr("Value") + " 3", gallery.tr("Value") + " 4"]
                    }
                    ListItem.ValueSelector {
                        text: gallery.tr("Disabled")
                        values: [gallery.tr("Value") + " 1", gallery.tr("Value") + " 2",
                            gallery.tr("Value") + " 3", gallery.tr("Value") + " 4"]
                        enabled: false
                    }
                    ListItem.ValueSelector {
                        text: gallery.tr("Expanded")
                        values: [gallery.tr("Value") + " 1", gallery.tr("Value") + " 2",
                            gallery.tr("Value") + " 3", gallery.tr("Value") + " 4"]
                        selectedIndex: 1
                        expanded: true
                    }
                    ListItem.ValueSelector {
                        text: gallery.tr("Icon")
                        icon: Qt.resolvedUrl("avatar_contacts_list.png")
                        values: [gallery.tr("Value") + " 1", gallery.tr("Value") + " 2",
                            gallery.tr("Value") + " 3", gallery.tr("Value") + " 4"]
                    }
                }
            }
        }
    }
}
