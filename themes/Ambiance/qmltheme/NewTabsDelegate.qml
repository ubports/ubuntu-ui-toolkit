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
    // styling properties
    /*!
      If this optional property is specified, it will be positioned
      between the bar with tab buttons, and the tab pages to act
      as a separator.
     */
    property Item separator

    /*!
      Enable left/right swiping in the contents area to go to
      the next/previous tab.
     */
    property bool swipeToSwitchTabs
    onSwipeToSwitchTabsChanged: print("swipeToSwitchTabs property is DEPRECATED. Please do not rely on swiping the Page's contents to switch tabs, this functionality will be removed.")

    property color headerTextColor
    property color headerTextSelectedColor
    property real headerTextOpacity
    property real headerTextSelectedOpacity

    property int headerTextFadeDuration
    property string headerFontSize
    property int headerFontWeight
    property real headerTextLeftMargin
    property real headerTextRightMargin
    property real headerTextBottomMargin

    property url indicatorImageSource

    property real tabBarHeight

    /*!
      The time of inactivity in ms before the tab bar is automatically deactivated
     */
    property int deactivateTime: 1000

    /*!
      The velocity at which buttons are repositioned when a new tab gets selected.
     */
    property real buttonPositioningVelocity

    // visuals
    id: tabsDelegate
    anchors.fill: parent

    property VisualItemModel tabModel: item.__tabsModel

    // use theTabs property because item gives problems in the loader
    property Tabs theTabs: item
    property Component headerContents: Component {
        NewTabBar {
            id: tabBar
            tabs: theTabs
            anchors {
                top: parent ? parent.top : undefined
                left: parent ? parent.left : undefined
                right: parent ? parent.right : undefined
            }
            //height: tabBarHeight
            headerTextColor: tabsDelegate.headerTextColor
            headerTextSelectedColor: tabsDelegate.headerTextSelectedColor
            headerTextOpacity: tabsDelegate.headerTextOpacity
            headerTextSelectedOpacity: tabsDelegate.headerTextSelectedOpacity
            headerTextFadeDuration: tabsDelegate.headerTextFadeDuration
            indicatorImageSource: tabsDelegate.indicatorImageSource
            headerFontSize: tabsDelegate.headerFontSize
            headerFontWeight: tabsDelegate.headerFontWeight
            headerTextLeftMargin: tabsDelegate.headerTextLeftMargin
            headerTextRightMargin: tabsDelegate.headerTextRightMargin
            headerTextBottomMargin: tabsDelegate.headerTextBottomMargin
            buttonPositioningVelocity: tabsDelegate.buttonPositioningVelocity
            deactivateTime: tabsDelegate.deactivateTime
        }
    }

    ListView {
        id: tabView
        anchors.fill: parent

        interactive: tabsDelegate.swipeToSwitchTabs
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
            }
            tabView.updateSelectedTabIndex();
        }

        function updateSelectedTabIndex() {
            if (tabView.currentIndex === item.selectedTabIndex) return;
            // The view is automatically updated, because highlightFollowsCurrentItem
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
    Component.onCompleted: {
        item.__headerContents = headerContents;
        tabView.updatePages();
    }
}
