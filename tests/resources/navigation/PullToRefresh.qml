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

    Column {
        anchors.fill: parent
        spacing: units.gu(0.5)
        Rectangle {
            clip: true
            width: parent.width
            height: units.gu(30)
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
                    model: listModel
                    refreshMethod: "refresh"
                }
            }
        }

        Flickable {
            id: flicker
            width: parent.width
            height: units.gu(30)

            clip: true
            contentHeight: rect.height
            contentWidth: rect.width
            RefreshControl {
                parent: flicker
                model: listModel
                refreshMethod: "refresh"
            }

            Rectangle {
                id: rect
                width: units.gu(100)
                height: units.gu(100)
                gradient: Gradient {
                    GradientStop {
                        color: "green"
                        position: 0.2
                    }
                    GradientStop {
                        color: "red"
                        position: 1.0
                    }
                }
            }
        }
    }
}
