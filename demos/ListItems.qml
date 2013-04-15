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
    objectName: "List Items"
    title: i18n.tr("List Items")

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
                width: units.gu(31)
                id: listItemTypes

                property variant typeList: ["Standard", "Single value", "Multiple values",
                    "Value selector", "Subtitled", "Controls", "Captions and Dividers",
                    "Grouped List", "Removable"]
                property string selectedType: "Standard"

                ListItem.Header { text: i18n.tr("Types of list items") }
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
                leftMargin: units.gu(2)
            }
            FadingRectangle {
                selected: listItemTypes.selectedType === "Standard"
                Column {
                    width: units.gu(31)

                    ListItem.Header { text: i18n.tr("Standard") }
                    ListItem.Standard {
                        text: i18n.tr("Idle")
                    }
                    ListItem.Standard {
                        text: i18n.tr("Disabled")
                        enabled: false
                    }
                    ListItem.Standard {
                        text: i18n.tr("Selected")
                        selected: true
                    }
                    ListItem.Standard {
                        text: i18n.tr("Progression")
                        progression: true
                    }
                    ListItem.Standard {
                        text: i18n.tr("Icon")
                        icon: Qt.resolvedUrl("avatar_contacts_list.png")
                    }
                    ListItem.Standard {
                        text: i18n.tr("Custom icon")
                        icon: Rectangle {
                            anchors.margins: units.dp(5)
                            width: height
                            color: "darkgrey"
                            Rectangle {
                                anchors {
                                    fill: parent
                                    margins: units.dp(5)
                                }
                                color: "lightgrey"
                                Rectangle {
                                    anchors {
                                        fill: parent
                                        margins: units.dp(5)
                                    }
                                    color: "darkgrey"
                                    Rectangle {
                                        anchors {
                                            fill: parent
                                            margins: units.dp(5)
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
                    width: units.gu(31)

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
                         * So we have all but 2 items that are 6 gu + 2 dp high,
                         * the others are 6 grid units high and there are 3 headings
                         * at 3 grid units high.
                         */
                        contentHeight: (units.gu(6) + units.dp(2)) * count + 3 * units.gu(4)
                        interactive: false

                        delegate: ListItem.Standard {
                            text: i18n.tr(name)
                        }
                        header: ListItem.Header { text: i18n.tr("Grouped List") }
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
                    width: units.gu(31)

                    ListItem.Header { text: i18n.tr("Single value") }
                    ListItem.SingleValue {
                        text: i18n.tr("Idle")
                        value: i18n.tr("Value")
                    }
                    ListItem.SingleValue {
                        text: i18n.tr("Disabled")
                        value: i18n.tr("Value")
                        enabled: false
                    }
                    ListItem.SingleValue {
                        text: i18n.tr("Selected")
                        value: i18n.tr("Value")
                        selected: true
                    }
                    ListItem.SingleValue {
                        text: i18n.tr("Progression")
                        value: i18n.tr("Value")
                        progression: true
                    }
                    ListItem.SingleValue {
                        text: i18n.tr("Icon")
                        value: i18n.tr("Value")
                        icon: Qt.resolvedUrl("avatar_contacts_list.png")
                    }
                    ListItem.SingleValue {
                        text: i18n.tr("Custom icon")
                        value: i18n.tr("Value")
                        icon: Rectangle {
                            anchors.margins: units.dp(5)
                            width: height
                            color: "darkgrey"
                            Rectangle {
                                anchors {
                                    fill: parent
                                    margins: units.dp(5)
                                }
                                color: "lightgrey"
                                Rectangle {
                                    anchors {
                                        fill: parent
                                        margins: units.dp(5)
                                    }
                                    color: "darkgrey"
                                    Rectangle {
                                        anchors {
                                            fill: parent
                                            margins: units.dp(5)
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
                    width: units.gu(31)
                    ListItem.Header { text: i18n.tr("Multiple values") }
                    ListItem.MultiValue {
                        text: i18n.tr("Idle")
                        values: [i18n.tr("Value") + " 1", i18n.tr("Value") + " 2",
                            i18n.tr("Value") + " 3", i18n.tr("Value") + " 4"]
                    }
                    ListItem.MultiValue {
                        text: i18n.tr("Disabled")
                        values: [i18n.tr("Value") + " 1", i18n.tr("Value") + " 2",
                            i18n.tr("Value") + " 3", i18n.tr("Value") + " 4"]
                        enabled: false
                    }
                    ListItem.MultiValue {
                        text: i18n.tr("Selected")
                        values: [i18n.tr("Value") + " 1", i18n.tr("Value") + " 2",
                            i18n.tr("Value") + " 3", i18n.tr("Value") + " 4"]
                        selected: true
                    }
                    ListItem.MultiValue {
                        text: i18n.tr("Progression")
                        values: [i18n.tr("Value") + " 1", i18n.tr("Value") + " 2",
                            i18n.tr("Value") + " 3", i18n.tr("Value") + " 4"]
                        progression: true
                    }
                    ListItem.MultiValue {
                        text: i18n.tr("Icon")
                        values: [i18n.tr("Value") + " 1", i18n.tr("Value") + " 2",
                            i18n.tr("Value") + " 3", i18n.tr("Value") + " 4"]
                        icon: Qt.resolvedUrl("avatar_contacts_list.png")
                    }
                }
            }
            FadingRectangle {
                selected: listItemTypes.selectedType === "Subtitled"
                Column {
                    width: units.gu(31)
                    ListItem.Header { text: i18n.tr("Subtitled") }
                    ListItem.Subtitled {
                        text: i18n.tr("Idle")
                        subText: i18n.tr("Secondary label")
                    }
                    ListItem.Subtitled {
                        text: i18n.tr("Disabled")
                        enabled: false
                        subText: i18n.tr("Secondary label")
                    }
                    ListItem.Subtitled {
                        text: i18n.tr("Selected")
                        selected: true
                        subText: i18n.tr("Secondary label")
                    }
                    ListItem.Subtitled {
                        text: i18n.tr("Progression")
                        subText: i18n.tr("Secondary label")
                        progression: true
                    }
                    ListItem.Subtitled {
                        text: i18n.tr("Icon")
                        subText: i18n.tr("Secondary label")
                        icon: Qt.resolvedUrl("avatar_contacts_list.png")
                    }
                    ListItem.Subtitled {
                        text: i18n.tr("Multiple lines")
                        subText: i18n.tr("This is a multi-line subText.\nUp to 5 lines are supported.")
                    }
                    ListItem.Subtitled {
                        text: i18n.tr("Multiple lines")
                        subText: i18n.tr("It also works well with icons and progression.")
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
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
                Column {
                    width: units.gu(31)
                    ListItem.Header { text: i18n.tr("Controls") }
                    ListItem.Standard {
                        text: i18n.tr("Label")
                        control: controlExample.createObject(parent)
                    }
                    ListItem.Standard {
                        enabled: false
                        text: i18n.tr("Disabled")
                        control: controlExample.createObject(parent)
                    }
                    ListItem.Standard {
                        text: i18n.tr("Control disabled")
                        control: Switch {
                            anchors.verticalCenter: parent.verticalCenter
                            enabled: false
                        }
                    }
                    ListItem.Standard {
                        selected: true
                        text: i18n.tr("Selected")
                        control: controlExample.createObject(parent)
                    }
                    ListItem.Standard {
                        text: i18n.tr("Split")
                        control: controlExample.createObject(parent)
                        progression: true
                        onClicked: print("Progression clicked!")
                    }
                    ListItem.Standard {
                        text: i18n.tr("Icon")
                        icon: Qt.resolvedUrl("avatar_contacts_list.png")
                        control: controlExample.createObject(parent)
                        showDivider: false
                    }
                    ListItem.Header { text: i18n.tr("Single control") }

                    // TODO: Add more single controls (TextField, Slider) when they become available
                    ListItem.SingleControl {
                        control: Button {
                            text: i18n.tr("Button")
                            anchors {
                                margins: units.gu(1)
                                fill: parent
                            }
                            onClicked: print(i18n.tr("Large button clicked"))
                        }
                    }
                    ListItem.SingleControl {
                        control: Slider {
                            anchors {
                                margins: units.gu(1)
                                fill: parent
                            }
                        }
                    }
                }
            }
            FadingRectangle {
                selected: listItemTypes.selectedType === "Captions and Dividers"
                Column {
                    width: units.gu(31)
                    ListItem.Header { text: i18n.tr("Captions and Dividers") }
                    ListItem.Standard { text: i18n.tr("Item") + " 1a" }
                    ListItem.Standard {
                        text: i18n.tr("Item") + " 1b"
                        showDivider: false
                    }
                    ListItem.Divider { }
                    ListItem.Standard { text: i18n.tr("Item") + " 2a" }
                    ListItem.Standard { text: i18n.tr("Item") + " 2b" }
                    ListItem.Caption {
                        text: "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Mauris hendrerit, erat ut vulputate dignissim, augue est vulputate ante, non tempus nulla est quis purus."
                    }
                }
            }
            FadingRectangle {
                selected: listItemTypes.selectedType === "Value selector"
                Column {
                    width: units.gu(31)
                    ListItem.Header { text: i18n.tr("Value selectors") }
                    ListItem.ValueSelector {
                        text: i18n.tr("Idle")
                        values: [i18n.tr("Value") + " 1", i18n.tr("Value") + " 2",
                            i18n.tr("Value") + " 3", i18n.tr("Value") + " 4"]
                    }
                    ListItem.ValueSelector {
                        text: i18n.tr("Disabled")
                        values: [i18n.tr("Value") + " 1", i18n.tr("Value") + " 2",
                            i18n.tr("Value") + " 3", i18n.tr("Value") + " 4"]
                        enabled: false
                    }
                    ListItem.ValueSelector {
                        text: i18n.tr("Expanded")
                        values: [i18n.tr("Value") + " 1", i18n.tr("Value") + " 2",
                            i18n.tr("Value") + " 3", i18n.tr("Value") + " 4"]
                        selectedIndex: 1
                        expanded: true
                    }
                    ListItem.ValueSelector {
                        text: i18n.tr("Icon")
                        icon: Qt.resolvedUrl("avatar_contacts_list.png")
                        values: [i18n.tr("Value") + " 1", i18n.tr("Value") + " 2",
                            i18n.tr("Value") + " 3", i18n.tr("Value") + " 4"]
                    }
                }
            }

            FadingRectangle {
                selected: listItemTypes.selectedType === "Removable"
                Column {
                    width: units.gu(31)
                    ListItem.Header { text: i18n.tr("Removable") }
                    ListItem.Standard {
                        id: _item0

                        removable: true
                        backgroundIndicator: RemovableBG {
                            state: _item0.swipingState
                        }

                        text: i18n.tr("Label")
                        control: controlExample.createObject(parent)
                    }
                    ListItem.Standard {
                        id: _item1

                        enabled: false
                        removable: true
                        backgroundIndicator: RemovableBG {
                            state: _item1.swipingState
                        }

                        text: i18n.tr("Disabled")
                        control: controlExample.createObject(parent)
                    }
                    ListItem.Standard {
                        id: _item2

                        selected: true
                        removable: true
                        backgroundIndicator: RemovableBG {
                            state: _item2.swipingState
                        }
                        text: i18n.tr("Selected")
                        control: controlExample.createObject(parent)
                    }
                    ListItem.Standard {
                        id: _item3

                        text: i18n.tr("Split")
                        removable: true
                        backgroundIndicator: RemovableBG {
                            state: _item3.swipingState
                        }
                        control: controlExample.createObject(parent)
                        progression: true
                        onClicked: print("Progression clicked!")
                    }
                    ListItem.Standard {
                        id: _item4

                        text: i18n.tr("Icon")
                        removable: true
                        backgroundIndicator: RemovableBG {
                            state: _item4.swipingState
                        }
                        icon: Qt.resolvedUrl("avatar_contacts_list.png")
                        control: controlExample.createObject(parent)
                        showDivider: false
                    }
                }
            }
        }
    }
}
