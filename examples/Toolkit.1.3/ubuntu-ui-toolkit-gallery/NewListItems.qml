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

import QtQuick 2.2
import Ubuntu.Components 1.3

Template {
    objectName: "listItemsTemplate"

    TemplateSection {
        className: "ListItem"
        // no spacing between the list items in the Column
        spacing: 0
        Item {
            // compensate for the spacing of 0 by adding this
            // Item inbetween the title and the list items.
            height: units.gu(3)
            width: parent.width
        }

        // clip the action delegates while swiping left/right
        clip: true

       ListItemWithLabel {
            color: UbuntuColors.blue
            text: i18n.tr("Colored")
        }
        ListItemWithLabel {
            text: i18n.tr("Highlight color")
            highlightColor: UbuntuColors.orange
            // no highlight without clicked() or leading/trailing actions
        }

        ListItemActions {
            id: exampleLeadingActions
            actions: [
                Action {
                    iconName: "tick"
                },
                Action {
                    iconName: "delete"
                }
            ]
        }
        ListItemActions {
            id: exampleTrailingActions
            actions: [
                Action {
                    iconName: "edit"
                },
                Action {
                    iconName: "save-to"
                }
            ]
        }

        ListItemWithLabel {
            text: i18n.tr("Leading actions")
            leadingActions: exampleLeadingActions
        }
        ListItemWithLabel {
            text: i18n.tr("Trailing actions")
            trailingActions: exampleTrailingActions
        }
        ListItemWithLabel {
            text: i18n.tr("Leading and trailing actions")
            leadingActions: exampleLeadingActions
            trailingActions: exampleTrailingActions
        }
        ListItemWithLabel {
            text: i18n.tr("Custom action delegates")
            leadingActions: ListItemActions {
                actions: [
                    Action {
                        iconName: "tick"
                        property color color: UbuntuColors.green
                    },
                    Action {
                        iconName: "delete"
                        property color color: UbuntuColors.red
                    }
                ]
                delegate: Rectangle {
                    width: height
                    color: pressed ? Qt.darker(action.color, 1.2) : action.color
                    Icon {
                        width: units.gu(3)
                        height: width
                        name: action.iconName
                        color: "white"
                        anchors.centerIn: parent
                    }
                }
            }
            trailingActions: ListItemActions {
                actions: [
                    Action {
                        text: i18n.tr("Edit")
                    },
                    Action {
                        text: i18n.tr("Move")
                    }
                ]
                delegate: Rectangle {
                    color: pressed ? Qt.darker("yellow", 1.1) : "yellow"
                    width: units.gu(10)
                    Label {
                        text: action.text
                        anchors.centerIn: parent
                    }
                }
            }
        }
    }

    TemplateSection {
        className: "ListItem"
        title: "Select mode"

        ListView {
            height: units.gu(20)
            width: parent.width
            clip: true

            ViewItems.dragMode: ViewItems.selectMode
            ViewItems.onDragUpdated: {
                if (event.status == ListItemDrag.Started) {
                    if (model[event.from] == "Immutable")
                        event.accept = false;
                    return;
                }
                if (model[event.to] == "Immutable") {
                    event.accept = false;
                    return;
                }
                // No instantaneous updates
                if (event.status == ListItemDrag.Moving) {
                    event.accept = false;
                    return;
                }
                if (event.status == ListItemDrag.Dropped) {
                    var fromItem = model[event.from];
                    var list = model;
                    list.splice(event.from, 1);
                    list.splice(event.to, 0, fromItem);
                    model = list;
                }
            }

            model: [ i18n.tr("Basic"), i18n.tr("Colored divider"), i18n.tr("Immutable"), i18n.tr("No divider") ]
            delegate: ListItemWithLabel {
                text: modelData
                color: dragging ? "lightblue" : "transparent"
                divider {
                    colorFrom: modelData == i18n.tr("Colored divider") ? UbuntuColors.red : Qt.rgba(0.0, 0.0, 0.0, 0.0)
                    colorTo: modelData == i18n.tr("Colored divider") ? UbuntuColors.green : Qt.rgba(0.0, 0.0, 0.0, 0.0)
                    visible: modelData != i18n.tr("No divider")
                }
            }
        }
    }

    TemplateSection {
        className: "ListItem"
        title: "Drag mode"

        UbuntuListView {
            height: units.gu(20)
            width: parent.width
            clip: true
            ViewItems.dragMode: true
            ViewItems.onDragUpdated: {
                if (event.status == ListItemDrag.Started) {
                    if (model.get(event.from).label == "Immutable")
                        event.accept = false;
                    return;
                }
                if (model.get(event.to).label == "Immutable") {
                    event.accept = false;
                    return;
                }
                // Live update as you drag
                if (event.status == ListItemDrag.Moving) {
                    model.move(event.from, event.to, 1);
                }
            }


            model: ListModel {
                ListElement { label: "Basic" }
                ListElement { label: "Colored divider" }
                ListElement { label: "Immutable" }
                ListElement { label: "No divider" }
            }

            delegate: ListItemWithLabel {
                text: modelData
                color: dragMode ? "lightblue" : "lightgray"
                divider {
                    colorFrom: modelData == i18n.tr("Colored divider") ? UbuntuColors.red : Qt.rgba(0.0, 0.0, 0.0, 0.0)
                    colorTo: modelData == i18n.tr("Colored divider") ? UbuntuColors.green : Qt.rgba(0.0, 0.0, 0.0, 0.0)
                    visible: modelData != i18n.tr("No divider")
                }
            }
        }
    }
}
