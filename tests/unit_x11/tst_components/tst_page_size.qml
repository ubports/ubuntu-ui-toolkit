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
                title: "Simple page"
                page: Page {
                    id: page
                    Label {
                        anchors.centerIn: parent
                        text: "simple"
                    }
                }
            }
            Tab {
                title: "Page with Flickable"
                page: Page {
                    id: pageWithFlickable
                    Flickable {
                        anchors.fill: parent
                        contentHeight: 1000
                        Label {
                            anchors.centerIn: parent
                            text: "flickable"
                        }
                    }
                }
            }
            Tab {
                title: "Loaded page"
                Component {
                    id: pageComponent
                    Page {
                        Label {
                            anchors.centerIn: parent
                            text: "loaded"
                        }
                    }
                }
                page: Loader {
                    id: loader
                    sourceComponent: pageComponent
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
            verify(tabs.selectedTabIndex == 0, "First tab not selected initially.");
        }
        function cleanupTestCase() {
            tabs.selectedTabIndex = 0;
        }

        function verify_item_rect(item, x, y, w, h, name) {
            compare(item.x, x, name + " does not anchor to the left.");
            compare(item.y, y, name + " does not align with the bottom of the header.");
            compare(item.width, w, name + " does not align with the main view on the right side.");
            compare(item.height, h, name + " does not align with the bottom of the main view.");
        }

        function test_size_bug1424635() {
            verify_item_rect(page, 0, testCase.headerHeight, mainView.width,
                             mainView.height - testCase.headerHeight, "Page without flickable");
            tabs.selectedTabIndex = 1;
            verify_item_rect(pageWithFlickable, 0, 0, mainView.width, mainView.height,
                             "Page with flickable");
            tabs.selectedTabIndex = 2;
            verify_item_rect(loader, 0, testCase.headerHeight, mainView.width,
                             mainView.height - testCase.headerHeight, "Page loader");
            verify_item_rect(loader.item, 0, 0, mainView.width, loader.height,
                             "Loaded page");
        }
    }
}
