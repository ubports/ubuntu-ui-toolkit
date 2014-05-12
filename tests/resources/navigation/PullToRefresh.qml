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

import QtQuick 2.2
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 0.1 as ListItem

MainView {
    id: root
    width: units.gu(40)
    height: units.gu(71)

    PageStack {
        id: pageStack
        Component.onCompleted: pageStack.push(page)
    }

    Component {
        id: page

        Page {
            title: "RefreshControl"
            ListModel {
                id: listModel
                property bool done: false

                function modelData(index) {
                    return {"name": "line #" + index}
                }

                function refresh() {
                    print("Refresh model...")
                    done = false;
                    clear();
                    refreshDelay.restart()
                }

                function fillModel() {
                    for (var i = 0; i < 50; i++) {
                        append(modelData(i));
                    }
                }

                Component.onCompleted: fillModel()
            }

            Timer {
                id: refreshDelay
                interval: 2000
                onTriggered: {
                    listModel.done = true;
                    listModel.fillModel();
                    //refreshControl.complete();
                }
            }

            UbuntuListView {
                id: view
                anchors.fill: parent
                model: listModel
                delegate: Rectangle {
                    color: "green"
                    width: ListView.view.width
                    height: units.gu(5)
                    ListItem.Standard {
                        anchors.fill: parent
                        text: modelData
                        onClicked: {
                            listModel.refresh()
                        }
                    }
                }

                RefreshControl {
                    id: refreshControl
                    height: units.gu(7)
                    completeWhen: listModel.done
                    onRefresh: listModel.refresh()
                }
            }
        }
    }
}
