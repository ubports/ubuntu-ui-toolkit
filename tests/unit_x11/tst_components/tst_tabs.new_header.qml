/*
 * Copyright 2012-2014 Canonical Ltd.
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

import QtQuick 2.2
import Ubuntu.Test 1.0
import Ubuntu.Components 1.1

MainView {
    id: mainView
    width: units.gu(50)
    height: units.gu(80)
    useDeprecatedToolbar: false

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
        Tab {
            id: tabNoFlickLoader
            title: "loadNoFlick"
            page: Loader {
                id: loaderNoFlick
                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                }
                // height compes from the loaded Page
                sourceComponent: tabs.selectedTabIndex === 6 ? pageComponentNoFlick : null
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
    Component {
        id: pageComponentNoFlick
        Page {
            title: "Loaded page without flickable"
        }
    }

    UbuntuTestCase {
        name: "TabsAPINewHeader"
        when: windowShown

        function test_tabsDefaults() {
            compare(tabs.selectedTabIndex, 0, "The default selectedTabIndex is 0 when Tabs has contents");
            compare(tabs.selectedTab, tab1, "The default selectedTab is the first tab");
            compare(tabs.currentPage, page1, "The default currentPage is the page of the first tab");
        }

        function test_tabsSetSelectedTab() {
            var tabArray = [tab1, tab2, tab3];
            var pageArray = [page1, page2, page3];
            for (var i=0; i < 3; i++) {
                tabs.selectedTabIndex = i;
                compare(tabs.selectedTabIndex, i, "Can set selectedTabIndex");
                compare(tabs.selectedTab, tabArray[i], "Can update selectedTab by setting selectedTabIndex");
                compare(tabs.currentPage, pageArray[i], "Can update currentPage by setting selectedTabIndex");
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

        function test_pageHeightLoaderNoFlick_bug1259917() {
            tabs.selectedTabIndex = 6;
            compare(tabs.selectedTab, tabNoFlickLoader, "Tab 6 was selected.");
            compare(mainView.__propagated.header.flickable, null, "Loaded page without flickable.");
            compare(loaderNoFlick.item.height, mainView.height - mainView.__propagated.header.height,
                    "Correct height for loaded Page without flickable.");
        }

        function test_index() {
            compare(tab1.index, 0, "tab1 is at 0");
            compare(tab2.index, 1, "tab2 is at 1");
            compare(tab3.index, 2, "tab3 is at 2");
            compare(tabFlick1.index, 3, "tabFlick1 is at 3");
            compare(tabFlick2.index, 4, "tabFlick2 is at 4");
            compare(tabFlickLoader.index, 5, "tabFlickLoader is at 5");
            compare(tabNoFlickLoader.index, 6, "tabNoFlickLoader is at 6");
        }
    }
}
