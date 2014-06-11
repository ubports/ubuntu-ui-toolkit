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

    MainView {
        id: mainView
        anchors.fill: parent
        PageStack {
            id: pageStack0
        }

        Page {
            id: page0
            tools: ToolbarItems {
                id: emptyToolbarItems
            }
        }

        Page {
            id: page1
            tools: toolbarItems
            ToolbarItems {
                id: toolbarItems
                ToolbarButton {
                    id: button
                }
            }
        }
    }

    TestCase {
        name: "ToolbarItemsAPI"
        when: windowShown

        function initTestCase() {
            compare(toolbarItems.pageStack, null, "pageStack initially null");
            compare(toolbarItems.locked, false, "toolbar items unlocked with children");
            compare(emptyToolbarItems.locked, true, "toolbar items unlocked without visible children");
            compare(toolbarItems.back === undefined, false, "back item set by default");
            compare(toolbarItems.back.hasOwnProperty("text"), true, "back item has text property");
            compare(toolbarItems.back.hasOwnProperty("visible"), true, "back item has visible property");
        }

        function test_opened() {
            compare(toolbarItems.opened, true, "Toolbar items opened when page is first loaded");
            toolbarItems.opened = false;
            compare(toolbarItems.opened, false, "Toolbar items can be closed");
            toolbarItems.opened = true;
            compare(toolbarItems.opened, true, "Toolbar items can be opened");
        }

        function test_locked() {
            compare(emptyToolbarItems.locked, true, "Toolbar items initially locked for empty toolbar");
            compare(toolbarItems.locked, false, "Toolbar items initially unlocked for non-empty toolbar");
            toolbarItems.locked = true;
            compare(toolbarItems.locked, true, "Toolbar items can be locked");
            toolbarItems.locked = false;
            compare(toolbarItems.locked, false, "Toolbar items can be unlocked");
        }

        function test_back() {
            compare(pageStack0.depth, 0, "pageStack is empty by default");
            compare(page0.tools.back.visible, false, "back item not visible without pageStack");
            compare(page0.tools.pageStack, null, "back.pageStack null by default");
            pageStack0.push(page0);
            compare(pageStack0.depth, 1, "pageStack.depth == 1 after pushing one page");
            compare(page0.tools.pageStack, pageStack0, "pushing a page updates the pageStack of tools item");
            compare(page0.tools.back.visible, false, "back item not visible with only one page on the stack");
            pageStack0.push(page1);
            compare(pageStack0.depth, 2, "pageStack.depth == 2 after pushing two pages");
            compare(page1.tools.pageStack, pageStack0, "pushing a page updates the pageStack of tools item");
            compare(page1.tools.back.visible, true, "back item visible with two pages on the stack");
            pageStack0.pop();
            compare(pageStack0.depth, 1, "pageStack.depth == 1 after popping second page");
            compare(page0.tools.back.visible, false, "back item not visible with only one page on the stack");
            pageStack0.pop();
            compare(pageStack0.depth, 0, "pageStack is empty after popping both pages");
        }

        function test_visible() {
            compare(toolbarItems.visible, true, "Toolbar items are visible when in toolbar");
            mainView.useDeprecatedToolbar = false;
            compare(toolbarItems.visible, false, "Toolbar items are invisible when not using deprecated toolbar");
            mainView.useDeprecatedToolbar = true;
        }
    }
}
