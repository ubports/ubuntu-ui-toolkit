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

Rectangle {
    color: "transparent"
    id: tabsDelegate
    anchors.fill: parent

        clip: true

    property VisualItemModel tabModel: item.__tabsModel

    Rectangle {
        id: header
        z: 10
        color: "pink"
        anchors {
            left: parent.left
            right: parent.right
        }
        y: 0
        height: tabBar.height + separator.height //childrenRect.height

        function show() {
            header.y = 0;
        }

        function hide() {
            header.y = - header.height;
        }

        Column {
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }
            height: tabBar.height + separator.height

            NewTabBar {
                id: tabBar
                tabs: item
                height: itemStyle.tabBarHeight
                anchors {
                    left: parent.left
                    right: parent.right
                }
            }

            ListItem.Divider {
                id: separator
                height: 0
            }
        }

        property Tab selectedTab: item ? item.__selectedTab : null
        property Flickable selectedFlickable: null
        property real previousContentY: 0
        onSelectedTabChanged: updateFlickable()
        Component.onCompleted: updateFlickable()

        function updateFlickable() {
            if (selectedFlickable) selectedFlickable.contentYChanged.disconnect(header.scrollContents);
            if (selectedTab && selectedTab.__flickable !== null) selectedFlickable = selectedTab.__flickable;
            else selectedFlickable = null;
            if (!selectedFlickable) {
                //                tabView.anchors.top = headerSpace.bottom;
                return;
            }
            //            tabView.anchors.top = tabsDelegate.top;
            previousContentY = selectedFlickable.contentY;
            selectedFlickable.contentYChanged.connect(header.scrollContents);
        }

        function scrollContents() {
            print("y: "+selectedFlickable.contentY+", dy:");
            if (selectedFlickable.contentY > -header.height) {
                var deltaContentY = selectedFlickable.contentY - previousContentY;
                header.y = MathUtils.clamp(header.y - deltaContentY, -header.height, 0);

                //                if (previousContentY === 0) {
                //                    headerSpace.height = 0;
                //                    selectedFlickable.contentY = selectedFlickable.contentY - headerSpace.height;
                //                    selectedFlickable.height += headerSpace.height;
                //                } else if (selectedFlickable.contentY === 0) {
                //                    headerSpace.height = header.height;
                //                    selectedFlickable.contentY -= header.height;
                //                }
            }
            previousContentY = selectedFlickable.contentY;
        }
    }

    Rectangle {
        id: headerSpace
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: header.height

        color: "brown"
        z: -10

    }

    ListView {
        id: tabView
        anchors {
            //            top: headerSpace.bottom //(header.selectedFlickable === null) ? headerSpace.bottom : parent.top
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        interactive: itemStyle.swipeToSwitchTabs
        model: tabsDelegate.tabModel
        onModelChanged: tabView.updatePages()
        currentIndex: item.selectedTabIndex
        onCurrentIndexChanged: item.selectedTabIndex = tabView.currentIndex

        orientation: ListView.Horizontal
        snapMode: ListView.SnapOneItem
        boundsBehavior: Flickable.DragOverBounds
        highlightFollowsCurrentItem: true
        highlightRangeMode: ListView.StrictlyEnforceRange

        function updatePages() {
            var tab;
            if (!tabsDelegate.tabModel) return; // not initialized yet

            var tabList = tabsDelegate.tabModel.children
            for (var i=0; i < tabList.length; i++) {
                tab = tabList[i];
                tab.width = tabView.width;
                tab.height = tabView.height;
                tab.anchors.fill = undefined;
                if (tab.hasOwnProperty("__active")) tab.__active = true;
                if (tab.__flickable) {
                    // Set-up the top-margin of the contents of the tab so that
                    //  it is never hidden by the header.
                    tab.__flickable.topMargin = header.height;
                    tab.__flickable.contentY = -header.height;
                }
            }
            tabView.updateSelectedTabIndex();
        }

        function updateSelectedTabIndex() {
            if (tabView.currentIndex === item.selectedTabIndex) return;
            // The view is automatically updated, because highlightFollowsCurrentItem
            tabView.currentIndex = item.selectedTabIndex;
        }

        Connections {
            target: item
            onSelectedTabIndexChanged: tabView.updateSelectedTabIndex()
        }
    }

    onWidthChanged: tabView.updatePages();
    onHeightChanged: tabView.updatePages();
    Component.onCompleted: {
        tabView.updatePages();
    }

    //    Flickable {
    //        id: flicker
    //        anchors {
    //            top: header.bottom
    //            left: parent.left
    //            right: parent.right
    //            bottom: parent.bottom
    //        }

    //        property Flickable itemFlickable: (item !== null) ? item.__tabs[item.selectedTabIndex].flickable : null
    //        enabled: false // (itemFlickable !== null)
    //        contentHeight: itemFlickable ? itemFlickable.contentHeight + itemStyle.tabBarHeight : 0
    //        property real __headerVisibleHeight
    //        onContentYChanged: {
    //            var deltaContentY = contentY - __previousContentY;
    //            __previousContentY = contentY;

    //            print(deltaContentY);
    //            var newHeaderY = MathUtils.clamp(header.y - deltaContentY, -header.height, 0);
    //            if (newHeaderY === header.y) return;
    //            header.y = newHeaderY;

    //            return;

    //            // first decide if movement will prompt the page header to change height
    //            if ((deltaContentY < 0 && header.height >= 0) ||
    //                    (deltaContentY > 0 && header.height <= itemStyle.tabBarHeight)) {

    //                // calculate header height - but prevent bounce from changing it
    //                if (contentY > 0 && contentY < contentHeight - height) {
    //                    header.height = MathUtils.clamp(header.height - deltaContentY, 0, itemStyle.tabBarHeight);
    //                }
    //            }

    //            // now we move list position, taking into account page header height

    //            // BUG: With section headers enabled, the values of originY and contentY appear not
    //            // correct at the exact point originY changes. originY changes when the ListView
    //            // deletes/creates hidden delegates which are above the visible delegates.
    //            // As a result of this bug, you experience jittering scrolling when rapidly moving
    //            // around in large lists. See https://bugreports.qt-project.org/browse/QTBUG-27997
    //            // A workaround is to use a large enough cacheBuffer to prevent deletions/creations
    //            // so effectively originY is always zero.
    ////            itemFlickable.contentY = flicker.contentY + itemFlickable.originY - itemStyle.tabBarHeight + header.height;
    //        }

    //        property real __previousContentY: 0
    //    }

}
