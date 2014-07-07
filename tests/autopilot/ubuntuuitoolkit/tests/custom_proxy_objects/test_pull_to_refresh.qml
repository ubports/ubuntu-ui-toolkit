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
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem

MainView {
    id: root
    width: units.gu(48)
    height: units.gu(60)

    ListModel {
        id: listModel
        property bool refreshing: refreshComplete.running

        function modelData(index) {
            return {'name': 'line #' + index}
        }

        function reload() {
            refreshComplete.restart();
            refreshedLabel.text = 'Refreshed.'
        }

        function fillModel() {
            clear();
            for (var i = 0; i < 50; i++) {
                append(modelData(i));
            }
        }

        Component.onCompleted: fillModel()
    }

    Timer {
        id: refreshComplete
        interval: 1000
        onTriggered: {
            listModel.fillModel();
        }
    }

    Column {

        anchors.fill: parent
        Label {
            id: refreshedLabel
            objectName: 'refreshedLabel'
            text: 'Not refreshed.'
        }

        Flickable {
            id: view
            objectName: 'flickableWithPullToRefresh'
            width: parent.width
            height: parent.height - refreshedLabel.paintedHeight
            clip: true
            contentWidth: column.childrenRect.width
            contentHeight: column.childrenRect.height
            Column {
                id: column
                Repeater {
                    model: listModel
                    ListItem.Standard {
                        width: root.width
                        height: units.gu(5)
                        text: index + ' - ' + modelData
                        onClicked: {
                            listModel.reload();
                        }
                    }
                }
            }

            PullToRefresh {
                id: pullToRefresh
                objectName: 'InFlickable'
                parent: view
                refreshing: listModel.refreshing
                onRefresh: listModel.reload()
            }
        }

        Flickable {
            objectName: 'flickableWithoutPullToRefresh'
        }
    }
}
