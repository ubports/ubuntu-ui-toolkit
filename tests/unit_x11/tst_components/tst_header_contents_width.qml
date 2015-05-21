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
    width: units.gu(50)
    height: units.gu(80)

    MainView {
        id: mainView
        anchors.fill: parent

        Tabs {
            id: tabs
            Tab {
                title: "Search"
                page: Page {
                    head.contents: TextField {
                        id: searchField
                        anchors {
                            left: parent ? parent.left : undefined
                            right: parent ? parent.right : undefined
                            rightMargin: units.gu(1)
                        }
                        placeholderText: i18n.tr("Search...")
                    }
                }
            }
            Tab {
                title: "Actions"
                page: Page {
                    head.actions: [
                        Action {
                            iconName: "add"
                        },
                        Action {
                            iconName: "clear"
                        },
                        Action {
                            iconName: "delete"
                        }
                    ]
                }
            }
        }
    }

    UbuntuTestCase {
        name: "HeaderContents"
        when: windowShown
        id: testCase

        property var head_style

        function initTestCase() {
            waitForHeaderAnimation(mainView);
        }

        function test_header_contents_width_bug1408481() {
            var initialWidth = searchField.width;
            testCase.verify(initialWidth > 0, "Initial width has a positive value.");
            // Select the tab that has more actions.
            tabs.selectedTabIndex = 1;
            waitForHeaderAnimation(mainView);
            // Now less space is available for the header contents, so that the action buttons fit.
            testCase.verify(searchField.width < initialWidth, "Contents width is reduced.");
            // Without this wait(), the test does not reproduce bug 1408481.
            wait(100);
            // Select the first tab again:
            tabs.selectedTabIndex = 0;
            waitForHeaderAnimation(mainView);
            // Without actions in tab one, the full width is available again for contents
            testCase.verify(searchField.width === initialWidth, "Contents width is reset.");
        }
    }
}
