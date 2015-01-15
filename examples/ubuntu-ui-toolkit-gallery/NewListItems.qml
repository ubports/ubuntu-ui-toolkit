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
import Ubuntu.Components 1.2

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
            text: i18n.tr("Basic")
        }
        ListItemWithLabel {
            text: i18n.tr("Colored divider")
            divider {
                colorFrom: UbuntuColors.red
                colorTo: UbuntuColors.green
            }
        }
        ListItemWithLabel {
            text: i18n.tr("No divider")
            divider.visible: false
        }
        ListItemWithLabel {
            color: UbuntuColors.blue
            text: i18n.tr("Colored")
        }
        ListItemWithLabel {
            text: i18n.tr("Highlight color")
            highlightColor: UbuntuColors.orange
            // no highlight without clicked() or leading/trailing actions
            onClicked: print(i18n.tr("Highlighting list item"))
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
            swipeOvershoot: 0
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
}
