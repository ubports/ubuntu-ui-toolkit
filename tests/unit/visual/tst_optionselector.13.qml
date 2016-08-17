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
            id: selector

            text: "Pick your poison"
            property int sectionIndex: sections.selectedIndex
            state: sectionIndex == 2 ? 'custom' : (sectionIndex == 1 ? 'expanded' : 'default')
            states: [
                State {
                    name: 'default'
                    PropertyChanges {
                        target: selector;
                        model: [
                            'Gin and Tonic',
                            'White Russian',
                            'Sex on the Beach',
                            'Strawberry Mojito'
                        ];
                        delegate: defaultDelegate
                    }
                },
                State {
                    name: 'expanded'
                    extend: 'default'
                    PropertyChanges { target: selector; expanded: true; currentlyExpanded: true }
                },
                State {
                    name: 'custom'
                    PropertyChanges { target: selector; model: customModel; delegate: selectorDelegate; }
                }
            ]
        }
    }

    Component {
        id: defaultDelegate
        OptionSelectorDelegate {
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

    SignalSpy {
        id: clickedSignal
        target: selector
        signalName: "delegateClicked"
    }

    SignalSpy {
        id: triggeredSignal
        target: selector
        signalName: "triggered"
    }

    SignalSpy {
        id: expansionSignal
        target: selector
        signalName: "expansionCompleted"
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
        function test_arrow_select(data) {
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
    }
}
