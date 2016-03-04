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

import QtQuick 2.4
import Ubuntu.Components 1.3

MainView {
    width: units.gu(48)
    height: units.gu(20)
    objectName: "mainView"

    Page {
        header: Item { }

        Column {
            id: column
            width: units.gu(48)
            height: units.gu(20)

            Label {
                id: clickedLabel
                objectName: "clickedLabel"
                text: "No element clicked."
            }

            ListView {
                id: testListView
                objectName: "testListView"
                anchors {
                    left: parent.left
                    right: parent.right
                }
                height: column.height - clickedLabel.paintedHeight
                clip: true
                model: 20

                delegate: ListItem {
                    objectName: "testListElement%1".arg(index)
                    Label {
                        anchors.centerIn: parent
                        text: "test list element %1".arg(index)
                    }
                    onClicked: clickedLabel.text = objectName
                    height: units.gu(5)
                }
            }
        }
    }
}
