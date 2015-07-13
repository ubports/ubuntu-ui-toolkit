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
import Ubuntu.Components 1.2
import QtTest 1.0
import Ubuntu.Test 1.0

MainView {
    id: main
    width: units.gu(40)
    height: units.gu(71)

    ListModel {
        id: testModel
        ListElement {
            text: "a"
        }
        ListElement {
            text: "b"
        }
        ListElement {
            text: "c"
        }
        ListElement {
            text: "d"
        }
        ListElement {
            text: "e"
        }
    }
    ListItemActions {
        id: itemActions
        actions: [
            Action {
                iconName: "edit"
            }
        ]
    }

    ListView {
        id: testView
        anchors {
            left: parent.left
            right: parent.right
        }
        height: main.height / 2
        clip: true
        model: testModel
        delegate: ListItem {
            objectName: "listItem" + index
            trailingActions: itemActions
            onPressAndHold: selectMode = !selectMode

            Label {
                anchors.centerIn: parent
                text: model.text
            }
        }
    }
    ListView {
        id: testView2
        anchors {
            top: testView.bottom
            left: parent.left
            right: parent.right
        }
        height: main.height / 2
        clip: true
        model: testModel
        delegate: ListItem {
            objectName: "listItem" + index
            leadingActions: itemActions
            onPressAndHold: selectMode = !selectMode

            Label {
                anchors.centerIn: parent
                text: model.text
            }
        }
    }

    UbuntuTestCase {
        when: windowShown

        // delayed swipe, gives few millisecond timeout between each move
        // so Repeater has time to create the panel actions in style
        function swipe(item, x, y, dx, dy) {
            flick(item, x, y, dx, dy, 0, 0, undefined, undefined, 100);
        }

        function cleanup() {
            testView.ViewItems.selectMode = false;
            testView2.ViewItems.selectMode = false;
            wait(400);
        }

        function test_long_press_after_swipe_breaks_selectmode_bug1468100_data() {
            return [
                {tag: "leading", view: testView, dx: units.gu(10)},
                {tag: "trailing", view: testView2, dx: -units.gu(10)},
            ]
        }
        function test_long_press_after_swipe_breaks_selectmode_bug1468100(data) {
            var listItem = findChild(data.view, "listItem2");
            verify(listItem);
            swipe(listItem, centerOf(listItem).x, centerOf(listItem).y, data.dx, 0);
            wait(200);

            // press and hold to activate select mode
            mouseLongPress(listItem, centerOf(listItem).x, centerOf(listItem).y);
            mouseRelease(listItem, centerOf(listItem).x, centerOf(listItem).y);
            wait(200);
            compare(listItem.selectMode, true, "selectMode not turned on");
        }

        function test_long_press_on_other_after_swipe_breaks_selectmode_bug1468100_data() {
            return [
                {tag: "leading", view: testView, dx: units.gu(10)},
                {tag: "trailing", view: testView2, dx: -units.gu(10)},
            ]
        }
        function test_long_press_on_other_after_swipe_breaks_selectmode_bug1468100(data) {
            var listItem = findChild(data.view, "listItem2");
            verify(listItem);
            var otherListItem = findChild(data.view, "listItem0");
            verify(otherListItem);
            swipe(listItem, centerOf(listItem).x, centerOf(listItem).y, data.dx, 0);
            wait(200);

            // long press on otherListItem
            mouseLongPress(otherListItem, centerOf(otherListItem).x, centerOf(otherListItem).y);
            mouseRelease(otherListItem, centerOf(otherListItem).x, centerOf(otherListItem).y);
            wait(200);
            compare(listItem.selectMode, true, "selectMode not turned on on swiped");
        }
    }
}
