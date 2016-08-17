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
import Ubuntu.Components 1.3
import Ubuntu.Test 1.0

MainView {
    id: mainView
    width: units.gu(30)
    height: units.gu(60)
    Component {
        id: pageComponent
        Page {
            id: page
            title: i18n.tr("The Page")
            head.sections.model: ["one","two","three"]
            head.sections.selectedIndex: -1
        }
    }

    PageStack {
        id: stack
    }

    Component.onCompleted: {
        stack.push(pageComponent)
    }

    UbuntuTestCase {
        id: testCase
        name: "Page.head.sections"
        when: windowShown

        property var sectionsItem
        function initTestCase() {
            testCase.sectionsItem = findChild(mainView, "headerSectionsItem");
            verify(testCase.sectionsItem);
        }

        function verifySelectedSection(index, isSelected, message) {
            var button = findChild(sectionsItem, "section_button_" + index);
            verify(button);
            if (message == undefined) {
                message = "";
            }
            compare(button.selected, isSelected, message);
        }

        function test_bug1511839() {
            for (var i = 0; i < stack.currentPage.head.sections.model.length; i++) {
                verifySelectedSection(i, false, "Section " + i + " is selected.");
            }
        }

        function test_selected_section_data() {
            return [
                {tag: "none selected", selIndex: -1, selected: [false, false, false]},
                {tag: "0 selected", selIndex: 0, selected: [true, false, false]},
                {tag: "1 selected", selIndex: 1, selected: [false, true, false]},
                {tag: "2 selected", selIndex: 2, selected: [false, false, true]},
            ];
        }
        function test_selected_section(data) {
            stack.currentPage.head.sections.selectedIndex = data.selIndex;
            for (var i = 0; i < stack.currentPage.head.sections.model.length; i++) {
                verifySelectedSection(i, data.selected[i]);
            }
        }
    }
}
