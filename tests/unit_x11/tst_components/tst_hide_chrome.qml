/*
 * Copyright 2013 Canonical Ltd.
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

Item {
    width: units.gu(50)
    height: units.gu(80)

    MainView {
        id: mainView
        anchors.fill: parent
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

                    tools: ToolbarItems {
                        ToolbarButton {
                            text: "something"
                        }
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
        }
    }

    TestCase {
        name: "HideChrome"
        when: windowShown
        id: testCase

        function openToolbar() {
            var toolbar = mainView.__propagated.toolbar;
            toolbar.open();
            compare(toolbar.opened, true, "Cannot open toolbar using open()");
            return toolbar;
        }

        function setTabBarSelectionMode(newSelectionMode) {
            var tabBar = tabs.tabBar;
            var header = mainView.__propagated.header;
            compare(tabBar, header.contents, "TabBar is not the active header contents");
            header.show();
            tabBar.selectionMode = newSelectionMode;
            return tabBar;
        }

        function test_tabBar_selectionMode_closes_toolbar_bug1223600() {
            testCase.setTabBarSelectionMode(false);
            var toolbar = testCase.openToolbar();
            testCase.setTabBarSelectionMode(true);
            compare(toolbar.opened, false, "Activating TabBar did not close toolbar");
        }
    }
}
