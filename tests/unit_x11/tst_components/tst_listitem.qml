/*
 * Copyright 2014 Canonical Ltd.
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
import Ubuntu.Test 1.0
import Ubuntu.Components 1.2

Item {
    id: main
    width: units.gu(40)
    height: units.gu(71)

    Action {
        id: stockAction
        iconName: "starred"
        objectName: "stockAction"
    }
    ListItemActions {
        id: leading
        actions: [
            Action {
                iconName: "starred"
                objectName: "leading_1"
            },
            Action {
                iconName: "edit"
                objectName: "leading_2"
            },
            Action {
                iconName: "camcorder"
                objectName: "leading_3"
            }
        ]
    }
    ListItemActions {
        id: trailing
        actions: [
            stockAction,
        ]
    }
    ListItemActions {
        id: actionsDefault
    }

    Component {
        id: customDelegate
        Rectangle {
            width: units.gu(10)
            color: "green"
            objectName: "custom_delegate"
        }
    }

    Column {
        width: parent.width
        ListItem {
            id: defaults
            width: parent.width
        }
        ListItem {
            id: highlightTest
        }
        ListItem {
            id: clickedConnected
            onClicked: {}
            onPressAndHold: {}
        }
        ListItem {
            id: testItem
            width: parent.width
            color: "blue"
            leadingActions: leading
            trailingActions: trailing
            Item {
                id: bodyItem
                anchors.fill: parent
            }
        }
        ListItem {
            id: controlItem
            Button {
                id: button
                objectName: "button_in_list"
                anchors.centerIn: parent
                text: "Button"
            }
        }
        ListView {
            id: listView
            width: parent.width
            height: units.gu(28)
            clip: true
            model: 10
            delegate: ListItem {
                objectName: "listItem" + index
                color: "lightgray"
                width: parent.width
                leadingActions: leading
                trailingActions: trailing
            }
        }
    }

    UbuntuTestCase {
        name: "ListItemAPI"
        when: windowShown

        SignalSpy {
            id: movingSpy
            signalName: "contentMovementEnded"
        }

        SignalSpy {
            id: pressedSpy
            signalName: "pressedChanged"
            target: testItem
        }

        SignalSpy {
            id: clickSpy
            signalName: "clicked"
            target: testItem;
        }

        SignalSpy {
            id: actionSpy
            signalName: "onTriggered"
        }
        SignalSpy {
            id: interactiveSpy
            signalName: "interactiveChanged"
        }

        function panelItem(item, leading) {
            return findInvisibleChild(item, (leading ? "ListItemPanelLeading" : "ListItemPanelTrailing"));
        }

        function rebound(item, watchTarget) {
            if (watchTarget === undefined) {
                watchTarget = item;
            }

            movingSpy.target = null;
            movingSpy.target = watchTarget;
            movingSpy.clear();
            mouseClick(item, centerOf(item).x, centerOf(item).y);
            if (watchTarget.contentMoving) {
                movingSpy.wait();
            }
            movingSpy.target = null;
        }

        function initTestCase() {
            TestExtras.registerTouchDevice();
            waitForRendering(main);
        }

        function cleanup() {
            testItem.selected = false;
            testItem.selectable = false;
            waitForRendering(testItem.contentItem, 400);
            controlItem.selected = false;
            controlItem.selectable = false;
            waitForRendering(controlItem.contentItem, 400);
            movingSpy.clear();
            pressedSpy.clear();
            clickSpy.clear();
            actionSpy.clear();
            pressAndHoldSpy.clear();
            buttonSpy.clear();
            interactiveSpy.clear();
            listView.interactive = true;
            // make sure we collapse
            mouseClick(defaults, 0, 0)
            movingSpy.target = null;
            movingSpy.clear();
            interactiveSpy.target = null;
            interactiveSpy.clear();
            trailing.delegate = null;
            listView.positionViewAtBeginning();
        }

        function test_0_defaults() {
            verify(defaults.contentItem !== null, "Defaults is null");
            compare(defaults.color, "#000000", "Transparent by default");
            compare(defaults.highlightColor, Theme.palette.selected.background, "Theme.palette.selected.background color by default")
            compare(defaults.pressed, false, "Not pressed buy default");
            compare(defaults.swipeOvershoot, 0.0, "No overshoot till the style is loaded!");
            compare(defaults.divider.visible, true, "divider is visible by default");
            compare(defaults.divider.leftMargin, units.dp(2), "divider's left margin is 2GU");
            compare(defaults.divider.rightMargin, units.dp(2), "divider's right margin is 2GU");
            compare(defaults.divider.colorFrom, "#000000", "colorFrom differs.");
            fuzzyCompare(defaults.divider.colorFrom.a, 0.14, 0.01, "colorFrom alpha differs");
            compare(defaults.divider.colorTo, "#ffffff", "colorTo differs.");
            fuzzyCompare(defaults.divider.colorTo.a, 0.07, 0.01, "colorTo alpha differs");
            compare(defaults.contentMoving, false, "default is not moving");
            compare(defaults.style, null, "Style is loaded upon first use.");
            compare(defaults.__styleInstance, null, "__styleInstance must be null.");

            compare(actionsDefault.delegate, null, "ListItemActions has no delegate set by default.");
            compare(actionsDefault.actions.length, 0, "ListItemActions has no actions set.");
        }

        function test_children_in_content_item() {
            compare(bodyItem.parent, testItem.contentItem, "Content is not in the right holder!");
        }

        function test_pressedChanged_on_click() {
            pressedSpy.target = testItem;
            mousePress(testItem, testItem.width / 2, testItem.height / 2);
            pressedSpy.wait();
            mouseRelease(testItem, testItem.width / 2, testItem.height / 2);
        }
        function test_pressedChanged_on_tap() {
            pressedSpy.target = testItem;
            TestExtras.touchPress(0, testItem, centerOf(testItem));
            pressedSpy.wait();
            TestExtras.touchRelease(0, testItem, centerOf(testItem));
            // local cleanup, wait few msecs to suppress double tap
            wait(400);
        }

        function test_clicked_on_mouse() {
            clickSpy.target = testItem;
            mouseClick(testItem, testItem.width / 2, testItem.height / 2);
            clickSpy.wait();
        }
        function test_clicked_on_tap() {
            clickSpy.target = testItem;
            TestExtras.touchClick(0, testItem, centerOf(testItem));
            clickSpy.wait();
        }

        function test_mouse_click_on_listitem() {
            var listItem = findChild(listView, "listItem0");
            verify(listItem, "Cannot find listItem0");

            mousePress(listItem, listItem.width / 2, 0);
            compare(listItem.pressed, true, "Item is not pressed?");
            // do 5 moves to be able to sense it
            var dy = 0;
            for (var i = 1; i <= 5; i++) {
                dy += i * 10;
                mouseMove(listItem, listItem.width / 2, dy);
            }
            compare(listItem.pressed, false, "Item is pressed still!");
            mouseRelease(listItem, listItem.width / 2, dy);
            // dismiss
            rebound(listItem);
        }
        function test_touch_click_on_listitem() {
            var listItem = findChild(listView, "listItem0");
            verify(listItem, "Cannot find listItem0");

            TestExtras.touchPress(0, listItem, Qt.point(listItem.width / 2, 5));
            compare(listItem.pressed, true, "Item is not pressed?");
            // do 5 moves to be able to sense it
            var dy = 0;
            for (var i = 1; i <= 5; i++) {
                dy += i * 10;
                TestExtras.touchMove(0, listItem, Qt.point(listItem.width / 2, dy));
            }
            compare(listItem.pressed, false, "Item is pressed still!");
            // cleanup, wait few milliseconds to avoid dbl-click collision
            TestExtras.touchRelease(0, listItem, Qt.point(listItem.width / 2, dy));
            // dismiss
            rebound(listItem);
        }

        function test_background_height_change_on_divider_visible() {
            // make sure the testItem's divider is shown
            testItem.divider.visible = true;
            verify(testItem.contentItem.height < testItem.height, "ListItem's background height must be less than the item itself.");
            testItem.divider.visible = false;
            compare(testItem.contentItem.height, testItem.height, "ListItem's background height must be the same as the item itself.");
            testItem.divider.visible = true;
        }

        function test_tug_actions_data() {
            var item = findChild(listView, "listItem0");
            return [
                {tag: "Trailing, mouse", item: item, pos: centerOf(item), dx: -units.gu(20), positiveDirection: false, mouse: true},
                {tag: "Leading, mouse", item: item, pos: centerOf(item), dx: units.gu(20), positiveDirection: true, mouse: true},
                {tag: "Trailing, touch", item: item, pos: centerOf(item), dx: -units.gu(20), positiveDirection: false, mouse: false},
                {tag: "Leading, touch", item: item, pos: centerOf(item), dx: units.gu(20), positiveDirection: true, mouse: false},
            ];
        }
        function test_tug_actions(data) {
            listView.positionViewAtBeginning();
            movingSpy.target = data.item;
            if (data.mouse) {
                flick(data.item, data.pos.x, data.pos.y, data.dx, 0);
            } else {
                TestExtras.touchDrag(0, data.item, data.pos, Qt.point(data.dx, 0));
            }
            movingSpy.wait();
            if (data.positiveDirection) {
                verify(data.item.contentItem.x > 0, data.tag + " actions did not show up");
            } else {
                verify(data.item.contentItem.x < 0, data.tag + " actions did not show up");
            }

            // dismiss
            rebound(data.item);
        }

        function test_attached_listitem_data() {
            var item = findChild(listView, "listItem3");
            return [
                {tag: "Trailing", item: item, pos: centerOf(item), dx: -units.gu(20), leading: true, index: 3},
                {tag: "Leading", item: item, pos: centerOf(item), dx: units.gu(20), leading: true, index: 3},
            ];
        }
        function test_attached_listitem(data) {
            listView.positionViewAtBeginning();
            movingSpy.target = data.item;
            flick(data.item, data.pos.x, data.pos.y, data.dx, 0);
            movingSpy.wait();
            var panel = panelItem(data.item, data.leading);
            verify(panel, "No panel found");
            compare(panel.ListItemActions.listItem, data.item, "The attached listItem differs from the actual item using the list.");
            compare(panel.ListItemActions.listItemIndex, data.index, "The attached listItem index is wrong.");
            verify(panel.ListItemActions.status != ListItemActions.Disconnected, "The attached status is wrong.");

            // dismiss
            rebound(data.item);
        }

        function test_rebound_when_pressed_outside_or_clicked_data() {
            var item0 = findChild(listView, "listItem0");
            var item1 = findChild(listView, "listItem1");
            return [
                {tag: "Click on an other Item", item: item0, pos: centerOf(item0), dx: -units.gu(20), clickOn: item1, mouse: true},
                {tag: "Click on the same Item", item: item0, pos: centerOf(item0), dx: -units.gu(20), clickOn: item0, mouse: true},
                {tag: "Tap on an other Item", item: item0, pos: centerOf(item0), dx: -units.gu(20), clickOn: item1, mouse: false},
                {tag: "Tap on the same Item", item: item0, pos: centerOf(item0), dx: -units.gu(20), clickOn: item0, mouse: false},
            ];
        }
        function test_rebound_when_pressed_outside_or_clicked(data) {
            listView.positionViewAtBeginning();
            movingSpy.target = data.item;
            if (data.mouse) {
                flick(data.item, data.pos.x, data.pos.y, data.dx, 0);
            } else {
                TestExtras.touchDrag(0, data.item, data.pos, Qt.point(data.dx, 0));
            }
            movingSpy.wait();
            verify(data.item.contentItem.x != 0, "The component wasn't tugged!");
            // dismiss
            rebound(data.clickOn, data.item)
        }

        function test_listview_not_interactive_while_tugged_data() {
            var item0 = findChild(listView, "listItem0");
            var item1 = findChild(listView, "listItem1");
            return [
                {tag: "Trailing", item: item0, pos: centerOf(item0), dx: -units.gu(20), clickOn: item1, mouse: true},
                {tag: "Leading", item: item0, pos: centerOf(item0), dx: units.gu(20), clickOn: item0.contentItem, mouse: true},
                {tag: "Trailing", item: item0, pos: centerOf(item0), dx: -units.gu(20), clickOn: item1, mouse: false},
                {tag: "Leading", item: item0, pos: centerOf(item0), dx: units.gu(20), clickOn: item0.contentItem, mouse: false},
            ];
        }
        function test_listview_not_interactive_while_tugged(data) {
            listView.positionViewAtBeginning();
            compare(listView.interactive, true, "ListView is not interactive");
            movingSpy.target = data.item;
            interactiveSpy.target = listView;
            if (data.mouse) {
                flick(data.item, data.pos.x, data.pos.y, data.dx, data.dy);
            } else {
                TestExtras.touchDrag(0, data.item, data.pos, Qt.point(data.dx, data.dy));
            }
            movingSpy.wait();
            // animation should no longer be running!
            verify(!data.item.__styleInstance.snapAnimation.running, "Animation is still running!");
            compare(listView.interactive, true, "The ListView is still non-interactive!");
            compare(interactiveSpy.count, 2, "Less/more times changed!");
            // check if it snapped in
            verify(data.item.contentItem.x != 0.0, "Not snapped in!!");
            // dismiss
            rebound(data.clickOn, data.item);
            // animation should no longer be running!
            verify(!data.item.__styleInstance.snapAnimation.running, "Animation is still running!");
            fuzzyCompare(data.item.contentItem.x, 0.0, 0.1, "Not snapped out!!");
        }

        function test_visualized_actions_data() {
            var listItem0 = findChild(listView, "listItem0");
            var listItem1 = findChild(listView, "listItem1");
            return [
                {tag: "Leading actions", item: listItem0, leading: true, expected: ["leading_1", "leading_2", "leading_3"]},
                {tag: "Trailing actions", item: listItem0, leading: false, expected: ["stockAction"]},
            ];
        }
        function test_visualized_actions(data) {
            movingSpy.target = data.item;
            flick(data.item, centerOf(data.item).x, centerOf(data.item).y, data.leading ? units.gu(20) : -units.gu(20), 0);
            movingSpy.wait();

            // check if the action is visible
            var panel = panelItem(data.item, data.leading);
            verify(panel, "Panel not visible");
            for (var i in data.expected) {
                var actionItem = findChild(panel, data.expected[i]);
                verify(actionItem, data.expected[i] + " action not found");
            }
            // dismiss
            rebound(data.item);
        }

        function test_selecting_action_rebounds_data() {
            var item0 = findChild(listView, "listItem0");
            return [
                {tag: "With mouse", item: item0, pos: centerOf(item0), dx: units.gu(20), leading: true, select: "leading_1", mouse: true},
                {tag: "With touch", item: item0, pos: centerOf(item0), dx: units.gu(20), leading: true, select: "leading_1", mouse: false},
            ]
        }
        function test_selecting_action_rebounds(data) {
            listView.positionViewAtBeginning();
            movingSpy.target = data.item;
            if (data.mouse) {
                flick(data.item, data.pos.x, data.pos.y, data.dx, 0);
            } else {
                TestExtras.touchDrag(0, data.item, data.pos, Qt.point(data.dx, 0));
            }
            movingSpy.wait();
            verify(data.item.contentItem.x > 0, "Not snapped in!");
            var panel = panelItem(data.item, data.leading);
            verify(panel, "panelItem not found");
            var selectedAction = findChild(panel, data.select);
            verify(selectedAction, "Cannot select action " + data.select);

            // dismiss
            movingSpy.clear();
            if (data.mouse) {
                mouseClick(selectedAction, centerOf(selectedAction).x, centerOf(selectedAction).y);
            } else {
                TestExtras.touchClick(0, selectedAction, centerOf(selectedAction));
            }
            movingSpy.wait();
            fuzzyCompare(data.item.contentItem.x, 0.0, 0.1, "Content not snapped out");
        }

        function test_custom_trailing_delegate() {
            trailing.delegate = customDelegate;
            listView.positionViewAtBeginning();
            var item = findChild(listView, "listItem0");
            movingSpy.target = item;
            flick(item, centerOf(item).x, centerOf(item).y, -units.gu(20), 0);
            var panel = panelItem(item, false);
            verify(panel, "Panel is not visible");
            var custom = findChild(panel, "custom_delegate");
            verify(custom, "Custom delegate not in use");
            movingSpy.wait();
            // cleanup
            rebound(item);
        }

        // execute as last so we make sure we have the panel created
        function test_snap_data() {
            var listItem = findChild(listView, "listItem0");
            verify(listItem, "ListItem cannot be found");

            return [
                // the list snaps out if the panel is dragged in > overshoot GU (hardcoded for now)
                {tag: "Snap out leading", item: listItem, dx: units.gu(2), snapIn: false},
                {tag: "Snap in leading", item: listItem, dx: units.gu(4), snapIn: true},
                {tag: "Snap out trailing", item: listItem, dx: -units.gu(2), snapIn: false},
                {tag: "Snap in trailing", item: listItem, dx: -units.gu(4), snapIn: true},
            ];
        }
        function test_snap(data) {
            movingSpy.target = data.item;
            flick(data.item, centerOf(data.item).x, centerOf(data.item).y, data.dx, 0);
            movingSpy.wait();
            waitForRendering(data.item, 400);
            movingSpy.clear();
            if (data.snapIn) {
                verify(data.item.contentItem.x != 0.0, "Not snapped to be visible");
                // cleanup
                rebound(data.item);
            } else {
                tryCompareFunction(function() { return data.item.contentItem.x; }, 0.0, 1000, "Not snapped back");
            }
        }

        function test_snap_gesture_data() {
            var listItem = findChild(listView, "listItem0");
            var front = Qt.point(units.gu(1), listItem.height / 2);
            var rear = Qt.point(listItem.width - units.gu(1), listItem.height / 2);
            return [
                // the first dx must be big enough to drag the panel in, it is always the last dx value
                // which decides the snap direction
                {tag: "Snap out, leading", item: listItem, grabPos: front, dx: [units.gu(10), -units.gu(2)], snapIn: false},
                {tag: "Snap in, leading", item: listItem, grabPos: front, dx: [units.gu(10), -units.gu(1), units.gu(1)], snapIn: true},
                // have less first dx as the trailing panel is shorter
                {tag: "Snap out, trailing", item: listItem, grabPos: rear, dx: [-units.gu(5), units.gu(2)], snapIn: false},
                {tag: "Snap in, trailing", item: listItem, grabPos: rear, dx: [-units.gu(5), units.gu(1), -units.gu(1)], snapIn: true},
            ];
        }
        function test_snap_gesture(data) {
            // performe the moves
            movingSpy.target = data.item;
            var pos = data.grabPos;
            mousePress(data.item.contentItem, pos.x, pos.y);
            for (var i in data.dx) {
                var dx = data.dx[i];
                mouseMoveSlowly(data.item.contentItem, pos.x, pos.y, dx, 0, 5, 100);
                pos.x += dx;
            }
            mouseRelease(data.item.contentItem, pos.x, pos.y);
            movingSpy.wait();

            if (data.snapIn) {
                // the contenTitem must be dragged in (snapIn)
                verify(data.item.contentItem.x != 0.0, "Not snapped in!");
                // dismiss
                rebound(data.item);
            } else {
                fuzzyCompare(data.item.contentItem.x, 0.0, 0.1, "Not snapped out!");
            }
        }

        function test_overshoot_from_style() {
            // scroll to the last ListView element and test on that, to make sure we don't have the style loaded for that component
            listView.positionViewAtEnd();
            var listItem = findChild(listView, "listItem" + (listView.count - 1));
            verify(listItem, "Cannot get list item for testing");

            compare(listItem.swipeOvershoot, 0.0, "No overshoot should be set yet!");
            // now swipe
            movingSpy.target = listItem;
            flick(listItem.contentItem, centerOf(listItem).x, centerOf(listItem).y, units.gu(5), 0);
            movingSpy.wait();
            compare(listItem.swipeOvershoot, listItem.__styleInstance.swipeOvershoot, "Overshoot not taken from style");

            // cleanup
            rebound(listItem);
        }

        function test_custom_overshoot_data() {
            // use different items to make sure the style doesn't update the overshoot values during the test
            return [
                {tag: "Positive value", index: listView.count - 1, value: units.gu(10), expected: units.gu(10)},
                {tag: "Zero value", index: listView.count - 2, value: 0, expected: 0},
                // synchronize the expected value with the one from Ambiance theme!
                {tag: "Negative value", index: listView.count - 3, value: -1, expected: units.gu(2)},
            ];
        }
        function test_custom_overshoot(data) {
            // scroll to the last ListView element and test on that, to make sure we don't have the style loaded for that component
            listView.positionViewAtEnd();
            var listItem = findChild(listView, "listItem" + data.index);
            verify(listItem, "Cannot get list item for testing");

            compare(listItem.swipeOvershoot, 0.0, "No overshoot should be set yet!");
            listItem.swipeOvershoot = data.value;
            // now swipe
            movingSpy.target = listItem;
            flick(listItem.contentItem, centerOf(listItem).x, centerOf(listItem).y, units.gu(5), 0);
            movingSpy.wait();
            compare(listItem.swipeOvershoot, data.expected, "Overshoot differs from one set!");

            // cleanup
            rebound(listItem);
        }

        function test_verify_action_value_data() {
            var item0 = findChild(listView, "listItem0");
            var item1 = findChild(listView, "listItem1");
            var item2 = findChild(listView, "listItem2");
            var item3 = findChild(listView, "listItem3");
            return [
                // testItem is the child item @index 3 in the topmost Column.
                {tag: "Standalone item, child index 3", item: testItem, result: 3},
                {tag: "ListView, item index 0", item: item0, result: 0},
                {tag: "ListView, item index 1", item: item1, result: 1},
                {tag: "ListView, item index 2", item: item2, result: 2},
                {tag: "ListView, item index 3", item: item3, result: 3},
            ];
        }
        function test_verify_action_value(data) {
            // tug actions in
            movingSpy.target = data.item;
            flick(data.item, centerOf(data.item).x, centerOf(data.item).y, units.gu(20), 0, 100, 10);
            movingSpy.wait();
            verify(data.item.contentItem.x != 0.0, "Not snapped in");

            var panel = panelItem(data.item, "Leading");
            var action = findChild(panel, "leading_2");
            verify(action, "actions panel cannot be reached");
            // we test the action closest to the list item's contentItem
            actionSpy.target = data.item.leadingActions.actions[1];

            // select the action
            movingSpy.clear();
            mouseClick(action, centerOf(action).x, centerOf(action).y);
            movingSpy.wait();

            // check the action param
            actionSpy.wait();
            // SignalSpy.signalArguments[0] is an array of arguments, where the index is set as index 0
            var param = actionSpy.signalArguments[0];
            compare(param[0], data.result, "Action parameter differs");
        }

        function test_highlight_data() {
            return [
                {tag: "No actions", item: highlightTest, x: centerOf(highlightTest).x, y: centerOf(highlightTest).y, pressed: false},
                {tag: "Leading/trailing actions", item: testItem, x: centerOf(testItem).x, y: centerOf(testItem).y, pressed: true},
                {tag: "Active component content", item: controlItem, x: units.gu(1), y: units.gu(1), pressed: true},
                {tag: "Center of active component content", item: controlItem, x: centerOf(controlItem).x, y: centerOf(controlItem).y, pressed: false},
                {tag: "clicked() connected", item: clickedConnected, x: centerOf(clickedConnected).x, y: centerOf(clickedConnected).y, pressed: true},
            ];
        }
        function test_highlight(data) {
            pressedSpy.target = data.item;
            mouseClick(data.item, data.x, data.y);
            if (data.pressed) {
                pressedSpy.wait();
            } else {
                compare(pressedSpy.count, 0, "Should not be pressed!");
            }
        }

        SignalSpy {
            id: pressAndHoldSpy
            signalName: "pressAndHold"
        }
        SignalSpy {
            id: buttonSpy
            signalName: "clicked"
            target: button
        }
        function test_pressandhold_suppress_click() {
            var center = centerOf(testItem);
            pressAndHoldSpy.target = testItem;
            clickSpy.target = testItem;
            clickSpy.clear();
            mouseLongPress(testItem, center.x, center.y);
            mouseRelease(testItem, center.x, center.y);
            pressAndHoldSpy.wait();
            compare(clickSpy.count, 0, "Click must be suppressed when long pressed");
        }

        function test_pressandhold_not_emitted_when_swiped() {
            var center = centerOf(testItem);
            pressAndHoldSpy.target = testItem;
            // move mouse slowly from left to right, the swipe threshold is 1.5 GU!!!,
            // so any value less than that will emit pressAndHold
            mouseMoveSlowly(testItem, center.x, center.y, units.gu(2), 0, 10, 100);
            mouseRelease(testItem, center.x + units.gu(1), center.y);
            compare(pressAndHoldSpy.count, 0, "pressAndHold should not be emitted!");
            // make sure we have collapsed item
            rebound(testItem);
        }

        function test_pressandhold_not_emitted_when_pressed_over_active_component() {
            var press = centerOf(button);
            pressAndHoldSpy.target = controlItem;
            mouseLongPress(button, press.x, press.y);
            compare(pressAndHoldSpy.count, 0, "")
            mouseRelease(button, press.x, press.y);
        }

        function test_click_on_button_suppresses_listitem_click() {
            buttonSpy.target = button;
            clickSpy.target = controlItem;
            mouseClick(button, centerOf(button).x, centerOf(button).y);
            buttonSpy.wait();
            compare(clickSpy.count, 0, "ListItem clicked() must be suppressed");
        }

        function test_pressandhold_connected_causes_highlight() {
            pressedSpy.target = clickedConnected;
            mouseLongPress(clickedConnected, centerOf(clickedConnected).x, centerOf(clickedConnected).y);
            pressedSpy.wait();
            mouseRelease(clickedConnected, centerOf(clickedConnected).x, centerOf(clickedConnected).y);
        }

        function test_toggle_selectable_data() {
            return [
                {tag: "When not selected", selected: false},
                {tag: "When selected", selected: true},
            ]
        }
        function test_toggle_selectable(data) {
            testItem.selected = data.selected;
            testItem.selectable = true;
            waitForRendering(testItem.contentItem);
            verify(findChildWithProperty(testItem, "inSelectionMode", true));
            compare(testItem.contentItem.enabled, false, "contentItem is not disabled.");
        }

        SignalSpy {
            id: selectedSpy
            signalName: "selectedChanged"
        }

        function test_toggle_selected_data() {
            return [
                // item = <test-item>, clickOk: <item-to-click-on>, offsetX|Y: <clickOn offset clicked>
                {tag: "Click over selection", item: controlItem, clickOn: "listitem_select", offsetX: units.gu(0.5), offsetY: units.gu(0.5)},
                {tag: "Click over contentItem", item: controlItem, clickOn: "ListItemHolder", offsetX: units.gu(0.5), offsetY: units.gu(0.5)},
                {tag: "Click over control", item: controlItem, clickOn: "button_in_list", offsetX: units.gu(0.5), offsetY: units.gu(0.5)},
            ];
         }
        function test_toggle_selected(data) {
            // make test item selectable first, so the panel is created
            data.item.selectable = true;
            waitForRendering(data.item.contentItem);
            // get the control to click on
            var clickOn = findChild(data.item, data.clickOn);
            verify(clickOn, "control to be clicked on not found");
            // click on the selection and check selected changed
            selectedSpy.target = data.item;
            selectedSpy.clear();
            mouseClick(clickOn, data.offsetX, data.offsetY);
            selectedSpy.wait();
        }

        function test_no_tug_when_selectable() {
            movingSpy.target = testItem;
            testItem.selectable = true;
            // wait till animation to selection mode ends
            waitForRendering(testItem.contentItem);

            // try to tug leading
            movingSpy.clear();
            flick(testItem, centerOf(testItem).x, centerOf(testItem).y, units.gu(10), 0);
            compare(movingSpy.count, 0, "No tug allowed when in selection mode");
        }

        function test_no_click_when_selectable() {
            testItem.selectable = true;
            // wait till animation to selection mode ends
            waitForRendering(testItem.contentItem);

            clickSpy.target = testItem;
            mouseClick(testItem, centerOf(testItem).x, centerOf(testItem).y);
            compare(clickSpy.count, 0);
        }

        function test_no_pressandhold_when_selectable() {
            testItem.selectable = true;
            // wait till animation to selection mode ends
            waitForRendering(testItem.contentItem);

            pressAndHoldSpy.target = testItem;
            mouseLongPress(testItem, centerOf(testItem).x, centerOf(testItem).y);
            compare(pressAndHoldSpy.count, 0);

            mouseRelease(testItem, centerOf(testItem).x, centerOf(testItem).y);
        }
    }
}
