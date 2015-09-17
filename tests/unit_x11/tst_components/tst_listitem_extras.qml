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
    }
}
