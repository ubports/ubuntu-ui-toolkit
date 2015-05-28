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

    property var sectionNames: [
        "first",
        "second",
        "third"
    ]

    Sections {
        id: enabledSections
        anchors {
            left: parent.left
            top: parent.top
            margins: units.gu(1)
        }
        model: root.sectionNames
    }
    Sections {
        id: disabledSections
        anchors {
            left: parent.left
            top: enabledSections.bottom
            margins: units.gu(1)
        }
        model: root.sectionNames
        enabled: false
    }

    UbuntuTestCase {
        id: testCase
        name: "SectionsApi"
        when: windowShown

        function initTestCase() {
        }

        // before each test
        function init() {
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
            var index = sections.model.indexOf(sectionName);
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
        //  for both sections and disabledSections.

        function test_0_first_section_initially_selected() {
            check_selected_section(enabledSections, 0, "first", "(init): ");
            check_selected_section(disabledSections, 0, "first", "(init disabled): ");
        }

        function test_number_of_section_buttons() {
            var n = root.sectionNames.length;
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

            click_section_button(disabledSections, name);
            // first button should still be selected:
            index = disabledSections.selectedIndex;
            name = "first";
            wait_for_animation(disabledSections);
            check_selected_section(disabledSections, index, name, "(click disabled): ");
        }

        function test_set_selectedIndex_to_select_section() {
            var index = 2;
            var name = "third";
            enabledSections.selectedIndex = index;
            wait_for_animation(enabledSections);
            check_selected_section(enabledSections, index, name, "(set index): ");

            disabledSections.selectedIndex = index;
            wait_for_animation(disabledSections);
            check_selected_section(disabledSections, index, name, "(set index disabled): ");
        }
    }
}
