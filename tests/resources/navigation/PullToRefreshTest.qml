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

    Page {
        id: page
        title: "Design"
        UbuntuListView {
            id: view
            anchors.fill: parent
            pullToRefresh {
                objectName: "XmlList"
                enabled: page.active
                refreshing: model.status === XmlListModel.Loading
                onRefresh: model.reload()
                contentItem: Item {
                    Icon {
                        name: view.pullToRefresh.releaseToRefresh ? "search" : ""
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
