/*
 * Copyright 2012 Canonical Ltd.
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


MainView {
    id: root
    width: units.gu(40)
    height: units.gu(71)

    ListModel {
        id: listModel

        function modelData(index) {
            return {"name": "line #" + index}
        }

        function refresh() {
            print("Refresh model...")
            refreshDelay.restart()
        }

        Component.onCompleted: {
            for (var i = 0; i < 50; i++) {
                append(modelData(i));
            }
        }
    }

    Timer {
        id: refreshDelay
        interval: 2000
        onTriggered: refreshControl.refreshEnds()
    }

    Rectangle {
        anchors.fill: parent
        anchors.topMargin: units.gu(10)
        border.color: "black"
        ListView {
            id: view
            anchors.fill: parent
            model: listModel
            delegate: Rectangle {
                color: "green"
                width: ListView.view.width
                height: units.gu(5)
                Label {
                    anchors.fill: parent
                    text: modelData
                    color: "white"
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }
            }

            RefreshControl {
                id: refreshControl
                target: listModel
                refreshMethod: "refresh"
            }
        }
    }
}
