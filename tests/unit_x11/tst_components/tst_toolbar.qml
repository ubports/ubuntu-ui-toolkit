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
    width: units.gu(50)
    height: units.gu(80)

    MainView {
        anchors.fill: parent
        id: mainView

        Page {
            id: page
            title: "test page"
            Label {
                anchors.centerIn: parent
                text: "testing the toolbar"
            }
            tools: ToolbarItems {
                id: toolbarItems
                ToolbarButton {
                    id: toolbarButton
                    text: "action1"
                }
            }
        }

        ToolbarItems {
            id: lockedTools
            ToolbarButton {
                text: "locked"
            }
            locked: true
            opened: true
        }
    }

    TestCase {
        name: "ToolbarAPI"
        when: windowShown

        function initTestCase() {
            compare(page.tools, toolbarItems, "Page tools are set initially");
            compare(page.__propagated, mainView.__propagated, "propagated property is propagated from mainView to page")
            compare(mainView.__propagated.toolbar.tools, toolbarItems, "Toolbar tools are set to page tools initially");
            compare(toolbarItems.opened, true, "Toolbar is opened initially");
            compare(toolbarItems.locked, false, "Toolbar is initially not locked");
        }

        function test_opened() {
            mainView.__propagated.toolbar.open()
            compare(mainView.__propagated.toolbar.opened, true, "Toolbar can be made opened");
            mainView.__propagated.toolbar.close();
            compare(mainView.__propagated.toolbar.opened, false, "Toolbar can be made closed");
            page.tools.opened = true;
            compare(mainView.__propagated.toolbar.opened, true, "Toolbar can be made opened by setting page.tools.opened");
            page.tools.opened = false;
            compare(mainView.__propagated.toolbar.opened, false, "Toolbar can be made closed by setting page.tools.opened to false");
        }

        function test_hideTimeout_bug1249031() {
            compare(mainView.__propagated.toolbar.hideTimeout, 5000, "Toolbar hide timeout is initially 5 seconds.");
            mainView.__propagated.toolbar.open();
            compare(mainView.__propagated.toolbar.opened, true, "Toolbar can be made opened");
            wait(mainView.__propagated.toolbar.hideTimeout + 500); // add 500 ms margin
            compare(mainView.__propagated.toolbar.opened, false, "Toolbar automatically closes after timeout");

            // now, wait in total more than hideTimeout, but less than 2*hideTimeout,
            //  and have user interaction half-way to verify that the interaction
            //  resets the timer and the toolbar is not closed.
            mainView.__propagated.toolbar.open();
            wait(0.6*mainView.__propagated.toolbar.hideTimeout);
            mouseClick(toolbarButton, toolbarButton.width/2, toolbarButton.height/2);
            wait(0.6*mainView.__propagated.toolbar.hideTimeout);
            compare(mainView.__propagated.toolbar.opened, true, "Interacting with toolbar contents resets the hide timer");
            // verify that the timer is still running by waiting a bit longer:
            wait(0.6*mainView.__propagated.toolbar.hideTimeout);
            compare(mainView.__propagated.toolbar.opened, false, "Interacting with the toolbar contents does not stop the timer")
        }

        function test_locked() {
            compare(mainView.__propagated.toolbar.tools.locked, false, "Toolbar initially not locked");
            mainView.__propagated.toolbar.locked = true;
            compare(mainView.__propagated.toolbar.locked, true, "Toolbar can be locked");
            mainView.__propagated.toolbar.locked = false;
            compare(mainView.__propagated.toolbar.locked, false, "Toolbar can be unlocked");
            page.tools.locked = true;
            compare(mainView.__propagated.toolbar.locked, true, "Toolbar can be locked by setting page.tools.locked");
            page.tools.locked = false;
            compare(mainView.__propagated.toolbar.locked, false, "Toolbar can be unlocked by setting page.tools.locked to false");
        }

        function test_bug1192673() {
            toolbarItems.opened = false;
            mainView.__propagated.toolbar.open();
            compare(toolbarItems.opened, true, "opening the toolbar updates toolbarItems.opened");
            toolbarItems.opened = false;
            compare(mainView.__propagated.toolbar.opened, false, "setting toolbarActions.opened to false closes the toolbar");
        }

        function test_dont_hide_locked_toolbar_bug1248759() {
            page.tools = lockedTools;
            compare(mainView.__propagated.toolbar.tools.locked, true, "Setting locked tools locks the toolbar");
            wait(mainView.__propagated.toolbar.hideTimeout + 500);
            compare(mainView.__propagated.toolbar.opened, true, "Don't auto-hide locked toolbar after timeout");
            // revert original tools for other tests:
            page.tools = toolbarItems;
        }
    }
}
