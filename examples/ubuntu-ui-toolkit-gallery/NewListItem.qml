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

    // FIXME TIM: Do something with the sections.
    TemplateSection {
        className: "ListItem"

        Rectangle {
            color: "white"
            width: parent.width
            height: units.gu(36)
            clip: true

            Flickable {
                anchors.fill: parent
                contentWidth: width
                contentHeight: column.height

                Column {
                    id: column
                    width: parent.width
                    height: childrenRect.height

                    ListItem {
                        Label {
                            //                            anchors {
                            //                                left: parent.left
                            //                                leftMargin: units.gu(2)
                            //                                verticalCenter: parent.verticalCenter
                            //                            }
                            text: i18n.tr("Basic")
                        }
                    }
                    ListItem {
                        color: UbuntuColors.blue
                        Label {
                            //                            anchors {
                            //                                left: parent.left
                            //                                leftMargin: units.gu(2)
                            //                                verticalCenter: parent.verticalCenter
                            //                            }
                            text: i18n.tr("Colored")
                        }
                    }
                    ListItem {
                        Label {
                            text: i18n.tr("No divider")
                        }
                        divider.visible: false
                    }
                    ListItem {
                        Label {
                            text: i18n.tr("Colored divider")
                        }
                        divider {
                            colorFrom: UbuntuColors.red
                            colorTo: UbuntuColors.green
                        }
                    }
                    // FIXME: Highlight doesn't work until we merge branch 75
                    ListItem {
                        Label {
                            text: i18n.tr("Highlight color")
                        }
                        highlightColor: UbuntuColors.purple
                        // no highlight without clicked() or leading/trailing actions
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

                    ListItem {
                        Label {
                            text: i18n.tr("Leading actions")
                        }
                        leadingActions: exampleLeadingActions
                    }
                    ListItem {
                        Label {
                            text: i18n.tr("Trailing actions")
                        }
                        trailingActions: exampleTrailingActions
                    }
                    ListItem {
                        Label {
                            text: i18n.tr("Leading and trailing actions")
                        }
                        leadingActions: exampleLeadingActions
                        trailingActions: exampleTrailingActions
                    }
                    ListItem {
                        Label {
                            text: i18n.tr("Custom action delegates")
                        }
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
                            delegate: Item {
                                width: height
                                Rectangle {
                                    anchors.fill: parent
                                    color: pressed ? Qt.darker(action.color, 1.2) : action.color
                                }
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
                                    text: "Move"
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
        }
    }
}
