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
    id: root
    width: units.gu(50)
    height: units.gu(80)

    TabBar {
        id: bar
        anchors.top: parent.top
        width: parent.width
        height: units.gu(10)
    }

    ListModel {
        id: pages
        ListElement {
            title: "Tab 1"
        }
        ListElement {
            title: "Tab 2"
        }
        ListElement {
            title: "Tab 3"
        }
        ListElement {
            title: "Tab 4"
        }
        ListElement {
            title: "Tab 5"
        }
    }

    TestCase {
        name: "TabBarAPI"

        // API test
        function test_0_tabsItem() {
            // deprecated
        }

        function test_0_model() {
            compare(bar.model, null, "model default is undefined");
        }

        function test_0_selectionMode() {
            waitForRendering(root);
            compare(bar.selectionMode, false, "default selectionMode");
        }

        function test_0_selectedIndex() {
            compare(bar.selectedIndex, -1, "default selectedIndex");
        }

        function test_0_alwaysSelectionMode() {
            compare(bar.alwaysSelectionMode, false, "default alwaysSelectionMode");
        }

        function test_0_animate() {
            compare(bar.animate, true, "default animate");
        }

        function test_1_modelSet() {
            bar.model = pages;
            compare(bar.selectedIndex, 0, "selectedIndex defaults to 0 when model is defined");
        }
    }
}
