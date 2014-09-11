/*
 * Copyright 2014 Canonical Ltd.
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
import Ubuntu.Components 1.1

MainView {
    id: main
    width: units.gu(50)
    height: units.gu(100)

    property bool override: false

    Action {
        objectName: "stock"
        id: stock
    }

    ListItemOptions {
        id: leading
        Action {
        }
        Action {
        }
        Action {
        }
    }

    Column {
        anchors {
            left: parent.left
            right: parent.right
        }

        ListItem {
            id: testItem
            contentItem.color: "lime"
            onClicked: {
                print("click")
                main.override = !main.override
            }
            Label {
                anchors.fill: parent
                text: units.gridUnit + "PX/unit"
            }
            leadingOptions: ListItemOptions {
                options: [stock]
            }
            trailingOptions: leading
        }

        ListView {
            id: view
            clip: true
            width: parent.width
            height: units.gu(20)
            model: 100
            pressDelay: 0
            delegate: ListItem {
                id: listItem
                onClicked: print(" clicked")
                leadingOptions: leading
                Label {
                    text: modelData + " item"
                }
                states: State {
                    name: "override"
                    when: main.override
                    PropertyChanges {
                        target: listItem.contentItem
                        pressedColor: "brown"
                    }
                }
            }
        }
        Flickable {
            id: flicker
            width: parent.width
            height: units.gu(20)
            clip: true
            contentHeight: column.childrenRect.height
            Column {
                id: column
                width: view.width
                Repeater {
                    model: 100
                    ListItem {
                        contentItem {
                            color: "red"
                            pressedColor: "lime"
                        }
                        divider.colorFrom: UbuntuColors.green

                        Label {
                            text: modelData + " Flickable item"
                        }
                        onClicked: divider.visible = !divider.visible
                    }
                }
            }
        }
    }
}
