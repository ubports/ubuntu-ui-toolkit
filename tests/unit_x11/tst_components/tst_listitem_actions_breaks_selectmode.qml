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
    width: units.gu(40)
    height: units.gu(71)


    ListView {
        id: listView
        anchors {
            fill: parent
        }

        model: ListModel {
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

        delegate: ListItem {
            objectName: "listItem" + index
            trailingActions: ListItemActions {
                actions: [
                    Action {
                        iconName: "edit"
                    }
                ]
            }

            onPressAndHold: {
                ListView.view.ViewItems.selectMode = !ListView.view.ViewItems.selectMode
            }

            Label {
                anchors {
                    centerIn: parent
                }
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
            listView.ViewItems.selectMode = false;
            wait(400);
        }

        function test_long_press_after_swipe_breaks_selectmode() {
            var listItem = findChild(listView, "listItem2");
            verify(listItem);
            swipe(listItem, centerOf(listItem).x, centerOf(listItem).y, units.gu(5), 0);
            wait(200);

            // press and hold to activate select mode
            mouseLongPress(listItem, centerOf(listItem).x, centerOf(listItem).y);
            wait(200);
            compare(listItem.selectMode, true, "selectMode not turned on");
        }

        function test_long_press_on_other_after_swipe_breaks_selectmode() {
            var listItem = findChild(listView, "listItem2");
            verify(listItem);
            var otherListItem = findChild(listView, "listItem0");
            verify(otherListItem);
            swipe(listItem, centerOf(listItem).x, centerOf(listItem).y, units.gu(5), 0);
            wait(200);

            // long press on otherListItem
            mouseLongPress(otherListItem, centerOf(otherListItem).x, centerOf(otherListItem).y);
            wait(200);
            compare(listItem.selectMode, true, "selectMode not turned on on swiped");
        }
    }
}
