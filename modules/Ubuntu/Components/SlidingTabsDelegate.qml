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

Item {
    id: slidingTabsDelegate
    anchors.fill: parent

    onWidthChanged: listView.updatePages();
    onHeightChanged: listView.updatePages();

    Component.onCompleted: listView.updatePages();

    clip: true

    // TODO: Remove after debugging
    Rectangle {
        color: "yellow"
        anchors.fill: parent
        opacity: 0.4
    }

    ListView {
        id: listView
        anchors.fill: parent
        model: item.__pagesModel
        onModelChanged: updatePages();
        orientation: ListView.Horizontal
        snapMode: ListView.SnapOneItem
        boundsBehavior: Flickable.DragOverBounds

        function updatePages() {
            var page;
            var pageList = item.__pagesModel.children
            for (var i=0; i < pageList.length; i++) {
                page = pageList[i];
                page.width = slidingTabsDelegate.width;
                page.height = slidingTabsDelegate.height;
                page.anchors.fill = null;
                if (page.hasOwnProperty("__active")) page.__active = true;
            }
//            listView.updateSelectedTabIndex();
        }
        onMovingChanged: {
            if(!moving) {
                // update the currentItem
                var index = contentX / slidingTabsDelegate.width;
                if (currentIndex !== index) {
                    currentIndex = index;
//                    selectedTabIndex = currentIndex;
//                    print("contentX = "+contentX);
//                    print("current index = "+currentIndex);
//                    print("indexAt = " + indexAt(contentX, contentY));
                }
            }
        }

//        function updateSelectedTabIndex() {
//            print("new index = "+item.selectedTabIndex);
//            listView.currentIndex = item.selectedTabIndex;
//            listView.positionViewAtIndex(item.selectedTabIndex, ListView.Center);
//            print("contentX = "+contentX);
//        }

        Connections {
            target: item
//            onSelectedTabIndexChanged: listView.updateSelectedTabIndex()
        }

//        Component.onCompleted: listView.updateSelectedTabIndex()

    }

}
