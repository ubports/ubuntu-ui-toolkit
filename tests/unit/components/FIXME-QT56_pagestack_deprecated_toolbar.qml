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

import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 1.1

// pagestack tests for the new header are in
// unit/visual/tst_pagestack_new_header.qml

TestCase {
    name: "PageStackAPI"

    function initTestCase() {
        compare(pageStack.currentPage, null, "is not set by default");
        compare(pageStack.__propagated, mainView.__propagated, "propagated property of pageStack equals mainView.__propagated")
        compare(mainView.__propagated.toolbar.tools, null, "no tools by default");
        compare(mainView.__propagated.header.title, "", "empty title by default");
    }

    function test_depth() {
        compare(pageStack.depth, 0, "depth is 0 by default");
        pageStack.push(page1);
        compare(pageStack.depth, 1, "depth is correctly increased when pushing a page");
        pageStack.push(page2);
        compare(pageStack.depth, 2, "depth is correctly updated when pushing a page");
        pageStack.pop();
        compare(pageStack.depth, 1, "depth is correctly decreased when popping a page");
        pageStack.clear();
        compare(pageStack.depth, 0, "depth is after clearing");
    }

    function test_currentPage() {
        compare(pageStack.currentPage, null, "currentPage is null by default");
        pageStack.push(page1);
        compare(pageStack.currentPage, page1, "currentPage properly updated");
        pageStack.clear();
        compare(pageStack.currentPage, null, "currentPage properly reset");
    }

    function test_active_bug1260116() {
        pageStack.push(page1);
        compare(page1.active, true, "Page is active after pushing");
        pageStack.push(page2);
        compare(page1.active, false, "Page no longer active after pushing a new page");
        compare(page2.active, true, "New page is active after pushing");
        pageStack.pop();
        compare(page1.active, true, "Page re-activated when on top of the stack");
        compare(page2.active, false, "Page no longer active after being popped");
        pageStack.clear();

        compare(pageInStack.active, false, "Page defined inside PageStack is not active by default");
        pageStack.push(pageInStack);
        compare(pageInStack.active, true, "Pushing a page on PageStack makes it active");
        pageStack.pop();
        compare(pageInStack.active, false, "Popping a page from PageStack makes it inactive");
        pageStack.clear();
    }

    function test_title_bug1143345_bug1317902() {
        pageStack.push(page1);
        compare(mainView.__propagated.header.title, "Title 1", "Header title is correctly set by page");
        page1.title = "New title";
        compare(mainView.__propagated.header.title, "New title", "Header title correctly updated by page");
        pageStack.push(page2);
        compare(mainView.__propagated.header.title, "Title 2", "Header title is correctly set by page");
        pageStack.clear();
        page1.title = "Title 1";

        pageStack.push(pageWithPage);
        compare(mainView.__propagated.header.title, pageWithPage.title, "Embedded page sets title of outer page");
        pageStack.clear();
    }

    function test_tools_bug1126197() {
        pageStack.push(page1);
        compare(mainView.__propagated.toolbar.tools, page1.tools, "Page successfully updated toolbar tools");
        pageStack.push(page2);
        compare(mainView.__propagated.toolbar.tools, page2.tools, "Page successfully updated toolbar tools again");
        pageStack.pop();
        compare(mainView.__propagated.toolbar.tools, page1.tools, "Tools updated after popping a page");
        pageStack.clear();
    }

    function test_tabs_inside_stack_bug1187850() {
        pageStack.push(tabs);
        compare(pageStack.currentPage, tabs, "Tabs can be pushed on a PageStack");
        compare(tabs.active, true, "Tabs on top of a PageStack are active");
        compare(mainView.__propagated.header.__styleInstance.__tabBar, tabs.tabBar, "Pushing Tabs on PageStack updates the header contents");
        pageStack.push(page1);
        compare(pageStack.currentPage, page1, "A page can be pushed on top of a Tabs");
        compare(tabs.active, false, "Tabs on a PageStack, but not on top, are inactive");
        compare(mainView.__propagated.header.__styleInstance.__tabBar, null, "Contents of inactive Tabs is not applied to header");
        pageStack.pop();
        compare(tabs.active, true, "Tabs on top of PageStack is active");
        compare(mainView.__propagated.header.__styleInstance.__tabBar, tabs.tabBar, "Active Tabs controls header contents");
        pageStack.clear();
    }

    function test_pop_to_tabs_bug1316736() {
        pageStack.push(tabs);
        tabs.selectedTabIndex = 1;
        pageStack.push(page1);
        compare(tabs.active, false, "Tabs on a PageStack, but not on top, are inactive");
        pageStack.pop();
        compare(tabs.active, true, "Tabs on top of PageStack is active");
        compare(tabs.selectedTabIndex, 1, "Pushing and popping another page on top of Tabs does not change selectedTabsIndex");
        pageStack.clear();
    }

    MainView {
        id: mainView
        useDeprecatedToolbar: true
        PageStack {
            id: pageStack
            Page {
                id: pageInStack
            }
        }
    }
    Page {
        id: page1
        title: "Title 1"
        tools: ToolbarItems {
            id: tools1
        }
    }
    Page {
        id: page2
        title: "Title 2"
        tools: ToolbarItems {
            id: tools2
        }
    }
    Page {
        id: pageWithPage
        title: "Outer"
        Page {
            title: "Inner"
        }
    }
    Tabs {
        id: tabs
        Tab {
            id: tab1
        }
        Tab {
            id: tab2
        }
    }
}
