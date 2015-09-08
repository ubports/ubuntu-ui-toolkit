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
import Ubuntu.Test 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.Styles 1.3
import QtQml.Models 2.1

Item {
    id: main
    width: units.gu(50)
    height: units.gu(100)

    Action {
        id: stockAction
        iconName: "torch-on"
        objectName: "stockAction"
        text: 'Switch lights on'
    }
    ListItemActions {
        id: leading
        actions: [
            Action {
                iconName: "starred"
                text: 'Bookmark'
                objectName: "leading_1"
            },
            Action {
                iconName: "edit"
                text: 'Edit'
                objectName: "leading_2"
                onTriggered: text = 'Edit Again'
            },
            Action {
                iconName: "camcorder"
                text: 'Record'
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
    ListModel {
        id: objectModel
        function reset() {
            clear();
            for (var i = 0; i < 25; i++) {
                append({data: i});
            }
        }
        Component.onCompleted: reset()
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
        id: testColumn
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
            Label {
                id: bodyItem
                anchors.fill: parent
                text: "Data"
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
            model: objectModel
            ViewItems.selectMode: false
            LayoutMirroring.childrenInherit: true
            delegate: ListItem {
                objectName: "listItem" + index
                color: "lightgray"
                leadingActions: leading
                trailingActions: trailing
                Label {
                    text: "Data: " + modelData + " @" + index
                }
            }
        }
        Flickable {
            id: testFlickable
            width: parent.width
            height: units.gu(21)
            ListView {
                id: nestedListView
                width: parent.width
                height: units.gu(28)
                clip: true
                model: 10
                delegate: ListItem {
                    objectName: "listItem" + index
                    leadingActions: leading
                }
            }
        }
        Flickable {
            id: flickable
            width: parent.width
            height: units.gu(14)
            clip: true
            contentHeight: column.height
            Column {
                id: column
                width: parent.width
                Repeater {
                    model: 10
                    ListItem {
                        objectName: "listItem" + index
                        color: "lightgreen"
                    }
                }
            }
        }
    }

    ListItemTestCase13 {
        id: testCase
        name: "ListItem13API"
        when: windowShown

        SignalSpy {
            id: movingSpy
            signalName: "contentMovementEnded"
        }
        SignalSpy {
            id: highlightedSpy
            signalName: "highlightedChanged"
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

        SignalSpy {
            id: dropSpy
            signalName: "stopped"
        }

        function initTestCase() {
            TestExtras.registerTouchDevice();
            waitForRendering(main);
        }

        function cleanup() {
            listView.model = objectModel;
            testItem.action = null;
            testItem.contentItem.anchors.margins = 0;
            testItem.selected = false;
            testColumn.ViewItems.selectMode = false;
            waitForRendering(testItem.contentItem, 200);
            controlItem.selected = false;
            waitForRendering(controlItem.contentItem, 200);
            movingSpy.clear();
            highlightedSpy.clear();
            clickSpy.clear();
            actionSpy.clear();
            pressAndHoldSpy.clear();
            buttonSpy.clear();
            interactiveSpy.clear();
            listView.interactive = true;
            listView.ViewItems.selectMode = false;
            listView.ViewItems.dragMode = false;
            // make sure we collapse
            mouseClick(defaults, 0, 0)
            movingSpy.target = null;
            movingSpy.clear();
            interactiveSpy.target = null;
            interactiveSpy.clear();
            trailing.delegate = null;
            listView.positionViewAtBeginning();
            // keep additional timeout for proper cleanup
            wait(200);
        }

        function test_0_defaults() {
            verify(defaults.contentItem !== null, "Defaults is null");
            compare(defaults.color, "#000000", "Transparent by default");
            compare(defaults.highlightColor, theme.palette.selected.background, "theme.palette.selected.background color by default")
            compare(defaults.highlighted, false, "Not highlighted by default");
            compare(defaults.divider.visible, true, "divider is visible by default");
            compare(defaults.divider.anchors.leftMargin, 0, "divider's left margin is 0");
            compare(defaults.divider.anchors.rightMargin, 0, "divider's right margin is 0");
            var mappedDividerPos = defaults.mapFromItem(defaults.divider, defaults.divider.x, defaults.divider.y);
            compare(mappedDividerPos.x, 0, "divider's left anchor is wrong");
            compare(mappedDividerPos.x + defaults.divider.width, defaults.width, "divider's right anchor is wrong");
            compare(defaults.divider.height, units.dp(2), "divider's thickness is wrong");
            compare(defaults.divider.colorFrom, "#000000", "colorFrom differs.");
            fuzzyCompare(defaults.divider.colorFrom.a, 0.14, 0.01, "colorFrom alpha differs");
            compare(defaults.divider.colorTo, "#ffffff", "colorTo differs.");
            fuzzyCompare(defaults.divider.colorTo.a, 0.07, 0.01, "colorTo alpha differs");
            compare(defaults.action, null, "No action by default.");
            compare(defaults.style, null, "Style is loaded upon first use.");
            compare(defaults.__styleInstance, null, "__styleInstance must be null.");
            compare(defaults.selected, false, "Not selected by default");
            compare(defaults.selectMode, false, "Not selectable by default");
            compare(testColumn.ViewItems.selectMode, false, "The parent attached property is not selectable by default");
            compare(testColumn.ViewItems.selectedIndices.length, 0, "No item is selected by default");
            compare(listView.ViewItems.dragMode, false, "Drag mode is off on ListView");

            compare(actionsDefault.delegate, null, "ListItemActions has no delegate set by default.");
            compare(actionsDefault.actions.length, 0, "ListItemActions has no actions set.");
        }

        Component { id: customStyle; ListItemStyle {} }

        function test_children_in_content_item() {
            compare(bodyItem.parent, testItem.contentItem, "Content is not in the right holder!");
        }

        function test_highlightedChanged_on_click() {
            highlightedSpy.target = testItem;
            mousePress(testItem, testItem.width / 2, testItem.height / 2);
            highlightedSpy.wait();
            mouseRelease(testItem, testItem.width / 2, testItem.height / 2);
        }
        function test_highlightedChanged_on_tap() {
            highlightedSpy.target = testItem;
            TestExtras.touchPress(0, testItem, centerOf(testItem));
            highlightedSpy.wait();
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

        SignalSpy {
            id: visibleSpy
            signalName: "visibleChanged"
        }

        function test_context_menu() {
            mouseClick(testItem, testItem.width / 2, testItem.height / 2, Qt.RightButton);
            wait(1000);
            compare(testItem.highlighted, true, "List item didn't highlight on right-click");
            var context_menu = findChild(main, "listItemContextMenu");
            verify(context_menu, "Context menu didn't open on right-click");
            waitForRendering(context_menu);
            var edit = findChildWithProperty(context_menu, "text", "Edit");
            verify(edit, "Context menu has no 'Edit' item");
            visibleSpy.target = context_menu;
            mouseClick(edit, edit.width / 2, edit.height / 2);
            compare(edit.text, 'Edit Again', "Item wasn't triggered'");
            visibleSpy.wait()
        }

        function test_no_click_when_swiped() {
            var item = findChild(listView, "listItem0");
            clickSpy.target = item;
            clickSpy.clear();
            swipe(item, centerOf(item).x, centerOf(item).y, units.gu(20), 0);

            // click over the contentItem
            movingSpy.target = item;
            mouseClick(item.contentItem, 1, 1);
            compare(clickSpy.count, 0, "No click() should be emitted on a swiped in ListItem.");
            movingSpy.wait();
        }

        function test_no_pressAndHold_when_swiped() {
            var item = findChild(listView, "listItem0");
            pressAndHoldSpy.target = item;
            pressAndHoldSpy.clear();
            swipe(item, centerOf(item).x, centerOf(item).y, units.gu(20), 0);

            // press and hold
            movingSpy.target = item;
            mouseLongPress(item.contentItem, 1, 1);
            mouseRelease(item.contentItem, 1, 1);
            mouseRelease(item.contentItem, 1, 1);
            compare(pressAndHoldSpy.count, 0, "No pressAndHold() should be emitted on a swiped in ListItem.");
            movingSpy.wait();
        }

        function test_vertical_listview_move_cancels_highlight_data() {
            return [
                {tag: "With touch", mouse: false},
                {tag: "With mouse", mouse: true},
            ];
        }
        function test_vertical_listview_move_cancels_highlight(data) {
            var listItem = findChild(listView, "listItem0");
            verify(listItem, "Cannot find listItem0");

            // convert positions and use the listView to move
            var pos = listView.mapFromItem(listItem, listItem.width / 2, 0);
            if (data.mouse) {
                // provide slow move
                mousePress(listView, pos.x, pos.y);
                for (var i = 1; i < 4; i++) {
                    pos.y += i * units.gu(0.5);
                    mouseMove(listView, pos.x, pos.y, 100);
                }
                compare(listItem.highlighted, false, "highlighted still!");
                mouseRelease(listView, pos.x, pos.y, undefined, undefined, 100);
            } else {
                // convert pos to point otherwise touch functions will get (0,0) points!!!
                var pt = Qt.point(pos.x, pos.y);
                TestExtras.touchPress(0, listView, pt);
                for (i = 1; i < 4; i++) {
                    pt.y += i * units.gu(0.5);
                    TestExtras.touchMove(0, listView, pt);
                    wait(100);
                }
                compare(listItem.highlighted, false, "highlighted still!");
                TestExtras.touchRelease(0, listView, pt);
            }
        }

        function test_background_height_change_on_divider_visible() {
            // make sure the testItem's divider is shown
            testItem.divider.visible = true;
            var margins = testItem.contentItem.anchors.topMargin + testItem.contentItem.anchors.bottomMargin;
            compare(testItem.contentItem.height, testItem.height - margins - testItem.divider.height, "ListItem's background height must be less than the item itself.");
            testItem.divider.visible = false;
            waitForRendering(testItem.contentItem);
            compare(testItem.contentItem.height, testItem.height - margins, "ListItem's background height must be the same as the item itself.");
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
            if (data.mouse) {
                swipe(data.item, data.pos.x, data.pos.y, data.dx, 0);
            } else {
                tug(data.item, data.pos.x, data.pos.y, data.dx, 0);
            }
            if (data.positiveDirection) {
                verify(data.item.contentItem.x > 0, data.tag + " actions did not show up");
            } else {
                verify(data.item.contentItem.x < 0, data.tag + " actions did not show up");
            }

            // dismiss
            rebound(data.item);
        }

        function test_tug_ignored_on_right_button() {
            listView.positionViewAtBeginning();
            var item = findChild(listView, "listItem0");
            movingSpy.target = item;
            flick(item, centerOf(item).x, centerOf(item).y, units.gu(20), 0, 0, 0, Qt.RightButton, undefined, 100);
            compare(movingSpy.count, 0, "Action panel should not budge!")
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
            if (data.mouse) {
                swipe(data.item, data.pos.x, data.pos.y, data.dx, 0);
            } else {
                tug(data.item, data.pos.x, data.pos.y, data.dx, 0);
            }
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
            interactiveSpy.target = listView;
            compare(listView.interactive, true, "ListView is not interactive");
            interactiveSpy.target = listView;
            if (data.mouse) {
                swipe(data.item, data.pos.x, data.pos.y, data.dx, data.dy);
            } else {
                tug(data.item, data.pos.x, data.pos.y, data.dx, data.dy);
            }
            // animation should no longer be running!
            compare(listView.interactive, true, "The ListView is still non-interactive!");
            compare(interactiveSpy.count, 2, "Less/more times changed!");
            // check if it snapped in
            verify(data.item.contentItem.x != 0.0, "Not snapped in!!");
            // dismiss
            rebound(data.clickOn, data.item);
            fuzzyCompare(data.item.contentItem.x, data.item.contentItem.anchors.leftMargin, 0.1, "Not snapped out!!");
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
            swipe(data.item, centerOf(data.item).x, centerOf(data.item).y, data.leading ? units.gu(20) : -units.gu(20), 0);

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

        function test_listitem_margins_data() {
            var item = findChild(listView, "listItem1");
            return [
                {tag: "leading", item: item, dx: units.gu(10), leading: true},
                {tag: "trailing", item: item, dx: -units.gu(10), leading: false}
            ];
        }
        function test_listitem_margins(data) {
            data.item.contentItem.anchors.margins = units.gu(1);
            swipe(data.item, centerOf(data.item).x, centerOf(data.item).y, data.dx, 0);
            var panel = panelItem(data.item, data.leading);
            verify(panel && panel.visible, "Panel not visible.");
            // cleanup
            rebound(data.item);
            compare(data.item.contentItem.x, units.gu(1), "contentItem.x differs from margin");
            data.item.contentItem.anchors.margins = 0;
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
            if (data.mouse) {
                swipe(data.item, data.pos.x, data.pos.y, data.dx, 0);
            } else {
                tug(data.item, data.pos.x, data.pos.y, data.dx, 0);
            }
            verify(data.item.contentItem.x > 0, "Not snapped in!");
            var panel = panelItem(data.item, data.leading);
            verify(panel, "panelItem not found");
            var selectedAction = findChild(panel, data.select);
            verify(selectedAction, "Cannot select action " + data.select);

            // dismiss
            movingSpy.target = data.item;
            if (data.mouse) {
                mouseClick(selectedAction, centerOf(selectedAction).x, centerOf(selectedAction).y);
            } else {
                TestExtras.touchClick(0, selectedAction, centerOf(selectedAction));
            }
            movingSpy.wait();
            fuzzyCompare(data.item.contentItem.x, data.item.contentItem.anchors.leftMargin, 0.1, "Content not snapped out");
        }

        function test_custom_trailing_delegate() {
            trailing.delegate = customDelegate;
            listView.positionViewAtBeginning();
            var item = findChild(listView, "listItem0");
            movingSpy.target = item;
            swipeNoWait(item, centerOf(item).x, centerOf(item).y, -units.gu(20), 0);
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
                {tag: "Snap in leading", item: listItem, dx: units.gu(6), snapIn: true},
                {tag: "Snap out trailing", item: listItem, dx: -units.gu(2), snapIn: false},
                {tag: "Snap in trailing", item: listItem, dx: -units.gu(6), snapIn: true},
            ];
        }
        function test_snap(data) {
            swipe(data.item, centerOf(data.item).x, centerOf(data.item).y, data.dx, 0);
            waitForRendering(data.item.contentItem, 400);
            if (data.snapIn) {
                verify(data.item.contentItem.x != 0.0, "Not snapped to be visible");
                // cleanup
                rebound(data.item);
            } else {
                tryCompareFunction(function() { return data.item.contentItem.x; }, data.item.contentItem.anchors.leftMargin, 1000, "Not snapped back");
            }
        }

        function test_snap_gesture_data() {
            var listItem = findChild(listView, "listItem0");
            var front = Qt.point(listItem.contentItem.anchors.leftMargin + units.gu(1), listItem.height / 2);
            var rear = Qt.point(listItem.width - (listItem.contentItem.anchors.rightMargin + units.gu(1)), listItem.height / 2);
            return [
                // the first dx must be big enough to drag the panel in, it is always the last dx value
                // which decides the snap direction
                {tag: "Snap out, leading", item: listItem, grabPos: front, dx: [units.gu(10), -units.gu(3)], snapIn: false},
                {tag: "Snap in, leading", item: listItem, grabPos: front, dx: [units.gu(10), -units.gu(1), units.gu(1.5)], snapIn: true},
                // have less first dx as the trailing panel is shorter
                {tag: "Snap out, trailing", item: listItem, grabPos: rear, dx: [-units.gu(5), units.gu(2)], snapIn: false},
                {tag: "Snap in, trailing", item: listItem, grabPos: rear, dx: [-units.gu(5), units.gu(1), -units.gu(1.5)], snapIn: true},
            ];
        }
        function test_snap_gesture(data) {
            // performe the moves
            movingSpy.target = data.item;
            var pos = data.grabPos;
            mousePress(data.item, pos.x, pos.y);
            for (var i in data.dx) {
                var dx = data.dx[i];
                mouseMoveSlowly(data.item, pos.x, pos.y, data.dx[i], 0, 5, 100);
                pos.x += data.dx[i];
            }
            mouseRelease(data.item, pos.x, pos.y);
            movingSpy.wait();

            if (data.snapIn) {
                // the contenTitem must be dragged in (snapIn)
                verify(data.item.contentItem.x != 0.0, "Not snapped in!");
                // dismiss
                rebound(data.item);
            } else {
                fuzzyCompare(data.item.contentItem.x, data.item.contentItem.anchors.leftMargin, 0.1, "Not snapped out!");
            }
        }

        function test_verify_action_value_data() {
            listView.positionViewAtBeginning();
            var item0 = findChild(listView, "listItem0");
            var item1 = findChild(listView, "listItem1");
            return [
                // testItem is the child item @index 3 in the topmost Column.
                {tag: "Standalone item, child index 3", item: testItem, result: 3},
                {tag: "ListView, item index 0", item: item0, result: 0},
                {tag: "ListView, item index 1", item: item1, result: 1},
            ];
        }
        function test_verify_action_value(data) {
            // tug actions in
            swipe(data.item, 1, centerOf(data.item).y, units.gu(40), 0);
            wait(2000);
            verify(data.item.contentItem.x != data.item.contentItem.anchors.leftMargin, "Not snapped in");

            var panel = panelItem(data.item, "Leading");
            var action = findChild(panel, "leading_2");
            verify(action, "actions panel cannot be reached");
            // we test the action closest to the list item's contentItem
            actionSpy.target = data.item.leadingActions.actions[1];

            // select the action
            movingSpy.target = data.item;
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
            highlightedSpy.target = data.item;
            mouseClick(data.item, data.x, data.y);
            if (data.pressed) {
                highlightedSpy.wait();
            } else {
                compare(highlightedSpy.count, 0, "Should not be highlighted!");
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
            highlightedSpy.target = clickedConnected;
            mouseLongPress(clickedConnected, centerOf(clickedConnected).x, centerOf(clickedConnected).y);
            highlightedSpy.wait();
            mouseRelease(clickedConnected, centerOf(clickedConnected).x, centerOf(clickedConnected).y);
        }

        function test_listitem_blocks_ascendant_flickables() {
            var listItem = findChild(nestedListView, "listItem0");
            verify(listItem, "Cannot find test item");
            interactiveSpy.target = testFlickable;
            // tug leading
            swipe(listItem, centerOf(listItem).x, centerOf(listItem).y, listItem.width / 2, 0);
            // check if interactive got changed
            interactiveSpy.wait();

            // cleanup!!!
            rebound(listItem);
        }

        function test_action_type_set() {
            stockAction.parameterType = Action.None;
            compare(stockAction.parameterType, Action.None, "No parameter type for stockAction!");
            testItem.action = stockAction;
            compare(stockAction.parameterType, Action.Integer, "No parameter type for stockAction!");
        }

        function test_action_triggered_on_clicked() {
            testItem.action = stockAction;
            actionSpy.target = stockAction;
            mouseClick(testItem, centerOf(testItem).x, centerOf(testItem).y);
            actionSpy.wait();
        }

        function test_action_suppressed_on_longpress() {
            testItem.action = stockAction;
            actionSpy.target = stockAction;
            clickSpy.target = testItem;
            pressAndHoldSpy.target = testItem;
            mouseLongPress(testItem, centerOf(testItem).x, centerOf(testItem).y);
            mouseRelease(testItem, centerOf(testItem).x, centerOf(testItem).y);
            pressAndHoldSpy.wait();
            compare(clickSpy.count, 0, "Click must be suppressed.");
            compare(actionSpy.count, 0, "Action triggered must be suppressed");
        }

        function test_select_indices_updates_selected_items() {
            listView.ViewItems.selectedIndices = [0,1,2];
            toggleSelectMode(listView, true);
            for (var i in listView.ViewItems.selectedIndices) {
                var index = listView.ViewItems.selectedIndices[i];
                var listItem = findChild(listView, "listItem" + index);
                compare(listItem.selected, true, "ListItem at index " + index + " is not selected!");
            }
            toggleSelectMode(listView, false);
            listView.ViewItems.selectedIndices = [];
        }

        function test_toggle_selectMode_data() {
            return [
                {tag: "When not selected", index: 0, selected: false},
                {tag: "When selected", index: 0, selected: true},
            ]
        }
        function test_toggle_selectMode(data) {
            var listItem = findChild(listView, "listItem" + data.index)
            verify(listItem, "Cannot get test item");
            listItem.selected = data.selected;
            toggleSelectMode(listView, true);
            // testItem is the 4th child, so index is 3
            verify(findChild(listItem, "selection_panel" + data.index), "Cannot find selection panel");
            compare(listItem.contentItem.enabled, true, "contentItem is not disabled.");
        }

        SignalSpy {
            id: selectedSpy
            signalName: "selectedChanged"
        }

        function test_toggle_selected_data() {
            return [
                // item = <test-item>, clickOk: <item-to-click-on>, offsetX|Y: <clickOn offset clicked>
                {tag: "Click over selection", selectableHolder: testColumn, item: controlItem, clickOn: "listitem_select", offsetX: units.gu(0.5), offsetY: units.gu(0.5), xfail: false},
                {tag: "Click over contentItem", selectableHolder: testColumn, item: controlItem, clickOn: "ListItemHolder", offsetX: units.gu(0.5), offsetY: units.gu(0.5), xfail: true},
                {tag: "Click over control", selectableHolder: testColumn, item: controlItem, clickOn: "button_in_list", offsetX: units.gu(0.5), offsetY: units.gu(0.5), xfail: true},
            ];
         }
        function test_toggle_selected(data) {
            // make test item selectable first, so the panel is created
            toggleSelectMode(data.selectableHolder, true);
            // get the control to click on
            var clickOn = findChild(data.item, data.clickOn);
            verify(clickOn, "control to be clicked on not found");
            // click on the selection and check selected changed
            selectedSpy.target = data.item;
            selectedSpy.clear();
            mouseClick(clickOn, data.offsetX, data.offsetY);
            if (data.xfail) {
                expectFail(data.tag, "Clicking anywhere else but selection panel should not toggle selection state!");
            }
            selectedSpy.wait();
        }

        SignalSpy {
            id: selectedIndicesSpy
            signalName: "selectedIndicesChanged"
            target: listView.ViewItems
        }

        function test_selectedIndices_change() {
            // move to the end of the view
            listView.positionViewAtEnd();
            var listItem = findChild(listView, "listItem" + (listView.count - 1));
            verify(listItem, "Cannot get tested list item");
            toggleSelectMode(listView, true, false);
            selectedSpy.target = listItem;
            selectedSpy.clear();

            listItem.selected = true;
            selectedSpy.wait();
            selectedIndicesSpy.wait();
        }

        function test_no_tug_when_selectable() {
            movingSpy.target = testItem;
            toggleSelectMode(testColumn, true);

            // try to tug leading
            movingSpy.clear();
            swipeNoWait(testItem, centerOf(testItem).x, centerOf(testItem).y, units.gu(10), 0);
            compare(movingSpy.count, 0, "No tug allowed when in selection mode");
        }

        function test_selectable_and_click() {
            toggleSelectMode(testColumn, true);

            clickSpy.target = testItem;
            mouseClick(testItem, centerOf(testItem).x, centerOf(testItem).y);
            clickSpy.wait();
        }

        function test_selectable_and_pressandhold() {
            toggleSelectMode(testColumn, true);

            pressAndHoldSpy.target = testItem;
            mouseLongPress(testItem, centerOf(testItem).x, centerOf(testItem).y);
            mouseRelease(testItem, centerOf(testItem).x, centerOf(testItem).y);
            pressAndHoldSpy.wait();
        }

        function test_proper_attached_properties_data() {
            return [
                {tag: "Attached to ListView", item: listView},
                {tag: "Attached to Column in Flickable", item: column},
            ];
        }
        function test_proper_attached_properties(data) {
            var listItem = findChild(data.item, "listItem0");
            verify(listItem, "ListItem not found!");
            toggleSelectMode(data.item, true);
            // check if the selection mode was activated by looking after the first selection panel
            var panel = findChild(listItem, "selection_panel0");
            // turn off selection mode so we have a proper cleanup
            toggleSelectMode(data.item, true);
            verify(panel, "Selection panel not found, wrong attached property target?");
        }

        function test_dragmode_availability_data() {
            return [
                {tag: "Attached to Column", item: testColumn, lookupOn: testItem, xfail: true},
                {tag: "Attached to ListView", item: listView, lookupOn: findChild(listView, "listItem0"), xfail: false},
            ];
        }
        function test_dragmode_availability(data) {
            if (data.xfail) {
                ignoreWarning(warningFormat(85, 5, "QML Column: Dragging mode requires ListView"));
            }
            data.item.ViewItems.dragMode = true;
            wait(400);
            var panel = findChild(data.lookupOn, "drag_panel0");
            if (data.xfail) {
                expectFailContinue(data.tag, "There should be no drag handler shown!")
            }
            verify(panel, "No drag handler found!");
        }

        function test_drag_data() {
            return [
                {tag: "Live 0->1 OK", live: true, from: 0, to: 1, count: 1, accept: true, indices:[1,0,2,3,4]},
                {tag: "Live 0->2 OK", live: true, from: 0, to: 2, count: 2, accept: true, indices:[1,2,0,3,4]},
                {tag: "Live 0->3 OK", live: true, from: 0, to: 3, count: 3, accept: true, indices:[1,2,3,0,4]},
                {tag: "Live 3->0 OK", live: true, from: 3, to: 0, count: 3, accept: true, indices:[3,0,1,2,4]},
                        // do not accept moves
                {tag: "Live 0->1 NOK", live: true, from: 0, to: 1, count: 0, accept: false, indices:[0,1,2,3,4]},
                {tag: "Live 0->2 NOK", live: true, from: 0, to: 2, count: 0, accept: false, indices:[0,1,2,3,4]},
                {tag: "Live 0->3 NOK", live: true, from: 0, to: 3, count: 0, accept: false, indices:[0,1,2,3,4]},
                {tag: "Live 3->0 NOK", live: true, from: 3, to: 0, count: 0, accept: false, indices:[0,1,2,3,4]},

                        // non-live updates
                {tag: "Drop 0->1 OK", live: false, from: 0, to: 1, count: 1, accept: true, indices:[1,0,2,3,4]},
                {tag: "Drop 0->2 OK", live: false, from: 0, to: 2, count: 1, accept: true, indices:[1,2,0,3,4]},
                {tag: "Drop 0->3 OK", live: false, from: 0, to: 3, count: 1, accept: true, indices:[1,2,3,0,4]},
                {tag: "Drop 3->0 OK", live: false, from: 3, to: 0, count: 1, accept: true, indices:[3,0,1,2,4]},
                        // do not accept moves
                {tag: "Drop 0->1 NOK", live: false, from: 0, to: 1, count: 0, accept: false, indices:[0,1,2,3,4]},
                {tag: "Drop 0->2 NOK", live: false, from: 0, to: 2, count: 0, accept: false, indices:[0,1,2,3,4]},
                {tag: "Drop 0->3 NOK", live: false, from: 0, to: 3, count: 0, accept: false, indices:[0,1,2,3,4]},
                {tag: "Drop 3->0 NOK", live: false, from: 3, to: 0, count: 0, accept: false, indices:[0,1,2,3,4]},
            ];
        }

        function test_drag(data) {
            var moveCount = 0;
            function liveUpdate(event) {
                if (event.status == ListItemDrag.Started) {
                    return;
                }
                if (data.accept) {
                    moveCount++;
                    listView.model.move(event.from, event.to, 1);
                }
                event.accept = data.accept;
            }
            function singleDrop(event) {
                if (event.status == ListItemDrag.Dropped) {
                    if (data.accept) {
                        moveCount++;
                        listView.model.move(event.from, event.to, 1);
                    }
                    event.accept = data.accept;
                } else if (event.status == ListItemDrag.Moving) {
                    event.accept = false;
                }
            }

            objectModel.reset();
            waitForRendering(listView);
            listView.positionViewAtBeginning();
            var func = data.live ? liveUpdate : singleDrop;
            listView.ViewItems.dragUpdated.connect(func);

            // enter drag mode
            toggleDragMode(listView, true);
            drag(listView, data.from, data.to);
            compare(moveCount, data.count, "Move did not happen or more than one item was moved");
            // compare array indices
            for (var i in data.indices) {
                compare(listView.model.get(i).data, data.indices[i], "data at index " + i + " is not the expected one");
            }

            // cleanup
            listView.ViewItems.dragUpdated.disconnect(func);
            toggleDragMode(listView, false);
        }

        // preconditions:
        // the first 2 items cannot be dragged anywhere, nothing can be dropped in this area
        // the 3-> items can be interchanged in between, cannot be dragged outside
        function test_drag_restricted_data() {
            return [
                {tag: "[0,1] locked, drag 0->1 NOK", from: 0, to: 1, count: 0, indices: [0,1,2,3,4]},
                {tag: "[0,1] locked, drag 1->2 NOK", from: 1, to: 2, count: 0, indices: [0,1,2,3,4]},
                {tag: "[0,1] locked, drag 2->1 NOK", from: 2, to: 1, count: 0, indices: [0,1,2,3,4]},
                {tag: "[0,1] locked, drag 2->0 NOK", from: 2, to: 0, count: 0, indices: [0,1,2,3,4]},
                        // drag
                {tag: "[0,1] locked, drag 2->3 OK", from: 2, to: 3, count: 1, indices: [0,1,3,2,4]},
            ];
        }
        function test_drag_restricted(data) {
            var moveCount = 0;
            function updateHandler(event) {
                if (event.status == ListItemDrag.Started) {
                    if (event.from < 2) {
                        event.accept = false;
                    } else {
                        event.minimumIndex = 2;
                    }
                } else if (event.status == ListItemDrag.Moving) {
                    listView.model.move(event.from, event.to, 1);
                    moveCount++;
                }
            }

            objectModel.reset();
            waitForRendering(listView);
            listView.positionViewAtBeginning();
            listView.ViewItems.dragUpdated.connect(updateHandler);

            // enter drag mode
            toggleDragMode(listView, true);
            drag(listView, data.from, data.to);
            compare(moveCount, data.count, "Move did not happen or more than one item was moved");
            // compare array indices
            for (var i in data.indices) {
                compare(listView.model.get(i).data, data.indices[i], "data at index " + i + " is not the expected one");
            }

            // cleanup
            listView.ViewItems.dragUpdated.disconnect(updateHandler);
            toggleDragMode(listView, false);
        }

        function test_drag_keeps_selected_indexes_data() {
            return [
                {tag: "[0,1,2] selected, move 0->3, live", selected: [0,1,2], from: 0, to: 3, expected: [0,1,3], live: true},
                {tag: "[1,2] selected, move 3->2, live", selected: [1,2], from: 3, to: 2, expected: [1,3], live: true},
                {tag: "[1,2] selected, move 0->3, live", selected: [1,2], from: 0, to: 3, expected: [0,1], live: true},
                {tag: "[1,2] selected, move 3->0, live", selected: [1,2], from: 3, to: 0, expected: [2,3], live: true},
                // non-live updates
                {tag: "[0,1,2] selected, move 0->3, non-live", selected: [0,1,2], from: 0, to: 3, expected: [0,1,3], live: false},
                {tag: "[1,2] selected, move 3->2, non-live", selected: [1,2], from: 3, to: 2, expected: [1,3], live: false},
                {tag: "[1,2] selected, move 0->3, non-live", selected: [1,2], from: 0, to: 3, expected: [0,1], live: false},
                {tag: "[1,2] selected, move 3->0, non-live", selected: [1,2], from: 3, to: 0, expected: [2,3], live: false},
            ];
        }
        function test_drag_keeps_selected_indexes(data) {
            function updateHandler(event) {
                if (event.status == ListItemDrag.Started) {
                    return;
                }
                if (data.live || event.status == ListItemDrag.Dropped) {
                    listView.model.move(event.from, event.to, 1);
                } else {
                    event.accept = false;
                }
            }
            objectModel.reset();
            waitForRendering(listView);
            listView.ViewItems.selectedIndices = data.selected;
            listView.ViewItems.dragUpdated.connect(updateHandler);
            toggleDragMode(listView, true);
            drag(listView, data.from, data.to);
            listView.ViewItems.dragUpdated.disconnect(updateHandler);
            toggleDragMode(listView, false);

            // NOTE: the selected indexes order is arbitrar and cannot be predicted by the test
            // therefore we check the selected indexes presence in the expected list.
            compare(listView.ViewItems.selectedIndices.length, data.expected.length, "The selected indexes and expected list size differs");
            for (var i = 0; i < listView.ViewItems.selectedIndices.length; i++) {
                var index = data.expected.indexOf(listView.ViewItems.selectedIndices[i]);
                verify(index >= 0, "Index " + listView.ViewItems.selectedIndices[i] + " is not expected to be selected!");
            }
        }

        // must run this immediately after the defaults are checked otherwise drag handler connected check will fail
        function test_1_warn_missing_dragUpdated_signal_handler() {
            ignoreWarning(warningFormat(121, 9, "QML ListView: ListView has no ViewItems.dragUpdated() signal handler implemented. No dragging will be possible."));
            toggleDragMode(listView, true);
            drag(listView, 0, 1);
            toggleDragMode(listView, true);
        }

        DelegateModel {
            id: delegateModel
            delegate: ListItem {
                objectName: "listItem" + index
                Label { text: modelData }
            }
        }
        ObjectModel {
            id: objectModel2
            Repeater {
                model: 3
                ListItem {
                    objectName: "listItem" + index
                    Label { text: modelData }
                }
            }
        }
        function test_warn_model_data() {
            var list = [1,2,3,4,5,6,7,8,9,10];
            return [
                {tag: "number", model: 20, warning: "Dragging is only supported when using a QAbstractItemModel, ListModel or list."},
                {tag: "list", model: list, warning: ""},
                {tag: "ListModel", model: objectModel, warning: ""},
                {tag: "DelegateModel with number", model: delegateModel, modelModel: 20, warning: "Dragging is only supported when using a QAbstractItemModel, ListModel or list."},
                {tag: "DelegateModel with list", model: delegateModel, modelModel: list, warning: ""},
                {tag: "DelegateModel with ListModel", model: delegateModel, modelModel: objectModel, warning: ""},
                {tag: "ObjectModel", model: objectModel2, warning: ""},
            ];
        }
        function test_warn_model(data) {
            function dummyFunc() {}
            if (data.warning !== "") {
                ignoreWarning(warningFormat(121, 9, "QML ListView: " + data.warning));
            }
            listView.model = data.model;
            if (typeof data.modelModel !== "undefined") {
                listView.model.model = data.modelModel;
            }
            waitForRendering(listView, 500);
            listView.ViewItems.dragUpdated.connect(dummyFunc);
            toggleDragMode(listView, true);
            toggleDragMode(listView, false);
            listView.ViewItems.dragUpdated.disconnect(dummyFunc);
        }

        function test_rtl_actions_data() {
            return [
                {tag: "Leading actions", item: "listItem0", leading: true, expected: ["leading_1", "leading_2", "leading_3"]},
                {tag: "Trailing actions", item: "listItem0", leading: false, expected: ["stockAction"]},
            ];
        }
        function test_rtl_actions(data) {
            listView.LayoutMirroring.enabled = true;
            waitForRendering(listView, 500);
            var listItem = findChild(listView, data.item);
            swipe(listItem, centerOf(listItem).x, centerOf(listItem).y, data.leading ? -units.gu(20) : units.gu(20), 0);

            // check if the action is visible
            var panel = panelItem(listItem, data.leading);
            verify(panel, "Panel not visible");
            for (var i in data.expected) {
                var actionItem = findChild(panel, data.expected[i]);
                verify(actionItem, data.expected[i] + " action not found");
            }
            // dismiss
            rebound(listItem);
            listView.LayoutMirroring.enabled = false;
            waitForRendering(listView, 500);
        }

        function test_rtl_selection_panel_position() {
            listView.LayoutMirroring.enabled = true;
            waitForRendering(listView, 500);
            toggleSelectMode(listView, true);
            // get the panel
            var listItem = findChild(listView, "listItem0");
            verify(listItem, "ListItem cannot be found");
            var panel = findChild(listView, "selection_panel0");
            verify(panel, "Selection panel not found.");
            verify(listItem.mapFromItem(panel, panel.x, panel.y).x > centerOf(listItem).x, "Panel is not in its proper place!");
            toggleSelectMode(listView, false);
            listView.LayoutMirroring.enabled = false;
            waitForRendering(listView, 500);
        }

        function test_rtl_drag_panel_position() {
            listView.LayoutMirroring.enabled = true;
            waitForRendering(listView, 500);
            toggleDragMode(listView, true);
            // get the panel
            var listItem = findChild(listView, "listItem0");
            verify(listItem, "ListItem cannot be found");
            var panel = findChild(listView, "drag_panel0");
            verify(panel, "Drag panel not found.");
            verify(listItem.mapFromItem(panel, panel.x, panel.y).x < centerOf(listItem).x, "Panel is not in its proper place!");
            toggleDragMode(listView, false);
            listView.LayoutMirroring.enabled = false;
            waitForRendering(listView, 500);
        }

        function test_listitem_actions_width_bug1465582_data() {
            return [
                {tag: "leading", dx: units.gu(5), action: "leading_1"},
                {tag: "trailing", dx: -units.gu(5), action: "stockAction"},
            ];
        }
        function test_listitem_actions_width_bug1465582(data) {
            var height = testItem.height;
            testItem.height = units.gu(15);

            swipe(testItem, centerOf(testItem).x, centerOf(testItem).y, data.dx);

            var icon = findChild(testItem, data.action);
            verify(icon);
            compare(icon.width, units.gu(5), "icon width should be the same no matter of the height set");

            rebound(testItem);

            // restore height
            testItem.height = height;
        }
    }
}
