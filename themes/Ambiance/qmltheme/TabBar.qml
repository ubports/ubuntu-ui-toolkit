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

Rectangle {
    id: tabBar
    color: "transparent"

    height: units.gu(6)

    property Tabs tabs
    property VisualItemModel tabModel: tabs.__pagesModel
    onTabsChanged: tabModel = tabs.__pagesModel

    property bool active: false
    onActiveChanged: buttonView.position()

    Flickable {
        id: buttonView
        anchors.fill: parent
        contentWidth: 600
        //            contentHeight: 100
        //            model: slidingTabsDelegate.tabModel.children

        clip: false

        //            orientation: ListView.Horizontal
        //            snapMode: ListView.NoSnap

        Connections {
            target: item
            onSelectedTabIndexChanged: buttonView.position()
        }

        function position() {
            print("positioning");
            if (tabBar.active) {
                print("active");
                if (tabs.selectedTabIndex > 0) {
                    print("hmm");
                    // move the current button a bit to the right so that the user can see
                    // that it is possible to scroll left
                    buttonView.contentX -= units.gu(8);
                }
            } else {
                print("inactive, tab index = "+tabs.selectedTabIndex+ " x = "+repeater.itemAt(tabs.selectedTabIndex).x);
                // not active, move the button of the current tab to the left
                buttonView.contentX = repeater.itemAt(tabs.selectedTabIndex).x;
            }
        }

        Row {
            height: parent.height
            width: childrenRect.width

            spacing: units.gu(2)

            Repeater {
                id: repeater
                //                    AbstractButton {
                model: tabBar.tabModel.children
                AbstractButton {
                    id: tabButton
                    width: text.width + 2*text.anchors.margins
                    height: parent.height
                    //                        color: "pink"
                    //                        visible:  tabBar.active || selected
                    //                ItemStyle.class: "transparent-button"
                    property bool selected: (index === tabs.selectedTabIndex)

                    Rectangle {
                        border.width: 2
                        radius: 10
                        color: "yellow"
                        anchors.fill: parent
                        visible: false
                    }

                    TextCustom {
                        visible:  tabBar.active || selected
                        anchors.centerIn: parent
                        anchors.margins: units.gu(2)
                        id: text
                        text: modelData.title
                        fontSize: "x-large"
                    }

                    //                text: modelData.title
                    //                iconSource: modelData.iconSource
                    onClicked: {
                        tabs.selectedTabIndex = index;
                        tabBar.active = false;
                    }
                }
            }
        }
    }
    MouseArea {
        // an inactive tabBar can be clicked to make it active
        anchors.fill: parent
        enabled: !tabBar.active
        onClicked: {
            print("activating tab bar!");
            tabBar.active = true;
        }
    }
}
