/*
 * Copyright 2016 Canonical Ltd.
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

Item {
    width: units.gu(40)
    height: units.gu(71)

    Rectangle {
        id: topItem
        activeFocusOnTab: true
        width: parent.width
        height: units.gu(2)
    }

    Loader {
        id: testLoader
        anchors {
            fill: parent
            topMargin: topItem.height
        }
    }

    Component {
        id: simpleListItem
        ListItem {
            objectName: "simple" + index
        }
    }

    Component {
        id: listItemWithContent
        ListItem {
            objectName: "withContent" + index
            Row {
                spacing: units.gu(1)
                CheckBox { objectName: "checkbox" }
                Switch { objectName: "switch" }
                Button { objectName: "button"; text: "test" }
            }
        }
    }

    Component {
        id: listView
        ListView {
            model: 10
        }
    }

    Component {
        id: generic
        Column {
            spacing: units,gu(1)
            Repeater {
                model: 2
                delegate: listItemWithContent
            }
            Repeater {
                model: 2
                delegate: simpleListItem

            }
        }
    }

    UbuntuTestCase {
        name: "ListItemFocus"
        when: windowShown

        function loadTest(component) {
            testLoader.sourceComponent = component;
            tryCompare(testLoader, "status", Loader.Ready);
            return testLoader.item;
        }

        function cleanup() {
            testLoader.sourceComponent = null;
            wait(200);
        }
        function init() {
            topItem.forceActiveFocus(Qt.TabFocusReason);
        }

        function test_focusing_listview_focuses_first_item_data() {
            return [
                {tag: "Tab, no content", preFocus: topItem, delegate: simpleListItem, key: Qt.Key_Tab, focusItem: "simple0"},
                {tag: "Tab, with content", preFocus: topItem, delegate: listItemWithContent, key: Qt.Key_Tab, focusItem: "withContent0"},
                {tag: "Backtab, no content", preFocus: topItem, delegate: simpleListItem, key: Qt.Key_Backtab, focusItem: "simple0"},
                // FIXME: this test requires tab fencing!
                {tag: "Backtab, with content", preFocus: topItem, delegate: listItemWithContent, key: Qt.Key_Backtab, focusItem: "withContent0"},
            ];
        }
        function test_focusing_listview_focuses_first_item(data) {
            var test = loadTest(listView);
            test.delegate = data.delegate;
            waitForRendering(test, 500);
            data.preFocus.forceActiveFocus();
            verify(data.preFocus.activeFocus);
            keyClick(data.key);
            var listItem = findChild(test, data.focusItem);
            verify(listItem);
            verify(listItem.activeFocus);
            verify(listItem.keyNavigationFocus);
        }

        function test_focus_and_navigate_in_listview_data() {
            return [
                {tag: "No content", delegate: simpleListItem, key: Qt.Key_Down, keyTimes: 3, focusItems: ["simple1", "simple2", "simple3"]},
                {tag: "With content", delegate: listItemWithContent, key: Qt.Key_Down, keyTimes: 3, focusItems: ["withContent1", "withContent2", "withContent3"]},
            ];
        }
        function test_focus_and_navigate_in_listview(data) {
            var test = loadTest(listView);
            test.delegate = data.delegate;
            waitForRendering(test, 500);
            keyClick(Qt.Key_Tab);
            for (var i = 0; i < data.keyTimes; i++) {
                keyClick(data.key);
                var item = findChild(test, data.focusItems[i]);
                verify(item);
                verify(item.keyNavigationFocus, "failure on key navigation on " + data.focusItems[i]);
            }
        }

        function test_tab_backtab_navigates_away_of_listview_data() {
            return [
                {tag: "Tab, simple", preFocus: topItem, delegate: simpleListItem, key: Qt.Key_Tab},
                {tag: "Tab, with content", preFocus: topItem, delegate: listItemWithContent, key: Qt.Key_Tab},
                {tag: "BackTab, simple", preFocus: topItem, delegate: simpleListItem, key: Qt.Key_Backtab},
                {tag: "BackTab, with content", preFocus: topItem, delegate: listItemWithContent, key: Qt.Key_Backtab},
            ];
        }
        function test_tab_backtab_navigates_away_of_listview(data) {
            var test = loadTest(listView);
            test.delegate = data.delegate;
            waitForRendering(test, 500);
            data.preFocus.forceActiveFocus();
            verify(data.preFocus.activeFocus);
            // the first tab focuses the ListView and its first child
            keyClick(data.key);
            verify(test.activeFocus);

            // the second tab should leave the ListView
            keyClick(data.key);
            verify(!test.activeFocus);
        }
    }
}

