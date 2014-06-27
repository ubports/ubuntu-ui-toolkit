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

import QtQuick 2.2
import QtQuick.XmlListModel 2.0
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0

MainView {
    id: root
    width: units.gu(40)
    height: units.gu(71)

    PageStack {
        id: stack;
        Component.onCompleted: push(page1Holder)
    }

    Component {
        id: page1Holder
        Page {
            id: page
            title: "Design"
            ListView {
                id: view
                anchors.fill: parent
                PullToRefresh {
                    id: pullToRefresh
                    objectName: "XmlList"
                    enabled: page.active
                    refreshing: view.model.status === XmlListModel.Loading
                    onRefresh: view.model.reload()
                    content: Item {
                        Icon {
                            name: pullToRefresh.releaseToRefresh ? "search" : ""
                            height: parent.height
                            width: height
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                    }
                }
                model: XmlListModel {
                    source: "http://design.canonical.com/feed/"
                    query: "/rss/channel/item"
                    XmlRole { name: "title"; query: "title/string()" }
                }
                delegate: Standard {
                    width: ListView.view.width
                    height: units.gu(5)
                    text: modelData
                    onClicked: {
                        stack.push(page2Holder)
                    }
                }
            }
        }
    }


    Component {
        id: page2Holder
        Page {
            id: page2
            title: "Design - Flickable"
            Flickable {
                id: view2
                anchors.fill: parent
                contentHeight: content.childrenRect.height
                PullToRefresh {
                    parent: view2
                    id: refresh2
                    refreshing: repeater.model.status === XmlListModel.Loading
                    onRefresh: repeater.model.reload()
                }

                Column {
                    id: content
                    width: parent.width
                    Repeater {
                        id: repeater
                        model: XmlListModel {
                            source: "http://design.canonical.com/feed/"
                            query: "/rss/channel/item"
                            XmlRole { name: "title"; query: "title/string()" }
                        }
                        Standard {
                            width: view2.width
                            height: units.gu(5)
                            text: modelData
                            onClicked: {
                                stack.push(page1Holder)
                            }
                        }
                    }
                }
            }
        }
    }
}
