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

Rectangle {
    id: root
    width: units.gu(60)
    height: units.gu(75)
    color: "white"

    property list<Action> actionList: [
        Action {
            text: "action 0";
            onTriggered: label.text = "First action triggered.";
        },
        Action {
            text: "action 1";
            onTriggered: label.text = "Second action triggered.";
        },
        Action {
            text: "action 2";
            onTriggered: label.text = "Third action triggered.";
        }
    ]

    property var stringList: [
        "string zero", "string one", "string two"
    ]

    property var longStringList: [
        "one", "two", "three", "four", "five", "six", "seven",
        "eight", "nine", "ten", "eleven", "twelve", "thirteen",
        "fourteen", "fifteen", "sixteen", "seventeen",
        "eighteen", "nineteen", "twenty"
    ]

    Column {
        id: column
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins: units.gu(2)
        }
        spacing: units.gu(2)

        Sections {
            // Not used in the tests below, but added here to
            //  verify that the Actions can be defined directly
            //  inside the list of actions.
            actions: [
                Action { text: "inline action 0" },
                Action { text: "inline action 1" }
            ]
            width: parent.width
        }
        Rectangle {
            anchors {
                left: parent.left
                right: parent.right
                margins: units.gu(2)
            }
            color: UbuntuColors.blue
            height: units.gu(5)
            Label {
                id: label
                anchors.centerIn: parent
                text: "No action triggered."
                color: "white"
            }
        }
        Sections {
            id: enabledSections
            actions: root.actionList
            width: parent.width // set width > implicitWidth to test for bug 1551356 below.
        }
        Sections {
            id: disabledSections
            actions: root.actionList
            enabled: false
        }
        Sections {
            id: noSelectionSections
            actions: root.actionList
            selectedIndex: -1
        }
        Sections {
            id: enabledStringSections
            model: root.stringList
        }
        Sections {
            id: disabledStringSections
            model: root.stringList
            enabled: false
        }
        Sections {
            id: selectedIndexSections
            property bool action0Triggered: false;
            property bool action1Triggered: false;
            property bool action2Triggered: false;
            property Action action0: Action {
                text: "action0"
                onTriggered: selectedIndexSections.action0Triggered = true;
            }
            property Action action1: Action {
                text: "action1 (selected)"
                onTriggered: selectedIndexSections.action1Triggered = true;
            }
            property Action action2: Action {
                text: "action2"
                onTriggered: selectedIndexSections.action2Triggered = true;
            }
            actions: [action0, action1, action2]
            selectedIndex: 1
        }
        Sections {
            id: scrollingSections
            model: root.longStringList
            width: parent.width
        }
    }

    UbuntuTestCase {
        id: testCase
        name: "SectionsApi"
        when: windowShown

        function initTestCase() {
            // The initially selected actions must be triggered.
            compare(label.text, "First action triggered.",
                    "The first action was not triggered automatically.");
            compare(selectedIndexSections.action0Triggered, false,
                    "Action 0 was triggered with selectedIndex: 1.");
            compare(selectedIndexSections.action1Triggered, true,
                    "Action 1 was not automatically triggered with selectedIndex: 1.");
        }

        function init() {
            enabledSections.actions = root.actionList;
            enabledSections.model = root.actionList;
            enabledSections.selectedIndex = 0;
            disabledSections.selectedIndex = 0;
            enabledStringSections.selectedIndex = 0;
            disabledStringSections.selectedIndex = 0;
            label.text = "No action triggered."
            wait_for_animation(enabledSections);
        }

        function wait_for_animation(sections) {
            var listView = findChild(sections, "sections_listview");
            if (!listView.moving) {
                // wait for potential animation to start
                wait(100);
            }
            tryCompare(listView, "moving", false, 2000);
        }

        function get_number_of_section_buttons(sections) {
            var listview = findChild(sections, "sections_listview");
            return listview.count;
        }

        // return the index of the selected section button,
        //  or -1 if no selected section button is found.
        function get_selected_section_button_index(sections) {
            var n = get_number_of_section_buttons(sections);
            var button;
            for (var i=0; i < n; i++) {
                button = findChild(sections, "section_button_"+i);
                if (button.selected) {
                    return i;
                }
            }
            return -1;
        }

        function get_selected_section_button_text(sections) {
            var index = get_selected_section_button_index(sections);
            if (index < 0) return "BUTTON NOT FOUND.";
            var button = findChild(sections, "section_button_label_"+index);
            return button.text;
        }

        function click_section_button(sections, sectionName) {
            var index = -1;
            var object;
            for (index = 0; index < sections.model.length; index++) {
                object = sections.model[index];
                // object may be a string or an Action
                if (object.hasOwnProperty("text")) {
                    if (object.text === sectionName) {
                        break;
                    }
                } else {
                    if (object === sectionName) {
                        break;
                    }
                }
            }
            verify(index >= 0, "Button with name \'"+sectionName+"\' not found.");
            var button = findChild(sections, "section_button_"+index);
            mouseClick(button, button.width/2, button.height/2);
        }

        function check_selected_section(sections, index, name) {
            var v = sections.selectedIndex;
            compare(v, index, "selectedIndex " + v + " does not match " + index);
            v = get_selected_section_button_index(sections);
            compare(v, index, "selected button index " + v + " does not match " + index);
            if (v === -1) return;
            var w = get_selected_section_button_text(sections);
            compare(w, name, "selected button text \'" + w + "\' does not match \'" + name + "\'");
        }

        // in each test function below, test the desired behavior
        //  for both enabledSections and disabledSections.

        function test_0_first_section_initially_selected_actions_enabled() {
            check_selected_section(enabledSections, 0, "action 0");
        }
        function test_0_first_section_initially_selected_actions_disabled() {
            check_selected_section(disabledSections, 0, "action 0");
        }
        function test_0_first_section_initially_selected_strings_enabled() {
            check_selected_section(enabledStringSections, 0, "string zero");
        }
        function test_0_first_section_initially_selected_strings_disabled() {
            check_selected_section(disabledStringSections, 0, "string zero");
        }
        function test_0_no_selected_section_initalization() {
            check_selected_section(noSelectionSections, -1, "");
        }

        function test_number_of_section_buttons() {
            var n = root.actionList.length;
            compare(get_number_of_section_buttons(enabledSections), n,
                    "Showing incorrect number of sections.");
            compare(get_number_of_section_buttons(disabledSections), n,
                    "Showing incorrect number of disabled sections.")
        }

        function test_click_to_select_section_and_trigger_action() {
            var index = 2;
            var name = "action 2";
            click_section_button(enabledSections, name);
            wait_for_animation(enabledSections);
            check_selected_section(enabledSections, index, name);
            var text = "Third action triggered.";
            compare(label.text, text, "Action for clicked button not triggered.");
        }

        function test_click_disabled_section_action() {
            var index = 2;
            var name = "action 2";
            click_section_button(disabledSections, name);
            wait_for_animation(disabledSections);
            // first button should still be selected:
            index = 0;
            name = "action 0";
            check_selected_section(disabledSections, index, name);
            var text = "No action triggered.";
            compare(label.text, text, "Clicking disabled button triggered something.");
        }

        function test_click_to_select_section_string() {
            var index = 2;
            var name = "string two";
            click_section_button(enabledStringSections, name);
            wait_for_animation(enabledStringSections);
            check_selected_section(enabledStringSections, index, name);
        }

        function test_click_disabled_section_string() {
            var name = "string two";
            click_section_button(disabledStringSections, name);
            wait_for_animation(disabledStringSections);
            // first button should still be selected:
            var index = 0;
            name = "string zero";
            check_selected_section(disabledStringSections, index, name);
        }

        function test_set_selectedIndex_to_select_section_and_trigger_action_enabled() {
            var index = 1;
            var name = "action 1";
            enabledSections.selectedIndex = index;
            wait_for_animation(enabledSections);
            check_selected_section(enabledSections, index, name);
            var text = "Second action triggered.";
            compare(label.text, text, "Changing selected index did not trigger action.");
        }

        function test_set_selectedIndex_to_select_section_and_trigger_action_disabled() {
            var index = 2;
            var name = "action 2";
            disabledSections.selectedIndex = index;
            wait_for_animation(disabledSections);
            check_selected_section(disabledSections, index, name);
            var text = "Third action triggered.";
            compare(label.text, text, "Changing selected index for disabled Sections " +
                    "did not trigger action.");
        }

        function test_set_selectedIndex_to_select_section_string_enabled() {
            var index = 1;
            var name = "string one";
            enabledStringSections.selectedIndex = index;
            wait_for_animation(enabledStringSections);
            check_selected_section(enabledStringSections, index, name);
        }

        function test_set_selectedIndex_to_select_section_string_disabled() {
            var index = 2;
            var name = "string two";
            disabledStringSections.selectedIndex = index;
            check_selected_section(disabledStringSections, index, name);
        }

        function test_selectedIndex_when_model_changes_bug1513933() {
            enabledSections.model = ["1", "2", "3"];
            enabledSections.selectedIndex = 2;
            enabledSections.model = ["1", "2"];
            wait_for_animation(enabledSections);
            compare(enabledSections.selectedIndex, 0,
                    "Changing the model does not set the selected index to 0.");
            check_selected_section(enabledSections, 0, "1");
            enabledSections.model = [];
            wait_for_animation(enabledSections);
            compare(enabledSections.selectedIndex, -1,
                    "Setting an empty model does not set the selected index to -1.");
            enabledSections.model = ["1", "2", "3"];
            wait_for_animation(enabledSections);
            compare(enabledSections.selectedIndex, 0,
                    "Setting a non-empty model does not set the selected index to 0.");
            check_selected_section(enabledSections, 0, "1");
        }

        function test_model_changes_triggers_action_0() {
            selectedIndexSections.action0Triggered = false;
            selectedIndexSections.action1Triggered = false;
            selectedIndexSections.action2Triggered = false;
            var originalActions = selectedIndexSections.actions;
            selectedIndexSections.actions = [
                        selectedIndexSections.action0,
                        selectedIndexSections.action2
                    ]
            wait_for_animation(selectedIndexSections);
            compare(selectedIndexSections.action0Triggered, true,
                    "Changing the model does not trigger the first action.");
            compare(selectedIndexSections.action1Triggered, false,
                    "Changing the model triggers the second action.");
            compare(selectedIndexSections.action2Triggered, false,
                    "Changing the model triggers the third action.");
            selectedIndexSections.actions = originalActions;
            wait_for_animation(selectedIndexSections);
        }

        SignalSpy {
            id: contentXChangedSpy
            signalName: "contentXChanged"
        }
        function test_click_disabled_scroll_button_bug1551356() {
            var listView = findChild(enabledSections, "sections_listview");
            var icon = findChild(enabledSections, "left_scroll_icon");
            contentXChangedSpy.target = listView;
            contentXChangedSpy.clear();
            compare(listView.contentX, 0.0, "listView is not at the leftmost position initially.");
            mouseClick(icon, icon.width/2, icon.height/2);
            wait(200); // give the listview ample time to scroll
            compare(contentXChangedSpy.count, 0,
                    "listView moved when clicking disabled scroll button.");
            contentXChangedSpy.clear();
            contentXChangedSpy.target = null;
        }

        SignalSpy {
            id: animationStartedSpy
            signalName: "started"
        }
        function test_press_release_on_different_icons() {
            var leftIcon = findChild(scrollingSections, "left_scroll_icon");
            var rightIcon = findChild(scrollingSections, "right_scroll_icon");
            var animation = findInvisibleChild(scrollingSections, "sections_scroll_animation");
            animationStartedSpy.target = animation;
            compare(leftIcon.width, rightIcon.width, "Scroll icons are not the same width.");
            compare(leftIcon.height, rightIcon.height, "Scroll icons are not the same height.");
            var w = leftIcon.width / 2;
            var h = leftIcon.height / 2;
            mouseMove(leftIcon, w, h);
            mousePress(leftIcon, w, h);
            mouseMove(rightIcon, w, h);
            mouseRelease(rightIcon, w, h);
            wait(200);
            compare(animationStartedSpy.count, 0,
                    "Clicked signal came after pressing left icon and releasing on right icon.");
            mousePress(rightIcon, w, h);
            mouseMove(leftIcon, w, h);
            mouseRelease(leftIcon, w, h);
            wait(200);
            compare(animationStartedSpy.count, 0,
                    "Clicked signal came after pressing right icon and releasing on left icon.");
            animationStartedSpy.target = null;
        }

        function test_keyboard_navigation_data() {
            return [
                        { tag: "actions",
                            item: enabledSections,
                            initialButton: root.actionList[0].text,
                            initialIndex: 0
                        },
                        { tag: "strings",
                            item: enabledStringSections,
                            initialButton: root.stringList[0],
                            initialIndex: 0
                        },
                        { tag: "scrolling",
                            item: scrollingSections,
                            initialButton: root.longStringList[0],
                            initialIndex: 0
                        }
                    ];
        }

        function test_keyboard_navigation(data) {
            var sections = data.item;
            var initialButtonName = data.initialButton;
            click_section_button(sections, initialButtonName);
            wait_for_animation(sections);
            compare(sections.selectedIndex, data.initialIndex,
                    "Clicking the initial button does not select section "+data.initialIndex);
            var numberOfSections = sections.model.length;
            var i = sections.selectedIndex;

            // Sections has focus because it was just clicked on,
            //  so we can use keyboard navigation.
            while (i < numberOfSections - 1) {
                i++;
                keyClick(Qt.Key_Right);
                wait_for_animation(sections);
                compare(sections.selectedIndex, i,
                        "Could not navigate to index " + i + " using right key.");
            }

            // Navigated to the last section. Right key should do nothing now.
            keyClick(Qt.Key_Right);
            wait_for_animation(sections);
            compare(sections.selectedIndex, i,
                    "Using right key navigates beyond the last section.");

            while (i > 0) {
                i--;
                keyClick(Qt.Key_Left);
                wait_for_animation(sections);
                compare(sections.selectedIndex, i,
                        "Could not navigate back to index " + i + " using left key.");
            }

            // Navigated back to the beginning. Left key should do nothing now.
            keyClick(Qt.Key_Left);
            wait_for_animation(sections);
            compare(sections.selectedIndex, i,
                    "Using left key navigates beyond the first section.");
        }
    }
}
