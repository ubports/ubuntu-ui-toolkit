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
import TestObjects 0.1

Item {
    id: root
    width: units.gu(50)
    height: units.gu(80)

    TabBar {
        id: bar
        anchors.top: parent.top
        width: parent.width
    }

    ListView {
        anchors {
            fill: parent
            topMargin: bar.height
        }
        orientation: Qt.Horizontal
        currentIndex: bar.selectedIndex
        model: pages
        delegate: Rectangle {
            width: ListView.view.width
            height: ListView.view.height
            Label {
                anchors.centerIn: parent
                text: title
                fontSize: "x-large"
            }
            color: Qt.rgba(Math.random(0.5), Math.random(1), Math.random(0.5), 1)
        }
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

    ListModel {
        id: pages2
    }

    TabsModel {
        id: pages3
    }

    Label {
        id: textSizer
        text: "Tab 9"
        visible: false
    }

    TestCase {
        name: "TabBarAPI"
        when: windowShown

        // API test
        function test_0_tabsItem() {
            // deprecated
        }

        function test_0_model() {
            compare(bar.model, null, "model default is undefined");
        }

        function test_0_selectionMode() {
            waitForRendering(root);
            compare(bar.selectionMode, true, "default selectionMode");
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

        function test_2_tabSelection_data() {
            // sync textSizer with style settings
            textSizer.fontSize = bar.__styleInstance.headerFontSize;
            textSizer.font.weight = bar.__styleInstance.headerFontWeight;
            var tabWidth = units.gu(2) + textSizer.paintedWidth + bar.__styleInstance.headerTextLeftMargin + bar.__styleInstance.headerTextRightMargin;

            return [
                {"position": tabWidth, "selectedIndex": 1},
                {"position": tabWidth, "selectedIndex": 2},
                {"position": tabWidth, "selectedIndex": 3},
                {"position": tabWidth, "selectedIndex": 4},
                {"position": tabWidth, "selectedIndex": 0},

                {"position": tabWidth, "selectedIndex": 1},
                {"position": tabWidth, "selectedIndex": 2},
                {"position": tabWidth, "selectedIndex": 3},
                {"position": tabWidth, "selectedIndex": 4},
                {"position": 2 * tabWidth, "selectedIndex": 1},

                {"position": 2 * tabWidth, "selectedIndex": 3},
                {"position": 2 * tabWidth, "selectedIndex": 0},
                {"position": 3 * tabWidth, "selectedIndex": 3},
                {"position": tabWidth, "selectedIndex": 4},
                {"position": 2 * tabWidth, "selectedIndex": 1},

                {"position": tabWidth, "selectedIndex": 2}
            ];
        }

        function test_2_tabSelection(data) {
            bar.model = pages;

            if (bar.selectionMode) {
                // wait till the TabBar goes off from selection mode, with an extra threshold
                wait(bar.__styleInstance.deactivateTime + 500);
            }

            // activate tab
            compare(bar.selectionMode, false, "TabBar is not yet in selection mode");
            mouseClick(root, units.gu(2), units.gu(2));
            wait(bar.__styleInstance.headerTextFadeDuration); // wait to show the selections
            compare(bar.selectionMode, true, "TabBar is in selection mode");

            // select the next tab
            mouseClick(root, data.position, units.gu(2));
            compare(bar.selectionMode, false, "TabBar is no longer in selection mode");
            // wait till the selection mode goes off by waiting ~1 second
            wait(bar.__styleInstance.headerTextFadeDuration);
            compare(bar.selectedIndex, data.selectedIndex, "the next tab is selected");
        }

        function test_addTabAfterShown2() {
            bar.model = pages2;
            wait(1000);
            pages2.append({title:"Title 1"});
            wait(10000);
        }

        function test_addTabAfterShown() {
            bar.model = pages3;
            wait(1000);
            pages3.append("Title 1");
            wait(10000);
        }
    }
}
