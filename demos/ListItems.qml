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
    title: _("List Items")

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

                ListItem.Header { text: _("Types of list items") }
                Repeater {
                    model: listItemTypes.typeList.length
                    ListItem.Standard {
                        text: _(listItemTypes.typeList[index])
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

                    ListItem.Header { text: _("Standard") }
                    ListItem.Standard {
                        text: _("Idle")
                    }
                    ListItem.Standard {
                        text: _("Disabled")
                        enabled: false
                    }
                    ListItem.Standard {
                        text: _("Selected")
                        selected: true
                    }
                    ListItem.Standard {
                        text: _("Progression")
                        progression: true
                    }
                    ListItem.Standard {
                        text: _("Icon")
                        icon: Qt.resolvedUrl("avatar_contacts_list.png")
                    }
                    ListItem.Standard {
                        text: _("Custom icon")
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
                            text: _(name)
                        }
                        header: ListItem.Header { text: _("Grouped List") }
                        section.property: "type"
                        section.criteria: ViewSection.FullString
                        section.delegate: ListItem.Header { text: _(section) }
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

                    ListItem.Header { text: _("Single value") }
                    ListItem.SingleValue {
                        text: _("Idle")
                        value: _("Value")
                    }
                    ListItem.SingleValue {
                        text: _("Disabled")
                        value: _("Value")
                        enabled: false
                    }
                    ListItem.SingleValue {
                        text: _("Selected")
                        value: _("Value")
                        selected: true
                    }
                    ListItem.SingleValue {
                        text: _("Progression")
                        value: _("Value")
                        progression: true
                    }
                    ListItem.SingleValue {
                        text: _("Icon")
                        value: _("Value")
                        icon: Qt.resolvedUrl("avatar_contacts_list.png")
                    }
                    ListItem.SingleValue {
                        text: _("Custom icon")
                        value: _("Value")
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
                    ListItem.Header { text: _("Multiple values") }
                    ListItem.MultiValue {
                        text: _("Idle")
                        values: [_("Value") + " 1", _("Value") + " 2",
                            _("Value") + " 3", _("Value") + " 4"]
                    }
                    ListItem.MultiValue {
                        text: _("Disabled")
                        values: [_("Value") + " 1", _("Value") + " 2",
                            _("Value") + " 3", _("Value") + " 4"]
                        enabled: false
                    }
                    ListItem.MultiValue {
                        text: _("Selected")
                        values: [_("Value") + " 1", _("Value") + " 2",
                            _("Value") + " 3", _("Value") + " 4"]
                        selected: true
                    }
                    ListItem.MultiValue {
                        text: _("Progression")
                        values: [_("Value") + " 1", _("Value") + " 2",
                            _("Value") + " 3", _("Value") + " 4"]
                        progression: true
                    }
                    ListItem.MultiValue {
                        text: _("Icon")
                        values: [_("Value") + " 1", _("Value") + " 2",
                            _("Value") + " 3", _("Value") + " 4"]
                        icon: Qt.resolvedUrl("avatar_contacts_list.png")
                    }
                }
            }
            FadingRectangle {
                selected: listItemTypes.selectedType === "Subtitled"
                Column {
                    width: 250
                    ListItem.Header { text: _("Subtitled") }
                    ListItem.Subtitled {
                        text: _("Idle")
                        subText: _("Secondary label")
                    }
                    ListItem.Subtitled {
                        text: _("Disabled")
                        enabled: false
                        subText: _("Secondary label")
                    }
                    ListItem.Subtitled {
                        text: _("Selected")
                        selected: true
                        subText: _("Secondary label")
                    }
                    ListItem.Subtitled {
                        text: _("Progression")
                        subText: _("Secondary label")
                        progression: true
                    }
                    ListItem.Subtitled {
                        text: _("Icon")
                        subText: _("Secondary label")
                        icon: Qt.resolvedUrl("avatar_contacts_list.png")
                    }
                    ListItem.Subtitled {
                        text: _("Multiple lines")
                        subText: _("This is a multi-line subText.\nUp to 5 lines are supported.")
                    }
                    ListItem.Subtitled {
                        text: _("Multiple lines")
                        subText: _("It also works well with icons and progression.")
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
                    ListItem.Header { text: _("Controls") }
                    ListItem.Standard {
                        text: _("Label")
                        control: controlExample.createObject(parent)
                    }
                    ListItem.Standard {
                        enabled: false
                        text: _("Disabled")
                        control: controlExample.createObject(parent)
                    }
                    ListItem.Standard {
                        selected: true
                        text: _("Selected")
                        control: controlExample.createObject(parent)
                    }
                    ListItem.Standard {
                        text: _("Split")
                        control: controlExample.createObject(parent)
                        progression: true
                        onClicked: print("Progression clicked!")
                    }
                    ListItem.Standard {
                        text: _("Icon")
                        icon: Qt.resolvedUrl("avatar_contacts_list.png")
                        control: controlExample.createObject(parent)
                        showDivider: false
                    }
                    ListItem.Header { text: _("Single control") }

                    // TODO: Add more single controls (TextField, Slider) when they become available
                    ListItem.SingleControl {
                        control: Button {
                            text: _("Button")
                            anchors {
                                margins: 10
                                fill: parent
                            }
                            onClicked: print(_("Large button clicked"))
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
                    ListItem.Header { text: _("Captions and Dividers") }
                    ListItem.Standard { text: _("Item") + " 1a" }
                    ListItem.Standard {
                        text: _("Item") + " 1b"
                        showDivider: false
                    }
                    ListItem.Divider { }
                    ListItem.Standard { text: _("Item") + " 2a" }
                    ListItem.Standard { text: _("Item") + " 2b" }
                    ListItem.Caption {
                        text: "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Mauris hendrerit, erat ut vulputate dignissim, augue est vulputate ante, non tempus nulla est quis purus."
                    }
                }
            }
            FadingRectangle {
                selected: listItemTypes.selectedType === "Value selector"
                Column {
                    width: 250
                    ListItem.Header { text: _("Value selectors") }
                    ListItem.ValueSelector {
                        text: _("Idle")
                        values: [_("Value") + " 1", _("Value") + " 2",
                            _("Value") + " 3", _("Value") + " 4"]
                    }
                    ListItem.ValueSelector {
                        text: _("Disabled")
                        values: [_("Value") + " 1", _("Value") + " 2",
                            _("Value") + " 3", _("Value") + " 4"]
                        enabled: false
                    }
                    ListItem.ValueSelector {
                        text: _("Expanded")
                        values: [_("Value") + " 1", _("Value") + " 2",
                            _("Value") + " 3", _("Value") + " 4"]
                        selectedIndex: 1
                        expanded: true
                    }
                    ListItem.ValueSelector {
                        text: _("Icon")
                        icon: Qt.resolvedUrl("avatar_contacts_list.png")
                        values: [_("Value") + " 1", _("Value") + " 2",
                            _("Value") + " 3", _("Value") + " 4"]
                    }
                }
            }
        }
    }
}
