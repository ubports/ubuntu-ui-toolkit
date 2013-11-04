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

TestCase {
    name: "TabsAPI"

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

    function test_z_addTab() {
        var newTab = tabs.addTab("Dynamic Tab", dynamicTab);
        compare((newTab !== null), true, "tab added");
        compare(newTab.active, false, "the inserted tab is inactive");
        compare(newTab.index, tabs.count - 1, "the tab is the last one");
    }

    function test_z_insertTab() {
        var newTab = tabs.insertTab(0, "Inserted tab", dynamicTab);
        compare((newTab !== null), true, "tab inserted");
        compare(newTab.active, false, "the inserted tab is inactive");
        compare(newTab.index, 0, "this is the first tab");
    }

    function test_z_insertTabAndActivate() {
        var newTab = tabs.insertTab(tabs.count, "Inserted tab", dynamicTab);
        compare((newTab !== null), true, "tab inserted");
        compare(newTab.active, false, "the inserted tab is inactive");
        compare(newTab.index, tabs.count - 1, "the tab is the last one");
        tabs.selectedTabIndex = newTab.index;
        compare(tabs.selectedTab, newTab, "the inserted tab is active");
    }

    function test_z_insertTabAtEnds() {
        var newTab = tabs.insertTab(-1, "PreTab", dynamicTab);
        compare(newTab !== null, true, "pre-tab inserted");
        compare(newTab.index, 0, "this is the new first tab");
        newTab = tabs.insertTab(tabs.count, "PostTab", dynamicTab);
        compare(newTab !== null, true, "post-tab inserted");
        compare(newTab.index, tabs.count - 1, "thsi is the new last tab");
    }

    function test_z_moveTab() {
        tabs.selectedTabIndex = tabs.count - 1;
        compare(tabs.moveTab(0, tabs.count - 1), true, "first tab moved to last");
        tabs.selectedTabIndex = 0;
        compare(tabs.moveTab(tabs.selectedTabIndex, tabs.selectedTabIndex + 1), true, "selected tab moved as next");
    }

    function test_z_moveSelectedTab() {
        tabs.selectedTabIndex = 0;
        tabs.moveTab(0, 1);
        compare(tabs.selectedTabIndex, 1, "selected tab moved");
    }

    function test_z_moveTabFail() {
        compare(tabs.moveTab(-1, tabs.count - 1), false, "from-parameter out of range");
        compare(tabs.moveTab(0, tabs.count), false, "to-parameter out of range");
    }

    function test_z_removeTab() {
        compare(tabs.removeTab(tabs.count - 1), true, "last tab removed");
        tabs.selectedTabIndex = 0;
        compare(tabs.removeTab(0), true, "active tab removed");
        compare(tabs.selectedTabIndex, 0, "the next tab is selected")
    }

    function test_z_removeTabAfterActiveTab() {
        var activeTab = tabs.count - 2;
        tabs.selectedTabIndex = activeTab;
        compare(tabs.removeTab(tabs.count - 1), true, "last tab removed");
        compare(tabs.selectedTabIndex, activeTab, "the selected tab wasn't moved");
    }

    function test_z_removeActiveTab() {
        tabs.selectedTabIndex = 1;
        compare(tabs.removeTab(1), true, "selected tab removed");
        compare(tabs.selectedTabIndex, 1, "selected tab is next");

        tabs.selectedTabIndex = tabs.count - 1;
        compare(tabs.removeTab(tabs.count - 1), true, "last tab removed");
        compare(tabs.selectedTabIndex, tabs.count - 1, "selected tab moved to last item");
    }

    function test_z_z() {
        compare(true, false, "STOP");
    }

    Tabs {
        id: emptyTabs
    }

    Component {
        id: dynamicTab
        Tab{}
    }

    MainView {
        id: mainView
        Tabs {
            id: tabs
            Tab {
                id: tab1
                page: Page {
                    id: page1
                }
            }
            Tab {
                id: tab2
                page: Page {
                    id: page2
                }
            }
            Tab {
                id: tab3
                page: Page {
                    id: page3
                }
            } 
            Tab {
                id: tabFlick1
                page: Page {
                    Flickable {
                        id: flickable1
                        anchors.fill: parent
                    }
                }
            }
            Tab {
                id: tabFlick2
                page: Page {
                    Flickable {
                        id: flickable2
                        anchors.fill: parent
                    }
                }
            }
            Tab {
                id: tabFlickLoader
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
}
