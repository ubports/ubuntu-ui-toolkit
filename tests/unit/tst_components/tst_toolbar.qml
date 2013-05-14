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
import Ubuntu.Components 0.1

Item {
    width: 200
    height: 200

    MainView {
        anchors.fill: parent
        id: mainView
        Page {
            id: page
            tools: ToolbarActions {
                id: toolbarActions
                Action {
                    text: "action1"
                }
            }
        }
    }

    TestCase {
        name: "ToolbarAPI"
        when: windowShown

        function initTestCase() {
            compare(page.tools, toolbarActions, "Page tools are set initially");
            compare(page.propagated, mainView.propagated, "propagated property is propagated from mainView to page")
            compare(mainView.propagated.toolbar.tools, page.tools, "Toolbar tools are set to page tools initially");
            compare(mainView.propagated.toolbar.tools.opened, false, "Toolbar is closed initially");
            compare(mainView.propagated.toolbar.tools.locked, false, "Toolbar is initially not locked");
        }

        function test_opened() {
            compare(mainView.propagated.toolbar.tools.opened, false, "Toolbar initially closed");
            mainView.propagated.toolbar.opened = true;
            compare(mainView.propagated.toolbar.opened, true, "Toolbar can be made opened");
            mainView.propagated.toolbar.opened = false;
            compare(mainView.propagated.toolbar.opened, false, "Toolbar can be made closed");
            page.tools.opened = true;
            compare(mainView.propagated.toolbar.opened, true, "Toolbar can be made opened by setting page.tools.opened");
            page.tools.opened = false;
            compare(mainView.propagated.toolbar.opened, false, "Toolbar can be made closed by setting page.tools.opened to false");
        }

        function test_locked() {
            compare(mainView.propagated.toolbar.tools.locked, false, "Toolbar initially not locked");
            mainView.propagated.toolbar.locked = true;
            compare(mainView.propagated.toolbar.locked, true, "Toolbar can be locked");
            mainView.propagated.toolbar.locked = false;
            compare(mainView.propagated.toolbar.locked, false, "Toolbar can be unlocked");
            page.tools.locked = true;
            compare(mainView.propagated.toolbar.locked, true, "Toolbar can be locked by setting page.tools.locked");
            page.tools.locked = false;
            compare(mainView.propagated.toolbar.locked, false, "Toolbar can be unlocked by setting page.tools.locked to false");
        }
    }
}
