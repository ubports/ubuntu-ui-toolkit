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

Rectangle {
    id: root
    width: 400
    height: 600
    color: "white"

    property list<Action> actionList: [
        Action {
            text: "first";
            onTriggered: label.text = "First action triggered.";
        },
        Action {
            text: "second";
            onTriggered: label.text = "Second action triggered.";
        },
        Action {
            text: "third";
            onTriggered: label.text = "Third action triggered.";
        }
    ]

    property var stringList: [
        "string one", "string two", "string three"
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

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Sections with actions"
        }
        Label {
            anchors.left: parent.left
            text: "enabled:"
            fontSize: "small"
        }
        Sections {
            id: enabledSections
            model: root.actionList
        }
        Label {
            text: "disabled:"
            fontSize: "small"
        }
        Sections {
            id: disabledSections
            model: root.actionList
            enabled: false
        }
        Rectangle {
            anchors {
                left: parent.left
                right: parent.right
                margins: units.gu(2)
            }
            color: UbuntuColors.blue
            height: units.gu(10)
            Label {
                id: label
                anchors.centerIn: parent
                text: "No action triggered."
                color: "white"
            }
        }
        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Sections with strings"
        }
        Label {
            anchors.left: parent.left
            text: "enabled:"
            fontSize: "small"
        }
        Sections {
            id: enabledStringSections
            model: root.stringList
        }
        Label {
            anchors.left: parent.left
            text: "disabled:"
            fontSize: "small"
        }
        Sections {
            id: disabledStringSections
            model: root.stringList
            enabled: false
        }
    }

    UbuntuTestCase {
        id: testCase
        name: "SectionsApi"
        when: windowShown

        function initTestCase() {
            compare(label.text, "No action triggered.", "An action was triggered initially.");
        }

        function cleanup() {
            enabledSections.selectedIndex = 0;
            disabledSections.selectedIndex = 0;
        }

        function wait_for_animation(sections) {
            // TODO when animations are added
        }

        function get_number_of_section_buttons(sections) {
            var repeater = findChild(sections, "sections_repeater");
            return repeater.count;
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

        function check_selected_section(sections, index, name, message) {
            var v = sections.selectedIndex;
            compare(v, index,  message+"selectedIndex "+v+" does not match "+index);
            v = get_selected_section_button_index(sections);
            compare(v, index, message+"selected button index "+v+" does not match "+index);
            var w = get_selected_section_button_text(sections);
            compare(w, name, message+"selected button text \'"+w+"\' does not match \'"+name+"\'");
        }

        // in each test function below, test the desired behavior
        //  for both enabledSections and disabledSections.

        function test_0_first_section_initially_selected() {
            check_selected_section(enabledSections, 0, "first", "(init): ");
            check_selected_section(disabledSections, 0, "first", "(init disabled): ");
            check_selected_section(enabledStringSections, 0, "string one", "(init str): ");
            check_selected_section(disabledStringSections, 0, "string one", "(init disabled str): ");
        }

        function test_number_of_section_buttons() {
            var n = root.actionList.length;
            compare(get_number_of_section_buttons(enabledSections), n,
                    "Showing incorrect number of sections.");
            compare(get_number_of_section_buttons(disabledSections), n,
                    "Showing incorrect number of disabled sections.")
        }

        function test_click_to_select_section() {
            var index = 2;
            var name = "third";
            click_section_button(enabledSections, name);
            wait_for_animation(enabledSections);
            check_selected_section(enabledSections, index, name, "(click): ");
            var text = "Third action triggered.";
            compare(label.text, text, "Action for clicked button not triggered.");

            click_section_button(disabledSections, name);
            wait_for_animation(disabledSections);
            // first button should still be selected:
            index = 0;
            name = "first";
            check_selected_section(disabledSections, index, name, "(click disabled): ");
            compare(label.text, text, "Clicking disabled button triggered something.");

            name = "string three";
            index = 2;
            click_section_button(enabledStringSections, name);
            wait_for_animation(enabledStringSections);
            check_selected_section(enabledStringSections, index, name, "(click str): ");
            // no actions triggered

            click_section_button(disabledStringSections, name);
            wait_for_animation(disabledStringSections);
            // first button should still be selected:
            index = 0;
            name = "string one";
            check_selected_section(disabledStringSections, index, name, "(click disabled str): ");
            // no actions triggered
        }

        function test_set_selectedIndex_to_select_section() {
            var index = 1;
            var name = "second";
            enabledSections.selectedIndex = index;
            wait_for_animation(enabledSections);
            check_selected_section(enabledSections, index, name, "(set index): ");
            var text = "Second action triggered.";
            compare(label.text, text, "Changing selected index did not trigger action.");

            index = 2;
            name = "third";
            disabledSections.selectedIndex = index;
            wait_for_animation(disabledSections);
            check_selected_section(disabledSections, index, name, "(set index disabled): ");
            text = "Third action triggered.";
            compare(label.text, text, "Changing selected index for disabled Sections " +
                                        "did not trigger action.");

            index = 1;
            name = "string two";
            enabledStringSections.selectedIndex = index;
            wait_for_animation(enabledStringSections);
            check_selected_section(enabledStringSections, index, name, "(set str index): ");
            // no action triggered

            index = 2;
            name = "string three";
            disabledStringSections.selectedIndex = index;
            check_selected_section(disabledStringSections, index, name, "(set str index disabled): ");
            // no action triggered
        }
    }
}
