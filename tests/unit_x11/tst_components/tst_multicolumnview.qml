/*
 * Copyright 2015 Canonical Ltd.
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

import QtQuick 2.4
import QtTest 1.0
import Ubuntu.Test 1.0
import Ubuntu.Components 1.3

Item {
    id: test
    width: units.gu(120)
    height: units.gu(71)

    MultiColumnView {
        id: testView
        width: parent.width
        height: parent.height

        primaryPage: page1

        Page {
            id: page1
            title: "Page1"
        }
        Page {
            id: page2
            title: "Page2"
        }
        Page {
            id: page3
            title: "Page3"
        }
        Page {
            id: page4
            title: "Page4"
        }
    }

    MultiColumnView {
        id: defaults
    }

    UbuntuTestCase {
        when: windowShown

        function cleanup() {
//            testView.columns = Qt.binding(function() {
//                return test.width > units.gu(100) ? 3 : (test.width > units.gu(80) ? 2 : 1);
//            });
            testView.width = test.width;
            testView.height = test.height;
            // remove allpages
            testView.removePages(page1);
        }

        function test_0_API() {
            compare(defaults.primaryPage, undefined, "primaryPage not undefined by default");
        }

        function test_add_to_first_column_data() {
            return [
                {tag: "null sourcePage, fail", sourcePage: null, page: page2, failMsg: "No sourcePage specified. Page will not be added."},
                {tag: "valid sourcePage, pass", sourcePage: page1, page: page2, failMsg: ""},
            ]
        }
        function test_add_to_first_column(data) {
            if (data.failMsg != "") {
                ignoreWarning(data.failMsg);
            }

            testView.addPageToCurrentColumn(data.sourcePage, data.page);
            var firstColumn = findChild(testView, "ColumnHolder0");
            verify(firstColumn);
            if (data.failMsg != "") {
                expectFail(data.tag, "Fail");
            }
            compare(firstColumn.pageWrapper.object, data.page);
        }

        function test_add_to_next_column_data() {
            return [
                {tag: "null sourcePage, fail", sourcePage: null, page: page2, failMsg: "No sourcePage specified. Page will not be added."},
                {tag: "valid sourcePage, pass", sourcePage: page1, page: page2, failMsg: ""},
            ]
        }
        function test_add_to_next_column(data) {
            if (data.failMsg != "") {
                ignoreWarning(data.failMsg);
            }

            testView.addPageToNextColumn(data.sourcePage, data.page);
            var secondColumn = findChild(testView, "ColumnHolder1");
            verify(secondColumn);
            if (data.failMsg != "") {
                expectFail(data.tag, "Fail");
            }
            verify(secondColumn.pageWrapper);
        }

        function test_change_primaryPage() {
            ignoreWarning("Cannot change primaryPage after completion.");
            testView.primaryPage = page3;
        }

        function test_add_to_same_column_when_source_page_not_in_stack() {
            ignoreWarning("sourcePage must be added to the view to add new page.");
            testView.addPageToCurrentColumn(page2, page3);
        }

        function test_add_to_next_column_when_source_page_not_in_stack() {
            ignoreWarning("sourcePage must be added to the view to add new page.");
            testView.addPageToNextColumn(page2, page3);
        }
    }
}
