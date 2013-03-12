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

TestCase {
    name: "PageAPI"

    function test_title() {
        compare(page.title, "", "is not set by default")
        compare(mainView.header.title, "", "header title is not set by default")
        var newTitle = "Hello World!"
        page.title = newTitle
        compare(page.title, newTitle, "can set/get")
        compare(mainView.header.title, newTitle, "header title updated by changing page title")
    }

    function test_tools() {
        compare(mainView.toolbar.tools, page.tools, "Page updates toolbar tools");
    }

    function test_active() {
        compare(page.active, true, "Page is active by default");
    }

    function test_pageStack() {
        compare(page.pageStack, null, "is not set by default")
    }

    MainView {
        id: mainView
        Page {
            id: page
        }
    }
}
