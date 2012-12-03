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

    NewTabBar {
        id: tabBar
        anchors {
            top: orangebar.bottom
            left: parent.left
            right: parent.right
        }
        tabs: item
    }

    ListView {
        id: tabView
        anchors {
            top: tabBar.bottom
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

    function updateSeparators() {
        if (itemStyle.separator) {
            itemStyle.separator.parent = tabsDelegate;
            itemStyle.separator.anchors.top = tabBar.bottom;
            itemStyle.separator.anchors.left = tabsDelegate.left;
            itemStyle.separator.anchors.right = tabsDelegate.right;
            tabView.anchors.top = itemStyle.separator.bottom;
        } else {
            // no separator
            tabView.anchors.top = tabBar.bottom;
        }

        if (itemStyle.topSeparator) {
            itemStyle.topSeparator.parent = tabsDelegate;
            itemStyle.topSeparator.anchors.top = tabsDelegate.top;
            itemStyle.topSeparator.anchors.left = tabsDelegate.left;
            itemStyle.topSeparator.anchors.right = tabsDelegate.right;
            tabBar.anchors.top = itemStyle.topSeparator.bottom;
        } else {
            // no top separator
            tabBar.anchors.top = tabsDelegate.top;
        }
    }

    onWidthChanged: tabView.updatePages();
    onHeightChanged: tabView.updatePages();
    Component.onCompleted: {
        tabsDelegate.updateSeparators();
        tabView.updatePages();
    }
}
