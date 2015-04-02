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
import Ubuntu.Test 1.0
import Ubuntu.Components 1.3

Item {
    width: 400
    height: 600

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

    UbuntuTestCase {
        id: testCase
        name: "Page13API"
        when: windowShown

        function init() {
            compare(page.title, "", "Page title is set by default.");
            compare(page.active, true, "Page is inactive by default.");
            compare(page.pageStack, null, "Page has a PageStack by default.");
        }

        function test_title() {
            var newTitle = "Hello World!";
            page.title = newTitle;
            compare(page.title, newTitle, "Could not set page title.");
            page.title = "";
            compare(page.title, "", "Could not unset page title.");
        }

        function test_header_title() {
            var newTitle = "Hello header!";
            page.title = newTitle;
            var header = mainView.__propagated.header;
            compare(header.title, newTitle, "Header title does not match active page title.");
            page.title = "";
        }

        function test_flickable_bug1200642_bug1192591() {
            var header = page.__propagated.header;
            compare(page.flickable, pageFlickable,
                    "Flickable is not correctly detected.");
            compare(header.flickable, pageFlickable,
                    "Header flickable is not correctly set.");

            page.flickable = testFlickable;
            compare(page.flickable, testFlickable, "Flickable could not be set.");
            compare(header.flickable, testFlickable,
                    "Header flickable was not update correctly.");

            page.flickable = null;
            compare(page.flickable, null, "Flickable cannot be unset.");
            compare(header.flickable, null,
                    "Header flickable was not correctly unset.");
        }

        function test_flickableY_bug1201452() {
            var header = findChild(mainView, "MainView_Header");
            var headerHeight = header.height;
            var flickableY = 150;
            page.flickable.contentY = flickableY;
            compare(page.flickable.contentY, flickableY,
                    "Flickable.contentY could not be set.");
            compare(page.flickable.topMargin, headerHeight,
                    "topMargin of the flickable does not equal header height.");

            page.head.locked = true;
            page.head.visible = false;
            waitForHeaderAnimation(mainView);

            compare(page.flickable.topMargin, 0,
                    "topMargin is not 0 when header is locked hidden.");
            compare(page.flickable.contentY, flickableY + headerHeight,
                    "contentY was not updated properly when header was hidden.");

            page.head.locked.locked = false;
            page.head.visible = true;
            waitForHeaderAnimation(mainView);

            compare(page.flickable.contentY, flickableY,
                    "Hiding and showing header changes flickable.contentY.");
            compare(page.flickable.topMargin, headerHeight,
                    "topMargin was not updated when header became visible.");
        }
    }
}
