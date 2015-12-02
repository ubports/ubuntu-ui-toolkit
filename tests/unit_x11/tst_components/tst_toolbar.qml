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
import Ubuntu.Test 1.0
import Ubuntu.Components 1.3

Item {
    id: root
    width: 400
    height: 600

    property list<Action> actionList:  [
        Action {
            iconName: "alarm-clock"
            text: "Tick tock"
        },
        Action {
            iconName: "appointment"
            text: "Date"
        },
        Action {
            iconName: "attachment"
            text: "Attach"
        },
        Action {
            iconName: "contact"
            text: "Contact"
        },
        Action {
            iconName: "like"
            text: "Like"
        },
        Action {
            iconName: "lock"
            text: "Lock"
        }
    ]

    property list<Action> shortActionList: [
        Action {
            iconName: "share"
            text: "Share"
        },
        Action {
            iconName: "starred"
            text: "Favorite"
        }
    ]

    property Action deleteAction: Action {
        iconName: "delete"
        text: "Delete"
        onTriggered: print("Delete action triggered")
        objectName: "delete_action"
    }

    Column {
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins: units.gu(2)
        }
        height: childrenRect.height
        Item {
            width: parent.width
            height: childrenRect.height

            Label {
                id: shortLabel
                anchors {
                    left: parent.left
                    verticalCenter: shortBar.verticalCenter
                }
                text: "" + shortActionList.length + " actions:"
            }
            Toolbar {
                id: shortBar
                anchors {
                    left: shortLabel.right
                    right: parent.right
                }
                actions: root.shortActionList
            }
        }

        Item {
            width: parent.width
            height: childrenRect.height
            Label {
                id: label
                anchors {
                    left: parent.left
                    verticalCenter: bar.verticalCenter
                }
                text: "" + actionList.length + " actions:"
            }
            Toolbar {
                id: bar
                anchors {
                    left: label.right
                    right: parent.right
                }
                actions: root.actionList
            }
        }

        Item {
            width: parent.width
            height: childrenRect.height

            Label {
                id: fixedLabel
                anchors {
                    left: parent.left
                    verticalCenter: fixedBar.verticalCenter
                }
                text: "Fixed action:"
            }
            Toolbar {
                id: fixedBar
                anchors {
                    left: fixedLabel.right
                    right: parent.right
                }
                actions: root.shortActionList
                fixedAction: deleteAction
            }
        }

        Item {
            width: parent.width
            height: childrenRect.height
            Label {
                id: orangeLabel
                anchors {
                    left: parent.left
                    verticalCenter: orangeBar.verticalCenter
                }
                text: "Custom delegate:"
            }
            Toolbar {
                id: orangeBar
                anchors {
                    left: orangeLabel.right
                    right: parent.right
                }
                actions: root.actionList
                fixedAction: deleteAction
                delegate: AbstractButton {
                    styleName: "IconButtonStyle"
                    action: modelData
                    StyleHints {
                        foregroundColor: UbuntuColors.orange
                    }
                    width: units.gu(3)
                    objectName: "custom_delegate_button_" + index
                }
            }
        }
    }

    UbuntuTestCase {
        id: testCase
        name: "ToolbarApi"
        when: windowShown

        function init() {
            // revert to initial values
            bar.actions = root.actionList;
            bar.fixedAction = null;
            shortBar.actions = root.shortActionList;
            shortBar.fixedAction = null;
        }

        // excludes the fixed action button
        function get_number_of_visible_buttons(toolbar) {
            var repeater = findChild(toolbar, "actions_repeater");
            return repeater.count;
        }

        function has_visible_fixed_action_button(toolbar) {
            var repeater = findChild(toolbar, "fixedAction_repeater");
            return repeater.count > 0;
        }

        function test_actions() {
            compare(bar.actions, root.actionList, "Actions property can be initialized.");
            bar.actions = root.shortActionList;
            compare(bar.actions, root.shortActionList, "Actions property can be updated.");
        }

        function test_fixed_action() {
            compare(false, has_visible_fixed_action_button(bar),
                    "Fixed action button visible without fixed action defined.");
            compare(true, has_visible_fixed_action_button(fixedBar),
                    "No fixed action button with fixed action defined.");
            var container = findChild(fixedBar, "fixedAction_container_item");
            var button = findChild(container, "delete_action_button");
            compare(null !== button, true,
                    "Incorrect delete action button.");
            deleteAction.visible = false;
            compare(false, has_visible_fixed_action_button(fixedBar),
                    "Toolbar shows invisible fixed action.");
            deleteAction.visible = true;
            compare(true, has_visible_fixed_action_button(fixedBar),
                    "Setting visible of fixed action does not show the button.");
        }

        function test_number_of_visible_buttons() {
            compare(shortActionList.length, get_number_of_visible_buttons(shortBar),
                    "Incorrect number of buttons visible for " + shortActionList.length + " actions.");
            compare(actionList.length, get_number_of_visible_buttons(bar),
                    "Incorrect number of buttons visible for " + actionList.length + " actions.");
            actionList[2].visible = false;
            compare(actionList.length - 1, get_number_of_visible_buttons(bar),
                    "Invisible action still gets represented by a button.");
            actionList[2].visible = true;
            compare(actionList.length, get_number_of_visible_buttons(bar),
                    "Making action visible does not make its button visible.");
        }

        function test_custom_delegate() {
            var i = 0; var button; var n = actionList.length;
            var row = findChild(orangeBar, "actions_container_row");
            for (i = 0; i < n; i++) {
                button = findChild(row, "custom_delegate_button_"+i);
                compare(button.text, actionList[i].text, "Incorrect custom button " + i);
            }
            button = findChild(row, "custom_delegate_button_" + n);
            compare(button, null, "Too many buttons.");
        }
    }
}
