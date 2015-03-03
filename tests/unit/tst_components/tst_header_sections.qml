/*
 * Copyright 2014 Canonical Ltd.
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

import QtQuick 2.2
import QtTest 1.0
import Ubuntu.Components 1.1
import Ubuntu.Test 1.0

UbuntuTestCase {
    name: "PageHeadSectionsAPI"

    MainView {
        id: mainView
        width: units.gu(50)
        height: units.gu(80)

        useDeprecatedToolbar: false

        Page {
            id: page
            title: "Sections"
            head {
                sections {
                    model: ["one", "two", "three"]
                }
            }
        }
    }

    function initTestCase() {
        compare(mainView.active, true, "MainView always active.");
        compare(page.active, true, "Single page is active in MainView.");
    }

    function test_number_of_sections() {
        var sectionsRepeater = findChild(mainView, "page_head_sections_repeater");
        compare(sectionsRepeater.count, 3, "Number of sections initialization failed.");
        page.head.sections.model = ["one"]
        compare(sectionsRepeater.count, 1, "Number of sections not updated.");
        page.head.sections.model = ["one", "two", "three"];
        compare(sectionsRepeater.count, 3, "Number of sections reverted.");
    }

    function check_selected_button(numberOfButtons, selectedButtonIndex) {
        for (var i = 0; i < numberOfButtons; i++) {
            var button = findChild(mainView, "section_button_"+i);
            if (i === selectedButtonIndex) {
                compare(button.selected, true, "Button "+i+" is not selected.");
            } else {
                compare(button.selected, false, "Button "+i+" is selected.");
            }
        }
    }

    function test_selected_section() {
        var sectionsRepeater = findChild(mainView, "page_head_sections_repeater");
        var count = sectionsRepeater.count;
        check_selected_button(count, 0);
        page.head.sections.selectedIndex = 2;
        check_selected_button(count, 2);
        page.head.sections.selectedIndex = 1;
        check_selected_button(count, 1);
        page.head.sections.selectedIndex = 0;
        check_selected_button(count, 0);
    }

    function test_warn_when_too_many_sections() {
        ignoreWarning("It is not recommended or supported to use more than three sections in Page.head.sections.model.")
        page.head.sections.model = ["1", "2", "3", "4"];
    }
}
