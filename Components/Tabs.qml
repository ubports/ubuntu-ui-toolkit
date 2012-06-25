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

import QtQuick 1.1

/*!
    \qmlclass Tabs
    \inqmlmodule UbuntuUIToolkit
    \brief The Tabs class provides an environment where multible Tab
    children can be added, and the user is presented with a tab
    bar with tab buttons to select different tab pages.

    Examples:
    \qml
        Tabs {
            id: tabs
            anchors.fill: parent

            Tab {
                text: "tab 1"
                page: Text {
                    anchors.centerIn: parent
                    text: "This is the first tab."
                }
            }
            Tab {
                text: "tab 2"
                page: Text {
                    anchors.centerIn: parent
                    text: "Tab number two."
                }
            }
            Tab {
                text: "tab 3"
                page:  Rectangle {
                    id: tab3
                    anchors.fill: parent
                    Text {
                        anchors.centerIn: parent
                        text: "Colorful tab 3"
                    }
                    color: "lightblue"
                } // Rectangle
            } // Tab
        } // Tabs
    \endqml

    \b{This component is under heavy development.}
*/

Item {
    id: tabsMain

    // Keep the Tab items that the user defines separate
    // from the other items that we create below.
    default property alias children: tabItems.children
    Item {
        id: tabItems
    }

    // encapsulation
    Item {
        id: tabVisuals
        anchors.fill: parent

        // The currently selected tab.
        // -1 means that no tab is selected.
        property int selectedTabIndex: -1

        Row {
            id: buttonRow
            height: 40
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }

            Repeater {
                model: tabItems.children
                TabButton {
                    property Item tab: modelData
                    text: tab.text
                    iconSource: tab.iconSource
                    width: paintedWidth + 20
                    selected: (index == tabVisuals.selectedTabIndex)
                    onClicked: tabVisuals.selectTab(index)
                }
            } // Repeater
        } // buttonRow

        // This is the item that will be the parent of the currently displayed page.
        Item {
            id: pages
            anchors {
                top: buttonRow.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
        } // pages

        function selectTab(index) {
            if (tabVisuals.selectedTabIndex != -1) {
                tabItems.children[tabVisuals.selectedTabIndex].getPage().visible = false;
            }
            tabVisuals.selectedTabIndex = index;
            var page = tabItems.children[tabVisuals.selectedTabIndex].getPage();
            page.parent = pages;
            page.visible = true;
        }

        Component.onCompleted: tabVisuals.selectTab(0)
    } // tabVisuals
}
