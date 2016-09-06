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
import QtQuick.Window 2.2

Item {
    id: main
    width: units.gu(40)
    height: units.gu(71)

    property Item activeFocusItem: Window.activeFocusItem

    PageHeader {
        id: header
        title: "ListItem focus test"
        extension: Sections {
            id: directionSections
            width: parent.width
            selectedIndex: -1
            function setupListView(showContent, orientation, reverseDirection) {
                testCase.cleanup();
                var test = testCase.loadTest(listView);
                test.orientation = orientation;
                if (reverseDirection) {
                    if (orientation === ListView.Horizontal) {
                        test.layoutDirection = Qt.RightToLeft;
                    } else {
                        test.verticalLayoutDirection = ListView.BottomToTop;
                    }
                }
                if (showContent) {
                    test.delegate = listItemWithContent;
                } else {
                    test.delegate = simpleListItem;
                }
            }

            actions: [
                Action {
                    text: "TopToBottom"
                    onTriggered: directionSections.setupListView(switchContentAction.showContent,
                                                                 ListView.Vertical, false);
                },
                Action {
                    text: "LeftToRight"
                    onTriggered: directionSections.setupListView(switchContentAction.showContent,
                                                                 ListView.Horizontal, false);
                },
                Action {
                    text: "BottomToTop"
                    onTriggered: directionSections.setupListView(switchContentAction.showContent,
                                                                 ListView.Vertical, true);
                },
                Action {
                    text: "RightToLeft"
                    onTriggered: directionSections.setupListView(switchContentAction.showContent,
                                                                 ListView.Horizontal, true);
                }

            ]
        }
        trailingActionBar.actions: [
            Action {
                id: switchContentAction
                property bool showContent: false
                iconName: showContent ? "select" : "select-none"
                onTriggered: {
                    showContent = !showContent;
                    if (directionSections.selectedIndex >= 0) {
                        directionSections.actions[directionSections.selectedIndex].trigger();
                    }
                }
            }
        ]
    }

    Rectangle {
        id: topFocusItem
        objectName: "topFocusItem"
        activeFocusOnTab: true
        anchors {
            left: parent.left
            right: parent.right
            top: header.bottom
        }
        height: units.gu(2)
        border {
            width: units.dp(3)
            color: topFocusItem.activeFocus ? UbuntuColors.orange : UbuntuColors.silk
        }
    }
    Loader {
        id: testLoader
        anchors {
            left: parent.left
            right: parent.right
            top: topFocusItem.bottom
            bottom: bottomFocusItem.top
        }
        clip: true
    }
    Rectangle {
        id: bottomFocusItem
        objectName: "bottomFocusItem"
        activeFocusOnTab: true
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: units.gu(2)
        border {
            width: units.dp(3)
            color: bottomFocusItem.activeFocus ? UbuntuColors.orange : UbuntuColors.silk
        }
    }

    Component {
        id: simpleListItem
        ListItem {
            id: listItem
            objectName: "simple" + index
            property int itemIndex: index
            Label {
                anchors {
                    left: parent.left
                    leftMargin: units.gu(2)
                    verticalCenter: parent.verticalCenter
                }
                text: listItem.objectName
            }
        }
    }

    Component {
        id: listItemWithContent
        ListItem {
            id: listItem
            objectName: "withContent" + index
            property int itemIndex: index
            Row {
                spacing: units.gu(1)
                CheckBox { objectName: "checkbox" + listItem.itemIndex }
                Switch { objectName: "switch" + listItem.itemIndex }
                Button { objectName: "button" + listItem.itemIndex; text: "test " + itemIndex }
            }
            leadingActions: ListItemActions {
                actions: Action {
                    iconName: "delete"
                }
            }
            trailingActions: ListItemActions {
                actions: Action {
                    iconName: "edit"
                }
            }
        }
    }

    property int listViewModelSize: 10
    Component {
        id: listView
        ListView {
            model: listViewModelSize
        }
    }

    Component {
        id: generic
        Column {
            spacing: units.gu(1)
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

    ListItemTestCase13 {
        id: testCase
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
            topFocusItem.forceActiveFocus(Qt.TabFocusReason);
        }

        function initTestCase() {
            TestExtras.registerTouchDevice();
        }

        // Tab/Backtab focuses the First ListItem in a ListView
        function test_focusing_listview_focuses_first_item_data() {
            return [
                {tag: "Tab, no content", preFocus: topFocusItem, delegate: simpleListItem, key: Qt.Key_Tab, focusItem: "simple0"},
                {tag: "Tab, with content", preFocus: topFocusItem, delegate: listItemWithContent, key: Qt.Key_Tab, focusItem: "withContent0"},
                {tag: "Backtab, no content", preFocus: bottomFocusItem, delegate: simpleListItem, key: Qt.Key_Backtab, focusItem: "simple0"},
                {tag: "Backtab, with content", preFocus: bottomFocusItem, delegate: listItemWithContent, key: Qt.Key_Backtab, focusItem: "withContent0"},
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
            tryCompare(listItem, "activeFocus", true, 500, "Focus hasn't been gained by the ListItem");
            verify(listItem.keyNavigationFocus);
        }

        // key navigation in ListView between ListItems
        function test_focus_and_navigate_in_listview_data() {
            var n = main.listViewModelSize;
            return [
                        {   tag: "Down, no content, from beginning",
                            delegate: simpleListItem,
                            orientation: ListView.Vertical,
                            initialIndex: 0,
                            key: Qt.Key_Down,
                            keyTimes: 3,
                            focusItems: ["simple1", "simple2", "simple3"]
                        },
                        {   tag: "BottomToTop up, no content, bottom to top direction, from beginning",
                            delegate: simpleListItem,
                            orientation: ListView.Vertical,
                            direction: ListView.BottomToTop,
                            initialIndex: 0,
                            key: Qt.Key_Up,
                            keyTimes: 3,
                            focusItems: ["simple1", "simple2", "simple3"]
                        },
                        {
                            tag: "Down, no contents, near end",
                            delegate: simpleListItem,
                            orientation: ListView.Vertical,
                            initialIndex: n - 3,
                            key: Qt.Key_Down,
                            keyTimes: 3,
                            // Don't navigate beyond the last item:
                            focusItems: ["simple"+(n-2), "simple"+(n-1), "simple"+(n-1)]
                        },
                        {
                            tag: "BottomToTop up, no contents, near end",
                            delegate: simpleListItem,
                            orientation: ListView.Vertical,
                            direction: ListView.BottomToTop,
                            initialIndex: n - 3,
                            key: Qt.Key_Up,
                            keyTimes: 3,
                            // Don't navigate beyond the last item:
                            focusItems: ["simple"+(n-2), "simple"+(n-1), "simple"+(n-1)]
                        },
                        {   tag: "Up, no content, near beginning",
                            delegate: simpleListItem,
                            orientation: ListView.Vertical,
                            initialIndex: 2,
                            key: Qt.Key_Up,
                            keyTimes: 3,
                            focusItems: ["simple1", "simple0", "simple0"]
                        },
                        {   tag: "BottomToTop down, no content, near beginning",
                            delegate: simpleListItem,
                            orientation: ListView.Vertical,
                            direction: ListView.BottomToTop,
                            initialIndex: 2,
                            key: Qt.Key_Down,
                            keyTimes: 3,
                            focusItems: ["simple1", "simple0", "simple0"]
                        },
                        {
                            tag: "Up, no contents, from end",
                            delegate: simpleListItem,
                            orientation: ListView.Vertical,
                            initialIndex: n - 1,
                            key: Qt.Key_Up,
                            keyTimes: 3,
                            focusItems: ["simple"+(n-2), "simple"+(n-3), "simple"+(n-4)]
                        },
                        {
                            tag: "BottomToTop down, no contents, from end",
                            delegate: simpleListItem,
                            orientation: ListView.Vertical,
                            direction: ListView.BottomToTop,
                            initialIndex: n - 1,
                            key: Qt.Key_Down,
                            keyTimes: 3,
                            focusItems: ["simple"+(n-2), "simple"+(n-3), "simple"+(n-4)]
                        },
                        {   tag: "Left, no content, near beginning",
                            delegate: simpleListItem,
                            orientation: ListView.Horizontal,
                            initialIndex: 2,
                            key: Qt.Key_Left,
                            keyTimes: 3,
                            focusItems: ["simple1", "simple0", "simple0"]
                        },
                        {   tag: "RightToLeft right, no content, near beginning",
                            delegate: simpleListItem,
                            orientation: ListView.Horizontal,
                            direction: Qt.RightToLeft,
                            initialIndex: 2,
                            key: Qt.Key_Right,
                            keyTimes: 3,
                            focusItems: ["simple1", "simple0", "simple0"]
                        },
                        {   tag: "Left, no content, from end",
                            delegate: simpleListItem,
                            orientation: ListView.Horizontal,
                            initialIndex: n - 1,
                            key: Qt.Key_Left,
                            keyTimes: 3,
                            focusItems: ["simple"+(n-2), "simple"+(n-3), "simple"+(n-4)]
                        },
                        {   tag: "RightToLeft right, no content, from end",
                            delegate: simpleListItem,
                            orientation: ListView.Horizontal,
                            direction: Qt.RightToLeft,
                            initialIndex: n - 1,
                            key: Qt.Key_Right,
                            keyTimes: 3,
                            focusItems: ["simple"+(n-2), "simple"+(n-3), "simple"+(n-4)]
                        },
                        {   tag: "Right, no content, from beginning",
                            delegate: simpleListItem,
                            orientation: ListView.Horizontal,
                            initialIndex: 0,
                            key: Qt.Key_Right,
                            keyTimes: 5,
                            focusItems: ["simple1", "simple2", "simple3", "simple4", "simple5"]
                        },
                        {   tag: "RightToLeft left, no content, from beginning",
                            delegate: simpleListItem,
                            orientation: ListView.Horizontal,
                            direction: Qt.RightToLeft,
                            initialIndex: 0,
                            key: Qt.Key_Left,
                            keyTimes: 5,
                            focusItems: ["simple1", "simple2", "simple3", "simple4", "simple5"]
                        },
                        {   tag: "Right, no content, near end",
                            delegate: simpleListItem,
                            orientation: ListView.Horizontal,
                            initialIndex: n - 3,
                            key: Qt.Key_Right,
                            keyTimes: 2,
                            focusItems: ["simple"+(n-2), "simple"+(n-1), "simple"+(n-1)]
                        },
                        {   tag: "RightToLeft left, no content, near end",
                            delegate: simpleListItem,
                            orientation: ListView.Horizontal,
                            direction: Qt.RightToLeft,
                            initialIndex: n - 3,
                            key: Qt.Key_Left,
                            keyTimes: 2,
                            focusItems: ["simple"+(n-2), "simple"+(n-1), "simple"+(n-1)]
                        },
                        {   tag: "Down, with content, from beginning",
                            delegate: listItemWithContent,
                            orientation: ListView.Vertical,
                            initialIndex: 0,
                            key: Qt.Key_Down,
                            keyTimes: 3,
                            focusItems: ["withContent1", "withContent2", "withContent3"]
                        },
                        {   tag: "BottomToTop up, with content, from beginning",
                            delegate: listItemWithContent,
                            orientation: ListView.Vertical,
                            direction: ListView.BottomToTop,
                            initialIndex: 0,
                            key: Qt.Key_Up,
                            keyTimes: 3,
                            focusItems: ["withContent1", "withContent2", "withContent3"]
                        },
                        {   tag: "Down, with content, near end",
                            delegate: listItemWithContent,
                            orientation: ListView.Vertical,
                            initialIndex: n - 3,
                            key: Qt.Key_Down,
                            keyTimes: 3,
                            focusItems: ["withContent"+(n-2), "withContent"+(n-1), "withContent"+(n-1)]
                        },
                        {   tag: "BottomToTop up, with content, near end",
                            delegate: listItemWithContent,
                            orientation: ListView.Vertical,
                            direction: ListView.BottomToTop,
                            initialIndex: n - 3,
                            key: Qt.Key_Up,
                            keyTimes: 3,
                            focusItems: ["withContent"+(n-2), "withContent"+(n-1), "withContent"+(n-1)]
                        },
                        {   tag: "Up, with content, near beginning",
                            delegate: listItemWithContent,
                            orientation: ListView.Vertical,
                            initialIndex: 2,
                            key: Qt.Key_Up,
                            keyTimes: 3,
                            focusItems: ["withContent1", "withContent0", "withContent0"]
                        },
                        {   tag: "BottomToTop down, with content, near beginning",
                            delegate: listItemWithContent,
                            orientation: ListView.Vertical,
                            direction: ListView.BottomToTop,
                            initialIndex: 2,
                            key: Qt.Key_Down,
                            keyTimes: 3,
                            focusItems: ["withContent1", "withContent0", "withContent0"]
                        },
                        {   tag: "Up, with content, from end",
                            delegate: listItemWithContent,
                            orientation: ListView.Vertical,
                            initialIndex: n - 1,
                            key: Qt.Key_Up,
                            keyTimes: 2,
                            focusItems: ["withContent"+(n-2), "withContent"+(n-3)]
                        },
                        {   tag: "BottomToTop down, with content, from end",
                            delegate: listItemWithContent,
                            orientation: ListView.Vertical,
                            direction: ListView.BottomToTop,
                            initialIndex: n - 1,
                            key: Qt.Key_Down,
                            keyTimes: 2,
                            focusItems: ["withContent"+(n-2), "withContent"+(n-3)]
                        }
                        // Horizontal navigation for ListItems with content is not supported.
            ];
        }
        function test_focus_and_navigate_in_listview(data) {
            var test = loadTest(listView);
            test.orientation = data.orientation;
            test.delegate = data.delegate;
            // The cases where data.direction is set to ListView.BottomtoTop or Qt.RightToLeft
            //  are regression tests for bug #1605634.
            if (data.direction) {
                if (data.orientation === ListView.Horizontal) {
                    test.layoutDirection = data.direction;
                } else {
                    test.verticalLayoutDirection = data.direction;
                }
            }
            test.currentIndex = data.initialIndex;
            waitForRendering(test, 500);
            keyClick(Qt.Key_Tab);
            for (var i = 0; i < data.keyTimes; i++) {
                keyClick(data.key);
                var item = findChild(test, data.focusItems[i]);
                verify(item);
                tryCompare(item, "activeFocus", true, 500, "Focus hasn't been gained by the ListItem");
                verify(item.keyNavigationFocus, "failure on key navigation on " + data.focusItems[i]);
                compare(test.currentItem, item);
                compare(test.currentIndex, item.itemIndex);
            }
        }

        // re-focusing ListView will focus on the last focused item
        function test_refocus_listview_on_last_focused_item_data() {
            return [
                {tag: "No content", delegate: simpleListItem, key: Qt.Key_Down, keyTimes: 3, focusItems: ["simple1", "simple2", "simple3"]},
                {tag: "With content", delegate: listItemWithContent, key: Qt.Key_Down, keyTimes: 3, focusItems: ["withContent1", "withContent2", "withContent3"]},
            ];
        }
        function test_refocus_listview_on_last_focused_item(data) {
            var test = loadTest(listView);
            test.delegate = data.delegate;
            waitForRendering(test, 500);
            // focus on ListView and focus the 3rd item
            test.forceActiveFocus();
            test.currentIndex = 2;
            waitForRendering(test, 400);
            verify(!test.currentItem.keyNavigationFocus, "Focus frame shown for the item");
            // focus away
            keyClick(Qt.Key_Tab);
            waitForRendering(test, 400);
            // then focus back
            keyClick(Qt.Key_Backtab);
            waitForRendering(test, 400);
            tryCompare(test.currentItem, "activeFocus", true, 500, "Focus hasn't been gained by the ListItem");
            verify(test.currentItem.keyNavigationFocus, "Focus frame not shown for the item");
        }

        // Tab/Backtab focuses, next Tab/Backtab focuses out of ListItem in a ListView
        function test_tab_backtab_navigates_away_of_listview_data() {
            return [
                {tag: "Tab, simple", preFocus: topFocusItem, delegate: simpleListItem, key: Qt.Key_Tab},
                {tag: "Tab, with content", preFocus: topFocusItem, delegate: listItemWithContent, key: Qt.Key_Tab},
                {tag: "BackTab, simple", preFocus: bottomFocusItem, delegate: simpleListItem, key: Qt.Key_Backtab},
                {tag: "BackTab, with content", preFocus: bottomFocusItem, delegate: listItemWithContent, key: Qt.Key_Backtab},
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
            tryCompare(test, "activeFocus", true, 500, "Focus hasn't been gained by the ListItem");

            // the second tab should leave the ListView
            keyClick(data.key);
            tryCompare(test, "activeFocus", false, 500, "Focus hasn't been lost by the ListItem");
        }

        // testing Tab/Backtab navigation when in a generic item
        function test_tab_navigation_when_not_in_listview_data() {
            return [
                {tag: "Tabs", firstFocus: topFocusItem, key: Qt.Key_Tab,
                            focusItems: ["withContent0", "checkbox0", "switch0", "button0"
                                , "withContent1", "checkbox1", "switch1", "button1"]},
                {tag: "Backtabs", firstFocus: bottomFocusItem, key: Qt.Key_Backtab,
                            focusItems: ["simple1", "simple0"
                                , "button1", "switch1", "checkbox1", "withContent1"]},
            ];
        }
        function test_tab_navigation_when_not_in_listview(data) {
            var test = loadTest(generic);
            data.firstFocus.forceActiveFocus();
            for (var i = 0; i < data.focusItems.length; i++) {
                keyClick(data.key);
                compare(main.activeFocusItem.objectName, data.focusItems[i], "Unexpected focused item");
            }
        }

        // focus frame should not be shown
        function test_mouse_or_tap_focus_doesnt_show_focusframe_data() {
            return [
                {tag: "Focus with mouse", mouse: true},
                {tag: "Focus with touch", mouse: false},
            ];
        }
        function test_mouse_or_tap_focus_doesnt_show_focusframe(data) {
            var test = loadTest(listView);
            test.delegate = simpleListItem;
            waitForRendering(test, 500);
            var item = findChild(test, "simple3");
            verify(item);
            if (data.mouse) {
                mouseClick(item, centerOf(item).x, centerOf(item).y);
            } else {
                TestExtras.touchClick(0, item, centerOf(item));
            }
            verify(!item.keyNavigationFocus, "Focus frame must not be shown!");
        }

        // focus with mouse, then press Tab/Backtab, then mouse
        function test_focus_with_mouse_and_tab() {
            var test = loadTest(listView);
            test.delegate = simpleListItem;
            waitForRendering(test, 500);
            var listItem0 = findChild(test, "simple0");
            verify(listItem0);
            var listItem1 = findChild(test, "simple1");
            verify(listItem1);
            var listItem2 = findChild(test, "simple2");
            verify(listItem2);

            // click on first
            mouseClick(listItem0, centerOf(listItem0).x, centerOf(listItem0).y);
            verify(listItem0.activeFocus, "Not focused");
        }

        function test_horizontal_navigation_between_listitem_children_with_tabstop_data() {
            return [
                {tag: "in ListView, rightwards", test: listView, focusItem: "withContent1",
                    key: Qt.Key_Right, focus: ["checkbox1", "switch1", "button1", "withContent1"]},
                {tag: "in ListView, leftwards", test: listView, focusItem: "withContent1",
                    key: Qt.Key_Left, focus: ["button1", "switch1", "checkbox1", "withContent1"]},
                {tag: "in generic, rightwards", test: generic, focusItem: "withContent1",
                    key: Qt.Key_Right, focus: ["checkbox1", "switch1", "button1", "withContent1"]},
                {tag: "in generic, leftwards", test: listView, focusItem: "withContent1",
                    key: Qt.Key_Left, focus: ["button1", "switch1", "checkbox1", "withContent1"]},
            ];
        }
        function test_horizontal_navigation_between_listitem_children_with_tabstop(data) {
            var test = loadTest(data.test);
            if (test.hasOwnProperty("delegate")) {
                test.delegate = listItemWithContent;
                waitForRendering(test, 500);
            }
            var item = findChild(test, data.focusItem);
            verify(item);
            item.forceActiveFocus();
            tryCompare(item, "activeFocus", true, 500, "Focus hasn't been gained by the ListItem");
            for (var i = 0; i < data.focus.length; i++) {
                keyClick(data.key);
                tryCompare(main.activeFocusItem, "objectName", data.focus[i]);
            }
        }

        // executes a combination of tab/navigation keys/backtab sequence
        function test_pattern_data() {
            return [
                {tag: "Tabs in ListView", test: listView, delegate: listItemWithContent, testPlan: [
                        {key: Qt.Key_Tab, focus: "withContent0"},
                        {key: Qt.Key_Tab, focus: "bottomFocusItem"},
                        {key: Qt.Key_Backtab, focus: "withContent0"},
                ]},
                {tag: "Tab and navigate in ListView", test: listView, delegate: listItemWithContent, testPlan: [
                        {key: Qt.Key_Tab, focus: "withContent0"},
                        {key: Qt.Key_Down, focus: "withContent1"},
                        {key: Qt.Key_Left, focus: "button1"},
                        {key: Qt.Key_Left, focus: "switch1"},
                        {key: Qt.Key_Down, focus: "withContent2"},
                        {key: Qt.Key_Right, focus: "checkbox2"},
                        {key: Qt.Key_Right, focus: "switch2"},
                        {key: Qt.Key_Down, focus: "withContent3"},
                        {key: Qt.Key_Down, focus: "withContent4"},
                        {key: Qt.Key_Backtab, focus: "topFocusItem"},
                        {key: Qt.Key_Tab, focus: "withContent4"},
                ]},
                {tag: "Tab and navigate in generic", test: generic, testPlan: [
                        {key: Qt.Key_Tab, focus: "withContent0"},
                        {key: Qt.Key_Down, focus: "withContent0"},
                        {key: Qt.Key_Left, focus: "button0"},
                        {key: Qt.Key_Left, focus: "switch0"},
                        {key: Qt.Key_Down, focus: "switch0"},
                        {key: Qt.Key_Right, focus: "button0"},
                        {key: Qt.Key_Right, focus: "withContent0"},
                        {key: Qt.Key_Down, focus: "withContent0"},
                        {key: Qt.Key_Down, focus: "withContent0"},
                        {key: Qt.Key_Backtab, focus: "topFocusItem"},
                        {key: Qt.Key_Tab, focus: "withContent0"},
                ]},
                {tag: "Mixed Tab and navigate keys in generic", test: generic, testPlan: [
                        {key: Qt.Key_Tab, focus: "withContent0"},
                        {key: Qt.Key_Tab, focus: "checkbox0"},
                        {key: Qt.Key_Tab, focus: "switch0"},
                        {key: Qt.Key_Right, focus: "button0"},
                        {key: Qt.Key_Right, focus: "withContent0"},
                        {key: Qt.Key_Tab, focus: "checkbox0"},
                        {key: Qt.Key_Left, focus: "withContent0"},
                        {key: Qt.Key_Left, focus: "button0"},
                        {key: Qt.Key_Tab, focus: "withContent1"},
                ]},
            ];
        }
        function test_pattern(data) {
            var test = loadTest(data.test);
            if (test.hasOwnProperty("delegate") && data.delegate) {
                test.delegate = data.delegate;
                waitForRendering(test, 500);
            }
            for (var i = 0; i < data.testPlan.length; i++) {
                var plan = data.testPlan[i];
                keyClick(plan.key);
                tryCompare(main.activeFocusItem, "activeFocus", true, 200, "Focus not set for " + plan.focus);
                compare(main.activeFocusItem.objectName, plan.focus);
                if (main.activeFocusItem.hasOwnProperty("keyNavigationFocus")) {
                    verify(main.activeFocusItem.keyNavigationFocus);
                }
            }
        }

        function test_do_not_focus_on_actions_data() {
            return [
                {tag: "leading actions revealed", test: listView, focusItem: "withContent1", leading: true, swipeDx: units.gu(10),
                    key: Qt.Key_Left, focus: ["button1", "switch1", "checkbox1", "withContent1"]},
                {tag: "trailing actions revealed", test: listView, focusItem: "withContent1", leading: false, swipeDx: -units.gu(10),
                    key: Qt.Key_Left, focus: ["button1", "switch1", "checkbox1", "withContent1"]},
            ]
        }
        function test_do_not_focus_on_actions(data) {
            var test = loadTest(data.test);
            if (test.hasOwnProperty("delegate")) {
                test.delegate = listItemWithContent;
                waitForRendering(test, 500);
            }
            var item = findChild(test, data.focusItem);
            verify(item);
            item.forceActiveFocus();
            tryCompare(item, "activeFocus", true, 500, "Focus hasn't been gained by the ListItem");
            // swipe in
            swipe(item, data.leading ? 1 : item.width - 1, centerOf(item).y, data.swipeDx, 0);
            // compare
            for (var i = 0; i < data.focus.length; i++) {
                keyClick(data.key);
                tryCompare(main.activeFocusItem, "objectName", data.focus[i]);
            }
        }

        SignalSpy {
            id: upKeySpy
            signalName: "upPressed"
        }
        SignalSpy {
            id: downKeySpy
            signalName: "downPressed"
        }
        function test_do_not_eat_up_down_key_events_in_listview_bug1554447() {
            var test = loadTest(listView);
            test.delegate = listItemWithContent;
            waitForRendering(test, 500);
            upKeySpy.target = test.Keys;
            downKeySpy.target = test.Keys;
            test.forceActiveFocus();

            // test up
            test.positionViewAtBeginning();
            keyClick(Qt.Key_Up);

            upKeySpy.wait(500);

            // test down
            test.positionViewAtEnd();
            keyClick(Qt.Key_Down);
            downKeySpy.wait(500);

            // test both up and down in the middle
            upKeySpy.clear(); downKeySpy.clear();
            test.positionViewAtIndex(test.count / 2, ListView.Center);
            keyClick(Qt.Key_Up);
            upKeySpy.wait(500);
            keyClick(Qt.Key_Down);
            downKeySpy.wait(500);
        }
    }
}

