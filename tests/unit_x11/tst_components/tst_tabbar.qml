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
import TestObjects 0.1

Item {
    id: root
    width: units.gu(40)
    height: units.gu(71)

    TabBar {
        id: bar
        anchors.top: parent.top
        width: parent.width
    }

    TabBar {
        id: bar2
        // anchor to bottom not to occlude the first TabBar
        anchors.bottom: parent.bottom
        width: parent.width
        model: invalidModel
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
        property int selectedIndex: 0
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
        id: invalidModel
        property int selectedIndex: 0
        ListElement {
            fruit: "Pear"
        }
    }

    ListModel {
        id: invalidModelTab
        property int selectedIndex: 0
        ListElement {
            tab: "Pear"
        }
    }

    Item {
        id: myTab
        property string title: "Pear"
    }

    ListModel {
        id: validModelTab
        property int selectedIndex: count > 0 ? 0 : -1
    }

    ListModel {
        id: emptyModelWillBeInvalid
        property int selectedIndex: count > 0 ? 0 : -1
    }

    ListModel {
        id: emptyModel
        property int selectedIndex: count > 0 ? 0 : -1
    }

    TabsModel {
        id: pagesCpp
        property int selectedIndex: count > 0 ? 0 : -1
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
            compare(bar.model, pages);
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
            ];
        }

        function test_2_tabSelection(data) {
            bar.model = pages;
            compare(bar.model, pages);

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

        function test_invalidModel() {
            bar.model = invalidModel;
            compare(bar.model, null, "the model has to be null when setting an invalid model");
        }

        function test_invalidModelTab() {
            bar.model = invalidModelTab;
            compare(bar.model, null, "the model has to be null when setting an invalid model");
        }

        function test_invalidModelTab2() {
            compare(bar2.model, null, "the model has to be null when setting an invalid model");
        }

        function test_validModelTab() {
            bar.model = validModelTab;
            compare(bar.model, validModelTab);
            validModelTab.append({tab: myTab});
            compare(bar.model, validModelTab);
        }

        function test_invalidModelEmptyAtTheBeginning() {
            bar.model = emptyModelWillBeInvalid;
            compare(bar.model, emptyModelWillBeInvalid);
            emptyModelWillBeInvalid.append({fruit: "Pear"});
            compare(bar.model, null, "the model has to be null when setting an invalid model");
        }

        function test_addTabAfterShownQML() {
            bar.model = emptyModel;
            compare(bar.model, emptyModel);
            compare(bar.selectedIndex, -1);
            emptyModel.append({title:"Title 1"});
            compare(bar.selectedIndex, 0);
        }

        function test_addTabAfterShownCpp() {
            bar.model = pagesCpp;
            compare(bar.model, pagesCpp);
            compare(bar.selectedIndex, -1);
            pagesCpp.append("Title 1");
            compare(bar.selectedIndex, 0);
        }
    }
}
