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
import Ubuntu.Components 1.1

Item {
    width: 200
    height: 200

    Action {
        id: action0
    }

    Flickable {
        id: testFlickable
    }

    MainView {
        anchors.fill: parent
        id: mainView
        Page {
            id: page
            Flickable {
                id: pageFlickable
                anchors.fill: parent
                contentHeight: column.height

                Column {
                    id: column
                    Repeater {
                        model: 100
                        Label {
                            text: "line "+index
                        }
                    }
                }
            }
        }
    }

    TestCase {
        name: "Page11API"
        when: windowShown

        function initTestCase() {
            compare(page.title, "", "is not set by default")
            compare(page.__propagated, mainView.__propagated, "page propagated equals mainView propagated")
            compare(page.__propagated.header, mainView.__propagated.header, "page header equals mainView header")
            compare(page.__propagated.header.title, page.title, "header title is same as page title")
            compare(page.__propagated.header.visible, false, "header is not visible initially because there is no title")
            compare(page.actions.length, 0, "page actions list empty by default")
        }

        function test_0_noHeader_bug1162028_bug1161910() {
            compare(mainView.__propagated.header.title, "", "no header title by default")
            compare(mainView.__propagated.header.visible, false, "header is hidden when title is not set")
            compare(page.height, mainView.height, "page uses full height when there is no header")
        }

        function test_title() {
            var newTitle = "Hello World!"
            page.title = newTitle
            compare(page.title, newTitle, "can set/get")
            page.title = ""
            compare(page.title, "", "can unset")
        }

        function test_header() {
            var newTitle = "Hello header!"
            page.title = newTitle
            compare(mainView.__propagated.header.title, newTitle, "header title updated by changing page title")
            compare(mainView.__propagated.header.visible, true, "header is visible when title is set")
            compare(mainView.__propagated.header.height > 0, true, "header has a height when title is set")
            page.title = ""
            compare(mainView.__propagated.header.title, "", "header title unset by unsetting page title")
            compare(mainView.__propagated.header.visible, false, "header is hidden when title is unset")
        }

        function test_tools() {
            compare(mainView.__propagated.toolbar.tools, page.tools, "Page updates toolbar tools");
        }

        function test_active() {
            compare(page.active, true, "Page is active by default");
        }

        function test_pageStack() {
            compare(page.pageStack, null, "is not set by default")
        }

        function test_actions() {
            // FIXME: Check the contents of page.actions. This is currently not
            //  possible because ActionContext.actions does not support it,
            //  so changes to ActionContext are needed.
            page.actions = [action0];
            compare(page.actions.length, 1, "Actions can be added to page actions");
            page.actions = [];
            compare(page.actions.length, 0, "Page action list can be cleared");
        }
        function test_flickable_bug1200642_bug1192591() {
            compare(page.flickable, pageFlickable, "page flickable is correctly detected");
            compare(page.__propagated.header.flickable, pageFlickable, "header flickable is correctly detected"); // bug 1200642 FAIL
            page.flickable = testFlickable;
            compare(page.flickable, testFlickable, "flickable can be set");
            compare(page.__propagated.header.flickable, testFlickable, "updating page flickable updates header flickable");
            page.flickable = null;
            compare(page.flickable, null, "flickable can be unset");
            compare(page.__propagated.header.flickable, null, "unsetting page flickable unsets header flickable");
        }

        function test_flickableY_bug1201452() {
            var pageTitle = "Hello bug!";
            page.title = pageTitle;
            var header = page.__propagated.header;
            compare(header.visible, true, "header is visible when title is set")
            compare(header.height > 0, true, "header has a height when title is set")
            var headerHeight = header.height
            var flickableY = 150;
            page.flickable.contentY = flickableY;
            compare(page.flickable.contentY, flickableY, "flickable.contentY can be set");
            compare(page.flickable.topMargin, headerHeight, "topMargin of the flickable equals header height");
            page.title = "";
            compare(header.visible, false, "header is hidden when title is unset")
            compare(page.flickable.topMargin, 0, "topMargin becomes 0 because header is hidden"); // used to be headerHeight
            compare(page.flickable.contentY, flickableY + headerHeight, "contentY is updated when header is made invisible to compensate for the change in topMargin");
            page.title = pageTitle;
            compare(page.flickable.contentY, flickableY, "Making header visible again does not reset flickable.contentY");
            compare(page.flickable.topMargin, headerHeight, "topMargin is updated when header becomes visible.")
        }
    }
}
