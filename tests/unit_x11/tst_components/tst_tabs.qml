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
import QtTest 1.0
import Ubuntu.Components 0.1

Item {
    width: units.gu(50)
    height: units.gu(80)

    Tabs {
        id: emptyTabs
    }

    MainView {
        id: mainView
        anchors.fill: parent
        Tabs {
            id: tabs
            Tab {
                id: tab1
                title: "tab 1"
                page: Page {
                    id: page1
                    Button {
                        id: button
                        anchors.centerIn: parent
                        text: "click"
                    }
                }
            }
            Tab {
                id: tab2
                title: "tab 2"
                page: Page {
                    id: page2
                }
            }
            Tab {
                id: tab3
                title: "tab 3"
                page: Page {
                    id: page3
                }
            }
            Tab {
                id: tabFlick1
                title: "flick"
                page: Page {
                    Flickable {
                        id: flickable1
                        anchors.fill: parent
                    }
                }
            }
            Tab {
                id: tabFlick2
                title: "flick 2"
                page: Page {
                    Flickable {
                        id: flickable2
                        anchors.fill: parent
                    }
                }
            }
            Tab {
                id: tabFlickLoader
                title: "load"
                page: Loader {
                    id: loader
                    sourceComponent: tabs.selectedTabIndex != 5 ? null : pageComponent
                }
            }
        }
        Component {
            id: pageComponent
            Page {
                title: "Loaded page"
                property Flickable flick: loadedFlickable
                Flickable {
                    id: loadedFlickable
                    anchors.fill: parent
                    contentHeight: 1000
                }
            }
        }
    }

    Tabs {
        id: tabsWithRepeater
        ListModel {
            id: inputModel
            Component.onCompleted: {
                append({ "name": "tab 1" });
                insert(0, { "name": "tab 0" });
                append({ "name": "tab 3" });
                insert(2, { "name": "tab 2" });
            }
        }
        Repeater {
            id: tabsRepeater
            model: inputModel
            Tab {
                title: name
            }
        }
    }

    TestCase {
        name: "TabsAPI"
        when: windowShown

        function test_tabOrder_bug1253804() {
            var tabsModel = tabsWithRepeater.tabBar.model;

            compare(tabsRepeater.count, inputModel.count, "Incorrect number of tabs in Tabs");
            compare(tabsModel.count, tabsRepeater.count, "Incorrect number of tabs in TabBar");
            for (var i=0; i < tabsModel.count; i++) {
                compare(tabsModel.get(i).title, inputModel.get(i).name, "Tab titles don't match for index "+i);
            }
        }

        function test_emptyTabs() {
            compare(emptyTabs.selectedTabIndex, -1, "The default value for selectedTabIndex is -1 when there are no tabs");
            compare(emptyTabs.selectedTab, null, "The default selected tab is null when there are no tabs");
            compare(emptyTabs.currentPage, null, "The default currentPage is null when there are no tabs");
        }

        function test_tabsDefaults() {
            compare(tabs.selectedTabIndex, 0, "The default selectedTabIndex is 0 when Tabs has contents");
            compare(tabs.selectedTab, tab1, "The default selectedTab is the first tab");
            compare(tabs.currentPage, page1, "The default currentPage is the page of the first tab");
            compare(mainView.__propagated.toolbar.tools, page1.tools, "The default tools are the tools of the first tab");
            compare(mainView.__propagated.header.contents, tabs.tabBar, "Tabs updates the Header contents");
        }

        function test_tabsSetSelectedTab() {
            var tabArray = [tab1, tab2, tab3];
            var pageArray = [page1, page2, page3];
            for (var i=0; i < 3; i++) {
                tabs.selectedTabIndex = i;
                compare(tabs.selectedTabIndex, i, "Can set selectedTabIndex");
                compare(tabs.selectedTab, tabArray[i], "Can update selectedTab by setting selectedTabIndex");
                compare(tabs.currentPage, pageArray[i], "Can update currentPage by setting selectedTabIndex");
                compare(mainView.__propagated.toolbar.tools, pageArray[i].tools, "Activating a Tab updates the tools of the Toolbar");
                for (var j=0; j < 3; j++) {
                    compare(pageArray[j].active, j===i, "Only the page of the selected tab is active");
                }
            }
        }

        function test_flickable() {
            // ensure that the flickable of the header is set to the flickable of the selected tab.
            tabs.selectedTabIndex = 3;
            compare(mainView.__propagated.header.flickable, flickable1, "Header flickable correctly initialized");
            tabs.selectedTabIndex = 4;
            compare(mainView.__propagated.header.flickable, flickable2, "Header flickable correctly updated");
            tabs.selectedTabIndex = 0;
        }

        function test_pageLoader() {
            tabs.selectedTabIndex = 0;
            compare(loader.item, null, "Page not loaded when tab is not selected");
            tabs.selectedTabIndex = 5;
            compare(tabs.currentPage, loader, "Selected loader for current page");
            compare(loader.item.title, "Loaded page", "Loaded item is a page");
            tabs.selectedTabIndex = 0;
            compare(loader.item, null, "Loaded page was properly unloaded");
        }

        function test_bug1088740() {
            tabs.selectedTabIndex = 5;
            compare(mainView.__propagated.header.flickable, loader.item.flick, "Header flickable correctly updated with Loader");
            compare(loader.item.flick.contentHeight, 1000, "Header flickable is correct flickable");
            tabs.selectedTabIndex = 0;
        }

        function test_index() {
            compare(tab1.index, 0, "tab1 is at 0");
            compare(tab2.index, 1, "tab2 is at 1");
            compare(tab3.index, 2, "tab3 is at 2");
            compare(tabFlick1.index, 3, "tabFlick1 is at 3");
            compare(tabFlick2.index, 4, "tabFlick2 is at 4");
            compare(tabFlickLoader.index, 5, "tabFlickLoader is at 5");
        }

        function test_deactivateByTimeout() {
            tabs.tabBar.selectionMode = true;
            compare(tabs.tabBar.selectionMode, true, "Tab bar can be put into selection mode");
            compare(tabs.tabBar.__styleInstance.deactivateTime > 0, true, "There is a positive deactivate time");
            wait(tabs.tabBar.__styleInstance.deactivateTime + 500); // add 500 ms margin
            compare(tabs.tabBar.selectionMode, false, "Tab bar automatically leaves selection mode after a timeout.");
        }

        function test_deactivateByAppInteraction() {
            tabs.tabBar.selectionMode = true;
            compare(tabs.tabBar.selectionMode, true, "Tab bar can be put into selection mode");
            mouseClick(button, units.gu(1), units.gu(1), Qt.LeftButton);
            compare(tabs.tabBar.selectionMode, false, "Tab bar deactivated by interacting with the page contents");
        }
    }
}
