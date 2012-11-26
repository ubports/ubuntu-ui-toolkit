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
    id: slidingTabsDelegate
    anchors.fill: parent

        clip: true

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


    Rectangle {
        id: orangebar
        color: "#c94212"
        anchors {
            left: parent.left
            right: parent.right
            top:parent.top
        }
        height: units.dp(2)
    }

    Rectangle {
        id: tabBar
        color: "transparent"
        anchors {
            top: orangebar.bottom
            left: parent.left
            right: parent.right
        }
        height: units.gu(6)

        property bool active: false
        onActiveChanged: buttonView.position()

        ListView {
            id: buttonView
            anchors.fill: parent
            model: slidingTabsDelegate.tabModel.children

            clip: false

            orientation: ListView.Horizontal
            snapMode: ListView.NoSnap

            Connections {
                target: item
                onSelectedTabIndexChanged: buttonView.position()
            }

            function position() {
                if (tabBar.active) {
                    if (buttonView.currentIndex > 0) {
                        // move the current button a bit to the right so that the user can see
                        // that it is possible to scroll left
                        buttonView.contentX -= units.gu(4);
                    }
                } else {
                    // not active, move the button of the current tab to the left
                    buttonView.currentIndex = item.selectedTabIndex;
                    buttonView.contentX = buttonView.currentItem.x;
                }
            }

            delegate: AbstractButton {
                id: tabButton
                width: text.width + 2*text.anchors.margins
                height: parent.height

                visible:  tabBar.active || selected
//                ItemStyle.class: "transparent-button"
                property bool selected: (index === item.selectedTabIndex)

                TextCustom {
                    anchors.centerIn: parent
                    anchors.margins: units.gu(2)
                    id: text
                    text: modelData.title
                    fontSize: "x-large"
                }

//                text: modelData.title
//                iconSource: modelData.iconSource
                onClicked: {
                    item.selectedTabIndex = index;
                    tabBar.active = false;
                }
            }
        }

        MouseArea {
            // an inactive tabBar can be clicked to make it active
            anchors.fill: parent
            enabled: !tabBar.active
            onClicked: tabBar.active = true
        }
    }

    ListItem.Divider {
        id: divider
        anchors.top: tabBar.bottom
    }

    ListView {
        id: tabView
        anchors {
            top: divider.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        model: slidingTabsDelegate.tabModel
        onModelChanged: tabView.updatePages();

        clip: true

        orientation: ListView.Horizontal
        snapMode: ListView.SnapOneItem
        //        snapMode: ListView.SnapToItem
        boundsBehavior: Flickable.DragOverBounds
        highlightFollowsCurrentItem: true

        function updatePages() {
            var tab;
            if (!slidingTabsDelegate.tabModel) return; // not initialized yet

            var tabList = slidingTabsDelegate.tabModel.children
            for (var i=0; i < tabList.length; i++) {
                tab = tabList[i];
                tab.width = tabView.width;
                tab.height = tabView.height;
                tab.anchors.fill = null;
                if (tab.hasOwnProperty("__active")) tab.__active = true;
            }
            tabView.updateSelectedTabIndex();
        }
        onMovingChanged: {
            if(!moving) {
                // update the currentItem
                tabView.currentIndex = contentX / slidingTabsDelegate.width;
                item.selectedTabIndex = tabView.currentIndex;
            }
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

        Component.onCompleted: {
            tabView.updatePages();
            tabView.positionViewAtIndex(1, ListView.Beginning);
        }

    }

    onWidthChanged: tabView.updatePages();
    onHeightChanged: tabView.updatePages();
    Component.onCompleted: tabView.updatePages();
}
