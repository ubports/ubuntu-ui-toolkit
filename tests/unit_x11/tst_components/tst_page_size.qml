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

import QtQuick 2.3
import QtTest 1.0
import Ubuntu.Components 1.2

Item {
    width: 300
    height: 400

    MainView {
        anchors.fill: parent
        id: mainView

        Tabs {
            id: tabs
            Tab {
                page: Page {
                    id: page
                    title: "Simple page"
                }
            }
            Page {
                id: pageWithFlickable
                title: "Page with flickable"
                Flickable {
                    anchors.fill: parent
                    contentHeight: 2*height
                }
            }
            Tab {
                Component {
                    id: pageComponent
                    Page {
                        title: "Loaded page"
                    }
                }
                page: Loader {
                    id: pageLoader
                }
            }
        }
    }

    TestCase {
        name: "PageSize"
        when: windowShown
        id: testCase

        property real headerHeight;
        function initTestCase() {
            testCase.headerHeight = mainView.__propagated.header.height;
            verify(testCase.headerHeight > 0, "Header has no height.");
        }
        function cleanupTestCase() {
            tabs.selectedTabIndex = 0;
        }

        function test_size_bug1424635() {
            compare(page.x, 0, "Page does not anchor to the left.");
            compare(page.y, testCase.headerHeight, "Page does not align with the bottom of the header.");
            compare(page.width, mainView.width, "Page does not align with the main view on the right side.");
            compare(page.height + testCase.headerHeight , mainView.height, "Page does not align with the bottom of the main view.");
        }
    }
}
