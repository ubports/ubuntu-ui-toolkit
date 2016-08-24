/*
 * Copyright 2016 Canonical Ltd.
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
import Ubuntu.Components 1.0
import Ubuntu.Components.ListItems 1.0

MainView {
    width: units.gu(48)
    height: units.gu(60)
    objectName: "mainView"

    Page {

        ListModel {
            id: testModel

            ListElement {
                name: "listitem_destroyed_on_remove_with_confirm"
                label: "Item destroyed on remove with confirmation"
                confirm: true
            }
            ListElement {
                name: "listitem_destroyed_on_remove_without_confirm"
                label: "Item destroyed on remove without confirmation"
                confirm: false
            }
        }

        Column {
            anchors { fill: parent }

            Standard {
                objectName: "listitem_standard"
                confirmRemoval: true
                removable: true
                text: 'Slide to remove'
            }

            Empty {
                objectName: "listitem_empty"
            }

            Standard {
                objectName: "listitem_without_confirm"
                confirmRemoval: false
                removable: true
                text: "Item without delete confirmation"
            }

            ListView {
                anchors { left: parent.left; right: parent.right }
                height: childrenRect.height
                model: testModel

                delegate: Standard {
                    removable: true
                    confirmRemoval: confirm
                    onItemRemoved: testModel.remove(index)
                    text: label
                    objectName: name
                }
            }
        }
    }
}
