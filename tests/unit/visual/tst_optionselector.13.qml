/*
 * Copyright 2012-2016 Canonical Ltd.
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
import Ubuntu.Components 1.3
import Ubuntu.Test 1.3

Page {
    width: units.gu(80)
    height: units.gu(60)

    header: PageHeader {
        id: pageHeader
        title: "OptionSelector"
        extension: Sections {
            id: sections
            model: [ "Collapsed", "Expanded", "Custom Delegate" ]
        }
    }

    Column {
        anchors {
            top: pageHeader.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: units.gu(4)
        }

        OptionSelector {
            id: defaultSelector

            text: "Pick your poison"
            property int sectionIndex: sections.selectedIndex
            state: sectionIndex == 2 ? 'custom' : (sectionIndex == 1 ? 'expanded' : 'default')
            states: [
                State {
                    name: 'default'
                    PropertyChanges {
                        target: defaultSelector;
                        model: [
                            'Gin and Tonic',
                            'White Russian',
                            'Sex on the Beach',
                            'Strawberry Mojito'
                        ];
                        currentlyExpanded: false;
                        expanded: false;
                    }
                },
                State {
                    name: 'expanded'
                    PropertyChanges {
                        target: defaultSelector;
                        model: [
                            'Ginger Ale',
                            'Coke',
                            'Ipanema',
                            'Virgin Mojito'
                        ];
                        currentlyExpanded: true;
                        expanded: true;
                    }
                },
                State {
                    name: 'custom'
                    PropertyChanges { target: defaultSelector; visible: false; }
                    PropertyChanges { target: customSelector; visible: true; }
                }
            ]
        }

        OptionSelector {
            id: customSelector
            model: customModel
            delegate: selectorDelegate
            visible: false
        }
    }

    Component {
        id: selectorDelegate

        OptionSelectorDelegate {
            text: name
            subText: description
            iconSource: image
            constrainImage: true
        }
    }

    ListModel {
        id: customModel
        ListElement { name: "Virgin Mary"; description: "With plenty hot sauce"; image: "../../resources/optionselector/test.png" }
        ListElement { name: "Shirley Temple"; description: "Grenadine for the color"; image: "../../resources/optionselector/test.png" }
        ListElement { name: "Ipanema"; description: "Non-alcoholic version of Caipirinha"; image: "../../resources/optionselector/test.png" }
        ListElement { name: "Millionaire Sour"; description: "Swaps ginger ale for Irish whiskey"; image: "../../resources/optionselector/test.png" }
    }

    UbuntuTestCase {
        name: "OptionSelectorAPI"
        when: windowShown

        function test_arrow_select_data() {
            return [
                // Collapsed: no movement
                { 'tag': 'Collapsed/Down', key: Qt.Key_Down, index: 0, newIndex: 0, expanded: false },
                { 'tag': 'Collapsed/Up', key: Qt.Key_Up, index: 3, newIndex: 3, expanded: false },
                // Expanded: arrows select
                { 'tag': 'Expanded/Up/First', key: Qt.Key_Up, index: 0, newIndex: 0, expanded: true },
                { 'tag': 'Expanded/Down/First', key: Qt.Key_Down, index: 0, newIndex: 1, expanded: true },
                { 'tag': 'Expanded/Up/Last', key: Qt.Key_Up, index: 3, newIndex: 2, expanded: true },
                { 'tag': 'Expanded/Down/Last', key: Qt.Key_Down, index: 3, newIndex: 3, expanded: true },
                // Expand and collapse on press
                { 'tag': 'Expand(Enter)', key: Qt.Key_Enter, index: 0, expanded: false, newExpanded: true },
                { 'tag': 'Expand(Return)', key: Qt.Key_Return, index: 0, expanded: false, newExpanded: true },
                { 'tag': 'Expand(Space)', key: Qt.Key_Space, index: 0, expanded: false, newExpanded: true },
                { 'tag': 'Collapse(Enter)', key: Qt.Key_Enter, index: 0, expanded: true, newExpanded: false },
                { 'tag': 'Collapse(Return)', key: Qt.Key_Return, index: 0, expanded: true, newExpanded: false },
                { 'tag': 'Collapse(Space)', key: Qt.Key_Space, index: 0, expanded: true, newExpanded: false },
            ]
        }
        function test_arrow_select(data, selector, sectionIndex) {
            if (!selector) {
                selector = defaultSelector;
                sectionIndex = 0;
            }
            sections.selectedIndex = sectionIndex;
            selector.forceActiveFocus();
            selector.currentlyExpanded = data.expanded;
            selector.selectedIndex = data.index;
            waitForRendering(selector);
            keyClick(data.key);
            waitForRendering(selector);
            if (data.newIndex)
                compare(selector.selectedIndex, data.newIndex, 'Wrong index after key press');
            if (data.newExpanded)
                compare(selector.currentlyExpanded, data.newExpanded, data.newExpanded ? 'Not expanded' : 'Not collapsed');
        }

        function test_expanded() {
            var selector = defaultSelector;
            sections.selectedIndex = 0;
            selector.forceActiveFocus();
            waitForRendering(selector);
            compare(selector.currentlyExpanded, false);
            compare(selector.expanded, false);
            keyClick(Qt.Key_Return);
            waitForRendering(selector);
            compare(selector.currentlyExpanded, true);
            compare(selector.expanded, false);

            sections.selectedIndex = 1;
            waitForRendering(selector);
            compare(selector.currentlyExpanded, true);
            compare(selector.expanded, true);

            sections.selectedIndex = 0;
            waitForRendering(selector);
            compare(selector.expanded, false);
            compare(selector.currentlyExpanded, false);
            compare(selector.selectedIndex, 0);
            selector.forceActiveFocus();
            waitForRendering(selector);
            keyClick(Qt.Key_Down);
            compare(selector.selectedIndex, 0);
        }

        function test_custom_data() {
            return test_arrow_select_data()
        }

        function test_custom(data) {
            test_arrow_select(data, customSelector, 2);
        }

        function test_count() {
            var selector = defaultSelector;
            sections.selectedIndex = 0;
            selector.forceActiveFocus();
            waitForRendering(selector);
            compare(selector.count, customModel.count);
        }
    }
}
