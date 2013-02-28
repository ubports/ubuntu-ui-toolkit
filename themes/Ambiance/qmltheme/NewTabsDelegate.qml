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
import Ubuntu.Components.ListItems 0.1 as ListItem

Item {
    id: tabsDelegate
    anchors.fill: parent

    clip: true

    property VisualItemModel tabModel: item.__tabsModel

//    Item {
//        id: header
//        z: 1 // header is on top of the tab's contents.

//        Image {
//            anchors {
//                left: parent.left
//                right: parent.right
//                top: parent.top
//                bottom: separator.bottom
//            }
//            source: "artwork/background_paper.png"
//            fillMode: Image.Tile
//        }

//        anchors {
//            left: parent.left
//            right: parent.right
//        }
//        y: 0

//        Behavior on y {
//            enabled: !(header.selectedFlickable && header.selectedFlickable.moving)
//            SmoothedAnimation {
//                duration: 200
//            }
//        }

//        height: tabBar.height + separator.height + separatorBottom.height

//        function show() {
//            header.y = 0;
//        }

//        function hide() {
//            header.y = - header.height;
//        }

    // use theTabs property because item gives problems in the loader
    property Tabs theTabs: item
    property Component headerContents: Component {
        NewTabBar {
            id: tabBar
            tabs: theTabs
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }
        }
    }

//        // FIXME: Define the separator in the theme when this bug is fixed:
//        // https://bugs.launchpad.net/goodhope/+bug/1089614
//        BorderImage {
//            id: separator
//            anchors {
//                top: tabBar.bottom
//                left: parent.left
//                right: parent.right
//            }
//            source: "artwork/PageHeaderBaseDividerLight.sci"
//        }
//        Image {
//            id: separatorBottom
//            anchors {
//                top: separator.bottom
//                left: parent.left
//                right: parent.right
//            }
//            source: "artwork/PageHeaderBaseDividerBottom.png"
//        }

//        property Tab selectedTab: item ? item.selectedTab : null
//        // use updateFlickable() to update selectedFlickable so that events from the
//        // previous selectedFlickable can be disconnected.
//        property Flickable selectedFlickable: null
//        property real previousContentY: 0
//        onSelectedTabChanged: updateFlickable()
//        Component.onCompleted: updateFlickable();

//        function updateFlickable() {
//            if (selectedFlickable) {
//                selectedFlickable.contentYChanged.disconnect(header.scrollContents);
//                selectedFlickable.movementEnded.disconnect(header.movementEnded);
//            }
//            if (selectedTab && selectedTab.autoHideTabBar && selectedTab.__flickable) {
//                selectedFlickable = selectedTab.__flickable;
//                previousContentY = selectedFlickable.contentY;
//                selectedFlickable.contentYChanged.connect(header.scrollContents);
//                selectedFlickable.movementEnded.connect(header.movementEnded);
//            } else {
//                selectedFlickable = null;
//            }
//        }

//        function scrollContents() {
//            // Avoid updating header.y when rebounding or being dragged over the bounds.
//            if (!selectedFlickable.atYBeginning && !selectedFlickable.atYEnd) {
//                var deltaContentY = selectedFlickable.contentY - previousContentY;
//                header.y = MathUtils.clamp(header.y - deltaContentY, -header.height, 0);
//            }
//            previousContentY = selectedFlickable.contentY;
//        }

//        function movementEnded() {
//            if (selectedFlickable.contentY < 0) header.show();
//            else if (header.y < -header.height/2) header.hide();
//            else header.show();
//        }
//    }

    ListView {
        id: tabView
        anchors.fill: parent

        interactive: itemStyle.swipeToSwitchTabs
        model: tabsDelegate.tabModel
        onModelChanged: tabView.updatePages()
        currentIndex: item.selectedTabIndex
        onCurrentIndexChanged: if (item.__tabsModel.count > 0) item.selectedTabIndex = tabView.currentIndex

        orientation: ListView.Horizontal
        snapMode: ListView.SnapOneItem
        boundsBehavior: Flickable.DragOverBounds
        highlightFollowsCurrentItem: true
        highlightRangeMode: ListView.StrictlyEnforceRange

        function updatePages() {
            if (!tabsDelegate.tabModel) return; // not initialized yet

            var tabList = tabsDelegate.tabModel.children
            var tab;
            for (var i=0; i < tabList.length; i++) {
                tab = tabList[i];
                tab.anchors.fill = undefined;
                tab.width = tabView.width;
                tab.height = tabView.height

                if (tab.hasOwnProperty("__active")) tab.__active = true;
            }
            tabView.updateSelectedTabIndex();
        }

        function updateSelectedTabIndex() {
            if (tabView.currentIndex === item.selectedTabIndex) return;
            // The view is automatically updated, because highlightFollowsCurrentItem
            print("setting tabView.currentIndex = "+item.selectedTabIndex);
            tabView.currentIndex = item.selectedTabIndex;
        }
    }

    Connections {
        target: item
        onSelectedTabIndexChanged: {
            tabView.updateSelectedTabIndex();
        }
    }

    onWidthChanged: tabView.updatePages();
    onHeightChanged: tabView.updatePages();
    Component.onCompleted: tabView.updatePages();
}
