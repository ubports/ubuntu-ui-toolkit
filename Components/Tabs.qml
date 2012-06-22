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
import "tabs.js" as Tabs

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

   // property Row buttonRow: tabVisuals.buttonRow
    //property TabGroup tabGroup: tabVisuals.tabGroup

    // Keep the Tab items that the user defines separate
    // from the other items that we create below.
    default property alias children: tabItems.children
    Item {
        id: tabItems
    }

    Item {
        // encapsulation.
        id: tabVisuals
        anchors.fill: parent

        // The currently selected tab.
        // -1 means that no tab is selected.
        property int selectedTabIndex: -1

     //   property Row buttonRow: buttonRow
     //   property TabGroup tabGroup: tabGroup

        // Ideally, I would like to keep a list of children/resources that are of
        // type Tab, however that is not possible, see bug reports:
        // https://bugreports.qt-project.org/browse/QTBUG-14986
        // https://bugreports.qt-project.org/browse/QTBUG-14645
        //property list<Tab> model: __tabsOnly(tabs.resources)

        //property TabFilterModel filter: TabFilterModel {
        //    proxyModel: tabs.resources
        //}

        //property ListModel tabModel: filter.model

        Row {
            id: buttonRow
            height: 40
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }

            Repeater {
                //model: Tabs.num()
                //model: tabsMain.resources
                model: tabItems.children
                TabButton {
                    //property Item tab: Tabs.get(index)
                    property Item tab: modelData
                    //text: modelData.text
                    text: tab.text
                    iconSource: tab.iconSource
                    //page: tab.page
                    width: 100
                    __selected: (index == tabVisuals.selectedTabIndex)
                    onClicked: tabVisuals.selectedTabIndex = index
                    //onClicked: Tabs.select(index) // working
                    //__selected: (index == Tabs.selectedIndex) //Tabs.isSelected(index) // not working
                    // seems like index and expressions are only evaluated once here
                }
            }
        }

        Rectangle {
            id: tabPageItem
            color: "yellow"
            anchors {
                top: buttonRow.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            //Component.onCompleted: Tabs.pageItem = tabPageItem
        }

        onSelectedTabIndexChanged: {
            tabItems.children[selectedTabIndex].page.parent = tabPageItem;
            print("Selecting tab "+tabVisuals.selectedTabIndex);
        }
//        TabGroup {
//            id: tabGroup
//            anchors {
//                top: buttonRow.bottom
//                left: parent.left
//                right: parent.right
//                bottom: parent.bottom
//            }

//            Repeater {
//                model: tabs.num()
//            }
//        }

/*
        function selectFirstTab() {
            var numTabs = tabGroup.children.length;
            if (numTabs > 0) tabGroup.currentTab = tabGroup.children[numTabs-1];
        }

        Component.onCompleted: {
            selectFirstTab();
  //          print("#tabs in model = "+filterModel.model.count);
        }
*/
/*
        property TabFilterModel filterModel: TabFilterModel {
            proxyModel: tabs.resources

        }
*/

    }

    onChildrenChanged: {
    //Component.onCompleted: {
        //Tabs.setPage(tabPageItem);
        // TODO: only add new children, not to clear every time.
        Tabs.clear();
        for (var i=0; i < children.length; i++) {
            var child = children[i];
            Tabs.add(child);
        } // for
        Tabs.printAll();
    } // onChildrenChanged
}
