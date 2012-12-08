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

    Rectangle {
        id: header
        z: 10
        color: "pink"
        anchors {
            left: parent.left
            right: parent.right
        }
        y: 0
        height: childrenRect.height

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
            }
        }
    }

    ListView {
        id: tabView
        anchors {
            //            top: header.bottom
            top:  parent.top
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

    Item {
        id: scroller
        property Tab selectedTab: item ? item.__selectedTab : null
        property Flickable selectedFlickable: null
        property real previousContentY: 0
        onSelectedTabChanged: updateFlickable()
        Component.onCompleted: updateFlickable()

        function updateFlickable() {
            if (selectedFlickable) selectedFlickable.contentYChanged.disconnect(scroller.scroll);
            if (selectedTab && selectedTab.__flickable !== null) selectedFlickable = selectedTab.__flickable;
            else selectedFlickable = null;
            print("selected flickable "+selectedFlickable);
            if (!selectedFlickable) return;
            //            selectedFlickable.boundsBehavior = Flickable.StopAtBounds;
            selectedFlickable.topMargin = header.height;
            previousContentY = selectedFlickable.contentY;
            selectedFlickable.contentYChanged.connect(scroller.scroll);

        }

        function scroll() {
            if (selectedFlickable.contentY > 0) {
                var deltaContentY = selectedFlickable.contentY - previousContentY;
                previousContentY = selectedFlickable.contentY;
                header.y = MathUtils.clamp(header.y - deltaContentY, -header.height, 0);
            }
        }

        function showHeader() {
            header.y = 0;
        }
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
