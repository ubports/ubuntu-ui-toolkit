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
