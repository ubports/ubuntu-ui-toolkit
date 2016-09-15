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

import QtTest 1.0
import Ubuntu.Test 1.3
import Ubuntu.Components 1.3
import QtQuick 2.4
import Ubuntu.Components.Popups 1.3

Button {
    id: root
    width: units.gu(50)
    height: units.gu(50)

    property Item popup: null

    Component {
        id: menuFactory
        ActionSelectionPopover {
            delegate: ListItemLayout {
                property Action action
                title.text: action.text
            }

            actions: ActionList {
                Action {
                    objectName: "foo"
                    text: "foo"
                }
                Action {
                    objectName: "bar"
                    text: "bar"
                }
            }
        }
    }
    onClicked: popup = PopupUtils.open(menuFactory, root)

    ActionList {
        id: actionList
        Action {
        }
        Action {
        }
    }

    Action {
        id: dynamicListAction
    }

    UbuntuTestCase {
        when: windowShown

        function test_actionlist() {
            verify(actionList.actions.length, 2, "Default actions not added to actionList");
        }

        function test_actionlist_dynamic_actions() {
            actionList.addAction(dynamicListAction);
            verify(actionList.actions.length, 3, "Dynamic action not added to actionList");
            actionList.removeAction(dynamicListAction);
            verify(actionList.actions.length, 2, "Dynamic action not remove from actionList");
        }

        function test_bug1623841() {
            mouseClick(root, centerOf(root).x, centerOf(root).y);
            waitForRendering(root.popup, 500);
            // check if the delegates are shown
            verify(findChild(root.popup, "foo_button"));
            verify(findChild(root.popup, "bar_button"));
        }
    }
}
