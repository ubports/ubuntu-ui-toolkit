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

Item {
    id: slidingTabsDelegate
    anchors.fill: parent

    clip: true

    // TODO: Remove after debugging
    Rectangle {
        color: "yellow"
        anchors.fill: parent
        opacity: 0.4
    }

    property VisualItemModel tabModel: item.__pagesModel

//    onTabModelChanged: {
//        print("updating tab model");
//        listView.updatePages();
//    }
//    VisualItemModel {
//        id: itemModel
//        Tab {
//            width: 500; height: 300
//            page: Rectangle { color: "red"; anchors.fill: parent }
//        }
//        Tab {
//            width: 500; height: 300
//            page: Rectangle { color: "white"; anchors.fill: parent }
//        }
//        Tab {
//            width: 500; height: 300
//            Rectangle { color: "blue"; anchors.fill: parent }
//        }
//    }


    ListView {
        id: listView
        anchors.fill: parent
        model: slidingTabsDelegate.tabModel
        onModelChanged: listView.updatePages();

        clip: true

        orientation: ListView.Horizontal
        snapMode: ListView.SnapOneItem
//        snapMode: ListView.SnapToItem
        boundsBehavior: Flickable.DragOverBounds
        highlightFollowsCurrentItem: true


        function updatePages() {
            var tab;
            var tabList = slidingTabsDelegate.tabModel.children
            print("w*h = "+slidingTabsDelegate.width+", "+slidingTabsDelegate.height);
            for (var i=0; i < tabList.length; i++) {
                tab = tabList[i];
                tab.width = slidingTabsDelegate.width;
                tab.height = slidingTabsDelegate.height;
                tab.anchors.fill = null;
                if (tab.hasOwnProperty("__active")) tab.__active = true;
            }
            print(tabList.length + " pages. updating tab index");
            listView.updateSelectedTabIndex();
        }
        onMovingChanged: {
            if(!moving) {
                // update the currentItem
                listView.currentIndex = contentX / slidingTabsDelegate.width;
                item.selectedTabIndex = listView.currentIndex;
            }
        }

        function updateSelectedTabIndex() {
            print("currentIndex = "+currentIndex+", selected tab = "+item.selectedTabIndex);
            if (listView.currentIndex === item.selectedTabIndex) return;
            // The view is automatically updated, because highlightFollowsCurrentItem
            listView.currentIndex = item.selectedTabIndex;
        }

        Connections {
            target: item
            onSelectedTabIndexChanged: listView.updateSelectedTabIndex()
        }

        Component.onCompleted: {
            listView.updatePages();
            listView.positionViewAtIndex(1, ListView.Beginning);
        }

    }

    onWidthChanged: listView.updatePages();
    onHeightChanged: listView.updatePages();
    Component.onCompleted: listView.updatePages();
}
