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
        property bool refreshing: refreshComplete.running

        function modelData(index) {
            return {"name": "line #" + index}
        }

        function reload() {
            print("Refresh model...")
            refreshComplete.restart();
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
            print("Done")
        }
    }

    PageStack {
        id: pageStack
        Component.onCompleted: pageStack.push(inListView)
    }

    Component {
        id: inListView

        Page {
            id: page
            title: "In ListView"
            UbuntuListView {
                id: view
                anchors.fill: parent
                model: listModel

                pullToRefresh {
                    objectName: "ULV"
                    enabled: page.active
                    refreshing: model.refreshing
                    onRefresh: model.reload()
                }

                delegate: Rectangle {
                    color: "green"
                    width: ListView.view.width
                    height: units.gu(5)
                    ListItem.Standard {
                        anchors.fill: parent
                        text: modelData
                        onClicked: {
                            if (index % 2)
                                view.model.reload();
                            else
                                pageStack.push(inFlickable);
                        }
                    }
                }
            }
        }
    }

    Component {
        id: inFlickable

        Page {
            id: page
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
                            text: index + " - " + modelData
                            onClicked: {
                                if (index % 2)
                                    listModel.reload();
                                else
                                    pageStack.push(xmlModel)
                            }
                        }
                    }
                }

                PullToRefresh {
                    id: pullToRefresh
                    enabled: page.active
                    objectName: "InFlickable"
                    parent: view
                    refreshing: listModel.refreshing
                    onRefresh: listModel.reload()
                }
            }
        }
    }

    Component {
        id: xmlModel

        Page {
            id: page
            title: "TechCrunch"
            UbuntuListView {
                anchors.fill: parent
                pullToRefresh {
                    objectName: "XmlList"
                    refreshing: model.status === XmlListModel.Loading
                    onRefresh: model.reload()
                }
                model: XmlListModel {
//                    source: "http://feeds.reuters.com/reuters/topNews"
                    source: "http://feeds.feedburner.com/TechCrunch/"
                    query: "/rss/channel/item"
                    XmlRole { name: "title"; query: "title/string()" }
                }
                delegate: ListItem.Standard {
                    width: ListView.view.width
                    height: units.gu(5)
                    text: modelData
                    onClicked: {
                        ListView.view.model.reload();
                    }
                }
            }
        }
    }
}
