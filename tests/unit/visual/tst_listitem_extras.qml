/*
 * Copyright 2015 Canonical Ltd.
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
import Ubuntu.Test 1.3
import Ubuntu.Components 1.3

Item {
    id: main
    width: units.gu(50)
    height: units.gu(100)

    ListItemActions {
        id: trailing
        actions: [
            Action {
                iconName: "starred"
                text: 'Bookmark'
                objectName: "trailing1"
            },
            Action {
                iconName: "edit"
                text: 'Edit'
                objectName: "trailing2"
                onTriggered: text = 'Edit Again'
            },
            Action {
                iconName: "camcorder"
                text: 'Record'
                objectName: "trailing3"
            }
        ]
    }
    ListItemActions {
        id: leading
        actions: Action {
            id: stockAction
            iconName: "torch-on"
            objectName: "leading1"
            text: 'Switch lights on'
        }
    }

    Column {
        anchors.fill: parent

        ListItem {
            id: testWithActiveItem
            leadingActions: leading
            trailingActions: trailing
            Button {
                id: activeItem
                text: "pressme"
                anchors.centerIn: parent
            }
        }

        ListItem {
            id: overlaidMouseArea
            leadingActions: leading
            trailingActions: trailing
            property bool acceptEvent
            MouseArea {
                id: overlayArea
                anchors.fill: parent
                onPressed: mouse.accepted = overlaidMouseArea.acceptEvent
            }
        }
        ListView {
            id: listView
            width: parent.width
            height: 4 * units.gu(7) // 4 items
            clip: true
            model: 5
            delegate: ListItem {
                objectName: "listItem" + index
                Label {
                    anchors.centerIn: parent
                    text: index
                }

                leadingActions: leading
            }
        }
        ListItem {
            id: emptyActionList
            leadingActions: ListItemActions {}
            trailingActions: ListItemActions {
                actions: []
            }
        }
        ListItem {
            id: contentDragging
            swipeEnabled: ma.drag.active
            Rectangle {
                id: draggedItem
                width: units.gu(2)
                height: width
                color: "red"
                MouseArea {
                    id: ma
                    anchors.fill: parent
                    drag.target: parent
                }
            }
        }
    }

    ListItemTestCase13 {
        name: "ListItemExtras"
        SignalSpy {
            id: clickSpy
            signalName: "clicked"
        }

        function initTestCase() {
            TestExtras.registerTouchDevice();
        }

        function cleanup() {
            rebound(testWithActiveItem);
            rebound(overlaidMouseArea);
            clickSpy.target = null;
            clickSpy.clear();
            wait(200);
        }

        function test_swipe_over_active_item() {
            clickSpy.target = activeItem;
            swipe(activeItem, centerOf(activeItem).x, centerOf(activeItem).y, units.gu(10));
            compare(clickSpy.count, 0, "activeItem was clicked");
        }

        function test_swipe_over_mousearea_not_accepting_press_data() {
            return [
                {tag: "Accept events", accept: true},
                {tag: "Do not accept events", accept: false},
            ];
        }
        function test_swipe_over_mousearea_not_accepting_press(data) {
            overlaidMouseArea.acceptEvent = data.accept;
            setupSpy(overlaidMouseArea, "contentMovementEnded");
            swipeNoWait(overlayArea, centerOf(overlayArea).x, centerOf(overlayArea).y, units.gu(10));
            spyWait();
        }

        function test_swipe_out_from_overlay_button_bug1497156_data() {
            return [
                {tag: "leading with mouse", touch: false, swipeInDx: units.gu(20), swipeOutDx: -units.gu(5)},
                {tag: "trailing with mouse", touch: false, swipeInDx: -units.gu(20), swipeOutDx: units.gu(5)},
                {tag: "leading with touch", touch: true, swipeInDx: units.gu(20), swipeOutDx: -units.gu(5)},
                {tag: "trailing with touch", touch: true, swipeInDx: -units.gu(20), swipeOutDx: units.gu(5)},
            ]
        }
        function test_swipe_out_from_overlay_button_bug1497156(data) {
            // swipe in and out from teh same point
            if (data.touch) {
                tug(testWithActiveItem, centerOf(testWithActiveItem).x, centerOf(testWithActiveItem).y, data.swipeInDx, 0);
            } else {
                swipe(testWithActiveItem, centerOf(testWithActiveItem).x, centerOf(testWithActiveItem).y, data.swipeInDx, 0);
            }
            verify(testWithActiveItem.contentItem.x != 0, "Not swiped in");
            // swipe out
            if (data.touch) {
                tug(testWithActiveItem, centerOf(testWithActiveItem).x, centerOf(testWithActiveItem).y, data.swipeOutDx, 0);
            } else {
                swipe(testWithActiveItem, centerOf(testWithActiveItem).x, centerOf(testWithActiveItem).y, data.swipeOutDx, 0);
            }
            tryCompareFunction(function() {
                return testWithActiveItem.contentItem.x == testWithActiveItem.contentItem.anchors.leftMargin;
            }, true, 500);
        }

        function test_swipe_over_contextual_actions_bug1486008_data() {
            return [
                {tag: "leading action with mouse", touch: false, dx: units.gu(20), leadingPanel: true, action: "leading1"},
                {tag: "trailing action with mouse", touch: false, dx: -units.gu(20), leadingPanel: false, action: "trailing1"},
                {tag: "leading action with touch", touch: true, dx: units.gu(20), leadingPanel: true, action: "leading1"},
                {tag: "trailing action with touch", touch: true, dx: -units.gu(20), leadingPanel: false, action: "trailing1"},
            ];
        }
        function test_swipe_over_contextual_actions_bug1486008(data) {
            if (data.touch) {
                tug(testWithActiveItem, centerOf(testWithActiveItem).x, centerOf(testWithActiveItem).y, data.dx, 0);
            } else {
                swipe(testWithActiveItem, centerOf(testWithActiveItem).x, centerOf(testWithActiveItem).y, data.dx, 0);
            }
            var panel = panelItem(testWithActiveItem, data.leadingPanel);
            var actionItem = findChild(panel, data.action);
            verify(actionItem, data.action + " action not found.");
            // swipe over the action
            setupSpy(testWithActiveItem, "contentMovementStarted");
            if (data.touch) {
                tugNoWait(actionItem, centerOf(actionItem).x, centerOf(actionItem).y, -data.dx, 0);
            } else {
                swipeNoWait(actionItem, centerOf(actionItem).x, centerOf(actionItem).y, -data.dx, 0);
            }
            expectFail(data.tag, "should not swipe");
            spyWait();
        }

        function test_button_inactive_while_swiped_data() {
            return [
                {tag: "mouse", touch: false, dx: units.gu(20)},
                {tag: "touch", touch: true, dx: units.gu(20)},
            ];
        }
        function test_button_inactive_while_swiped(data) {
            clickSpy.target = activeItem;
            if (data.touch) {
                tug(testWithActiveItem, centerOf(testWithActiveItem).x, centerOf(testWithActiveItem).y, data.dx, 0);
                TestExtras.touchClick(0, activeItem, centerOf(activeItem));
            } else {
                swipe(testWithActiveItem, centerOf(testWithActiveItem).x, centerOf(testWithActiveItem).y, data.dx, 0);
                mouseClick(activeItem, centerOf(activeItem).x, centerOf(activeItem).y);
            }
            expectFail(data.tag, "Button is inactive while swiped");
            clickSpy.wait(200);
        }

        function test_click_before_and_after_snapout_bug1496468_data() {
            var item0 = findChild(listView, "listItem0");
            var item1 = findChild(listView, "listItem1");
            return [
                {tag: "mouse", touch: false, clickedItem: item0, swipedItem: item1, dx: units.gu(20), reboundDx: -units.gu(5)},
                {tag: "touch", touch: true, clickedItem: item0, swipedItem: item1, dx: units.gu(20), reboundDx: -units.gu(5)},
            ];
        }
        function test_click_before_and_after_snapout_bug1496468(data) {
            clickSpy.target = data.clickedItem;
            if (data.touch) {
                TestExtras.touchClick(0, data.clickedItem, centerOf(data.clickedItem));
            } else {
                mouseClick(data.clickedItem, centerOf(data.clickedItem).x, centerOf(data.clickedItem).y);
            }
            clickSpy.wait(200);
            // swipe in then rebound
            if (data.touch) {
                tug(data.swipedItem, centerOf(data.swipedItem).x, centerOf(data.swipedItem).y, data.dx, 0);
                wait(200);
                tug(data.swipedItem, centerOf(data.swipedItem).x, centerOf(data.swipedItem).y, data.reboundDx, 0);
            } else {
                swipe(data.swipedItem, centerOf(data.swipedItem).x, centerOf(data.swipedItem).y, data.dx, 0);
                wait(200);
                swipe(data.swipedItem, centerOf(data.swipedItem).x, centerOf(data.swipedItem).y, data.reboundDx, 0);
            }
            // then test click
            clickSpy.target = data.swipedItem;
            clickSpy.clear();
            if (data.touch) {
                TestExtras.touchClick(0, data.swipedItem, centerOf(data.swipedItem));
            } else {
                mouseClick(data.swipedItem, centerOf(data.swipedItem).x, centerOf(data.swipedItem).y);
            }
            clickSpy.wait(200);
        }

        function test_swipe_on_empty_actions_bug1500416_data() {
            return [
                {tag: "swipe leading, touch", item: emptyActionList, dx: units.gu(5), touch: true},
                {tag: "swipe trailing, touch", item: emptyActionList, dx: -units.gu(5), touch: true},
                {tag: "swipe leading, mouse", item: emptyActionList, dx: units.gu(5), touch: false},
                {tag: "swipe trailing, mouse", item: emptyActionList, dx: -units.gu(5), touch: false}
            ];
        }
        function test_swipe_on_empty_actions_bug1500416(data) {
            setupSpy(data.item, "contentMovementEnded");
            if (data.touch) {
                tugNoWait(data.item, centerOf(data.item).x, centerOf(data.item).y, data.dx, 0);
            } else {
                swipeNoWait(data.item, centerOf(data.item).x, centerOf(data.item).y, data.dx, 0);
            }
            expectFailContinue(data.tag, "No swipe should happen");
            spyWait(200);
        }

        function test_swipe_not_possible_when_swipe_disabled_data() {
            listView.positionViewAtBeginning();
            return [
                {tag: "leading, touch", item: findChild(listView, "listItem0"), dx: units.gu(10), touch: true},
                {tag: "trailing, touch", item: findChild(listView, "listItem0"), dx: -units.gu(10), touch: true},
                {tag: "leading, mouse", item: findChild(listView, "listItem0"), dx: units.gu(10), touch: false},
                {tag: "trailing, mouse", item: findChild(listView, "listItem0"), dx: -units.gu(10), touch: false},
            ];
        }
        function test_swipe_not_possible_when_swipe_disabled(data) {
            verify(data.item, "test item not found");
            data.item.swipeEnabled = false;
            setupSpy(data.item, "contentMovementEnded");
            if (data.touch) {
                tugNoWait(data.item, centerOf(data.item).x, centerOf(data.item).y, data.dx, 0);
            } else {
                swipeNoWait(data.item, centerOf(data.item).x, centerOf(data.item).y, data.dx, 0);
            }
            expectFailContinue(data.tag, "No swipe should happen");
            spyWait(200);
            data.item.swipeEnabled = true;
        }

        function test_drag_listitem_content_bug1500409_data() {
            return [
                {tag: "touch", touch: true},
                {tag: "mouse", touch: false}
            ];
        }
        function test_drag_listitem_content_bug1500409(data) {
            setupSpy(contentDragging, "contentMovementStarted");
            if (data.touch) {
                TestExtras.touchDrag(0, draggedItem, centerOf(draggedItem), Qt.point(units.gu(10), units.gu(3)));
            } else {
                mouseDrag(draggedItem, centerOf(draggedItem).x, centerOf(draggedItem).y, units.gu(10), units.gu(3));
            }
            expectFailContinue("", "drag disables swipe");
            spyWait(200);
        }
    }
}
