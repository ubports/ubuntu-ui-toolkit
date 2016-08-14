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
import QtQuick.Layouts 1.1
import Ubuntu.Test 1.0
import Ubuntu.Components 1.3

Item {
    id: root
    width: units.gu(50)
    height: units.gu(60)

    property list<Action> actionList:  [
        Action {
            iconName: "alarm-clock"
            text: "Tick tock"
            onTriggered: print("tock")
        },
        Action {
            iconName: "appointment"
            text: "Date"
            onTriggered: print("date")
        },
        Action {
            iconName: "attachment"
            text: "Attach"
            onTriggered: print("attach")
        },
        Action {
            iconName: "contact"
            text: "Contact"
            onTriggered: print("contact")
        },
        Action {
            iconName: "like"
            text: "Like"
            onTriggered: print("+1")
        },
        Action {
            iconName: "lock"
            text: "Lock"
            onTriggered: print("lock")
        },
        Action {
            iconName: "camcorder"
            text: "Camera"
            onTriggered: print("cam")
        },
        Action {
            iconName: "location"
            text: "Location"
            onTriggered: print("loc")
            enabled: false
        },
        Action {
            iconName: "message"
            text: "Message"
            onTriggered: print("msg")
        },
        Action {
            iconName: "livetv"
            text: "Television"
            onTriggered: print("tv")
        },
        Action {
            iconName: "lock-broken"
            text: "Unlock"
            onTriggered: print("unlock")
        }
    ]

    property list<Action> shortActionList: [
        Action {
            iconName: "share"
            text: "Share"
            onTriggered: print("share")
        },
        Action {
            iconName: "starred"
            text: "Favorite"
            onTriggered: print("fav")
        }
    ]

    Column {
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins: units.gu(2)
        }
        height: childrenRect.height

        RowLayout {
            width: parent.width
            height: childrenRect.height

            Label {
                anchors {
                    verticalCenter: shortBar.verticalCenter
                }
                text: "" + shortBar.numberOfSlots + " slot(s):"
            }

            ActionBar {
                // no numberOfSlots specified. Using default value.
                id: shortBar
                Layout.fillWidth: true
                actions: root.shortActionList
            }
        }

        RowLayout {
            width: parent.width
            height: childrenRect.height

            Label {
                anchors {
                    verticalCenter: bar.verticalCenter
                }
                text: "" + bar.numberOfSlots + " slot(s):"
            }

            ActionBar {
                id: bar
                Layout.fillWidth: true
                numberOfSlots: numberOfActionsSlider.value.toFixed(0)
                actions: root.actionList
            }
        }

        RowLayout {
            width: parent.width
            height: childrenRect.height
            Label {
                anchors {
                    verticalCenter: coloredBar.verticalCenter
                }
                text: "colored:"
            }
            ActionBar {
                id: coloredBar
                Layout.fillWidth: true
                numberOfSlots: numberOfActionsSlider.value.toFixed(0)
                actions: root.actionList
                StyleHints {
                    ignoreUnknownProperties: false
                    backgroundColor: UbuntuColors.blue
                    buttons {
                        foregroundColor: "white"
                        disabledForegroundColor: UbuntuColors.silk
                        pressedBackgroundColor: UbuntuColors.ash
                        disabledBackgroundColor: UbuntuColors.slate
                    }
                }
            }
        }

        Slider {
            id: numberOfActionsSlider
            anchors {
                left: parent.left
                right: parent.right
            }
            minimumValue: -1
            maximumValue: 10
            value: 3
            live: true
        }

        RowLayout {
            width: parent.width
            height: childrenRect.height

            Label {
                anchors {
                    verticalCenter: customDelegateBar.verticalCenter
                }
                text: "Custom delegate"
            }
            ActionBar {
                id: customDelegateBar
                Layout.fillWidth: true
                actions: root.shortActionList
                delegate: Button {
                    action: modelData
                    width: units.gu(14)
                    strokeColor: UbuntuColors.purple
                    objectName: "custom_delegate_button_" + index
                }
            }
        }

        RowLayout {
            width: parent.width
            height: childrenRect.height
            Label {
                anchors {
                    verticalCenter: greenButtonsBar.verticalCenter
                }
                text: "Custom delegate 2"
            }
            ActionBar {
                // Note: The same result (green buttons) can be accomplished
                //  by setting the foregroundColor in the style.
                id: greenButtonsBar
                Layout.fillWidth: true
                actions: root.actionList
                delegate: AbstractButton {
                    styleName: "IconButtonStyle"
                    action: modelData
                    height: greenButtonsBar.height
                    StyleHints {
                        foregroundColor: UbuntuColors.green
                    }
                }
            }
        }
    }

    UbuntuTestCase {
        id: testCase
        name: "ActionBarApi"
        when: windowShown

        function initTestCase() {
            compare(shortBar.numberOfSlots, 3, "Default number of slots should be 3.");
        }

        function init() {
            // revert to initial values
            bar.numberOfSlots = 3;
            shortBar.numberOfSlots = 3;
            bar.actions = root.actionList;
            shortBar.actions = root.shortActionList;
        }

        function get_overflow_button_visible(actionBar) {
            var overflowButton = findChild(actionBar, "overflow_action_button")
            return overflowButton !== null && overflowButton.visible
        }

        function get_number_of_visible_buttons(actionBar) {
            var repeater = findChild(actionBar, "actions_repeater");
            return repeater.count;
        }

        function get_number_of_actions_in_overflow(actionBar) {
            if (get_overflow_button_visible(actionBar)) {
                var overflowButton = findChild(actionBar, "overflow_action_button");
                // click the overflow button in order to create the overflow panel
                mouseClick(overflowButton, overflowButton.width/2, overflowButton.height/2);
                // the overflow panel is not a child of the ActionBar, so use
                //  root to find it
                var panel = findChild(root, "actions_overflow_panel");
                var n = panel.actions.length;
                // click again to close the overflow panel
                mouseClick(overflowButton, overflowButton.width/2, overflowButton.height/2);
                return n;
            } else {
                // empty overflow
                return 0;
            }
        }

        function test_number_of_slots() {
            compare(shortBar.numberOfSlots, 3, "Initial number of slots should be 3.");
            shortBar.numberOfSlots = 10;
            compare(shortBar.numberOfSlots, 10, "Number of slots cannot be set.");
        }

        function test_actions() {
            compare(bar.actions, root.actionList, "Actions property can be initialized.");
            bar.actions = root.shortActionList;
            compare(bar.actions, root.shortActionList, "Actions property can be updated.");
        }

        function test_number_of_visible_buttons() {
            compare(shortActionList.length, get_number_of_visible_buttons(shortBar),
                    "Incorrect number of actions visible for num actions < num slots.");
            compare(bar.numberOfSlots, get_number_of_visible_buttons(bar),
                    "Incorrect number of actions visible for num actions > num slots.");
            bar.numberOfSlots = 0;
            compare(1, get_number_of_visible_buttons(bar),
                    "No slot visible when numberOfSlots < 1.");
        }

        function test_overflow_button_visible() {
            compare (false, get_overflow_button_visible(shortBar),
                     "Overflow button visible when num actions < num slots.");
            bar.numberOfSlots = actionList.length;
            compare(false, get_overflow_button_visible(bar),
                    "Overflow button visible when num actions = num slots.");
            bar.numberOfSlots = actionList.length - 1;
            compare(true, get_overflow_button_visible(bar),
                    "Overflow button not visible when num actions > num slots.");
        }

        function test_number_of_actions_in_overflow() {
            compare(0, get_number_of_actions_in_overflow(shortBar),
                    "Incorrect number of actions in overflow when num actions < num slots.");
            bar.numberOfSlots = actionList.length;
            compare(0, get_number_of_actions_in_overflow(bar),
                    "Incorrect number of actions in overflow when num actions = num slots.");
            bar.numberOfSlots--;
            // one action too many, plus one slot used for the overflow button:
            compare(2, get_number_of_actions_in_overflow(bar),
                    "Incorrect number of actions in overflow when num actions = num slots + 1.");
            bar.numberOfSlots--;
            compare(3, get_number_of_actions_in_overflow(bar),
                    "Incorrect number of actions in overflow when num actions = num slots + 2.");
            bar.numberOfSlots = 0;
            compare(actionList.length, get_number_of_actions_in_overflow(bar),
                    "Incorrect number of actions in overflow when num slots = 0.");
        }

        function test_custom_delegate() {
            var i = 0; var button; var n = shortActionList.length;
            for (i = 0; i < n; i++) {
                button = findChild(customDelegateBar, "custom_delegate_button_"+i);
                compare(button.text, shortActionList[n-1-i].text, "Incorrect custom button " + i);
            }
            button = findChild(customDelegateBar, "custom_delegate_button_" + n);
            compare(button, null, "Too many buttons.");
        }
    }
}
