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

import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1

MainView {
    id: root
    width: units.gu(40)
    height: units.gu(71)

    property InverseMouseArea ima: null

    Component {
        id: editor
        TextArea {
            text: QuickUtils.consoleLog
        }
    }

    Page {
        title: "Test"

        ListView {
            id: list
            objectName: "ListView"
            anchors.fill: parent
            model: ListModel {
                Component.onCompleted: {
                    append({"contentData": editor})
                }
            }

            delegate: Empty {
                objectName: "Card"
                width: parent.width - units.gu(5)
                anchors.horizontalCenter: parent.horizontalCenter
                height: units.gu(50)

                Loader {
                    anchors.fill: parent
                    sourceComponent: contentData
                }

                InverseMouseArea {
                    id: ima
                    objectName: "Test_IMA"
                    anchors.fill: parent
                    topmostItem: true
                    propagateComposedEvents: true

                    onPressed: QuickUtils.log("IMA pressed")
                    onClicked: QuickUtils.log("IMA clicked")
                    Component.onCompleted: root.ima = ima
                }
            }
        }
    }
}
