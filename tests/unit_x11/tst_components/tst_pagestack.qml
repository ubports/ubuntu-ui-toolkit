/*
 * Copyright 2012-2015 Canonical Ltd.
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
import Ubuntu.Components 1.3
import Ubuntu.Test 1.0

// pagestack tests for deprecated toolbar are in
// unit/tst_components/tst_pagestack_deprecated_toolbar.qml

Item {
    width: units.gu(50)
    height: units.gu(80)

    MainView {
        id: mainView
        anchors.fill: parent
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
    }
    Page {
        id: page2
        title: "Title 2"
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
            page: Page {
                title: "Tab 1"
            }
        }
        Tab {
            id: tab2
            page: Page {
                title: "Tab 2"
            }
        }
    }
    Page {
        id: pageWithHeader
        title: "Page with PageHeader"
        header: PageHeader {
            title: pageWithHeader.title
        }
    }

    Component {
        id: pageComponent
        Page {
            title: "Page from component"
        }
    }

    UbuntuTestCase {
        name: "PageStackAPI"
        when: windowShown
        id: testCase

        function initTestCase() {
            waitForHeaderAnimation(mainView);
            compare(pageStack.currentPage, null, "is not set by default");
            compare(pageStack.__propagated, mainView.__propagated, "propagated property of pageStack equals mainView.__propagated")
            compare(mainView.__propagated.header.title, "", "empty title by default");
        }

        function cleanup() {
            pageStack.clear();
            waitForHeaderAnimation(mainView);
            compare(pageStack.depth, 0, "depth is not 0 after clearing.");
            compare(pageStack.currentPage, null, "currentPage is not null after clearing.");
        }

        function test_depth() {
            compare(pageStack.depth, 0, "depth is 0 by default");
            pageStack.push(page1);
            waitForHeaderAnimation(mainView);
            compare(pageStack.depth, 1, "depth is correctly increased when pushing a page");
            pageStack.push(page2);
            waitForHeaderAnimation(mainView);
            compare(pageStack.depth, 2, "depth is correctly updated when pushing a page");
            pageStack.pop();
            waitForHeaderAnimation(mainView);
            compare(pageStack.depth, 1, "depth is correctly decreased when popping a page");
        }

        function test_currentPage() {
            compare(pageStack.currentPage, null, "currentPage is null by default");
            pageStack.push(page1);
            waitForHeaderAnimation(mainView);
            compare(pageStack.currentPage, page1, "currentPage properly updated");
        }

        function test_page_order() {
            compare(pageStack.depth, 0, "depth is 0 initially");
            pageStack.push(page1);
            pageStack.push(page2);
            waitForHeaderAnimation(mainView);
            compare(pageStack.currentPage, page2, "last pushed page is on top");
            pageStack.pop();
            waitForHeaderAnimation(mainView);
            compare(pageStack.currentPage, page1, "popping puts previously pushed page on top");
        }

        function test_multipop_bug1461729() {
            for (var i=0; i < 10; i++) {
                pageStack.push(pageComponent);
            }
            waitForHeaderAnimation(mainView);
            compare(pageStack.depth, 10, "couldn't push 10 new pages");
            // When updating depth after animating out the header, depth
            // is not reliable to be used to guard a loop:
            while(pageStack.depth > 1) {
                pageStack.pop();
            }
            waitForHeaderAnimation(mainView);
            compare(pageStack.depth, 1, "popping until one page is left failed. " +
                        pageStack.depth + " pages left on stack");
        }

        function test_active_bug1260116() {
            pageStack.push(page1);
            waitForHeaderAnimation(mainView);

            compare(page1.active, true, "Page is active after pushing");
            pageStack.push(page2);
            waitForHeaderAnimation(mainView);

            compare(page1.active, false, "Page no longer active after pushing a new page");
            compare(page2.active, true, "New page is active after pushing");
            pageStack.pop();
            waitForHeaderAnimation(mainView);
            compare(page1.active, true, "Page re-activated when on top of the stack");
            compare(page2.active, false, "Page no longer active after being popped");
            pageStack.clear();
            waitForHeaderAnimation(mainView);

            compare(pageInStack.active, false, "Page defined inside PageStack is not active by default");
            pageStack.push(pageInStack);
            waitForHeaderAnimation(mainView);
            compare(pageInStack.active, true, "Pushing a page on PageStack makes it active");
            pageStack.pop();
            waitForHeaderAnimation(mainView);
            compare(pageInStack.active, false, "Popping a page from PageStack makes it inactive");
        }

        function test_title_bug1143345_bug1317902() {
            pageStack.push(page1);
            waitForHeaderAnimation(mainView);
            compare(mainView.__propagated.header.title, "Title 1", "Header title is correctly set by page");
            page1.title = "New title";
            compare(mainView.__propagated.header.title, "New title", "Header title correctly updated by page");
            pageStack.push(page2);
            waitForHeaderAnimation(mainView);
            compare(mainView.__propagated.header.title, "Title 2", "Header title is correctly set by page");
            pageStack.clear();
            page1.title = "Title 1";

            pageStack.push(pageWithPage);
            waitForHeaderAnimation(mainView);
            compare(mainView.__propagated.header.title, pageWithPage.title, "Embedded page sets title of outer page");
        }

        function get_tabs_button() {
            var button = findChild(mainView, "tabsButton");
            if (!button.visible) return null;
            return button;
        }

        function test_tabs_inside_stack_bug1187850() {
            compare(get_tabs_button(), null, "Without tabs there is no visible tabs button.");
            pageStack.push(tabs);
            waitForHeaderAnimation(mainView);
            compare(pageStack.currentPage, tabs, "Tabs can be pushed on a PageStack");
            compare(tabs.active, true, "Tabs on top of a PageStack are active");
            compare(get_tabs_button().visible, true, "Pushing tabs on pagestack enables the tabs button");
            pageStack.push(page1);
            waitForHeaderAnimation(mainView);
            compare(pageStack.currentPage, page1, "A page can be pushed on top of a Tabs");
            compare(tabs.active, false, "Tabs on a PageStack, but not on top, are inactive");
            compare(get_tabs_button(), null, "Contents of inactive Tabs is not applied to header");
            pageStack.pop();
            waitForHeaderAnimation(mainView);
            compare(tabs.active, true, "Tabs on top of PageStack is active");
            compare(get_tabs_button().visible, true, "Active Tabs controls header contents");
        }

        function test_pop_to_tabs_bug1316736() {
            pageStack.push(tabs);
            waitForHeaderAnimation(mainView);
            tabs.selectedTabIndex = 1;
            pageStack.push(page1);
            waitForHeaderAnimation(mainView);
            compare(tabs.active, false, "Tabs on a PageStack, but not on top, are inactive");
            pageStack.pop();
            waitForHeaderAnimation(mainView);
            compare(tabs.active, true, "Tabs on top of PageStack is active");
            compare(tabs.selectedTabIndex, 1, "Pushing and popping another page on top of Tabs does not change selectedTabsIndex");
        }

        function test_push_return_values() {
            var pushedPage = pageStack.push(page1);
            compare(pushedPage, page1,
                    "PageStack.push() returns pushed Page");
            pushedPage = pageStack.push(pageComponent);
            compare(pushedPage.title, "Page from component",
                    "PageStack.push() returns Page created from Component");
            pushedPage = pageStack.push(Qt.resolvedUrl("MyExternalPage.qml"));
            compare(pushedPage.title, "Page from QML file",
                    "PageStack.push() returns Page created from QML file");
        }

        function test_page_header_back_button() {
            pageStack.push(pageWithHeader);
            var backButton = findChild(pageWithHeader.header, "pagestack_back_action_button");
            // FIXME TIM: when visibleActions is fixed, only check for backButton, null.
            compare(backButton == null || !backButton.visible, true,
                    "Page header shows back button with only one page on the stack.");

            pageStack.pop();
            pageStack.push(page1);
            pageStack.push(pageWithHeader);
            waitForHeaderAnimation(mainView);
            backButton = findChild(pageWithHeader.header, "pagestack_back_action_button");
            compare(backButton.visible, true,
                    "Page header has no back button with two pages on the stack.");
        }
    }
}
