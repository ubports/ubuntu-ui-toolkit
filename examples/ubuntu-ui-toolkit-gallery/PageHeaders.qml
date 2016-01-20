/*
 * Copyright (C) 2015 Canonical Ltd.
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
import Ubuntu.Components 1.3

Template {
    id: page
    header: standardHeader

    TemplateSection {
        title: "Page Header"
        className: "PageHeader"

        Label {
            text: "Use the icons in the header."
        }
    }

    PageHeader {
        id: standardHeader
        visible: page.header === standardHeader
        title: "Default title"
        trailingActionBar.actions: [
            Action {
                iconName: "search"
                text: "Search"
                onTriggered: page.header = searchHeader
            },
            Action {
                iconName: "edit"
                text: "Edit"
                onTriggered: page.header = editHeader
            }
        ]
    }

    PageHeader {
        id: searchHeader
        visible: page.header === searchHeader
        leadingActionBar.actions: [
            Action {
                iconName: "back"
                text: "Back"
                onTriggered: page.header = standardHeader
            }
        ]
        contents: TextField {
            anchors {
                left: parent.left
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            placeholderText: "Search..."
        }
    }

    PageHeader {
        id: editHeader
        visible: page.header === editHeader
        property Component delegate: Component {
            // FIXME: Replace this delegate with the new
            //  text button when it becomes available.
            //  Also update example code in Toolbar.qml.
            AbstractButton {
                id: button
                action: modelData
                width: label.width + units.gu(4)
                height: parent.height
                Rectangle {
                    color: UbuntuColors.slate
                    opacity: 0.1
                    anchors.fill: parent
                    visible: button.pressed
                }
                Label {
                    anchors.centerIn: parent
                    id: label
                    text: action.text
                    font.weight: text === "Confirm" ? Font.Normal : Font.Light
                }
            }
        }

        leadingActionBar {
            anchors.leftMargin: 0
            actions: Action {
                text: "Cancel"
                iconName: "close"
                onTriggered: page.header = standardHeader
            }
            delegate: editHeader.delegate
        }
        trailingActionBar {
            anchors.rightMargin: 0
            actions: Action {
                text: "Confirm"
                iconName: "tick"
                onTriggered: page.header = standardHeader
            }
            delegate: editHeader.delegate
        }

        extension: Toolbar {
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            trailingActionBar.actions: [
                Action { iconName: "bookmark-new" },
                Action { iconName: "add" },
                Action { iconName: "edit-select-all" },
                Action { iconName: "edit-copy" },
                Action { iconName: "select" }
            ]
            leadingActionBar.actions: Action {
                iconName: "delete"
                text: "delete"
                onTriggered: print("Delete action triggered")
            }
        }
    }
}
