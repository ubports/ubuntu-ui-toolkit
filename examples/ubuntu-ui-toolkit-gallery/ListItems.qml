/*
 * Copyright 2015 Canonical Ltd.
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

import QtQuick 2.4
import Ubuntu.Components 1.3 as Toolkit
import Ubuntu.Components.ListItems 1.3 as ListItem

Template {
    objectName: "listItemsTemplate"

    ListItemsSection {
        title: i18n.tr("Standard")
        className: "Standard"
        delegate: ListItem.Standard {
            text: i18n.tr("Label")
        }
    }

    ListItemsSection {
        title: i18n.tr("Subtitled")
        className: "Subtitled"
        delegate: ListItem.Subtitled {
            text: i18n.tr("Label")
            subText: i18n.tr("Secondary label")

        }
    }

    ListItemsSection {
        title: i18n.tr("Icon")
        className: "Standard"
        delegate: ListItem.Standard {
            text: i18n.tr("Label")
            iconSource: Qt.resolvedUrl("avatar_contacts_list.png")
        }
    }

    TemplateSection {
        title: i18n.tr("ThinDivider")
        className: "ListItems-ThinDivider"

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            ListItem.Standard {
                text: i18n.tr("Label")
                showDivider: false
            }
            ListItem.ThinDivider {
            }
            ListItem.Standard {
                text: i18n.tr("Label")
                showDivider: false
            }
        }
    }

    ListItemsSection {
        title: i18n.tr("Progression")
        className: "Standard"
        delegate: ListItem.Standard {
            text: i18n.tr("Label")
            progression: true
        }
    }

    ListItemsSection {
        title: i18n.tr("Single value")
        className: "SingleValue"
        delegate: ListItem.SingleValue {
            text: i18n.tr("Label")
            value: i18n.tr("Value")
        }
    }

    ListItemsSection {
        title: i18n.tr("Multi value")
        className: "MultiValue"
        delegate: ListItem.MultiValue {
            text: i18n.tr("Label")
            values: [i18n.tr("Value 1"),
                     i18n.tr("Value 2"),
                     i18n.tr("Value 3"),
                     i18n.tr("Value 4")]
        }
    }

    ListItemsSection {
        title: i18n.tr("Item selector")
        className: "ItemSelector"

        Column {
            anchors.left: parent.left
            anchors.right: parent.right
            spacing: units.gu(3)

            ListItem.ItemSelector {
                text: i18n.tr("Expanding")
                model: [i18n.tr("Value 1"),
                        i18n.tr("Value 2"),
                        i18n.tr("Value 3"),
                        i18n.tr("Value 4")]
            }

            ListItem.ItemSelector {
                text: i18n.tr("Expanded")
                expanded: true
                model: [i18n.tr("Value 1"),
                        i18n.tr("Value 2"),
                        i18n.tr("Value 3"),
                        i18n.tr("Value 4")]
            }

            ListItem.ItemSelector {
                text: i18n.tr("Multiple Selection")
                expanded: false
                multiSelection: true
                model: [i18n.tr("Value 1"),
                        i18n.tr("Value 2"),
                        i18n.tr("Value 3"),
                        i18n.tr("Value 4")]
            }

            ListItem.ItemSelector {
                text: i18n.tr("Custom Model")
                model: customModel
                expanded: true
                colourImage: true
                delegate: selectorDelegate
            }

            Component {
                id: selectorDelegate
                Toolkit.OptionSelectorDelegate { text: name; subText: description; iconSource: image }
            }

            ListModel {
                id: customModel
                ListElement { name: "Name 1"; description: "Description 1"; image: "images.png" }
                ListElement { name: "Name 2"; description: "Description 2"; image: "images.png" }
                ListElement { name: "Name 3"; description: "Description 3"; image: "images.png" }
                ListElement { name: "Name 4"; description: "Description 4"; image: "images.png" }
            }

            ListItem.ItemSelector {
                text: i18n.tr("Label")
                model: [i18n.tr("Value 1"),
                        i18n.tr("Value 2"),
                        i18n.tr("Value 3"),
                        i18n.tr("Value 4"),
                        i18n.tr("Value 5"),
                        i18n.tr("Value 6"),
                        i18n.tr("Value 7"),
                        i18n.tr("Value 8")]
                containerHeight: itemHeight * 4
            }

            ListItem.ItemSelector {
                text: i18n.tr("Label")
                expanded: true
                selectedIndex: -1
                model: [i18n.tr("Value 1"),
                        i18n.tr("Value 2"),
                        i18n.tr("Value 3"),
                        i18n.tr("Value 4"),
                        i18n.tr("Value 5"),
                        i18n.tr("Value 6"),
                        i18n.tr("Value 7"),
                        i18n.tr("Value 8")]
                containerHeight: itemHeight * 4
            }
        }
    }

    ListItemsSection {
        title: i18n.tr("Control")
        className: "Standard"
        delegate: ListItem.Standard {
            text: i18n.tr("Label")
            control: Toolkit.Switch {
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

    ListItemsSection {
        id: removableSection
        title: i18n.tr("Removable")
        className: "Standard"
        delegate: ListItem.Standard {
            text: i18n.tr("Slide to remove")
            removable: true
            confirmRemoval: (index % 2)
            backgroundIndicator: Rectangle {
                anchors.fill: parent
                color: theme.palette.normal.base
            }
        }

        Toolkit.Button {
            text: i18n.tr("Reset")
            anchors.right: parent.right
            onClicked: {
                for (var i=0, iMax=removableSection.count; i < iMax; i++) {
                    removableSection.itemAt(i).cancelItemRemoval()
                }
            }
        }
    }

    TemplateSection {
        title: i18n.tr("Grouped list")
        className: "ListItems-Header"

        Rectangle {
            color: Qt.rgba(0.0, 0.0, 0.0, 0.01)
            anchors.left: parent.left
            anchors.right: parent.right
            height: groupedList.height

            ListModel {
                id: groupedModel
                ListElement { name: "Orange"; type: "Fruit"}
                ListElement { name: "Apple"; type: "Fruit" }
                ListElement { name: "Tomato"; type: "Fruit" }
                ListElement { name: "Carrot"; type: "Vegetable" }
                ListElement { name: "Potato"; type: "Vegetable" }
            }

            ListView {
                id: groupedList
                model: groupedModel
                width: parent.width
                height: contentHeight
                interactive: false
                delegate: ListItem.Standard {
                    text: i18n.tr(name)
                }
                section.property: "type"
                section.criteria: ViewSection.FullString
                section.delegate: ListItem.Header { text: i18n.tr(section) }
            }
        }
    }

    TemplateSection {
        title: i18n.tr("UbuntuListView")
        className: "UbuntuListView"

        ListModel {
            id: fruitModel
            ListElement { name: "Orange"; details: "The orange (specifically, the sweet orange) is the fruit of the citrus species Citrus × ​sinensis in the family Rutaceae. The fruit of the Citrus sinensis is called sweet orange to distinguish it from that of the Citrus aurantium, the bitter orange. The orange is a hybrid, possibly between pomelo (Citrus maxima) and mandarin (Citrus reticulata), cultivated since ancient times.\n-\nWikipedia"}
            ListElement { name: "Apple"; details: "Fruit" }
            ListElement { name: "Tomato"; details: "The tomato is the edible, often red fruit of the plant Solanum lycopersicum, commonly known as a tomato plant. Both the species and its use as a food originated in Mexico, and spread throughout the world following the Spanish colonization of the Americas. Its many varieties are now widely grown, sometimes in greenhouses in cooler climates. The tomato is consumed in diverse ways, including raw, as an ingredient in many dishes, sauces, salads, and drinks. While it is botanically a fruit, it is considered a vegetable for culinary purposes (as well as under U.S. customs regulations, see Nix v. Hedden), which has caused some confusion. The fruit is rich in lycopene, which may have beneficial health effects. The tomato belongs to the nightshade family (Solanaceae). The plants typically grow to 1–3 meters (3–10 ft) in height and have a weak stem that often sprawls over the ground and vines over other plants. It is a perennial in its native habitat, although often grown outdoors in temperate climates as an annual. An average common tomato weighs approximately 100 grams (4 oz).\n-\nWikipedia" }
            ListElement { name: "Carrot"; details: "Vegetable" }
            ListElement { name: "Potato"; details: "Vegetable" }
        }

        Toolkit.SortFilterModel {
            id: processedFruits
            model: fruitModel
            sort.property: "title"
            sort.order: Qt.DescendingOrder
            filter.property: "details"
            filter.pattern: /Vegetable/
        }

        Toolkit.UbuntuListView {
            id: ubuntuListView
            anchors { left: parent.left; right: parent.right }
            height: units.gu(24)
            model: processedFruits
            clip: true

            delegate: ListItem.Expandable {
                id: expandingItem

                expandedHeight: contentColumn.height + units.gu(1)

                onClicked: {
                    ubuntuListView.expandedIndex = index;
                }

                Column {
                    id: contentColumn
                    anchors { left: parent.left; right: parent.right }
                    Item {
                        anchors { left: parent.left; right: parent.right}
                        height: expandingItem.collapsedHeight
                        Toolkit.Label {
                            anchors { left: parent.left; right: parent.right; verticalCenter: parent.verticalCenter}
                            text: model.name
                        }
                    }

                    Toolkit.Label {
                        anchors { left: parent.left; right: parent.right }
                        text: model.details
                        wrapMode: Text.WordWrap
                    }
                }
            }
        }
    }

    ListItemsSection {
        title: i18n.tr("ExpandablesColumn")
        className: "ExpandablesColumn"

        ListItem.ExpandablesColumn {
            anchors { left: parent.left; right: parent.right }
            clip: true
            height: units.gu(24)

            Repeater {
                model: 8
                ListItem.Expandable {
                    id: expandingColumnItem
                    expandedHeight: contentColumn1.height + units.gu(1)
                    collapseOnClick: index % 2 == 0

                    onClicked: {
                        expanded = true;
                    }

                    Column {
                        id: contentColumn1
                        anchors { left: parent.left; right: parent.right }
                        Item {
                            anchors { left: parent.left; right: parent.right}
                            height: expandingColumnItem.collapsedHeight
                            Toolkit.Label {
                                anchors { left: parent.left; right: parent.right; verticalCenter: parent.verticalCenter}
                                text: "Item " + index + (expandingColumnItem.collapseOnClick ? " (with collapseOnClick)" : "")
                            }
                        }

                        Toolkit.UbuntuShape {
                            anchors { left: parent.left; right: parent.right }
                            height: index % 2 == 0 ? units.gu(6) : units.gu(18)
                            backgroundColor: "khaki"
                        }
                    }
                }
            }
        }
    }

    ListItemsSection {
        title: i18n.tr("Expandable")
        className: "Expandable"

        Column {
            anchors { left: parent.left; right: parent.right }
            clip: true

            Repeater {
                model: 2
                ListItem.Expandable {
                    id: expandingItem1
                    expandedHeight: contentCol1.height + units.gu(1)

                    onClicked: {
                        expanded = !expanded;
                    }

                    Column {
                        id: contentCol1
                        anchors { left: parent.left; right: parent.right }
                        Item {
                            anchors { left: parent.left; right: parent.right}
                            height: expandingItem1.collapsedHeight
                            Toolkit.Label {
                                anchors { left: parent.left; right: parent.right; verticalCenter: parent.verticalCenter}
                                text: "Item " + index
                            }
                        }

                        Toolkit.UbuntuShape {
                            anchors { left: parent.left; right: parent.right }
                            height: index % 2 == 0 ? units.gu(6) : units.gu(18)
                            backgroundColor: "khaki"
                        }
                    }
                }
            }
        }
    }
}
