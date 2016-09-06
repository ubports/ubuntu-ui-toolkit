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

    Row {
        id: scrollingSwitchRow
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            margins: units.gu(2)
        }
        spacing: units.gu(2)

        Label { text: "Overflow" }
        Switch {
            id: scrollingSwitch
            checked: false
        }
        Label { text: "Scrolling" }
    }
    property string actionBarStyleName: scrollingSwitch.checked ?
                                            "ScrollingActionBarStyle" :
                                            "ActionBarStyle"

    GridLayout {
        columns: 2
        anchors {
            left: parent.left
            right: parent.right
            top: scrollingSwitchRow.bottom
            margins: units.gu(2)
        }
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
            styleName: root.actionBarStyleName
        }

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
            styleName: root.actionBarStyleName
        }

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
            styleName: root.actionBarStyleName
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

        Slider {
            id: numberOfActionsSlider
            Layout.columnSpan: 2
            anchors {
                left: parent.left
                right: parent.right
            }
            minimumValue: -1
            maximumValue: 10
            value: 3
            live: true
        }

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
            styleName: root.actionBarStyleName
        }

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
            styleName: root.actionBarStyleName
        }
    }

    UbuntuTestCase {
        id: actionBarTestCase
        name: "ActionBarApi"
        when: windowShown

        function initTestCase() {
            scrollingSwitch.checked = false;
            waitForRendering(bar);
            compare(root.actionBarStyleName, "ActionBarStyle");
            compare(shortBar.styleName, "ActionBarStyle");
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
            compare(actionBar.styleName, "ActionBarStyle", "Style has no overflow button.");
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


        function test_actions_visibility_data() {
            // bar has 11 actions, shortBar has 2 actions.
            return [
                        {   tag: "Small number of actions < number of slots",
                            bar: shortBar,
                            number_of_slots: 3,
                            number_of_visible_buttons: 2,
                            overflow_button_visible: false,
                            number_of_actions_in_overflow: 0
                        },
                        {
                            tag: "Small number of actions = number of slots",
                            bar: shortBar,
                            number_of_slots: 2,
                            number_of_visible_buttons: 2,
                            overflow_button_visible: false,
                            number_of_actions_in_overflow: 0
                        },
                        {   tag: "Number of actions > number of slots",
                            bar: bar,
                            number_of_slots: 3,
                            number_of_visible_buttons: 3,
                            overflow_button_visible: true,
                            number_of_actions_in_overflow: actionList.length - 2
                        },
                        {   tag: "Number of slots < 1",
                            bar: bar,
                            number_of_slots: 0,
                            number_of_visible_buttons: 1,
                            overflow_button_visible: true,
                            number_of_actions_in_overflow: actionList.length
                        },
                        {   tag: "Number of actions = number of slots",
                            bar: bar,
                            number_of_slots: actionList.length,
                            number_of_visible_buttons: actionList.length,
                            overflow_button_visible: false,
                            number_of_actions_in_overflow: 0
                        },
                        {   tag: "Number of actions + 1 = number of slots",
                            bar: bar,
                            number_of_slots: actionList.length - 1,
                            number_of_visible_buttons: actionList.length - 1,
                            overflow_button_visible: true,
                            number_of_actions_in_overflow: 2
                        },
                        {   tag: "Number of actions + 2 = number of slots",
                            bar: bar,
                            number_of_slots: actionList.length - 2,
                            number_of_visible_buttons: actionList.length - 2,
                            overflow_button_visible: true,
                            number_of_actions_in_overflow: 3
                        },
                    ];
        }

        function test_actions_visibility(data) {
            data.bar.numberOfSlots = data.number_of_slots;
            compare(get_number_of_visible_buttons(data.bar), data.number_of_visible_buttons,
                    "Incorrect number of actions visible.");
            compare(get_overflow_button_visible(data.bar), data.overflow_button_visible,
                    "Incorrect overflow button visibility.");
            compare(get_number_of_actions_in_overflow(data.bar), data.number_of_actions_in_overflow,
                    "Incorrect number of actions in overflow.");
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

    UbuntuTestCase {
        id: scrollingActionBarTestCase
        name: "ScrollingActionBarApi"
        when: windowShown

        function get_scroll_button(actionBar, name) {
            compare(actionBar.styleName, "ScrollingActionBarStyle",
                    "Only scrolling action bar has scroll buttons.");
            var button = findChild(actionBar, name);
            verify(button !== null, "Style has no button with objectName " + name);
            return button;
        }

        function get_leading_scroll_button(actionBar) {
            return get_scroll_button(actionBar, "leading_scroll_button");
        }

        function get_trailing_scroll_button(actionBar) {
            return get_scroll_button(actionBar, "trailing_scroll_button");
        }

        function get_number_of_visible_buttons(actionBar) {
            compare(actionBar.styleName, "ScrollingActionBarStyle", "Unknown style name.");
            var listView = findChild(actionBar, "actions_listview");
            return listView.count;
        }

        function initTestCase() {
            scrollingSwitch.checked = true;
            waitForRendering(bar);
            compare(root.actionBarStyleName, "ScrollingActionBarStyle");
            compare(shortBar.styleName, "ScrollingActionBarStyle");
        }

        function test_scroll_buttons_visibility_data() {
            // bar has 11 actions, shortBar has 2 actions.
            return [
                        {   tag: "Small number of actions",
                            bar: shortBar,
                            number_of_visible_buttons: shortActionList.length,
                            view_position: 0,
                            leading_scroll_button_visible: false,
                            trailing_scroll_button_visible: false
                        },
                        {
                            tag: "Can scroll to the left",
                            bar: bar,
                            number_of_visible_buttons: actionList.length,
                            view_position: 0, // the right-most action
                            leading_scroll_button_visible: true,
                            trailing_scroll_button_visible: false
                        },
                        {
                            tag: "Can scroll to the right",
                            bar: bar,
                            number_of_visible_buttons: actionList.length,
                            view_position: actionList.length - 1, // the left-most action
                            leading_scroll_button_visible: false,
                            trailing_scroll_button_visible: true
                        },
                        {
                            tag: "Can scroll both ways",
                            bar: bar,
                            number_of_visible_buttons: actionList.length,
                            view_position: actionList.length / 2,
                            leading_scroll_button_visible: true,
                            trailing_scroll_button_visible: true
                        }
                    ];
        }

        function test_scroll_buttons_visibility(data) {
            compare(get_number_of_visible_buttons(data.bar), data.number_of_visible_buttons,
                    "Incorrect number of visible buttons.");

            var listView = findChild(data.bar, "actions_listview");
            listView.positionViewAtIndex(data.view_position, ListView.Center);
            var button = get_leading_scroll_button(data.bar);
            var opacity = data.leading_scroll_button_visible ? 1.0 : 0.0
            // use tryCompare() because the fade-in/out animation of the button may still be ongoing.
            tryCompare(button, "opacity", opacity, 1000, "Incorrect leading scroll button visibility.");

            button = get_trailing_scroll_button(data.bar);
            opacity = data.trailing_scroll_button_visible ? 1.0 : 0.0;
            tryCompare(button, "opacity", opacity, 1000, "Incorrect trailing scroll button visbility.");

            // revert to the initial position:
            listView.positionViewAtBeginning();
        }

        function test_scroll_buttons_functionality() {
            var leadingScrollButton = get_leading_scroll_button(bar);
            var trailingScrollButton = get_trailing_scroll_button(bar);

            var listView = findChild(bar, "actions_listview");
            var x = listView.contentX;
            tryCompare(leadingScrollButton, "opacity", 1.0, 1000,
                       "Leading scroll button is not visible initially.");

            mouseClick(leadingScrollButton, leadingScrollButton.width/2, leadingScrollButton.height/2);
            wait(UbuntuAnimation.FastDuration + 100); // wait for scrolling animation.
            verify(listView.contentX < x, "Clicking the leading scroll button does not scroll to the left.");

            x = listView.contentX;
            tryCompare(trailingScrollButton, "opacity", 1.0, 1000,
                       "Trailing scroll button is not visible after scrolling to the left.");

            mouseClick(trailingScrollButton, trailingScrollButton.width/2, trailingScrollButton.height/2);
            wait(UbuntuAnimation.FastDuration + 100); // wait for scrolling animation.
            verify(listView.contentX > x, "Clicking the leading scroll button does not scroll to the left.");

            // revert to the initial position:
            listView.positionViewAtBeginning();
        }
    }
}
