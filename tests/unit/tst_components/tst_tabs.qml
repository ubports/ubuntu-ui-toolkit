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
        compare(mainView.toolbar.tools, page1.tools, "The default tools are the tools of the first tab");
        compare(mainView.header.contents, tabs.headerContents, "Tabs updates the Header contents");
    }

    function test_tabsSetSelectedTab() {
        var tabArray = [tab1, tab2, tab3];
        var pageArray = [page1, page2, page3];
        for (var i=0; i < 3; i++) {
            tabs.selectedTabIndex = i;
            compare(tabs.selectedTabIndex, i, "Can set selectedTabIndex");
            compare(tabs.selectedTab, tabArray[i], "Can update selectedTab by setting selectedTabIndex");
            compare(tabs.currentPage, pageArray[i], "Can update currentPage by setting selectedTabIndex");
            compare(mainView.toolbar.tools, pageArray[i].tools, "Activating a Tab updates the tools of the Toolbar");
            for (var j=0; j < 3; j++) {
                compare(pageArray[j].active, j===i, "Only the page of the selected tab is active");
            }
        }
    }

    Tabs {
        id: emptyTabs
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
        }
    }
}
