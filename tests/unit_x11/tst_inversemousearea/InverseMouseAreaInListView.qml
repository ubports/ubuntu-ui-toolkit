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

import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.0

MainView {
    id: root
    width: units.gu(40)
    height: units.gu(71)

    property InverseMouseArea ima: null

    Component {
        id: editor
        TextArea {
        }
    }

    Page {
        title: "Test"

        ListView {
            id: list
            objectName: "ListView"
            anchors.fill: parent
            model: 1

            delegate: Empty {
                objectName: "Card"
                width: parent.width - units.gu(5)
                anchors.horizontalCenter: parent.horizontalCenter
                height: units.gu(50)

                Loader {
                    anchors.fill: parent
                    sourceComponent: editor
                }

                InverseMouseArea {
                    id: imaItem
                    objectName: "Test_IMA"
                    anchors.fill: parent
                    topmostItem: true
                    Component.onCompleted: root.ima = imaItem
                }
            }
        }
    }
}
