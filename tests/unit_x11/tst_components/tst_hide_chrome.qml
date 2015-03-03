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
import Ubuntu.Components 1.1

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
                            id: toolbarButton
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

        function cleanup() {
            // clean buffers
            waitForRendering(mainView, 500);
        }

        function openToolbar() {
            var toolbar = mainView.__propagated.toolbar;
            toolbar.open();
            compare(toolbar.opened, true, "Cannot open toolbar using open()");
            wait(UbuntuAnimation.FastDuration); // wait for the open animation to finish
            return toolbar;
        }

        function setTabBarSelectionMode(newSelectionMode) {
            var tabBar = tabs.tabBar;
            var header = mainView.__propagated.header;
            compare(tabBar, header.__styleInstance.__tabBar, "TabBar is not the active header contents");
            header.show();
            tabBar.selectionMode = newSelectionMode;
            compare(tabBar.selectionMode, newSelectionMode, "Failed to set the tab bar selection mode");
            return tabBar;
        }

        function test_tabBar_press_closes_toolbar_bug1223600() {
            testCase.setTabBarSelectionMode(false);
            var toolbar = testCase.openToolbar();
            testCase.mousePress(tabs.tabBar);
            compare(toolbar.opened, false, "Activating TabBar did not close toolbar");
            testCase.mouseRelease(tabs.tabBar);
            testCase.setTabBarSelectionMode(false);
        }

        function test_toolbar_press_closes_tabBar_bug1223606 () {
            var tabBar = testCase.setTabBarSelectionMode(true);
            var toolbar = testCase.openToolbar();
            testCase.mouseClick(toolbarButton,
                                toolbarButton.width/2, toolbarButton.height/2,
                                Qt.LeftButton);
            compare(tabBar.selectionMode, false,
                    "Clicking a toolbar button does not disable tab bar selection mode");

            toolbar.open();
            testCase.setTabBarSelectionMode(true);
            testCase.mousePress(toolbar, toolbar.width/2, toolbar.height/2, Qt.LeftButton);
            compare(tabBar.selectionMode, false, "Pressing the toolbar does not disable tab bar selection mode");
            testCase.mouseRelease(toolbar, toolbar.width/2, toolbar.height/2, Qt.LeftButton);

            toolbar.close();
            testCase.setTabBarSelectionMode(true);
            testCase.mousePress(mainView,
                                mainView.width/2, mainView.height - toolbar.triggerSize/2,
                                Qt.LeftButton);
            compare(tabBar.selectionMode, false,
                    "Triggering the toolbar hint does not disable tab selection mode");
            testCase.mouseRelease(mainView,
                                mainView.width/2, mainView.height - toolbar.triggerSize/2,
                                Qt.LeftButton);
        }
    }
}
