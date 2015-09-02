/*
 * Copyright 2014-2015 Canonical Ltd.
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
import QtTest 1.0
import Ubuntu.Test 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.Styles 1.3

Item {
    width: units.gu(40)
    height: units.gu(71)

    ListModel {
        id: testModel
        Component.onCompleted: reload()
        function reload() {
            clear();
            for (var i = 0; i < 25; i++) {
                append({data: i});
            }
        }
    }

    ListItemActions {
        id: actions
        actions: Action {
            iconName: "delete"
        }
    }

    property real expandedHeight//: units.gu(12)

    Column {
        id: defColumn
        anchors.fill: parent
        spacing: units.gu(0.5)
        ListItem {
            id: defaults
        }

        ListView {
            id: listView
            width: parent.width
            height: units.gu(28)
            clip: true
            model: testModel
            delegate: ListItem {
                objectName: "listItem" + index
                Label { text: "ListItem " + modelData }
                expansion.height: expandedHeight
                leadingActions: actions
            }
        }
        Flickable {
            id: flickable
            width: parent.width
            height: units.gu(28)
            clip: true
            contentHeight: flickableColumn.height
            Column {
                id: flickableColumn
                width: parent.width
                Repeater {
                    model: testModel
                    ListItem {
                        objectName: "listItem" + index
                        Label { text: "ListItem " + modelData }
                        expansion.height: expandedHeight
                        leadingActions: actions
                        onClicked: expansion.expanded = !expansion.expanded
                    }
                }
            }
        }
    }

    ListItemTestCase13 {
        name: "ListItemExpansion"
        when: windowShown

        function init() {
            expandedHeight = units.gu(12);
        }

        function cleanup() {
            expandedHeight = 0;
            // move both views to the top
            listView.positionViewAtBeginning();
            flickable.contentY = 0;

            listView.ViewItems.expandedIndices = [];
            flickableColumn.ViewItems.expandedIndices = [];
            listView.ViewItems.expansionFlags = ViewItems.Exclusive | ViewItems.LockExpanded;
            flickableColumn.ViewItems.expansionFlags = ViewItems.Exclusive | ViewItems.LockExpanded;
            wait(200);
        }

        function initTestCase() {
            compare(defaults.expansion.expanded, false, "Not expanded by default");
            compare(defaults.expansion.height, 0, "No expansion height by default");
            verify(defColumn.ViewItems.expandedIndices, "ViewItems.expandedIndices not defined");
            verify(defColumn.ViewItems.expansionFlags, ViewItems.Exclusive | ViewItems.LockExpanded);
        }

        function test_exclusive_expansion_data() {
            return [
                {tag: "ListView", test: listView, expand1: 0, expand2: 2},
                {tag: "Flickable", test: flickableColumn, expand1: 0, expand2: 2},
            ];
        }
        function test_exclusive_expansion(data) {
            var item1 = findChild(data.test, "listItem" + data.expand1);
            var item2 = findChild(data.test, "listItem" + data.expand2);
            verify(item1);
            verify(item2);

            expand(item1, true);
            compare(data.test.ViewItems.expandedIndices.length, 1, "More items expanded");
            compare(data.test.ViewItems.expandedIndices[0], data.expand1, "More items expanded");
            // expand the other one
            expand(item2, true);
            compare(data.test.ViewItems.expandedIndices.length, 1, "More items expanded");
            compare(data.test.ViewItems.expandedIndices[0], data.expand2, "More items expanded");
        }

        function test_multiple_expanded_data() {
            return [
                {tag: "ListView", test: listView, expand1: 0, expand2: 2},
                {tag: "Flickable", test: flickableColumn, expand1: 0, expand2: 2},
            ];
        }
        function test_multiple_expanded(data) {
            var item1 = findChild(data.test, "listItem" + data.expand1);
            var item2 = findChild(data.test, "listItem" + data.expand2);
            verify(item1);
            verify(item2);

            data.test.ViewItems.expansionFlags = 0;
            expand(item1, true);
            expand(item2, true);
            compare(data.test.ViewItems.expandedIndices.length, 2, "Different amount of items expanded");
        }

        function test_locked_while_expanded_data() {
            return [
                {tag: "ListView", test: listView, expand1: 0},
                {tag: "Flickable", test: flickableColumn, expand1: 0},
            ];
        }
        function test_locked_while_expanded(data) {
            var item = findChild(data.test, "listItem" + data.expand1);
            verify(item);

            data.test.ViewItems.expansionFlags = ViewItems.LockExpanded;
            expand(item, true);
            setupSpy(item, "contentMovementEnded");
            swipeNoWait(item, centerOf(item).x, centerOf(item).y, units.gu(10), 0);
            expectFailContinue(data.tag, "No panel swipe is allowed")
            spyWait();
        }

        function test_colapse_on_external_click_data() {
            return [
                {tag: "ListView", test: listView, expand1: 1, clickOn: 0},
                {tag: "Flickable", test: flickableColumn, expand1: 1, clickOn: 0},
            ];
        }
        function test_colapse_on_external_click(data) {
            var item = findChild(data.test, "listItem" + data.expand1);
            var clickItem = findChild(data.test, "listItem" + data.clickOn);
            verify(item);
            verify(clickItem);
            var collapsedHeight = item.height;

            data.test.ViewItems.expansionFlags = ViewItems.CollapseOnOutsidePress;
            expand(item, true);
            mouseClick(clickItem, centerOf(clickItem).x, centerOf(clickItem).y);
            tryCompareFunction(function() { return item.height; }, collapsedHeight, 500);
        }
    }
}
