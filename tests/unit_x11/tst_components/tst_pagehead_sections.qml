/*
 * Copyright 2014-2015 Canonical Ltd.
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

Item {
    width: units.gu(50)
    height: units.gu(70)

    MainView {
        id: mainView
        anchors.fill: parent

        Page {
            id: page
            title: "Sections"
            head {
                sections {
                    actions: [
                        Action { text: "navy" },
                        Action { text: "lightblue" },
                        Action { text: "brown" }
                    ]
                }
                actions: [
                    Action {
                        iconName: "appointment-new"
                        text: "New"
                    },
                    Action {
                        iconName: "attachment"
                        text: "Attach"
                    },
                    Action {
                        iconName: "calendar-today"
                        text: "Today"
                    },
                    Action {
                        iconName: "delete"
                        text: "Delete"
                    }

                ]
            }

            ListView {
                anchors.fill: parent
                model: 50
                delegate: ListItem {
                    UbuntuShape {
                        id: shape
                        anchors {
                            left: parent.left
                            top: parent.top
                            bottom: parent.bottom
                            margins: units.gu(1)
                        }
                        width: height
                        property int index: page.head.sections.selectedIndex
                        backgroundColor: index >= 0
                                         ? page.head.sections.actions[index].text
                                         : "black"
                    }

                    Label {
                        id: label
                        anchors {
                            left: shape.right
                            leftMargin: units.gu(2)
                            verticalCenter: parent.verticalCenter
                        }
                        text: "List item #"+index
                    }
                }
            }
        }
    }

    UbuntuTestCase {
        name: "PageHeadSectionsAPI"
        when: windowShown
        id: testCase

        property var sectionsItem
        function initTestCase() {
            compare(mainView.active, true, "MainView always active.");
            compare(page.active, true, "Single page is active in MainView.");
            testCase.sectionsItem = findChild(mainView, "headerSectionsItem");
        }
        function init() {
            page.head.sections.selectedIndex = 0;
        }

        function test_number_of_sections() {
            compare(sectionsItem.model.length, 3, "Number of sections initialization failed.");
            page.head.sections.model = ["red"];
            compare(sectionsItem.model.length, 1, "Number of sections not updated.");
            page.head.sections.model = ["navy", "lightblue", "brown"];
            compare(sectionsItem.model.length, 3, "Number of sections reverted.");
            // Tests for verifying that itemModel.length equals the amount
            //  of section buttons are in tst_sections.qml.
        }

        function check_selected_button(selectedButtonIndex) {
            compare(sectionsItem.selectedIndex, selectedButtonIndex,
                    "Incorrect button selected.");
            var button = findChild(sectionsItem, "section_button_"+selectedButtonIndex);
            verify(button);
            verify(button.selected);
        }

        function test_selected_section() {
            var sectionsRepeater = findChild(mainView, "page_head_sections_repeater");
            check_selected_button(0);
            page.head.sections.selectedIndex = 2;
            check_selected_button(2);
            page.head.sections.selectedIndex = 1;
            check_selected_button(1);
            page.head.sections.selectedIndex = 0;
            check_selected_button(0);
        }

        function test_warn_when_too_many_sections() {
            ignoreWarning("PageHeadSections does not support more than 3 sections. Use PageHeader and Sections instead.");
            page.head.sections.model = ["red", "orange", "yellow", "green"];
        }
    }
}
