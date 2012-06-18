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
    id: tabs

    onChildrenChanged: print("children changed")
    //onResourcesChanged: print("bla")

    Component.onCompleted: print("component completed with "+tabs.children.length+" children and "+tabs.resources.length+" resources.")


    property list<Tab> model

    Item {
        // encapsulation.
        id: tabVisuals
        anchors.fill: parent

        // Ideally, I would like to keep a list of children/resources that are of
        // type Tab, however that is not possible, see bug reports:
        // https://bugreports.qt-project.org/browse/QTBUG-14986
        // https://bugreports.qt-project.org/browse/QTBUG-14645
        //property list<Tab> model

        function __tabsOnly(someList) {
            return null;
        }

        function __conditionalTab(tab) {
            if (("text" in tab) && ("page" in tab)) {
                return 2;

            } else {
                return null;
            }

        }

        Row {
            id: buttonRow
            height: 40
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }

            Repeater {
                model: tabVisuals.theTabs

               TabButton {
                   //text: "i "+tabs.resources[index].text
                   text: modelData.text
                   //visible: "page" in modelData
               }
            }
        }

        TabGroup {
            id: tabGroup
            anchors {
                top: buttonRow.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
/*
            Repeater {
                model: tabs.resources
                Text {
                    anchors.fill: tabGroup
                    text: "page " + modelData.text
                }
             }
            */
        }

        function __selectFirstTab() {
            var numTabs = tabGroup.children.length;
            if (numTabs > 0) tabGroup.currentTab = tabGroup.children[numTabs-1];
        }

        Component.onCompleted: {
            __selectFirstTab();
            print("completed Tabs with " + tabGroup.children.length + " tabs.");
        }
    }
}
