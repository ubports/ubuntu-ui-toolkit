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
import QtQuick.XmlListModel 2.0
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem

MainView {
    id: root
    width: units.gu(40)
    height: units.gu(71)

    ListModel {
        id: listModel
        property bool ready: !refreshDelay.running
        signal completed()

        onReadyChanged: if(ready) completed()

        function modelData(index) {
            return {"name": "line #" + index}
        }

        function reload() {
            print("Refresh model...")
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
            listModel.fillModel();
        }
    }

    PageStack {
        id: pageStack
        Component.onCompleted: pageStack.push(inListView)
    }

    Component {
        id: inListView

        Page {
            title: "In ListView"
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
                            pageStack.push(inFlickable)
                        }
                    }
                }

                RefreshControl {
                    id: refreshControl
                    enabled: view.visible
                }
            }
        }
    }

    Component {
        id: inFlickable

        Page {
            title: "In Flickable"
            Flickable {
                id: view
                anchors.fill: parent
                contentWidth: column.childrenRect.width
                contentHeight: column.childrenRect.height
                Column {
                    id: column
                    Repeater {
                        model: listModel
                        ListItem.Standard {
                            width: root.width
                            height: units.gu(5)
                            text: modelData
                            onClicked: {
                                pageStack.push(xmlModel)
                            }
                        }
                    }
                }

                RefreshControl {
                    id: refreshControl
                    parent: view
                    refreshing: !listModel.ready
                    onRefresh: listModel.reload()
                }
            }
        }
    }

    Component {
        id: xmlModel

        Page {
            id: page
            title: "Using XmlListModel"
            UbuntuListView {
                anchors.fill: parent
                model: XmlListModel {
                    source: "http://feeds.reuters.com/reuters/topNews"
                    query: "/rss/channel/item"
                    XmlRole { name: "title"; query: "title/string()" }

                    onStatusChanged: if (status === XmlListModel.Ready) completed()
                    signal completed()
                }
                delegate: ListItem.Standard {
                    width: ListView.view.width
                    height: units.gu(5)
                    text: modelData
                    onClicked: {
                        pageStack.push(inFlickable)
                    }
                }
                RefreshControl {
                    enabled: page.active
                }
            }
        }
    }
}
